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

		// Convert Skyrim input scan codes to ImGui native keycodes.
		// Taken from Modex
		static inline ImGuiKey ScanCodeToImGuiKey(const uint32_t a_key)
		{
			switch (a_key) {
			case 0x0F:
				return ImGuiKey_Tab;
			case 0xCB:
				return ImGuiKey_LeftArrow;
			case 0xCD:
				return ImGuiKey_RightArrow;
			case 0xC8:
				return ImGuiKey_UpArrow;
			case 0xD0:
				return ImGuiKey_DownArrow;
			case 0xC9:
				return ImGuiKey_PageUp;
			case 0xD1:
				return ImGuiKey_PageDown;
			case 0xC7:
				return ImGuiKey_Home;
			case 0xCF:
				return ImGuiKey_End;
			case 0xD2:
				return ImGuiKey_Insert;
			case 0xD3:
				return ImGuiKey_Delete;
			case 0x0E:
				return ImGuiKey_Backspace;
			case 0x39:
				return ImGuiKey_Space;
			case 0x1C:
				return ImGuiKey_Enter;
			case 0x01:
				return ImGuiKey_Escape;
			case 0x28:
				return ImGuiKey_Apostrophe;
			case 0x33:
				return ImGuiKey_Comma;
			case 0x0C:
				return ImGuiKey_Minus;
			case 0x34:
				return ImGuiKey_Period;
			case 0x35:
				return ImGuiKey_Slash;
			case 0x27:
				return ImGuiKey_Semicolon;
			case 0x0D:
				return ImGuiKey_Equal;
			case 0x1A:
				return ImGuiKey_LeftBracket;
			case 0x2B:
				return ImGuiKey_Backslash;
			case 0x1B:
				return ImGuiKey_RightBracket;
			case 0x29:
				return ImGuiKey_GraveAccent;
			case 0x3A:
				return ImGuiKey_CapsLock;
			case 0x46:
				return ImGuiKey_ScrollLock;
			case 0x45:
				return ImGuiKey_NumLock;
			case 0xB7:
				return ImGuiKey_PrintScreen;
			case 0xC5:
				return ImGuiKey_Pause;
			case 0x52:
				return ImGuiKey_Keypad0;
			case 0x4F:
				return ImGuiKey_Keypad1;
			case 0x50:
				return ImGuiKey_Keypad2;
			case 0x51:
				return ImGuiKey_Keypad3;
			case 0x4B:
				return ImGuiKey_Keypad4;
			case 0x4C:
				return ImGuiKey_Keypad5;
			case 0x4D:
				return ImGuiKey_Keypad6;
			case 0x47:
				return ImGuiKey_Keypad7;
			case 0x48:
				return ImGuiKey_Keypad8;
			case 0x49:
				return ImGuiKey_Keypad9;
			case 0x53:
				return ImGuiKey_KeypadDecimal;
			case 0xB5:
				return ImGuiKey_KeypadDivide;
			case 0x37:
				return ImGuiKey_KeypadMultiply;
			case 0x4A:
				return ImGuiKey_KeypadSubtract;
			case 0x4E:
				return ImGuiKey_KeypadAdd;
			case 0x9C:
				return ImGuiKey_KeypadEnter;
			case 0x2A:
				return ImGuiKey_LeftShift;
			case 0x1D:
				return ImGuiKey_LeftCtrl;
			case 0x38:
				return ImGuiKey_LeftAlt;
			case 0x5B:
				return ImGuiKey_LeftSuper;
			case 0x36:
				return ImGuiKey_RightShift;
			case 0x9D:
				return ImGuiKey_RightCtrl;
			case 0xB8:
				return ImGuiKey_RightAlt;
			case 0x5C:
				return ImGuiKey_RightSuper;
			case 0x5D:
				return ImGuiKey_Menu;
			case 0x0B:
				return ImGuiKey_0;
			case 0x02:
				return ImGuiKey_1;
			case 0x03:
				return ImGuiKey_2;
			case 0x04:
				return ImGuiKey_3;
			case 0x05:
				return ImGuiKey_4;
			case 0x06:
				return ImGuiKey_5;
			case 0x07:
				return ImGuiKey_6;
			case 0x08:
				return ImGuiKey_7;
			case 0x09:
				return ImGuiKey_8;
			case 0x0A:
				return ImGuiKey_9;
			case 0x1E:
				return ImGuiKey_A;
			case 0x30:
				return ImGuiKey_B;
			case 0x2E:
				return ImGuiKey_C;
			case 0x20:
				return ImGuiKey_D;
			case 0x12:
				return ImGuiKey_E;
			case 0x21:
				return ImGuiKey_F;
			case 0x22:
				return ImGuiKey_G;
			case 0x23:
				return ImGuiKey_H;
			case 0x17:
				return ImGuiKey_I;
			case 0x24:
				return ImGuiKey_J;
			case 0x25:
				return ImGuiKey_K;
			case 0x26:
				return ImGuiKey_L;
			case 0x32:
				return ImGuiKey_M;
			case 0x31:
				return ImGuiKey_N;
			case 0x18:
				return ImGuiKey_O;
			case 0x19:
				return ImGuiKey_P;
			case 0x10:
				return ImGuiKey_Q;
			case 0x13:
				return ImGuiKey_R;
			case 0x1F:
				return ImGuiKey_S;
			case 0x14:
				return ImGuiKey_T;
			case 0x16:
				return ImGuiKey_U;
			case 0x2F:
				return ImGuiKey_V;
			case 0x11:
				return ImGuiKey_W;
			case 0x2D:
				return ImGuiKey_X;
			case 0x15:
				return ImGuiKey_Y;
			case 0x2C:
				return ImGuiKey_Z;
			case 0x3B:
				return ImGuiKey_F1;
			case 0x3C:
				return ImGuiKey_F2;
			case 0x3D:
				return ImGuiKey_F3;
			case 0x3E:
				return ImGuiKey_F4;
			case 0x3F:
				return ImGuiKey_F5;
			case 0x40:
				return ImGuiKey_F6;
			case 0x41:
				return ImGuiKey_F7;
			case 0x42:
				return ImGuiKey_F8;
			case 0x43:
				return ImGuiKey_F9;
			case 0x44:
				return ImGuiKey_F10;
			case 0x57:
				return ImGuiKey_F11;
			case 0x58:
				return ImGuiKey_F12;
			default:
				return ImGuiKey_None;
			}
		}

		static inline bool IsKeyboardTextShortcut(ImGuiKey a_key)
		{
			return (a_key == ImGuiKey_Backspace ||
				a_key == ImGuiKey_Delete ||
				a_key == ImGuiKey_Enter ||
				a_key == ImGuiKey_Space ||
				a_key == ImGuiKey_Tab ||
				a_key == ImGuiKey_End ||
				a_key == ImGuiKey_Home ||
				a_key == ImGuiKey_PageUp ||
				a_key == ImGuiKey_PageDown ||
				a_key == ImGuiKey_LeftArrow ||
				a_key == ImGuiKey_RightArrow ||
				a_key == ImGuiKey_UpArrow ||
				a_key == ImGuiKey_DownArrow);
		}

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
