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

	static inline bool checkedOnOpen = false;
	static inline bool checkedOnClose = true;
	static inline bool requestedClose = false;
	static inline bool openRestoreAllPopup = false;
	static inline bool isRestoreCancelled = false;
	static inline bool openSavePopup = false;
	static inline bool isSaveCancelled = false;
	bool initialized = false;
	static inline bool cocInProgress = false;
	static inline bool tintAll = false;
	static inline bool checkTinting = false;
	bool prevFreezeState;
	static inline int item_selected_idx;
	static inline std::string newProfile;
	static inline std::string newBaseID;
	static inline std::string addFromID;

	void Init();
	void DrawOverlay();
	void RestoreAll();
	void Open();
	void Close();
	void PrepClose();
	void Toggle();

private:
	Menu() = default;

	static inline Settings::Config config;

	void ReferenceControlSection();
	void ProfilesSection();
};