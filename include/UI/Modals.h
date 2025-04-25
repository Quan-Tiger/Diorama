#pragma once

namespace UI {

	class Modals
	{
	public:

		using cm_func_t = std::function<bool()>;
		Modals() = default;

		static bool TextInputDialog(
			const char* a_name,
			const char* a_text,
			ImGuiInputTextFlags         a_flags,
			const cm_func_t& a_func,
			const std::optional<float>& a_twsz,
			bool                        a_allowEmpty,
			std::string& a_out);

		template <class... Args>
		static bool TextInputDialog(
			const char* a_name,
			const char* a_text,
			ImGuiInputTextFlags         a_flags,
			const cm_func_t& a_func,
			const std::optional<float>& a_twsz,
			bool                        a_allowEmpty,
			std::string& a_out,
			Args... args);

		static bool ConfirmDialog(
			const char* name,
			const cm_func_t& a_func,
			const std::optional<float>& a_twsz,
			bool& isCancelled,
			const char* text);

		template <class... Args>
		static bool ConfirmDialog(
			const char* name,
			const cm_func_t& a_func,
			const std::optional<float>& a_twsz,
			bool& isCancelled,
			const char* text,
			Args... args);

	private:
		template <class... Args>
		static bool TextInputDialogImpl(
			bool                        a_multiLine,
			const char* a_name,
			const char* a_text,
			ImGuiInputTextFlags         a_flags,
			const cm_func_t& a_func,
			const std::optional<float>& a_twsz,
			bool                        a_allowEmpty,
			std::string& a_out,
			Args... args);
	};

	template <class... Args>
	auto Modals::TextInputDialog(
		const char* a_name,
		const char* a_text,
		ImGuiInputTextFlags         a_flags,
		const cm_func_t& a_func,
		const std::optional<float>& a_twsz,
		bool                        a_allowEmpty,
		std::string& a_out,
		Args... a_args)
		-> bool
	{
		return TextInputDialogImpl(
			false,
			a_name,
			a_text,
			a_flags,
			a_func,
			a_twsz,
			a_allowEmpty,
			a_out,
			a_args...);
	}
	
	template <class... Args>
		auto Modals::TextInputDialogImpl(
			bool                        a_multiLine,
			const char* a_name,
			const char* a_text,
			ImGuiInputTextFlags         a_flags,
			const cm_func_t& a_func,
			const std::optional<float>& a_twsz,
			bool                        a_allowEmpty,
			std::string& a_out,
			Args... a_args)
		-> bool
	{
		bool ret = false;

		auto& io = ImGui::GetIO();

		ImGui::SetNextWindowPos(
			{ io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f },
			ImGuiCond_Appearing,
			{ 0.5f, 0.5f });

		const auto modalFlags =
			a_multiLine ?
			ImGuiWindowFlags_None :
			ImGuiWindowFlags_AlwaysAutoResize;

		if (ImGui::BeginPopupModal(
			a_name,
			nullptr,
			modalFlags))
		{
			if (a_text && a_text[0] != 0)
			{
				if (modalFlags & ImGuiWindowFlags_AlwaysAutoResize)
				{
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * (a_twsz ? *a_twsz : 25.0f));
				}

				ImGui::Text(a_text, a_args...);

				if (modalFlags & ImGuiWindowFlags_AlwaysAutoResize)
				{
					ImGui::PopTextWrapPos();
				}

				ImGui::Spacing();
			}

			if (ImGui::IsWindowAppearing())
			{
				ImGui::SetKeyboardFocusHere();
			}

			bool inputResult = ImGui::InputText(
					"##slt_input",
					std::addressof(a_out),
					a_flags);
			
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::Selectable("Paste")) a_out = ImGui::GetClipboardText();
				ImGui::EndPopup();
			}

			if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V))
				a_out = ImGui::GetClipboardText();

			if (inputResult)
			{
				ImGui::CloseCurrentPopup();
				ret = true;
			}

			ImGui::Spacing();
			ImGui::Separator();

			bool disabled = false;

			if (a_func)
			{
				ImGui::PushID("extra");
				disabled = !a_func();
				ImGui::PopID();

				ImGui::Separator();
			}

			if (!a_allowEmpty && a_out.empty())
			{
				disabled = true;
			}

			if (disabled)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(
					ImGuiStyleVar_Alpha,
					ImGui::GetStyle().Alpha * 0.5f);
			}

			if (ImGui::Button(
				"OK",
				{ 120.f, 0.f }))
			{
				ImGui::CloseCurrentPopup();
				ret = true;
			}

			if (disabled)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			ImGui::SameLine();

			ImGui::SetItemDefaultFocus();
			if (ImGui::Button(
				"Cancel",
				{ 120.f, 0.f }))
			{
				ImGui::CloseCurrentPopup();
				ret = false;
			}

			ImGui::EndPopup();
		}

		return ret;
	}

	template <class... Args>
	auto Modals::ConfirmDialog(
		const char* name,
		const cm_func_t& a_func,
		const std::optional<float>& a_twsz,
		bool& isCancelled,
		const char* text,
		Args... args)
		-> bool
	{
		bool ret = false;

		auto& io = ImGui::GetIO();

		ImGui::SetNextWindowPos(
			{ io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f },
			ImGuiCond_Appearing,
			{ 0.5f, 0.5f });

		if (ImGui::BeginPopupModal(
			name,
			nullptr,
			ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * (a_twsz ? *a_twsz : 25.0f));
			ImGui::Text(text, args...);
			ImGui::PopTextWrapPos();
			ImGui::Spacing();
			ImGui::Separator();

			bool disabled = false;

			if (a_func)
			{
				ImGui::PushID("extra");
				disabled = !a_func();
				ImGui::PopID();

				ImGui::Separator();
			}

			if (disabled)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(
					ImGuiStyleVar_Alpha,
					ImGui::GetStyle().Alpha * 0.5f);
			}

			bool r = ImGui::Button(
				"OK",
				{ 120.0f, 0.0f });

			if (disabled)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			if (r)
			{
				ImGui::CloseCurrentPopup();
				ret = true;
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button(
				"Cancel",
				{ 120.0f, 0.0f }))
			{
				ImGui::CloseCurrentPopup();
				ret = false;
				isCancelled = true;
			}

			ImGui::EndPopup();
		}

		return ret;
	}
}