// -*- C++ -*-
//=============================================================================
/**
 *      Copyright: (c) 2013-2026 Guan Lisheng (guanlisheng@gmail.com)
 *      Copyright: (c) 2017-2018 Stefano Giorgio (stef145g)
 *      Copyright: (c) 2022      Mark Whalley (mark@ipx.co.uk)
 *      Copyright: (c) 2026      George Ef (george.a.ef@gmail.com)
 *
 *      TransactionShareTable.h
 *
 *      Interface to database table SHAREINFO_V1
 *
 *      @author [sqlite2cpp.py]
 *
 *      Revision History:
 *          AUTO GENERATED at 2026-02-23 02:03:14.067947.
 *          DO NOT EDIT!
 */
//=============================================================================

#pragma once

#include "_TableBase.h"

// Columns in database table SHAREINFO_V1
struct TransactionShareCol
{
    enum COL_ID
    {
        COL_ID_SHAREINFOID = 0,
        COL_ID_CHECKINGACCOUNTID,
        COL_ID_SHARENUMBER,
        COL_ID_SHAREPRICE,
        COL_ID_SHARECOMMISSION,
        COL_ID_SHARELOT,
        COL_ID_size
    };

    static const wxArrayString COL_NAME_A;
    static const COL_ID PRIMARY_ID;
    static const wxString PRIMARY_NAME;

    static wxString col_name(COL_ID col_id) { return COL_NAME_A[col_id]; }

    struct SHAREINFOID : public TableOpV<int64>
    {
        static COL_ID col_id() { return COL_ID_SHAREINFOID; }
        static wxString col_name() { return COL_NAME_A[COL_ID_SHAREINFOID]; }
        explicit SHAREINFOID(const int64 &v): TableOpV<int64>(OP_EQ, v) {}
        explicit SHAREINFOID(OP op, const int64 &v): TableOpV<int64>(op, v) {}
    };

    struct CHECKINGACCOUNTID : public TableOpV<int64>
    {
        static COL_ID col_id() { return COL_ID_CHECKINGACCOUNTID; }
        static wxString col_name() { return COL_NAME_A[COL_ID_CHECKINGACCOUNTID]; }
        explicit CHECKINGACCOUNTID(const int64 &v): TableOpV<int64>(OP_EQ, v) {}
        explicit CHECKINGACCOUNTID(OP op, const int64 &v): TableOpV<int64>(op, v) {}
    };

    struct SHARENUMBER : public TableOpV<double>
    {
        static COL_ID col_id() { return COL_ID_SHARENUMBER; }
        static wxString col_name() { return COL_NAME_A[COL_ID_SHARENUMBER]; }
        explicit SHARENUMBER(const double &v): TableOpV<double>(OP_EQ, v) {}
        explicit SHARENUMBER(OP op, const double &v): TableOpV<double>(op, v) {}
    };

    struct SHAREPRICE : public TableOpV<double>
    {
        static COL_ID col_id() { return COL_ID_SHAREPRICE; }
        static wxString col_name() { return COL_NAME_A[COL_ID_SHAREPRICE]; }
        explicit SHAREPRICE(const double &v): TableOpV<double>(OP_EQ, v) {}
        explicit SHAREPRICE(OP op, const double &v): TableOpV<double>(op, v) {}
    };

    struct SHARECOMMISSION : public TableOpV<double>
    {
        static COL_ID col_id() { return COL_ID_SHARECOMMISSION; }
        static wxString col_name() { return COL_NAME_A[COL_ID_SHARECOMMISSION]; }
        explicit SHARECOMMISSION(const double &v): TableOpV<double>(OP_EQ, v) {}
        explicit SHARECOMMISSION(OP op, const double &v): TableOpV<double>(op, v) {}
    };

    struct SHARELOT : public TableOpV<wxString>
    {
        static COL_ID col_id() { return COL_ID_SHARELOT; }
        static wxString col_name() { return COL_NAME_A[COL_ID_SHARELOT]; }
        explicit SHARELOT(const wxString &v): TableOpV<wxString>(OP_EQ, v) {}
        explicit SHARELOT(OP op, const wxString &v): TableOpV<wxString>(op, v) {}
    };
};

// A single record in database table SHAREINFO_V1
struct TransactionShareRow
{
    using Col = TransactionShareCol;

    int64 SHAREINFOID; // primary key
    int64 CHECKINGACCOUNTID;
    double SHARENUMBER;
    double SHAREPRICE;
    double SHARECOMMISSION;
    wxString SHARELOT;

    explicit TransactionShareRow();
    explicit TransactionShareRow(wxSQLite3ResultSet& q);
    TransactionShareRow(const TransactionShareRow& other) = default;

    int64 id() const { return SHAREINFOID; }
    void id(const int64 id) { SHAREINFOID = id; }
    void to_insert_stmt(wxSQLite3Statement& stmt, int64 id) const;
    void to_update_stmt(wxSQLite3Statement& stmt) const;
    TransactionShareRow& from_select_result(wxSQLite3ResultSet& q);
    wxString to_json() const;
    void as_json(PrettyWriter<StringBuffer>& json_writer) const;
    row_t to_html_row() const;
    void to_html_template(html_template& t) const;
    void destroy() { delete this; }

    TransactionShareRow& operator= (const TransactionShareRow& other);
    TransactionShareRow& clone_from(const TransactionShareRow& other);
    bool equals(const TransactionShareRow* other) const;
    bool operator< (const TransactionShareRow& other) const { return id() < other.id(); }
    bool operator< (const TransactionShareRow* other) const { return id() < other->id(); }

    template<typename C>
    bool match(const C&)
    {
        return false;
    }

    // TODO: check if col.m_operator == OP_EQ

    bool match(const Col::SHAREINFOID& col)
    {
        return SHAREINFOID == col.m_value;
    }

    bool match(const Col::CHECKINGACCOUNTID& col)
    {
        return CHECKINGACCOUNTID == col.m_value;
    }

    bool match(const Col::SHARENUMBER& col)
    {
        return SHARENUMBER == col.m_value;
    }

    bool match(const Col::SHAREPRICE& col)
    {
        return SHAREPRICE == col.m_value;
    }

    bool match(const Col::SHARECOMMISSION& col)
    {
        return SHARECOMMISSION == col.m_value;
    }

    bool match(const Col::SHARELOT& col)
    {
        return SHARELOT.CmpNoCase(col.m_value) == 0;
    }

    template<typename Arg1, typename... Args>
    bool match(const Arg1& arg1, const Args&... args)
    {
        return (match(arg1) && ... && match(args));
    }

    struct SorterBySHAREINFOID
    {
        bool operator()(const TransactionShareRow& x, const TransactionShareRow& y)
        {
            return x.SHAREINFOID < y.SHAREINFOID;
        }
    };

    struct SorterByCHECKINGACCOUNTID
    {
        bool operator()(const TransactionShareRow& x, const TransactionShareRow& y)
        {
            return x.CHECKINGACCOUNTID < y.CHECKINGACCOUNTID;
        }
    };

    struct SorterBySHARENUMBER
    {
        bool operator()(const TransactionShareRow& x, const TransactionShareRow& y)
        {
            return x.SHARENUMBER < y.SHARENUMBER;
        }
    };

    struct SorterBySHAREPRICE
    {
        bool operator()(const TransactionShareRow& x, const TransactionShareRow& y)
        {
            return x.SHAREPRICE < y.SHAREPRICE;
        }
    };

    struct SorterBySHARECOMMISSION
    {
        bool operator()(const TransactionShareRow& x, const TransactionShareRow& y)
        {
            return x.SHARECOMMISSION < y.SHARECOMMISSION;
        }
    };

    struct SorterBySHARELOT
    {
        bool operator()(const TransactionShareRow& x, const TransactionShareRow& y)
        {
            return x.SHARELOT < y.SHARELOT;
        }
    };
};

// Interface to database table SHAREINFO_V1
struct TransactionShareTable : public TableBase
{
    using Row = TransactionShareRow;
    using Col = typename Row::Col;

    TransactionShareTable();
    ~TransactionShareTable() {}
};

inline TransactionShareRow::TransactionShareRow(wxSQLite3ResultSet& q)
{
    from_select_result(q);
}

inline void TransactionShareRow::to_update_stmt(wxSQLite3Statement& stmt) const
{
    to_insert_stmt(stmt, id());
}

inline TransactionShareRow& TransactionShareRow::clone_from(const TransactionShareRow& other)
{
    *this = other;
    id(-1);
    return *this;
}
