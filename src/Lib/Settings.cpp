#include "Lib/Settings.h"
#include "SimpleIni.h"

void Settings::GetIni(const std::filesystem::path& a_path, const std::function<void(const std::filesystem::path&, CSimpleIniA&)> a_func)
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

	a_func(a_path, ini);

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

	if constexpr (std::is_same_v<T, std::string>) {
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
	else {
		logger::error("Unhandled type passed to GetIniValue in Menu.cpp.");
		return a_default;
	}
}

void Settings::LoadSettings(const std::filesystem::path& a_path) {
	GetIni(a_path, [](const std::filesystem::path& a_path, CSimpleIniA& a_ini) {
		Settings::GetSingleton()->LoadMasterIni(a_ini);
	});
}

void Settings::LoadMasterIni(CSimpleIniA& a_ini)
{
	auto& _default = def;

	config.showMenuKey = GetIniValue<uint32_t>(a_ini, "Input", "ShowMenuKey", _default.showMenuKey);
	config.selectedProfile = GetIniValue<std::string>(a_ini, "Profiles", "Current", _default.selectedProfile);
}

void Settings::SaveSettings(const std::filesystem::path& a_path) {
	GetIni(a_path, [](const std::filesystem::path& a_path, CSimpleIniA& a_ini) {
		Settings::GetSingleton()->SaveMasterIni(a_ini);
		const std::wstring wide_path = a_path.wstring();
		a_ini.SaveFile(wide_path.c_str());
	});

}

void Settings::SaveMasterIni(CSimpleIniA& a_ini) {
	a_ini.SetValue("Input", "ShowMenuKey", std::to_string(config.showMenuKey).c_str());
	if (!config.selectedProfile.empty()) {
		a_ini.SetValue("Profiles", "Current", config.selectedProfile.c_str());
	}
}
