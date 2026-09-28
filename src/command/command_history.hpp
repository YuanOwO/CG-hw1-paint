#pragma once

#include <memory>
#include <vector>

#include "command/command.hpp"

namespace paint {

class CommandHistory {
   public:
    void execute(std::unique_ptr<IUndoableCommand> command);

    void undo();
    void redo();

    bool canUndo() const { return !_undoStack.empty(); }
    bool canRedo() const { return !_redoStack.empty(); }

   private:
    std::vector<std::unique_ptr<IUndoableCommand>> _undoStack;
    std::vector<std::unique_ptr<IUndoableCommand>> _redoStack;
};

}  // namespace paint
