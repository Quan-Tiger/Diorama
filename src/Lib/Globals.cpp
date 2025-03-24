#include "Lib/Globals.h"

namespace globals
{
	namespace d3d
	{
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		IDXGISwapChain* swapChain = nullptr;
	}

	namespace game
	{
		RE::BSGraphics::Renderer* renderer = nullptr;
	}

	Menu* menu = nullptr;
	InputManager* inputManager = nullptr;
	bool menuOpen = false;

	void OnInit()
	{
		menu = Menu::GetSingleton();
		inputManager = InputManager::GetSingleton();
		menuOpen = false;
	}

	void ReInit()
	{
		{
			using namespace game;
			renderer = RE::BSGraphics::Renderer::GetSingleton();
		}

		d3d::device = reinterpret_cast<ID3D11Device*>(game::renderer->GetSingleton()->data.forwarder);
		d3d::context = reinterpret_cast<ID3D11DeviceContext*>(game::renderer->GetSingleton()->data.context);
		d3d::swapChain = reinterpret_cast<IDXGISwapChain*>(game::renderer->GetSingleton()->data.renderWindows->swapChain);
	}
}