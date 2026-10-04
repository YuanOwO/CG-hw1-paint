#pragma once

#include <string>

#include "app/document.hpp"
#include "command/command.hpp"
#include "io/document_storage.hpp"
#include "io/image_exporter.hpp"

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

class LoadCommand : public FileCommand {
   public:
    LoadCommand(Document& document, const Path& filename) : FileCommand(document), _filename(filename) {}

    void execute() override {
        auto data = io::DocumentStorage::read(_filename);

        // read 完全成功後才替換原文件。
        _document.replaceContent(std::move(data.scene), _filename);
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
        io::DocumentStorage::write(data);

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
