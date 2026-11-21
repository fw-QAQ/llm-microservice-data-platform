
#include <curl/curl.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

size_t callback(
    char* buffer,
    size_t size,
    size_t nitems,
    void* userdata
) {
    auto* ss = static_cast<std::stringstream*>(userdata);

    ss->read(buffer, size * nitems);

    return static_cast<size_t>(ss->gcount());
}

int main() {
    const char* user = std::getenv("MAIL_USER");
    const char* pass = std::getenv("MAIL_PASS");
    const char* recipient = std::getenv("MAIL_TO");

    if (!user || !pass || !recipient) {
        std::cerr << "Missing mail environment variables\n";
        return 1;
    }

    const std::string username = user;
    const std::string password = pass;
    const std::string to = recipient;

    const std::string url = "smtps://smtp.163.com:465";

    // 1. 初始化全局环境
    CURLcode ret = curl_global_init(CURL_GLOBAL_DEFAULT);

    if (ret != CURLE_OK) {
        std::cerr << curl_easy_strerror(ret) << '\n';
        return 1;
    }

    // 2. 创建 CURL 句柄
    CURL* curl = curl_easy_init();

    if (!curl) {
        curl_global_cleanup();
        return 1;
    }

    // 3. 创建收件人列表
    curl_slist* recipients =
        curl_slist_append(nullptr, to.c_str());

    if (!recipients) {
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    // 4. 构造邮件内容
    std::stringstream body;

    body << "From: " << username << "\r\n";
    body << "To: " << to << "\r\n";
    body << "Subject: Verification code\r\n";
    body << "MIME-Version: 1.0\r\n";
    body << "Content-Type: text/html; charset=UTF-8\r\n";
    body << "\r\n";
    body << "<html><body>";
    body << "<p>Your verification code: <b>6789</b></p>";
    body << "</body></html>\r\n";

    // 5. 配置 SMTP
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(
        curl, CURLOPT_USERNAME, username.c_str()
    );
    curl_easy_setopt(
        curl, CURLOPT_PASSWORD, password.c_str()
    );
    curl_easy_setopt(
        curl, CURLOPT_MAIL_FROM, username.c_str()
    );
    curl_easy_setopt(
        curl, CURLOPT_MAIL_RCPT, recipients
    );

    // 6. 配置邮件内容回调
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &body);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    // 7. 配置超时
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    // 资料中的开发测试选项如下：
    // CURLOPT_SSL_VERIFYPEER = 0L
    // CURLOPT_SSL_VERIFYHOST = 0L
    // 为复现该行为，可在受控测试环境编译时定义宏；
    // 真实 SMTP 账户连接公网服务器时保持默认验证。
#ifdef COURSE_DEMO_SKIP_TLS_VERIFY
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

    // 8. 执行 SMTP 邮件传输
    ret = curl_easy_perform(curl);

    if (ret != CURLE_OK) {
        std::cerr << "SMTP failed: "
                  << curl_easy_strerror(ret) << '\n';
    } else {
        std::cout << "SMTP transfer succeeded\n";
    }

    // 9. 释放资源
    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return ret == CURLE_OK ? 0 : 1;
}
