#pragma once

#include "Lib/Misc.h"
#include "Lib/Globals.h"
#include "Lib/Settings.h"
#include <list>

class Diorama {
    class Selection {
    public:
        static inline RE::TESObjectREFR* object;
    };

    public:

        class tesRef {

        public:
            tesRef() = default;

            std::string FormID;
            std::string Name;
            bool Saved;
            float PositionX;
            float PositionY;
            float PositionZ;
            float AngleX;
            float AngleY;
            float AngleZ;
            float Scale;
            bool IsSelected;
            std::string mod;

            static tesRef toTesRef(RE::TESObjectREFR* ref) {
                tesRef tesRef;
                tesRef.FormID = Misc::FormIdToHex(ref->GetFormID());
                tesRef.Name = ref->GetName();
                tesRef.Saved = false;
                tesRef.PositionX = ref->GetPositionX();
                tesRef.PositionY = ref->GetPositionY();
                tesRef.PositionZ = ref->GetPositionZ();
                tesRef.AngleX = ref->GetAngleX();
                tesRef.AngleY = ref->GetAngleY();
                tesRef.AngleZ = ref->GetAngleZ();
                tesRef.Scale = ref->GetScale();
                tesRef.mod = Misc::GetModName(ref);
                return tesRef;
            }
        };

        static void Init();
        static void SetProfile(const char* profile);
        static void SetAndSaveProfile(const char* profile);
        static void AddProfile(const char* profile);
        static void RemoveProfile(const char* profile);
        static void SelectObjectInView();
        static void SelectFromConsole();
        static void SelectObject(RE::TESObjectREFR* ref);
        static void AddObject(RE::TESObjectREFR* ref);
        static void RemoveObject(Diorama::tesRef ref);
        static RE::TESObjectREFR* GetSelectedObject();
        static void Store();

        static inline std::list<std::string> profiles;
        static inline const char* currentProfile;
        static inline std::string currentFile;
        static inline std::list<Diorama::tesRef> references;
};