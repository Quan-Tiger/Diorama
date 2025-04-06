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
	Diorama::profiles = {};

	if (std::filesystem::exists(Settings::json_path)) {
		logger::info("Json directory found");
		for (const auto& entry : std::filesystem::directory_iterator(Settings::json_path))
		{
			std::string profile = entry.path().stem().generic_string();
			logger::info("Profile found: {}", profile);
			Diorama::profiles.push_back(profile);
		}
	}
	if (Diorama::references.size() > 0) {
		selectedHistory = Diorama::references.front();
	}
}

void Menu::DrawOverlay()
{
	if (!globals::menuOpen) {
		return;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::SetNextWindowSize(ImVec2(1000, 1000));

	ImGui::Begin("Diorama", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginCombo("##combo", Diorama::currentProfile)) // The second parameter is the label previewed before opening the combo.
	{
		for (const auto& profileStr : Diorama::profiles)
		{
			auto* profile = profileStr.c_str();
			bool is_selected = (Diorama::currentProfile == profile); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(profile, is_selected))
				Diorama::SetAndSaveProfile(profile);
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("+")) {
		ImGui::OpenPopup("ProfileName");
		newProfile.clear();
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

	if (ImGui::Button("-")) {
		ImGui::OpenPopup("ConfirmDeleteProfile");
	}

	if (UI::Modals::ConfirmDialog(
		"ConfirmDeleteProfile",
		UI::Modals::cm_func_t(),
		std::optional<float>{},
		"%s",
		"Delete the selected profile?"))
	{
		Diorama::RemoveProfile(Diorama::currentProfile);
	}

	ImGui::Separator();

	if (ImGui::Button("Select By Target")) {
		Diorama::SelectObjectInView();
	}

	if (ImGui::Button("Select From Console")) {
		Diorama::SelectFromConsole();
	}

	RE::TESObjectREFR* selected = Diorama::GetSelectedObject();
	//const auto mod = Misc::GetModName(selected);
	if (selected) {
		ImGui::Text(std::format("Selected object [{}] name: {}",
			selected->GetFormID(),
			selected->GetName()).c_str());

		//ImGui::Text(std::format("Looking at object [{}] name: {}, x: {}, y: {}, z: {}, rotX: {}, rotY: {}, rotZ: {}, scale: {}",
		//	Misc::FormIdToHex(selected->GetFormID()),
		//	selected->GetName(),
		//	selected->GetPositionX(),
		//	selected->GetPositionY(),
		//	selected->GetPositionZ(),
		//	selected->GetAngleX(),
		//	selected->GetAngleY(),
		//	selected->GetAngleZ(),
		//	selected->GetScale()).c_str());

		ImGui::Text(("Position X" + std::to_string(selected->GetPositionX())).c_str());
		ImGui::Text(("Position Y" + std::to_string(selected->GetPositionY())).c_str());
		ImGui::Text(("Position Z" + std::to_string(selected->GetPositionZ())).c_str());
		ImGui::Text(("Rotation X" + std::to_string(selected->GetAngleX())).c_str());
		ImGui::Text(("Rotation Y" + std::to_string(selected->GetAngleY())).c_str());
		ImGui::Text(("Rotation Z" + std::to_string(selected->GetAngleZ())).c_str());
		ImGui::Text(("Scale" + std::to_string(selected->GetScale())).c_str());

		//ImGui::Text(std::format("Mod: {}", mod).c_str());

		if (ImGui::Button("Add Reference")) {
			logger::info("Storing...");
			Diorama::AddObject(selected);
		}
	}
	
	ImGui::Separator();

	if (Diorama::currentProfile != nullptr) {

		if (Diorama::references.size() > 0) {

			// imgui clipper
			// If hover
			// Is selected
			//Diorama::tesRef contextRef;
			//for (Diorama::tesRef ref : Diorama::references) {
			//	std::string hexRef = "##" + Misc::FormIdToHex(ref.FormID);
			//	if (ImGui::BeginChild(hexRef.c_str(), ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoMove)) {
			//		std::string value;
			//		if (!ref.Saved) {
			//			value += "*";
			//		}
			//		value += hexRef;
			//		ImGui::Text(value.c_str());

			//		ImGui::EndChild();
			//	}

			//	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			//		contextRef = ref;
			//		logger::info("Right clicked on {}", Misc::FormIdToHex(contextRef.FormID));
			//		ImGui::OpenPopup("TableViewContextMenu");
			//	}
			//}

			//if (ImGui::BeginPopupContextItem("TableViewContextMenu")) {
			//	if (ImGui::MenuItem("Remove reference")) {
			//		Diorama::RemoveObject(contextRef);
			//	}
			//	ImGui::EndPopup();
			//}
			ImGui::Columns(2);
			ImGui::BeginListBox("##RefList");
			for (Diorama::tesRef ref : Diorama::references) {
				std::string value;
				if (!ref.Saved) {
					value += "*";
				}
				value += ref.FormID;
				//ImGui::Text(value.c_str());
				if (ImGui::Selectable(value.c_str(), ref.IsSelected))
				{
					logger::info("Selected {}", value);
					selectedHistory = ref;
				}
			}
			ImGui::EndListBox();
			ImGui::NextColumn();

			//ImGui::Text(std::format("Selected object [{}] name: {}, x: {}, y: {}, z: {}, rotX: {}, rotY: {}, rotZ: {}, scale: {}",
			//	Misc::FormIdToHex(selectedHistory.FormID),
			//	selectedHistory.Name,
			//	selectedHistory.PositionX,
			//	selectedHistory.PositionY,
			//	selectedHistory.PositionZ,
			//	selectedHistory.AngleX,
			//	selectedHistory.AngleY,
			//	selectedHistory.AngleZ,
			//	selectedHistory.Scale).c_str());

			//ImGui::Text(std::format("Selected object [{}] name: {}",
			//	selectedHistory.FormID,
			//	selected->GetName()).c_str());

			ImGui::Text(("Position X" + std::to_string(selectedHistory.PositionX)).c_str());
			ImGui::Text(("Position Y" + std::to_string(selectedHistory.PositionY)).c_str());
			ImGui::Text(("Position Z" + std::to_string(selectedHistory.PositionZ)).c_str());
			ImGui::Text(("Rotation X" + std::to_string(selectedHistory.AngleX)).c_str());
			ImGui::Text(("Rotation Y" + std::to_string(selectedHistory.AngleY)).c_str());
			ImGui::Text(("Rotation Z" + std::to_string(selectedHistory.AngleZ)).c_str());
			ImGui::Text(("Scale" + std::to_string(selectedHistory.Scale)).c_str());

			//ImGui::Text(std::format("Mod: {}", mod).c_str());

			if (ImGui::Button("Add Reference")) {
				logger::info("Storing...");
				Diorama::AddObject(selected);
			}
		}

		if (ImGui::Button("Save all references")) {
			Diorama::Store();
		}
	}

	ImGui::End();
	ImGui::EndFrame();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Menu::Open()
{

	const auto UIManager = RE::UI::GetSingleton();

	if (//UIManager->IsMenuOpen("Console") ||         // Text Input
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
		Close();
	}
	else {
		Open();
	}
}