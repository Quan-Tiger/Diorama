#pragma once

#include <spdlog/sinks/basic_file_sink.h>
#include <shared_mutex>
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"

namespace logger = SKSE::log;
using namespace std::literals;

using FormID = RE::FormID;
using RefID = RE::FormID;

const RefID player_refid = 20;

using SharedLock = std::shared_mutex;
using WriteLocker = std::unique_lock<SharedLock>;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace stl
{
	using namespace SKSE::stl;

	template <class T, std::size_t Size = 5>
	void write_thunk_call(std::uintptr_t a_src)
	{
		SKSE::AllocTrampoline(14);
		auto& trampoline = SKSE::GetTrampoline();
		if (Size == 6) {
			T::func = *(uintptr_t*)trampoline.write_call<6>(a_src, T::thunk);
		}
		else {
			T::func = trampoline.write_call<Size>(a_src, T::thunk);
		}
	}

	template <class F, size_t index, class T>
	void write_vfunc()
	{
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[index] };
		T::func = vtbl.write_vfunc(T::size, T::thunk);
	}

	template <std::size_t idx, class T>
	void write_vfunc(REL::VariantID id)
	{
		REL::Relocation<std::uintptr_t> vtbl{ id };
		T::func = vtbl.write_vfunc(idx, T::thunk);
	}

	template <class T>
	void write_thunk_jmp(std::uintptr_t a_src)
	{
		SKSE::AllocTrampoline(14);
		auto& trampoline = SKSE::GetTrampoline();
		T::func = trampoline.write_branch<5>(a_src, T::thunk);
	}

	template <class F, class T>
	void write_vfunc()
	{
		write_vfunc<F, 0, T>();
	}
}

#include "Lib/Globals.h"
