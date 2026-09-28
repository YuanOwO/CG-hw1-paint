#pragma once

namespace paint {

class ICommand {
   public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
};

class IUndoableCommand : public ICommand {
   public:
    virtual void undo() = 0;
};

}  // namespace paint
