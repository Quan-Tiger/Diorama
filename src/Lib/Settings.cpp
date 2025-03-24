#include "Lib/Settings.h"
#include "SimpleIni.h"

void Settings::GetIni(const std::filesystem::path& a_path, const std::function<void(CSimpleIniA&)> a_func)
{
	CSimpleIniA ini;

	if (a_path.empty()) {
		logger::critical("[Settings] Invalid Path provided to GetIni");
	}

	// This is okay since we're interfacing with an external API
	const std::wstring wide_path = a_path.wstring();

	ini.SetUnicode();

	if (const auto rc = ini.LoadFile(wide_path.c_str())) {
		if (rc < SI_OK) {
			(void)ini.SaveFile(wide_path.c_str());  // Could not locate, let's procreate.
		}
	}

	a_func(ini);

	(void)ini.SaveFile(wide_path.c_str());
}

template <class T>
T GetIniValue(CSimpleIniA& a_ini, const char* section, const char* key, T a_default)
{
	std::string value;

	if (a_ini.GetValue(section, key) == nullptr) {
		a_ini.SetValue(section, key, Settings::ToString(a_default, false).c_str());
		return a_default;
	}
	else {
		value = a_ini.GetValue(section, key);
	}

	if (value.empty()) {
		logger::warn("[Settings] Failed to parse value from .ini file! Ensure you're using the correct format!");
		return a_default;
	}

	// A+ plus de-serialization.
	/*if constexpr (std::is_same_v<T, ImVec4>) {
		auto color = Settings::GetColor<T>(value);
		return (color.second ? color.first : a_default);
	}
	else if constexpr (std::is_same_v<T, ImVec2>) {
		auto vec = Settings::GetVec2(value);
		return (vec.second ? vec.first : a_default);
	}
	else*/ if constexpr (std::is_same_v<T, std::string>) {
		return value;
	}
	else if constexpr (std::is_same_v<T, bool>) {
		return value == "true";
	}
	else if constexpr (std::is_same_v<T, int>) {
		return std::stoi(value);
	}
	else if constexpr (std::is_same_v<T, uint32_t>) {
		return std::stoul(value);
	}
	else if constexpr (std::is_same_v<T, float>) {
		return std::stof(value);
	}
	//else if constexpr (std::is_same_v<T, GraphicManager::Image>) {
	//	return GraphicManager::GetImage(value);
	//}
	//else if constexpr (std::is_same_v<T, Language::GlyphRanges>) {
	//	return Language::GetGlyphRange(value);
	//}
	else {
		logger::error("[Settings] Unhandled type passed to GET_VALUE in Menu.cpp!");
		return a_default;
	}
}

void Settings::LoadSettings(const std::filesystem::path& a_path) {
	GetIni(a_path, [](CSimpleIniA& a_ini) {
		Settings::GetSingleton()->LoadMasterIni(a_ini);
	});
}

void Settings::LoadMasterIni(CSimpleIniA& a_ini)
{
	auto& _default = def;

	//config.showMenuKey = a_ini.GetValue(;// 209;
	config.showMenuKey = GetIniValue<uint32_t>(a_ini, "Input", "ShowMenuKey", _default.showMenuKey);
	logger::info("Found value in ini {}", config.showMenuKey);
	/*auto& _default = def.config;

	config.showMenuKey = GET_VALUE<uint32_t>(rSections[Main], "ShowMenuKey", _default.showMenuKey, a_ini);
	config.showMenuModifier = GET_VALUE<uint32_t>(rSections[Main], "ShowMenuModifier", _default.showMenuModifier, a_ini);
	config.pauseGame = GET_VALUE<bool>(rSections[Main], "PauseGame", _default.pauseGame, a_ini);
	config.disableInMenu = GET_VALUE<bool>(rSections[Main], "DisableInMenu", _default.disableInMenu, a_ini);*/
}