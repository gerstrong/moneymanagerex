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

    static const CurrencyData* currency_p(const Data& account_d);
    static wxString to_currency(double value, const Data& account_d);
    static wxString to_string(double value, const Data& account_d, int precision = 2);

    static const TrxModel::DataA transactionsByDateTimeId(const Data& account_d);
    static const SchedModel::DataA billsdeposits(const Data& account_d);
    static double balance(const Data& account_d);
    static std::pair<double, double> investment_balance(const Data& account_d);

    static NavigatorTypes::TYPE_ID type_id(const Data& account_d);

public:
    AccountModel();
    ~AccountModel();

public:
    // Remove the Data record from memory and the database. */
    bool purge_id(int64 id) override;

    const Data* get_key_data_n(const wxString& name);
    const Data* get_num_data_n(const wxString& num);
    const wxString get_id_name(int64 account_id);
    const CurrencyData* get_id_currency_p(int64 account_id);
    DataA find_name_a(const wxString& name);

    wxArrayString all_checking_account_names(bool skip_closed = false);
    const std::map<wxString, int64> all_accounts(bool skip_closed = false);
    const DataA FilterAccounts(const wxString& account_pattern, bool skip_closed = false);
    void resetAccountType(wxString oldtype);
    void resetUnknownAccountTypes();
    wxArrayString getUsedAccountTypes(bool skip_closed = true);
    int money_accounts_num();
};

//----------------------------------------------------------------------------

inline NavigatorTypes::TYPE_ID AccountModel::type_id(const Data& account)
{
    return static_cast<NavigatorTypes::TYPE_ID>(NavigatorTypes::instance().getTypeIdFromDBName(account.m_type_));
}

