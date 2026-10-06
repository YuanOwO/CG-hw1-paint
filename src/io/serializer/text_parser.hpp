#pragma once

#include <cstddef>
#include <istream>
#include <stdexcept>
#include <string>
#include <vector>

namespace paint::io::serializer {

// 一筆以空白分隔的文字記錄。保留行號是為了讓各格式能提供清楚的錯誤訊息。
struct Record {
    std::vector<std::string> fields;
    std::size_t lineNumber;
};

// 共用的文字格式解析器。它只處理輸入、型別與行號，不理解任何特定格式的 grammar。
class TextParser {
   protected:
    TextParser(std::istream& input, const std::string& formatName);

    std::runtime_error error(const std::string& message, std::size_t lineNumber = 0) const;

    Record readRecord();
    Record expectRecord(const char* keyword, std::size_t fieldCount);

    float parseFloat(const std::string& token, const char* field, std::size_t lineNumber) const;
    int parseNonNegativeInt(const std::string& token, const char* field,
                            std::size_t lineNumber) const;
    std::size_t parseCount(const std::string& token, const char* field,
                           std::size_t lineNumber) const;

    // 讀取有明確 byte 長度的 UTF-8 內容，並吃掉內容後方的換行分隔符。
    std::string readUtf8Payload(std::size_t byteCount);

    // 允許檔尾有空白，但不允許再出現其他資料。
    void expectEnd();

   private:
    std::istream& _input;
    std::string _formatName;
    std::size_t _lineNumber = 0;
};

}  // namespace paint::io::serializer
