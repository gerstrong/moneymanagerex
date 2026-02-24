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
// This is only sample code re-used from "table/BudgetTable.cpp".

#include "BudgetData.h"

BudgetData::BudgetData()
{
    BUDGETENTRYID = -1;
    BUDGETYEARID = -1;
    CATEGID = -1;
    AMOUNT = 0.0;
    ACTIVE = -1;
}

// Convert BudgetData to BudgetRow
BudgetRow BudgetData::to_row() const
{
    BudgetRow row;

    row.BUDGETENTRYID = BUDGETENTRYID;
    row.BUDGETYEARID = BUDGETYEARID;
    row.CATEGID = CATEGID;
    row.PERIOD = PERIOD;
    row.AMOUNT = AMOUNT;
    row.NOTES = NOTES;
    row.ACTIVE = ACTIVE;

    return row;
}

// Convert BudgetRow to BudgetData
BudgetData& BudgetData::from_row(const BudgetRow& row)
{
    BUDGETENTRYID = row.BUDGETENTRYID; // int64
    BUDGETYEARID = row.BUDGETYEARID; // int64
    CATEGID = row.CATEGID; // int64
    PERIOD = row.PERIOD; // wxString
    AMOUNT = row.AMOUNT; // double
    NOTES = row.NOTES; // wxString
    ACTIVE = row.ACTIVE; // int64

    return *this;
}

bool BudgetData::equals(const BudgetData* other) const
{
    if ( BUDGETENTRYID != other->BUDGETENTRYID) return false;
    if ( BUDGETYEARID != other->BUDGETYEARID) return false;
    if ( CATEGID != other->CATEGID) return false;
    if (!PERIOD.IsSameAs(other->PERIOD)) return false;
    if ( AMOUNT != other->AMOUNT) return false;
    if (!NOTES.IsSameAs(other->NOTES)) return false;
    if ( ACTIVE != other->ACTIVE) return false;

    return true;
}
