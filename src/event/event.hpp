#pragma once

namespace paint {

class EventTarget;

class Event {
   public:
    explicit Event(bool bubbles = false) : _bubbles(bubbles) {}

    virtual ~Event() = default;

    EventTarget* target() const { return _target; }
    EventTarget* currentTarget() const { return _currentTarget; }

    bool bubbles() const { return _bubbles; }
    bool propagationStopped() const { return _propagationStopped; }

    void stopPropagation() { _propagationStopped = true; }

   private:
    friend class EventTarget;  // EventTarget 負責事件分派期間的內部狀態

    EventTarget* _target = nullptr;
    EventTarget* _currentTarget = nullptr;

    bool _bubbles;
    bool _propagationStopped = false;
};

}  // namespace paint
