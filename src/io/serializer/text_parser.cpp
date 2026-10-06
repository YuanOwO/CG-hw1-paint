#include "io/serializer/text_parser.hpp"

#include <algorithm>
#include <limits>
#include <locale>

#include "io/serializer/utils.hpp"

namespace paint::io::serializer {

TextParser::TextParser(std::istream& input, const std::string& formatName)
    : _input(input), _formatName(formatName) {
    // 文字格式固定使用英文語系，避免小數點受到作業系統語系影響。
    _input.imbue(std::locale::classic());
}

std::runtime_error TextParser::error(const std::string& message, std::size_t lineNumber) const {
    // 沒有指定行號時，錯誤位置就是下一筆原本應該出現的記錄。
    if (lineNumber == 0) {
        lineNumber = _lineNumber + 1;
    }
    return std::runtime_error("Invalid " + _formatName + " document at line " +
                              std::to_string(lineNumber) + ": " + message);
}

Record TextParser::readRecord() {
    std::string line;
    if (!utils::readLine(_input, line)) {
        throw error("unexpected end of file");
    }
    ++_lineNumber;

    Record record;
    record.fields = utils::splitFields(line);
    record.lineNumber = _lineNumber;

    if (record.fields.empty()) {
        throw error("unexpected empty record", record.lineNumber);
    }
    return record;
}

Record TextParser::expectRecord(const char* keyword, std::size_t fieldCount) {
    Record record = readRecord();
    if (record.fields[0] != keyword) {
        throw error(std::string("expected ") + keyword, record.lineNumber);
    }
    if (record.fields.size() != fieldCount) {
        throw error(std::string(keyword) + " has the wrong number of fields", record.lineNumber);
    }
    return record;
}

float TextParser::parseFloat(const std::string& token, const char* field,
                             std::size_t lineNumber) const {
    float value;
    if (!utils::parseFloat(token, value)) {
        throw error(std::string(field) + " must be a finite number", lineNumber);
    }
    return value;
}

int TextParser::parseNonNegativeInt(const std::string& token, const char* field,
                                    std::size_t lineNumber) const {
    long long value;
    if (!utils::parseInteger(token, value) || value < 0 ||
        value > std::numeric_limits<int>::max()) {
        throw error(std::string(field) + " must be a non-negative integer", lineNumber);
    }
    return static_cast<int>(value);
}

std::size_t TextParser::parseCount(const std::string& token, const char* field,
                                   std::size_t lineNumber) const {
    long long value;
    if (!utils::parseInteger(token, value) || value < 0 ||
        static_cast<unsigned long long>(value) > std::numeric_limits<std::size_t>::max()) {
        throw error(std::string(field) + " must be a non-negative integer", lineNumber);
    }
    return static_cast<std::size_t>(value);
}

std::string TextParser::readUtf8Payload(std::size_t byteCount) {
    std::string text;
    if (!utils::readExact(_input, text, byteCount)) {
        throw error("text payload is truncated");
    }

    // Payload 本身可以跨行，所以也要把其中的換行計入目前行號。
    _lineNumber += static_cast<std::size_t>(std::count(text.begin(), text.end(), '\n'));

    char delimiter;
    if (!_input.get(delimiter)) {
        throw error("text payload has no trailing newline");
    }
    if (delimiter == '\r') {
        if (!_input.get(delimiter) || delimiter != '\n') {
            throw error("text payload has an invalid trailing newline");
        }
    } else if (delimiter != '\n') {
        throw error("text payload has an invalid trailing newline");
    }
    ++_lineNumber;

    if (!utils::isValidUtf8(text)) {
        throw error("text payload is not valid UTF-8", _lineNumber);
    }
    return text;
}

void TextParser::expectEnd() {
    _input >> std::ws;
    if (_input.peek() != std::char_traits<char>::eof()) {
        throw error("unexpected data after end of document");
    }
}

}  // namespace paint::io::serializer
