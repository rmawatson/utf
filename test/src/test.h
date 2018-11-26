#include <utf/utf.h>
#include <string>
#include <fstream>
#include <iostream>

template <typename T>
std::basic_string<T> read_file(const std::string& filename)
{
     std::basic_ifstream<char> data(filename.c_str(),std::ios::binary);
     return std::basic_string<T>(std::istreambuf_iterator<char>(data),std::istreambuf_iterator<char>());
}

template <>
inline std::basic_string<char16_t> read_file<char16_t>(const std::string& filename)
{
     std::basic_ifstream<char> data(filename.c_str(),std::ios::binary);
     utf::bytes_to_short_iterator<std::istreambuf_iterator<char>> iter(std::istreambuf_iterator<char>{data},std::istreambuf_iterator<char>{});
     utf::bytes_to_short_iterator<std::istreambuf_iterator<char>> end(std::istreambuf_iterator<char>{},std::istreambuf_iterator<char>{});
     return std::basic_string<char16_t>(iter,end);
}

template <>
inline std::basic_string<char32_t> read_file<char32_t>(const std::string& filename)
{
     std::basic_ifstream<char> data(filename.c_str(),std::ios::binary);
     utf::bytes_to_long_iterator<std::istreambuf_iterator<char>> iter(std::istreambuf_iterator<char>{data},std::istreambuf_iterator<char>{});
     utf::bytes_to_long_iterator<std::istreambuf_iterator<char>> end(std::istreambuf_iterator<char>{},std::istreambuf_iterator<char>{});
     return std::basic_string<char32_t>(iter,end);
}

     