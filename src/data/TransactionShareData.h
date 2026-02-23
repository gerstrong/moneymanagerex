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
// This is only sample code re-used from "table/TransactionShareTable.h".
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
#include "table/TransactionShareTable.h"

// User-friendly representation of a record in table SHAREINFO_V1.
struct TransactionShareData
{
    int64 SHAREINFOID; // primary key
    int64 CHECKINGACCOUNTID;
    double SHARENUMBER;
    double SHAREPRICE;
    double SHARECOMMISSION;
    wxString SHARELOT;

    explicit TransactionShareData();
    explicit TransactionShareData(wxSQLite3ResultSet& q);
    TransactionShareData(const TransactionShareData& other) = default;

    int64 id() const { return SHAREINFOID; }
    void id(const int64 id) { SHAREINFOID = id; }
    TransactionShareRow to_row() const;
    TransactionShareData& from_row(const TransactionShareRow& row);
    void to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const;
    void to_update_stmt(wxSQLite3Statement& stmt) const;
    TransactionShareData& from_select_result(wxSQLite3ResultSet& q);
    wxString to_json() const;
    void as_json(PrettyWriter<StringBuffer>& json_writer) const;
    row_t to_html_row() const;
    void to_html_template(html_template& t) const;
    void destroy() { delete this; }

    TransactionShareData& operator= (const TransactionShareData& other);
    TransactionShareData& clone_from(const TransactionShareData& other);
    bool equals(const TransactionShareData* other) const;
    bool operator< (const TransactionShareData& other) const { return id() < other.id(); }
    bool operator< (const TransactionShareData* other) const { return id() < other->id(); }

    struct SorterBySHAREINFOID
    {
        bool operator()(const TransactionShareData& x, const TransactionShareData& y)
        {
            return x.SHAREINFOID < y.SHAREINFOID;
        }
    };

    struct SorterByCHECKINGACCOUNTID
    {
        bool operator()(const TransactionShareData& x, const TransactionShareData& y)
        {
            return x.CHECKINGACCOUNTID < y.CHECKINGACCOUNTID;
        }
    };

    struct SorterBySHARENUMBER
    {
        bool operator()(const TransactionShareData& x, const TransactionShareData& y)
        {
            return x.SHARENUMBER < y.SHARENUMBER;
        }
    };

    struct SorterBySHAREPRICE
    {
        bool operator()(const TransactionShareData& x, const TransactionShareData& y)
        {
            return x.SHAREPRICE < y.SHAREPRICE;
        }
    };

    struct SorterBySHARECOMMISSION
    {
        bool operator()(const TransactionShareData& x, const TransactionShareData& y)
        {
            return x.SHARECOMMISSION < y.SHARECOMMISSION;
        }
    };

    struct SorterBySHARELOT
    {
        bool operator()(const TransactionShareData& x, const TransactionShareData& y)
        {
            return x.SHARELOT < y.SHARELOT;
        }
    };
};

inline TransactionShareData::TransactionShareData(wxSQLite3ResultSet& q)
{
    from_select_result(q);
}

inline void TransactionShareData::to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const
{
    to_row().to_insert_stmt(stmt, id);
}

inline void TransactionShareData::to_update_stmt(wxSQLite3Statement& stmt) const
{
    to_row().to_update_stmt(stmt);
}

inline TransactionShareData& TransactionShareData::from_select_result(wxSQLite3ResultSet& q)
{
    return from_row(TransactionShareRow().from_select_result(q));
}

inline wxString TransactionShareData::to_json() const
{
    to_row().to_json();
}

inline void TransactionShareData::as_json(PrettyWriter<StringBuffer>& json_writer) const
{
    to_row().as_json(json_writer);
}

inline row_t TransactionShareData::to_html_row() const
{
    return to_row().to_html_row();
}

inline void TransactionShareData::to_html_template(html_template& t) const
{
    to_row().to_html_template(t);
}

inline TransactionShareData& TransactionShareData::clone_from(const TransactionShareData& other)
{
    *this = other;
    id(-1);
    return *this;
}
