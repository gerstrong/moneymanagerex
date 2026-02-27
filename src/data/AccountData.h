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

#pragma once

#include "table/_TableBase.h"
#include "table/AccountTable.h"

// User-friendly representation of a record in table ACCOUNTLIST_V1.
struct AccountData
{
    int64    m_id;
    wxString m_name;
    wxString m_type_;
    wxString m_num;
    wxString m_status_;
    wxString m_notes;
    wxString m_held_at;
    wxString m_website;
    wxString m_contact_info;
    wxString m_access_info;
    double   m_open_balance;
    wxString m_open_date;
    wxString m_favorite_;
    int64    m_currency_id;
    bool     m_stmt_locked;
    wxString m_stmt_date;
    double   m_min_balance;
    double   m_credit_limit;
    double   m_interest_rate;
    wxString m_payment_due_date;
    double   m_min_payment;

    explicit AccountData();
    explicit AccountData(wxSQLite3ResultSet& q);
    AccountData(const AccountData& other) = default;

    int64 id() const { return m_id; }
    void id(const int64 id) { m_id = id; }
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

    AccountData& clone_from(const AccountData& other);
    bool equals(const AccountData* other) const;
    bool operator< (const AccountData& other) const { return id() < other.id(); }
    bool operator< (const AccountData* other) const { return id() < other->id(); }

    struct SorterByACCOUNTID
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_id < y.m_id;
        }
    };

    struct SorterByACCOUNTNAME
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            // Locale case-insensitive
            return std::wcscoll(x.m_name.Lower().wc_str(), y.m_name.Lower().wc_str()) < 0;
        }
    };

    struct SorterByACCOUNTTYPE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_type_ < y.m_type_;
        }
    };

    struct SorterByACCOUNTNUM
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_num < y.m_num;
        }
    };

    struct SorterBySTATUS
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_status_ < y.m_status_;
        }
    };

    struct SorterByNOTES
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_notes < y.m_notes;
        }
    };

    struct SorterByHELDAT
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_held_at < y.m_held_at;
        }
    };

    struct SorterByWEBSITE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_website < y.m_website;
        }
    };

    struct SorterByCONTACTINFO
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_contact_info < y.m_contact_info;
        }
    };

    struct SorterByACCESSINFO
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_access_info < y.m_access_info;
        }
    };

    struct SorterByINITIALBAL
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_open_balance < y.m_open_balance;
        }
    };

    struct SorterByINITIALDATE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_open_date < y.m_open_date;
        }
    };

    struct SorterByFAVORITEACCT
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_favorite_ < y.m_favorite_;
        }
    };

    struct SorterByCURRENCYID
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_currency_id < y.m_currency_id;
        }
    };

    struct SorterBySTATEMENTLOCKED
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return (x.m_stmt_locked ? 1 : 0) < (y.m_stmt_locked ? 1 : 0);
        }
    };

    struct SorterBySTATEMENTDATE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_stmt_date < y.m_stmt_date;
        }
    };

    struct SorterByMINIMUMBALANCE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_min_balance < y.m_min_balance;
        }
    };

    struct SorterByCREDITLIMIT
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_credit_limit < y.m_credit_limit;
        }
    };

    struct SorterByINTERESTRATE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_interest_rate < y.m_interest_rate;
        }
    };

    struct SorterByPAYMENTDUEDATE
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_payment_due_date < y.m_payment_due_date;
        }
    };

    struct SorterByMINIMUMPAYMENT
    {
        bool operator()(const AccountData& x, const AccountData& y)
        {
            return x.m_min_payment < y.m_min_payment;
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
    return to_row().to_json();
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
