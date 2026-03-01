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
// This is only sample code re-used from "table/CurrencyHistoryTable.cpp".

#include "CurrencyHistoryData.h"

CurrencyHistoryData::CurrencyHistoryData()
{
    CURRHISTID = -1;
    CURRENCYID = -1;
    CURRVALUE = 0.0;
    CURRUPDTYPE = -1;
}

// Convert CurrencyHistoryData to CurrencyHistoryRow
CurrencyHistoryRow CurrencyHistoryData::to_row() const
{
    CurrencyHistoryRow row;

    row.CURRHISTID = CURRHISTID;
    row.CURRENCYID = CURRENCYID;
    row.CURRDATE = CURRDATE;
    row.CURRVALUE = CURRVALUE;
    row.CURRUPDTYPE = CURRUPDTYPE;

    return row;
}

// Convert CurrencyHistoryRow to CurrencyHistoryData
CurrencyHistoryData& CurrencyHistoryData::from_row(const CurrencyHistoryRow& row)
{
    CURRHISTID = row.CURRHISTID; // int64
    CURRENCYID = row.CURRENCYID; // int64
    CURRDATE = row.CURRDATE; // wxString
    CURRVALUE = row.CURRVALUE; // double
    CURRUPDTYPE = row.CURRUPDTYPE; // int64

    return *this;
}

CurrencyHistoryData& CurrencyHistoryData::operator= (const CurrencyHistoryData& other)
{
    if (this == &other) return *this;

    CURRHISTID = other.CURRHISTID;
    CURRENCYID = other.CURRENCYID;
    CURRDATE = other.CURRDATE;
    CURRVALUE = other.CURRVALUE;
    CURRUPDTYPE = other.CURRUPDTYPE;

    return *this;
}

bool CurrencyHistoryData::equals(const CurrencyHistoryData* other) const
{
    if ( CURRHISTID != other->CURRHISTID) return false;
    if ( CURRENCYID != other->CURRENCYID) return false;
    if (!CURRDATE.IsSameAs(other->CURRDATE)) return false;
    if ( CURRVALUE != other->CURRVALUE) return false;
    if ( CURRUPDTYPE != other->CURRUPDTYPE) return false;

    return true;
}
