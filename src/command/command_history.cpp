#include "command/command_history.hpp"

#include <utility>

namespace paint {

void CommandHistory::execute(std::unique_ptr<IUndoableCommand> command) {
    command->execute();

    _undoStack.push_back(std::move(command));
    _redoStack.clear();
}

void CommandHistory::undo() {
    if (_undoStack.empty()) {  // 沒有東西可以 undo 了
        return;
    }

    auto command = std::move(_undoStack.back());
    _undoStack.pop_back();

    command->undo();

    _redoStack.push_back(std::move(command));
}

void CommandHistory::redo() {
    if (_redoStack.empty()) {  // 沒有東西可以 redo 了
        return;
    }

    auto command = std::move(_redoStack.back());
    _redoStack.pop_back();

    command->execute();

    _undoStack.push_back(std::move(command));
}

}  // namespace paint
