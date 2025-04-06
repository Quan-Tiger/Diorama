#pragma once

#include "Lib/Settings.h"
#include "Lib/Raycast.h"
#include "Lib/Misc.h"
#include "Diorama.h";

class InputManager {
    public:
		~InputManager();

        static inline InputManager* GetSingleton()
        {
            static InputManager singleton;
            return std::addressof(singleton);
        }

        void Init();
        void UpdateSettings();
        void AddEventToQueue(RE::InputEvent** a_event);
        void ProcessInputEvents(RE::InputEvent* const* a_events);
		void ProcessInputEventQueue();

    private:
		InputManager() = default;

		// members
        uint32_t       showMenuKey = 0;
        uint32_t       showMenuModifier = 0;

		class CharEvent : public RE::InputEvent
		{
		public:
			uint32_t keyCode;  // 18 (ascii code)
		};

		struct KeyEvent
		{
			explicit KeyEvent(const RE::ButtonEvent* a_event) :
				keyCode(a_event->GetIDCode()),
				device(a_event->GetDevice()),
				eventType(a_event->GetEventType()),
				value(a_event->Value()),
				heldDownSecs(a_event->HeldDuration()) {
			}

			explicit KeyEvent(const CharEvent* a_event) :
				keyCode(a_event->keyCode),
				device(a_event->GetDevice()),
				eventType(a_event->GetEventType()) {
			}

			[[nodiscard]] constexpr bool IsPressed() const noexcept { return value > 0.0F; }
			[[nodiscard]] constexpr bool IsRepeating() const noexcept { return heldDownSecs > 0.0F; }
			[[nodiscard]] constexpr bool IsDown() const noexcept { return IsPressed() && (heldDownSecs == 0.0F); }
			[[nodiscard]] constexpr bool IsHeld() const noexcept { return IsPressed() && IsRepeating(); }
			[[nodiscard]] constexpr bool IsUp() const noexcept { return (value == 0.0F) && IsRepeating(); }

			uint32_t keyCode;
			RE::INPUT_DEVICE device;
			RE::INPUT_EVENT_TYPE eventType;
			float value = 0;
			float heldDownSecs = 0;
		};

		mutable std::shared_mutex _inputEventMutex;
		std::vector<KeyEvent> _keyEventQueue{};
        
        mutable SharedLock _inputLock;
        std::vector<RE::InputEvent*> inputQueue;
        void addToEventQueue(KeyEvent e);
		void ProcessQueue();
		
};
