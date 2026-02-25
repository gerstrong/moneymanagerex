/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)
 Copyright (C) 2022 Mark Whalley (mark@ipx.co.uk)
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

#include "PayeeModel.h"
#include "TrxModel.h" // detect whether the payee is used or not
#include "SchedModel.h"

PayeeModel::PayeeModel() :
    TableFactory<PayeeTable, PayeeData>()
{
}

PayeeModel::~PayeeModel()
{
}

/**
* Initialize the global PayeeModel table.
* Reset the PayeeModel table or create the table if it does not exist.
*/
PayeeModel& PayeeModel::instance(wxSQLite3Database* db)
{
    PayeeModel& ins = Singleton<PayeeModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();
    ins.preload_cache();

    return ins;
}

/** Return the static instance of PayeeModel table */
PayeeModel& PayeeModel::instance()
{
    return Singleton<PayeeModel>::instance();
}

const PayeeModel::DataA PayeeModel::FilterPayees(const wxString& payee_pattern, bool includeInActive)
{
    DataA payees;
    for (auto& payee_d : this->find_all(PayeeCol::COL_ID_PAYEENAME)) {
        if (payee_d.m_name.Lower().Matches(payee_pattern.Lower().Append("*")) &&
            (includeInActive || payee_d.m_active)
        ) {
            payees.push_back(payee_d);
        }
    }
    return payees;
}

const PayeeData* PayeeModel::get_key(const wxString& name)
{
    const Data* payee_n = search_cache_n(PayeeCol::PAYEENAME(name));
    if (payee_n)
        return payee_n;

    DataA payee_a = this->find(PayeeCol::PAYEENAME(name));
    if (!payee_a.empty())
        payee_n = get_data_n(payee_a[0].m_id);
    return payee_n;
}

wxString PayeeModel::get_payee_name(int64 payee_id)
{
    const Data* payee_n = instance().get_data_n(payee_id);
    if (payee_n)
        return payee_n->m_name;
    else
        return _t("Payee Error");
}

bool PayeeModel::purge_id(int64 id)
{
    if (is_used(id))
        return false;

    // FIXME: remove AttachmentData owned by id

    return unsafe_remove_data(id);
}

const wxArrayString PayeeModel::all_payee_names()
{
    wxArrayString payees;
    for (const auto& payee_d: this->find_all(Col::COL_ID_PAYEENAME)) {
        payees.Add(payee_d.m_name);
    }
    return payees;
}

const std::map<wxString, int64> PayeeModel::all_payees(bool excludeHidden)
{
    std::map<wxString, int64> payees;
    for (const auto& payee_d : this->find_all()) {
        if (!excludeHidden || payee_d.m_active)
            payees[payee_d.m_name] = payee_d.m_id;
    }
    return payees;
}

const std::map<wxString, int64> PayeeModel::used_payee()
{
    std::map<int64, wxString> cache;
    for (const auto& payee_d : find_all())
        cache[payee_d.m_id] = payee_d.m_name;

    std::map<wxString, int64> payees;
    for (const auto& trx_d : TrxModel::instance().find_all()) {
        if (cache.count(trx_d.PAYEEID) > 0)
            payees[cache[trx_d.PAYEEID]] = trx_d.PAYEEID;
    }
    for (const auto& sched_d : SchedModel::instance().find_all()) {
        if (cache.count(sched_d.PAYEEID) > 0)
            payees[cache[sched_d.PAYEEID]] = sched_d.PAYEEID;
    }
    return payees;
}

// -- Check if Payee if being used

bool PayeeModel::is_used(int64 id)
{
    const auto& trans = TrxModel::instance().find(
        TrxCol::PAYEEID(id)
    );
    // FIXME: do not exclude deleted transactions; the payee is still used.
    // deleted transactions are shown in a panel and must have a valid payee id.
    for (const auto& txn : trans)
        if (txn.DELETEDTIME.IsEmpty())
            return true;

    const auto& sched_a = SchedModel::instance().find(
        SchedCol::PAYEEID(id)
    );
    if (!sched_a.empty())
        return true;

    return false;
}

