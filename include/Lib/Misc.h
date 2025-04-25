#pragma once
#include <glm/ext.hpp>
#include <glm/glm.hpp>

// Taken from Object Manipulation Overhaul https://github.com/Thiago099/Object-Manipulation-Overhaul

namespace Misc {
    bool IsEqual(const std::string& a, const std::string& b);
    void SetPosition(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position);
    void SetAngle(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position);
    float DistanceBetweenTwoPoints(RE::NiPoint3& a, RE::NiPoint3& b);
    float NormalizeAngle(float angle_rad);
    RE::NiColorA CreateColor(uint32_t color);
    bool IsStatic(RE::COL_LAYER& col);
    RE::NiObject* GetPlayer3d();
    void MoveTo_Impl(RE::TESObjectREFR* ref, const RE::ObjectRefHandle& a_targetHandle, RE::TESObjectCELL* a_targetCell,
                     RE::TESWorldSpace* a_selfWorldSpace, const RE::NiPoint3& a_position,
                     const RE::NiPoint3& a_rotation);
    bool DoesButtonTriggerAction(RE::ButtonEvent* button, std::string action);
    RE::MATERIAL_ID StringToMaterialId(const std::string& str);
    RE::FormType StringToFormType(const std::string& str);
    std::string ToLowerCase(const std::string& str);
    std::vector<std::string> GetLowerCaseComponents(const std::string& s);

    /////////

    std::string GetModName(RE::TESForm* ref);
    std::string FormIdToHex(uint32_t FormID);
    uint64_t ParseFormID(std::string FormID);
    bool HexMatchesFormID(std::string toCheck);

    template <class T>
    static T* ParseFormIDFromMod(const std::string& id, const std::string& mod)
    {
        if (mod.empty()) {
            return RE::TESForm::LookupByID<T>(Misc::ParseFormID(id));
        }

        RE::FormID localFormID;
        std::istringstream{ id } >> std::hex >> localFormID;

        RE::FormID formID = RE::TESDataHandler::GetSingleton()->LookupFormID(localFormID, mod);
        T* form = RE::TESForm::LookupByID<T>(formID);
        if (form) {
            return form;
        }
        else {
            return RE::TESForm::LookupByID<T>(Misc::ParseFormID(id));
        }
    }

}

