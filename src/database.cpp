#include "database.h"

Database::Database(){ 
    data.clear();
    path = "/home/linux/Documents/program/app/downloader/db/test.db";
}

void Database::init()
{
    try
    {
        //Connect to database.............................
        int rc = sqlite3_open(path.c_str(), &db);
        if(rc)
            std::cout << "Can't open database ...!" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}


bool Database::prepareStmt(std::vector<std::string> &buffer, std::string sql, int row)
{
    if(sqlite3_prepare_v2(db, (char*)sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cout << "Faield prepare_v2 function" << std::endl; 
        std::cout << sql << std::endl; 
        return false;
    }
    
    int total_parm = buffer.size();

    for (int i = 0; i < total_parm; i++)
        if(sqlite3_bind_text(stmt, i+1, (char*)buffer[i].c_str(), buffer[i].size(), 0) != SQLITE_OK)
        {
            std::cout << "Faield to bind " << i << "th paremeter" << std::endl; 
            return false;
        }

    int result = sqlite3_step(stmt);
    if(result != SQLITE_DONE)
        if (result == SQLITE_ROW)
        {
            do
            {
                std::vector<std::string> str;
                for (int i = 0; i < row; i++)
                    str.push_back((char*)sqlite3_column_text(stmt, i));            

                data.push_back(str);
            } while (sqlite3_step(stmt) == SQLITE_ROW);
        }
        else
            return false;

    return true;
}

void Database::print()
{
    std::cout << "print database data : " << std::endl;
    std::cout << "....................." << std::endl;

    for(int i = 0; i < data.size(); i++)
        for(int j = 0; j < data[i].size(); j++)
            std::cout << data[i][j] << std::endl;

    std::cout << "*************************************" << std::endl;
}

void Database::close()
{
    sqlite3_close(db);
    data.clear();
}

Database::~Database()
{
    close();
}



