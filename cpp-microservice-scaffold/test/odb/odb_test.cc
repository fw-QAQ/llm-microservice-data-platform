
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

#include <odb/transaction.hxx>
#include <odb/query.hxx>
#include <odb/result.hxx>
#include <odb/exceptions.hxx>

#include "odb.h"
#include "student.hxx"
#include "student-odb.hxx"

std::string get_env(const char* key)
{
    const char* value = std::getenv(key);

    if (!value || !*value) {
        throw std::runtime_error(
            std::string("Missing environment variable: ") + key
        );
    }

    return value;
}

int main()
{
    try {
        // 1. 初始化数据库配置
        biteodb::mysql_settings settings;

        settings.host = get_env("ODB_DB_HOST");
        settings.user = get_env("ODB_DB_USER");
        settings.passwd = get_env("ODB_DB_PASS");
        settings.db = get_env("ODB_DB_NAME");

        settings.port = 3306;
        settings.cset = "utf8mb4";
        settings.connection_pool_size = 5;

        // 2. 使用我们封装的工厂方法
        auto handler = biteodb::DBFactory::mysql(settings);

        // 3. 开启事务
        odb::transaction tx(handler->begin());

        // 4. 根据班级名称查询学生
        using Query = odb::query<ClassesStudent>;
        using Result = odb::result<ClassesStudent>;

        Result result = handler->query<ClassesStudent>(
            Query::Classes::name == "一年级一班"
        );

        // 5. 输出查询结果
for (const auto& item : result) {
    std::cout
        << "班级: " << item.classes_name
        << " 姓名: " << item.student_name
        << std::endl;
}

        tx.commit();

        std::cout << "ODB factory test passed!"
                  << std::endl;
    }
    catch (const odb::exception& e) {
        std::cerr << "ODB error: "
                  << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: "
                  << e.what() << std::endl;
        return 1;
    }

    return 0;
}
