#include "UI/Menu.h"
#include "Diorama.h"
#include "UI/Modals.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

Menu::~Menu()
{
	//ImGui_ImplDX11_Shutdown();
	//ImGui_ImplWin32_Shutdown();
	//ImGui::DestroyContext();
	//dxgiAdapter3 = nullptr;
}

void Menu::Init()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& imgui_io = ImGui::GetIO();

	imgui_io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard;
	imgui_io.BackendFlags = ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_RendererHasVtxOffset;

	ImFontConfig font_config;
	font_config.GlyphExtraSpacing.x = -0.5;

	DXGI_SWAP_CHAIN_DESC desc;
	globals::d3d::swapChain->GetDesc(&desc);

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(desc.OutputWindow);
	ImGui_ImplDX11_Init(globals::d3d::device, globals::d3d::context);

	config = Settings::GetSingleton()->config;
}

void Menu::DrawOverlay()
{
	if (Diorama::TryMoveToCell()) {
		return;
	}

	if (checkTinting) {
		for (auto& ref : Diorama::references) {
			Diorama::TintReference(ref);
		}
		checkTinting = false;
	}

	if (!globals::menuOpen) {
		return;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	auto width = (ImGui::GetMainViewport()->Size.x - 20) * 0.25f;
	auto height = ImGui::GetMainViewport()->Size.y - 20;
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::SetNextWindowSize(ImVec2(width, height));

	ImGui::Begin("Diorama", nullptr, ImGuiWindowFlags_MenuBar);

	CheckRestoreAll();

	if (CheckSaveProfile()) 
		return;

	ImGui::SeparatorText("Profiles");

	ProfilesSection();
	AllReferencesControls();

	ImGui::SeparatorText("References");

	if (Diorama::currentProfile != nullptr) {

		ReferenceControlSection();

		if (Diorama::references.size() > 0) {

			{
				ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.3f, 260), ImGuiChildFlags_None);
				ImGui::BeginListBox("##RefList");

				int i = 0;
				for (Diorama::tesRef& ref : Diorama::references) {
					ref.IsSelected = (item_selected_idx == i);

					std::string value;
					if (!ref.Saved) {
						value += "*";
					}
					value += ref.FormID;

					if (ImGui::Selectable(value.c_str(), ref.IsSelected))
					{
						item_selected_idx = i;
						selected = &ref;
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (ref.IsSelected)
						ImGui::SetItemDefaultFocus();

					i++;
				}

				ImGui::EndListBox();
				ImGui::EndChild();
			}

			ImGui::SameLine();

			{
				ImGui::BeginChild("ChildR", ImVec2(0, 500), ImGuiChildFlags_None);
				/*for (Diorama::tesRef& ref : Diorama::references) {
					if (ref.IsSelected) {*/
				if (selected) {
					std::string disabled = selected->IsDisabled ? "true" : "false";
					if (ImGui::BeginTable("details", 2))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Form ID:");
						ImGui::TableSetColumnIndex(1);
						ImGui::Text("Name:");
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(selected->FormID.c_str());
						ImGui::TableSetColumnIndex(1);
						ImGui::Text(selected->Name.c_str());
						ImGui::TableNextRow();

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Position X:");
						ImGui::TableSetColumnIndex(1);
						ImGui::Text("Rotation X:");
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(std::to_string(selected->PositionX).c_str());
						ImGui::TableSetColumnIndex(1);
						ImGui::Text(std::to_string(selected->AngleX).c_str());
						ImGui::TableNextRow();

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Position Y:");
						ImGui::TableSetColumnIndex(1);
						ImGui::Text("Rotation Y:");
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(std::to_string(selected->PositionY).c_str());
						ImGui::TableSetColumnIndex(1);
						ImGui::Text(std::to_string(selected->AngleY).c_str());
						ImGui::TableNextRow();

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Position Z:");
						ImGui::TableSetColumnIndex(1);
						ImGui::Text("Rotation Z:");
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(std::to_string(selected->PositionZ).c_str());
						ImGui::TableSetColumnIndex(1);
						ImGui::Text(std::to_string(selected->AngleZ).c_str());
						ImGui::TableNextRow();

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Scale:");
						ImGui::TableSetColumnIndex(1);
						ImGui::Text("Disabled:");
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(std::to_string(selected->Scale).c_str());
						ImGui::TableSetColumnIndex(1);
						ImGui::Text(disabled.c_str());
						ImGui::TableNextRow();

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Base Form ID:");
						ImGui::TableSetColumnIndex(1);
						ImGui::Text("Cell Form ID:");
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(selected->baseFormId.c_str());
						ImGui::TableSetColumnIndex(1);
						ImGui::Text(selected->cell.c_str());

						ImGui::EndTable();
					}
				}
				ImGui::EndChild();
			}

			if (selected->IsDisabled) {
				if (ImGui::Button("Enable")) {
					logger::info("Enabling {}", selected->FormID);
					Diorama::ToggleDisable(*(selected));
					checkTinting = true;
				}
			}
			else {
				if (ImGui::Button("Disable")) {
					logger::info("Disabling {}", selected->FormID);
					Diorama::ToggleDisable(*(selected));
				}
			}

			if (ImGui::Button("Remove reference")) {
				logger::info("Removing {}", selected->FormID);
				selected->IsTinted = false;
				Diorama::TintReference(*(selected));
				Diorama::RemoveObject(*(selected));
				//break;
			}

			if (ImGui::Button("Restore from profile")) {
				Diorama::UpdateFromProfile(*(selected));
				checkTinting = true;
			}

			if (Diorama::PlayerNearRef(*(selected))) {
				if (ImGui::Button("Move to reference")) {
					logger::info("Moving to {}", selected->FormID);
					Diorama::MoveTo(*(selected));
				}
			}
			else {
				if (ImGui::Button("Move to cell")) {
					Menu::Close();
					Diorama::PrepMoveToCell(*(selected));
				}
			}

			if (ImGui::Button("Swap Base")) {
				ImGui::OpenPopup("NewBaseIDModal");
				newBaseID.clear();
			}

			if (UI::Modals::TextInputDialog(
				"NewBaseIDModal",
				"%s",
				ImGuiInputTextFlags_EnterReturnsTrue,
				{},
				{},
				false,
				newBaseID,
				"New Base FormID"))
			{
				logger::info("Updating base ID of {} to {}", selected->FormID, newBaseID);
				Diorama::SwapBase(*(selected), newBaseID.c_str(), "");
				checkTinting = true;
			}

			if (ImGui::Button("Update from world")) {
				logger::info("Updating from world {}", selected->FormID);
				Diorama::UpdateFromWorld(*(selected));
			}

			if (!tintAll) {
				if (ImGui::Checkbox("Tint reference", &selected->IsTinted)) {
					logger::info("Tinting {}", selected->FormID);
					Diorama::TintReference(*(selected));
				}
			}
		}
	}
	else {
		ImGui::Text("Create a profile to get started.");
	}

	ImGui::End();
	ImGui::EndFrame();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Menu::AllReferencesControls()
{
	if (ImGui::Button("Save Profile")) {
		Diorama::Store();
	}

	if (ImGui::Button("Restore Profile")) {
		RestoreAll();
		checkTinting = true;
	}

	if (ImGui::Button("Update Profile from World")) {
		logger::info("Updating all");
		for (auto& ref : Diorama::references) {
			Diorama::UpdateFromWorld(ref);
		}
	}

	if (ImGui::Checkbox("Tint all", &tintAll)) {
		logger::info("Tinting all");
		for (auto& ref : Diorama::references) {
			ref.IsTinted = tintAll;
			Diorama::TintReference(ref);
		}
	}
}

bool Menu::CheckSaveProfile()
{
	if (requestedClose) {
		logger::info("Requested close");
		if (!checkedOnClose && config.checkReferencesOnClose && Diorama::references.size() > 0 && Diorama::currentProfile != nullptr) {
			logger::info("Checking for unsaved changes");
			for (auto& ref : Diorama::references) {
				if (!ref.Saved) {
					logger::info("Unsaved found");
					openSavePopup = true;
					break;
				}
			}
			requestedClose = false;
		}

		if (!openSavePopup) {
			logger::info("No unsaved changes found");
			requestedClose = false;
			Menu::Close();
			return true;
		}

		checkedOnClose = true;
	}

	if (openSavePopup) {
		ImGui::OpenPopup("SaveChangesPopup");
	}

	if (UI::Modals::ConfirmDialog(
		"SaveChangesPopup",
		UI::Modals::cm_func_t(),
		std::optional<float>{},
		isSaveCancelled,
		"%s",
		"Save changes?"))
	{
		logger::info("Popup returned okay");
		Diorama::Store();
		openSavePopup = false;
		requestedClose = false;
		Menu::Close();
		return true;
	}

	if (isSaveCancelled) {
		logger::info("Popup returned cancel");
		openSavePopup = false;
		isSaveCancelled = false;
		requestedClose = false;
		Menu::Close();
		return true;
	}
	
	return false;
}

void Menu::CheckRestoreAll()
{
	if (!checkedOnOpen && config.checkReferencesOnOpen && Diorama::references.size() > 0 && Diorama::currentProfile != nullptr) {
		logger::info("Checking for changes compared to profile");
		for (auto& ref : Diorama::references) {
			ref.matchesProfile = Diorama::ReferenceMatchesWorld(ref);
			if (!ref.matchesProfile) {
				openRestoreAllPopup = true;
				break;
			}
		}

		checkedOnOpen = true;
	}

	if (openRestoreAllPopup) {
		ImGui::OpenPopup("RestoreAllOnOpenPopup");
	}

	if (UI::Modals::ConfirmDialog(
		"RestoreAllOnOpenPopup",
		UI::Modals::cm_func_t(),
		std::optional<float>{},
		isRestoreCancelled,
		"%s",
		"Restore references from profile?"))
	{
		Menu::RestoreAll();
		openRestoreAllPopup = false;
	}

	if (isRestoreCancelled) {
		openRestoreAllPopup = false;
		isRestoreCancelled = false;
	}
}

void Menu::RestoreAll()
{
	logger::info("Restoring all");
	for (auto& ref : Diorama::references) {
		Diorama::UpdateFromProfile(ref);
	}
}

void Menu::ReferenceControlSection()
{
	if (ImGui::Button("Add Targeted")) {
		Diorama::SelectObjectInView();
		Diorama::AddObject(Diorama::GetSelectedObject());
		item_selected_idx = Diorama::references.size() - 1;
		if (tintAll) {
			Diorama::references.back().IsTinted = tintAll;
			Diorama::TintReference(Diorama::references.back());
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Add From Console")) {
		Diorama::SelectFromConsole();
		Diorama::AddObject(Diorama::GetSelectedObject());
		item_selected_idx = Diorama::references.size() - 1;
		if (tintAll) {
			Diorama::references.back().IsTinted = tintAll;
			Diorama::TintReference(Diorama::references.back());
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Add From ID")) {
		ImGui::OpenPopup("AddFromIDModel");
		addFromID.clear();
		item_selected_idx = Diorama::references.size() - 1;
		if (tintAll) {
			Diorama::references.back().IsTinted = tintAll;
			Diorama::TintReference(Diorama::references.back());
		}
	}

	if (UI::Modals::TextInputDialog(
		"AddFromIDModel",
		"%s",
		ImGuiInputTextFlags_EnterReturnsTrue,
		{},
		{},
		false,
		addFromID,
		"Add from FormID"))
	{
		RE::TESObjectREFR* form = RE::TESForm::LookupByID<RE::TESObjectREFR>(Misc::ParseFormID(addFromID));
		Diorama::AddObject(form);
	}
}

void Menu::ProfilesSection()
{
	if (ImGui::BeginCombo("##combo", Diorama::currentProfile)) // The second parameter is the label previewed before opening the combo.
	{
		for (const auto& profileStr : Diorama::profiles)
		{
			auto* profile = profileStr.c_str();
			bool is_selected = (Diorama::currentProfile == profile); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(profile, is_selected)) {
				Diorama::SetProfile(profile);
				Diorama::SaveProfile(profile);
				checkedOnOpen = false;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();

	if (ImGui::Button("+")) {
		ImGui::OpenPopup("ProfileName");
		newProfile.clear();
	}

	ImGui::SameLine();

	if (ImGui::Button("-")) {
		ImGui::OpenPopup("ConfirmDeleteProfile");
	}

	if (UI::Modals::TextInputDialog(
		"ProfileName",
		"%s",
		ImGuiInputTextFlags_EnterReturnsTrue,
		{},
		{},
		false,
		newProfile,
		"Profile new"))
	{
		logger::info("Add new: {}", newProfile);
		Diorama::AddProfile(newProfile.c_str());
	}

	bool confirmCancelled;
	if (UI::Modals::ConfirmDialog(
		"ConfirmDeleteProfile",
		UI::Modals::cm_func_t(),
		std::optional<float>{},
		confirmCancelled,
		"%s",
		"Delete the selected profile?"))
	{
		Diorama::RemoveProfile(Diorama::currentProfile);
	}
}

void Menu::Open()
{
	logger::info("Opening menu");
	const auto UIManager = RE::UI::GetSingleton();

	if (//UIManager->IsMenuOpen("Console") ||       // Text Input
		UIManager->IsMenuOpen("Dialogue Menu") ||   // Dialogue
		UIManager->IsMenuOpen("Crafting Menu") ||   // Text Input
		UIManager->IsMenuOpen("Training Menu") ||   // Just Incase
		UIManager->IsMenuOpen("MagicMenu") ||       // Text Input
		UIManager->IsMenuOpen("Quantity Menu") ||   // Text Input
		UIManager->IsMenuOpen("RaceSex Menu") ||    // Text Input
		UIManager->IsMenuOpen("BarterMenu") ||      // Text Input
		UIManager->IsMenuOpen("InventoryMenu") ||   // Text Input
		UIManager->IsMenuOpen("ContainerMenu") ||   // Text Input
		UIManager->IsMenuOpen("MessageBoxMenu")) {  // Text Input
		return;
	}

	if (RE::Main* Game = RE::Main::GetSingleton()) {
		prevFreezeState = Game->freezeTime;

		//if (Settings::GetSingleton()->GetConfig().pauseGame) {
			Game->freezeTime = true;
		//}
	}

	if (ImGui::GetCurrentContext() != nullptr) {
		auto& io = ImGui::GetIO();

		io.MouseDrawCursor = true;
		io.ClearInputKeys();

		// TODO: Revisit when assigning default focus on open/close.
		ImGui::SetWindowFocus(NULL);
	}

	globals::menuOpen = true;
	checkedOnClose = false;
	Diorama::SelectObject(nullptr);
}

void Menu::PrepClose() {
	requestedClose = true;
}

void Menu::Close()
{
	if (RE::Main* Game = RE::Main::GetSingleton()) {
		Game->freezeTime = prevFreezeState;
	}

	if (ImGui::GetCurrentContext() != nullptr) {
		auto& io = ImGui::GetIO();

		io.MouseDrawCursor = false;
		io.ClearInputKeys();
	}

	globals::menuOpen = false;
}

void Menu::Toggle()
{
	if (globals::menuOpen) {
		PrepClose();
	}
	else {
		Open();
	}
}