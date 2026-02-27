/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2021,2024 Mark Whalley (mark@ipx.co.uk)

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

#include "base/defs.h"
#include <algorithm>
#include "base/constants.h"
#include "util/_util.h"
#include "util/mmDateRange.h"
#include "htmlbuilder.h"

#include "model/AccountModel.h"
#include "model/CurrencyModel.h"
#include "model/CurrencyHistoryModel.h"
#include "model/StockHistoryModel.h"

#include "panel/StockPanel.h"
#include "StocksReport.h"
#include "budget.h"
#include "uicontrols/navigatortypes.h"

StocksReport::StocksReport()
    : ReportBase(_n("Summary of Stocks"))
{
    setReportParameters(REPORT_ID::StocksReportSummary);
}

void  StocksReport::refreshData()
{
    m_stocks.clear();
    m_real_gain_loss_sum_total = 0.0;
    m_unreal_gain_loss_sum_total = 0.0;
    m_real_gain_loss_excl_forex = 0.0;
    m_unreal_gain_loss_excl_forex = 0.0;
    m_stock_balance = 0.0;

    data_holder line;
    account_holder account;
    const wxDate today = wxDate::Today();

    for (const auto& a : AccountModel::instance().find_all(AccountCol::COL_ID_ACCOUNTNAME))
    {
        if (AccountModel::type_id(a) != NavigatorTypes::TYPE_ID_INVESTMENT) continue;
        if (AccountModel::status_id(a) != AccountModel::STATUS_ID_OPEN) continue;

        account.id = a.id();
        account.name = a.ACCOUNTNAME;
        account.realgainloss = 0.0;
        account.unrealgainloss = 0.0;
        account.total = AccountModel::investment_balance(a).first;
        account.data.clear();

        for (const auto& stock : StockModel::instance().find(StockCol::HELDAT(a.ACCOUNTID)))
        {
            const CurrencyData* currency = AccountModel::currency(a);
            const double today_rate = CurrencyHistoryModel::getDayRate(currency->m_id, today);
            m_stock_balance += today_rate * StockModel::CurrentValue(stock);
            line.realgainloss = StockModel::RealGainLoss(stock);
            account.realgainloss += line.realgainloss;
            line.unrealgainloss = StockModel::UnrealGainLoss(stock);
            account.unrealgainloss += line.unrealgainloss;
            m_unreal_gain_loss_sum_total += StockModel::UnrealGainLoss(stock, true);
            m_real_gain_loss_sum_total += StockModel::RealGainLoss(stock, true);
            m_real_gain_loss_excl_forex += line.realgainloss * today_rate;
            m_unreal_gain_loss_excl_forex += line.unrealgainloss * today_rate;

            line.name       = stock.m_name;
            line.symbol     = stock.m_symbol;
            line.date       = stock.m_purchase_date;
            line.qty        = stock.m_num_shares;
            line.purchase   = StockModel::InvestmentValue(stock);
            line.current    = stock.m_current_price;
            line.commission = stock.m_commission;
            line.value      = StockModel::CurrentValue(stock);
            account.data.push_back(line);
        }
        m_stocks.push_back(account);
    }
}

wxString StocksReport::getHTMLText()
{
    // Grab the data  
    refreshData();

    // Build the report
    mmHTMLBuilder hb;
    hb.init();
    hb.addReportHeader(getTitle());

    hb.addDivContainer("shadow");
    {
        hb.startTable();
        {
            hb.startThead();
            {
                hb.startTableRow();
                {
                    hb.addTableHeaderCell(_t("Name"));
                    hb.addTableHeaderCell(_t("Symbol"));
                    hb.addTableHeaderCell(_t("Purchase Date"));
                    hb.addTableHeaderCell(_t("Quantity"), "text-right");
                    hb.addTableHeaderCell(_t("Total Cost"), "text-right");
                    hb.addTableHeaderCell(_t("Current Price"), "text-right");
                    hb.addTableHeaderCell(_t("Commission"), "text-right");
                    hb.addTableHeaderCell(_t("Realized Gain/Loss"), "text-right");
                    hb.addTableHeaderCell(_t("Unrealized Gain/Loss"), "text-right");
                    hb.addTableHeaderCell(_t("Current Value"), "text-right");
                }
                hb.endTableRow();
            }
            hb.endThead();

            for (const auto& acct : m_stocks)
            {
                const AccountData* account = AccountModel::instance().get_data_n(acct.id);
                const CurrencyData* currency = AccountModel::currency(account);

                hb.startThead();
                {
                    hb.startTableRow();
                    {
                        hb.addTableHeaderCell(acct.name, "text-left", 10);
                    }
                    hb.endTableRow();
                }
                hb.endThead();

                hb.startTbody();
                {
                    for (const auto& entry : acct.data)
                    {
                        hb.startTableRow();
                        {
                            hb.addTableCell(entry.name);
                            hb.addTableCell(entry.symbol);
                            hb.addTableCellDate(entry.date);
                            hb.addTableCell(AccountModel::toString(entry.qty, account, trunc(entry.qty) == entry.qty ? 0 : 4), "text-right");
                            hb.addCurrencyCell(entry.purchase, currency, 4);
                            hb.addCurrencyCell(entry.current, currency, 4);
                            hb.addCurrencyCell(entry.commission, currency, 4);
                            hb.addCurrencyCell(entry.realgainloss, currency);
                            hb.addCurrencyCell(entry.unrealgainloss, currency);
                            hb.addCurrencyCell(entry.value, currency);
                        }
                        hb.endTableRow();
                    }
                    hb.startTotalTableRow();
                    {
                        hb.addTableCell(_t("Total:"));
                        hb.addEmptyTableCell(6);
                        hb.addCurrencyCell(acct.realgainloss, currency);
                        hb.addCurrencyCell(acct.unrealgainloss, currency);
                        hb.addCurrencyCell(acct.total, currency);
                    }
                    hb.endTableRow();
                    hb.addEmptyTableRow(9);
                }
                hb.endTbody();
            }

            hb.startTfoot();
            {
                // Round FX gain/loss to the scale of the base currency for display
                int scale = pow(10, log10(CurrencyModel::instance().GetBaseCurrency()->m_scale.GetValue()));
                double forex_real_gain_loss = std::round((m_real_gain_loss_sum_total - m_real_gain_loss_excl_forex) * scale) / scale;
                double forex_unreal_gain_loss = std::round((m_unreal_gain_loss_sum_total - m_unreal_gain_loss_excl_forex) * scale) / scale;

                hb.startTotalTableRow();
                hb.addTableCell(_t("Grand Total:"));
                hb.addEmptyTableCell(6);

                hb.startTableCell(" style='text-align:right;' nowrap");
                if (forex_real_gain_loss != 0) {
                    hb.startSpan(CurrencyModel::toCurrency(m_real_gain_loss_excl_forex), wxString::Format(" style='text-align:right;%s' nowrap"
                        , m_real_gain_loss_excl_forex < 0 ? "color:red;" : ""));
                    hb.endSpan();
                    hb.startSpan(" + ", "");
                    hb.endSpan();
                    hb.startSpan(CurrencyModel::toCurrency(forex_real_gain_loss), wxString::Format(" style='text-align:right;%s' nowrap"
                        , forex_real_gain_loss < 0 ? "color:red;" : ""));
                    hb.endSpan();
                    hb.startSpan(" FX", "");
                    hb.endSpan();
                    hb.addLineBreak();
                }
                hb.startSpan(CurrencyModel::toCurrency(m_real_gain_loss_sum_total), wxString::Format(" style='text-align:right;%s' nowrap"
                    , m_real_gain_loss_sum_total < 0 ? "color:red;" : ""));
                hb.endSpan();

                hb.endTableCell();

                hb.startTableCell(" style='text-align:right;' nowrap");
                if (forex_unreal_gain_loss != 0) {
                    hb.startSpan(CurrencyModel::toCurrency(m_unreal_gain_loss_excl_forex), wxString::Format(" style='text-align:right;%s' nowrap"
                        , m_unreal_gain_loss_excl_forex < 0 ? "color:red;" : ""));
                    hb.endSpan();
                    hb.startSpan(" + ", "");
                    hb.endSpan();
                    hb.startSpan(CurrencyModel::toCurrency(forex_unreal_gain_loss), wxString::Format(" style='text-align:right;%s' nowrap"
                        , forex_unreal_gain_loss < 0 ? "color:red;" : ""));
                    hb.endSpan();
                    hb.startSpan(" FX", "");
                    hb.endSpan();
                    hb.addLineBreak();
                }
                hb.startSpan(CurrencyModel::toCurrency(m_unreal_gain_loss_sum_total), wxString::Format(" style='text-align:right;%s' nowrap"
                    , m_unreal_gain_loss_sum_total < 0 ? "color:red;" : ""));
                hb.endSpan();

                hb.endTableCell();
                
                hb.startTableCell(" style='text-align:right;' nowrap");
                hb.startSpan(CurrencyModel::toCurrency(m_stock_balance), "");
                hb.endSpan();

                hb.endTableCell();
            }
            hb.endTfoot();
        }
        hb.endTable();
    }
    hb.endDiv();

    hb.end();

    return hb.getHTMLText();
}

mmReportChartStocks::mmReportChartStocks()
    : ReportBase(_n("Stocks Performance Charts"))
{
    setReportParameters(REPORT_ID::StocksReportPerformance);
}

mmReportChartStocks::~mmReportChartStocks()
{
}

wxString mmReportChartStocks::getHTMLText()
{
    // Build the report
    mmHTMLBuilder hb;
    hb.init();
    hb.addReportHeader(getTitle(), m_date_range->startDay(), m_date_range->isFutureIgnored());
    wxTimeSpan dtDiff = m_date_range->end_date() - m_date_range->start_date();
    if (m_date_range->is_with_date() && dtDiff.GetDays() <= 366)
        hb.displayDateHeading(m_date_range->start_date(), m_date_range->end_date(), true);

    wxTimeSpan dist;
    wxArrayString symbols;
    for (const auto& stock_d : StockModel::instance().find_all(StockCol::COL_ID_SYMBOL)) {
        const AccountData* account = AccountModel::instance().get_data_n(stock_d.m_account_id);
        if (AccountModel::status_id(account) != AccountModel::STATUS_ID_OPEN) continue;
        if (symbols.Index(stock_d.m_symbol) != wxNOT_FOUND) continue;

        symbols.Add(stock_d.m_symbol);
        int dataCount = 0, freq = 1;
        auto sh_a = StockHistoryModel::instance().find(
            StockHistoryCol::SYMBOL(stock_d.m_symbol),
            StockHistoryModel::DATE(OP_GE, m_date_range->start_date()),
            StockHistoryModel::DATE(OP_LE, m_date_range->end_date())
        );
        std::stable_sort(sh_a.begin(), sh_a.end(), StockHistoryData::SorterByDATE());

        //bool showGridLines = (sh_a.size() <= 366);
        //bool pointDot = (sh_a.size() <= 30);
        if (sh_a.size() > 366) {
            freq = sh_a.size() / 366;
        }

        GraphData gd;
        GraphSeries data;

        for (const auto& sh_d : sh_a) {
            if (dataCount % freq == 0) {
                const wxDate d = StockHistoryModel::DATE(sh_d);
                gd.labels.push_back(d.FormatISODate());
                data.values.push_back(sh_d.m_price);
            }
            dataCount++;
        }
        gd.series.push_back(data);

        if (!gd.series.empty()) {
            hb.addHeader(1, wxString::Format("%s / %s - (%s)",
                stock_d.m_symbol, stock_d.m_name, account->ACCOUNTNAME
            ));
            gd.type = GraphData::LINE_DATETIME;
            hb.addChart(gd);
        }
    }
    hb.endDiv();
    
    hb.end();

    wxLogDebug("======= mmReportChartStocks:getHTMLText =======");
    wxLogDebug("%s", hb.getHTMLText());    

    return hb.getHTMLText();
}
