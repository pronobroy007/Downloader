#ifndef DATABASE
#define DATABASE

#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h>

class Database
{
    public:
        std::vector<std::vector<std::string>> data;

        Database();
            void init();
            /*******************************************
             * buffer = data for prepareStmt.
             * sql = Sql command.
             * row = total number of paremeter return by "sql" query.
             */
            bool prepareStmt(std::vector<std::string> &buffer, std::string sql, int row);

            void print();
            void close();
        ~Database();

    private:
        sqlite3 *db;
        sqlite3_stmt *stmt;
        std::string path;
};


#endif
