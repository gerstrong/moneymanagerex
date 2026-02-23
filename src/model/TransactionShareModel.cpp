/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)
 Copyright (C) 2016 Stefano Giorgio

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

#include "TransactionModel.h"
#include "TransactionShareModel.h"

TransactionShareModel::TransactionShareModel() :
    Model<TransactionShareTable, TransactionShareData>()
{
}

TransactionShareModel::~TransactionShareModel()
{
}

/**
* Initialize the global TransactionShareModel table.
* Reset the TransactionShareModel table or create the table if it does not exist.
*/
TransactionShareModel& TransactionShareModel::instance(wxSQLite3Database* db)
{
    TransactionShareModel& ins = Singleton<TransactionShareModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();

    return ins;
}

/** Return the static instance of TransactionShareModel table */
TransactionShareModel& TransactionShareModel::instance()
{
    return Singleton<TransactionShareModel>::instance();
}

TransactionShareModel::DataA TransactionShareModel::ShareList(const int64 checking_id)
{
    // SQL equivalent statement:
    // select * from TransactionShareModel where CHECKINGACCOUNTID = checking_account_id;
    TransactionShareModel::DataA trans_list = TransactionShareModel::instance().find(
        TransactionShareCol::CHECKINGACCOUNTID(checking_id)
    );

    return trans_list;
}

TransactionShareData* TransactionShareModel::ShareEntry(const int64 checking_id)
{
    DataA list = TransactionShareModel::ShareList(checking_id);
    if (!list.empty()) {
        return TransactionShareModel::instance().unsafe_get_data_n(list.at(0).SHAREINFOID);
    }
    return nullptr;
}

void TransactionShareModel::ShareEntry(
    int64 trx_id,
    double share_number,
    double share_price,
    double share_commission,
    const std::vector<Split>& commission_splits,
    const wxString& share_lot
) {
    bool updateTimestamp = false;
    Data old_entry;
    Data ts_d;
    DataA share_list = ShareList(trx_id);

    if (share_list.empty()) {
        ts_d = Data();
        ts_d.CHECKINGACCOUNTID = trx_id;
        updateTimestamp = true;
    }
    else {
        old_entry = share_list[0];
        ts_d = share_list[0];
    }

    ts_d.SHARENUMBER     = share_number;
    ts_d.SHAREPRICE      = share_price;
    ts_d.SHARECOMMISSION = share_commission;
    ts_d.SHARELOT        = share_lot;
    TransactionShareModel::instance().save_data(ts_d);
    int64 id = ts_d.id();

    TransactionSplitModel::instance().update(commission_splits, id);

    if (updateTimestamp || !ts_d.equals(&old_entry))
        TransactionModel::instance().updateTimestamp(trx_id);
}

void TransactionShareModel::RemoveShareEntry(const int64 checking_id)
{
    DataA list = ShareList(checking_id);
    if (!list.empty())
    {
        TransactionShareModel::instance().remove_depen(list.at(0).SHAREINFOID);
    }
}
