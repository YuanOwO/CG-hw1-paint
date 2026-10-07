#pragma once

#include <utility>

#include "app/document.hpp"
#include "command/command.hpp"
#include "drawing/scene.hpp"
#include "io/document_storage.hpp"
#include "io/image_exporter.hpp"
#include "io/serializer/gptd_serializer.hpp"

using paint::app::Document;

namespace paint {

class FileCommand : public ICommand {
   public:
    FileCommand(Document& document) : _document(document) {}

   protected:
    Document& _document;
};

class NewFileCommand : public FileCommand {
   public:
    NewFileCommand(Document& document) : FileCommand(document) {}

    void execute() override {
        _document.replaceContent(Scene{}, "");  // 清空場景並重置文件名
    }
};

class OpenCommand : public FileCommand {
   public:
    OpenCommand(Document& document, const Path& filename) : FileCommand(document), _filename(filename) {}

    void execute() override {
        io::GptdSerializer serializer;
        auto data = io::DocumentStorage::read(_filename, serializer);

        // read 完全成功後才替換原文件。
        _document.replaceContent(std::move(data));
    }

   private:
    Path _filename;
};

class SaveCommand : public FileCommand {
   public:
    SaveCommand(Document& document, const Path& filename) : FileCommand(document), _filename(filename) {}

    void execute() override {
        auto data = _document.data();
        data.filename = _filename;
        io::GptdSerializer serializer;
        io::DocumentStorage::write(data, serializer);

        // 必須等 write 成功後才更新。
        _document.setFilename(_filename);
        _document.markSaved();
    }

   private:
    Path _filename;
};

class ExportCommand : public FileCommand {
   public:
    ExportCommand(Document& document, const Path& filename) : FileCommand(document), _filename(filename) {}

    void execute() override {
        // 實現導出圖像的邏輯
        auto data = _document.data();
        data.filename = _filename;
        io::PpmExporter::write(data);
    }

   private:
    Path _filename;
};

}  // namespace paint
