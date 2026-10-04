#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "command/command.hpp"

namespace paint {

class CommandHistory {
   public:
    using StateId = std::uint64_t;

    void execute(std::unique_ptr<IUndoableCommand> command);

    void undo();
    void redo();
    void reset();

    bool canUndo() const { return !_undoStack.empty(); }
    bool canRedo() const { return !_redoStack.empty(); }

    void markSaved() { _savedState = _currentState; }

    bool isModified() const { return _currentState != _savedState; }

   private:
    struct HistoryEntry {
        std::unique_ptr<IUndoableCommand> command;
        StateId stateBefore;
        StateId stateAfter;
    };

    StateId _currentState = 0;
    StateId _savedState = 0;
    StateId _nextState = 1;  // 下一個未使用的狀態 ID

    std::vector<HistoryEntry> _undoStack;
    std::vector<HistoryEntry> _redoStack;
};

}  // namespace paint
