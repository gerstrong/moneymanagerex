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
// This is only sample code re-used from "table/CurrencyTable.h".
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
#include "table/CurrencyTable.h"

// User-friendly representation of a record in table CURRENCYFORMATS_V1.
struct CurrencyData
{
    int64 CURRENCYID; // primary key
    wxString CURRENCYNAME;
    wxString PFX_SYMBOL;
    wxString SFX_SYMBOL;
    wxString DECIMAL_POINT;
    wxString GROUP_SEPARATOR;
    wxString UNIT_NAME;
    wxString CENT_NAME;
    int64 SCALE;
    double BASECONVRATE;
    wxString CURRENCY_SYMBOL;
    wxString CURRENCY_TYPE;

    explicit CurrencyData();
    explicit CurrencyData(wxSQLite3ResultSet& q);
    CurrencyData(const CurrencyData& other) = default;

    int64 id() const { return CURRENCYID; }
    void id(const int64 id) { CURRENCYID = id; }
    CurrencyRow to_row() const;
    CurrencyData& from_row(const CurrencyRow& row);
    void to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const;
    void to_update_stmt(wxSQLite3Statement& stmt) const;
    CurrencyData& from_select_result(wxSQLite3ResultSet& q);
    wxString to_json() const;
    void as_json(PrettyWriter<StringBuffer>& json_writer) const;
    row_t to_html_row() const;
    void to_html_template(html_template& t) const;
    void destroy() { delete this; }

    CurrencyData& clone_from(const CurrencyData& other);
    bool equals(const CurrencyData* other) const;
    bool operator< (const CurrencyData& other) const { return id() < other.id(); }
    bool operator< (const CurrencyData* other) const { return id() < other->id(); }

    struct SorterByCURRENCYID
    {
        bool operator()(const CurrencyData& x, const CurrencyData& y)
        {
            return x.CURRENCYID < y.CURRENCYID;
        }
    };

    struct SorterByCURRENCYNAME
    {
        bool operator()(const CurrencyData& x, const CurrencyData& y)
        {
            return wxGetTranslation(x.CURRENCYNAME) < wxGetTranslation(y.CURRENCYNAME);
        }
    };

    struct SorterByPFX_SYMBOL
    {
        bool operator()(const CurrencyData& x, const CurrencyData& y)
        {
            return x.PFX_SYMBOL < y.PFX_SYMBOL;
        }
    };

    struct SorterBySFX_SYMBOL
    {
        bool operator()(const CurrencyData& x, const CurrencyData& y)
        {
            return x.SFX_SYMBOL < y.SFX_SYMBOL;
        }
    };

    struct SorterByDECIMAL_POINT
    {
        bool operator()(const CurrencyData& x, const CurrencyData& y)
        {
            return x.DECIMAL_POINT < y.DECIMAL_POINT;
        }
    };

    struct SorterByGROUP_SEPARATOR
    {
        bool operator()(const CurrencyData& x, const CurrencyData& y)
        {
            return x.GROUP_SEPARATOR < y.GROUP_SEPARATOR;
        }
    };

    struct SorterByUNIT_NAME
    {
        bool operator()(const CurrencyData& x, const CurrencyData& y)
        {
            return x.UNIT_NAME < y.UNIT_NAME;
        }
    };

    struct SorterByCENT_NAME
    {
        bool operator()(const CurrencyData& x, const CurrencyData& y)
        {
            return x.CENT_NAME < y.CENT_NAME;
        }
    };

    struct SorterBySCALE
    {
        bool operator()(const CurrencyData& x, const CurrencyData& y)
        {
            return x.SCALE < y.SCALE;
        }
    };

    struct SorterByBASECONVRATE
    {
        bool operator()(const CurrencyData& x, const CurrencyData& y)
        {
            return x.BASECONVRATE < y.BASECONVRATE;
        }
    };

    struct SorterByCURRENCY_SYMBOL
    {
        bool operator()(const CurrencyData& x, const CurrencyData& y)
        {
            return x.CURRENCY_SYMBOL < y.CURRENCY_SYMBOL;
        }
    };

    struct SorterByCURRENCY_TYPE
    {
        bool operator()(const CurrencyData& x, const CurrencyData& y)
        {
            return x.CURRENCY_TYPE < y.CURRENCY_TYPE;
        }
    };
};

inline CurrencyData::CurrencyData(wxSQLite3ResultSet& q)
{
    from_select_result(q);
}

inline void CurrencyData::to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const
{
    to_row().to_insert_stmt(stmt, id);
}

inline void CurrencyData::to_update_stmt(wxSQLite3Statement& stmt) const
{
    to_row().to_update_stmt(stmt);
}

inline CurrencyData& CurrencyData::from_select_result(wxSQLite3ResultSet& q)
{
    return from_row(CurrencyRow().from_select_result(q));
}

inline wxString CurrencyData::to_json() const
{
    return to_row().to_json();
}

inline void CurrencyData::as_json(PrettyWriter<StringBuffer>& json_writer) const
{
    to_row().as_json(json_writer);
}

inline row_t CurrencyData::to_html_row() const
{
    return to_row().to_html_row();
}

inline void CurrencyData::to_html_template(html_template& t) const
{
    to_row().to_html_template(t);
}

inline CurrencyData& CurrencyData::clone_from(const CurrencyData& other)
{
    *this = other;
    id(-1);
    return *this;
}
