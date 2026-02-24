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
// This is only sample code re-used from "table/SettingTable.cpp".

#include "SettingData.h"

SettingData::SettingData()
{
    SETTINGID = -1;
}

// Convert SettingData to SettingRow
SettingRow SettingData::to_row() const
{
    SettingRow row;

    row.SETTINGID = SETTINGID;
    row.SETTINGNAME = SETTINGNAME;
    row.SETTINGVALUE = SETTINGVALUE;

    return row;
}

// Convert SettingRow to SettingData
SettingData& SettingData::from_row(const SettingRow& row)
{
    SETTINGID = row.SETTINGID; // int64
    SETTINGNAME = row.SETTINGNAME; // wxString
    SETTINGVALUE = row.SETTINGVALUE; // wxString

    return *this;
}

bool SettingData::equals(const SettingData* other) const
{
    if ( SETTINGID != other->SETTINGID) return false;
    if (!SETTINGNAME.IsSameAs(other->SETTINGNAME)) return false;
    if (!SETTINGVALUE.IsSameAs(other->SETTINGVALUE)) return false;

    return true;
}
