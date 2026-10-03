#pragma once

#include <cstddef>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "command/command_history.hpp"
#include "drawing/scene.hpp"
#include "drawing/shape.hpp"

using paint::drawing::Scene;
using paint::drawing::Shape;
using Path = std::filesystem::path;

namespace paint::app {

struct DocumentData {
    Path filename;
    Scene scene;

    int canvasWidth = 0;
    int canvasHeight = 0;

    // Other Metadata
    // 未來可以加入：
    // ColorRGBA background;
};

class Document {
   public:
    Document() {}
    Document(const Path& filename) : _filename(filename) {}
    Document(DocumentData data)
        : _filename(std::move(data.filename)),
          _scene(std::move(data.scene)),
          _canvasWidth(data.canvasWidth),
          _canvasHeight(data.canvasHeight) {}

    // 禁止拷貝與移動操作，確保元素的唯一性
    Document(const Document&) = delete;
    Document& operator=(const Document&) = delete;
    Document(Document&&) = delete;
    Document& operator=(Document&&) = delete;

    DocumentData data() const;  // 打包 Scene 與其他 Metadata 成 DocumentData 回傳。

    const Scene& scene() const { return _scene; }

    Path filename() const { return _filename; }
    void setFilename(const Path& filename) { _filename = filename; }

    int canvasWidth() const { return _canvasWidth; }
    int canvasHeight() const { return _canvasHeight; }
    // 目前記錄可見畫布尺寸；0 表示尚未設定或沒有可繪製區域。
    void setCanvasSize(int width, int height);

    bool isModified() const { return _history.isModified(); }
    void markSaved() { _history.markSaved(); }

    void replaceContent(Scene scene, Path filename);

    // 文件操作

    void newFile();
    void load(const Path& filename);
    void save();  // 保存到當前文件名
    void save(const Path& filename);
    void exportImage(const Path& filename);

    // 編輯操作

    void addShape(std::shared_ptr<Shape> shape);
    void insertShape(std::size_t index, std::shared_ptr<Shape> shape);
    void removeShape(std::shared_ptr<Shape> shape);
    void clearScene();

    void undo();
    void redo();

    bool canUndo() const { return _history.canUndo(); }
    bool canRedo() const { return _history.canRedo(); }

    // 事件訂閱
    std::function<void(bool)> onModifiedChanged;

   private:
    Path _filename;
    int _canvasWidth = 0;
    int _canvasHeight = 0;
    CommandHistory _history;
    Scene _scene;

    void onModifiedChangedInternal() {
        if (onModifiedChanged) {
            onModifiedChanged(isModified());
        }
    }
};

}  // namespace paint::app
