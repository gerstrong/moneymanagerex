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
// This is only sample code re-used from "table/ScheduledSplitTable.h".
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
#include "table/ScheduledSplitTable.h"

// User-friendly representation of a record in table BUDGETSPLITTRANSACTIONS_V1.
struct ScheduledSplitData
{
    int64 SPLITTRANSID; // primary key
    int64 TRANSID;
    int64 CATEGID;
    double SPLITTRANSAMOUNT;
    wxString NOTES;

    explicit ScheduledSplitData();
    explicit ScheduledSplitData(wxSQLite3ResultSet& q);
    ScheduledSplitData(const ScheduledSplitData& other) = default;

    int64 id() const { return SPLITTRANSID; }
    void id(const int64 id) { SPLITTRANSID = id; }
    ScheduledSplitRow to_row() const;
    ScheduledSplitData& from_row(const ScheduledSplitRow& row);
    void to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const;
    void to_update_stmt(wxSQLite3Statement& stmt) const;
    ScheduledSplitData& from_select_result(wxSQLite3ResultSet& q);
    wxString to_json() const;
    void as_json(PrettyWriter<StringBuffer>& json_writer) const;
    row_t to_html_row() const;
    void to_html_template(html_template& t) const;
    void destroy() { delete this; }

    ScheduledSplitData& operator= (const ScheduledSplitData& other);
    ScheduledSplitData& clone_from(const ScheduledSplitData& other);
    bool equals(const ScheduledSplitData* other) const;
    bool operator< (const ScheduledSplitData& other) const { return id() < other.id(); }
    bool operator< (const ScheduledSplitData* other) const { return id() < other->id(); }

    struct SorterBySPLITTRANSID
    {
        bool operator()(const ScheduledSplitData& x, const ScheduledSplitData& y)
        {
            return x.SPLITTRANSID < y.SPLITTRANSID;
        }
    };

    struct SorterByTRANSID
    {
        bool operator()(const ScheduledSplitData& x, const ScheduledSplitData& y)
        {
            return x.TRANSID < y.TRANSID;
        }
    };

    struct SorterByCATEGID
    {
        bool operator()(const ScheduledSplitData& x, const ScheduledSplitData& y)
        {
            return x.CATEGID < y.CATEGID;
        }
    };

    struct SorterBySPLITTRANSAMOUNT
    {
        bool operator()(const ScheduledSplitData& x, const ScheduledSplitData& y)
        {
            return x.SPLITTRANSAMOUNT < y.SPLITTRANSAMOUNT;
        }
    };

    struct SorterByNOTES
    {
        bool operator()(const ScheduledSplitData& x, const ScheduledSplitData& y)
        {
            return x.NOTES < y.NOTES;
        }
    };
};

inline ScheduledSplitData::ScheduledSplitData(wxSQLite3ResultSet& q)
{
    from_select_result(q);
}

inline void ScheduledSplitData::to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const
{
    to_row().to_insert_stmt(stmt, id);
}

inline void ScheduledSplitData::to_update_stmt(wxSQLite3Statement& stmt) const
{
    to_row().to_update_stmt(stmt);
}

inline ScheduledSplitData& ScheduledSplitData::from_select_result(wxSQLite3ResultSet& q)
{
    return from_row(ScheduledSplitRow().from_select_result(q));
}

inline wxString ScheduledSplitData::to_json() const
{
    return to_row().to_json();
}

inline void ScheduledSplitData::as_json(PrettyWriter<StringBuffer>& json_writer) const
{
    to_row().as_json(json_writer);
}

inline row_t ScheduledSplitData::to_html_row() const
{
    return to_row().to_html_row();
}

inline void ScheduledSplitData::to_html_template(html_template& t) const
{
    to_row().to_html_template(t);
}

inline ScheduledSplitData& ScheduledSplitData::clone_from(const ScheduledSplitData& other)
{
    *this = other;
    id(-1);
    return *this;
}
