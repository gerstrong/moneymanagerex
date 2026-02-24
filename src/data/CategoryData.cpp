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
// This is only sample code re-used from "table/CategoryTable.cpp".

#include "CategoryData.h"

CategoryData::CategoryData()
{
    CATEGID = -1;
    ACTIVE = -1;
    PARENTID = -1;
}

// Convert CategoryData to CategoryRow
CategoryRow CategoryData::to_row() const
{
    CategoryRow row;

    row.CATEGID = CATEGID;
    row.CATEGNAME = CATEGNAME;
    row.ACTIVE = ACTIVE;
    row.PARENTID = PARENTID;

    return row;
}

// Convert CategoryRow to CategoryData
CategoryData& CategoryData::from_row(const CategoryRow& row)
{
    CATEGID = row.CATEGID; // int64
    CATEGNAME = row.CATEGNAME; // wxString
    ACTIVE = row.ACTIVE; // int64
    PARENTID = row.PARENTID; // int64

    return *this;
}

bool CategoryData::equals(const CategoryData* other) const
{
    if ( CATEGID != other->CATEGID) return false;
    if (!CATEGNAME.IsSameAs(other->CATEGNAME)) return false;
    if ( ACTIVE != other->ACTIVE) return false;
    if ( PARENTID != other->PARENTID) return false;

    return true;
}
