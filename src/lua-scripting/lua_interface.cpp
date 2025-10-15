/********************************************************************************
 * Copyright (c) 2005-2025 Blake Madden
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

#include "lua_interface.h"
#include "../app/readability_app.h"
#include "lua_application.h"
#include "lua_batch_project.h"
#include "lua_debug.h"
#include "lua_screenshot.h"
#include "lua_standard_project.h"

// NOLINTBEGIN(readability-identifier-length)
// NOLINTBEGIN(readability-implicit-bool-conversion)

wxDECLARE_APP(ReadabilityApp);

bool LuaInterpreter::m_isRunning = false;
bool LuaInterpreter::m_quitRequested = false;

//------------------------------------------------------
LuaInterpreter::LuaInterpreter()
    {
    m_L = luaL_newstate();
    luaL_openlibs(m_L);
    luaL_newlib(m_L, LuaScripting::ScreenshotLib);
    lua_setglobal(m_L, "ScreenshotLib");
    luaL_newlib(m_L, LuaScripting::ApplicationLib);
    lua_setglobal(m_L, "Application");
    luaL_newlib(m_L, LuaScripting::DebugLib);
    lua_setglobal(m_L, "Debug");
    Luna<LuaScripting::StandardProject>::Register(m_L);
    Luna<LuaScripting::BatchProject>::Register(m_L);
    }

//------------------------------------------------------
LuaInterpreter::~LuaInterpreter()
    {
    lua_gc(m_L, LUA_GCCOLLECT, 0);
    lua_close(m_L);
    }

//------------------------------------------------------
void LuaInterpreter::RunLuaFile(const wxString& filePath)
    {
    if (IsRunning())
        {
        wxMessageBox(_(L"Another Lua script is already running. "
                       "Please wait for the other script to finish."),
                     _(L"Lua Script"), wxOK | wxICON_INFORMATION);
        return;
        }
    m_quitRequested = false;
    m_isRunning = true;
    SetScriptFilePath(filePath);

    lua_sethook(m_L, &LuaInterpreter::LineHookCallback, LUA_MASKLINE, 0);
    const wxDateTime startTime(wxDateTime::Now());
    if (luaL_dofile(m_L, filePath.utf8_str()) != 0)
        {
        // Error message from Lua has cryptic section in front of it showing the first line of the
        // script and just shows the line number without saying "line" in front of it,
        // so reformat this message to make it more readable.
        wxString errorMessage(luaL_checkstring(m_L, -1), wxConvUTF8);
        const auto endOfErrorHeader = errorMessage.find(L"]:");
        if (endOfErrorHeader != wxString::npos)
            {
            errorMessage.erase(0, endOfErrorHeader + 2);
            }
        wxMessageBox(_(L"Line #") + errorMessage, _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
        LuaScripting::DebugPrint(wxString::Format(
            // TRANSLATORS: %s around "Error" are highlight tags.
            // The last one is a line number.
            _(L"%sError%s: Line #%s"), L"<span style='color:#FF7386; font-weight:bold;'>",
            L"</span>", errorMessage));
        }
    const wxDateTime endTime(wxDateTime::Now());
    LuaScripting::DebugPrint(
        wxString::Format(_(L"Script ran for %s"), endTime.Subtract(startTime).Format()));

    m_quitRequested = false;
    m_isRunning = false;
    }

//------------------------------------------------------
void LuaInterpreter::RunLuaCode(const wxString& code, const wxString& filePath,
                                wxString& errorMessage)
    {
    if (IsRunning())
        {
        wxMessageBox(_(L"Another Lua script is already running. "
                       "Please wait for the other script to finish."),
                     _(L"Lua Script"), wxOK | wxICON_INFORMATION);
        return;
        }
    errorMessage.clear();
    m_quitRequested = false;
    m_isRunning = true;
    SetScriptFilePath(filePath);

    lua_sethook(m_L, &LuaInterpreter::LineHookCallback, LUA_MASKLINE, 0);
    const wxDateTime startTime(wxDateTime::Now());
    if (luaL_dostring(m_L, code.utf8_str()) != 0)
        {
        constexpr std::wstring_view BREAK_LINE{ L"BREAK_LINE:" };
        errorMessage = wxString{ luaL_checkstring(m_L, -1), wxConvUTF8 };

        // user stopped the script
        if (errorMessage.starts_with(BREAK_LINE.data()))
            {
            errorMessage.erase(0, BREAK_LINE.length());
            long lineNumber{ 0 };
            errorMessage.ToLong(&lineNumber);
            LuaScripting::DebugPrint(
                wxString::Format(_(L"Script stopped by user at chunk line #%ld"), lineNumber));
            errorMessage.clear();
            }
        // an actual error
        else
            {
            // Error message from Lua has cryptic section in front of it showing the first line of
            // the script also just shows the line number without saying "line" in front of it,
            // so reformat this message to make it more readable.
            const auto endOfErrorHeader = errorMessage.find(L"]:");
            if (endOfErrorHeader != wxString::npos)
                {
                errorMessage.erase(0, endOfErrorHeader + 2);
                }
            LuaScripting::DebugPrint(
                wxString::Format( // TRANSLATORS: %s around "Error" are highlight
                                  // tags. The last one is a line number.
                    _(L"%sError%s: Chunk line #%s"),
                    L"<span style='color:#FF7386; font-weight:bold;'>", L"</span>", errorMessage));
            }
        }
    const wxDateTime endTime(wxDateTime::Now());
    LuaScripting::DebugPrint(
        wxString::Format(_(L"Script ran for %s"), endTime.Subtract(startTime).Format()));

    m_quitRequested = false;
    m_isRunning = false;

    // in case the script window was hidden and the script either forgot to show it again
    // or the script failed, then show it
    wxGetApp().GetMainFrameEx()->GetLuaEditor()->Show(true);
    }

//------------------------------------------------------
void LuaInterpreter::LineHookCallback(lua_State* L, lua_Debug* ar)
    {
    if (m_quitRequested)
        {
        luaL_error(L, "BREAK_LINE:%d", ar->currentline);
        }
    }

// NOLINTEND(readability-implicit-bool-conversion)
// NOLINTEND(readability-identifier-length)
