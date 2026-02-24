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

#include "base/defs.h"
#include <tuple>
#include "util/mmDateRange.h"

#include "AccountModel.h"
#include "CategoryModel.h"
#include "CurrencyHistoryModel.h"
#include "PreferencesModel.h"
#include "ScheduledModel.h"
#include "TransactionModel.h"

CategoryModel::CategoryModel() :
    TableFactory<CategoryTable, CategoryData>()
{
}

CategoryModel::~CategoryModel()
{
}

/**
* Initialize the global CategoryModel table.
* Reset the CategoryModel table or create the table if it does not exist.
*/
CategoryModel& CategoryModel::instance(wxSQLite3Database* db)
{
    CategoryModel& ins = Singleton<CategoryModel>::instance();
    ins.reset_cache();
    ins.m_db = db;
    ins.ensure_table();
    ins.preload_cache();

    return ins;
}

/** Return the static instance of CategoryModel table */
CategoryModel& CategoryModel::instance()
{
    return Singleton<CategoryModel>::instance();
}

const wxArrayString CategoryModel::FilterCategory(const wxString& category_pattern)
{
    wxArrayString categories;
    for (auto &category : CategoryModel::instance().find_all())
    {
        if (category.CATEGNAME.Lower().Matches(category_pattern.Lower().Append("*")))
            categories.push_back(category.CATEGNAME);
    }
    return categories;
}

const CategoryData* CategoryModel::get_name(const wxString& name, const wxString& parentname)
{
    const Data* category_n = nullptr;
    DataA items = this->find(CategoryCol::CATEGNAME(name));
    for (const auto& item : items) {
        if (item.PARENTID != -1) {
            if (instance().get_data_n(item.PARENTID)->CATEGNAME.Lower() == parentname.Lower()) {
                category_n = get_data_n(item.CATEGID);
                break;
            }
        }
    }

    return category_n;
}

const CategoryData* CategoryModel::get_key(const wxString& name, const int64 parentid)
{
    const Data* category_n = search_cache_n(
        CategoryCol::CATEGNAME(name),
        CategoryCol::PARENTID(parentid)
    );
    if (category_n)
        return category_n;

    DataA items = this->find(CategoryCol::CATEGNAME(name), CategoryCol::PARENTID(parentid));
    if (!items.empty())
        category_n = get_data_n(items[0].CATEGID);
    return category_n;
}

const std::map<wxString, int64> CategoryModel::all_categories(bool excludeHidden)
{
    std::map<wxString, int64> full_categs;
    for (const auto& c : instance().find_all(Col::COL_ID_CATEGID))
    {
        if (excludeHidden && (c.ACTIVE == 0))
            continue;

        full_categs[full_name(c.CATEGID)] = c.CATEGID;
    }
    return full_categs;
}

CategoryModel::DataA CategoryModel::sub_category(const Data* r)
{
    return instance().find(CategoryCol::PARENTID(r->CATEGID));
}

CategoryModel::DataA CategoryModel::sub_category(const Data& r)
{
    return instance().find(CategoryCol::PARENTID(r.CATEGID));
}

CategoryModel::DataA CategoryModel::sub_tree(const Data* r)
{
    DataA subtree;
    DataA subcategories = instance().find(CategoryCol::PARENTID(r->CATEGID));
    std::stable_sort(subcategories.begin(), subcategories.end(), CategoryData::SorterByCATEGNAME());
    for (const auto& subcategory : subcategories) {
        subtree.push_back(subcategory);
        DataA subtreecats = sub_tree(subcategory);
        for (const auto& cat : subtreecats) {
            subtree.push_back(cat);
        }
    }
    return subtree;
}

CategoryModel::DataA CategoryModel::sub_tree(const Data& r)
{
    return sub_tree(&r);
}

const wxString CategoryModel::full_name(const Data* category)
{
    static wxString delimiter;
    if (delimiter.empty()) {
        delimiter = InfoModel::instance().getString("CATEG_DELIMITER", ":");
    }
    if (!category) return "";
    if (category->PARENTID == -1)
        return category->CATEGNAME;
    else {
        wxString name = category->CATEGNAME;
        const Data* parentCategory = instance().get_data_n(category->PARENTID);
        while (parentCategory) {
            name = name.Prepend(delimiter).Prepend(parentCategory->CATEGNAME);
            parentCategory = instance().get_data_n(parentCategory->PARENTID);
        }
        return name;
    }
}

const wxString CategoryModel::full_name(int64 category_id)
{
    const Data* category = instance().get_data_n(category_id);
    return full_name(category);
}

const wxString CategoryModel::full_name(int64 category_id, wxString delimiter)
{
    const Data* category = instance().get_data_n(category_id);
    if (!category)
        return "";
    if (category->PARENTID == -1)
        return category->CATEGNAME;
    else {
        wxString name = category->CATEGNAME;
        const Data* parentCategory = instance().get_data_n(category->PARENTID);
        while (parentCategory) {
            name = name.Prepend(delimiter).Prepend(parentCategory->CATEGNAME);
            parentCategory = instance().get_data_n(parentCategory->PARENTID);
        }
        return name;
    }
}

// -- Check if Category should be made available for use.
//    Hiding a category hides all sub-categories

bool CategoryModel::is_hidden(int64 catID)
{
    const auto category = CategoryModel::instance().get_data_n(catID);
    if (category && category->ACTIVE == 0)
        return true;

    return false;
}

bool CategoryModel::is_used(int64 id)
{
    if (id < 0) return false;
    const auto& trans = TransactionModel::instance().find(TransactionCol::CATEGID(id));
    if (!trans.empty())
    {
        for (const auto& txn : trans)
            if (txn.DELETEDTIME.IsEmpty())
                return true;
    }
    const auto& split = TransactionSplitModel::instance().find(TransactionCol::CATEGID(id));
    if (!split.empty())
    {
        for (const auto& txn : split)
            if (TransactionModel::instance().get_data_n(txn.TRANSID)->DELETEDTIME.IsEmpty())
                return true;
    }
    const auto& deposits = ScheduledModel::instance().find(ScheduledCol::CATEGID(id));
    if (!deposits.empty()) return true;
    const auto& deposit_split = ScheduledSplitModel::instance().find(ScheduledCol::CATEGID(id));
    if (!deposit_split.empty()) return true;
    DataA children = instance().find(CategoryCol::PARENTID(id));
    if (!children.empty()){
        bool used = false;
        for(const auto& child : children){
            used = used || is_used(child.CATEGID);
        }
        return used;
    }
    return false;
}

bool CategoryModel::has_income(int64 id)
{
    double sum = 0.0;
    auto splits = TransactionSplitModel::instance().get_all_id();
    for (const auto& tran: TransactionModel::instance().find(TransactionCol::CATEGID(id)))
    {
        if (!tran.DELETEDTIME.IsEmpty()) continue;

        switch (TransactionModel::type_id(tran))
        {
        case TransactionModel::TYPE_ID_WITHDRAWAL:
            sum -= tran.TRANSAMOUNT;
            break;
        case TransactionModel::TYPE_ID_DEPOSIT:
            sum += tran.TRANSAMOUNT;
        case TransactionModel::TYPE_ID_TRANSFER:
        default:
            break;
        }

        for (const auto& split: splits[tran.id()])
        {
            switch (TransactionModel::type_id(tran))
            {
            case TransactionModel::TYPE_ID_WITHDRAWAL:
                sum -= split.SPLITTRANSAMOUNT;
                break;
            case TransactionModel::TYPE_ID_DEPOSIT:
                sum += split.SPLITTRANSAMOUNT;
            case TransactionModel::TYPE_ID_TRANSFER:
            default:
                break;
            }
        }
    }

    return sum > 0;
}

void CategoryModel::getCategoryStats(
    std::map<int64, std::map<int, double>>& categoryStats
    , wxSharedPtr<wxArrayString> accountArray
    , mmDateRange* date_range
    , bool WXUNUSED(ignoreFuture) //TODO: deprecated
    , bool group_by_month
    , std::map<int64, double> *budgetAmt
    , [[maybe_unused]] bool fin_months)
{
    //Initialization
    //Set std::map with zerros
    const auto& allcategories = instance().find_all();
    double value = 0;
    int columns = group_by_month ? 12 : 1;
    const wxDateTime start_date(date_range->start_date());

    std::vector<std::pair<wxDateTime, int>> monthMap;
    for (int m = 0; m < columns; m++)
    {
        const wxDateTime d = start_date.Add(wxDateSpan::Months(m));
        monthMap.emplace_back(d, m);
    }
    std::reverse(monthMap.begin(), monthMap.end());

    for (const auto& category : allcategories)
    {
        for (int m = 0; m < columns; m++)
        {
            int month = group_by_month ? m : 0;
            categoryStats[category.CATEGID][month] = value;
        }
    }
    //Calculations
    auto splits = TransactionSplitModel::instance().get_all_id();
    for (const auto& transaction : TransactionModel::instance().find(
        TransactionModel::STATUS(OP_NE, TransactionModel::STATUS_ID_VOID),
        TransactionModel::TRANSDATE(OP_GE, date_range->start_date()),
        TransactionCol::TRANSDATE(OP_LE, date_range->end_date().FormatISOCombined())
    )) {
        if (!transaction.DELETEDTIME.IsEmpty()) continue;

        if (accountArray)
        {
            const auto account = AccountModel::instance().get_data_n(transaction.ACCOUNTID);
            if (wxNOT_FOUND == accountArray->Index(account->ACCOUNTNAME)) {
                continue;
            }
        }

        const double convRate = CurrencyHistoryModel::getDayRate(
            AccountModel::instance().get_data_n(transaction.ACCOUNTID)->CURRENCYID,
            transaction.TRANSDATE
        );
        wxDateTime d = TransactionModel::getTransDateTime(transaction);

        int month = 0;
        if (group_by_month)
        {
            auto it = std::find_if(monthMap.begin(), monthMap.end()
                , [d](std::pair<wxDateTime, int> date){return d >= date.first;});
            month = it->second;
        }

        int64 categID = transaction.CATEGID;

        if (splits[transaction.id()].empty())
        {
            if (TransactionModel::type_id(transaction) != TransactionModel::TYPE_ID_TRANSFER)
            {
                // Do not include asset or stock transfers in income expense calculations.
                if (TransactionModel::is_foreignAsTransfer(transaction))
                    continue;
                categoryStats[categID][month] += TransactionModel::account_flow(transaction, transaction.ACCOUNTID) * convRate;
            }
            else if (budgetAmt != 0)
            {
                double amt = transaction.TRANSAMOUNT * convRate;
                if ((*budgetAmt)[categID] < 0)
                    categoryStats[categID][month] -= amt;
                else
                    categoryStats[categID][month] += amt;
            }
        }
        else
        {
            for (const auto& entry : splits[transaction.id()])
            {
                categoryStats[entry.CATEGID][month] += entry.SPLITTRANSAMOUNT
                    * convRate * ((TransactionModel::type_id(transaction) == TransactionModel::TYPE_ID_WITHDRAWAL) ? -1 : 1);
            }
        }
    }
}
