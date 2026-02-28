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
    AssetModel();
    ~AssetModel();

public:
    static AssetModel& instance(wxSQLite3Database* db);
    static AssetModel& instance();

    static AssetCol::ASSETTYPE ASSETTYPE(OP op, AssetType type);
    static AssetCol::STARTDATE STARTDATE(OP op, const mmDate& date);

public:
    // FIXME: add purge_id() to remove AttachmentData owned by AssetData

    // Return the asset name of a given id
    wxString get_id_name(int64 asset_id);

    // Return the value of an asset at a given date
    std::pair<double, double> valueAtDate(const Data& asset_d, const wxDate& date);
    // Return the current value of an asset
    std::pair<double, double> value(const Data& asset_d);
    // Return the current value of all assets
    double balance();
};
