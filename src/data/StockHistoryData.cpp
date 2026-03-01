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
// This is only sample code re-used from "table/StockHistoryTable.cpp".

#include "StockHistoryData.h"

StockHistoryData::StockHistoryData()
{
    HISTID = -1;
    VALUE = 0.0;
    UPDTYPE = -1;
}

// Convert StockHistoryData to StockHistoryRow
StockHistoryRow StockHistoryData::to_row() const
{
    StockHistoryRow row;

    row.HISTID = HISTID;
    row.SYMBOL = SYMBOL;
    row.DATE = DATE;
    row.VALUE = VALUE;
    row.UPDTYPE = UPDTYPE;

    return row;
}

// Convert StockHistoryRow to StockHistoryData
StockHistoryData& StockHistoryData::from_row(const StockHistoryRow& row)
{
    HISTID = row.HISTID; // int64
    SYMBOL = row.SYMBOL; // wxString
    DATE = row.DATE; // wxString
    VALUE = row.VALUE; // double
    UPDTYPE = row.UPDTYPE; // int64

    return *this;
}

StockHistoryData& StockHistoryData::operator= (const StockHistoryData& other)
{
    if (this == &other) return *this;

    HISTID = other.HISTID;
    SYMBOL = other.SYMBOL;
    DATE = other.DATE;
    VALUE = other.VALUE;
    UPDTYPE = other.UPDTYPE;

    return *this;
}

bool StockHistoryData::equals(const StockHistoryData* other) const
{
    if ( HISTID != other->HISTID) return false;
    if (!SYMBOL.IsSameAs(other->SYMBOL)) return false;
    if (!DATE.IsSameAs(other->DATE)) return false;
    if ( VALUE != other->VALUE) return false;
    if ( UPDTYPE != other->UPDTYPE) return false;

    return true;
}
