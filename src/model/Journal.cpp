/*******************************************************
 Copyright (C) 2024 George Ef (george.a.ef@gmail.com)

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

#include "Journal.h"

TransactionData Journal::execute_bill(const ScheduledData& r, wxString date)
{
    TransactionData t;
    t.TRANSID           = 0;
    t.ACCOUNTID         = r.ACCOUNTID;
    t.TOACCOUNTID       = r.TOACCOUNTID;
    t.PAYEEID           = r.PAYEEID;
    t.TRANSCODE         = r.TRANSCODE;
    t.TRANSAMOUNT       = r.TRANSAMOUNT;
    t.STATUS            = r.STATUS;
    t.TRANSACTIONNUMBER = r.TRANSACTIONNUMBER;
    t.NOTES             = r.NOTES;
    t.CATEGID           = r.CATEGID;
    t.TRANSDATE         = date;
    t.FOLLOWUPID        = r.FOLLOWUPID;
    t.TOTRANSAMOUNT     = r.TOTRANSAMOUNT;
    t.COLOR             = r.COLOR;
    return t;
}

TransactionModel::Full_Data Journal::execute_bill_full(const ScheduledData& r, wxString date)
{
    TransactionModel::Full_Data t;
    t.TRANSID           = 0;
    t.ACCOUNTID         = r.ACCOUNTID;
    t.TOACCOUNTID       = r.TOACCOUNTID;
    t.PAYEEID           = r.PAYEEID;
    t.TRANSCODE         = r.TRANSCODE;
    t.TRANSAMOUNT       = r.TRANSAMOUNT;
    t.STATUS            = r.STATUS;
    t.TRANSACTIONNUMBER = r.TRANSACTIONNUMBER;
    t.NOTES             = r.NOTES;
    t.CATEGID           = r.CATEGID;
    t.TRANSDATE         = date;
    t.FOLLOWUPID        = r.FOLLOWUPID;
    t.TOTRANSAMOUNT     = r.TOTRANSAMOUNT;
    t.COLOR             = r.COLOR;
    return t;
}

TransactionSplitModel::DataA Journal::execute_splits(const Budgetsplit_DataA& rs)
{
    TransactionSplitModel::DataA ts;
    for (auto &rs1 : rs)
    {
        TransactionSplitData ts1;
        ts1.SPLITTRANSID     = rs1.SPLITTRANSID;
        ts1.TRANSID          = 0;
        ts1.CATEGID          = rs1.CATEGID;
        ts1.SPLITTRANSAMOUNT = rs1.SPLITTRANSAMOUNT;
        ts1.NOTES            = rs1.NOTES;
        ts.push_back(ts1);
    }
    return ts;
}

Journal::Data::Data()
    : TransactionData(), m_bdid(0), m_repeat_num(0)
{
}

Journal::Data::Data(const TransactionData& t)
    : TransactionData(t), m_bdid(0), m_repeat_num(0)
{
}

Journal::Data::Data(const ScheduledData& r)
    : Data(r, r.TRANSDATE, 1)
{
}

Journal::Data::Data(const ScheduledData& r, wxString date, int repeat_num)
    : TransactionData(execute_bill(r, date)), m_bdid(r.BDID), m_repeat_num(repeat_num)
{
    if (m_repeat_num < 1) {
        wxFAIL;
    }
}

Journal::Data::~Data()
{
}

Journal::Full_Data::Full_Data(const TransactionData& t)
    : TransactionModel::Full_Data(t), m_bdid(0), m_repeat_num(0)
{
}

Journal::Full_Data::Full_Data(const TransactionData& t,
    const std::map<int64 /* TRANSID */, Split_DataA>& splits,
    const std::map<int64 /* TRANSID */, Taglink_DataA>& tags)
:
    TransactionModel::Full_Data(t, splits, tags), m_bdid(0), m_repeat_num(0)
{
}

Journal::Full_Data::Full_Data(const ScheduledData& r)
    : Full_Data(r, r.TRANSDATE, 1)
{
}

Journal::Full_Data::Full_Data(const ScheduledData& r,
    wxString date, int repeat_num)
:
    TransactionModel::Full_Data(execute_bill_full(r, date), {}, {}),
    m_bdid(r.BDID), m_repeat_num(repeat_num)
{
    if (m_repeat_num < 1) {
        wxFAIL;
    }

    m_splits = execute_splits(ScheduledModel::split(r));

    m_tags = ScheduledModel::taglink(r);

    TransactionModel::Full_Data::fill_data();
    displayID = wxString("");
}

Journal::Full_Data::Full_Data(const ScheduledData& r,
    wxString date, int repeat_num,
    const std::map<int64 /* BDID */, Budgetsplit_DataA>& budgetsplits,
    const std::map<int64 /* BDID */, Taglink_DataA>& tags)
:
    TransactionModel::Full_Data(execute_bill_full(r, date), {}, {}),
    m_bdid(r.BDID), m_repeat_num(repeat_num)
{
    if (m_repeat_num < 1) {
        wxFAIL;
    }

    const auto budgetsplits_it = budgetsplits.find(m_bdid);
    if (budgetsplits_it != budgetsplits.end()) {
        m_splits = execute_splits(budgetsplits_it->second);
    }

    const auto tag_it = tags.find(m_bdid);
    if (tag_it != tags.end()) m_tags = tag_it->second;

    TransactionModel::Full_Data::fill_data();
    displayID = wxString("");
}

Journal::Full_Data::~Full_Data()
{
}


void Journal::setEmptyData(Journal::Data &data, int64 accountID)
{
    TransactionModel::setEmptyData(data, accountID);
    data.m_bdid = 0;
    data.m_repeat_num = 0;
}

bool Journal::setJournalData(Journal::Data &data, Journal::IdB journal_id)
{
    if (!journal_id.second) {
        const TransactionData *trx_n = TransactionModel::instance().get_data_n(journal_id.first);
        if (!trx_n)
            return false;
        data.m_repeat_num = 0;
        data.m_bdid = 0;
        data.TRANSID           = trx_n->TRANSID;
        data.ACCOUNTID         = trx_n->ACCOUNTID;
        data.TOACCOUNTID       = trx_n->TOACCOUNTID;
        data.PAYEEID           = trx_n->PAYEEID;
        data.TRANSCODE         = trx_n->TRANSCODE;
        data.TRANSAMOUNT       = trx_n->TRANSAMOUNT;
        data.STATUS            = trx_n->STATUS;
        data.TRANSACTIONNUMBER = trx_n->TRANSACTIONNUMBER;
        data.NOTES             = trx_n->NOTES;
        data.CATEGID           = trx_n->CATEGID;
        data.TRANSDATE         = trx_n->TRANSDATE;
        data.LASTUPDATEDTIME   = trx_n->LASTUPDATEDTIME;
        data.DELETEDTIME       = trx_n->DELETEDTIME;
        data.FOLLOWUPID        = trx_n->FOLLOWUPID;
        data.TOTRANSAMOUNT     = trx_n->TOTRANSAMOUNT;
        data.COLOR             = trx_n->COLOR;
    }
    else {
        const ScheduledData *sched_n = ScheduledModel::instance().get_data_n(journal_id.first);
        if (!sched_n)
            return false;
        data.m_repeat_num = 1;
        data.TRANSID = 0;
        data.m_bdid            = sched_n->BDID;
        data.ACCOUNTID         = sched_n->ACCOUNTID;
        data.TOACCOUNTID       = sched_n->TOACCOUNTID;
        data.PAYEEID           = sched_n->PAYEEID;
        data.TRANSCODE         = sched_n->TRANSCODE;
        data.TRANSAMOUNT       = sched_n->TRANSAMOUNT;
        data.STATUS            = sched_n->STATUS;
        data.TRANSACTIONNUMBER = sched_n->TRANSACTIONNUMBER;
        data.NOTES             = sched_n->NOTES;
        data.CATEGID           = sched_n->CATEGID;
        data.TRANSDATE         = sched_n->TRANSDATE;
        data.LASTUPDATEDTIME   = "";
        data.DELETEDTIME       = "";
        data.FOLLOWUPID        = sched_n->FOLLOWUPID;
        data.TOTRANSAMOUNT     = sched_n->TOTRANSAMOUNT;
        data.COLOR             = sched_n->COLOR;
    }
    return true;
}

const TransactionSplitModel::DataA Journal::split(Journal::Data &r)
{
    return (r.m_repeat_num == 0)
        ? TransactionSplitModel::instance().find(
            TransactionSplitCol::TRANSID(r.TRANSID)
        ) : Journal::execute_splits(ScheduledSplitModel::instance().find(
            ScheduledSplitCol::TRANSID(r.m_bdid)
        ));
}
