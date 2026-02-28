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
#include "PrefModel.h"
#include "StockModel.h"
#include "TrxLinkModel.h"
#include "TrxShareModel.h"

AccountModel::AccountModel() :
    TableFactory<AccountTable, AccountData>()
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
    for (const auto& account_d : this->find_all(Col::COL_ID_ACCOUNTNAME)) {
        if (skip_closed && account_d.is_closed())
            continue;
        if (type_id(account_d) == NavigatorTypes::TYPE_ID_SHARES)
            continue;
        if (account_d.m_name.empty())
            continue;
        accounts.Add(account_d.m_name);
    }
    return accounts;
}

const std::map<wxString, int64> AccountModel::all_accounts(bool skip_closed)
{
    std::map<wxString, int64> accounts;
    for (const auto& account_d : this->find_all(Col::COL_ID_ACCOUNTNAME)) {
        if (skip_closed && account_d.is_closed())
            continue;
        if (type_id(account_d) == NavigatorTypes::TYPE_ID_SHARES)
            continue;
        if (account_d.m_name.empty())
            continue;
        accounts[account_d.m_name] = account_d.m_id;
    }
    return accounts;
}

/** Get the Data record instance in memory. */
const AccountData* AccountModel::get_key_data_n(const wxString& name)
{
    const Data* account_n = search_cache_n(AccountCol::ACCOUNTNAME(name));
    if (account_n)
        return account_n;

    DataA account_a = find(AccountCol::ACCOUNTNAME(name));
    if (!account_a.empty())
        account_n = get_data_n(account_a[0].m_id);
    return account_n;
}

/** Get the Data record instance in memory. */
const AccountData* AccountModel::get_num_data_n(const wxString& num)
{
    const Data* account_n = search_cache_n(AccountCol::ACCOUNTNUM(num));
    if (account_n)
        return account_n;

    DataA account_a = find(AccountCol::ACCOUNTNUM(num));
    if (!account_a.empty())
        account_n = get_data_n(account_a[0].m_id);
    return account_n;
}

const wxString AccountModel::get_id_name(int64 account_id)
{
    const Data* account_n = get_data_n(account_id);
    if (account_n)
        return account_n->m_name;
    else
        return _t("Account Error");
}

/** Remove the Data record instance from memory and the database. */
bool AccountModel::purge_id(int64 id)
{
    // FIXME: check if id is used in InfoTable

    db_savepoint();

    for (const auto& trx_d : TrxModel::instance().find_or(
        TrxCol::ACCOUNTID(id),
        TrxCol::TOACCOUNTID(id)
    )) {
        if (TrxModel::is_foreign(trx_d)) {
            TrxShareModel::instance().remove_trx_share(trx_d.TRANSID);
            TrxLinkData tr = TrxLinkModel::TranslinkRecord(trx_d.TRANSID);
            TrxLinkModel::instance().purge_id(tr.TRANSLINKID);
        }
        TrxModel::instance().purge_id(trx_d.TRANSID);
    }

    for (const auto& sched_d : SchedModel::instance().find_or(
        SchedCol::ACCOUNTID(id),
        SchedCol::TOACCOUNTID(id)
    ))
        SchedModel::instance().purge_id(sched_d.BDID);

    for (const auto& stock_d : StockModel::instance().find(StockCol::HELDAT(id))) {
        TrxLinkModel::RemoveTransLinkRecords<StockModel>(stock_d.m_id);
        StockModel::instance().purge_id(stock_d.m_id);
    }

    // FIXME: remove AttachmentData owned by id

    db_release_savepoint();

    return unsafe_remove_data(id);
}

AccountCol::STATUS AccountModel::STATUS(OP op, AccountStatus status)
{
    return AccountCol::STATUS(op, status.name());
}

const CurrencyData* AccountModel::currency_p(const Data& account_d)
{
    const CurrencyData* currency_n = CurrencyModel::instance().get_data_n(
        account_d.m_currency_id
    );
    if (currency_n)
        return currency_n;
    else {
        wxASSERT(false);
        return CurrencyModel::GetBaseCurrency();
    }
}

const CurrencyData* AccountModel::get_id_currency_p(int64 account_id)
{
    const Data* account_n = get_data_n(account_id);
    if (account_n)
        return AccountModel::currency_p(*account_n);
    else {
        wxASSERT(false);
        return CurrencyModel::GetBaseCurrency();
    }
}

const TrxModel::DataA AccountModel::transactionsByDateTimeId(const Data& account_d)
{
    auto trx_a = TrxModel::instance().find_or(
        TrxCol::ACCOUNTID(account_d.m_id),
        TrxCol::TOACCOUNTID(account_d.m_id)
    );
    std::sort(trx_a.begin(), trx_a.end());
    if (PrefModel::instance().UseTransDateTime())
        std::stable_sort(trx_a.begin(), trx_a.end(), TrxData::SorterByTRANSDATE());
    else
        std::stable_sort(trx_a.begin(), trx_a.end(), TrxModel::SorterByTRANSDATE_DATE());
    return trx_a;
}

const SchedModel::DataA AccountModel::billsdeposits(const Data& account_d)
{
    return SchedModel::instance().find_or(
        SchedCol::ACCOUNTID(account_d.m_id),
        SchedCol::TOACCOUNTID(account_d.m_id)
    );
}

double AccountModel::balance(const Data& account_d)
{
    double sum = account_d.m_open_balance;
    for (const auto& trx_a: transactionsByDateTimeId(account_d)) {
        sum += TrxModel::account_flow(trx_a, account_d.m_id);
    }
    return sum;
}

std::pair<double, double> AccountModel::investment_balance(const Data& account_d)
{
    std::pair<double /*market value*/, double /*invest value*/> sum;
    for (const auto& stock: StockModel::instance().find(StockCol::HELDAT(account_d.m_id))) {
        sum.first  += StockModel::CurrentValue(stock);
        sum.second += StockModel::InvestmentValue(stock);
    }

    for (const auto& asset: AssetModel::instance().find_or(
        AssetCol::ASSETNAME(account_d.m_name),
        AssetCol::ASSETTYPE(account_d.m_name)
    )) {
        auto asset_bal = AssetModel::instance().value(asset);
        sum.first  += asset_bal.second;
        sum.second += asset_bal.first;
    }
    return sum;
}

wxString AccountModel::to_currency(double value, const Data& account_d)
{
    return CurrencyModel::toCurrency(value, AccountModel::currency_p(account_d));
}

wxString AccountModel::to_string(double value, const Data& account_d, int precision)
{
    return CurrencyModel::toString(value, AccountModel::currency_p(account_d), precision);
}

int AccountModel::money_accounts_num()
{
    return
        find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_CASH))
        ).size() + find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_CHECKING))
        ).size() + find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_CREDIT_CARD))
        ).size() + find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_LOAN))
        ).size() + find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_TERM))
        ).size() + find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_ASSET))
        ).size() + find(
            AccountCol::ACCOUNTTYPE(NavigatorTypes::instance().type_name(NavigatorTypes::TYPE_ID_SHARES))
        ).size();
}

AccountModel::DataA AccountModel::find_name_a(const wxString& name)
{
    return find(AccountCol::ACCOUNTNAME(name));
}

const AccountModel::DataA AccountModel::FilterAccounts(const wxString& account_pattern, bool skip_closed)
{
    DataA accounts;
    for (auto &account_d : this->find_all(AccountCol::COL_ID_ACCOUNTNAME))
    {
        if (skip_closed && account_d.is_closed())
            continue;
        if (type_id(account_d) == NavigatorTypes::TYPE_ID_INVESTMENT)
            continue;
        if (account_d.m_name.Lower().Matches(account_pattern.Lower().Append("*")))
            accounts.push_back(account_d);
    }
    return accounts;
}

void AccountModel::resetAccountType(wxString oldtype)
{
    for (auto& account_d : AccountModel::instance().find(
        AccountCol::ACCOUNTTYPE(oldtype)
    )) {
        // CHECK: use account_d directly
        AccountData acc_d = *(get_key_data_n(account_d.m_name));
        acc_d.m_type_ = "Checking";
        save_data_n(acc_d);
    }
}

void AccountModel::resetUnknownAccountTypes()
{
    for (const auto& account_d : find_all(Col::COL_ID_ACCOUNTNAME)) {
        if (NavigatorTypes::instance().getTypeIdFromDBName(account_d.m_type_, -1) == -1) {
            // CHECK: use account_d directly
            AccountData acc_d = *(get_key_data_n(account_d.m_name));
            acc_d.m_type_ = "Checking";
            save_data_n(acc_d);
        }
    }
}

wxArrayString AccountModel::getUsedAccountTypes(bool skip_closed)
{
    wxArrayString usedTypes;
    for (auto& account_d : this->find_all(AccountCol::COL_ID_ACCOUNTTYPE)) {
        if (skip_closed && account_d.is_closed())
            continue;
        if (type_id(account_d) == NavigatorTypes::TYPE_ID_INVESTMENT)
            continue;
        if (usedTypes.Index(account_d.m_type_) == wxNOT_FOUND) {
            usedTypes.Add(account_d.m_type_);
        }
    }
    return usedTypes;
}
