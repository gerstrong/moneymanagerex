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
// This is only sample code re-used from "table/ScheduledSplitTable.cpp".

#include "ScheduledSplitData.h"

ScheduledSplitData::ScheduledSplitData()
{
    SPLITTRANSID = -1;
    TRANSID = -1;
    CATEGID = -1;
    SPLITTRANSAMOUNT = 0.0;
}

// Convert ScheduledSplitData to ScheduledSplitRow
ScheduledSplitRow ScheduledSplitData::to_row() const
{
    ScheduledSplitRow row;

    row.SPLITTRANSID = SPLITTRANSID;
    row.TRANSID = TRANSID;
    row.CATEGID = CATEGID;
    row.SPLITTRANSAMOUNT = SPLITTRANSAMOUNT;
    row.NOTES = NOTES;

    return row;
}

// Convert ScheduledSplitRow to ScheduledSplitData
ScheduledSplitData& ScheduledSplitData::from_row(const ScheduledSplitRow& row)
{
    SPLITTRANSID = row.SPLITTRANSID; // int64
    TRANSID = row.TRANSID; // int64
    CATEGID = row.CATEGID; // int64
    SPLITTRANSAMOUNT = row.SPLITTRANSAMOUNT; // double
    NOTES = row.NOTES; // wxString

    return *this;
}

bool ScheduledSplitData::equals(const ScheduledSplitData* other) const
{
    if ( SPLITTRANSID != other->SPLITTRANSID) return false;
    if ( TRANSID != other->TRANSID) return false;
    if ( CATEGID != other->CATEGID) return false;
    if ( SPLITTRANSAMOUNT != other->SPLITTRANSAMOUNT) return false;
    if (!NOTES.IsSameAs(other->NOTES)) return false;

    return true;
}
