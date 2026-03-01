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
// This is only sample code re-used from "table/ReportTable.cpp".

#include "ReportData.h"

ReportData::ReportData()
{
    REPORTID = -1;
    ACTIVE = -1;
}

// Convert ReportData to ReportRow
ReportRow ReportData::to_row() const
{
    ReportRow row;

    row.REPORTID = REPORTID;
    row.REPORTNAME = REPORTNAME;
    row.GROUPNAME = GROUPNAME;
    row.ACTIVE = ACTIVE;
    row.SQLCONTENT = SQLCONTENT;
    row.LUACONTENT = LUACONTENT;
    row.TEMPLATECONTENT = TEMPLATECONTENT;
    row.DESCRIPTION = DESCRIPTION;

    return row;
}

// Convert ReportRow to ReportData
ReportData& ReportData::from_row(const ReportRow& row)
{
    REPORTID = row.REPORTID; // int64
    REPORTNAME = row.REPORTNAME; // wxString
    GROUPNAME = row.GROUPNAME; // wxString
    ACTIVE = row.ACTIVE; // int64
    SQLCONTENT = row.SQLCONTENT; // wxString
    LUACONTENT = row.LUACONTENT; // wxString
    TEMPLATECONTENT = row.TEMPLATECONTENT; // wxString
    DESCRIPTION = row.DESCRIPTION; // wxString

    return *this;
}

ReportData& ReportData::operator= (const ReportData& other)
{
    if (this == &other) return *this;

    REPORTID = other.REPORTID;
    REPORTNAME = other.REPORTNAME;
    GROUPNAME = other.GROUPNAME;
    ACTIVE = other.ACTIVE;
    SQLCONTENT = other.SQLCONTENT;
    LUACONTENT = other.LUACONTENT;
    TEMPLATECONTENT = other.TEMPLATECONTENT;
    DESCRIPTION = other.DESCRIPTION;

    return *this;
}

bool ReportData::equals(const ReportData* other) const
{
    if ( REPORTID != other->REPORTID) return false;
    if (!REPORTNAME.IsSameAs(other->REPORTNAME)) return false;
    if (!GROUPNAME.IsSameAs(other->GROUPNAME)) return false;
    if ( ACTIVE != other->ACTIVE) return false;
    if (!SQLCONTENT.IsSameAs(other->SQLCONTENT)) return false;
    if (!LUACONTENT.IsSameAs(other->LUACONTENT)) return false;
    if (!TEMPLATECONTENT.IsSameAs(other->TEMPLATECONTENT)) return false;
    if (!DESCRIPTION.IsSameAs(other->DESCRIPTION)) return false;

    return true;
}
