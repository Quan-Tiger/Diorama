#include "UI/Modals.h";

namespace UI {
	auto Modals::TextInputDialog(
		const char* a_name,
		const char* a_text,
		ImGuiInputTextFlags         a_flags,
		const cm_func_t& a_func,
		const std::optional<float>& a_twsz,
		bool                        a_allowEmpty,
		std::string& a_out)
		-> bool
	{
		return TextInputDialog(
			a_name,
			"%s",
			a_flags,
			a_func,
			a_twsz,
			a_allowEmpty,
			a_out,
			a_text);
	}

	auto Modals::ConfirmDialog(
		const char* name,
		const cm_func_t& a_func,
		const std::optional<float>& a_twsz,
		const char* text)
		-> bool
	{
		return ConfirmDialog(name, a_func, a_twsz, "%s", text);
	}
}