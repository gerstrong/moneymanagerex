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
// This is only sample code re-used from "table/PayeeTable.h".
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
#include "table/PayeeTable.h"

// User-friendly representation of a record in table PAYEE_V1.
struct PayeeData
{
    int64 PAYEEID; // primary key
    wxString PAYEENAME;
    int64 CATEGID;
    wxString NUMBER;
    wxString WEBSITE;
    wxString NOTES;
    int64 ACTIVE;
    wxString PATTERN;

    explicit PayeeData();
    explicit PayeeData(wxSQLite3ResultSet& q);
    PayeeData(const PayeeData& other) = default;

    int64 id() const { return PAYEEID; }
    void id(const int64 id) { PAYEEID = id; }
    PayeeRow to_row() const;
    PayeeData& from_row(const PayeeRow& row);
    void to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const;
    void to_update_stmt(wxSQLite3Statement& stmt) const;
    PayeeData& from_select_result(wxSQLite3ResultSet& q);
    wxString to_json() const;
    void as_json(PrettyWriter<StringBuffer>& json_writer) const;
    row_t to_html_row() const;
    void to_html_template(html_template& t) const;
    void destroy() { delete this; }

    PayeeData& operator= (const PayeeData& other);
    PayeeData& clone_from(const PayeeData& other);
    bool equals(const PayeeData* other) const;
    bool operator< (const PayeeData& other) const { return id() < other.id(); }
    bool operator< (const PayeeData* other) const { return id() < other->id(); }

    struct SorterByPAYEEID
    {
        bool operator()(const PayeeData& x, const PayeeData& y)
        {
            return x.PAYEEID < y.PAYEEID;
        }
    };

    struct SorterByPAYEENAME
    {
        bool operator()(const PayeeData& x, const PayeeData& y)
        {
            // Locale case-insensitive
            return std::wcscoll(x.PAYEENAME.Lower().wc_str(), y.PAYEENAME.Lower().wc_str()) < 0;
        }
    };

    struct SorterByCATEGID
    {
        bool operator()(const PayeeData& x, const PayeeData& y)
        {
            return x.CATEGID < y.CATEGID;
        }
    };

    struct SorterByNUMBER
    {
        bool operator()(const PayeeData& x, const PayeeData& y)
        {
            return x.NUMBER < y.NUMBER;
        }
    };

    struct SorterByWEBSITE
    {
        bool operator()(const PayeeData& x, const PayeeData& y)
        {
            return x.WEBSITE < y.WEBSITE;
        }
    };

    struct SorterByNOTES
    {
        bool operator()(const PayeeData& x, const PayeeData& y)
        {
            return x.NOTES < y.NOTES;
        }
    };

    struct SorterByACTIVE
    {
        bool operator()(const PayeeData& x, const PayeeData& y)
        {
            return x.ACTIVE < y.ACTIVE;
        }
    };

    struct SorterByPATTERN
    {
        bool operator()(const PayeeData& x, const PayeeData& y)
        {
            return x.PATTERN < y.PATTERN;
        }
    };
};

inline PayeeData::PayeeData(wxSQLite3ResultSet& q)
{
    from_select_result(q);
}

inline void PayeeData::to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const
{
    to_row().to_insert_stmt(stmt, id);
}

inline void PayeeData::to_update_stmt(wxSQLite3Statement& stmt) const
{
    to_row().to_update_stmt(stmt);
}

inline PayeeData& PayeeData::from_select_result(wxSQLite3ResultSet& q)
{
    return from_row(PayeeRow().from_select_result(q));
}

inline wxString PayeeData::to_json() const
{
    to_row().to_json();
}

inline void PayeeData::as_json(PrettyWriter<StringBuffer>& json_writer) const
{
    to_row().as_json(json_writer);
}

inline row_t PayeeData::to_html_row() const
{
    return to_row().to_html_row();
}

inline void PayeeData::to_html_template(html_template& t) const
{
    to_row().to_html_template(t);
}

inline PayeeData& PayeeData::clone_from(const PayeeData& other)
{
    *this = other;
    id(-1);
    return *this;
}
