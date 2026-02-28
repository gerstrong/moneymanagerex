/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)
 Copyright (C) 2022,2025 Mark Whalley (mark@ipx.co.uk)

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************/

#include "StockModel.h"
#include "StockHistoryModel.h"
#include "TrxLinkModel.h"
#include "TrxShareModel.h"
#include "CurrencyHistoryModel.h"

StockModel::StockModel() :
    TableFactory<StockTable, StockData>()
{
}

StockModel::~StockModel()
{
}

/**
* Initialize the global StockModel table.
* Reset the StockModel table or create the table if it does not exist.
*/
StockModel& StockModel::instance(wxSQLite3Database* db)
{
    StockModel& ins = Singleton<StockModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();

    return ins;
}

wxString StockModel::get_stock_name(int64 stock_id)
{
    const Data* stock = instance().get_data_n(stock_id);
    if (stock)
        return stock->m_name;
    else
        return _t("Stock Error");
}

/** Return the static instance of StockModel table */
StockModel& StockModel::instance()
{
    return Singleton<StockModel>::instance();
}

wxDate StockModel::PURCHASEDATE(const Data* stock)
{
    return parseDateTime(stock->m_purchase_date);
}

wxDate StockModel::PURCHASEDATE(const Data& stock)
{
    return parseDateTime(stock.m_purchase_date);
}

/** Original value of Stocks */
double StockModel::InvestmentValue(const Data* r)
{
    return r->m_purchase_value;
}

/** Original value of Stocks */
double StockModel::InvestmentValue(const Data& r)
{
    return InvestmentValue(&r);
}

double StockModel::CurrentValue(const Data* r)
{
    return r->m_num_shares * r->m_current_price;
}

double StockModel::CurrentValue(const Data& r)
{
    return CurrentValue(&r);
}

/**
* Remove the Data record from memory and the database.
* Delete also all stock history
*/
bool StockModel::purge_id(int64 id)
{
    const StockData *stock_n = get_data_n(id);
    const auto& stock_a = StockModel::instance().find(
        StockCol::SYMBOL(stock_n->m_symbol)
    );
    if (stock_a.size() == 1) {
        Savepoint();
        for (const auto& sh_d : StockHistoryModel::instance().find(
            StockHistoryCol::SYMBOL(stock_n->m_symbol)
        ))
            StockHistoryModel::instance().purge_id(sh_d.id());
        ReleaseSavepoint();
    }

    // FIXME: remove AttachmentData owned by id

    return unsafe_remove_data(id);
}

/**
Returns the last price date of a given stock
*/
wxString StockModel::lastPriceDate(const Data* entity)
{
    wxString dtStr = entity->m_purchase_date;
    StockHistoryModel::DataA sh_a = StockHistoryModel::instance().find(
        StockCol::SYMBOL(entity->m_symbol)
    );

    std::sort(sh_a.begin(), sh_a.end(), StockHistoryData::SorterByDATE());
    if (!sh_a.empty())
        dtStr = sh_a.back().m_date;

    return dtStr;
}

/**
Returns the total stock balance at a given date
*/
double StockModel::getDailyBalanceAt(const AccountData *account, const wxDate& date)
{
    wxString strDate = date.FormatISODate();
    std::map<int64, double> totBalance;

    DataA stocks = this->instance().find(StockCol::HELDAT(account->id()));
    for (const auto & stock : stocks)
    {
        wxString precValueDate, nextValueDate;
        StockHistoryModel::DataA stock_hist = StockHistoryModel::instance().find(
            StockCol::SYMBOL(stock.m_symbol)
        );
        std::stable_sort(stock_hist.begin(), stock_hist.end(), StockHistoryData::SorterByDATE());
        std::reverse(stock_hist.begin(), stock_hist.end());

        double valueAtDate = 0.0,  precValue = 0.0, nextValue = 0.0;

        for (const auto & sh_d : stock_hist)
        {
            // test for the date requested
            if (sh_d.m_date == strDate) {
                valueAtDate = sh_d.m_price;
                break;
            }
            // if not found, search for previous and next date
            if (precValue == 0.0 && sh_d.m_date < strDate) {
                precValue = sh_d.m_price;
                precValueDate = sh_d.m_date;
            }
            if (sh_d.m_date > strDate) {
                nextValue = sh_d.m_price;
                nextValueDate = sh_d.m_date;
            }
            // end conditions: prec value assigned and price date < requested date
            if (precValue != 0.0 && sh_d.m_date < strDate)
                break;
        }
        if (valueAtDate == 0.0) {
            //  if previous not found but if the given date is after purchase date, takes purchase price
            if (precValue == 0.0 && date >= PURCHASEDATE(stock)) {
                precValue = stock.m_purchase_price;
                precValueDate = stock.m_purchase_date;
            }
            //  if next not found and the accoung is open, takes previous date
            if (nextValue == 0.0 && account->is_open()) {
                nextValue = precValue;
                nextValueDate = precValueDate;
            }
            if (precValue > 0.0 && nextValue > 0.0 && precValueDate >= stock.m_purchase_date && nextValueDate >= stock.m_purchase_date)
                valueAtDate = precValue;
        }

        double numShares = 0.0;

        TrxLinkModel::DataA linkrecords = TrxLinkModel::TranslinkList<StockModel>(stock.m_id);
        for (const auto& linkrecord : linkrecords) {
            const TrxData* txn = TrxModel::instance().get_data_n(linkrecord.CHECKINGACCOUNTID);
            if (txn->TRANSID > -1 && txn->DELETEDTIME.IsEmpty() && TrxModel::getTransDateTime(*txn).FormatISODate() <= strDate) {
                numShares += TrxShareModel::instance().unsafe_get_trx_share_n(linkrecord.CHECKINGACCOUNTID)->SHARENUMBER;
            }
        }

        if (linkrecords.empty() && stock.m_purchase_date <= strDate)
            numShares = stock.m_num_shares;

        totBalance[stock.id()] += numShares * valueAtDate;
    }

    double balance = 0.0;
    for (const auto& it : totBalance)
        balance += it.second;

    return balance;
}

/**
Returns the realized gain/loss of the stock due to sold shares.
If the optional parameter to_base_curr = true is passed values are converted
to base currency.
*/
double StockModel::RealGainLoss(const Data* stock_n, bool to_base_curr)
{
    const CurrencyData* currency = AccountModel::instance().get_id_currency_p(
        stock_n->m_account_id
    );
    TrxLinkModel::DataA trans_list = TrxLinkModel::TranslinkList<StockModel>(stock_n->m_id);
    double real_gain_loss = 0;
    double total_shares = 0;
    double total_initial_value = 0;
    double avg_share_price = 0;
    double conv_rate = 1;

    TrxModel::DataA checking_list;
    for (const auto &trans : trans_list)
    {
        const TrxData* checking_entry = TrxModel::instance().get_data_n(trans.CHECKINGACCOUNTID);
        if (checking_entry->TRANSID > -1 && checking_entry->DELETEDTIME.IsEmpty()) checking_list.push_back(*checking_entry);
    }
    std::stable_sort(checking_list.begin(), checking_list.end(), TrxData::SorterByTRANSDATE());

    for (const auto &trans : checking_list)
    {
        const TrxShareData* share_entry = TrxShareModel::instance().unsafe_get_trx_share_n(trans.TRANSID);
        conv_rate = to_base_curr ? CurrencyHistoryModel::getDayRate(currency->m_id, trans.TRANSDATE) : 1;
        total_shares += share_entry->SHARENUMBER;

        if (share_entry->SHARENUMBER > 0) {
            total_initial_value += (share_entry->SHARENUMBER * share_entry->SHAREPRICE + share_entry->SHARECOMMISSION) * conv_rate;
        }
        else {
            total_initial_value += share_entry->SHARENUMBER * avg_share_price;
            real_gain_loss += -share_entry->SHARENUMBER * (share_entry->SHAREPRICE * conv_rate - avg_share_price) - share_entry->SHARECOMMISSION * conv_rate;
        }

        if (total_shares < 0) total_shares = 0;
        if (total_initial_value < 0) total_initial_value = 0;
        if (total_shares > 0) avg_share_price = total_initial_value / total_shares;
        else avg_share_price = 0;
    }

    return real_gain_loss;
}

/**
Returns the realized gain/loss of the stock due to sold shares.
If the optional parameter to_base_curr = true is passed values are converted
to base currency.
*/
double StockModel::RealGainLoss(const Data& r, bool to_base_curr)
{
    return RealGainLoss(&r, to_base_curr);
}

/**
Returns the current unrealized gain/loss.
If the optional parameter to_base_curr = true is passed values are converted
to base currency.
*/
double StockModel::UnrealGainLoss(const Data& r, bool to_base_curr)
{
    return UnrealGainLoss(&r, to_base_curr);
}

/**
Returns the current unrealized gain/loss.
If the optional parameter to_base_curr = true is passed values are converted
to base currency.
*/
double StockModel::UnrealGainLoss(const Data* r, bool to_base_curr)
{
    if (!to_base_curr)
        return CurrentValue(r) - InvestmentValue(r);

    const CurrencyData* currency = AccountModel::instance().get_id_currency_p(
        r->m_account_id
    );
    double conv_rate = CurrencyHistoryModel::getDayRate(currency->m_id);
    TrxLinkModel::DataA trans_list = TrxLinkModel::TranslinkList<StockModel>(r->m_id);
    if (!trans_list.empty()) {
        double total_shares = 0;
        double total_initial_value = 0;
        double avg_share_price = 0;

        TrxModel::DataA checking_list;
        for (const auto &trans : trans_list) {
            const TrxData* checking_entry = TrxModel::instance().get_data_n(trans.CHECKINGACCOUNTID);
            if (checking_entry->TRANSID > -1 && checking_entry->DELETEDTIME.IsEmpty())
                checking_list.push_back(*checking_entry);
        }
        std::stable_sort(checking_list.begin(), checking_list.end(),
            TrxData::SorterByTRANSDATE()
        );

        for (const auto &trans : checking_list) {
            const TrxShareData* share_entry = TrxShareModel::instance().unsafe_get_trx_share_n(trans.TRANSID);
            conv_rate = CurrencyHistoryModel::getDayRate(currency->m_id, trans.TRANSDATE);
            total_shares += share_entry->SHARENUMBER;
            if (total_shares < 0) total_shares = 0;

            if (share_entry->SHARENUMBER > 0) {
                total_initial_value += (share_entry->SHARENUMBER * share_entry->SHAREPRICE + share_entry->SHARECOMMISSION) * conv_rate;
            }
            else {
                total_initial_value += share_entry->SHARENUMBER * avg_share_price;
            }

            if (total_initial_value < 0) total_initial_value = 0;
            if (total_shares > 0) avg_share_price = total_initial_value / total_shares;
        }
        conv_rate = CurrencyHistoryModel::getDayRate(currency->m_id);
        return CurrentValue(r) * conv_rate - total_initial_value;
    }
    else {
        return (CurrentValue(r) - InvestmentValue(r)) * conv_rate;
    }
}

/** Updates the current price across all accounts which hold the stock */
void StockModel::UpdateCurrentPrice(const wxString& symbol, const double price)
{
    double current_price = price;
    if (price == -1) {
        StockHistoryModel::DataA sh_a = StockHistoryModel::instance().find(
            StockHistoryCol::SYMBOL(symbol)
        );
        if (!sh_a.empty()) {
            std::sort(sh_a.begin(), sh_a.end(), StockHistoryData::SorterByDATE());
            current_price = sh_a.back().m_price;
        }
    }
    if (current_price != -1) {
        StockModel::DataA stock_a = StockModel::instance().find(
            StockCol::SYMBOL(symbol)
        );
        for (auto& stock_d : stock_a) {
            // CHECK: use stock_d directly
            StockData* stock_n = StockModel::instance().unsafe_get_data_n(stock_d.m_id);
            stock_n->m_current_price = current_price;
            StockModel::instance().unsafe_update_data_n(stock_n);
        }
    }
}

void StockModel::UpdatePosition(StockData* stock_n)
{
    TrxLinkModel::DataA tl_a = TrxLinkModel::TranslinkList<StockModel>(stock_n->m_id);
    double total_shares = 0;
    double total_initial_value = 0;
    double total_commission = 0;
    double avg_share_price = 0;
    wxString earliest_date = wxDate::Today().FormatISODate();
    TrxModel::DataA trx_a;
    for (const auto& tl_d : tl_a) {
        const TrxData* trx_n = TrxModel::instance().get_data_n(tl_d.CHECKINGACCOUNTID);
        if (trx_n->TRANSID > -1 && trx_n->DELETEDTIME.IsEmpty() && TrxModel::status_id(trx_n->STATUS) != TrxModel::STATUS_ID_VOID)
            trx_a.push_back(*trx_n);
    }
    std::stable_sort(trx_a.begin(), trx_a.end(), TrxData::SorterByTRANSDATE());
    for (const auto& trx_d : trx_a) {
        const TrxShareData* ts_n = TrxShareModel::instance().unsafe_get_trx_share_n(trx_d.TRANSID);

        total_shares += ts_n->SHARENUMBER;
        if (total_shares < 0)
            total_shares = 0;

        if (ts_n->SHARENUMBER > 0)
            total_initial_value += ts_n->SHARENUMBER * ts_n->SHAREPRICE + ts_n->SHARECOMMISSION;
        else
            total_initial_value += ts_n->SHARENUMBER * avg_share_price;

        if (total_initial_value < 0)
            total_initial_value = 0;
        if (total_shares > 0)
            avg_share_price = total_initial_value / total_shares;

        total_commission += ts_n->SHARECOMMISSION;

        wxString transdate = trx_d.TRANSDATE;
        if (transdate < earliest_date)
            earliest_date = transdate;
    }

    // The stock record contains the total of share transactions.
    if (tl_a.empty()) {
        stock_n->m_purchase_price = stock_n->m_current_price;
    }
    else {
        wxDateTime purchasedate;
        purchasedate.ParseDateTime(earliest_date) || purchasedate.ParseDate(earliest_date);
        stock_n->m_purchase_date  = purchasedate.FormatISODate();
        stock_n->m_purchase_price = avg_share_price;
        stock_n->m_num_shares     = total_shares;
        stock_n->m_purchase_value = total_initial_value;
        stock_n->m_commission     = total_commission;
    }
    StockModel::instance().unsafe_save_data_n(stock_n);
}
