#include "BaseSpec/Files.hpp"
namespace FSpec{
    static string dirname = "db/";
    
    string genCatalogName() { return dirname + "sql.ctl"; }

    string genTableName(const string& tablename) { return dirname + "sql_" + tablename + ".tbl"; }

    string genIndexName(const string& indexName) { return dirname + "sql_" + indexName + ".idx"; }
}