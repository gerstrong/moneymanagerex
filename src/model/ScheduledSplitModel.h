/*******************************************************
 Copyright (C) 2013,2014 James Higley

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

#include "table/ScheduledSplitTable.h"
#include "data/ScheduledSplitData.h"

#include "_ModelBase.h"

class ScheduledSplitModel : public TableFactory<ScheduledSplitTable, ScheduledSplitData>
{
public:
    ScheduledSplitModel();
    ~ScheduledSplitModel();

public:
    /**
    Initialize the global ScheduledSplitModel table on initial call.
    Resets the global table on subsequent calls.
    * Return the static instance address for ScheduledSplitModel table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static ScheduledSplitModel& instance(wxSQLite3Database* db);

    /**
    * Return the static instance address for ScheduledSplitModel table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static ScheduledSplitModel& instance();

public:
    double get_total(const DataA& rows);
    std::map<int64, DataA> get_all_id();
    int update(DataA& rows, int64 transactionID);
    bool remove_depen(int64 id) override;

public:
    static const wxString refTypeName;
};

