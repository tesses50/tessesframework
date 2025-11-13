#include "TessesFramework/Serialization/SQLite.hpp"
#if defined(TESSESFRAMEWORK_ENABLE_SQLITE)
#include "sqlite/sqlite3-mod.h"
#endif
namespace Tesses::Framework::Serialization {
    int SQLiteDatabase::collector(void* user, int count,char** vals, char** keys)
    {
        auto list = static_cast<std::vector<std::vector<std::pair<std::string,std::optional<std::string>>>>*>(user);
        std::vector<std::pair<std::string,std::optional<std::string>>> d;
        for(int i = 0; i < count; i++)
        {
            std::string key = keys[i] == nullptr ? "" : keys[i];
            std::optional<std::string> value = vals[i] == nullptr ? std::nullopt : (std::optional<std::string>)vals[i];
            
            d.push_back(std::pair<std::string,std::optional<std::string>>(key,value));
        }
        list->push_back(d);
        return 0;
    }
        
    SQLiteDatabase::SQLiteDatabase(Tesses::Framework::Filesystem::VFSPath path)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_SQLITE)
        auto name = path.ToString();
        sqlite3* sqlite;
        int rc =sqlite3_open(name.c_str(),&sqlite);
        if(rc)
        {
            std::string error = sqlite3_errmsg(sqlite);
            throw std::runtime_error(error);
        }
        this->data = static_cast<void*>(sqlite);
        #endif
    }
    std::string SQLiteDatabase::Escape(std::string text)
    {
        std::string myStr = "\'";
        for(auto c : text)
        {
            if(c == '\'') myStr += "\'\'";
            else
                myStr += c;
        }
        myStr += '\'';
        return myStr;
    }
    bool SQLiteDatabase::IsEnabled()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_SQLITE)
        return true;
        #endif
        return false;
    }
    std::vector<std::vector<std::pair<std::string,std::optional<std::string>>>> SQLiteDatabase::Exec(std::string statement)
    {
        std::vector<std::vector<std::pair<std::string,std::optional<std::string>>>> items;
        
        Exec(statement,items);
       
        return items;
    }
    void SQLiteDatabase::Exec(std::string statement,std::vector<std::vector<std::pair<std::string,std::optional<std::string>>>>& items)
    {

        #if defined(TESSESFRAMEWORK_ENABLE_SQLITE)
        char* err;
        int res = sqlite3_exec(static_cast<sqlite3*>(this->data),statement.c_str(),SQLiteDatabase::collector,&items,&err);
        if(res != SQLITE_OK)
        {
            std::string errstr = err == nullptr ? "" : err;
            sqlite3_free(err);
            throw std::runtime_error(errstr);
        }
        #endif
    }
    SQLiteDatabase::~SQLiteDatabase()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_SQLITE)
        sqlite3_close(static_cast<sqlite3*>(this->data));
        #endif
    }
}
