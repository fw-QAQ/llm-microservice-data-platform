#include "odb.h"

#include <memory>
#include <stdexcept>
#include <utility>

#include <odb/mysql/database.hxx>

namespace biteodb {

std::shared_ptr<odb::database>DBFactory::mysql(const mysql_settings& settings)
{

    if(settings.port == 0 || settings.port > 65535
        || settings.connection_pool_size <= 0
        ||settings.host.empty()
        ||settings.user.empty()
        ||settings.db.empty()
        ||settings.cset.empty())
    {
        throw std::invalid_argument("settings are invalid");
    }

    auto pool =
        std::make_unique<odb::mysql::connection_pool_factory>(
            settings.connection_pool_size,
            0,
            true
        );

    std::shared_ptr<odb::database> db =
        std::make_shared<odb::mysql::database>(
            settings.user,
            settings.passwd,
            settings.db,
            settings.host,
            settings.port,
            nullptr,
            settings.cset,
            0,
            std::move(pool)
        );
    return db;
}

} // namespace biteodb