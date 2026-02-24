/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)
 Copyright (C) 2021,2022 Mark Whalley (mark@ipx.co.uk)

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

#include "AccountModel.h"
#include "AttachmentModel.h"
#include "CategoryModel.h"
#include "FieldValueModel.h"
#include "PayeeModel.h"
#include "PreferencesModel.h"
#include "ScheduledModel.h"
#include "TagModel.h"

 // TODO: Move attachment management outside of AttachmentDialog
#include "dialog/AttachmentDialog.h"

// -- static methods --

// Initialize the global ScheduledModel table.
// Reset the ScheduledModel table or create the table if it does not exist.
ScheduledModel& ScheduledModel::instance(wxSQLite3Database* db)
{
    ScheduledModel& ins = Singleton<ScheduledModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();

    return ins;
}

// Return the static instance of ScheduledModel table
ScheduledModel& ScheduledModel::instance()
{
    return Singleton<ScheduledModel>::instance();
}

TransactionModel::TYPE_ID ScheduledModel::type_id(const Data& this_d)
{
    return static_cast<TransactionModel::TYPE_ID>(
        TransactionModel::type_id(this_d.TRANSCODE)
    );
}

TransactionModel::STATUS_ID ScheduledModel::status_id(const Data& this_d)
{
    return static_cast<TransactionModel::STATUS_ID>(
        TransactionModel::status_id(this_d.STATUS)
    );
}

wxDate ScheduledModel::getTransDateTime(const Data& this_d)
{
    return parseDateTime(this_d.TRANSDATE);
}

wxDate ScheduledModel::NEXTOCCURRENCEDATE(const Data& this_d)
{
    return parseDateTime(this_d.NEXTOCCURRENCEDATE);
}

bool ScheduledModel::encode_repeat_num(Data& this_d, const RepeatNum& rn)
{
    if (rn.freq == REPEAT_FREQ_INVALID)
        return false;

    this_d.REPEATS = rn.exec * BD_REPEATS_MULTIPLEX_BASE + rn.freq;
    this_d.NUMOCCURRENCES = (
        rn.freq >= ScheduledModel::REPEAT_FREQ_IN_X_DAYS &&
        rn.freq <= ScheduledModel::REPEAT_FREQ_EVERY_X_MONTHS
    ) ? rn.x : rn.num;

    return true;
}

bool ScheduledModel::decode_repeat_num(const Data& this_d, RepeatNum& rn)
{
    rn.exec = static_cast<REPEAT_EXEC>(this_d.REPEATS.GetValue() / BD_REPEATS_MULTIPLEX_BASE);
    rn.freq = static_cast<REPEAT_FREQ>(this_d.REPEATS.GetValue() % BD_REPEATS_MULTIPLEX_BASE);
    rn.num  = static_cast<int>(this_d.NUMOCCURRENCES.GetValue());
    rn.x    = REPEAT_X_VOID;

    if (rn.freq == REPEAT_FREQ_ONCE) {
        rn.num = 1;
    }
    else if (rn.freq >= REPEAT_FREQ_IN_X_DAYS && rn.freq <= REPEAT_FREQ_IN_X_MONTHS) {
        rn.x = rn.num > 0 ? rn.num : REPEAT_X_INVALID;
        rn.num = 2;
    }
    else if (rn.freq >= REPEAT_FREQ_EVERY_X_DAYS && rn.freq <= REPEAT_FREQ_EVERY_X_MONTHS) {
        rn.x = rn.num > 0 ? rn.num : REPEAT_X_INVALID;
        rn.num = REPEAT_NUM_INFINITY;
    }
    else if (rn.num < 1 && rn.num != REPEAT_NUM_INFINITY) {
        rn.num = REPEAT_NUM_INVALID;
    }

    return (rn.freq >= 0 && rn.freq < REPEAT_FREQ_size &&
        rn.num != REPEAT_NUM_INVALID && rn.x != REPEAT_X_INVALID
    );
}

bool ScheduledModel::next_repeat_num(RepeatNum& rn)
{
    if (rn.freq == REPEAT_FREQ_INVALID)
        return false;

    if (rn.num > 0)
        --rn.num;

    if (rn.num == 0)
        return false;

    // change REPEAT_FREQ_IN_X_* to REPEAT_FREQ_ONCE
    if (rn.x != REPEAT_X_VOID && rn.num == 1)
        rn.freq = REPEAT_FREQ_ONCE;

    return true;
}

bool ScheduledModel::requires_execution(const Data& this_d)
{
    return (
        ScheduledModel::NEXTOCCURRENCEDATE(this_d)
            .Subtract(wxDate::Today()).GetSeconds().GetValue()
        / 86400 < 1
    );
}

const wxDateTime ScheduledModel::nextOccurDate(
    wxDateTime this_date,
    const RepeatNum& rn,
    bool reverse
) {
    int k = reverse ? -1 : 1;

    wxDateTime next_date = this_date;
    if (rn.freq == REPEAT_FREQ_WEEKLY)
        next_date.Add(wxTimeSpan::Weeks(k));
    else if (rn.freq == REPEAT_FREQ_BI_WEEKLY)
        next_date.Add(wxTimeSpan::Weeks(2 * k));
    else if (rn.freq == REPEAT_FREQ_MONTHLY)
        next_date.Add(wxDateSpan::Months(k));
    else if (rn.freq == REPEAT_FREQ_BI_MONTHLY)
        next_date.Add(wxDateSpan::Months(2 * k));
    else if (rn.freq == REPEAT_FREQ_FOUR_MONTHLY)
        next_date.Add(wxDateSpan::Months(4 * k));
    else if (rn.freq == REPEAT_FREQ_HALF_YEARLY)
        next_date.Add(wxDateSpan::Months(6 * k));
    else if (rn.freq == REPEAT_FREQ_YEARLY)
        next_date.Add(wxDateSpan::Years(k));
    else if (rn.freq == REPEAT_FREQ_QUARTERLY)
        next_date.Add(wxDateSpan::Months(3 * k));
    else if (rn.freq == REPEAT_FREQ_FOUR_WEEKLY)
        next_date.Add(wxDateSpan::Weeks(4 * k));
    else if (rn.freq == REPEAT_FREQ_DAILY)
        next_date.Add(wxDateSpan::Days(k));
    else if (rn.freq == REPEAT_FREQ_IN_X_DAYS)
        next_date.Add(wxDateSpan::Days(rn.x * k));
    else if (rn.freq == REPEAT_FREQ_IN_X_MONTHS)
        next_date.Add(wxDateSpan::Months(rn.x * k));
    else if (rn.freq == REPEAT_FREQ_EVERY_X_DAYS)
        next_date.Add(wxDateSpan::Days(rn.x * k));
    else if (rn.freq == REPEAT_FREQ_EVERY_X_MONTHS)
        next_date.Add(wxDateSpan::Months(rn.x * k));
    else if (rn.freq == REPEAT_FREQ_MONTHLY_LAST_DAY ||
             rn.freq == REPEAT_FREQ_MONTHLY_LAST_BUSINESS_DAY)
    {
        next_date.Add(wxDateSpan::Months(k));
        next_date.SetToLastMonthDay(next_date.GetMonth(), next_date.GetYear());
        if (rn.freq == REPEAT_FREQ_MONTHLY_LAST_BUSINESS_DAY) {
            // last weekday of month
            if (next_date.GetWeekDay() == wxDateTime::Sun ||
                next_date.GetWeekDay() == wxDateTime::Sat
            )
                next_date.SetToPrevWeekDay(wxDateTime::Fri);
        }
    }
    wxLogDebug("init date: %s -> next date: %s",
        this_date.FormatISOCombined(), next_date.FormatISOCombined()
    );
    return next_date;
}

wxArrayString ScheduledModel::unroll(const Data& sched_d, const wxString end_date, int limit)
{
    wxArrayString dates;

    RepeatNum rn;
    if (!decode_repeat_num(sched_d, rn))
        return dates;

    wxString date = sched_d.TRANSDATE;
    while (date <= end_date && limit != 0) {
        if (limit > 0)
            --limit;
        dates.push_back(date);

        if (rn.num == 1)
            break;

        wxDateTime date_curr;
        date_curr.ParseDateTime(date) || date_curr.ParseDate(date);
        const wxDateTime& date_next = nextOccurDate(date_curr, rn);
        date = date_next.FormatISOCombined();

        next_repeat_num(rn);
    }

    return dates;
}

ScheduledCol::STATUS ScheduledModel::STATUS(OP op, TransactionModel::STATUS_ID status)
{
    return ScheduledCol::STATUS(op, TransactionModel::status_key(status));
}

ScheduledCol::TRANSCODE ScheduledModel::TRANSCODE(OP op, TransactionModel::TYPE_ID type)
{
    return ScheduledCol::TRANSCODE(op, TransactionModel::type_name(type));
}

const ScheduledSplitModel::DataA ScheduledModel::split(const Data& sched_d)
{
    return ScheduledSplitModel::instance().find(
        ScheduledSplitCol::TRANSID(sched_d.BDID)
    );
}

const TagLinkModel::DataA ScheduledModel::taglink(const Data& sched_d)
{
    return TagLinkModel::instance().find(
        TagLinkCol::REFTYPE(ScheduledModel::refTypeName),
        TagLinkCol::REFID(sched_d.BDID)
    );
}

// -- constructor --

ScheduledModel::ScheduledModel() :
    TableFactory<ScheduledTable, ScheduledData>()
{
}

ScheduledModel::~ScheduledModel()
{
}

// -- instance methods --

// Remove the Data record instance from memory and the database
// including any splits associated with the Data Record.
bool ScheduledModel::remove_depen(int64 id)
{
    for (auto &item : ScheduledModel::split(*get_data_n(id)))
        ScheduledSplitModel::instance().remove_depen(item.SPLITTRANSID);
    // Delete tags for the scheduled transaction
    TagLinkModel::instance().DeleteAllTags(this->refTypeName, id);
    return remove_data(id);
}

bool ScheduledModel::AllowTransaction(const Data& r)
{
    if (r.STATUS == TransactionModel::STATUS_KEY_VOID)
        return true;
    if (r.TRANSCODE != TransactionModel::TYPE_NAME_WITHDRAWAL && r.TRANSCODE != TransactionModel::TYPE_NAME_TRANSFER)
        return true;

    const int64 acct_id = r.ACCOUNTID;
    const AccountData* account = AccountModel::instance().get_data_n(acct_id);

    if (account->MINIMUMBALANCE == 0 && account->CREDITLIMIT == 0)
        return true;

    double current_balance = AccountModel::balance(account);
    double new_balance = current_balance - r.TRANSAMOUNT;

    bool allow_transaction = true;
    wxString limitDescription;
    double limitAmount{ 0.0L };
    if (account->MINIMUMBALANCE != 0 && new_balance < account->MINIMUMBALANCE)
    {
        allow_transaction = false;
        limitDescription = _t("Minimum Balance");
        limitAmount = account->MINIMUMBALANCE;
    }
    else if (account->CREDITLIMIT != 0 && new_balance < -(account->CREDITLIMIT))
    {
        allow_transaction = false;
        limitDescription = _t("Credit Limit");
        limitAmount = account->CREDITLIMIT;
    }

    if (!allow_transaction)
    {
        wxString message = _t("A scheduled transaction will exceed the account limit.\n\n"
            "Account: %1$s\n"
            "Current Balance: %2$6.2f\n"
            "Transaction amount: %3$6.2f\n"
            "%4$s: %5$6.2f") + "\n\n" +
            _t("Do you want to continue?");
        message.Printf(message, account->ACCOUNTNAME, current_balance, r.TRANSAMOUNT, limitDescription, limitAmount);

        if (wxMessageBox(message, _t("MMEX Scheduled Transaction Check"), wxYES_NO | wxICON_WARNING) == wxYES)
            allow_transaction = true;
    }

    return allow_transaction;
}

void ScheduledModel::completeBDInSeries(int64 bdID)
{
    Data* sched_n = unsafe_get_data_n(bdID);
    if (!sched_n)
        return;

    RepeatNum rn;
    if (!decode_repeat_num(*sched_n, rn) || rn.num == 1) {
        mmAttachmentManage::DeleteAllAttachments(this->refTypeName, bdID);
        remove_depen(bdID);
        return;
    }

    wxDateTime transdate;
    transdate.ParseDateTime(sched_n->TRANSDATE) || transdate.ParseDate(sched_n->TRANSDATE);
    const wxDateTime& payment_date_current = transdate;
    const wxDateTime& payment_date_update = nextOccurDate(payment_date_current, rn);
    sched_n->TRANSDATE = payment_date_update.FormatISOCombined();

    const wxDateTime& due_date_current = NEXTOCCURRENCEDATE(*sched_n);
    const wxDateTime& due_date_update = nextOccurDate(due_date_current, rn);
    sched_n->NEXTOCCURRENCEDATE = due_date_update.FormatISODate();

    next_repeat_num(rn);
    encode_repeat_num(*sched_n, rn);

    unsafe_save_data_n(sched_n);
}

// -- Full_Data --

ScheduledModel::Full_Data::Full_Data()
{}

ScheduledModel::Full_Data::Full_Data(const Data& r) :
    Data(r),
    m_bill_splits(split(r)),
    m_tags(TagLinkModel::instance().find(
        TagLinkCol::REFTYPE(ScheduledModel::refTypeName),
        TagLinkCol::REFID(r.BDID)))
{
    if (!m_tags.empty()) {
        wxArrayString tagnames;
        for (const auto& entry : m_tags)
            tagnames.Add(TagModel::instance().get_data_n(entry.TAGID)->TAGNAME);
        // Sort TAGNAMES
        tagnames.Sort();
        for (const auto& name : tagnames)
            this->TAGNAMES += (this->TAGNAMES.empty() ? "" : " ") + name;
    }

    if (!m_bill_splits.empty())
    {
        for (const auto& entry : m_bill_splits)
        {
            CATEGNAME += (CATEGNAME.empty() ? " + " : ", ")
                + CategoryModel::full_name(entry.CATEGID);

            wxString splitTags;
            for (const auto& tag : TagLinkModel::instance().get_ref(ScheduledSplitModel::refTypeName, entry.SPLITTRANSID))
                splitTags.Append(tag.first + " ");
            if (!splitTags.IsEmpty())
                TAGNAMES.Append((TAGNAMES.IsEmpty() ? "" : ", ") + splitTags.Trim());
        }
    }
    else
        CATEGNAME = CategoryModel::full_name(r.CATEGID);

    ACCOUNTNAME = AccountModel::get_id_name(r.ACCOUNTID);

    PAYEENAME = PayeeModel::get_payee_name(r.PAYEEID);
    if (ScheduledModel::type_id(r) == TransactionModel::TYPE_ID_TRANSFER)
    {
        PAYEENAME = AccountModel::get_id_name(r.TOACCOUNTID);
    }

}

wxString ScheduledModel::Full_Data::real_payee_name() const
{
    return (TransactionModel::type_id(this->TRANSCODE) == TransactionModel::TYPE_ID_TRANSFER)
        ? ("> " + this->PAYEENAME)
        : this->PAYEENAME;
}
