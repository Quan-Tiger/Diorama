#include "Diorama.h"
#include "Lib/Globals.h"

void Diorama::Init() {
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

	config = Settings::GetSingleton()->config;
	if (!std::empty(config.selectedProfile)) {
		Diorama::SetProfile(config.selectedProfile.c_str());
	}
}

void Diorama::SaveProfile(const char* profile) {
	auto& config = Settings::GetSingleton()->config;
	config.selectedProfile = profile;
	Settings::GetSingleton()->SaveSettings(std::filesystem::path(Settings::ini_path));
}

void Diorama::SetProfile(const char* profile) {
	Diorama::currentProfile = profile;

	auto path = Settings::json_path + "\\" + Diorama::currentProfile + ".json";
	if (!std::filesystem::exists(path))
	{
		Diorama::RemoveProfile(profile);
		return;
	}

	Diorama::currentFile = path;
	logger::info("Reading json from file: {}", Diorama::currentFile);
	std::ifstream i(Diorama::currentFile);
	json j;
	i >> j;
	Diorama::references.clear();

	for (auto it = j.begin(); it != j.end(); ++it) {
		Diorama::tesRef t;

		t.FormID = it.value()["FormID"];
		t.Name = it.value()["Name"];
		t.Saved = true;
		t.PositionX = it.value()["PosX"];
		t.PositionY = it.value()["PosY"];
		t.PositionZ = it.value()["PosZ"];
		t.AngleX = it.value()["RotX"];
		t.AngleY = it.value()["RotY"];
		t.AngleZ = it.value()["RotZ"];
		t.Scale = it.value()["Scale"];
		t.mod = it.value()["Mod"];
		t.cell = it.value()["Cell"];
		t.cellMod = it.value()["CellMod"];
		t.baseFormId = it.value()["BaseFormId"];
		t.baseFormMod = it.value()["BaseFormMod"];
		t.IsDisabled = it.value()["IsDisabled"];

		Diorama::references.push_back(t);
	}
}

void Diorama::AddProfile(const char* profile) {
	Diorama::profiles.push_back(profile);

	Diorama::currentFile = Settings::json_path + "\\" + profile + ".json";
	logger::info("Adding profile {}", Diorama::currentFile);
	Diorama::references.clear();
	std::ofstream o(Diorama::currentFile);
	o << "[]" << std::endl;
	Diorama::currentProfile = profile;
	Diorama::SaveProfile(profile);
}

void Diorama::RemoveProfile(const char* profile) {
	logger::info("Removing profile {}", profile);
	std::string filepath = Settings::json_path + "\\" + Diorama::currentProfile + ".json";
	for (std::list<std::string>::iterator i = Diorama::profiles.begin(), e = Diorama::profiles.end(); i != e; )
	{
		if (*i == profile)
			i = Diorama::profiles.erase(i);
		else
			++i;
	}

	if (std::filesystem::exists(filepath)) {
		std::remove(filepath.c_str());
	}
	
	if (Diorama::profiles.size() > 0) {
		auto* profile = Diorama::profiles.front().c_str();
		Diorama::SetProfile(profile);
		Diorama::SaveProfile(profile);
	}
	else
	{
		Diorama::currentProfile = nullptr;
		Diorama::references.clear();
		Diorama::SaveProfile("");
	}
}

void Diorama::SelectObjectInView() {
	auto player3d = Misc::GetPlayer3d();
	const auto evaluator = [player3d](RE::NiAVObject* obj) {
		if (obj == player3d) {
			return false;
		}
		return true;
	};

	auto ray = RayCast::Cast(evaluator, 1000);
	Diorama::SelectObject(ray.object);

}

void Diorama::SelectFromConsole() {
	RE::TESObjectREFR* ref = RE::Console::GetSelectedRef().get();
	if (ref != nullptr) {
		logger::info("From console {}", ref->GetFormID());
		Diorama::SelectObject(ref);
	}
	else {
		logger::info("Console ref not found");
	}
}

void Diorama::SelectObject(RE::TESObjectREFR* ref) {
	if (ref) {
		logger::trace("Selecting object [{}]", ref->GetFormID());
	}
	Selection::object = ref;
}

void Diorama::AddObject(RE::TESObjectREFR* ref) {
	if (ref) {
		Diorama::tesRef selected = Diorama::tesRef::toTesRef(ref);
		logger::info("Adding reference [{}]", selected.FormID);
		selected.Saved = false;
		selected.IsSelected = true;

		for (std::list<Diorama::tesRef>::iterator i = Diorama::references.begin(), e = Diorama::references.end(); i != e; )
		{
			if ((*i).FormID == selected.FormID) {
				if ((*i).IsTinted) {
					selected.IsTinted = true;
					Diorama::TintReference(selected);
				}
				i = Diorama::references.erase(i);
				break;
			}
			else
				++i;
		}

		Diorama::references.push_back(selected);
	}
}

void Diorama::RemoveObject(Diorama::tesRef ref) {
	logger::info("Removing reference [{}]", ref.FormID);
	for (std::list<Diorama::tesRef>::iterator i = Diorama::references.begin(), e = Diorama::references.end(); i != e; )
	{
		if ((*i).FormID == ref.FormID) {
			i = Diorama::references.erase(i);
		}
		else
			++i;
	}
	//Diorama::Store();
}

void Diorama::ToggleDisable(Diorama::tesRef& ref) {
	RE::TESObjectREFR* form = Misc::ParseFormIDFromMod<RE::TESObjectREFR>(ref.FormID, ref.mod);
	if (form) {
		if (form->IsDisabled()) {
			ref.IsDisabled = false;
			form->Enable(false);
		}
		else {
			ref.IsDisabled = true;
			form->Disable();
		}
		ref.Saved = false;
	}
}

RE::TESObjectREFR* Diorama::GetSelectedObject() { return Selection::object; }

void Diorama::UpdateFromProfile(Diorama::tesRef& ref) {
	logger::info("Restoring {}", ref.FormID);
	RE::TESObjectREFR* form = Misc::ParseFormIDFromMod<RE::TESObjectREFR>(ref.FormID, ref.mod);
	
	// Dynamically created objects will disappear on reload if the game was not saved after spawning them
	if (!form && ref.FormID.starts_with("0xff")) {
		// If allowed in settings, attempt to spawn a copy of the disappeared object, update its position etc. and replace the old reference in the list
		// Since 0xFFXXXXXX refs that are not loaded will not be able to be looked up, only spawn objects nearby. This prevents duplicates
		if (config.spawnNew && Diorama::PlayerNearRef(ref)) {
			form = Diorama::SpawnObject(ref);
			if (form) {
				ref.FormID = Misc::FormIdToHex(form->GetFormID());
				Diorama::UpdateObject(form, ref);
			}
		}
		return;
	}

	Diorama::UpdateObject(form, ref);
}

void Diorama::UpdateObject(RE::TESObjectREFR* form, Diorama::tesRef& ref)
{
	if (form) {
		form->SetPosition(ref.PositionX, ref.PositionY, ref.PositionZ);
		form->SetAngle(RE::NiPoint3(ref.AngleX, ref.AngleY, ref.AngleZ));
		form->SetScale(ref.Scale);
		Diorama::SwapBase(ref, ref.baseFormId, ref.baseFormMod);
		form->Disable(); // Positions are only reset when forms are re-enabled
		if (!ref.IsDisabled) {
			form->Enable(false); // Only re-enable if set to enabled in profile
		}
	}
}

void Diorama::RunCommand(const std::string cmd)
{
	const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
	const auto script = scriptFactory ? scriptFactory->Create() : nullptr;

	if (script) {
		auto queue = RE::UIMessageQueue::GetSingleton();

		if (queue) {
			queue->AddMessage(RE::BSFixedString("Console"), RE::UI_MESSAGE_TYPE::kShow, nullptr);
			script->SetCommand(cmd);
			script->CompileAndRun(nullptr);
			queue->AddMessage(RE::BSFixedString("Console"), RE::UI_MESSAGE_TYPE::kHide, nullptr);
		}
		else {
			stl::report_and_fail("Failed to get UIMessageQueue singleton");
		}
		delete script;
	}
	else {
		stl::report_and_fail("Failed to create script using scriptFactory.");
	}
}

void Diorama::UpdateFromWorld(Diorama::tesRef& ref) {
	RE::TESObjectREFR* form = Misc::ParseFormIDFromMod<RE::TESObjectREFR>(ref.FormID, ref.mod);
	if (form) {
		Diorama::AddObject(form);
	}
}

void Diorama::MoveTo(const Diorama::tesRef ref) {
	auto* player = RE::PlayerCharacter::GetSingleton();
	RE::TESObjectREFR* form = Misc::ParseFormIDFromMod<RE::TESObjectREFR>(ref.FormID, ref.mod);
	if (form) {
		player->MoveTo(form);
	}
}

void Diorama::PrepMoveToCell(const Diorama::tesRef ref) {
	cocCell = ref.cell;
}

bool Diorama::TryMoveToCell() {
	if (!cocCell.empty()) {
		logger::info("coc to {}", cocCell);
		auto* player = RE::PlayerCharacter::GetSingleton();
		RE::TESObjectCELL* form = RE::TESForm::LookupByID<RE::TESObjectCELL>(Misc::ParseFormID(cocCell));
		if (form) {
			player->CenterOnCell(form);
			return true;
		}
		cocCell = "";
	}
	return false;
}

RE::TESObjectREFR* Diorama::SpawnObject(const Diorama::tesRef ref) {
	logger::info("Trying to spawn new base {}:{}", ref.baseFormId, ref.baseFormMod);
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (player) {
		RE::TESBoundObject* base = Misc::ParseFormIDFromMod<RE::TESBoundObject>(ref.baseFormId, ref.baseFormMod);
		if (base) {
			auto refptr = player->PlaceObjectAtMe(base, false);
			return refptr.get();
		}
	}
	logger::warn("Failed to find base");
	return nullptr;
}


bool Diorama::PlayerNearRef(const Diorama::tesRef ref) {
	auto* player = RE::PlayerCharacter::GetSingleton();
	return std::abs(player->GetPositionX() - ref.PositionX) < config.maxDistanceFromPlayer
		&& std::abs(player->GetPositionY() - ref.PositionY) < config.maxDistanceFromPlayer
		&& std::abs(player->GetPositionZ() - ref.PositionZ) < config.maxDistanceFromPlayer;
}

void Diorama::TintReference(Diorama::tesRef& ref) {
	logger::info("Tinting {}:{}", ref.FormID, ref.mod);
	RE::TESObjectREFR* form = Misc::ParseFormIDFromMod<RE::TESObjectREFR>(ref.FormID, ref.mod);
	if (form) {
		if (auto obj3d = form->Get3D()) {

			ref.matchesProfile = Diorama::ReferenceMatchesWorld(ref);

			auto colour = ref.IsTinted 
				? ref.matchesProfile ? Misc::CreateColor(Misc::ParseFormID(config.tintColour)) : Misc::CreateColor(Misc::ParseFormID(config.tintColourNotMatched))
				: RE::NiColorA(0, 0, 0, 0);
			obj3d->TintScenegraph(colour);
		}
	}
	else {
		logger::warn("Form {}:{} not found", ref.FormID, ref.mod);
	}
}

void Diorama::SwapBase(Diorama::tesRef& ref, std::string baseID, std::string baseMod) {
	if (Misc::HexMatchesFormID(baseID)) {
		if (!baseID.starts_with("0x")) {
			baseID = "0x" + baseID;
		}
		RE::TESObjectREFR* form = Misc::ParseFormIDFromMod<RE::TESObjectREFR>(ref.FormID, ref.mod);
		RE::TESBoundObject* base = Misc::ParseFormIDFromMod<RE::TESBoundObject>(baseID, baseMod);

		if (!form) {
			logger::warn("Form {}:{} not found", ref.FormID, ref.mod);
			return;
		}

		if (!base) {
			logger::warn("Base {}:{} not found", baseID, baseMod);
			return;
		}

		form->SetObjectReference(base);
		form->Disable(); // Bases are only reset when forms are re-enabled
		form->Enable(false);
		ref.baseFormId = Misc::FormIdToHex(base->formID);
		ref.baseFormMod = Misc::GetModName(base);
		ref.Saved = false;
		ref.matchesProfile = false;
	}
	else {
		logger::warn("Invalid id {}", baseID);
	}
}

void Diorama::Store() {
	json j = {};
	
	for (auto& reference : Diorama::references) {
		json j1 = {
			{ "FormID", reference.FormID },
			{ "Name", reference.Name },
			{ "PosX", reference.PositionX },
			{ "PosY", reference.PositionY },
			{ "PosZ", reference.PositionZ },
			{ "RotX", reference.AngleX },
			{ "RotY", reference.AngleY },
			{ "RotZ", reference.AngleZ },
			{ "Scale", reference.Scale },
			{ "Mod", reference.mod },
			{ "Cell", reference.cell },
			{ "CellMod", reference.cellMod },
			{ "BaseFormId", reference.baseFormId },
			{ "BaseFormMod", reference.baseFormMod },
			{ "IsDisabled", reference.IsDisabled }
		};
		j.push_back(j1);
		reference.Saved = true;
	}
	logger::info("Saving json to file: {}", Diorama::currentFile);
	std::ofstream o(Diorama::currentFile);
	o << std::setw(4) << j << std::endl;
}

bool Diorama::ReferenceMatchesWorld(const Diorama::tesRef ref) {
	RE::TESObjectREFR* form = Misc::ParseFormIDFromMod<RE::TESObjectREFR>(ref.FormID, ref.mod);

	if (!form) {
		return false;
	}

	bool posMatches = ref.PositionX == form->GetPositionX()
		&& ref.PositionY == form->GetPositionY()
		&& ref.PositionZ == form->GetPositionZ();
	bool angleMatches = ref.AngleX == form->GetAngleX()
		&& ref.AngleY == form->GetAngleY()
		&& ref.AngleZ == form->GetAngleZ();
	bool scaleMatches = ref.Scale == form->GetScale();
	bool disabledMatches = ref.IsDisabled == form->IsDisabled();

	if (!posMatches || !angleMatches || !scaleMatches || !disabledMatches) {
		return false;
	}
	return Misc::ParseFormID(ref.baseFormId) == form->GetBaseObject()->GetFormID();
}
