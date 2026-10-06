#include "io/serializer/gptd_serializer.hpp"

#include <limits>
#include <locale>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "drawing/shape_object.hpp"
#include "drawing/text_object.hpp"

namespace paint::io {

namespace {

#pragma region Format Definitions

// Magic 與版本屬於 GPTD 格式，不應由 DocumentStorage 或應用程式名稱決定。
static const char GPT_DOCUMENT_HEADER[] = "GPTD 1";

// 下面的 using 只在這個檔案中使用，讓格式轉換程式不會被過長的 namespace 影響閱讀。
using drawing::EllipseShape;
using drawing::LineShape;
using drawing::ObjectKind;
using drawing::PathShape;
using drawing::PointShape;
using drawing::PolygonShape;
using drawing::RectangleShape;
using drawing::SceneObject;
using drawing::ShapeKind;
using drawing::ShapeObject;
using drawing::ShapeStyle;
using drawing::TextObject;
using drawing::TextStyle;
using drawing::TwoPointShape;

#pragma endregion  // Format Definitions

#pragma region Common Values

// enum 轉換失敗時加上格式名稱，呼叫端顯示時才知道是哪一層出錯。
std::runtime_error makeSerializationError(const std::string& message) {
    return std::runtime_error("Cannot serialize GPTD document: " + message);
}

// 顏色一律寫入 RGBA 分量，不儲存色名，才能完整保留自訂顏色。
void writeColor(std::ostream& output, const ColorRGBA& color) {
    output << color.r << ' ' << color.g << ' ' << color.b << ' ' << color.a;
}

void writePoint(std::ostream& output, const Point& point) {
    output << point.x() << ' ' << point.y();
}

#pragma endregion  // Common Values

#pragma region Enum Names

// 檔案保存穩定的名稱，不直接保存 enum 整數，避免未來調整列舉順序後誤讀舊檔。
const char* fillModeName(drawing::FillMode mode) {
    switch (mode) {
    case drawing::FillMode::OUTLINE:
        return "outline";
    case drawing::FillMode::FILLED:
        return "filled";
    case drawing::FillMode::ADVANCED:
        return "advanced";
    }

    throw makeSerializationError("unknown fill mode");
}

const char* lineJoinName(drawing::LineJoin join) {
    switch (join) {
    case drawing::LineJoin::NONE:
        return "none";
    case drawing::LineJoin::MITER:
        return "miter";
    case drawing::LineJoin::BEVEL:
        return "bevel";
    case drawing::LineJoin::ROUND:
        return "round";
    }

    throw makeSerializationError("unknown line join");
}

const char* lineCapName(drawing::LineCap cap) {
    switch (cap) {
    case drawing::LineCap::BUTT:
        return "butt";
    case drawing::LineCap::SQUARE:
        return "square";
    case drawing::LineCap::ROUND:
        return "round";
    }

    throw makeSerializationError("unknown line cap");
}

const char* shapeKindName(ShapeKind kind) {
    switch (kind) {
    case ShapeKind::Point:
        return "point";
    case ShapeKind::Line:
        return "line";
    case ShapeKind::Rectangle:
        return "rectangle";
    case ShapeKind::Ellipse:
        return "ellipse";
    case ShapeKind::Path:
        return "path";
    case ShapeKind::Polygon:
        return "polygon";
    }

    throw makeSerializationError("unknown shape kind");
}

#pragma endregion  // Enum Names

#pragma region Shape Serialization

// Shape style 的欄位順序與 gpt_file_format.md 相同，即使某種圖形沒有用到也完整寫出。
void writeShapeStyle(std::ostream& output, const ShapeStyle& style) {
    output << "SHAPE_STYLE " << fillModeName(style.fillMode) << ' ' << style.pointSize << '\n';
    output << "FILL ";
    writeColor(output, style.fill.color);
    output << '\n';
    output << "STROKE " << style.stroke.width << ' ';
    writeColor(output, style.stroke.color);
    output << ' ' << lineJoinName(style.stroke.join) << ' ' << lineCapName(style.stroke.cap) << ' '
           << style.stroke.miterLimit << '\n';
}

void writeTwoPointGeometry(std::ostream& output, const TwoPointShape& shape) {
    output << "BOUNDS ";
    writePoint(output, shape.start());
    output << ' ';
    writePoint(output, shape.end());
    output << '\n';
}

// Path 與 Polygon 使用計數加逐點記錄，反序列化時才能先檢查資料量。
void writePointList(std::ostream& output, const std::vector<Point>& points) {
    output << "POINTS " << points.size() << '\n';
    for (const Point& point : points) {
        output << "POINT ";
        writePoint(output, point);
        output << '\n';
    }
}

void writeShape(std::ostream& output, const ShapeObject& shape) {
    output << "OBJECT shape\n";
    output << "SHAPE " << shapeKindName(shape.shapeKind()) << '\n';
    writeShapeStyle(output, shape.style());

    // 這裡保存的是物件原本的幾何參數。矩形與橢圓不能寫成繪製時的採樣點，
    // 否則載入後就無法還原成原本的參數化圖形。
    // 每個 Shape class 都固定回傳自己的 ShapeKind，因此這裡可以直接轉回對應類型。
    switch (shape.shapeKind()) {
    case ShapeKind::Point: {
        const auto& point = static_cast<const PointShape&>(shape);
        output << "POSITION ";
        writePoint(output, point.getPosition());
        output << '\n';
        break;
    }
    case ShapeKind::Line:
        writeTwoPointGeometry(output, static_cast<const LineShape&>(shape));
        break;
    case ShapeKind::Rectangle:
        writeTwoPointGeometry(output, static_cast<const RectangleShape&>(shape));
        break;
    case ShapeKind::Ellipse:
        writeTwoPointGeometry(output, static_cast<const EllipseShape&>(shape));
        break;
    case ShapeKind::Path:
        writePointList(output, static_cast<const PathShape&>(shape).getVertices());
        break;
    case ShapeKind::Polygon:
        writePointList(output, static_cast<const PolygonShape&>(shape).getVertices());
        break;
    }

    output << "END_OBJECT\n";
}

#pragma endregion  // Shape Serialization

#pragma region Font Serialization

const char* bitmapFontName(BitmapFont font) {
    switch (font) {
    case BitmapFont::BITMAP_8_BY_13:
        return "8x13";
    case BitmapFont::BITMAP_9_BY_15:
        return "9x15";
    case BitmapFont::BITMAP_HELVETICA_10:
        return "helvetica-10";
    case BitmapFont::BITMAP_HELVETICA_12:
        return "helvetica-12";
    case BitmapFont::BITMAP_HELVETICA_18:
        return "helvetica-18";
    case BitmapFont::BITMAP_TIMES_ROMAN_10:
        return "times-roman-10";
    case BitmapFont::BITMAP_TIMES_ROMAN_24:
        return "times-roman-24";
    }

    throw makeSerializationError("unknown bitmap font");
}

const char* strokeFontName(StrokeFont font) {
    switch (font) {
    case StrokeFont::STROKE_ROMAN:
        return "roman";
    case StrokeFont::STROKE_MONO_ROMAN:
        return "mono-roman";
    }

    throw makeSerializationError("unknown stroke font");
}

const char* gfntFontName(GfntFontId font) {
    switch (font) {
    case GfntFontId::CUBIC_11:
        return "cubic-11";
    case GfntFontId::UNIFONT_16:
        return "unifont-16";
    }

    throw makeSerializationError("unknown GFNT font");
}

void writeFont(std::ostream& output, const FontStyle& font) {
    // FontStyle 是 variant；這裡依實際類型寫出對應的 font-kind 與參數。
    if (const auto* style = std::get_if<BitmapFontStyle>(&font)) {
        output << "FONT bitmap " << bitmapFontName(style->font) << '\n';
        return;
    }

    if (const auto* style = std::get_if<StrokeFontStyle>(&font)) {
        output << "FONT stroke " << strokeFontName(style->font) << ' ' << style->size << '\n';
        return;
    }

    if (const auto* style = std::get_if<GfntFontStyle>(&font)) {
        output << "FONT gfnt " << gfntFontName(style->font) << '\n';
        return;
    }

    throw makeSerializationError("unknown font style");
}

#pragma endregion  // Font Serialization

#pragma region Text Serialization

void writeText(std::ostream& output, const TextObject& text) {
    const TextStyle& style = text.style();

    output << "OBJECT text\n";
    output << "POSITION ";
    writePoint(output, text.position());
    output << '\n';
    output << "TEXT_STYLE ";
    writeColor(output, style.color);
    output << ' ' << style.lineSpacing << '\n';
    writeFont(output, style.font);

    // 文字可能含有換行、前置空白，也可能是空字串，因此先寫明 UTF-8 byte 數。
    output << "TEXT " << text.text().size() << '\n';
    output.write(text.text().data(), static_cast<std::streamsize>(text.text().size()));
    output << "\nEND_OBJECT\n";
}

#pragma endregion  // Text Serialization

#pragma region Object Dispatch

// OBJECT 只分 shape 與 text；這兩個 kind 由對應的 class 固定回傳。
// shape 的細部類型再交給 writeShape 處理。
void writeObject(std::ostream& output, const SceneObject& object) {
    switch (object.objectKind()) {
    case ObjectKind::Shape:
        writeShape(output, static_cast<const ShapeObject&>(object));
        return;
    case ObjectKind::Text:
        writeText(output, static_cast<const TextObject&>(object));
        return;
    }

    throw makeSerializationError("unknown object kind");
}

#pragma endregion  // Object Dispatch

}  // namespace

#pragma region Serializer Interface

void GptdSerializer::serialize(std::ostream& output, const app::DocumentData& document) const {
    // 固定 locale 可避免小數點被寫成逗號；max_digits10 則讓 float 載入後不失真。
    output.imbue(std::locale::classic());
    output.precision(std::numeric_limits<float>::max_digits10);

    output << GPT_DOCUMENT_HEADER << '\n';
    output << "CANVAS " << document.canvasWidth << ' ' << document.canvasHeight << '\n';
    output << "OBJECTS " << document.scene.size() << '\n';

    for (const auto& object : document.scene.objects()) {
        if (!object) {
            throw makeSerializationError("scene contains a null object");
        }
        writeObject(output, *object);
    }

    output << "END\n";
    if (!output) {
        throw std::runtime_error("Failed while serializing GPTD document");
    }
}

app::DocumentData GptdSerializer::deserialize(std::istream& input) const {
    (void)input;
    throw std::runtime_error("GPTD deserialization is not implemented yet");
}

#pragma endregion  // Serializer Interface

}  // namespace paint::io
