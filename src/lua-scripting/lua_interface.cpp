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

/*== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =*\
||                                                                                              ||
||                    +----------------------------------------------+                          ||
||                    | /   /   /   /   /   /      @@@@@@@@@@@@@@@@@@|                          ||
||                    |                       /      @@@@@@@@@@@@@@@@|                          ||
||                    |                  /        /    @@@@@@@@@@@@@@|                          ||
||                    |              /        /          @@@@@@@@@@@@|                          ||
||                    |          /         /        /      @@@@@@@@@@|                          ||
||                    |      /          /         /    /     @@@@@@@@|                          ||
||                    |              /          /     /     /  @@@@@@|                          ||
||                    |           /           /      /      /    @@@@|                          ||
||                    |@       /            /       /       /      @@|                          ||
||                    |@@@                /        /        /       @|                          ||
||                    |@@@@@@           /         /         /        |                          ||
||                    |@@@@@@@@@                 /          /        |                          ||
||                    |@@@@@@@@@@@              /           /        |                          ||
||                    |@@@@@@@@@@@@@@          /            /        |                          ||
||                    |@@@@@@@@@@@@@@@@@                    /        |                          ||
||                    |@@@@@@@@@@@@@@@@@@@@                 /        |                          ||
||                    |@@@@@@@@@@@@@@@@@@@@@@@                       |                          ||
||                    +----------------------------------------------+                          ||
||                                                                                              ||
||    RRRRR   EEEEE   AAAAA   DDDD    AAAAA   BBBBB   IIIII   L       IIIII   TTTTT    Y   Y    ||
||    R   R   E       A   A   D   D   A   A   B   B     I     L         I       T      Y   Y    ||
||    RRRR    EEEE    AAAAA   D   D   AAAAA   BBBB      I     L         I       T       Y Y     ||
||    R  R    E       A   A   D   D   A   A   B   B     I     L         I       T        Y      ||
||    R   R   EEEEE   A   A   DDDD    A   A   BBBBB   IIIII   LLLLL   IIIII     T        Y      ||
||                                                                                              ||
||                                            SSSS    TTTTT   U   U   DDDD    IIIII   OOOO      ||
||                                            S         T     U   U   D   D     I    O    O     ||
||                                            SSSS      T     U   U   D   D     I    O    O     ||
||                                               S      T     U   U   D   D     I    O    O     ||
||                                            SSSS      T     UUUUU   DDDD    IIIII   OOOO      ||
||                                                                                              ||
\*== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =*/

#include "lua_interface.h"
#include "../app/readability_app.h"
#include "lua_application.h"
#include "lua_batch_project.h"
#include "lua_debug.h"
#include "lua_screenshot.h"
#include "lua_standard_project.h"
#include <cstring>

// NOLINTBEGIN(readability-identifier-length)
// NOLINTBEGIN(readability-implicit-bool-conversion)

wxDECLARE_APP(ReadabilityApp);

bool LuaInterpreter::m_isRunning = false;
bool LuaInterpreter::m_quitRequested = false;
bool LuaInterpreter::m_isPaused = false;
bool LuaInterpreter::m_continueRequested = false;
int LuaInterpreter::m_pausedLine = -1;
int LuaInterpreter::m_justResumedFromLine = -1;
std::set<int> LuaInterpreter::m_breakpointLines;
std::function<void(int)> LuaInterpreter::m_pauseStateChangedCallback;

//------------------------------------------------------
bool LuaInterpreter::ParseLuaError(wxString& errorMessage, long& lineNumber)
    {
    constexpr std::wstring_view BREAK_LINE{ _DT(
        L"BREAK_LINE:", DTExplanation::InternalKeyword,
        L"String is from Lua itself and always in English") };
    const bool isUserStop = errorMessage.find(BREAK_LINE.data()) != wxString::npos;
    // strip whichever chunk-name header Lua prepended: "[string ...]:" for
    // ad-hoc code (e.g., RunLuaFile()), or "workbench:" for the fixed chunk
    // name used by RunLuaCode()
    const wxString workbenchHeader = wxString(WORKBENCH_CHUNK_NAME + 1) + L":";
    if (const auto endOfErrorHeader = errorMessage.find(L"]:"); endOfErrorHeader != wxString::npos)
        {
        errorMessage.erase(0, endOfErrorHeader + 2);
        }
    else if (const auto workbenchHeaderPos = errorMessage.find(workbenchHeader);
             workbenchHeaderPos != wxString::npos)
        {
        errorMessage.erase(0, workbenchHeaderPos + workbenchHeader.length());
        }
    if (isUserStop)
        {
        if (errorMessage.starts_with(BREAK_LINE.data()))
            {
            errorMessage.erase(0, BREAK_LINE.length());
            }
        lineNumber = 0;
        errorMessage.ToLong(&lineNumber);
        }
    return !isUserStop;
    }

//------------------------------------------------------
void LuaInterpreter::Initialize()
    {
    if (m_L != nullptr)
        {
        return;
        }

    m_L = luaL_newstate();

    // NOLINTBEGIN(readability-math-missing-parentheses,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

    // always load safe standard libraries
    luaL_requiref(m_L, LUA_GNAME, luaopen_base, 1);
    lua_pop(m_L, 1);
    luaL_requiref(m_L, LUA_TABLIBNAME, luaopen_table, 1);
    lua_pop(m_L, 1);
    luaL_requiref(m_L, LUA_STRLIBNAME, luaopen_string, 1);
    lua_pop(m_L, 1);
    luaL_requiref(m_L, LUA_MATHLIBNAME, luaopen_math, 1);
    lua_pop(m_L, 1);
    luaL_requiref(m_L, LUA_UTF8LIBNAME, luaopen_utf8, 1);
    lua_pop(m_L, 1);
    luaL_requiref(m_L, LUA_COLIBNAME, luaopen_coroutine, 1);
    lua_pop(m_L, 1);
    luaL_requiref(m_L, LUA_LOADLIBNAME, luaopen_package, 1);
    lua_pop(m_L, 1);

    // only load potentially dangerous libraries if unsafe mode is enabled
    wxASSERT_MSG(wxGetApp().GetAppOptions() != nullptr,
                 L"App options must be loaded before initializing Lua interpreter!");
    if (wxGetApp().GetAppOptions() != nullptr &&
        wxGetApp().GetAppOptions()->IsLuaUnsafeModeEnabled())
        {
        luaL_requiref(m_L, LUA_IOLIBNAME, luaopen_io, 1);
        lua_pop(m_L, 1);
        luaL_requiref(m_L, LUA_OSLIBNAME, luaopen_os, 1);
        lua_pop(m_L, 1);
        luaL_requiref(m_L, LUA_DBLIBNAME, luaopen_debug, 1);
        lua_pop(m_L, 1);
        }

    luaL_newlib(m_L, LuaScripting::ScreenshotLib);
    lua_setglobal(m_L, "ScreenshotLib");
    luaL_newlib(m_L, LuaScripting::ApplicationLib);
    lua_setglobal(m_L, "Application");
    luaL_newlib(m_L, LuaScripting::DebugLib);
    lua_setglobal(m_L, "Debug");
    Luna<LuaScripting::StandardProject>::Register(m_L);
    Luna<LuaScripting::BatchProject>::Register(m_L);
    // NOLINTEND(readability-math-missing-parentheses,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
    }

//------------------------------------------------------
void LuaInterpreter::Restart()
    {
    if (IsRunning())
        {
        return;
        }

    if (m_L != nullptr)
        {
        lua_gc(m_L, LUA_GCCOLLECT, 0);
        lua_close(m_L);
        m_L = nullptr;
        }
    Initialize();
    }

//------------------------------------------------------
LuaInterpreter::~LuaInterpreter()
    {
    if (m_L != nullptr)
        {
        lua_gc(m_L, LUA_GCCOLLECT, 0);
        lua_close(m_L);
        }
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
    // no UI to service a pause here, so never break on stale breakpoints from the workbench
    m_isPaused = false;
    m_pausedLine = -1;
    m_continueRequested = false;
    m_breakpointLines.clear();
    SetScriptFilePath(filePath);

    lua_sethook(m_L, &LuaInterpreter::LineHookCallback, LUA_MASKLINE, 0);
    const wxDateTime startTime(wxDateTime::Now());
    if (luaL_dofile(m_L, filePath.utf8_str()) != 0)
        {
        wxString errorMessage(luaL_checkstring(m_L, -1), wxConvUTF8);
        long lineNumber{ 0 };
        if (ParseLuaError(errorMessage, lineNumber))
            {
            const bool isUnsafeLibError = errorMessage.Contains(_DT(L"(global 'os')")) ||
                                          errorMessage.Contains(_DT(L"(global 'io')")) ||
                                          errorMessage.Contains(_DT(L"(global 'debug')"));
            wxMessageBox(
                _(L"Line #") + errorMessage +
                    (isUnsafeLibError ?
                         _(L"\n\nTo fix this, enable \"Allow Lua scripts to access system "
                           L"commands\" under Tools » Options » General Settings and restart.") :
                         wxString{}),
                _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            LuaScripting::DebugPrint(wxString::Format(
                // TRANSLATORS: %s around "Error" are highlight tags.
                // The last one is a line number.
                _(L"%sError%s: Line #%s"), L"<span style='color:#FF7386; font-weight:bold;'>",
                L"</span>", errorMessage));
            if (isUnsafeLibError)
                {
                LuaScripting::DebugPrint(
                    _(L"To fix this, enable \"Allow Lua scripts to access system commands\" "
                      "under Tools » Options » General Settings and restart."));
                }
            }
        else
            {
            LuaScripting::DebugPrint(
                wxString::Format(_(L"Script stopped by user at line #%ld"), lineNumber));
            }
        }
    const wxDateTime endTime(wxDateTime::Now());
    LuaScripting::DebugPrint(
        wxString::Format(_(L"⏱️Script ran for %s"), endTime.Subtract(startTime).Format()));

    m_quitRequested = false;
    m_isRunning = false;
    m_isPaused = false;
    m_pausedLine = -1;
    m_breakpointLines.clear();
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
    m_isPaused = false;
    m_pausedLine = -1;
    m_continueRequested = false;
    SetScriptFilePath(filePath);

    lua_sethook(m_L, &LuaInterpreter::LineHookCallback, LUA_MASKLINE, 0);
    const wxDateTime startTime(wxDateTime::Now());
    const auto codeUtf8 = code.utf8_str();
    // load with a fixed chunk name (rather than luaL_dostring's default of using
    // the code itself) so breakpoints can be scoped to this specific chunk
    if (luaL_loadbuffer(m_L, codeUtf8.data(), codeUtf8.length(), WORKBENCH_CHUNK_NAME) != 0 ||
        lua_pcall(m_L, 0, LUA_MULTRET, 0) != 0)
        {
        errorMessage = wxString{ luaL_checkstring(m_L, -1), wxConvUTF8 };
        long lineNumber{ 0 };
        if (ParseLuaError(errorMessage, lineNumber))
            {
            const bool isUnsafeLibError = errorMessage.Contains(_DT(L"(global 'os')")) ||
                                          errorMessage.Contains(_DT(L"(global 'io')")) ||
                                          errorMessage.Contains(_DT(L"(global 'debug')"));
            LuaScripting::DebugPrint(
                wxString::Format( // TRANSLATORS: %s around "Error" are highlight
                                  // tags. The last one is a line number.
                    _(L"❌%sError%s: Chunk line #%s"),
                    L"<span style='color:#FF7386; font-weight:bold;'>", L"</span>", errorMessage));
            if (isUnsafeLibError)
                {
                LuaScripting::DebugPrint(
                    _(L"To fix this, enable \"Allow Lua scripts to access system commands\" "
                      "under Tools » Options » General Settings and restart."));
                }
            }
        else
            {
            LuaScripting::DebugPrint(
                wxString::Format(_(L"Script stopped by user at chunk line #%ld"), lineNumber));
            errorMessage.clear();
            }
        }
    const wxDateTime endTime(wxDateTime::Now());
    LuaScripting::DebugPrint(
        wxString::Format(_(L"⏱️Script ran for %s"), endTime.Subtract(startTime).Format()));

    m_quitRequested = false;
    m_isRunning = false;
    m_isPaused = false;
    m_pausedLine = -1;
    m_breakpointLines.clear();
    m_pauseStateChangedCallback = nullptr;

    // in case the script window was hidden and the script either forgot to show it again
    // or the script failed, then show it
    wxGetApp().GetMainFrameEx()->ActivateScriptWorkbench();
    }

//------------------------------------------------------
void LuaInterpreter::LineHookCallback(lua_State* L, lua_Debug* ar)
    {
    static uint64_t callCount{ 0 };
    // Periodically yield to the UI thread to keep the application responsive
    // (e.g., allowing the Stop button to be clicked) without degrading script performance.
    if (++callCount % 10 == 0)
        {
        wxSafeYield(wxGetApp().GetMainFrameEx(), true);
        }
    if (m_quitRequested)
        {
        // NOLINTBEGIN(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
        luaL_error(L, "BREAK_LINE:%d", ar->currentline);
        // NOLINTEND(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
        }

    // checked on every line (not just every 10th, like the yield above) so a
    // breakpoint can never be stepped over
    bool isWorkbenchBreakpointLine = false;
    if (!m_isPaused && m_breakpointLines.contains(ar->currentline))
        {
        // Only pause if this line actually belongs to the workbench's own chunk,
        // otherwise, a coincidentally-matching line number inside a dofile()'d/
        // required chunk (e.g., the Lua constants file) could falsely trigger this.
        lua_getinfo(L, "S", ar);
        isWorkbenchBreakpointLine = (std::strcmp(ar->short_src, WORKBENCH_CHUNK_NAME + 1) == 0);
        }

    // ignore a single spurious re-hit of the line just resumed from; only ever suppresses once
    if (ar->currentline == m_justResumedFromLine)
        {
        isWorkbenchBreakpointLine = false;
        }
    m_justResumedFromLine = -1;

    if (isWorkbenchBreakpointLine)
        {
        m_isPaused = true;
        m_pausedLine = ar->currentline;
        m_continueRequested = false;
        if (m_pauseStateChangedCallback)
            {
            m_pauseStateChangedCallback(m_pausedLine);
            }

        while (!m_continueRequested && !m_quitRequested)
            {
            wxMilliSleep(20);
            wxSafeYield(wxGetApp().GetMainFrameEx(), true);
            }

        m_isPaused = false;
        m_justResumedFromLine = m_pausedLine;
        m_pausedLine = -1;
        m_continueRequested = false;
        if (m_pauseStateChangedCallback)
            {
            m_pauseStateChangedCallback(-1);
            }

        if (m_quitRequested)
            {
            // NOLINTBEGIN(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
            luaL_error(L, "BREAK_LINE:%d", ar->currentline);
            // NOLINTEND(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
            }
        }
    }

// NOLINTEND(readability-implicit-bool-conversion)
// NOLINTEND(readability-identifier-length)
