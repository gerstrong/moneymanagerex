/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)
 Copyright (C) 2022 Mark Whalley (mark@ipx.co.uk)

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

#include "AssetModel.h"
#include "TrxLinkModel.h"
#include "CurrencyHistoryModel.h"

AssetModel::AssetModel() :
    TableFactory<AssetTable, AssetData>()
{
}

AssetModel::~AssetModel()
{
}

/**
* Initialize the global AssetModel table.
* Reset the AssetModel table or create the table if it does not exist.
*/
AssetModel& AssetModel::instance(wxSQLite3Database* db)
{
    AssetModel& ins = Singleton<AssetModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();

    return ins;
}

/** Return the static instance of AssetModel table */
AssetModel& AssetModel::instance()
{
    return Singleton<AssetModel>::instance();
}

AssetCol::ASSETTYPE AssetModel::ASSETTYPE(OP op, AssetType type)
{
    return AssetCol::ASSETTYPE(op, type.name());
}

AssetCol::STARTDATE AssetModel::STARTDATE(OP op, const wxDate& date)
{
    return AssetCol::STARTDATE(op, date.FormatISODate());
}

wxString AssetModel::get_id_name(int64 asset_id)
{
    const Data* asset_n = get_data_n(asset_id);
    if (asset_n)
        return asset_n->m_name;
    else
        return _t("Asset Error");
}

std::pair<double, double> AssetModel::valueAtDate(const Data& asset_d, const wxDate& date)
{
    std::pair<double /*initial*/, double /*market*/> balance;
    if (date < asset_d.STARTDATE())
        return balance;

    TrxLinkModel::DataA translink_records = TrxLinkModel::instance().find(
        TrxLinkCol::LINKRECORDID(asset_d.m_id),
        TrxLinkCol::LINKTYPE(this->refTypeName)
    );

    double dailyRate = asset_d.m_change_rate / 36500.0;
    mmChoiceId changeType = asset_d.m_change.id();

    auto applyChangeRate = [changeType, dailyRate](double& value, double days) {
        if (changeType == AssetChange::e_appreciates) {
            value *= exp(dailyRate * days);
        }
        else if (changeType == AssetChange::e_depreciates) {
            value *= exp(-dailyRate * days);
        }
    };

    if (!translink_records.empty()) {
        TrxModel::DataA trans;
        for (const auto& link : translink_records) {
            const TrxData* tran = TrxModel::instance().get_data_n(link.CHECKINGACCOUNTID);
            if(tran && tran->DELETEDTIME.IsEmpty()) trans.push_back(*tran);
        }

        std::stable_sort(trans.begin(), trans.end(), TrxData::SorterByTRANSDATE());

        wxDate last = date;
        for (const auto& tran: trans) {
            if (tran.ACCOUNTID < 0) {
              continue;
            }

            const wxDate tranDate = TrxModel::getTransDateTime(tran);
            if (tranDate > date) break;

            if (last == date) last = tranDate;
            if (last < tranDate) {
                applyChangeRate(balance.second, static_cast<double>((tranDate - last).GetDays()));
                last = tranDate;
            }

            double accflow = TrxModel::account_flow(tran, tran.ACCOUNTID);
            double amount = -1 * accflow *
                CurrencyHistoryModel::getDayRate(
                    AccountModel::instance().get_data_n(tran.ACCOUNTID)->m_currency_id,
                    tranDate
            );
            //double amount = -1 * TrxModel::account_flow(tran, tran.ACCOUNTID) *
            //    CurrencyHistoryModel::getDayRate(AccountModel::instance().get_data_n(tran.ACCOUNTID)->CURRENCYID, tranDate);

            if (amount >= 0) {
                balance.first += amount;
            }
            else {
                double unrealized_gl = balance.second - balance.first;
                balance.first += std::min(unrealized_gl + amount, 0.0);
            }

            balance.second += amount;

            // Self Transfer as Revaluation
            if (tran.ACCOUNTID == tran.TOACCOUNTID &&
                TrxModel::type_id(tran.TRANSCODE) == TrxModel::TYPE_ID_TRANSFER
            ) {
                // TODO honor TRANSAMOUNT => TOTRANSAMOUNT
                balance.second = tran.TOTRANSAMOUNT;
            }
        }

        applyChangeRate(balance.second, static_cast<double>((date - last).GetDays()));
    }
    else {
        balance = {asset_d.m_value, asset_d.m_value};
        applyChangeRate(
            balance.second,
            static_cast<double>((date - asset_d.STARTDATE()).GetDays())
        );
    }
    return balance;
}

std::pair<double, double> AssetModel::value(const Data& asset_d)
{
    return valueAtDate(asset_d, wxDate::Today());
}

double AssetModel::balance()
{
    double balance = 0.0;
    for (const auto& asset_d: this->find_all()) {
        balance += value(asset_d).second;
    }
    return balance;
}
