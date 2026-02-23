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
// This is only sample code re-used from "table/TransactionLinkTable.cpp".

#include "TransactionLinkData.h"

TransactionLinkData::TransactionLinkData()
{
    TRANSLINKID = -1;
    CHECKINGACCOUNTID = -1;
    LINKRECORDID = -1;
}

// Convert TransactionLinkData to TransactionLinkRow
TransactionLinkRow TransactionLinkData::to_row() const
{
    TransactionLinkRow row;

    row.TRANSLINKID = TRANSLINKID;
    row.CHECKINGACCOUNTID = CHECKINGACCOUNTID;
    row.LINKTYPE = LINKTYPE;
    row.LINKRECORDID = LINKRECORDID;

    return row;
}

// Convert TransactionLinkRow to TransactionLinkData
TransactionLinkData& TransactionLinkData::from_row(const TransactionLinkRow& row)
{
    TRANSLINKID = row.TRANSLINKID; // int64
    CHECKINGACCOUNTID = row.CHECKINGACCOUNTID; // int64
    LINKTYPE = row.LINKTYPE; // wxString
    LINKRECORDID = row.LINKRECORDID; // int64

    return *this;
}

TransactionLinkData& TransactionLinkData::operator= (const TransactionLinkData& other)
{
    if (this == &other) return *this;

    TRANSLINKID = other.TRANSLINKID;
    CHECKINGACCOUNTID = other.CHECKINGACCOUNTID;
    LINKTYPE = other.LINKTYPE;
    LINKRECORDID = other.LINKRECORDID;

    return *this;
}

bool TransactionLinkData::equals(const TransactionLinkData* other) const
{
    if ( TRANSLINKID != other->TRANSLINKID) return false;
    if ( CHECKINGACCOUNTID != other->CHECKINGACCOUNTID) return false;
    if (!LINKTYPE.IsSameAs(other->LINKTYPE)) return false;
    if ( LINKRECORDID != other->LINKRECORDID) return false;

    return true;
}
