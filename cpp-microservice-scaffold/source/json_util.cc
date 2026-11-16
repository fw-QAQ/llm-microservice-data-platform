#include "json_util.h"

#include <memory>
#include <sstream>

namespace bitejson
{
    bool serialize(const Json::Value &value, std::string *str)
    {
        if (str == nullptr)
        {
            return false;
        }
        Json::StreamWriterBuilder builder;

        std::unique_ptr<Json::StreamWriter> psw(
            builder.newStreamWriter());

        std::stringstream ss;

        int ret = psw->write(value, &ss);
        if(ret != 0)
        {
            return false;
        }
        *str = ss.str();
        return true;
    }

    bool unserialize(const std::string &str,
                     Json::Value *value,
                     std::string *err)
    {
        if (value == nullptr || err == nullptr)
        {
            return false;
        }

        err->clear();
        Json::CharReaderBuilder build;

        std::unique_ptr<Json::CharReader>prc(build.newCharReader());
        bool ret = prc->parse(str.data(),str.data() + str.size(),value,err);
        return ret;
    }
}