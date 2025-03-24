#pragma once

#include "Lib/Menu.h"
#include "Lib/InputManager.h"

namespace globals
{
	namespace d3d
	{
		extern ID3D11Device* device;
		extern ID3D11DeviceContext* context;
		extern IDXGISwapChain* swapChain;
	}

	namespace game
	{
		extern RE::BSGraphics::Renderer* renderer;
	}

	extern Menu* menu;
	extern InputManager* inputManager;
	extern bool menuOpen;

	void OnInit();
	void ReInit();
}