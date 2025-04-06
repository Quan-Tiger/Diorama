#pragma once
#include "SimpleIni.h"

class Settings
{
	public:
		void GetIni(const std::filesystem::path& a_path, const std::function<void(const std::filesystem::path&, CSimpleIniA&)> a_func);
		void LoadSettings(const std::filesystem::path& a_path);
		void LoadMasterIni(CSimpleIniA& a_ini);
		void SaveMasterIni(CSimpleIniA& a_ini);
		void SaveSettings(const std::filesystem::path& a_path);

		static inline Settings* GetSingleton()
		{
			static Settings singleton;
			return std::addressof(singleton);
		}

		static inline const std::string main_path = "Data/Diorama";
		static inline const std::string ini_path = main_path + "/Diorama.ini";
		static inline const std::string json_path = main_path + "/json";

		struct Config
		{
			uint32_t showMenuKey = 209;
			uint32_t showMenuModifier = 0;
			std::string selectedProfile = "";
			bool pauseGame = false;
			bool disableInMenu = false;
		};

		Config config;
		Config def;

		// https://github.com/powerof3/PhotoMode | License: MIT
		template <class T>
		static std::string ToString(const T& a_style, bool a_hex = false)
		{
			if constexpr (std::is_same_v<ImVec4, T>) {
				if (a_hex) {
					return std::format("#{:02X}{:02X}{:02X}{:02X}", std::uint8_t(255.0f * a_style.x), std::uint8_t(255.0f * a_style.y), std::uint8_t(255.0f * a_style.z), std::uint8_t(255.0f * a_style.w));
				}
				return std::format("{}{},{},{},{}{}", "{", std::uint8_t(255.0f * a_style.x), std::uint8_t(255.0f * a_style.y), std::uint8_t(255.0f * a_style.z), std::uint8_t(255.0f * a_style.w), "}");
			}
			else if constexpr (std::is_same_v<ImVec2, T>) {
				return std::format("{}{},{}{}", "{", a_style.x, a_style.y, "}");
			}
			else if constexpr (std::is_same_v<std::string, T>) {
				return a_style;
			}
			/*else if constexpr (std::is_same_v<FontManager::FontData, T>) {
				return a_style.name;
			}
			else if constexpr (std::is_same_v<GraphicManager::Image, T>) {
				return GraphicManager::GetImageName(a_style);
			}
			else if constexpr (std::is_same_v<Language::GlyphRanges, T>) {
				return Language::GetGlyphName(a_style);
			}*/
			else if constexpr (std::is_same_v<float, T>) {
				return std::format("{:.3f}", a_style);
			}
			else if constexpr (std::is_same_v<bool, T>) {
				return a_style ? "true" : "false";
			}
			else if constexpr (std::is_same_v<int, T>) {
				return std::to_string(a_style);
			}
			else if constexpr (std::is_same_v<uint32_t, T>) {
				return std::to_string(a_style);
			}
			else {
				stl::report_and_fail("Unsupported type for ToString");  // FIXME: static_assert?
			}
		}
};