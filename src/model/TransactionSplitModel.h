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

#include "table/TransactionSplitTable.h"
#include "data/TransactionSplitData.h"

#include "_ModelBase.h"
#include "CurrencyModel.h"

struct Split
{
    int64 CATEGID;
    double SPLITTRANSAMOUNT;
    wxArrayInt64 TAGS;
    wxString NOTES;
};

class TransactionSplitModel : public TableFactory<TransactionSplitTable, TransactionSplitData>
{
public:
    TransactionSplitModel();
    ~TransactionSplitModel();

public:
    /**
    Initialize the global TransactionSplitModel table on initial call.
    Resets the global table on subsequent calls.
    * Return the static instance address for TransactionSplitModel table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static TransactionSplitModel& instance(wxSQLite3Database* db);

    /**
    * Return the static instance address for TransactionSplitModel table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static TransactionSplitModel& instance();

public:
    static double get_total(const DataA& rows);
    static double get_total(const std::vector<Split>& local_splits);
    static const wxString get_tooltip(const std::vector<Split>& local_splits, const CurrencyData* currency);
    std::map<int64, TransactionSplitModel::DataA> get_all_id();
    int update(DataA& rows, int64 transactionID);
    int update(const std::vector<Split>& rows, int64 transactionID);
    bool remove_depen(int64 id) override;

public:
    static const wxString refTypeName;
};

