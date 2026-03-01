/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)

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

#include "BudgetPeriodModel.h"
#include "BudgetModel.h"

BudgetPeriodModel::BudgetPeriodModel() :
    Model<BudgetPeriodTable, BudgetPeriodData>()
{
}

BudgetPeriodModel::~BudgetPeriodModel()
{
}

/**
* Initialize the global BudgetPeriodModel table.
* Reset the BudgetPeriodModel table or create the table if it does not exist.
*/
BudgetPeriodModel& BudgetPeriodModel::instance(wxSQLite3Database* db)
{
    BudgetPeriodModel& ins = Singleton<BudgetPeriodModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();

    return ins;
}

/** Return the static instance of BudgetPeriodModel table */
BudgetPeriodModel& BudgetPeriodModel::instance()
{
    return Singleton<BudgetPeriodModel>::instance();
}

bool BudgetPeriodModel::remove_depen(int64 id)
{
    for (const BudgetData& d : BudgetModel::instance().find(
        BudgetCol::BUDGETYEARID(id)
    ))
        BudgetModel::instance().remove_depen(d.BUDGETENTRYID);
    return remove_data(id);
}

// Setter
void BudgetPeriodModel::Set(int64 year_id, const wxString& value)
{
    Data* bp_n = unsafe_get_data_n(year_id);
    if (bp_n) {
        bp_n->BUDGETYEARNAME = value;
        unsafe_update_data_n(bp_n);
    }
    else {
        Data new_bp_d = Data();
        new_bp_d.BUDGETYEARID = year_id;
        new_bp_d.BUDGETYEARNAME = value;
        add_data_n(new_bp_d);
    }
}

int64 BudgetPeriodModel::Add(const wxString& value)
{
    int64 year_id = this->Get(value);
    if (year_id < 0) {
        Data new_bp_d = Data();
        new_bp_d.BUDGETYEARNAME = value;
        add_data_n(new_bp_d);
        year_id = new_bp_d.id();
    }
    return year_id;
}

// Getter
wxString BudgetPeriodModel::Get(int64 year_id)
{
    const Data* bp_n = get_data_n(year_id);
    return bp_n ? bp_n->BUDGETYEARNAME : "";
}

int64 BudgetPeriodModel::Get(const wxString& year_name)
{
    for (const auto& record: this->find_all()) {
        if (record.BUDGETYEARNAME == year_name)
            return record.BUDGETYEARID;
    }

    return -1;
}

bool BudgetPeriodModel::Exists(int64 year_id)
{
    return get_data_n(year_id) != nullptr;
}

bool BudgetPeriodModel::Exists(const wxString& year_name)
{
    for (const auto& record: this->find_all()) {
        if (record.BUDGETYEARNAME == year_name) 
            return true;
    }
    return false;
}
