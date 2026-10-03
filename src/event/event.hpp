#pragma once

namespace paint::ui {

class EventTarget;

}  // namespace paint::ui

namespace paint {

class Event {
   public:
    explicit Event(bool bubbles = false) : _bubbles(bubbles) {}

    virtual ~Event() = default;

    ui::EventTarget* target() const { return _target; }
    ui::EventTarget* currentTarget() const { return _currentTarget; }

    bool bubbles() const { return _bubbles; }
    bool propagationStopped() const { return _propagationStopped; }

    void stopPropagation() { _propagationStopped = true; }

   private:
    friend class ui::EventTarget;  // EventTarget 負責事件分派期間的內部狀態

    ui::EventTarget* _target = nullptr;
    ui::EventTarget* _currentTarget = nullptr;

    bool _bubbles;
    bool _propagationStopped = false;
};

}  // namespace paint
