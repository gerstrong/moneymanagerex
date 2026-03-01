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
// This is only sample code re-used from "table/UsageTable.cpp".

#include "UsageData.h"

UsageData::UsageData()
{
    USAGEID = -1;
}

// Convert UsageData to UsageRow
UsageRow UsageData::to_row() const
{
    UsageRow row;

    row.USAGEID = USAGEID;
    row.USAGEDATE = USAGEDATE;
    row.JSONCONTENT = JSONCONTENT;

    return row;
}

// Convert UsageRow to UsageData
UsageData& UsageData::from_row(const UsageRow& row)
{
    USAGEID = row.USAGEID; // int64
    USAGEDATE = row.USAGEDATE; // wxString
    JSONCONTENT = row.JSONCONTENT; // wxString

    return *this;
}

UsageData& UsageData::operator= (const UsageData& other)
{
    if (this == &other) return *this;

    USAGEID = other.USAGEID;
    USAGEDATE = other.USAGEDATE;
    JSONCONTENT = other.JSONCONTENT;

    return *this;
}

bool UsageData::equals(const UsageData* other) const
{
    if ( USAGEID != other->USAGEID) return false;
    if (!USAGEDATE.IsSameAs(other->USAGEDATE)) return false;
    if (!JSONCONTENT.IsSameAs(other->JSONCONTENT)) return false;

    return true;
}
