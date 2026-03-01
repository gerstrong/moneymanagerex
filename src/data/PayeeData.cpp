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
// This is only sample code re-used from "table/PayeeTable.cpp".

#include "PayeeData.h"

PayeeData::PayeeData()
{
    PAYEEID = -1;
    CATEGID = -1;
    ACTIVE = -1;
}

// Convert PayeeData to PayeeRow
PayeeRow PayeeData::to_row() const
{
    PayeeRow row;

    row.PAYEEID = PAYEEID;
    row.PAYEENAME = PAYEENAME;
    row.CATEGID = CATEGID;
    row.NUMBER = NUMBER;
    row.WEBSITE = WEBSITE;
    row.NOTES = NOTES;
    row.ACTIVE = ACTIVE;
    row.PATTERN = PATTERN;

    return row;
}

// Convert PayeeRow to PayeeData
PayeeData& PayeeData::from_row(const PayeeRow& row)
{
    PAYEEID = row.PAYEEID; // int64
    PAYEENAME = row.PAYEENAME; // wxString
    CATEGID = row.CATEGID; // int64
    NUMBER = row.NUMBER; // wxString
    WEBSITE = row.WEBSITE; // wxString
    NOTES = row.NOTES; // wxString
    ACTIVE = row.ACTIVE; // int64
    PATTERN = row.PATTERN; // wxString

    return *this;
}

PayeeData& PayeeData::operator= (const PayeeData& other)
{
    if (this == &other) return *this;

    PAYEEID = other.PAYEEID;
    PAYEENAME = other.PAYEENAME;
    CATEGID = other.CATEGID;
    NUMBER = other.NUMBER;
    WEBSITE = other.WEBSITE;
    NOTES = other.NOTES;
    ACTIVE = other.ACTIVE;
    PATTERN = other.PATTERN;

    return *this;
}

bool PayeeData::equals(const PayeeData* other) const
{
    if ( PAYEEID != other->PAYEEID) return false;
    if (!PAYEENAME.IsSameAs(other->PAYEENAME)) return false;
    if ( CATEGID != other->CATEGID) return false;
    if (!NUMBER.IsSameAs(other->NUMBER)) return false;
    if (!WEBSITE.IsSameAs(other->WEBSITE)) return false;
    if (!NOTES.IsSameAs(other->NOTES)) return false;
    if ( ACTIVE != other->ACTIVE) return false;
    if (!PATTERN.IsSameAs(other->PATTERN)) return false;

    return true;
}
