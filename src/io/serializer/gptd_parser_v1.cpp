#include "io/serializer/gptd_parser_v1.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "drawing/shape_object.hpp"
#include "drawing/text_object.hpp"

namespace paint::io {

using drawing::EllipseShape;
using drawing::LineShape;
using drawing::PathShape;
using drawing::PointShape;
using drawing::PolygonShape;
using drawing::RectangleShape;
using drawing::SceneObject;
using drawing::ShapeKind;
using drawing::ShapeStyle;
using drawing::TextObject;
using drawing::TextStyle;
using drawing::TwoPointShape;

#pragma region Deserialization Helpers

Point GptdParserV1::parsePoint(const Record& record, std::size_t firstValue) const {
    return {parseFloat(record.fields[firstValue], "x coordinate", record.lineNumber),
            parseFloat(record.fields[firstValue + 1], "y coordinate", record.lineNumber)};
}

ColorRGBA GptdParserV1::parseColor(const Record& record, std::size_t firstValue) const {
    // 檔案可能來自外部，所以不依賴 ColorRGBA 的建構式自動修正數值。
    ColorRGBA color{
        parseFloat(record.fields[firstValue], "red channel", record.lineNumber),
        parseFloat(record.fields[firstValue + 1], "green channel", record.lineNumber),
        parseFloat(record.fields[firstValue + 2], "blue channel", record.lineNumber),
        parseFloat(record.fields[firstValue + 3], "alpha channel", record.lineNumber),
    };

    if (color.r < 0.0f || color.r > 1.0f || color.g < 0.0f || color.g > 1.0f || color.b < 0.0f ||
        color.b > 1.0f || color.a < 0.0f || color.a > 1.0f) {
        throw error("color channels must be between 0 and 1", record.lineNumber);
    }
    return color;
}

drawing::FillMode GptdParserV1::parseFillMode(const std::string& token, std::size_t lineNumber) const {
    if (token == "outline") return drawing::FillMode::OUTLINE;
    if (token == "filled") return drawing::FillMode::FILLED;
    if (token == "advanced") return drawing::FillMode::ADVANCED;
    throw error("unknown fill mode: " + token, lineNumber);
}

drawing::LineJoin GptdParserV1::parseLineJoin(const std::string& token, std::size_t lineNumber) const {
    if (token == "none") return drawing::LineJoin::NONE;
    if (token == "miter") return drawing::LineJoin::MITER;
    if (token == "bevel") return drawing::LineJoin::BEVEL;
    if (token == "round") return drawing::LineJoin::ROUND;
    throw error("unknown line join: " + token, lineNumber);
}

drawing::LineCap GptdParserV1::parseLineCap(const std::string& token, std::size_t lineNumber) const {
    if (token == "butt") return drawing::LineCap::BUTT;
    if (token == "square") return drawing::LineCap::SQUARE;
    if (token == "round") return drawing::LineCap::ROUND;
    throw error("unknown line cap: " + token, lineNumber);
}

ShapeKind GptdParserV1::parseShapeKind(const std::string& token, std::size_t lineNumber) const {
    if (token == "point") return ShapeKind::Point;
    if (token == "line") return ShapeKind::Line;
    if (token == "rectangle") return ShapeKind::Rectangle;
    if (token == "ellipse") return ShapeKind::Ellipse;
    if (token == "path") return ShapeKind::Path;
    if (token == "polygon") return ShapeKind::Polygon;
    throw error("unknown shape kind: " + token, lineNumber);
}

#pragma endregion  // Deserialization Helpers

#pragma region Shape Deserialization

ShapeStyle GptdParserV1::parseShapeStyle() {
    // Shape style 在檔案中固定由三筆記錄組成，載入後再合併回一個物件。
    Record shapeStyleRecord = expectRecord("SHAPE_STYLE", 3);
    Record fillRecord = expectRecord("FILL", 5);
    Record strokeRecord = expectRecord("STROKE", 9);

    ShapeStyle style;
    style.fillMode = parseFillMode(shapeStyleRecord.fields[1], shapeStyleRecord.lineNumber);
    style.pointSize =
        parseFloat(shapeStyleRecord.fields[2], "point size", shapeStyleRecord.lineNumber);
    style.fill.color = parseColor(fillRecord, 1);
    style.stroke.width =
        parseFloat(strokeRecord.fields[1], "stroke width", strokeRecord.lineNumber);
    style.stroke.color = parseColor(strokeRecord, 2);
    style.stroke.join = parseLineJoin(strokeRecord.fields[6], strokeRecord.lineNumber);
    style.stroke.cap = parseLineCap(strokeRecord.fields[7], strokeRecord.lineNumber);
    style.stroke.miterLimit =
        parseFloat(strokeRecord.fields[8], "miter limit", strokeRecord.lineNumber);

    if (style.pointSize < 1.0f || style.stroke.width < 1.0f || style.stroke.miterLimit < 1.0f) {
        throw error("point size, stroke width, and miter limit must be at least 1",
                    strokeRecord.lineNumber);
    }
    return style;
}

std::vector<Point> GptdParserV1::parsePointList() {
    Record pointsRecord = expectRecord("POINTS", 2);
    std::size_t count =
        parseCount(pointsRecord.fields[1], "point count", pointsRecord.lineNumber);

    // 先 reserve 只是避免大型 path 反覆擴充 vector，不另外限制點數。
    std::vector<Point> points;
    points.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        Record pointRecord = expectRecord("POINT", 3);
        points.push_back(parsePoint(pointRecord, 1));
    }
    return points;
}

std::shared_ptr<SceneObject> GptdParserV1::parseShape() {
    // 先解析共用 style，再依 ShapeKind 讀取對應的 geometry。
    Record shapeRecord = expectRecord("SHAPE", 2);
    ShapeKind kind = parseShapeKind(shapeRecord.fields[1], shapeRecord.lineNumber);
    ShapeStyle style = parseShapeStyle();
    std::shared_ptr<SceneObject> object;

    switch (kind) {
    case ShapeKind::Point: {
        Record positionRecord = expectRecord("POSITION", 3);
        auto shape = std::make_shared<PointShape>(style);
        shape->setPosition(parsePoint(positionRecord, 1));
        object = shape;
        break;
    }
    case ShapeKind::Line:
    case ShapeKind::Rectangle:
    case ShapeKind::Ellipse: {
        Record boundsRecord = expectRecord("BOUNDS", 5);
        std::shared_ptr<TwoPointShape> shape;
        if (kind == ShapeKind::Line) shape = std::make_shared<LineShape>(style);
        if (kind == ShapeKind::Rectangle) shape = std::make_shared<RectangleShape>(style);
        if (kind == ShapeKind::Ellipse) shape = std::make_shared<EllipseShape>(style);
        shape->setStart(parsePoint(boundsRecord, 1));
        shape->setEnd(parsePoint(boundsRecord, 3));
        object = shape;
        break;
    }
    case ShapeKind::Path: {
        auto shape = std::make_shared<PathShape>(style);
        // addPoint 會為了滑鼠輸入過濾過密點，載入檔案時必須直接還原。
        shape->setPoints(parsePointList());
        object = shape;
        break;
    }
    case ShapeKind::Polygon: {
        auto shape = std::make_shared<PolygonShape>(style);
        shape->setPoints(parsePointList());
        object = shape;
        break;
    }
    }

    expectRecord("END_OBJECT", 1);
    return object;
}

#pragma endregion  // Shape Deserialization

#pragma region Text Deserialization

FontStyle GptdParserV1::parseFont() {
    // FONT 的欄位數會依 font-kind 改變，因此不能使用固定欄位數的 expectRecord。
    Record record = readRecord();
    if (record.fields[0] != "FONT") {
        throw error("expected FONT", record.lineNumber);
    }

    if (record.fields.size() == 3 && record.fields[1] == "bitmap") {
        const std::string& name = record.fields[2];
        if (name == "8x13") return BitmapFontStyle{BitmapFont::BITMAP_8_BY_13};
        if (name == "9x15") return BitmapFontStyle{BitmapFont::BITMAP_9_BY_15};
        if (name == "helvetica-10") return BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_10};
        if (name == "helvetica-12") return BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_12};
        if (name == "helvetica-18") return BitmapFontStyle{BitmapFont::BITMAP_HELVETICA_18};
        if (name == "times-roman-10") return BitmapFontStyle{BitmapFont::BITMAP_TIMES_ROMAN_10};
        if (name == "times-roman-24") return BitmapFontStyle{BitmapFont::BITMAP_TIMES_ROMAN_24};
        throw error("unknown bitmap font: " + name, record.lineNumber);
    }

    if (record.fields.size() == 4 && record.fields[1] == "stroke") {
        StrokeFont font;
        if (record.fields[2] == "roman") {
            font = StrokeFont::STROKE_ROMAN;
        } else if (record.fields[2] == "mono-roman") {
            font = StrokeFont::STROKE_MONO_ROMAN;
        } else {
            throw error("unknown stroke font: " + record.fields[2], record.lineNumber);
        }

        float size = parseFloat(record.fields[3], "stroke font size", record.lineNumber);
        if (size <= 0.0f) {
            throw error("stroke font size must be positive", record.lineNumber);
        }
        return StrokeFontStyle{font, size};
    }

    if (record.fields.size() == 3 && record.fields[1] == "gfnt") {
        if (record.fields[2] == "cubic-11") return GfntFontStyle{GfntFontId::CUBIC_11};
        if (record.fields[2] == "unifont-16") return GfntFontStyle{GfntFontId::UNIFONT_16};
        throw error("unknown GFNT font: " + record.fields[2], record.lineNumber);
    }

    throw error("invalid FONT record", record.lineNumber);
}

std::shared_ptr<SceneObject> GptdParserV1::parseText() {
    // 文字內容最後才讀，因為它不是一般的單行記錄。
    Record positionRecord = expectRecord("POSITION", 3);
    Record styleRecord = expectRecord("TEXT_STYLE", 6);

    TextStyle style;
    style.color = parseColor(styleRecord, 1);
    style.lineSpacing =
        parseFloat(styleRecord.fields[5], "line spacing", styleRecord.lineNumber);
    if (style.lineSpacing <= 0.0f) {
        throw error("line spacing must be positive", styleRecord.lineNumber);
    }
    style.font = parseFont();

    Record textRecord = expectRecord("TEXT", 2);
    std::size_t byteCount =
        parseCount(textRecord.fields[1], "text byte count", textRecord.lineNumber);
    std::string text = readUtf8Payload(byteCount);
    expectRecord("END_OBJECT", 1);

    return std::make_shared<TextObject>(parsePoint(positionRecord, 1), text, style);
}

std::shared_ptr<SceneObject> GptdParserV1::parseObject() {
    // OBJECT 只決定下一層 parser；shape 的具體種類由 SHAPE 記錄決定。
    Record objectRecord = expectRecord("OBJECT", 2);
    if (objectRecord.fields[1] == "shape") return parseShape();
    if (objectRecord.fields[1] == "text") return parseText();
    throw error("unknown object kind: " + objectRecord.fields[1], objectRecord.lineNumber);
}

#pragma endregion  // Text Deserialization

#pragma region Document Deserialization

app::DocumentData GptdParserV1::parse() {
    // 先檢查 magic 與版本，避免把其他格式當成 GPTD 繼續解析。
    Record header = readRecord();
    if (header.fields.size() != 2 || header.fields[0] != "GPTD") {
        throw error("invalid GPTD magic", header.lineNumber);
    }
    if (header.fields[1] != "1") {
        throw error("unsupported GPTD version: " + header.fields[1], header.lineNumber);
    }

    Record canvasRecord = expectRecord("CANVAS", 3);
    Record objectsRecord = expectRecord("OBJECTS", 2);

    app::DocumentData document;
    document.canvasWidth =
        parseNonNegativeInt(canvasRecord.fields[1], "canvas width", canvasRecord.lineNumber);
    document.canvasHeight =
        parseNonNegativeInt(canvasRecord.fields[2], "canvas height", canvasRecord.lineNumber);

    std::size_t objectCount =
        parseCount(objectsRecord.fields[1], "object count", objectsRecord.lineNumber);

    // 先在暫時 DocumentData 建立完整 Scene；任何一筆失敗都不會修改目前文件。
    for (std::size_t i = 0; i < objectCount; ++i) {
        document.scene.add(parseObject());
    }

    expectRecord("END", 1);

    // END 後可以有換行或空白，但不能再有其他記錄。
    expectEnd();
    return document;
}

#pragma endregion  // Document Deserialization

}  // namespace paint::io
