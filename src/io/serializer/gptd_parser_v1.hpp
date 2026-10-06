#pragma once

#include <cstddef>
#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "app/document.hpp"
#include "common/color.hpp"
#include "common/font.hpp"
#include "common/point.hpp"
#include "drawing/scene_object.hpp"
#include "drawing/shape_object.hpp"
#include "io/serializer/text_parser.hpp"

namespace paint::io {

class GptdParserV1 : public serializer::TextParser {
   public:
    GptdParserV1(std::istream& input) : TextParser(input, "GPTD v1") {}

    // 只有整份文件都符合 GPTD v1 格式時，才回傳完整的 DocumentData。
    app::DocumentData parse();

   private:
    using Record = serializer::Record;

    Point parsePoint(const Record& record, std::size_t firstValue) const;
    ColorRGBA parseColor(const Record& record, std::size_t firstValue) const;

    drawing::FillMode parseFillMode(const std::string& token, std::size_t lineNumber) const;
    drawing::LineJoin parseLineJoin(const std::string& token, std::size_t lineNumber) const;
    drawing::LineCap parseLineCap(const std::string& token, std::size_t lineNumber) const;
    drawing::ShapeKind parseShapeKind(const std::string& token, std::size_t lineNumber) const;

    void parseShapeStyle(drawing::PaintStyle& paint, drawing::ShapeStyle& style);
    std::vector<Point> parsePointList();
    std::shared_ptr<drawing::SceneObject> parseShape();

    FontStyle parseFont();
    std::shared_ptr<drawing::SceneObject> parseText();
    std::shared_ptr<drawing::SceneObject> parseObject();
};

}  // namespace paint::io
