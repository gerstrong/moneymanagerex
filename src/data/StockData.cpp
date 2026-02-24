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
// This is only sample code re-used from "table/StockTable.cpp".

#include "StockData.h"

StockData::StockData()
{
    STOCKID = -1;
    HELDAT = -1;
    NUMSHARES = 0.0;
    PURCHASEPRICE = 0.0;
    CURRENTPRICE = 0.0;
    VALUE = 0.0;
    COMMISSION = 0.0;
}

// Convert StockData to StockRow
StockRow StockData::to_row() const
{
    StockRow row;

    row.STOCKID = STOCKID;
    row.HELDAT = HELDAT;
    row.PURCHASEDATE = PURCHASEDATE;
    row.STOCKNAME = STOCKNAME;
    row.SYMBOL = SYMBOL;
    row.NUMSHARES = NUMSHARES;
    row.PURCHASEPRICE = PURCHASEPRICE;
    row.NOTES = NOTES;
    row.CURRENTPRICE = CURRENTPRICE;
    row.VALUE = VALUE;
    row.COMMISSION = COMMISSION;

    return row;
}

// Convert StockRow to StockData
StockData& StockData::from_row(const StockRow& row)
{
    STOCKID = row.STOCKID; // int64
    HELDAT = row.HELDAT; // int64
    PURCHASEDATE = row.PURCHASEDATE; // wxString
    STOCKNAME = row.STOCKNAME; // wxString
    SYMBOL = row.SYMBOL; // wxString
    NUMSHARES = row.NUMSHARES; // double
    PURCHASEPRICE = row.PURCHASEPRICE; // double
    NOTES = row.NOTES; // wxString
    CURRENTPRICE = row.CURRENTPRICE; // double
    VALUE = row.VALUE; // double
    COMMISSION = row.COMMISSION; // double

    return *this;
}

bool StockData::equals(const StockData* other) const
{
    if ( STOCKID != other->STOCKID) return false;
    if ( HELDAT != other->HELDAT) return false;
    if (!PURCHASEDATE.IsSameAs(other->PURCHASEDATE)) return false;
    if (!STOCKNAME.IsSameAs(other->STOCKNAME)) return false;
    if (!SYMBOL.IsSameAs(other->SYMBOL)) return false;
    if ( NUMSHARES != other->NUMSHARES) return false;
    if ( PURCHASEPRICE != other->PURCHASEPRICE) return false;
    if (!NOTES.IsSameAs(other->NOTES)) return false;
    if ( CURRENTPRICE != other->CURRENTPRICE) return false;
    if ( VALUE != other->VALUE) return false;
    if ( COMMISSION != other->COMMISSION) return false;

    return true;
}
