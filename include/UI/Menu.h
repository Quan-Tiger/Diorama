#pragma once
#include "Lib/Settings.h"
#include "UI/Modals.h"
#include <dxgi1_4.h>
#include "Diorama.h"

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
	bool prevFreezeState;
	static inline std::string newProfile;
	static inline Diorama::tesRef selectedHistory;

	void Init();
	void DrawOverlay();
	void Open();
	void Close();
	void Toggle();

private:
	Menu() = default;

	Settings::Config config;
};