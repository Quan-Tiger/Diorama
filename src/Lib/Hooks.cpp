#include "Lib/Hooks.h";
#include "Lib/InputManager.h"


struct BSInputDeviceManager_PollInputDevices
{
	static void thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_events)
	{
		bool blockedDevice = true;

		auto inputManager = globals::inputManager;

		if (a_events) {
			inputManager->ProcessInputEvents(a_events);

			if (*a_events) {
				if (auto device = (*a_events)->GetDevice()) {
					// Check that the device is not a Gamepad or VR controller. If it is, unblock input.
					bool vrDevice = false;
//#ifdef ENABLE_SKYRIM_VR
//					vrDevice = (globals::game::isVR && ((device == RE::INPUT_DEVICES::INPUT_DEVICE::kVivePrimary) ||
//						(device == RE::INPUT_DEVICES::INPUT_DEVICE::kViveSecondary) ||
//						(device == RE::INPUT_DEVICES::INPUT_DEVICE::kOculusPrimary) ||
//						(device == RE::INPUT_DEVICES::INPUT_DEVICE::kOculusSecondary) ||
//						(device == RE::INPUT_DEVICES::INPUT_DEVICE::kWMRPrimary) ||
//						(device == RE::INPUT_DEVICES::INPUT_DEVICE::kWMRSecondary)));
//#endif
					blockedDevice = !((device == RE::INPUT_DEVICES::INPUT_DEVICE::kGamepad) || vrDevice);
				}
			}
		}

		if (blockedDevice && globals::menuOpen) {  //the menu is open, eat all keypresses
			constexpr RE::InputEvent* const dummy[] = { nullptr };
			func(a_dispatcher, dummy);
			return;
		}

		func(a_dispatcher, a_events);
	}
	static inline REL::Relocation<decltype(thunk)> func;
};

namespace Hooks {
	struct BSGraphics_Renderer_Init_InitD3D
	{
		static void thunk()
		{
			logger::info("Calling original Init3D");

			func();

			logger::info("Accessing render device information");
			globals::ReInit();
			globals::menu->Init();
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static inline std::atomic<bool> init{ false };
	};

	struct WndProcHandler_Hook
	{
		static LRESULT thunk(HWND a_hwnd, UINT a_msg, WPARAM a_wParam, LPARAM a_lParam)
		{
			auto menu = globals::menu;
			if (a_msg == WM_KILLFOCUS && menu->initialized) {
				//menu->OnFocusLost();
				auto& io = ImGui::GetIO();
				io.ClearInputKeys();
				io.ClearEventsQueue();
			}
			return func(a_hwnd, a_msg, a_wParam, a_lParam);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct RegisterClassA_Hook
	{
		static ATOM thunk(WNDCLASSA* a_wndClass)
		{
			WndProcHandler_Hook::func = reinterpret_cast<uintptr_t>(a_wndClass->lpfnWndProc);
			a_wndClass->lpfnWndProc = &WndProcHandler_Hook::thunk;

			return func(a_wndClass);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct DXGIPresentHook
	{
		static void thunk(std::uint32_t a_p1)
		{
			func(a_p1);

			InputManager::GetSingleton()->ProcessInputEventQueue();  //Synchronize Inputs to frame
			Menu::GetSingleton()->DrawOverlay();
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		logger::info("Hooking BSInputDeviceManager::PollInputDevices");
		stl::write_thunk_call<BSInputDeviceManager_PollInputDevices>(REL::RelocationID(67315, 68617).address() + REL::Relocate(0x7B, 0x7B, 0x81));

		logger::info("Hooking BSGraphics::Renderer::InitD3D");
		stl::write_thunk_call<BSGraphics_Renderer_Init_InitD3D>(REL::RelocationID(75595, 77226).address() + REL::Relocate(0x50, 0x2BC));
		BSGraphics_Renderer_Init_InitD3D::init.store(true);

		logger::info("Hooking WndProcHandler");
		stl::write_thunk_call<RegisterClassA_Hook, 6>(REL::VariantID(75591, 77226, 0xDC4B90).address() + REL::VariantOffset(0x8E, 0x15C, 0x99).offset());

		logger::info("Hooking DXGI Present");
		stl::write_thunk_call<DXGIPresentHook>(REL::RelocationID(75461, 77246).address() + REL::VariantOffset(0x9, 0x9, 0x15).offset());
	}
}