#include "Diorama.h"
#include "Lib/Globals.h"

void Diorama::Init() {
	auto& config = Settings::GetSingleton()->config;
	if (!std::empty(config.selectedProfile)) {
		Diorama::SetProfile(config.selectedProfile.c_str());
	}
}

void Diorama::SetAndSaveProfile(const char* profile) {
	Diorama::SetProfile(profile);

	auto& config = Settings::GetSingleton()->config;
	config.selectedProfile = Diorama::currentProfile;
	Settings::GetSingleton()->SaveSettings(std::filesystem::path(Settings::ini_path));
}

void Diorama::SetProfile(const char* profile) {
	Diorama::currentProfile = profile;

	Diorama::currentFile = Settings::json_path + "\\" + Diorama::currentProfile + ".json";

	if (!std::filesystem::exists(Diorama::currentFile)) {
		Diorama::AddProfile(profile);
	}
	else {
		logger::info("Reading json from file: {}", Diorama::currentFile);
		std::ifstream i(Diorama::currentFile);
		json j;
		i >> j;
		Diorama::references = {};

		// TODO: Build from FormID instead, handle missing
		logger::info("Started reading json");

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

			Diorama::references.push_back(t);
		}
		logger::info("Finished reading json");
	}
}

void Diorama::AddProfile(const char* profile) {
	Diorama::profiles.push_back(profile);
	Diorama::currentProfile = profile;

	Diorama::currentFile = Settings::json_path + "\\" + Diorama::currentProfile + ".json";
	std::ofstream o(Diorama::currentFile);
	o << "[]" << std::endl;
}

void Diorama::RemoveProfile(const char* profile) {
	std::string filepath = Settings::json_path + "\\" + Diorama::currentProfile + ".json";
	for (std::list<std::string>::iterator i = Diorama::profiles.begin(), e = Diorama::profiles.end(); i != e; )
	{
		if (*i == profile)
			i = Diorama::profiles.erase(i);
		else
			++i;
	}
	std::remove(filepath.c_str());
	Diorama::currentProfile = nullptr;
	if (Diorama::profiles.size() > 0) {
		Diorama::currentProfile = Diorama::profiles.front().c_str();
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
	//static REL::Relocation<RE::ObjectRefHandle*> selectedRef{ RELOCATION_ID(519394, 504099) }; // TODO: Upgrade to latest SKSE
	//RE::NiPointer<RE::TESObjectREFR> ref = selectedRef->get();
	logger::info("Checking console");

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
		Selection::object = ref;
	}
}

void Diorama::AddObject(RE::TESObjectREFR* ref) {
	if (ref) {
		logger::info("Adding reference [{}]", ref->GetFormID());
		Diorama::tesRef selected = Diorama::tesRef::toTesRef(Selection::object);
		selected.Saved = false;

		for (Diorama::tesRef existing : Diorama::references) {
			if (existing.FormID == selected.FormID) {
				return;
			}
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
}

RE::TESObjectREFR* Diorama::GetSelectedObject() { return Selection::object; }

void Diorama::Store() {
	logger::info("Deserialising selected object: {}", Selection::object->formID);
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
			{ "Mod", reference.mod }
		};
		j.push_back(j1);
		reference.Saved = true;
	}
	logger::info("Saving json to file: {}", Diorama::currentFile);
	std::ofstream o(Diorama::currentFile);
	o << std::setw(4) << j << std::endl;
}