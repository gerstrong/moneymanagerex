/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)
 Copyright (C) 2016 Stefano Giorgio
 Copyright (C) 2025 Klaus Wich

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

#include "TransactionLinkModel.h"
#include "TransactionShareModel.h"
#include "CurrencyHistoryModel.h"

TransactionLinkModel::TransactionLinkModel() :
    Model<TransactionLinkTable, TransactionLinkData>()
{
}

TransactionLinkModel::~TransactionLinkModel()
{
}

/**
* Initialize the global TransactionLinkModel table.
* Reset the TransactionLinkModel table or create the table if it does not exist.
*/
TransactionLinkModel& TransactionLinkModel::instance(wxSQLite3Database* db)
{
    TransactionLinkModel& ins = Singleton<TransactionLinkModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();

    return ins;
}

/** Return the static instance of TransactionLinkModel table */
TransactionLinkModel& TransactionLinkModel::instance()
{
    return Singleton<TransactionLinkModel>::instance();
}

TransactionLinkModel::CHECKING_TYPE TransactionLinkModel::type_checking(const int64 tt)
{
    if (tt == AS_INCOME_EXPENSE || tt == -1)
    {
        return AS_INCOME_EXPENSE;
    }
    else
    {
        return AS_TRANSFER;
    }
}

void TransactionLinkModel::SetAssetTranslink(const int64 asset_id
    , const int64 checking_id
    , const CHECKING_TYPE checking_type)
{
    SetTranslink(checking_id, checking_type, AssetModel::refTypeName, asset_id);
}

void TransactionLinkModel::SetStockTranslink(
    const int64 stock_id,
    const int64 checking_id,
    const CHECKING_TYPE checking_type
) {
    SetTranslink(checking_id, checking_type, StockModel::refTypeName, stock_id);
}

void TransactionLinkModel::SetTranslink(
    const int64 checking_id,
    [[maybe_unused]] const CHECKING_TYPE checking_type,
    const wxString& link_type,
    const int64 link_record_id
) {
    TransactionLinkData new_tl_d = TransactionLinkData();
    new_tl_d.CHECKINGACCOUNTID = checking_id;
    new_tl_d.LINKTYPE          = link_type;
    new_tl_d.LINKRECORDID      = link_record_id;
    TransactionLinkModel::instance().add_data_n(new_tl_d);

    // set the checking entry to recognise it as a foreign transaction
    // set the checking type as AS_INCOME_EXPENSE = 32701 or AS_TRANSFER
    TransactionData* trx_n = TransactionModel::instance().unsafe_get_data_n(checking_id);
    // trx_n->TOACCOUNTID = checking_type;
    TransactionModel::instance().unsafe_save_trx(trx_n);
    //TransactionLinkModel::instance().get_data_n(new_tl_d.id());
}

template <typename T>
TransactionLinkModel::DataA TransactionLinkModel::TranslinkList(const int64 link_entry_id)
{
    TransactionLinkModel::DataA translink_list = TransactionLinkModel::instance().find(
        TransactionLinkCol::LINKTYPE(T::refTypeName),
        TransactionLinkCol::LINKRECORDID(link_entry_id)
    );

    return translink_list;
}

TransactionLinkModel::DataA TransactionLinkModel::TranslinkListBySymbol(const wxString symbol)
{
    TransactionLinkModel::DataA result;
    StockModel::DataA stocks = StockModel::instance().find(StockCol::SYMBOL(symbol));
    for (auto& stock : stocks) {
       TransactionLinkModel::DataA t = TransactionLinkModel::instance().find(
            TransactionLinkCol::LINKRECORDID(stock.STOCKID)
        );
       result.insert(result.end(), t.begin(), t.end());
    }
    return result;
}

bool TransactionLinkModel::HasShares(const int64 stock_id)
{
    if (TranslinkList<StockModel>(stock_id).empty())
    {
        return false;
    }

    return true;
}

TransactionLinkData TransactionLinkModel::TranslinkRecord(const int64 checking_id)
{
    auto i = TransactionLinkCol::CHECKINGACCOUNTID(checking_id);
    TransactionLinkModel::DataA translink_list = TransactionLinkModel::instance().find(i);

    if (!translink_list.empty())
        return *translink_list.begin();
    else {
        wxSharedPtr<TransactionLinkData> t(new TransactionLinkData);
        return *t;
    }
}

template <typename T>
void TransactionLinkModel::RemoveTransLinkRecords(const int64 entry_id)
{
    for (const auto& translink : TranslinkList<T>(entry_id))
    {
        TransactionModel::instance().remove_depen(translink.CHECKINGACCOUNTID);
    }
}

// Explicit Instantiation
template void TransactionLinkModel::RemoveTransLinkRecords<AssetModel>(const int64);
template void TransactionLinkModel::RemoveTransLinkRecords<StockModel>(const int64);

void TransactionLinkModel::RemoveTranslinkEntry(const int64 checking_account_id)
{
    Data translink = TranslinkRecord(checking_account_id);
    TransactionShareModel::RemoveShareEntry(translink.CHECKINGACCOUNTID);
    TransactionLinkModel::instance().remove_depen(translink.TRANSLINKID);

    if (translink.LINKTYPE == AssetModel::refTypeName) {
        AssetData* asset_entry = AssetModel::instance().unsafe_get_data_n(translink.LINKRECORDID);
        UpdateAssetValue(asset_entry);
    }

    if (translink.LINKTYPE == StockModel::refTypeName) {
        StockData* stock_entry = StockModel::instance().unsafe_get_data_n(translink.LINKRECORDID);
        StockModel::UpdatePosition(stock_entry);
    }
}

void TransactionLinkModel::UpdateAssetValue(AssetData* asset_n)
{
    DataA trans_list = TranslinkList<AssetModel>(asset_n->ASSETID);
    double new_value = 0;
    for (const auto &trans : trans_list) {
        const TransactionData* trx_n = TransactionModel::instance().get_data_n(trans.CHECKINGACCOUNTID);
        if (trx_n && trx_n->DELETEDTIME.IsEmpty()
            && TransactionModel::status_id(trx_n->STATUS) != TransactionModel::STATUS_ID_VOID
        ) {
            const CurrencyData* currency_n = AccountModel::currency(
                AccountModel::instance().get_data_n(trx_n->ACCOUNTID)
            );
            const double conv_rate = CurrencyHistoryModel::getDayRate(
                currency_n->CURRENCYID,
                trx_n->TRANSDATE
            );

            if (trx_n->TRANSCODE == TransactionModel::TYPE_NAME_DEPOSIT) {
                new_value -= trx_n->TRANSAMOUNT * conv_rate; // Withdrawal from asset value
            }
            else {
                new_value += trx_n->TRANSAMOUNT * conv_rate;  // Deposit to asset value
            }
        }
    }

    if (asset_n->VALUE != new_value) {
        asset_n->VALUE = new_value;
        AssetModel::instance().unsafe_save_data_n(asset_n);
    }
}

bool TransactionLinkModel::ShareAccountId(int64& stock_entry_id)
{
    TransactionLinkModel::DataA stock_translink_list = TranslinkList<StockModel>(stock_entry_id);

    if (!stock_translink_list.empty())
    {
        TransactionModel::DataA checking_entry = TransactionModel::instance().find(
            TransactionCol::TRANSID(stock_translink_list.at(0).CHECKINGACCOUNTID));
        if (!checking_entry.empty())
        {
            const AccountData* account_entry = AccountModel::instance().get_data_n(checking_entry.at(0).ACCOUNTID);
            stock_entry_id = account_entry->ACCOUNTID;
            return true;
        }
    }

    return false;
}
