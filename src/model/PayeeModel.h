/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)
 Copyright (C) 2022 Mark Whalley (mark@ipx.co.uk)
 Copyright (C) 2025 Klaus Wich

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

#include "table/PayeeTable.h"
#include "data/PayeeData.h"

#include "_ModelBase.h"

class PayeeModel : public TableFactory<PayeeTable, PayeeData>
{
public:
    static const wxString refTypeName;

public:
    // Initialize the global PayeeModel table on initial call.
    // Resets the global table on subsequent calls.
    // Return the static instance address for PayeeModel table
    // Note: Assigning the address to a local variable can destroy the instance.
    static PayeeModel& instance(wxSQLite3Database* db);

    // Return the static instance address for PayeeModel table
    // Note: Assigning the address to a local variable can destroy the instance.
    static PayeeModel& instance();

    static bool is_used(int64 id);

public:
    PayeeModel();
    ~PayeeModel();

public:
    bool purge_id(int64 id) override;

    const Data* get_key_data_n(const wxString& name);
    const wxString get_id_name(int64 payee_id);

    const wxArrayString find_name_a();
    const std::map<wxString, int64> find_name_id(bool excludeHidden = false);
    const std::map<wxString, int64> find_name_id_used();
    const DataA filter_name(const wxString& name_pattern, bool includeInActive = true);
};
