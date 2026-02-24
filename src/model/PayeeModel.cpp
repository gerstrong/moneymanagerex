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
#include "TransactionModel.h" // detect whether the payee is used or not
#include "ScheduledModel.h"

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
    for (auto &payee : this->find_all(PayeeCol::COL_ID_PAYEENAME))
    {
        if (payee.PAYEENAME.Lower().Matches(payee_pattern.Lower().Append("*")) &&
            (includeInActive || payee.ACTIVE == 1)) {
            payees.push_back(payee);
            }
    }
    return payees;
}

const PayeeData* PayeeModel::get_key(const wxString& name)
{
    const Data* payee_n = search_cache_n(PayeeCol::PAYEENAME(name));
    if (payee_n)
        return payee_n;

    DataA items = this->find(PayeeCol::PAYEENAME(name));
    if (!items.empty())
        payee_n = get_data_n(items[0].PAYEEID);
    return payee_n;
}

wxString PayeeModel::get_payee_name(int64 payee_id)
{
    const Data* payee_n = instance().get_data_n(payee_id);
    if (payee_n)
        return payee_n->PAYEENAME;
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
    for (const auto &payee: this->find_all(Col::COL_ID_PAYEENAME)) {
        payees.Add(payee.PAYEENAME);
    }
    return payees;
}

const std::map<wxString, int64> PayeeModel::all_payees(bool excludeHidden)
{
    std::map<wxString, int64> payees;
    for (const auto& payee : this->find_all())
    {
        if (!excludeHidden || (payee.ACTIVE == 1))
            payees[payee.PAYEENAME] = payee.PAYEEID;
    }
    return payees;
}

const std::map<wxString, int64> PayeeModel::used_payee()
{
    std::map<int64, wxString> cache;
    for (const auto& p : find_all())
        cache[p.PAYEEID] = p.PAYEENAME;

    std::map<wxString, int64> payees;
    for (const auto &t : TransactionModel::instance().find_all())
    {
        if (cache.count(t.PAYEEID) > 0)
            payees[cache[t.PAYEEID]] = t.PAYEEID;
    }
    for (const auto& b : ScheduledModel::instance().find_all())
    {
        if (cache.count(b.PAYEEID) > 0)
            payees[cache[b.PAYEEID]] = b.PAYEEID;
    }
    return payees;
}

// -- Check if Payee should be made available for use

bool PayeeModel::is_hidden(const Data& this_d)
{
    return this_d.ACTIVE == 0;
}

// -- Check if Payee if being used

bool PayeeModel::is_used(int64 id)
{
    const auto& trans = TransactionModel::instance().find(
        TransactionCol::PAYEEID(id)
    );
    // FIXME: do not exclude deleted transactions; the payee is still used.
    // deleted transactions are shown in a panel and must have a valid payee id.
    for (const auto& txn : trans)
        if (txn.DELETEDTIME.IsEmpty())
            return true;

    const auto& bills = ScheduledModel::instance().find(
        ScheduledCol::PAYEEID(id)
    );
    if (!bills.empty())
        return true;

    return false;
}

