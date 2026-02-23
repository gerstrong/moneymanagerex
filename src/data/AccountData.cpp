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
// This is only sample code re-used from "table/AccountTable.cpp".

#include "AccountData.h"

AccountData::AccountData()
{
    ACCOUNTID = -1;
    INITIALBAL = 0.0;
    CURRENCYID = -1;
    STATEMENTLOCKED = -1;
    MINIMUMBALANCE = 0.0;
    CREDITLIMIT = 0.0;
    INTERESTRATE = 0.0;
    MINIMUMPAYMENT = 0.0;
}

// Convert AccountData to AccountRow
AccountRow AccountData::to_row() const
{
    AccountRow row;

    row.ACCOUNTID = ACCOUNTID;
    row.ACCOUNTNAME = ACCOUNTNAME;
    row.ACCOUNTTYPE = ACCOUNTTYPE;
    row.ACCOUNTNUM = ACCOUNTNUM;
    row.STATUS = STATUS;
    row.NOTES = NOTES;
    row.HELDAT = HELDAT;
    row.WEBSITE = WEBSITE;
    row.CONTACTINFO = CONTACTINFO;
    row.ACCESSINFO = ACCESSINFO;
    row.INITIALBAL = INITIALBAL;
    row.INITIALDATE = INITIALDATE;
    row.FAVORITEACCT = FAVORITEACCT;
    row.CURRENCYID = CURRENCYID;
    row.STATEMENTLOCKED = STATEMENTLOCKED;
    row.STATEMENTDATE = STATEMENTDATE;
    row.MINIMUMBALANCE = MINIMUMBALANCE;
    row.CREDITLIMIT = CREDITLIMIT;
    row.INTERESTRATE = INTERESTRATE;
    row.PAYMENTDUEDATE = PAYMENTDUEDATE;
    row.MINIMUMPAYMENT = MINIMUMPAYMENT;

    return row;
}

// Convert AccountRow to AccountData
AccountData& AccountData::from_row(const AccountRow& row)
{
    ACCOUNTID = row.ACCOUNTID; // int64
    ACCOUNTNAME = row.ACCOUNTNAME; // wxString
    ACCOUNTTYPE = row.ACCOUNTTYPE; // wxString
    ACCOUNTNUM = row.ACCOUNTNUM; // wxString
    STATUS = row.STATUS; // wxString
    NOTES = row.NOTES; // wxString
    HELDAT = row.HELDAT; // wxString
    WEBSITE = row.WEBSITE; // wxString
    CONTACTINFO = row.CONTACTINFO; // wxString
    ACCESSINFO = row.ACCESSINFO; // wxString
    INITIALBAL = row.INITIALBAL; // double
    INITIALDATE = row.INITIALDATE; // wxString
    FAVORITEACCT = row.FAVORITEACCT; // wxString
    CURRENCYID = row.CURRENCYID; // int64
    STATEMENTLOCKED = row.STATEMENTLOCKED; // int64
    STATEMENTDATE = row.STATEMENTDATE; // wxString
    MINIMUMBALANCE = row.MINIMUMBALANCE; // double
    CREDITLIMIT = row.CREDITLIMIT; // double
    INTERESTRATE = row.INTERESTRATE; // double
    PAYMENTDUEDATE = row.PAYMENTDUEDATE; // wxString
    MINIMUMPAYMENT = row.MINIMUMPAYMENT; // double

    return *this;
}

AccountData& AccountData::operator= (const AccountData& other)
{
    if (this == &other) return *this;

    ACCOUNTID = other.ACCOUNTID;
    ACCOUNTNAME = other.ACCOUNTNAME;
    ACCOUNTTYPE = other.ACCOUNTTYPE;
    ACCOUNTNUM = other.ACCOUNTNUM;
    STATUS = other.STATUS;
    NOTES = other.NOTES;
    HELDAT = other.HELDAT;
    WEBSITE = other.WEBSITE;
    CONTACTINFO = other.CONTACTINFO;
    ACCESSINFO = other.ACCESSINFO;
    INITIALBAL = other.INITIALBAL;
    INITIALDATE = other.INITIALDATE;
    FAVORITEACCT = other.FAVORITEACCT;
    CURRENCYID = other.CURRENCYID;
    STATEMENTLOCKED = other.STATEMENTLOCKED;
    STATEMENTDATE = other.STATEMENTDATE;
    MINIMUMBALANCE = other.MINIMUMBALANCE;
    CREDITLIMIT = other.CREDITLIMIT;
    INTERESTRATE = other.INTERESTRATE;
    PAYMENTDUEDATE = other.PAYMENTDUEDATE;
    MINIMUMPAYMENT = other.MINIMUMPAYMENT;

    return *this;
}

bool AccountData::equals(const AccountData* other) const
{
    if ( ACCOUNTID != other->ACCOUNTID) return false;
    if (!ACCOUNTNAME.IsSameAs(other->ACCOUNTNAME)) return false;
    if (!ACCOUNTTYPE.IsSameAs(other->ACCOUNTTYPE)) return false;
    if (!ACCOUNTNUM.IsSameAs(other->ACCOUNTNUM)) return false;
    if (!STATUS.IsSameAs(other->STATUS)) return false;
    if (!NOTES.IsSameAs(other->NOTES)) return false;
    if (!HELDAT.IsSameAs(other->HELDAT)) return false;
    if (!WEBSITE.IsSameAs(other->WEBSITE)) return false;
    if (!CONTACTINFO.IsSameAs(other->CONTACTINFO)) return false;
    if (!ACCESSINFO.IsSameAs(other->ACCESSINFO)) return false;
    if ( INITIALBAL != other->INITIALBAL) return false;
    if (!INITIALDATE.IsSameAs(other->INITIALDATE)) return false;
    if (!FAVORITEACCT.IsSameAs(other->FAVORITEACCT)) return false;
    if ( CURRENCYID != other->CURRENCYID) return false;
    if ( STATEMENTLOCKED != other->STATEMENTLOCKED) return false;
    if (!STATEMENTDATE.IsSameAs(other->STATEMENTDATE)) return false;
    if ( MINIMUMBALANCE != other->MINIMUMBALANCE) return false;
    if ( CREDITLIMIT != other->CREDITLIMIT) return false;
    if ( INTERESTRATE != other->INTERESTRATE) return false;
    if (!PAYMENTDUEDATE.IsSameAs(other->PAYMENTDUEDATE)) return false;
    if ( MINIMUMPAYMENT != other->MINIMUMPAYMENT) return false;

    return true;
}
