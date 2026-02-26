/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)
 Copyright (C) 2022 Mark Whalley (mark@ipx.co.uk)

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

#pragma once

#include "base/defs.h"
#include "util/mmChoice.h"

#include "table/AssetTable.h"
#include "data/AssetData.h"

#include "_ModelBase.h"
#include "CurrencyModel.h"

class AssetModel : public TableFactory<AssetTable, AssetData>
{
public:
    static const wxString refTypeName;

public:
    /**
    Initialize the global AssetModel table on initial call.
    Resets the global table on subsequent calls.
    * Return the static instance address for AssetModel table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static AssetModel& instance(wxSQLite3Database* db);

    /**
    * Return the static instance address for AssetModel table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static AssetModel& instance();

public:
    static AssetCol::ASSETTYPE ASSETTYPE(OP op, AssetType type);
    static AssetCol::STARTDATE STARTDATE(OP op, const wxDate& date);
    
public:
    static wxString get_asset_name(int64 asset_id);
    static wxDate STARTDATE(const Data& r);

    // Returns the base currency Data record pointer
    static const CurrencyData* currency(const Data* /* r */);
    // Returns the calculated current value
    static std::pair<double, double> value(const Data* r);
    // Returns the calculated current value
    static std::pair<double, double> value(const Data& r);

public:
    AssetModel();
    ~AssetModel();

public:
    // FIXME: add purge_id() to remove AttachmentData owned by AssetData
    double balance();
    // Returns the calculated value at a given date
    std::pair<double, double> valueAtDate(const Data* r, const wxDate& date);
};
