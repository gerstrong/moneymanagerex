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
// This is only sample code re-used from "table/AssetTable.cpp".

#include "AssetData.h"

AssetData::AssetData()
{
    ASSETID = -1;
    CURRENCYID = -1;
    VALUE = 0.0;
    VALUECHANGERATE = 0.0;
}

// Convert AssetData to AssetRow
AssetRow AssetData::to_row() const
{
    AssetRow row;

    row.ASSETID = ASSETID;
    row.STARTDATE = STARTDATE;
    row.ASSETNAME = ASSETNAME;
    row.ASSETSTATUS = ASSETSTATUS;
    row.CURRENCYID = CURRENCYID;
    row.VALUECHANGEMODE = VALUECHANGEMODE;
    row.VALUE = VALUE;
    row.VALUECHANGE = VALUECHANGE;
    row.NOTES = NOTES;
    row.VALUECHANGERATE = VALUECHANGERATE;
    row.ASSETTYPE = ASSETTYPE;

    return row;
}

// Convert AssetRow to AssetData
AssetData& AssetData::from_row(const AssetRow& row)
{
    ASSETID = row.ASSETID; // int64
    STARTDATE = row.STARTDATE; // wxString
    ASSETNAME = row.ASSETNAME; // wxString
    ASSETSTATUS = row.ASSETSTATUS; // wxString
    CURRENCYID = row.CURRENCYID; // int64
    VALUECHANGEMODE = row.VALUECHANGEMODE; // wxString
    VALUE = row.VALUE; // double
    VALUECHANGE = row.VALUECHANGE; // wxString
    NOTES = row.NOTES; // wxString
    VALUECHANGERATE = row.VALUECHANGERATE; // double
    ASSETTYPE = row.ASSETTYPE; // wxString

    return *this;
}

AssetData& AssetData::operator= (const AssetData& other)
{
    if (this == &other) return *this;

    ASSETID = other.ASSETID;
    STARTDATE = other.STARTDATE;
    ASSETNAME = other.ASSETNAME;
    ASSETSTATUS = other.ASSETSTATUS;
    CURRENCYID = other.CURRENCYID;
    VALUECHANGEMODE = other.VALUECHANGEMODE;
    VALUE = other.VALUE;
    VALUECHANGE = other.VALUECHANGE;
    NOTES = other.NOTES;
    VALUECHANGERATE = other.VALUECHANGERATE;
    ASSETTYPE = other.ASSETTYPE;

    return *this;
}

bool AssetData::equals(const AssetData* other) const
{
    if ( ASSETID != other->ASSETID) return false;
    if (!STARTDATE.IsSameAs(other->STARTDATE)) return false;
    if (!ASSETNAME.IsSameAs(other->ASSETNAME)) return false;
    if (!ASSETSTATUS.IsSameAs(other->ASSETSTATUS)) return false;
    if ( CURRENCYID != other->CURRENCYID) return false;
    if (!VALUECHANGEMODE.IsSameAs(other->VALUECHANGEMODE)) return false;
    if ( VALUE != other->VALUE) return false;
    if (!VALUECHANGE.IsSameAs(other->VALUECHANGE)) return false;
    if (!NOTES.IsSameAs(other->NOTES)) return false;
    if ( VALUECHANGERATE != other->VALUECHANGERATE) return false;
    if (!ASSETTYPE.IsSameAs(other->ASSETTYPE)) return false;

    return true;
}
