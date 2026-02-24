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
// This is only sample code re-used from "table/TransactionTable.h".
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
#include "table/TransactionTable.h"

// User-friendly representation of a record in table CHECKINGACCOUNT_V1.
struct TransactionData
{
    int64 TRANSID; // primary key
    int64 ACCOUNTID;
    int64 TOACCOUNTID;
    int64 PAYEEID;
    wxString TRANSCODE;
    double TRANSAMOUNT;
    wxString STATUS;
    wxString TRANSACTIONNUMBER;
    wxString NOTES;
    int64 CATEGID;
    wxString TRANSDATE;
    wxString LASTUPDATEDTIME;
    wxString DELETEDTIME;
    int64 FOLLOWUPID;
    double TOTRANSAMOUNT;
    int64 COLOR;

    explicit TransactionData();
    explicit TransactionData(wxSQLite3ResultSet& q);
    TransactionData(const TransactionData& other) = default;

    int64 id() const { return TRANSID; }
    void id(const int64 id) { TRANSID = id; }
    TransactionRow to_row() const;
    TransactionData& from_row(const TransactionRow& row);
    void to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const;
    void to_update_stmt(wxSQLite3Statement& stmt) const;
    TransactionData& from_select_result(wxSQLite3ResultSet& q);
    wxString to_json() const;
    void as_json(PrettyWriter<StringBuffer>& json_writer) const;
    row_t to_html_row() const;
    void to_html_template(html_template& t) const;
    void destroy() { delete this; }

    TransactionData& clone_from(const TransactionData& other);
    bool equals(const TransactionData* other) const;
    bool operator< (const TransactionData& other) const { return id() < other.id(); }
    bool operator< (const TransactionData* other) const { return id() < other->id(); }

    struct SorterByTRANSID
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.TRANSID < y.TRANSID;
        }
    };

    struct SorterByACCOUNTID
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.ACCOUNTID < y.ACCOUNTID;
        }
    };

    struct SorterByTOACCOUNTID
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.TOACCOUNTID < y.TOACCOUNTID;
        }
    };

    struct SorterByPAYEEID
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.PAYEEID < y.PAYEEID;
        }
    };

    struct SorterByTRANSCODE
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.TRANSCODE < y.TRANSCODE;
        }
    };

    struct SorterByTRANSAMOUNT
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.TRANSAMOUNT < y.TRANSAMOUNT;
        }
    };

    struct SorterBySTATUS
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.STATUS < y.STATUS;
        }
    };

    struct SorterByTRANSACTIONNUMBER
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.TRANSACTIONNUMBER < y.TRANSACTIONNUMBER;
        }
    };

    struct SorterByNOTES
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.NOTES < y.NOTES;
        }
    };

    struct SorterByCATEGID
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.CATEGID < y.CATEGID;
        }
    };

    struct SorterByTRANSDATE
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.TRANSDATE < y.TRANSDATE;
        }
    };

    struct SorterByLASTUPDATEDTIME
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.LASTUPDATEDTIME < y.LASTUPDATEDTIME;
        }
    };

    struct SorterByDELETEDTIME
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.DELETEDTIME < y.DELETEDTIME;
        }
    };

    struct SorterByFOLLOWUPID
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.FOLLOWUPID < y.FOLLOWUPID;
        }
    };

    struct SorterByTOTRANSAMOUNT
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.TOTRANSAMOUNT < y.TOTRANSAMOUNT;
        }
    };

    struct SorterByCOLOR
    {
        bool operator()(const TransactionData& x, const TransactionData& y)
        {
            return x.COLOR < y.COLOR;
        }
    };
};

inline TransactionData::TransactionData(wxSQLite3ResultSet& q)
{
    from_select_result(q);
}

inline void TransactionData::to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const
{
    to_row().to_insert_stmt(stmt, id);
}

inline void TransactionData::to_update_stmt(wxSQLite3Statement& stmt) const
{
    to_row().to_update_stmt(stmt);
}

inline TransactionData& TransactionData::from_select_result(wxSQLite3ResultSet& q)
{
    return from_row(TransactionRow().from_select_result(q));
}

inline wxString TransactionData::to_json() const
{
    return to_row().to_json();
}

inline void TransactionData::as_json(PrettyWriter<StringBuffer>& json_writer) const
{
    to_row().as_json(json_writer);
}

inline row_t TransactionData::to_html_row() const
{
    return to_row().to_html_row();
}

inline void TransactionData::to_html_template(html_template& t) const
{
    to_row().to_html_template(t);
}

inline TransactionData& TransactionData::clone_from(const TransactionData& other)
{
    *this = other;
    id(-1);
    return *this;
}
