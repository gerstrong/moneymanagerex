/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)

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

#include "table/ReportTable.h"
#include "data/ReportData.h"

#include "_ModelBase.h"

class ReportModel : public TableFactory<ReportTable, ReportData>
{
private:
    struct Values
    {
        wxString label;
        wxString type;
        wxString def_value;
        int ID;
        wxString name;
    };
    static const std::vector<Values> SqlPlaceHolders();

public:
    /**
    Initialize the global ReportModel table on initial call.
    Resets the global table on subsequent calls.
    * Return the static instance address for ReportModel table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static ReportModel& instance(wxSQLite3Database* db);

    /**
    * Return the static instance address for ReportModel table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static ReportModel& instance();

    static const std::vector<std::pair<wxString, wxString>> getParamNames();
    static bool prepare_sql(wxString& sql, std::map <wxString, wxString>& rep_params);

public:
    ReportModel(); 
    ~ReportModel();

public:
    const Data* get_key_data_n(const wxString& name);

    wxArrayString find_group_name_a();
    bool get_objects_from_sql(const wxString& query, PrettyWriter<StringBuffer>& json_writer);
    int get_html(const Data* r, wxString& out);
    //wxString get_html(const Data& r);
};

