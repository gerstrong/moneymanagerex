/*******************************************************
Copyright (C) 2014 Nikolay Akimov
Copyright (C) 2022  Mark Whalley (mark@ipx.co.uk)

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
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/string.h>
#include <wx/richtooltip.h>

#include "util/_util.h"
#include "util/_simple.h"

#include "model/AccountModel.h"
#include "model/PayeeModel.h"
#include "import_export/webapp.h"

class mmComboBoxText : public wxTextCtrl
{
public:
    using wxTextCtrl::Connect;

    mmComboBoxText() {}
    mmComboBoxText(wxWindow *parent, wxWindowID id
        , const wxString &value
        , bool payee
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style = 0
        , const wxValidator &validator = wxDefaultValidator
        , const wxString &name = wxTextCtrlNameStr)
        : wxTextCtrl(parent, id, value, pos, size, style, validator, name)
        , m_payee(payee)
    {
        if (m_payee)
            this->AutoComplete(PayeeModel::instance().all_payee_names());
        else
            this->AutoComplete(AccountModel::instance().all_checking_account_names());

    }
    wxString GetValue() const
    {
        return wxTextCtrl::GetValue();
    }

    void setSelection(int &id)
    {
        if (m_payee) {
            for (const auto &payee : PayeeModel::instance().find_all(PayeeCol::COL_ID_PAYEENAME))
                if (payee.PAYEEID == id) this->ChangeValue(payee.PAYEENAME);
        }
        else
        {
            for (const auto &acc : AccountModel::instance().find_all(AccountCol::COL_ID_ACCOUNTNAME))
                if (acc.ACCOUNTID == id) this->ChangeValue(acc.ACCOUNTNAME);
        }
    }

    int64 getID()
    {
        int64 id = -1;
        if (m_payee) {
            const PayeeData* payee_n = PayeeModel::instance().get_key(this->GetValue());
            if (payee_n) {
                id = payee_n->PAYEEID;
            }
            else {
                PayeeData new_payee_d = PayeeData();
                new_payee_d.PAYEENAME = this->GetValue();
                new_payee_d.ACTIVE    = 1;
                PayeeModel::instance().add_data_n(new_payee_d);
                mmWebApp::MMEX_WebApp_UpdatePayee();
            }
        }
        else {
            const AccountData* account_n = AccountModel::instance().get_key(this->GetValue());
            if (account_n) {
                id = account_n->ACCOUNTID;
            }
            else {
                //TODO:
            }
        }
        return id;
    }

private:
    bool m_payee = false;
};
