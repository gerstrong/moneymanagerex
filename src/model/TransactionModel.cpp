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

ChoicesName TransactionModel::TYPE_CHOICES = ChoicesName({
    { TYPE_ID_WITHDRAWAL, _n("Withdrawal") },
    { TYPE_ID_DEPOSIT,    _n("Deposit") },
    { TYPE_ID_TRANSFER,   _n("Transfer") }
});

ChoicesName TransactionModel::TRADE_TYPE_CHOICES = ChoicesName({
    { TYPE_ID_WITHDRAWAL,  _n("Buy") },
    { TYPE_ID_DEPOSIT, _n("Sell") },
    { TYPE_ID_TRANSFER, _n("Revalue") }
});

ChoicesKeyName TransactionModel::STATUS_CHOICES = ChoicesKeyName({
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
    Model<TransactionTable, TransactionData>()
{
}

TransactionModel::~TransactionModel()
{
}

/**
* Initialize the global TransactionModel table.
* Reset the TransactionModel table or create the table if it does not exist.
*/
TransactionModel& TransactionModel::instance(wxSQLite3Database* db)
{
    TransactionModel& ins = Singleton<TransactionModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();

    return ins;
}

/** Return the static instance of TransactionModel table */
TransactionModel& TransactionModel::instance()
{
    return Singleton<TransactionModel>::instance();
}

bool TransactionModel::remove_depen(int64 id)
{
    // TODO: remove all split at once
    // TransactionSplitModel::instance().remove_depen(TransactionSplitModel::instance().find(TransactionSplitCol::TRANSID(id)));
    for (const auto& ts_d : TransactionSplitModel::instance().find(
        TransactionSplitCol::TRANSID(id)
    )) {
        TransactionSplitModel::instance().remove_depen(ts_d.SPLITTRANSID);
    }
    if (foreignTransaction(*instance().get_data_n(id)))
        TransactionLinkModel::RemoveTranslinkEntry(id);

    const wxString& RefType = TransactionModel::refTypeName;
    // remove all attachments
    mmAttachmentManage::DeleteAllAttachments(RefType, id);
    // remove all custom fields for the transaction
    FieldValueModel::DeleteAllData(RefType, id);
    TagLinkModel::instance().DeleteAllTags(RefType, id);
    return remove_data(id);
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
    return unsafe_save_data(trx_n);
}

const TransactionData* TransactionModel::save_trx(Data& trx_d)
{
    update_timestamp(trx_d);
    return save_data(trx_d);
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

const TransactionModel::DataA TransactionModel::allByDateTimeId()
{
    auto trans = TransactionModel::instance().find_all();
    std::sort(trans.begin(), trans.end());
    if (PreferencesModel::instance().UseTransDateTime())
        std::stable_sort(trans.begin(), trans.end(), TransactionData::SorterByTRANSDATE());
    else
        std::stable_sort(trans.begin(), trans.end(), TransactionModel::SorterByTRANSDATE_DATE());
    return trans;
}

const TransactionSplitModel::DataA TransactionModel::split(const Data* r)
{
    return TransactionSplitModel::instance().find(TransactionSplitCol::TRANSID(r->TRANSID));
}

const TransactionSplitModel::DataA TransactionModel::split(const Data& r)
{
    return TransactionSplitModel::instance().find(TransactionSplitCol::TRANSID(r.TRANSID));
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

wxDateTime TransactionModel::getTransDateTime(const Data* r)
{
    return parseDateTime(r->TRANSDATE);
}

wxDateTime TransactionModel::getTransDateTime(const Data& r)
{
    return parseDateTime(r.TRANSDATE);
}

double TransactionModel::account_flow(const Data* r, int64 account_id)
{
    if (r->ACCOUNTID == r->TOACCOUNTID && type_id(r->TRANSCODE) == TYPE_ID_TRANSFER)
        return 0.0;  // Self Transfer as Revaluation
    if (TransactionModel::status_id(r->STATUS) == TransactionModel::STATUS_ID_VOID || !r->DELETEDTIME.IsEmpty())
        return 0.0;
    if (account_id == r->ACCOUNTID && type_id(r->TRANSCODE) == TYPE_ID_WITHDRAWAL)
        return -(r->TRANSAMOUNT);
    if (account_id == r->ACCOUNTID && type_id(r->TRANSCODE) == TYPE_ID_DEPOSIT)
        return r->TRANSAMOUNT;
    if (account_id == r->ACCOUNTID && type_id(r->TRANSCODE) == TYPE_ID_TRANSFER)
        return -(r->TRANSAMOUNT);
    if (account_id == r->TOACCOUNTID && type_id(r->TRANSCODE) == TYPE_ID_TRANSFER)
        return r->TOTRANSAMOUNT;
    return 0.0;
}

double TransactionModel::account_flow(const Data& r, int64 account_id)
{
    return account_flow(&r, account_id);
}

double TransactionModel::account_outflow(const Data* r, int64 account_id)
{
    double bal = account_flow(r, account_id);
    return bal <= 0 ? -bal : 0;
}

double TransactionModel::account_outflow(const Data& r, int64 account_id)
{
    return account_outflow(&r, account_id);
}

double TransactionModel::account_inflow(const Data* r, int64 account_id)
{
    double bal = account_flow(r, account_id);
    return bal >= 0 ? bal : 0;
}

double TransactionModel::account_inflow(const Data& r, int64 account_id)
{
    return account_inflow(&r, account_id);
}

double TransactionModel::account_recflow(const Data* r, int64 account_id)
{
    return (TransactionModel::status_id(r->STATUS) == TransactionModel::STATUS_ID_RECONCILED) ?
        account_flow(r, account_id) : 0;
}

double TransactionModel::account_recflow(const Data& r, int64 account_id)
{
    return account_recflow(&r, account_id);
}

bool TransactionModel::is_locked(const Data* r)
{
    bool val = false;
    const AccountData* account_n = AccountModel::instance().get_data_n(r->ACCOUNTID);

    if (AccountModel::BoolOf(account_n->STATEMENTLOCKED)) {
        wxDateTime transaction_date;
        if (transaction_date.ParseDate(r->TRANSDATE)) {
            if (transaction_date <= parseDateTime(account_n->STATEMENTDATE)) {
                val = true;
            }
        }
    }
    return val;
}


bool TransactionModel::is_transfer(const wxString& r)
{
    return type_id(r) == TransactionModel::TYPE_ID_TRANSFER;
}
bool TransactionModel::is_transfer(const Data* r)
{
    return is_transfer(r->TRANSCODE);
}
bool TransactionModel::is_deposit(const wxString& r)
{
    return type_id(r) == TransactionModel::TYPE_ID_DEPOSIT;
}
bool TransactionModel::is_deposit(const Data* r)
{
    return is_deposit(r->TRANSCODE);
}

bool TransactionModel::is_split(const Data* r)
{
    if (split(r).empty())
        return false;
    else
        return true;

}

bool TransactionModel::is_split(const Data& r)
{
    return is_split(&r);

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

bool TransactionModel::Full_Data::is_foreign() const
{
    return (this->TOACCOUNTID > 0) &&
        (type_id(this->TRANSCODE) == TYPE_ID_DEPOSIT || type_id(this->TRANSCODE) == TYPE_ID_WITHDRAWAL);
}

bool TransactionModel::Full_Data::is_foreign_transfer() const
{
    return is_foreign() && (this->TOACCOUNTID == TransactionLinkModel::AS_TRANSFER);
}

wxString TransactionModel::Full_Data::info() const
{
    // TODO more info
    wxDate date = TransactionModel::getTransDateTime(this);
    wxString info = wxGetTranslation(wxDate::GetEnglishWeekDayName(date.GetWeekDay()));
    return info;
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

void TransactionModel::setEmptyData(Data &data, int64 accountID)
{
    data.TRANSID = -1;
    data.PAYEEID = -1;
    const wxString today_date = wxDate::Now().FormatISOCombined();
    wxString max_trx_date;
    if (PreferencesModel::instance().getTransDateDefault() != PreferencesModel::NONE) {
        auto trans = instance().find_or(
            TransactionCol::ACCOUNTID(accountID),
            TransactionCol::TOACCOUNTID(accountID)
        );

        for (const auto& t: trans) {
            if (t.DELETEDTIME.IsNull() && max_trx_date < t.TRANSDATE && today_date >= t.TRANSDATE) {
                max_trx_date = t.TRANSDATE;
            }
        }
    }

    if (max_trx_date.empty()) {
        max_trx_date = today_date;
    }

    data.TRANSDATE = max_trx_date;
    data.ACCOUNTID = accountID;
    data.STATUS = status_key(PreferencesModel::instance().getTransStatusReconciled());
    data.TRANSCODE = TYPE_NAME_WITHDRAWAL;
    data.CATEGID = -1;
    data.FOLLOWUPID = -1;
    data.TRANSAMOUNT = 0;
    data.TOTRANSAMOUNT = 0;
    data.TRANSACTIONNUMBER = "";
    data.COLOR = -1;
}

bool TransactionModel::getTransactionData(Data &data, const Data* r)
{
    if (r) {
        data.TRANSDATE = r->TRANSDATE;
        data.STATUS = r->STATUS;
        data.ACCOUNTID = r->ACCOUNTID;
        data.TOACCOUNTID = r->TOACCOUNTID;
        data.TRANSCODE = r->TRANSCODE;
        data.CATEGID = r->CATEGID;
        data.TRANSAMOUNT = r->TRANSAMOUNT;
        data.TOTRANSAMOUNT = r->TOTRANSAMOUNT;
        data.FOLLOWUPID = r->FOLLOWUPID;
        data.NOTES = r->NOTES;
        data.TRANSACTIONNUMBER = r->TRANSACTIONNUMBER;
        data.PAYEEID = r->PAYEEID;
        data.TRANSID = r->TRANSID;
        data.LASTUPDATEDTIME = r->LASTUPDATEDTIME;
        data.DELETEDTIME = r->DELETEDTIME;
        data.COLOR = r->COLOR;
    }
    return r ? true : false;
}

void TransactionModel::putDataToTransaction(Data *r, const Data &data)
{
    r->STATUS = data.STATUS;
    r->TRANSCODE = data.TRANSCODE;
    r->TRANSDATE = data.TRANSDATE;
    r->PAYEEID = data.PAYEEID;
    r->ACCOUNTID = data.ACCOUNTID;
    r->TRANSAMOUNT = data.TRANSAMOUNT;
    r->CATEGID = data.CATEGID;
    r->TOACCOUNTID = data.TOACCOUNTID;
    r->TOTRANSAMOUNT = data.TOTRANSAMOUNT;
    r->NOTES = data.NOTES;
    r->TRANSACTIONNUMBER = data.TRANSACTIONNUMBER;
    r->FOLLOWUPID = data.FOLLOWUPID;
    r->COLOR = data.COLOR;
}

const wxString TransactionModel::Full_Data::to_json()
{
    StringBuffer json_buffer;
    PrettyWriter<StringBuffer> json_writer(json_buffer);
    json_writer.StartObject();

    TransactionData::as_json(json_writer);

    json_writer.Key("ACCOUNTNAME");
    json_writer.String(this->ACCOUNTNAME.utf8_str());

    if (is_transfer(this))
    {
        json_writer.Key("TOACCOUNTNAME");
        json_writer.String(this->TOACCOUNTNAME.utf8_str());
    }
    else
    {
        json_writer.Key("PAYEENAME");
        json_writer.String(this->PAYEENAME.utf8_str());
    }
    if (this->has_tags())
    {
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

bool TransactionModel::foreignTransaction(const Data& data)
{
    return (data.TOACCOUNTID > 0) && (data.TRANSCODE == TYPE_NAME_DEPOSIT || data.TRANSCODE == TYPE_NAME_WITHDRAWAL);
}

bool TransactionModel::foreignTransactionAsTransfer(const Data& data)
{
    return foreignTransaction(data) && (data.TOACCOUNTID == TransactionLinkModel::AS_TRANSFER || data.TOACCOUNTID == data.ACCOUNTID);
}

void TransactionModel::updateTimestamp(int64 id)
{
    Data* trx_n = instance().unsafe_get_data_n(id);
    if (trx_n && trx_n->TRANSID == id) {
        trx_n->LASTUPDATEDTIME = wxDateTime::Now().ToUTC().FormatISOCombined();
        unsafe_update_data(trx_n);
    }
}
