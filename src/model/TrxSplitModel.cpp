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

#include "TrxSplitModel.h"
#include "CategoryModel.h"
#include "TrxModel.h"

TrxSplitModel::TrxSplitModel() :
    TableFactory<TrxSplitTable, TrxSplitData>()
{
}

TrxSplitModel::~TrxSplitModel()
{
}

/**
* Initialize the global TrxSplitModel table.
* Reset the TrxSplitModel table or create the table if it does not exist.
*/
TrxSplitModel& TrxSplitModel::instance(wxSQLite3Database* db)
{
    TrxSplitModel& ins = Singleton<TrxSplitModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();

    return ins;
}

/** Return the static instance of TrxSplitModel table */
TrxSplitModel& TrxSplitModel::instance()
{
    return Singleton<TrxSplitModel>::instance();
}

bool TrxSplitModel::purge_id(int64 id)
{
    // remove TagLinkData owned by id
    TagLinkModel::instance().DeleteAllTags(TrxSplitModel::refTypeName, id);

    return unsafe_remove_id(id);
}

double TrxSplitModel::get_total(const DataA& rows)
{
    double total = 0.0;
    for (auto& r : rows) total += r.SPLITTRANSAMOUNT;
    return total;
}
double TrxSplitModel::get_total(const std::vector<Split>& rows)
{
    double total = 0.0;
    for (auto& r : rows) total += r.SPLITTRANSAMOUNT;
    return total;
}

std::map<int64, TrxSplitModel::DataA> TrxSplitModel::get_all_id()
{
    std::map<int64, TrxSplitModel::DataA> data;
    for (const auto &split : instance().find_all()) {
        data[split.TRANSID].push_back(split);
    }
    return data;
}

int TrxSplitModel::update(DataA& rows, int64 transactionID)
{
    bool save_timestamp = false;
    std::map<int, int64> row_id_map;

    DataA split = instance().find(TrxSplitCol::TRANSID(transactionID));
    if (split.size() != rows.size()) save_timestamp = true;

    for (const auto& split_item : split) {
        if (!save_timestamp) {
            bool match = false;
            for (decltype(rows.size()) i = 0; i < rows.size(); i++) {
                match = (rows[i].CATEGID == split_item.CATEGID
                        && rows[i].SPLITTRANSAMOUNT == split_item.SPLITTRANSAMOUNT
                        && rows[i].NOTES.IsSameAs(split_item.NOTES))
                    && (row_id_map.find(i) == row_id_map.end());
                if (match) {
                    row_id_map[i] = split_item.SPLITTRANSID;
                    break;
                }
                    
            }
            save_timestamp = save_timestamp || !match;
        }

        instance().purge_id(split_item.SPLITTRANSID);
    }

    if (!rows.empty()) {
        for (auto &item : rows) {
            Data split_d = Data();
            split_d.TRANSID          = transactionID;
            split_d.SPLITTRANSAMOUNT = item.SPLITTRANSAMOUNT;
            split_d.CATEGID          = item.CATEGID;
            split_d.NOTES            = item.NOTES;
            instance().add_data_n(split_d);
            item.SPLITTRANSID = split_d.id();
        }
    }

    if (save_timestamp)
        TrxModel::instance().save_timestamp(transactionID);
    
    return rows.size();
}

int TrxSplitModel::update(const std::vector<Split>& rows, int64 transactionID)
{

    DataA splits;
    for (const auto& entry : rows) {
        Data s = Data();
        s.CATEGID = entry.CATEGID;
        s.SPLITTRANSAMOUNT = entry.SPLITTRANSAMOUNT;
        s.NOTES = entry.NOTES;
        splits.push_back(s);
    }

    return this->update(splits, transactionID);
}

const wxString TrxSplitModel::get_tooltip(const std::vector<Split>& rows, const CurrencyData* currency)
{
    wxString split_tooltip = "";
    for (const auto& entry : rows)
    {
        split_tooltip += wxString::Format("%s = %s"
                    , CategoryModel::full_name(entry.CATEGID)
                    , CurrencyModel::toCurrency(entry.SPLITTRANSAMOUNT, currency));
        if (!entry.NOTES.IsEmpty())
        {
            wxString value = entry.NOTES;
            value.Replace("\n", " ");
            split_tooltip += wxString::Format(" (%s)", value);
        }
        split_tooltip += "\n";
    }
    split_tooltip = split_tooltip.Left(split_tooltip.Len()-1);
    return split_tooltip;
}
