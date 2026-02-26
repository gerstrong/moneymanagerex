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

#include "_DataEnum.h"

mmChoiceNameA AssetType::s_choice_a = mmChoiceNameA({
    { e_property,   _n("Property") },
    { e_automobile, _n("Automobile") },
    { e_household,  _n("Household Object") },
    { e_art,        _n("Art") },
    { e_jewellery,  _n("Jewellery") },
    { e_cash,       _n("Cash") },
    { e_other,      _n("Other") },
}, e_property, true );

mmChoiceNameA AssetStatus::s_choice_a = mmChoiceNameA({
    { e_closed, _n("Closed") },
    { e_open,   _n("Open") }
}, e_open, true);

mmChoiceNameA AssetChange::s_choice_a = mmChoiceNameA({
    { e_none,        _n("None") },
    { e_appreciates, _n("Appreciates") },
    { e_depreciates, _n("Depreciates") }
}, e_none, true);

mmChoiceNameA AssetChangeMode::s_choice_a = mmChoiceNameA({
    { e_percentage, _n("Percentage") },
    { e_linear,     _n("Linear") }
}, e_percentage, true);

