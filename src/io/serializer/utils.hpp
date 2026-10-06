#pragma once

#include <cstddef>
#include <istream>
#include <string>
#include <vector>

namespace paint::io::serializer::utils {

// 讀取一行並統一移除 LF 或 CRLF。
bool readLine(std::istream& input, std::string& line);

// 將記錄依空白分成欄位，不在這一層解釋任何格式關鍵字。
std::vector<std::string> splitFields(const std::string& line);

// 只接受完整數字；尾端含有其他字元時回傳 false。
bool parseFloat(const std::string& text, float& value);
bool parseInteger(const std::string& text, long long& value);

// 按指定 byte 數讀取，適合文字 payload 或二進位資料。
bool readExact(std::istream& input, std::string& data, std::size_t size);

bool isValidUtf8(const std::string& text);

}  // namespace paint::io::serializer::utils
