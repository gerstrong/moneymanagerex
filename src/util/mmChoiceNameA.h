/*******************************************************
 Copyright (C) 2025-2026 George Ef (george.a.ef@gmail.com)
 Copyright (C) 2025      Klaus Wich

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

#include <unordered_map>
#include "base/defs.h"

class mmChoiceNameA {
public:
    struct Choice { int id; wxString name; };

private:
    const std::vector<Choice> m_choice_a;
    const int m_default_id_n;                    // -1 or 0 .. m_choice_a.size()-1
    const bool m_nocase;                         // collate nocase
    std::unordered_map<wxString, int> m_index_m; // name -> id_n

public:
    mmChoiceNameA(
        const std::vector<Choice>& choice_a,
        int default_id_n = -1,
        bool nocase = true
    ) :
        m_choice_a(choice_a),
        m_default_id_n(default_id_n),
        m_nocase(nocase)
    {}
    ~mmChoiceNameA() {}

    int default_id_n() const { return m_default_id_n; }
    int valid_id_n(int id_n) const;
    const wxString get_name(int id) const;
    int find_name_n(const wxString& name);
};

class mmChoiceKeyNameA {
public:
    struct Choice { int id; wxString key; wxString name; };

private:
    const std::vector<Choice> m_choice_a;
    const int m_default_id_n;                    // -1 or 0 .. m_choice_a.size()-1
    const bool m_nocase;                         // collate nocase (for both key and name)
    std::unordered_map<wxString, int> m_index_m; // key or name -> id

public:
    mmChoiceKeyNameA(
        const std::vector<Choice>& choice_a,
        int default_id_n = -1,
        bool nocase = true
    ) :
        m_choice_a(choice_a),
        m_default_id_n(default_id_n),
        m_nocase(nocase)
    {}
    ~mmChoiceKeyNameA() {}

    int default_id_n() const { return m_default_id_n; }
    int valid_id_n(int id_n) const;
    const wxString get_key(int id) const;
    const wxString get_name(int id) const;
    int find_keyname_n(const wxString& keyname);
};
