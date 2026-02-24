/*******************************************************
 Copyright (C) 2025 George Ef (george.a.ef@gmail.com)

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

#include "mmChoiceNameA.h"

mmChoiceNameA::mmChoiceNameA(const std::vector<Item>& array_, bool nocase_) :
    array(array_),
    nocase(nocase_)
{
}

mmChoiceNameA::~mmChoiceNameA()
{
}

const wxString mmChoiceNameA::getName(int id) const
{
    wxASSERT(id >= 0 && id < static_cast<int>(array.size()));
    wxASSERT(array[id].id == id);
    return array[id].name;
}

int mmChoiceNameA::findName(const wxString& name, int default_id)
{
    if (const auto it = name_id.find(name); it != name_id.end())
        return it->second;

    int id = default_id;
    for (const Item& item : array) {
        bool match = nocase ? (name.CmpNoCase(item.name) == 0) : (name == item.name);
        if (match) { id = item.id; break; }
    }
    name_id.insert({name, id});
    return id;
}

//----------------------------------------------------------------------------

mmChoiceKeyNameA::mmChoiceKeyNameA(const std::vector<Item>& array_, bool nocase_) :
    array(array_),
    nocase(nocase_)
{
}

mmChoiceKeyNameA::~mmChoiceKeyNameA()
{
}

const wxString mmChoiceKeyNameA::getKey(int id) const
{
    wxASSERT(id >= 0 && id < static_cast<int>(array.size()));
    wxASSERT(array[id].id == id);
    return array[id].key;
}

const wxString mmChoiceKeyNameA::getName(int id) const
{
    wxASSERT(id >= 0 && id < static_cast<int>(array.size()));
    wxASSERT(array[id].id == id);
    return array[id].name;
}

int mmChoiceKeyNameA::findKeyName(const wxString& keyOrName, int default_id)
{
    if (const auto it = keyOrName_id.find(keyOrName); it != keyOrName_id.end())
        return it->second;

    int id = default_id;
    for (const Item& item : array) {
        bool match = nocase ?
            (keyOrName.CmpNoCase(item.key) == 0 || keyOrName.CmpNoCase(item.name) == 0) :
            (keyOrName == item.key || keyOrName == item.name);
        if (match) { id = item.id; break; }
    }
    keyOrName_id.insert({keyOrName, id});
    return id;
}

