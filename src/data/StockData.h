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
// This is only sample code re-used from "table/StockTable.h".
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
#include "table/StockTable.h"

// User-friendly representation of a record in table STOCK_V1.
struct StockData
{
    int64 STOCKID; // primary key
    int64 HELDAT;
    wxString PURCHASEDATE;
    wxString STOCKNAME;
    wxString SYMBOL;
    double NUMSHARES;
    double PURCHASEPRICE;
    wxString NOTES;
    double CURRENTPRICE;
    double VALUE;
    double COMMISSION;

    explicit StockData();
    explicit StockData(wxSQLite3ResultSet& q);
    StockData(const StockData& other) = default;

    int64 id() const { return STOCKID; }
    void id(const int64 id) { STOCKID = id; }
    StockRow to_row() const;
    StockData& from_row(const StockRow& row);
    void to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const;
    void to_update_stmt(wxSQLite3Statement& stmt) const;
    StockData& from_select_result(wxSQLite3ResultSet& q);
    wxString to_json() const;
    void as_json(PrettyWriter<StringBuffer>& json_writer) const;
    row_t to_html_row() const;
    void to_html_template(html_template& t) const;
    void destroy() { delete this; }

    StockData& operator= (const StockData& other);
    StockData& clone_from(const StockData& other);
    bool equals(const StockData* other) const;
    bool operator< (const StockData& other) const { return id() < other.id(); }
    bool operator< (const StockData* other) const { return id() < other->id(); }

    struct SorterBySTOCKID
    {
        bool operator()(const StockData& x, const StockData& y)
        {
            return x.STOCKID < y.STOCKID;
        }
    };

    struct SorterByHELDAT
    {
        bool operator()(const StockData& x, const StockData& y)
        {
            return x.HELDAT < y.HELDAT;
        }
    };

    struct SorterByPURCHASEDATE
    {
        bool operator()(const StockData& x, const StockData& y)
        {
            return x.PURCHASEDATE < y.PURCHASEDATE;
        }
    };

    struct SorterBySTOCKNAME
    {
        bool operator()(const StockData& x, const StockData& y)
        {
            return x.STOCKNAME < y.STOCKNAME;
        }
    };

    struct SorterBySYMBOL
    {
        bool operator()(const StockData& x, const StockData& y)
        {
            return x.SYMBOL < y.SYMBOL;
        }
    };

    struct SorterByNUMSHARES
    {
        bool operator()(const StockData& x, const StockData& y)
        {
            return x.NUMSHARES < y.NUMSHARES;
        }
    };

    struct SorterByPURCHASEPRICE
    {
        bool operator()(const StockData& x, const StockData& y)
        {
            return x.PURCHASEPRICE < y.PURCHASEPRICE;
        }
    };

    struct SorterByNOTES
    {
        bool operator()(const StockData& x, const StockData& y)
        {
            return x.NOTES < y.NOTES;
        }
    };

    struct SorterByCURRENTPRICE
    {
        bool operator()(const StockData& x, const StockData& y)
        {
            return x.CURRENTPRICE < y.CURRENTPRICE;
        }
    };

    struct SorterByVALUE
    {
        bool operator()(const StockData& x, const StockData& y)
        {
            return x.VALUE < y.VALUE;
        }
    };

    struct SorterByCOMMISSION
    {
        bool operator()(const StockData& x, const StockData& y)
        {
            return x.COMMISSION < y.COMMISSION;
        }
    };
};

inline StockData::StockData(wxSQLite3ResultSet& q)
{
    from_select_result(q);
}

inline void StockData::to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const
{
    to_row().to_insert_stmt(stmt, id);
}

inline void StockData::to_update_stmt(wxSQLite3Statement& stmt) const
{
    to_row().to_update_stmt(stmt);
}

inline StockData& StockData::from_select_result(wxSQLite3ResultSet& q)
{
    return from_row(StockRow().from_select_result(q));
}

inline wxString StockData::to_json() const
{
    to_row().to_json();
}

inline void StockData::as_json(PrettyWriter<StringBuffer>& json_writer) const
{
    to_row().as_json(json_writer);
}

inline row_t StockData::to_html_row() const
{
    return to_row().to_html_row();
}

inline void StockData::to_html_template(html_template& t) const
{
    to_row().to_html_template(t);
}

inline StockData& StockData::clone_from(const StockData& other)
{
    *this = other;
    id(-1);
    return *this;
}
