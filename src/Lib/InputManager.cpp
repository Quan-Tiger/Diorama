#include "Lib/InputManager.h"

InputManager::~InputManager()
{
	//ImGui_ImplDX11_Shutdown();
	//ImGui_ImplWin32_Shutdown();
	//ImGui::DestroyContext();
	//dxgiAdapter3 = nullptr;
}

void InputManager::UpdateSettings()
{
    auto& config = Settings::GetSingleton()->config;
    showMenuKey = config.showMenuKey;
    showMenuModifier = config.showMenuModifier;
}

void InputManager::Init()
{
    UpdateSettings();
}

void InputManager::AddEventToQueue(RE::InputEvent** a_event)
{
    if (a_event) {
        for (auto inputEvent = *a_event; inputEvent; inputEvent = inputEvent->next) {
            WriteLocker locker(_inputLock);
            inputQueue.emplace_back(inputEvent);
        }
    }
}

void InputManager::ProcessInputEvents(RE::InputEvent* const* a_events)
{
	for (auto it = *a_events; it; it = it->next) {
        if (it->GetEventType() != RE::INPUT_EVENT_TYPE::kButton && it->GetEventType() != RE::INPUT_EVENT_TYPE::kChar)  // we do not care about non button or char events
            continue;

        auto event = it->GetEventType() == RE::INPUT_EVENT_TYPE::kButton ? KeyEvent(static_cast<RE::ButtonEvent*>(it)) : KeyEvent(static_cast<CharEvent*>(it));

        addToEventQueue(event);
    }
}

void InputManager::ProcessInputEventQueue() {
	ProcessQueue();
}

void InputManager::ProcessQueue()
{
	std::unique_lock<std::shared_mutex> mutex(_inputEventMutex);

	if (_keyEventQueue.empty()) {
		return;
	}

	if (ImGui::GetCurrentContext() == nullptr) {
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	for (auto& event : _keyEventQueue) {
		if (globals::menuOpen && event.eventType == RE::INPUT_EVENT_TYPE::kChar) {
			io.AddInputCharacter(event.keyCode);
			continue;
		}
		else if (event.device == RE::INPUT_DEVICE::kMouse) {
			if (globals::menuOpen) {
				//logger::trace("Detect mouse scan code {} value {} pressed: {}", event.keyCode, event.value, event.IsPressed());
				if (event.keyCode > 7) {  // middle scroll
					io.AddMouseWheelEvent(0, event.value * (event.keyCode == 8 ? 1 : -1));
				}
				else {
					if (event.keyCode > 5)
						event.keyCode = 5;
					io.AddMouseButtonEvent(event.keyCode, event.IsPressed());
				}
			}
		}
		else if (event.device == RE::INPUT_DEVICE::kKeyboard) {
			if (event.keyCode == showMenuKey && event.IsDown()) {
				Menu::GetSingleton()->Toggle();
				break;
			}
			if (globals::menuOpen) {
				const ImGuiKey imGuiKey = InputManager::ScanCodeToImGuiKey(event.keyCode);

				if (event.keyCode == 0x01 && event.IsDown()) {  // esc
					Menu::GetSingleton()->PrepClose();
					break;
				}

				// IMPORTANT: We break out of the above code on escape to prevent unpaired press/release events.
				if (InputManager::IsKeyboardTextShortcut(imGuiKey)) {
					if (globals::menuOpen && io.WantTextInput) {
						io.ClearInputKeys();
						io.AddKeyEvent(imGuiKey, event.IsDown());
					}
				}
			}
		}
	}

	_keyEventQueue.clear();
}

void InputManager::addToEventQueue(KeyEvent e)
{
    std::unique_lock<std::shared_mutex> mutex(_inputEventMutex);
    _keyEventQueue.emplace_back(e);
}
