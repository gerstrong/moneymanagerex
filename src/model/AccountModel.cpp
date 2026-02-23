/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)
 Copyright (C) 2014, 2020 - 2022 Nikolay Akimov
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

#include <unordered_set>

#include "AccountModel.h"
#include "PreferencesModel.h"
#include "StockModel.h"
#include "TransactionLinkModel.h"
#include "TransactionShareModel.h"

ChoicesName AccountModel::STATUS_CHOICES = ChoicesName({
    { STATUS_ID_OPEN,   _n("Open") },
    { STATUS_ID_CLOSED, _n("Closed") }
});

const wxString AccountModel::STATUS_NAME_OPEN   = status_name(STATUS_ID_OPEN);
const wxString AccountModel::STATUS_NAME_CLOSED = status_name(STATUS_ID_CLOSED);

AccountModel::AccountModel() :
    Model<AccountTable, AccountData>()
{
}

AccountModel::~AccountModel()
{
}

/**
* Initialize the global AccountModel table.
* Reset the AccountModel table or create the table if it does not exist.
*/
AccountModel& AccountModel::instance(wxSQLite3Database* db)
{
    AccountModel& ins = Singleton<AccountModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();
    ins.preload_cache();

    return ins;
}

/** Return the static instance of AccountModel table */
AccountModel& AccountModel::instance()
{
    return Singleton<AccountModel>::instance();
}

wxArrayString AccountModel::all_checking_account_names(bool skip_closed)
{
    wxArrayString accounts;
    for (const auto &account : this->find_all(Col::COL_ID_ACCOUNTNAME))
    {
        if (skip_closed && status_id(account) == STATUS_ID_CLOSED)
            continue;
        if (type_id(account) == NavigatorTypes::TYPE_ID_SHARES)
            continue;
        if (account.ACCOUNTNAME.empty())
            continue;
        accounts.Add(account.ACCOUNTNAME);
    }
    return accounts;
}

const std::map<wxString, int64> AccountModel::all_accounts(bool skip_closed)
{
    std::map<wxString, int64> accounts;
    for (const auto& account : this->find_all(Col::COL_ID_ACCOUNTNAME))
    {
        if (skip_closed && status_id(account) == STATUS_ID_CLOSED)
            continue;
        if (type_id(account) == NavigatorTypes::TYPE_ID_SHARES)
            continue;
        if (account.ACCOUNTNAME.empty())
            continue;
        accounts[account.ACCOUNTNAME] = account.ACCOUNTID;
    }
    return accounts;
}

/** Get the Data record instance in memory. */
const AccountData* AccountModel::get_key(const wxString& name)
{
    const Data* account_n = search_cache_n(AccountCol::ACCOUNTNAME(name));
    if (account_n)
        return account_n;

    DataA items = find(AccountCol::ACCOUNTNAME(name));
    if (!items.empty())
        account_n = get_data_n(items[0].ACCOUNTID);
    return account_n;
}

/** Get the Data record instance in memory. */
const AccountData* AccountModel::get_num(const wxString& num)
{
    const Data* account_n = search_cache_n(AccountCol::ACCOUNTNUM(num));
    if (account_n)
        return account_n;

    DataA items = find(AccountCol::ACCOUNTNUM(num));
    if (!items.empty())
        account_n = get_data_n(items[0].ACCOUNTID);
    return account_n;
}

wxString AccountModel::get_id_name(int64 account_id)
{
    const Data* account_n = instance().get_data_n(account_id);
    if (account_n)
        return account_n->ACCOUNTNAME;
    else
        return _t("Account Error");
}

/** Remove the Data record instance from memory and the database. */
bool AccountModel::remove_depen(int64 id)
{
    Savepoint();
    for (const auto& r: TransactionModel::instance().find_or(
        TransactionCol::ACCOUNTID(id),
        TransactionCol::TOACCOUNTID(id)
    )) {
        if (TransactionModel::foreignTransaction(r)) {
            TransactionShareModel::RemoveShareEntry(r.TRANSID);
            TransactionLinkData tr = TransactionLinkModel::TranslinkRecord(r.TRANSID);
            TransactionLinkModel::instance().remove_depen(tr.TRANSLINKID);
        }
        TransactionModel::instance().remove_depen(r.TRANSID);
    }
    for (const auto& r: ScheduledModel::instance().find_or(
        ScheduledCol::ACCOUNTID(id),
        ScheduledCol::TOACCOUNTID(id)
    ))
        ScheduledModel::instance().remove_depen(r.BDID);

    for (const auto& r : StockModel::instance().find(StockCol::HELDAT(id))) {
        TransactionLinkModel::RemoveTransLinkRecords<StockModel>(r.STOCKID);
        StockModel::instance().remove_depen(r.STOCKID);
    }
    ReleaseSavepoint();

    return remove_data(id);
}

const CurrencyData* AccountModel::currency(const Data* r)
{
    const CurrencyData * currency_n = CurrencyModel::instance().get_data_n(r->CURRENCYID);
    if (currency_n)
        return currency_n;
    else
    {
        wxASSERT(false);
        return CurrencyModel::GetBaseCurrency();
    }
}

const CurrencyData* AccountModel::currency(const Data& r)
{
    return currency(&r);
}

const TransactionModel::DataA AccountModel::transactionsByDateTimeId(const Data*r)
{
    auto trans = TransactionModel::instance().find_or(
        TransactionCol::ACCOUNTID(r->ACCOUNTID),
        TransactionCol::TOACCOUNTID(r->ACCOUNTID)
    );
    std::sort(trans.begin(), trans.end());
    if (PreferencesModel::instance().UseTransDateTime())
        std::stable_sort(trans.begin(), trans.end(), TransactionData::SorterByTRANSDATE());
    else
        std::stable_sort(trans.begin(), trans.end(), TransactionModel::SorterByTRANSDATE_DATE());
    return trans;
}

const TransactionModel::DataA AccountModel::transactionsByDateTimeId(const Data& r)
{
    return transactionsByDateTimeId(&r);
}

const ScheduledModel::DataA AccountModel::billsdeposits(const Data* r)
{
    return ScheduledModel::instance().find_or(
        ScheduledCol::ACCOUNTID(r->ACCOUNTID),
        ScheduledCol::TOACCOUNTID(r->ACCOUNTID)
    );
}

const ScheduledModel::DataA AccountModel::billsdeposits(const Data& r)
{
    return billsdeposits(&r);
}

double AccountModel::balance(const Data* r)
{
    double sum = r->INITIALBAL;
    for (const auto& tran: transactionsByDateTimeId(r))
    {
        sum += TransactionModel::account_flow(tran, r->ACCOUNTID);
    }

    return sum;
}

double AccountModel::balance(const Data& r)
{
    return balance(&r);
}

std::pair<double, double> AccountModel::investment_balance(const Data* r)
{
    std::pair<double /*market value*/, double /*invest value*/> sum;
    for (const auto& stock: StockModel::instance().find(StockCol::HELDAT(r->ACCOUNTID)))
    {
        sum.first += StockModel::CurrentValue(stock);
        sum.second += StockModel::InvestmentValue(stock);
    }

    for (const auto& asset: AssetModel::instance().find_or(
        AssetCol::ASSETNAME(r->ACCOUNTNAME),
        AssetCol::ASSETTYPE(r->ACCOUNTNAME)
    )) {
        auto asset_bal = AssetModel::value(asset);
        sum.first += asset_bal.second;
        sum.second += asset_bal.first;
    }
    return sum;
}

std::pair<double, double> AccountModel::investment_balance(const Data& r)
{
    return investment_balance(&r);
}

wxString AccountModel::toCurrency(double value, const Data* r)
{
    return CurrencyModel::toCurrency(value, currency(r));
}

wxString AccountModel::toString(double value, const Data* r, int precision)
{
    return CurrencyModel::toString(value, currency(r), precision);
}

wxString AccountModel::toString(double value, const Data& r, int precision)
{
    return toString(value, &r, precision);
}

AccountCol::STATUS AccountModel::STATUS(OP op, STATUS_ID status)
{
    return AccountCol::STATUS(op, status_name(status));
}

bool AccountModel::FAVORITEACCT(const Data* r)
{
    return r->FAVORITEACCT.CmpNoCase("TRUE") == 0;
}
bool AccountModel::FAVORITEACCT(const Data& r)
{
    return FAVORITEACCT(&r);
}

bool AccountModel::is_used(const CurrencyData* c)
{
    if (!c) return false;
    const auto &accounts = AccountModel::instance().find(
        AccountCol::CURRENCYID(c->CURRENCYID),
        AccountModel::STATUS(OP_NE, STATUS_ID_CLOSED)
    );
    return !accounts.empty();
}

bool AccountModel::is_used(const CurrencyData& c)
{
    return is_used(&c);
}

int AccountModel::money_accounts_num()
{
    return
        AccountModel::instance().find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_CASH))
        ).size() + AccountModel::instance().find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_CHECKING))
        ).size() + AccountModel::instance().find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_CREDIT_CARD))
        ).size() + AccountModel::instance().find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_LOAN))
        ).size() + AccountModel::instance().find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_TERM))
        ).size() + AccountModel::instance().find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_ASSET))
        ).size() + AccountModel::instance().find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_SHARES))
        ).size();
}

bool AccountModel::Exist(const wxString& account_name)
{
    DataA list = instance().find(AccountCol::ACCOUNTNAME(account_name));

    return !list.empty();
}

bool AccountModel::BoolOf(int64 value)
{
    return value > 0 ? true : false;
}

const AccountModel::DataA AccountModel::FilterAccounts(const wxString& account_pattern, bool skip_closed)
{
    DataA accounts;
    for (auto &account : this->find_all(AccountCol::COL_ID_ACCOUNTNAME))
    {
        if (skip_closed && status_id(account) == STATUS_ID_CLOSED)
            continue;
        if (type_id(account) == NavigatorTypes::TYPE_ID_INVESTMENT)
            continue;
        if (account.ACCOUNTNAME.Lower().Matches(account_pattern.Lower().Append("*")))
            accounts.push_back(account);
    }
    return accounts;
}

void AccountModel::resetAccountType(wxString oldtype)
{
    for (auto& account_d : AccountModel::instance().find(AccountCol::ACCOUNTTYPE(oldtype))) {
        // CHECK: use account_d directly
        AccountData acc_d = *(get_key(account_d.ACCOUNTNAME));
        acc_d.ACCOUNTTYPE = "Checking";
        save_data(acc_d);
    }
}

void AccountModel::resetUnknownAccountTypes()
{
    for (const auto& account_d : find_all(Col::COL_ID_ACCOUNTNAME)) {
        if (NavigatorTypes::instance().getTypeIdFromDBName(account_d.ACCOUNTTYPE, -1) == -1) {
            // CHECK: use account_d directly
            AccountData acc_d = *(get_key(account_d.ACCOUNTNAME));
            acc_d.ACCOUNTTYPE = "Checking";
            save_data(acc_d);
        }
    }
}

wxArrayString AccountModel::getUsedAccountTypes(bool skip_closed)
{
    wxArrayString usedTypes;
    for (auto &account : this->find_all(AccountCol::COL_ID_ACCOUNTTYPE))
    {
        if (skip_closed && status_id(account) == STATUS_ID_CLOSED)
            continue;
        if (type_id(account) == NavigatorTypes::TYPE_ID_INVESTMENT)
            continue;
        if (usedTypes.Index(account.ACCOUNTTYPE) == wxNOT_FOUND) {
            usedTypes.Add(account.ACCOUNTTYPE);
        }
    }
    return usedTypes;
}
