/*******************************************************
 Copyright (C) 2016 Guan Lisheng

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

#include "TagModel.h"
#include "TagLinkModel.h"
#include "AttachmentModel.h"
#include "TransactionModel.h"

TagModel::TagModel() :
    Model<TagTable, TagData>()
{
}

TagModel::~TagModel()
{
}

/**
* Initialize the global TagModel.
*/
TagModel& TagModel::instance(wxSQLite3Database* db)
{
    TagModel& ins = Singleton<TagModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();

    return ins;
}

/** Return the static instance of TagModel */
TagModel& TagModel::instance()
{
    return Singleton<TagModel>::instance();
}

const TagData* TagModel::get_key(const wxString& name)
{
    const Data* tag_n = search_cache_n(TagCol::TAGNAME(name));
    if (tag_n)
        return tag_n;

    DataA items = this->find(TagCol::TAGNAME(name));
    if (!items.empty())
        tag_n = get_data_n(items[0].TAGID);
    return tag_n;
}

int TagModel::is_used(int64 id)
{
    TagLinkModel::DataA taglink = TagLinkModel::instance().find(TagLinkCol::TAGID(id));

    if (taglink.empty())
        return 0;

    for (const auto& link : taglink)
    {
        if (link.REFTYPE == TransactionModel::refTypeName)
        {
            const TransactionData* t = TransactionModel::instance().get_data_n(link.REFID);
            if (t && t->DELETEDTIME.IsEmpty())
                return 1;
        }
        else if (link.REFTYPE == TransactionSplitModel::refTypeName)
        {
            const TransactionSplitData* s = TransactionSplitModel::instance().get_data_n(link.REFID);
            if (s) {
                const TransactionData* t = TransactionModel::instance().get_data_n(s->TRANSID);
                if (t && t->DELETEDTIME.IsEmpty())
                    return 1;
            }
        }
        else
            return 1;
    }

    return -1;
}

