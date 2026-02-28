/*******************************************************
 Copyright (C) 2013,2014 James Higley
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

#include "SchedSplitModel.h"
#include "AttachmentModel.h"
#include "TagLinkModel.h"

SchedSplitModel::SchedSplitModel() :
    TableFactory<SchedSplitTable, SchedSplitData>()
{
}

SchedSplitModel::~SchedSplitModel()
{
}

/**
* Initialize the global SchedSplitModel table.
* Reset the SchedSplitModel table or create the table if it does not exist.
*/
SchedSplitModel& SchedSplitModel::instance(wxSQLite3Database* db)
{
    SchedSplitModel& ins = Singleton<SchedSplitModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();

    return ins;
}

/** Return the static instance of SchedSplitModel table */
SchedSplitModel& SchedSplitModel::instance()
{
    return Singleton<SchedSplitModel>::instance();
}

double SchedSplitModel::get_total(const DataA& rows)
{
    double total = 0.0;
    for (auto& r : rows) total += r.SPLITTRANSAMOUNT;

    return total;
}

bool SchedSplitModel::purge_id(int64 id)
{
    // remove TagLinkData owned by id
    TagLinkModel::instance().DeleteAllTags(SchedSplitModel::refTypeName, id);

    return unsafe_remove_id(id);
}

std::map<int64, SchedSplitModel::DataA> SchedSplitModel::get_all_id()
{
    std::map<int64, SchedSplitModel::DataA> data;
    for (const auto & split : instance().find_all())
    {
        data[split.TRANSID].push_back(split);
    }

    return data;
}

int SchedSplitModel::update(DataA& rows, int64 transactionID)
{

    DataA split_a = instance().find(SchedSplitCol::TRANSID(transactionID));
    for (const auto& split_d : split_a) {
        instance().purge_id(split_d.id());
    }

    if (!rows.empty()) {
        DataA new_split_a;
        for (const auto &item : rows) {
            Data new_split_d = Data();
            new_split_d.TRANSID          = transactionID;
            new_split_d.SPLITTRANSAMOUNT = item.SPLITTRANSAMOUNT;
            new_split_d.CATEGID          = item.CATEGID;
            new_split_d.NOTES            = item.NOTES;
            new_split_a.push_back(new_split_d);
        }
        instance().save_data_a(new_split_a);

        // Send back the new SPLITTRANSID which is needed to update taglinks
        for (int i = 0; i < static_cast<int>(rows.size()); i++)
            rows.at(i).SPLITTRANSID = new_split_a.at(i).SPLITTRANSID;
    }
    return rows.size();
}
