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
// This is only sample code re-used from "table/BudgetPeriodTable.cpp".

#include "BudgetPeriodData.h"

BudgetPeriodData::BudgetPeriodData()
{
    BUDGETYEARID = -1;
}

// Convert BudgetPeriodData to BudgetPeriodRow
BudgetPeriodRow BudgetPeriodData::to_row() const
{
    BudgetPeriodRow row;

    row.BUDGETYEARID = BUDGETYEARID;
    row.BUDGETYEARNAME = BUDGETYEARNAME;

    return row;
}

// Convert BudgetPeriodRow to BudgetPeriodData
BudgetPeriodData& BudgetPeriodData::from_row(const BudgetPeriodRow& row)
{
    BUDGETYEARID = row.BUDGETYEARID; // int64
    BUDGETYEARNAME = row.BUDGETYEARNAME; // wxString

    return *this;
}

BudgetPeriodData& BudgetPeriodData::operator= (const BudgetPeriodData& other)
{
    if (this == &other) return *this;

    BUDGETYEARID = other.BUDGETYEARID;
    BUDGETYEARNAME = other.BUDGETYEARNAME;

    return *this;
}

bool BudgetPeriodData::equals(const BudgetPeriodData* other) const
{
    if ( BUDGETYEARID != other->BUDGETYEARID) return false;
    if (!BUDGETYEARNAME.IsSameAs(other->BUDGETYEARNAME)) return false;

    return true;
}
