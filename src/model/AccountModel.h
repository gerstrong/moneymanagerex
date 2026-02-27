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

#pragma once

#include "base/defs.h"
#include "util/mmDate.h"

#include "table/AccountTable.h"
#include "data/AccountData.h"

#include "_ModelBase.h"
#include "CurrencyModel.h"
#include "TrxModel.h"
#include "SchedModel.h"

#include "uicontrols/navigatortypes.h"  // remove later

class AccountModel : public TableFactory<AccountTable, AccountData>
{
public:
    static const wxString refTypeName;

public:
    /**
    Initialize the global AccountModel table on initial call.
    Resets the global table on subsequent calls.
    * Return the static instance address for AccountModel table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static AccountModel& instance(wxSQLite3Database* db);

    /**
    * Return the static instance address for AccountModel table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static AccountModel& instance();

public:
    static AccountCol::STATUS STATUS(OP op, AccountStatus status);

    static wxString get_id_name(int64 account_id);

    static const CurrencyData* currency(const Data* r);
    static const CurrencyData* currency(const Data& r);

    static const TrxModel::DataA transactionsByDateTimeId(const Data* r);
    static const TrxModel::DataA transactionsByDateTimeId(const Data& r);

    static const SchedModel::DataA billsdeposits(const Data* r);
    static const SchedModel::DataA billsdeposits(const Data& r);

    static double balance(const Data* r);
    static double balance(const Data& r);

    static std::pair<double, double> investment_balance(const Data* r);
    static std::pair<double, double> investment_balance(const Data& r);
    static wxString toCurrency(double value, const Data* r);

    static wxString toString(double value, const Data* r, int precision = 2);
    static wxString toString(double value, const Data& r, int precision = 2);

    static NavigatorTypes::TYPE_ID type_id(const Data* account);
    static NavigatorTypes::TYPE_ID type_id(const Data& account);

    static bool FAVORITEACCT(const Data* r);
    static bool FAVORITEACCT(const Data& r);

    static bool is_used(const CurrencyData* c);
    static bool is_used(const CurrencyData& c);

    static int money_accounts_num();

    static bool Exist(const wxString& account_name);

    static bool BoolOf(int64 value);
    static bool is_positive(int value);

public:
    AccountModel();
    ~AccountModel();

public:
    // Return the Data record for the given account name */
    const Data* get_key(const wxString& name);
    // Return the Data record for the given account num */
    const Data* get_num(const wxString& num);
    // Remove the Data record from memory and the database. */
    bool purge_id(int64 id) override;

    wxArrayString all_checking_account_names(bool skip_closed = false);
    const std::map<wxString, int64> all_accounts(bool skip_closed = false);

    const DataA FilterAccounts(const wxString& account_pattern, bool skip_closed = false);

    void resetAccountType(wxString oldtype);
    void resetUnknownAccountTypes();
    wxArrayString getUsedAccountTypes(bool skip_closed = true);
};

//----------------------------------------------------------------------------

inline NavigatorTypes::TYPE_ID AccountModel::type_id(const Data* account)
{
    return static_cast<NavigatorTypes::TYPE_ID>(NavigatorTypes::instance().getTypeIdFromDBName(account->m_type_));
}
inline NavigatorTypes::TYPE_ID AccountModel::type_id(const Data& account)
{
    return type_id(&account);
}

inline bool AccountModel::is_positive(int value)
{
    return value > 0 ? true : false;
}

