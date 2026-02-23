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
// This is only sample code re-used from "table/CurrencyHistoryTable.h".
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
#include "table/CurrencyHistoryTable.h"

// User-friendly representation of a record in table CURRENCYHISTORY_V1.
struct CurrencyHistoryData
{
    int64 CURRHISTID; // primary key
    int64 CURRENCYID;
    wxString CURRDATE;
    double CURRVALUE;
    int64 CURRUPDTYPE;

    explicit CurrencyHistoryData();
    explicit CurrencyHistoryData(wxSQLite3ResultSet& q);
    CurrencyHistoryData(const CurrencyHistoryData& other) = default;

    int64 id() const { return CURRHISTID; }
    void id(const int64 id) { CURRHISTID = id; }
    CurrencyHistoryRow to_row() const;
    CurrencyHistoryData& from_row(const CurrencyHistoryRow& row);
    void to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const;
    void to_update_stmt(wxSQLite3Statement& stmt) const;
    CurrencyHistoryData& from_select_result(wxSQLite3ResultSet& q);
    wxString to_json() const;
    void as_json(PrettyWriter<StringBuffer>& json_writer) const;
    row_t to_html_row() const;
    void to_html_template(html_template& t) const;
    void destroy() { delete this; }

    CurrencyHistoryData& operator= (const CurrencyHistoryData& other);
    CurrencyHistoryData& clone_from(const CurrencyHistoryData& other);
    bool equals(const CurrencyHistoryData* other) const;
    bool operator< (const CurrencyHistoryData& other) const { return id() < other.id(); }
    bool operator< (const CurrencyHistoryData* other) const { return id() < other->id(); }

    struct SorterByCURRHISTID
    {
        bool operator()(const CurrencyHistoryData& x, const CurrencyHistoryData& y)
        {
            return x.CURRHISTID < y.CURRHISTID;
        }
    };

    struct SorterByCURRENCYID
    {
        bool operator()(const CurrencyHistoryData& x, const CurrencyHistoryData& y)
        {
            return x.CURRENCYID < y.CURRENCYID;
        }
    };

    struct SorterByCURRDATE
    {
        bool operator()(const CurrencyHistoryData& x, const CurrencyHistoryData& y)
        {
            return x.CURRDATE < y.CURRDATE;
        }
    };

    struct SorterByCURRVALUE
    {
        bool operator()(const CurrencyHistoryData& x, const CurrencyHistoryData& y)
        {
            return x.CURRVALUE < y.CURRVALUE;
        }
    };

    struct SorterByCURRUPDTYPE
    {
        bool operator()(const CurrencyHistoryData& x, const CurrencyHistoryData& y)
        {
            return x.CURRUPDTYPE < y.CURRUPDTYPE;
        }
    };
};

inline CurrencyHistoryData::CurrencyHistoryData(wxSQLite3ResultSet& q)
{
    from_select_result(q);
}

inline void CurrencyHistoryData::to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const
{
    to_row().to_insert_stmt(stmt, id);
}

inline void CurrencyHistoryData::to_update_stmt(wxSQLite3Statement& stmt) const
{
    to_row().to_update_stmt(stmt);
}

inline CurrencyHistoryData& CurrencyHistoryData::from_select_result(wxSQLite3ResultSet& q)
{
    return from_row(CurrencyHistoryRow().from_select_result(q));
}

inline wxString CurrencyHistoryData::to_json() const
{
    return to_row().to_json();
}

inline void CurrencyHistoryData::as_json(PrettyWriter<StringBuffer>& json_writer) const
{
    to_row().as_json(json_writer);
}

inline row_t CurrencyHistoryData::to_html_row() const
{
    return to_row().to_html_row();
}

inline void CurrencyHistoryData::to_html_template(html_template& t) const
{
    to_row().to_html_template(t);
}

inline CurrencyHistoryData& CurrencyHistoryData::clone_from(const CurrencyHistoryData& other)
{
    *this = other;
    id(-1);
    return *this;
}
