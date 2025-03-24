#include <Plugin.h>

void OnDataLoaded()
{
   
}

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		globals::OnInit();
		InputManager::GetSingleton()->Init();
		break;
	case SKSE::MessagingInterface::kPostLoad:
		break;
	case SKSE::MessagingInterface::kPostPostLoad:
		Settings::GetSingleton()->LoadSettings(Settings::ini_mem_path);
		Hooks::Install();
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
        break;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
	SetupLog();

	logger::info("Plugin loaded");
	logger::info("init");

    auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}

	
    return true;
}