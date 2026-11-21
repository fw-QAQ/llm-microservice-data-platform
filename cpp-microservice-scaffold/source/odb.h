#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include <odb/database.hxx>

namespace biteodb {

struct mysql_settings {
    std::string host = "mysql-service";
    std::string user;
    std::string passwd;
    std::string db;
    std::string cset = "utf8mb4";

    unsigned int port = 3306;
    std::size_t connection_pool_size = 3;
};

class DBFactory {
public:
    static std::shared_ptr<odb::database> mysql(
        const mysql_settings& settings
    );
};

} // namespace biteodb