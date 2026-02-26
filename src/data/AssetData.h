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
#include "table/AssetTable.h"
#include "util/mmChoice.h"

struct AssetType
{
public:
    enum
    {
        e_property = 0,
        e_automobile,
        e_household,
        e_art,
        e_jewellery,
        e_cash,
        e_other,
        size
    };
    static mmChoiceNameA s_choice_a;

private:
    int m_id;

public:
    AssetType(int id = s_choice_a.default_id_n()) : m_id(s_choice_a.valid_id_n(id)) {}
    AssetType(const wxString& name) : m_id(AssetType::s_choice_a.find_name_n(name)) {}

    int id() const { return m_id; }
    const wxString name() const { return AssetType::s_choice_a.get_name(m_id); }
};

// User-friendly representation of a record in table ASSETS_V1.
struct AssetData
{
    int64    ASSETID;
    wxString ASSETTYPE;
    wxString ASSETSTATUS;
    wxString ASSETNAME;
    wxString STARTDATE;
    int64    CURRENCYID;
    double   VALUE;
    wxString VALUECHANGE;
    wxString VALUECHANGEMODE;
    double   VALUECHANGERATE;
    wxString NOTES;

    explicit AssetData();
    explicit AssetData(wxSQLite3ResultSet& q);
    AssetData(const AssetData& other) = default;

    int64 id() const { return ASSETID; }
    void id(const int64 id) { ASSETID = id; }
    AssetRow to_row() const;
    AssetData& from_row(const AssetRow& row);
    void to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const;
    void to_update_stmt(wxSQLite3Statement& stmt) const;
    AssetData& from_select_result(wxSQLite3ResultSet& q);
    wxString to_json() const;
    void as_json(PrettyWriter<StringBuffer>& json_writer) const;
    row_t to_html_row() const;
    void to_html_template(html_template& t) const;
    void destroy() { delete this; }

    AssetData& clone_from(const AssetData& other);
    bool equals(const AssetData* other) const;
    bool operator< (const AssetData& other) const { return id() < other.id(); }
    bool operator< (const AssetData* other) const { return id() < other->id(); }

    struct SorterByASSETID
    {
        bool operator()(const AssetData& x, const AssetData& y)
        {
            return x.ASSETID < y.ASSETID;
        }
    };

    struct SorterBySTARTDATE
    {
        bool operator()(const AssetData& x, const AssetData& y)
        {
            return x.STARTDATE < y.STARTDATE;
        }
    };

    struct SorterByASSETNAME
    {
        bool operator()(const AssetData& x, const AssetData& y)
        {
            return x.ASSETNAME < y.ASSETNAME;
        }
    };

    struct SorterByASSETSTATUS
    {
        bool operator()(const AssetData& x, const AssetData& y)
        {
            return x.ASSETSTATUS < y.ASSETSTATUS;
        }
    };

    struct SorterByCURRENCYID
    {
        bool operator()(const AssetData& x, const AssetData& y)
        {
            return x.CURRENCYID < y.CURRENCYID;
        }
    };

    struct SorterByVALUECHANGEMODE
    {
        bool operator()(const AssetData& x, const AssetData& y)
        {
            return x.VALUECHANGEMODE < y.VALUECHANGEMODE;
        }
    };

    struct SorterByVALUE
    {
        bool operator()(const AssetData& x, const AssetData& y)
        {
            return x.VALUE < y.VALUE;
        }
    };

    struct SorterByVALUECHANGE
    {
        bool operator()(const AssetData& x, const AssetData& y)
        {
            return x.VALUECHANGE < y.VALUECHANGE;
        }
    };

    struct SorterByNOTES
    {
        bool operator()(const AssetData& x, const AssetData& y)
        {
            return x.NOTES < y.NOTES;
        }
    };

    struct SorterByVALUECHANGERATE
    {
        bool operator()(const AssetData& x, const AssetData& y)
        {
            return x.VALUECHANGERATE < y.VALUECHANGERATE;
        }
    };

    struct SorterByASSETTYPE
    {
        bool operator()(const AssetData& x, const AssetData& y)
        {
            return x.ASSETTYPE < y.ASSETTYPE;
        }
    };
};

inline AssetData::AssetData(wxSQLite3ResultSet& q)
{
    from_select_result(q);
}

inline void AssetData::to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const
{
    to_row().to_insert_stmt(stmt, id);
}

inline void AssetData::to_update_stmt(wxSQLite3Statement& stmt) const
{
    to_row().to_update_stmt(stmt);
}

inline AssetData& AssetData::from_select_result(wxSQLite3ResultSet& q)
{
    return from_row(AssetRow().from_select_result(q));
}

inline wxString AssetData::to_json() const
{
    return to_row().to_json();
}

inline void AssetData::as_json(PrettyWriter<StringBuffer>& json_writer) const
{
    to_row().as_json(json_writer);
}

inline row_t AssetData::to_html_row() const
{
    return to_row().to_html_row();
}

inline void AssetData::to_html_template(html_template& t) const
{
    to_row().to_html_template(t);
}

inline AssetData& AssetData::clone_from(const AssetData& other)
{
    *this = other;
    id(-1);
    return *this;
}
