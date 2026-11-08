/********************************************************************************
 * Copyright (c) 2005-2026 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

#include "readability_lua_interpreter.h"
#include "../app/readability_app.h"
#include "lua_application.h"
#include "lua_batch_project.h"
#include "lua_screenshot.h"
#include "lua_standard_project.h"

// NOLINTBEGIN(readability-identifier-length)

wxDECLARE_APP(ReadabilityApp);

//------------------------------------------------------
void ReadabilityLuaInterpreter::RegisterLibraries(lua_State* L)
    {
    // NOLINTBEGIN(readability-math-missing-parentheses,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
    luaL_newlib(L, LuaScripting::ScreenshotLib2);
    lua_setglobal(L, "Screenshot2");
    luaL_newlib(L, LuaScripting::ApplicationLib);
    lua_setglobal(L, "Application");
    Luna<LuaScripting::StandardProject>::Register(L);
    Luna<LuaScripting::BatchProject>::Register(L);
    // NOLINTEND(readability-math-missing-parentheses,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
    }

//------------------------------------------------------
bool ReadabilityLuaInterpreter::IsUnsafeModeEnabled() const
    {
    wxASSERT_MSG(wxGetApp().GetAppOptions() != nullptr,
                 L"App options must be loaded before initializing Lua interpreter!");
    return wxGetApp().GetAppOptions() != nullptr &&
           wxGetApp().GetAppOptions()->IsLuaUnsafeModeEnabled();
    }

//------------------------------------------------------
wxString ReadabilityLuaInterpreter::GetUnsafeModeHint() const
    {
    return _(L"To fix this, enable \"Allow Lua scripts to access system commands\" "
             "under Tools » Options » General Settings and restart.");
    }

//------------------------------------------------------
void ReadabilityLuaInterpreter::OnLuaCodeFinished()
    {
    // in case the script window was hidden and the script either forgot to show it again
    // or the script failed, then show it
    wxGetApp().GetMainFrameEx()->ActivateScriptWorkbench();
    }

//------------------------------------------------------
void ReadabilityLuaInterpreter::DescribeUserData(LuaVariableInfo& info, const int idx) const
    {
    if (info.m_type == wxString::FromUTF8(LuaScripting::StandardProject::className))
        {
        if (const auto* project =
                Luna<LuaScripting::StandardProject>::lightcheck(GetLuaState(), idx);
            project != nullptr)
            {
            if (const wxString docSummary = project->GetDebugSummary(); !docSummary.empty())
                {
                info.m_value = docSummary;
                }
            }
        }
    }

// NOLINTEND(readability-identifier-length)
