/*******************************************************
 Copyright (C) 2026 George Ef (george.a.ef@gmail.com)

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

// PLEASE EDIT!
//
// This is only sample code re-used from "table/AccountTable.h".
//
// The data structure can be refined by:
// * using more user-frielndly filed name
// * using stronger field types
// * adding enumerations for fields with limited choices
// * demultiplexing composite values in database columns
//
// See also an implementation in Swift:
//   https://github.com/moneymanagerex/mmex-ios/tree/master/MMEX/Data
// and an implementation in Java:
//   https://github.com/moneymanagerex/android-money-manager-ex/tree/master/app/src/main/java/com/money/manager/ex/domainmodel

#pragma once

#include "table/_TableBase.h"
#include "table/AccountTable.h"

// User-friendly representation of a record in table ACCOUNTLIST_V1.
struct AccountData
{
    int64 ACCOUNTID; // primary key
    wxString ACCOUNTNAME;
    wxString ACCOUNTTYPE;
    wxString ACCOUNTNUM;
    wxString STATUS;
    wxString NOTES;
    wxString HELDAT;
    wxString WEBSITE;
    wxString CONTACTINFO;
    wxString ACCESSINFO;
    double INITIALBAL;
    wxString INITIALDATE;
    wxString FAVORITEACCT;
    int64 CURRENCYID;
    int64 STATEMENTLOCKED;
    wxString STATEMENTDATE;
    double MINIMUMBALANCE;
    double CREDITLIMIT;
    double INTERESTRATE;
    wxString PAYMENTDUEDATE;
    double MINIMUMPAYMENT;

    explicit AccountData();
    explicit AccountData(wxSQLite3ResultSet& q);
    AccountData(const AccountData& other) = default;

    int64 id() const { return ACCOUNTID; }
    void id(const int64 id) { ACCOUNTID = id; }
    AccountRow to_row() const;
    AccountData& from_row(const AccountRow& row);
    void to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const;
    void to_update_stmt(wxSQLite3Statement& stmt) const;
    AccountData& from_select_result(wxSQLite3ResultSet& q);
    wxString to_json() const;
    void as_json(PrettyWriter<StringBuffer>& json_writer) const;
    row_t to_html_row() const;
    void to_html_template(html_template& t) const;
    void destroy() { delete this; }

    AccountData& operator= (const AccountData& other);
    AccountData& clone_from(const AccountData& other);
    bool equals(const AccountData* other) const;
    bool operator< (const AccountData& other) const { return id() < other.id(); }
    bool operator< (const AccountData* other) const { return id() < other->id(); }

    struct SorterByACCOUNTID
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.ACCOUNTID < y.ACCOUNTID;
        }
    };

    struct SorterByACCOUNTNAME
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            // Locale case-insensitive
            return std::wcscoll(x.ACCOUNTNAME.Lower().wc_str(), y.ACCOUNTNAME.Lower().wc_str()) < 0;
        }
    };

    struct SorterByACCOUNTTYPE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.ACCOUNTTYPE < y.ACCOUNTTYPE;
        }
    };

    struct SorterByACCOUNTNUM
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.ACCOUNTNUM < y.ACCOUNTNUM;
        }
    };

    struct SorterBySTATUS
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.STATUS < y.STATUS;
        }
    };

    struct SorterByNOTES
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.NOTES < y.NOTES;
        }
    };

    struct SorterByHELDAT
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.HELDAT < y.HELDAT;
        }
    };

    struct SorterByWEBSITE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.WEBSITE < y.WEBSITE;
        }
    };

    struct SorterByCONTACTINFO
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.CONTACTINFO < y.CONTACTINFO;
        }
    };

    struct SorterByACCESSINFO
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.ACCESSINFO < y.ACCESSINFO;
        }
    };

    struct SorterByINITIALBAL
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.INITIALBAL < y.INITIALBAL;
        }
    };

    struct SorterByINITIALDATE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.INITIALDATE < y.INITIALDATE;
        }
    };

    struct SorterByFAVORITEACCT
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.FAVORITEACCT < y.FAVORITEACCT;
        }
    };

    struct SorterByCURRENCYID
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.CURRENCYID < y.CURRENCYID;
        }
    };

    struct SorterBySTATEMENTLOCKED
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.STATEMENTLOCKED < y.STATEMENTLOCKED;
        }
    };

    struct SorterBySTATEMENTDATE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.STATEMENTDATE < y.STATEMENTDATE;
        }
    };

    struct SorterByMINIMUMBALANCE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.MINIMUMBALANCE < y.MINIMUMBALANCE;
        }
    };

    struct SorterByCREDITLIMIT
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.CREDITLIMIT < y.CREDITLIMIT;
        }
    };

    struct SorterByINTERESTRATE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.INTERESTRATE < y.INTERESTRATE;
        }
    };

    struct SorterByPAYMENTDUEDATE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.PAYMENTDUEDATE < y.PAYMENTDUEDATE;
        }
    };

    struct SorterByMINIMUMPAYMENT
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.MINIMUMPAYMENT < y.MINIMUMPAYMENT;
        }
    };
};

inline AccountData::AccountData(wxSQLite3ResultSet& q)
{
    from_select_result(q);
}

inline void AccountData::to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const
{
    to_row().to_insert_stmt(stmt, id);
}

inline void AccountData::to_update_stmt(wxSQLite3Statement& stmt) const
{
    to_row().to_update_stmt(stmt);
}

inline AccountData& AccountData::from_select_result(wxSQLite3ResultSet& q)
{
    return from_row(AccountRow().from_select_result(q));
}

inline wxString AccountData::to_json() const
{
    to_row().to_json();
}

inline void AccountData::as_json(PrettyWriter<StringBuffer>& json_writer) const
{
    to_row().as_json(json_writer);
}

inline row_t AccountData::to_html_row() const
{
    return to_row().to_html_row();
}

inline void AccountData::to_html_template(html_template& t) const
{
    to_row().to_html_template(t);
}

inline AccountData& AccountData::clone_from(const AccountData& other)
{
    *this = other;
    id(-1);
    return *this;
}
