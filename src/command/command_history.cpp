#include "command/command_history.hpp"

#include <utility>

namespace paint {

void CommandHistory::execute(std::unique_ptr<IUndoableCommand> command) {
    command->execute();

    const StateId newState = _nextState++;

    _undoStack.push_back({
        std::move(command),
        _currentState,
        newState,
    });

    _currentState = newState;  // 更新當前狀態為新狀態

    _redoStack.clear();
}

void CommandHistory::undo() {
    if (_undoStack.empty()) {  // 沒有東西可以 undo 了
        return;
    }

    auto history = std::move(_undoStack.back());
    _undoStack.pop_back();

    history.command->undo();
    _currentState = history.stateBefore;  // 更新當前狀態為 undo 後的狀態

    _redoStack.push_back(std::move(history));
}

void CommandHistory::redo() {
    if (_redoStack.empty()) {  // 沒有東西可以 redo 了
        return;
    }

    auto history = std::move(_redoStack.back());
    _redoStack.pop_back();

    history.command->execute();
    _currentState = history.stateAfter;  // 更新當前狀態為 redo 後的狀態

    _undoStack.push_back(std::move(history));
}

void CommandHistory::reset() {
    markSaved();
    _undoStack.clear();
    _redoStack.clear();
}

}  // namespace paint
