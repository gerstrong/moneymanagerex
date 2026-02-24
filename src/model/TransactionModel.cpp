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

#include <queue>

#include "util/_util.h"

#include "AccountModel.h"
#include "CategoryModel.h"
#include "FieldValueModel.h"
#include "PayeeModel.h"
#include "PreferencesModel.h"
#include "TagModel.h"
#include "TransactionLinkModel.h"
#include "TransactionModel.h"

#include "dialog/AttachmentDialog.h"

mmChoiceNameA TransactionModel::TYPE_CHOICES = mmChoiceNameA({
    { TYPE_ID_WITHDRAWAL, _n("Withdrawal") },
    { TYPE_ID_DEPOSIT,    _n("Deposit") },
    { TYPE_ID_TRANSFER,   _n("Transfer") }
});

mmChoiceNameA TransactionModel::TRADE_TYPE_CHOICES = mmChoiceNameA({
    { TYPE_ID_WITHDRAWAL,  _n("Buy") },
    { TYPE_ID_DEPOSIT, _n("Sell") },
    { TYPE_ID_TRANSFER, _n("Revalue") }
});

mmChoiceKeyNameA TransactionModel::STATUS_CHOICES = mmChoiceKeyNameA({
    { STATUS_ID_NONE,       "",  _n("Unreconciled") },
    { STATUS_ID_RECONCILED, "R", _n("Reconciled") },
    { STATUS_ID_VOID,       "V", _n("Void") },
    { STATUS_ID_FOLLOWUP,   "F", _n("Follow Up") },
    { STATUS_ID_DUPLICATE,  "D", _n("Duplicate") }
});

const wxString TransactionModel::TYPE_NAME_WITHDRAWAL = type_name(TYPE_ID_WITHDRAWAL);
const wxString TransactionModel::TYPE_NAME_DEPOSIT    = type_name(TYPE_ID_DEPOSIT);
const wxString TransactionModel::TYPE_NAME_TRANSFER   = type_name(TYPE_ID_TRANSFER);

const wxString TransactionModel::STATUS_KEY_NONE       = status_key(STATUS_ID_NONE);
const wxString TransactionModel::STATUS_KEY_RECONCILED = status_key(STATUS_ID_RECONCILED);
const wxString TransactionModel::STATUS_KEY_VOID       = status_key(STATUS_ID_VOID);
const wxString TransactionModel::STATUS_KEY_FOLLOWUP   = status_key(STATUS_ID_FOLLOWUP);
const wxString TransactionModel::STATUS_KEY_DUPLICATE  = status_key(STATUS_ID_DUPLICATE);

const wxString TransactionModel::STATUS_NAME_NONE       = status_name(STATUS_ID_NONE);
const wxString TransactionModel::STATUS_NAME_RECONCILED = status_name(STATUS_ID_RECONCILED);
const wxString TransactionModel::STATUS_NAME_VOID       = status_name(STATUS_ID_VOID);
const wxString TransactionModel::STATUS_NAME_FOLLOWUP   = status_name(STATUS_ID_FOLLOWUP);
const wxString TransactionModel::STATUS_NAME_DUPLICATE  = status_name(STATUS_ID_DUPLICATE);

TransactionModel::TransactionModel() :
    TableFactory<TransactionTable, TransactionData>()
{
}

TransactionModel::~TransactionModel()
{
}

// Initialize the global TransactionModel table.
// Reset the TransactionModel table or create the table if it does not exist.
TransactionModel& TransactionModel::instance(wxSQLite3Database* db)
{
    TransactionModel& ins = Singleton<TransactionModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();

    return ins;
}

// Return the static instance of TransactionModel table
TransactionModel& TransactionModel::instance()
{
    return Singleton<TransactionModel>::instance();
}

void TransactionModel::copy_from_trx(Data *this_n, const Data& other_d)
{
    this_n->STATUS            = other_d.STATUS;
    this_n->TRANSCODE         = other_d.TRANSCODE;
    this_n->TRANSDATE         = other_d.TRANSDATE;
    this_n->PAYEEID           = other_d.PAYEEID;
    this_n->ACCOUNTID         = other_d.ACCOUNTID;
    this_n->TRANSAMOUNT       = other_d.TRANSAMOUNT;
    this_n->CATEGID           = other_d.CATEGID;
    this_n->TOACCOUNTID       = other_d.TOACCOUNTID;
    this_n->TOTRANSAMOUNT     = other_d.TOTRANSAMOUNT;
    this_n->NOTES             = other_d.NOTES;
    this_n->TRANSACTIONNUMBER = other_d.TRANSACTIONNUMBER;
    this_n->FOLLOWUPID        = other_d.FOLLOWUPID;
    this_n->COLOR             = other_d.COLOR;
}

bool TransactionModel::is_transfer(const wxString& r)
{
    return type_id(r) == TransactionModel::TYPE_ID_TRANSFER;
}
bool TransactionModel::is_transfer(const Data& this_d)
{
    return type_id(this_d.TRANSCODE) == TransactionModel::TYPE_ID_TRANSFER;
}

bool TransactionModel::is_deposit(const wxString& r)
{
    return type_id(r) == TransactionModel::TYPE_ID_DEPOSIT;
}
bool TransactionModel::is_deposit(const Data& this_d)
{
    return type_id(this_d.TRANSCODE) == TransactionModel::TYPE_ID_DEPOSIT;
}

wxDateTime TransactionModel::getTransDateTime(const Data& this_d)
{
    return parseDateTime(this_d.TRANSDATE);
}

double TransactionModel::account_flow(const Data& this_d, int64 account_id)
{
    if (this_d.ACCOUNTID == this_d.TOACCOUNTID && type_id(this_d.TRANSCODE) == TYPE_ID_TRANSFER)
        return 0.0;  // Self Transfer as Revaluation
    if (TransactionModel::status_id(this_d.STATUS) == TransactionModel::STATUS_ID_VOID || !this_d.DELETEDTIME.IsEmpty())
        return 0.0;
    if (account_id == this_d.ACCOUNTID && type_id(this_d.TRANSCODE) == TYPE_ID_WITHDRAWAL)
        return -(this_d.TRANSAMOUNT);
    if (account_id == this_d.ACCOUNTID && type_id(this_d.TRANSCODE) == TYPE_ID_DEPOSIT)
        return this_d.TRANSAMOUNT;
    if (account_id == this_d.ACCOUNTID && type_id(this_d.TRANSCODE) == TYPE_ID_TRANSFER)
        return -(this_d.TRANSAMOUNT);
    if (account_id == this_d.TOACCOUNTID && type_id(this_d.TRANSCODE) == TYPE_ID_TRANSFER)
        return this_d.TOTRANSAMOUNT;
    return 0.0;
}

double TransactionModel::account_outflow(const Data& this_d, int64 account_id)
{
    double bal = account_flow(this_d, account_id);
    return bal <= 0 ? -bal : 0;
}

double TransactionModel::account_inflow(const Data& this_d, int64 account_id)
{
    double bal = account_flow(this_d, account_id);
    return bal >= 0 ? bal : 0;
}

double TransactionModel::account_recflow(const Data& this_d, int64 account_id)
{
    return (TransactionModel::status_id(this_d.STATUS) == TransactionModel::STATUS_ID_RECONCILED)
        ? account_flow(this_d, account_id)
        : 0;
}

// same as TransactionModel::Full_Data::is_foreign()
bool TransactionModel::is_foreign(const Data& this_d)
{
    return (this_d.TOACCOUNTID > 0) && (
        this_d.TRANSCODE == TYPE_NAME_DEPOSIT || this_d.TRANSCODE == TYPE_NAME_WITHDRAWAL
    );
}

// see also TransactionModel::Full_Data::is_foreign_transfer()
bool TransactionModel::is_foreignAsTransfer(const Data& this_d)
{
    return is_foreign(this_d) && (
        this_d.TOACCOUNTID == TransactionLinkModel::AS_TRANSFER ||
        this_d.TOACCOUNTID == this_d.ACCOUNTID
    );
}

TransactionCol::TRANSDATE TransactionModel::TRANSDATE(OP op, const wxString& date_iso_str)
{
    return TransactionCol::TRANSDATE(op, date_iso_str);
}

TransactionCol::TRANSDATE TransactionModel::TRANSDATE(OP op, const mmDateDay& date)
{
    // OP_EQ and OP_NE should not be used for date comparisons.
    // if needed, create an equivalent AND/OR combination of two other operators.
    wxString bound =
        (op == OP_GE || op == OP_LT) ? date.isoStart()
        : (op == OP_LE || op == OP_GT) ? date.isoEnd()
        : date.isoDate();
    return TransactionCol::TRANSDATE(op, bound);
}

TransactionCol::TRANSDATE TransactionModel::TRANSDATE(OP op, const wxDateTime& date)
{
    // the boundary has granularity of a day
    return TransactionModel::TRANSDATE(op, mmDateDay(date));
}

TransactionCol::DELETEDTIME TransactionModel::DELETEDTIME(OP op, const wxString& date)
{
    return TransactionCol::DELETEDTIME(op, date);
}

TransactionCol::STATUS TransactionModel::STATUS(OP op, STATUS_ID status)
{
    return TransactionCol::STATUS(op, status_key(status));
}

TransactionCol::TRANSCODE TransactionModel::TRANSCODE(OP op, TYPE_ID type)
{
    return TransactionCol::TRANSCODE(op, type_name(type));
}

TransactionCol::TRANSACTIONNUMBER TransactionModel::TRANSACTIONNUMBER(OP op, const wxString& num)
{
    return TransactionCol::TRANSACTIONNUMBER(op, num);
}

const TransactionModel::DataA TransactionModel::find_allByDateTimeId()
{
    auto trx_a = TransactionModel::instance().find_all();
    // first sort by id, then stable sort by datetime or date only
    std::sort(trx_a.begin(), trx_a.end());
    if (PreferencesModel::instance().UseTransDateTime())
        std::stable_sort(trx_a.begin(), trx_a.end(), TransactionData::SorterByTRANSDATE());
    else
        std::stable_sort(trx_a.begin(), trx_a.end(), TransactionModel::SorterByTRANSDATE_DATE());
    return trx_a;
}

const TransactionSplitModel::DataA TransactionModel::find_split(const Data& trx_d)
{
    return TransactionSplitModel::instance().find(
        TransactionSplitCol::TRANSID(trx_d.TRANSID)
    );
}

bool CompareUsedNotes(const std::tuple<int, wxString, wxString>& a, const std::tuple<int, wxString, wxString>& b)
{
    if (std::get<0>(a) < std::get<0>(b)) return true;
    if (std::get<0>(b) < std::get<0>(a)) return false;

    // a=b for primary condition, go to secondary (but reverse order)
    if (std::get<1>(a) > std::get<1>(b)) return true;
    if (std::get<1>(b) > std::get<1>(a)) return false;

    return false;
}

void TransactionModel::getFrequentUsedNotes(std::vector<wxString> &frequentNotes, int64 accountID)
{
    frequentNotes.clear();
    size_t max = 20;

    const auto notes = instance().find(
        TransactionCol::NOTES(OP_NE, ""),
        accountID > 0 ? TransactionCol::ACCOUNTID(accountID) : TransactionCol::ACCOUNTID(OP_NE, -1)
    );

    // Count frequency
    std::map <wxString, std::pair<int, wxString> > counterMap;
    for (const auto& entry : notes) {
        auto& counter = counterMap[entry.NOTES];
        counter.first--;
        if (entry.TRANSDATE > counter.second)
            counter.second = entry.TRANSDATE;
    }

    // Convert to vector
    std::vector<std::tuple<int, wxString, wxString> > vec;
    for (const auto& [note, counter] : counterMap)
        vec.emplace_back(counter.first, counter.second, note);

    // Sort by frequency then date
    std::sort(vec.begin(), vec.end(), CompareUsedNotes);

    // Pull out top 20 (max)
    for (const auto& kv : vec)
    {
        if (0 == max--)
            break;
        frequentNotes.push_back(std::get<2>(kv));
    }
}

void TransactionModel::setEmptyData(Data &trx_d, int64 accountID)
{
    trx_d.TRANSID = -1;
    trx_d.PAYEEID = -1;
    const wxString today_date = wxDate::Now().FormatISOCombined();
    wxString max_trx_date;
    if (PreferencesModel::instance().getTransDateDefault() != PreferencesModel::NONE) {
        auto trx_a = instance().find_or(
            TransactionCol::ACCOUNTID(accountID),
            TransactionCol::TOACCOUNTID(accountID)
        );

        for (const auto& t_d: trx_a) {
            if (t_d.DELETEDTIME.IsNull() && max_trx_date < t_d.TRANSDATE && today_date >= t_d.TRANSDATE) {
                max_trx_date = t_d.TRANSDATE;
            }
        }
    }

    if (max_trx_date.empty()) {
        max_trx_date = today_date;
    }

    trx_d.TRANSDATE         = max_trx_date;
    trx_d.ACCOUNTID         = accountID;
    trx_d.STATUS            = status_key(PreferencesModel::instance().getTransStatusReconciled());
    trx_d.TRANSCODE         = TYPE_NAME_WITHDRAWAL;
    trx_d.CATEGID           = -1;
    trx_d.FOLLOWUPID        = -1;
    trx_d.TRANSAMOUNT       = 0;
    trx_d.TOTRANSAMOUNT     = 0;
    trx_d.TRANSACTIONNUMBER = "";
    trx_d.COLOR             = -1;
}

bool TransactionModel::is_locked(const Data& trx_d)
{
    bool val = false;
    const AccountData* account_n = AccountModel::instance().get_data_n(trx_d.ACCOUNTID);

    if (AccountModel::BoolOf(account_n->STATEMENTLOCKED)) {
        wxDateTime transaction_date;
        if (transaction_date.ParseDate(trx_d.TRANSDATE)) {
            if (transaction_date <= parseDateTime(account_n->STATEMENTDATE)) {
                val = true;
            }
        }
    }
    return val;
}

bool TransactionModel::purge_id(int64 id)
{
    // TODO: remove all split at once
    // TransactionSplitModel::instance().purge_id(TransactionSplitModel::instance().find(TransactionSplitCol::TRANSID(id)));
    for (const auto& ts_d : TransactionSplitModel::instance().find(
        TransactionSplitCol::TRANSID(id)
    )) {
        TransactionSplitModel::instance().purge_id(ts_d.SPLITTRANSID);
    }
    if (is_foreign(*instance().get_data_n(id)))
        TransactionLinkModel::RemoveTranslinkEntry(id);

    const wxString& RefType = TransactionModel::refTypeName;
    // remove all attachments
    mmAttachmentManage::DeleteAllAttachments(RefType, id);
    // remove all custom fields for the transaction
    FieldValueModel::DeleteAllData(RefType, id);
    TagLinkModel::instance().DeleteAllTags(RefType, id);
    return unsafe_remove_data(id);
}

void TransactionModel::save_timestamp(int64 id)
{
    Data* trx_n = instance().unsafe_get_data_n(id);
    if (trx_n && trx_n->TRANSID == id) {
        trx_n->LASTUPDATEDTIME = wxDateTime::Now().ToUTC().FormatISOCombined();
        unsafe_update_data_n(trx_n);
    }
}

void TransactionModel::update_timestamp(Data& trx_d)
{
    // get record from database bypassing the cache
    TransactionModel::DataA trx_a = TransactionModel::instance().find(
        TransactionCol::TRANSID(trx_d.TRANSID)
    );
    if (trx_a.size() == 0 || (!trx_a[0].equals(&trx_d)
        && trx_a[0].DELETEDTIME.IsEmpty() && trx_d.DELETEDTIME.IsEmpty()
    )) {
        trx_d.LASTUPDATEDTIME = wxDateTime::Now().ToUTC().FormatISOCombined();
    }
}

const TransactionData* TransactionModel::unsafe_save_trx(Data* trx_n)
{
    update_timestamp(*trx_n);
    return unsafe_save_data_n(trx_n);
}

const TransactionData* TransactionModel::save_trx(Data& trx_d)
{
    update_timestamp(trx_d);
    return save_data_n(trx_d);
}

bool TransactionModel::save_trx_a(DataA& trx_a)
{
    bool ok = true;

    Savepoint();
    for (auto& trx_d : trx_a) {
        if (trx_d.id() < 0)
            wxLogDebug("Incorrect function call to save %s", trx_d.to_json().utf8_str());
        if (!save_trx(trx_d)) {
            ok = false;
            break;
        }
    }
    ReleaseSavepoint();

    return ok;
}

TransactionModel::Full_Data::Full_Data() :
    Data(), TAGNAMES(""),
    ACCOUNTID_W(-1), ACCOUNTID_D(-1), TRANSAMOUNT_W(0), TRANSAMOUNT_D(0),
    SN(0), ACCOUNT_FLOW(0), ACCOUNT_BALANCE(0)
{
}

TransactionModel::Full_Data::Full_Data(const Data& r) :
    Data(r),
    m_splits(TransactionSplitModel::instance().find(
        TransactionSplitCol::TRANSID(r.TRANSID))),
    m_tags(TagLinkModel::instance().find(
        TagLinkCol::REFTYPE(TransactionModel::refTypeName),
        TagLinkCol::REFID(r.TRANSID))),
    ACCOUNTID_W(-1), ACCOUNTID_D(-1), TRANSAMOUNT_W(0), TRANSAMOUNT_D(0),
    SN(0), ACCOUNT_FLOW(0), ACCOUNT_BALANCE(0)
{
    fill_data();
}

TransactionModel::Full_Data::Full_Data(
    const Data& r,
    const std::map<int64 /* TRANSID */, TransactionSplitModel::DataA>& splits,
    const std::map<int64 /* TRANSID */, TagLinkModel::DataA>& tags
) :
    Data(r),
    ACCOUNTID_W(-1), ACCOUNTID_D(-1), TRANSAMOUNT_W(0), TRANSAMOUNT_D(0),
    SN(0), ACCOUNT_FLOW(0), ACCOUNT_BALANCE(0)
{
    if (const auto it = splits.find(this->id()); it != splits.end()) m_splits = it->second;

    if (const auto tag_it = tags.find(this->id()); tag_it != tags.end()) m_tags = tag_it->second;

    fill_data();
}

void TransactionModel::Full_Data::fill_data()
{
    displayID = wxString::Format("%lld", TRANSID);
    ACCOUNTNAME = AccountModel::get_id_name(ACCOUNTID);

    if (TransactionModel::type_id(TRANSCODE) == TransactionModel::TYPE_ID_TRANSFER) {
        TOACCOUNTNAME = AccountModel::get_id_name(TOACCOUNTID);
        PAYEENAME = TOACCOUNTNAME;
    }
    else {
        PAYEENAME = PayeeModel::get_payee_name(PAYEEID);
    }

    if (!m_splits.empty()) {
        for (const auto& entry : m_splits)
            CATEGNAME += (CATEGNAME.empty() ? " + " : ", ")
                + CategoryModel::full_name(entry.CATEGID);
    }
    else {
        CATEGNAME = CategoryModel::full_name(CATEGID);
    }

    if (!m_tags.empty()) {
        wxArrayString tagnames;
        for (const auto& entry : m_tags)
            tagnames.Add(TagModel::instance().get_data_n(entry.TAGID)->TAGNAME);
        // Sort TAGNAMES
        tagnames.Sort(CaseInsensitiveCmp);
        for (const auto& name : tagnames)
            TAGNAMES += (TAGNAMES.empty() ? "" : " ") + name;
    }

    if (type_id(TRANSCODE) == TYPE_ID_WITHDRAWAL) {
        ACCOUNTID_W = ACCOUNTID; TRANSAMOUNT_W = TRANSAMOUNT;
    }
    else if (type_id(TRANSCODE) == TYPE_ID_DEPOSIT) {
        ACCOUNTID_D = ACCOUNTID; TRANSAMOUNT_D = TRANSAMOUNT;
    }
    else if (type_id(TRANSCODE) == TYPE_ID_TRANSFER) {
        ACCOUNTID_W = ACCOUNTID; TRANSAMOUNT_W = TRANSAMOUNT;
        ACCOUNTID_D = TOACCOUNTID; TRANSAMOUNT_D = TOTRANSAMOUNT;
    }
}

TransactionModel::Full_Data::~Full_Data()
{
}

wxString TransactionModel::Full_Data::real_payee_name(int64 account_id) const
{
    if (TYPE_ID_TRANSFER == type_id(this->TRANSCODE))
    {
        if (this->ACCOUNTID == account_id || account_id < 0)
            return ("> " + this->TOACCOUNTNAME);
        else
            return ("< " + this->ACCOUNTNAME);
    }

    return this->PAYEENAME;
}

const wxString TransactionModel::Full_Data::get_currency_code(int64 account_id) const
{
    if (TYPE_ID_TRANSFER == type_id(this->TRANSCODE))
    {
        if (this->ACCOUNTID == account_id || account_id == -1)
            account_id = this->ACCOUNTID;
        else
            account_id = this->TOACCOUNTID;
    }
    const AccountData* account_n = AccountModel::instance().get_data_n(account_id);
    int64 currency_id = account_n ? account_n->CURRENCYID: -1;
    const CurrencyData* curr = CurrencyModel::instance().get_data_n(currency_id);

    return curr ? curr->CURRENCY_SYMBOL : "";
}

const wxString TransactionModel::Full_Data::get_account_name(int64 account_id) const
{
    if (TYPE_ID_TRANSFER == type_id(this->TRANSCODE)) {
        if (this->ACCOUNTID == account_id || account_id == -1) {
            return this->ACCOUNTNAME;
        }
        else {
            const AccountData* account_n = AccountModel::instance().get_data_n(TOACCOUNTID);
            return account_n ? account_n->ACCOUNTNAME : "";
        }
    }

    return this->ACCOUNTNAME;
}

// same as TransactionModel::is_foreign()
bool TransactionModel::Full_Data::is_foreign() const
{
    return (this->TOACCOUNTID > 0) && (
        type_id(this->TRANSCODE) == TYPE_ID_DEPOSIT ||
        type_id(this->TRANSCODE) == TYPE_ID_WITHDRAWAL
    );
}

// see also TransactionModel::is_foreignAsTransfer()
bool TransactionModel::Full_Data::is_foreign_transfer() const
{
    return is_foreign() && (this->TOACCOUNTID == TransactionLinkModel::AS_TRANSFER);
}

wxString TransactionModel::Full_Data::info() const
{
    // TODO more info
    wxDate date = TransactionModel::getTransDateTime(*this);
    wxString info = wxGetTranslation(wxDate::GetEnglishWeekDayName(date.GetWeekDay()));
    return info;
}

const wxString TransactionModel::Full_Data::to_json()
{
    StringBuffer json_buffer;
    PrettyWriter<StringBuffer> json_writer(json_buffer);
    json_writer.StartObject();

    TransactionData::as_json(json_writer);

    json_writer.Key("ACCOUNTNAME");
    json_writer.String(this->ACCOUNTNAME.utf8_str());

    if (is_transfer(*this)) {
        json_writer.Key("TOACCOUNTNAME");
        json_writer.String(this->TOACCOUNTNAME.utf8_str());
    }
    else {
        json_writer.Key("PAYEENAME");
        json_writer.String(this->PAYEENAME.utf8_str());
    }
    if (this->has_tags()) {
        json_writer.Key("TAGS");
        json_writer.StartArray();
        for (const auto& item : m_splits)
        {
            json_writer.StartObject();
            json_writer.Key(CategoryModel::full_name(item.CATEGID).utf8_str());
            json_writer.Double(item.SPLITTRANSAMOUNT);
            json_writer.EndObject();
        }
        json_writer.EndArray();
    }
    if (this->has_split())
    {
        json_writer.Key("CATEGS");
        json_writer.StartArray();
        for (const auto & item : m_splits)
        {
            json_writer.StartObject();
            json_writer.Key(CategoryModel::full_name(item.CATEGID).utf8_str());
            json_writer.Double(item.SPLITTRANSAMOUNT);
            json_writer.EndObject();
        }
        json_writer.EndArray();
    }
    else
    {
        json_writer.Key("CATEG");
        json_writer.String(CategoryModel::full_name(this->CATEGID).utf8_str());
    }

    json_writer.EndObject();

    wxLogDebug("======= TransactionModel::FullData::to_json =======");
    wxLogDebug("FullData using rapidjson:\n%s", wxString::FromUTF8(json_buffer.GetString()));

    return wxString::FromUTF8(json_buffer.GetString());
}

