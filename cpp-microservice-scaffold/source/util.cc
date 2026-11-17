#include "util.h"
#include <random>

namespace biteutil {

void STR::split(const std::string &str, const std::string &sep,
                std::vector<std::string> &result) {

  if (str.empty() || sep.empty()) {
    return;
  }

  result.clear();

  auto pos = 0;
  auto end = str.find(sep, pos);
  while (end != std::string::npos) {
    if (end > pos) {
      result.push_back(str.substr(pos, end - pos));
    }
    pos = end + sep.size();
    end = str.find(sep, pos);
  }

  if (pos < str.size()) {
    result.push_back(str.substr(pos));
  }
}
std::string Random::code() {

  std::random_device rd;
  std::seed_seq seed{rd(), rd(), rd(), rd()};
  std::mt19937_64 engine(seed);

  std::uniform_int_distribution<int> dist(0, 15);

  const std::string chars = "0123456789abcdef";


  std::string ret ="";
  for(int i = 0;i < 32;i++)
  {
    int index = dist(engine);
    char c = chars[index];
    ret.push_back(c);
  }

  return ret;
}

} // namespace biteutil