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
            std::string cell;
            std::string cellMod;
            std::string baseFormId;
            std::string baseFormMod;
            bool IsDisabled;
            bool IsTinted = false;
            bool matchesProfile;

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
                auto* cell = ref->parentCell;
                //auto group = ref->
                tesRef.cell = Misc::FormIdToHex(cell->GetFormID());
                tesRef.cellMod = Misc::GetModName(cell);
                auto* base = ref->GetBaseObject();
                tesRef.baseFormId = Misc::FormIdToHex(base->formID);
                tesRef.baseFormMod = Misc::GetModName(base);
                tesRef.IsDisabled = ref->IsDisabled();
                tesRef.matchesProfile = true;
                return tesRef;
            }
        };

        static void Init();
        static void SetProfile(const char* profile);
        static void SaveProfile(const char* profile);
        static void AddProfile(const char* profile);
        static void RemoveProfile(const char* profile);
        static void SelectObjectInView();
        static void SelectFromConsole();
        static void SelectObject(RE::TESObjectREFR* ref);
        static void AddObject(RE::TESObjectREFR* ref);
        static void RemoveObject(Diorama::tesRef ref);
        static void SwapBase(Diorama::tesRef& ref, std::string baseId, std::string baseMod);
        static RE::TESObjectREFR* GetSelectedObject();
        static void Store();
        static void ToggleDisable(Diorama::tesRef& ref);
        static void UpdateFromProfile(Diorama::tesRef& ref);
        static void UpdateObject(RE::TESObjectREFR* form, Diorama::tesRef& ref);
        static void RunCommand(const std::string cmd);
        static void UpdateFromWorld(Diorama::tesRef& ref);
        static void MoveTo(const Diorama::tesRef ref);
        static bool TryMoveToCell();
        static void PrepMoveToCell(const Diorama::tesRef ref);
        static RE::TESObjectREFR* SpawnObject(const Diorama::tesRef ref);
        static bool PlayerNearRef(const Diorama::tesRef ref);
        static void TintReference(Diorama::tesRef& ref);
        static bool ReferenceMatchesWorld(const Diorama::tesRef ref);

        static inline std::list<std::string> profiles;
        static inline const char* currentProfile;
        static inline std::string currentFile;
        static inline std::list<Diorama::tesRef> references;
        static inline std::string cocCell = "";
        //static inline uint32_t tintColour;
        //static inline bool spawnNew;
        //static inline bool spawnNew;
        //static inline bool spawnNew;
        //static inline uint32_t maxDistanceFromPlayer;
        static inline Settings::Config config;
};