#pragma once
#include "Lib/Settings.h"
#include <dxgi1_4.h>

class Menu
{
public:
	~Menu();

	static Menu* GetSingleton()
	{
		static Menu menu;
		return &menu;
	}

	bool initialized = false;

	void Init();
	void DrawOverlay();

private:
	Menu() = default;

	Settings::Config config;
};