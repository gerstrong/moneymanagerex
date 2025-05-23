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

#include "summarystocks.h"
#include "reports/htmlbuilder.h"

#include "constants.h"
#include "stockspanel.h"
#include "budget.h"
#include "util.h"
#include "reports/mmDateRange.h"
#include "model/Model_Account.h"
#include "model/Model_Currency.h"
#include "model/Model_CurrencyHistory.h"
#include "model/Model_StockHistory.h"

#include <algorithm>

mmReportSummaryStocks::mmReportSummaryStocks()
    : mmPrintableBase(_n("Summary of Stocks"))
{
    setReportParameters(Reports::StocksReportSummary);
}

void  mmReportSummaryStocks::RefreshData()
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

    for (const auto& a : Model_Account::instance().all(Model_Account::COL_ACCOUNTNAME))
    {
        if (Model_Account::type_id(a) != Model_Account::TYPE_ID_INVESTMENT) continue;
        if (Model_Account::status_id(a) != Model_Account::STATUS_ID_OPEN) continue;

        account.id = a.id();
        account.name = a.ACCOUNTNAME;
        account.realgainloss = 0.0;
        account.unrealgainloss = 0.0;
        account.total = Model_Account::investment_balance(a).first;
        account.data.clear();

        for (const auto& stock : Model_Stock::instance().find(Model_Stock::HELDAT(a.ACCOUNTID)))
        {
            const Model_Currency::Data* currency = Model_Account::currency(a);
            const double today_rate = Model_CurrencyHistory::getDayRate(currency->CURRENCYID, today);
            m_stock_balance += today_rate * Model_Stock::CurrentValue(stock);
            line.realgainloss = Model_Stock::RealGainLoss(stock);
            account.realgainloss += line.realgainloss;
            line.unrealgainloss = Model_Stock::UnrealGainLoss(stock);
            account.unrealgainloss += line.unrealgainloss;
            m_unreal_gain_loss_sum_total += Model_Stock::UnrealGainLoss(stock, true);
            m_real_gain_loss_sum_total += Model_Stock::RealGainLoss(stock, true);
            m_real_gain_loss_excl_forex += line.realgainloss * today_rate;
            m_unreal_gain_loss_excl_forex += line.unrealgainloss * today_rate;

            line.name = stock.STOCKNAME;
            line.symbol = stock.SYMBOL;
            line.date = stock.PURCHASEDATE;
            line.qty = stock.NUMSHARES;
            line.purchase = Model_Stock::InvestmentValue(stock);
            line.current = stock.CURRENTPRICE;
            line.commission = stock.COMMISSION;
            line.value = Model_Stock::CurrentValue(stock);
            account.data.push_back(line);
        }
        m_stocks.push_back(account);
    }
}

wxString mmReportSummaryStocks::getHTMLText()
{
    // Grab the data  
    RefreshData();

    // Build the report
    mmHTMLBuilder hb;
    hb.init();
    hb.addReportHeader(getReportTitle());

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
                const Model_Account::Data* account = Model_Account::instance().get(acct.id);
                const Model_Currency::Data* currency = Model_Account::currency(account);

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
                            hb.addTableCell(Model_Account::toString(entry.qty, account, trunc(entry.qty) == entry.qty ? 0 : 4), "text-right");
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
                int scale = pow(10, log10(Model_Currency::instance().GetBaseCurrency()->SCALE.GetValue()));
                double forex_real_gain_loss = std::round((m_real_gain_loss_sum_total - m_real_gain_loss_excl_forex) * scale) / scale;
                double forex_unreal_gain_loss = std::round((m_unreal_gain_loss_sum_total - m_unreal_gain_loss_excl_forex) * scale) / scale;

                hb.startTotalTableRow();
                hb.addTableCell(_t("Grand Total:"));
                hb.addEmptyTableCell(6);

                hb.startTableCell(" style='text-align:right;' nowrap");
                if (forex_real_gain_loss != 0) {
                    hb.startSpan(Model_Currency::toCurrency(m_real_gain_loss_excl_forex), wxString::Format(" style='text-align:right;%s' nowrap"
                        , m_real_gain_loss_excl_forex < 0 ? "color:red;" : ""));
                    hb.endSpan();
                    hb.startSpan(" + ", "");
                    hb.endSpan();
                    hb.startSpan(Model_Currency::toCurrency(forex_real_gain_loss), wxString::Format(" style='text-align:right;%s' nowrap"
                        , forex_real_gain_loss < 0 ? "color:red;" : ""));
                    hb.endSpan();
                    hb.startSpan(" FX", "");
                    hb.endSpan();
                    hb.addLineBreak();
                }
                hb.startSpan(Model_Currency::toCurrency(m_real_gain_loss_sum_total), wxString::Format(" style='text-align:right;%s' nowrap"
                    , m_real_gain_loss_sum_total < 0 ? "color:red;" : ""));
                hb.endSpan();

                hb.endTableCell();

                hb.startTableCell(" style='text-align:right;' nowrap");
                if (forex_unreal_gain_loss != 0) {
                    hb.startSpan(Model_Currency::toCurrency(m_unreal_gain_loss_excl_forex), wxString::Format(" style='text-align:right;%s' nowrap"
                        , m_unreal_gain_loss_excl_forex < 0 ? "color:red;" : ""));
                    hb.endSpan();
                    hb.startSpan(" + ", "");
                    hb.endSpan();
                    hb.startSpan(Model_Currency::toCurrency(forex_unreal_gain_loss), wxString::Format(" style='text-align:right;%s' nowrap"
                        , forex_unreal_gain_loss < 0 ? "color:red;" : ""));
                    hb.endSpan();
                    hb.startSpan(" FX", "");
                    hb.endSpan();
                    hb.addLineBreak();
                }
                hb.startSpan(Model_Currency::toCurrency(m_unreal_gain_loss_sum_total), wxString::Format(" style='text-align:right;%s' nowrap"
                    , m_unreal_gain_loss_sum_total < 0 ? "color:red;" : ""));
                hb.endSpan();

                hb.endTableCell();
                
                hb.startTableCell(" style='text-align:right;' nowrap");
                hb.startSpan(Model_Currency::toCurrency(m_stock_balance), "");
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
    : mmPrintableBase(_n("Stocks Performance Charts"))
{
    setReportParameters(Reports::StocksReportPerformance);
}

mmReportChartStocks::~mmReportChartStocks()
{
}

wxString mmReportChartStocks::getHTMLText()
{
    // Build the report
    mmHTMLBuilder hb;
    hb.init();
    hb.addReportHeader(getReportTitle(), m_date_range->startDay(), m_date_range->isFutureIgnored());
    wxTimeSpan dtDiff = m_date_range->end_date() - m_date_range->start_date();
    if (m_date_range->is_with_date() && dtDiff.GetDays() <= 366)
        hb.DisplayDateHeading(m_date_range->start_date(), m_date_range->end_date(), true);

    wxTimeSpan dist;
    wxArrayString symbols;
    for (const auto& stock : Model_Stock::instance().all(Model_Stock::COL_SYMBOL))
    {
        Model_Account::Data* account = Model_Account::instance().get(stock.HELDAT);
        if (Model_Account::status_id(account) != Model_Account::STATUS_ID_OPEN) continue;
        if (symbols.Index(stock.SYMBOL) != wxNOT_FOUND) continue;

        symbols.Add(stock.SYMBOL);
        int dataCount = 0, freq = 1;
        auto histData = Model_StockHistory::instance().find(Model_StockHistory::SYMBOL(stock.SYMBOL),
            Model_StockHistory::DATE(m_date_range->start_date(), GREATER_OR_EQUAL),
            Model_StockHistory::DATE(m_date_range->end_date(), LESS_OR_EQUAL));
        std::stable_sort(histData.begin(), histData.end(), SorterByDATE());

        //bool showGridLines = (histData.size() <= 366);
        //bool pointDot = (histData.size() <= 30);
        if (histData.size() > 366) {
            freq = histData.size() / 366;
        }

        GraphData gd;
        GraphSeries data;

        for (const auto& hist : histData)
        {
            if (dataCount % freq == 0)
            {
                const wxDate d = Model_StockHistory::DATE(hist);
                gd.labels.push_back(d.FormatISODate());
                data.values.push_back(hist.VALUE);
            }
            dataCount++;
        }
        gd.series.push_back(data);

        if (!gd.series.empty())
        {
            hb.addHeader(1, wxString::Format("%s / %s - (%s)", stock.SYMBOL, stock.STOCKNAME, account->ACCOUNTNAME));
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
