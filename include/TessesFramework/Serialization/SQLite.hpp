#pragma once
#include "TessesFramework/Filesystem/VFSFix.hpp"
#include "TessesFramework/Filesystem/VFS.hpp"
#include <optional>
namespace Tesses::Framework::Serialization {
    class SQLiteDatabase {
        private:
            void* data;
            static int collector(void* user, int count,char** vals, char** keys);
        public:
            SQLiteDatabase(Tesses::Framework::Filesystem::VFSPath path);
            static std::string Escape(std::string text);
            static bool IsEnabled();
            void Exec(std::string statement,std::vector<std::vector<std::pair<std::string,std::optional<std::string>>>>& results);
            std::vector<std::vector<std::pair<std::string,std::optional<std::string>>>> Exec(std::string statement);
            ~SQLiteDatabase();
    };
}