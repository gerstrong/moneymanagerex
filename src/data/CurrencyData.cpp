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
// This is only sample code re-used from "table/CurrencyTable.cpp".

#include "CurrencyData.h"

CurrencyData::CurrencyData()
{
    CURRENCYID = -1;
    SCALE = -1;
    BASECONVRATE = 0.0;
}

// Convert CurrencyData to CurrencyRow
CurrencyRow CurrencyData::to_row() const
{
    CurrencyRow row;

    row.CURRENCYID = CURRENCYID;
    row.CURRENCYNAME = CURRENCYNAME;
    row.PFX_SYMBOL = PFX_SYMBOL;
    row.SFX_SYMBOL = SFX_SYMBOL;
    row.DECIMAL_POINT = DECIMAL_POINT;
    row.GROUP_SEPARATOR = GROUP_SEPARATOR;
    row.UNIT_NAME = UNIT_NAME;
    row.CENT_NAME = CENT_NAME;
    row.SCALE = SCALE;
    row.BASECONVRATE = BASECONVRATE;
    row.CURRENCY_SYMBOL = CURRENCY_SYMBOL;
    row.CURRENCY_TYPE = CURRENCY_TYPE;

    return row;
}

// Convert CurrencyRow to CurrencyData
CurrencyData& CurrencyData::from_row(const CurrencyRow& row)
{
    CURRENCYID = row.CURRENCYID; // int64
    CURRENCYNAME = row.CURRENCYNAME; // wxString
    PFX_SYMBOL = row.PFX_SYMBOL; // wxString
    SFX_SYMBOL = row.SFX_SYMBOL; // wxString
    DECIMAL_POINT = row.DECIMAL_POINT; // wxString
    GROUP_SEPARATOR = row.GROUP_SEPARATOR; // wxString
    UNIT_NAME = row.UNIT_NAME; // wxString
    CENT_NAME = row.CENT_NAME; // wxString
    SCALE = row.SCALE; // int64
    BASECONVRATE = row.BASECONVRATE; // double
    CURRENCY_SYMBOL = row.CURRENCY_SYMBOL; // wxString
    CURRENCY_TYPE = row.CURRENCY_TYPE; // wxString

    return *this;
}

bool CurrencyData::equals(const CurrencyData* other) const
{
    if ( CURRENCYID != other->CURRENCYID) return false;
    if (!CURRENCYNAME.IsSameAs(other->CURRENCYNAME)) return false;
    if (!PFX_SYMBOL.IsSameAs(other->PFX_SYMBOL)) return false;
    if (!SFX_SYMBOL.IsSameAs(other->SFX_SYMBOL)) return false;
    if (!DECIMAL_POINT.IsSameAs(other->DECIMAL_POINT)) return false;
    if (!GROUP_SEPARATOR.IsSameAs(other->GROUP_SEPARATOR)) return false;
    if (!UNIT_NAME.IsSameAs(other->UNIT_NAME)) return false;
    if (!CENT_NAME.IsSameAs(other->CENT_NAME)) return false;
    if ( SCALE != other->SCALE) return false;
    if ( BASECONVRATE != other->BASECONVRATE) return false;
    if (!CURRENCY_SYMBOL.IsSameAs(other->CURRENCY_SYMBOL)) return false;
    if (!CURRENCY_TYPE.IsSameAs(other->CURRENCY_TYPE)) return false;

    return true;
}
