#pragma once

#include "io/serializer/gptd_dumper_v1.hpp"
#include "io/serializer/gptd_parser_v1.hpp"
#include "io/serializer/serializer.hpp"

namespace paint::io {

class GptdSerializer : public Serializer {
   public:
    void serialize(std::ostream& output, const app::DocumentData& document) const override {
        // GptdSerializer 只負責選擇格式版本，實際寫入規則交給對應版本的 dumper。
        GptdDumperV1 dumper(output);
        dumper.dump(document);
    }

    app::DocumentData deserialize(std::istream& input) const override {
        // 目前只有 V1；新增格式版本時，可以在這一層依檔頭選擇對應的 parser。
        GptdParserV1 parser(input);
        return parser.parse();
    }
};

}  // namespace paint::io
