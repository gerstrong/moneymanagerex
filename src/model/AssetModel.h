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
    enum STATUS_ID {
        STATUS_ID_CLOSED = 0,
        STATUS_ID_OPEN,
        STATUS_ID_size
    };
    enum CHANGE_ID
    {
        CHANGE_ID_NONE = 0,
        CHANGE_ID_APPRECIATE,
        CHANGE_ID_DEPRECIATE,
        CHANGE_ID_size
    };
    enum CHANGEMODE_ID {
        CHANGEMODE_ID_PERCENTAGE = 0,
        CHANGEMODE_ID_LINEAR,
        CHANGEMODE_ID_size
    };

private:
    static mmChoiceNameA STATUS_CHOICES;
    static mmChoiceNameA CHANGE_CHOICES;
    static mmChoiceNameA CHANGEMODE_CHOICES;

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

    static const wxString status_name(int id);
    static int status_id(const wxString& name);
    static STATUS_ID status_id(const Data& r);

    static const wxString change_name(int id);
    static int change_id(const wxString& name);
    static CHANGE_ID change_id(const Data& r);

    static const wxString changemode_name(int id);
    static int changemode_id(const wxString& name);
    static CHANGEMODE_ID changemode_id(const Data& r);

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

//----------------------------------------------------------------------------

inline const wxString AssetModel::status_name(int id)
{
    return STATUS_CHOICES.get_name(id);
}
inline int AssetModel::status_id(const wxString& name)
{
    return STATUS_CHOICES.find_name_n(name);
}
inline AssetModel::STATUS_ID AssetModel::status_id(const Data& asset)
{
    return static_cast<STATUS_ID>(status_id(asset.ASSETSTATUS));
}

inline const wxString AssetModel::change_name(int id)
{
    return CHANGE_CHOICES.get_name(id);
}
inline int AssetModel::change_id(const wxString& name)
{
    return CHANGE_CHOICES.find_name_n(name);
}
inline AssetModel::CHANGE_ID AssetModel::change_id(const Data& asset)
{
    return static_cast<CHANGE_ID>(change_id(asset.VALUECHANGE));
}

inline const wxString AssetModel::changemode_name(int id)
{
    return CHANGEMODE_CHOICES.get_name(id);
}
inline int AssetModel::changemode_id(const wxString& name)
{
    return CHANGEMODE_CHOICES.find_name_n(name);
}
inline AssetModel::CHANGEMODE_ID AssetModel::changemode_id(const Data& asset)
{
    return static_cast<CHANGEMODE_ID>(changemode_id(asset.VALUECHANGEMODE));
}

