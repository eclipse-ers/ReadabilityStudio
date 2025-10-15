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

#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H

#include "luna.h"
#include <wx/wx.h>

// NOLINTBEGIN(readability-identifier-length)
// NOLINTBEGIN(readability-implicit-bool-conversion)

/// @brief Interface for running Lua code.
/// @details This manages the Lua session, registering our custom libraries,
///     and keeps track of the running script.\n
///     Also prevents more than one script from running at a time.
class LuaInterpreter
    {
  public:
    /// @brief Constructor.
    LuaInterpreter();
    /// @private
    ~LuaInterpreter();
    /// @private
    LuaInterpreter(const LuaInterpreter&) = delete;
    /// @private
    LuaInterpreter& operator=(const LuaInterpreter&) = delete;
    /** @brief Runs a Lua script.
        @param filePath The script's file path. Code will be loaded from this file.*/
    void RunLuaFile(const wxString& filePath);
    /** @brief Runs a block of Lua code.
        @param code The code to run.
        @param filePath The script's file path. This is only used for any calls to GetScriptPath()
                        from the script, it will not affect the code being run.
                        It can be left empty, that will just cause any calls to
                        GetScriptPath() to return empty as well.
        @param[out] errorMessage If an error is encountered,
            the message reported by the interpreter.*/
    void RunLuaCode(const wxString& code, const wxString& filePath, wxString& errorMessage);

    /// @returns @c true if another block of Lua code is already running.
    [[nodiscard]]
    static bool IsRunning() noexcept
        {
        return m_isRunning;
        }

    /// @brief Stop running the current script.
    static void Quit() { m_quitRequested = true; }

    /// @returns The file path of the currently running script
    ///     (might be empty if RunLuaCode() was called with no defined file path).
    [[nodiscard]]
    const wxString& GetScriptFilePath() const noexcept
        {
        return m_scriptFilePath;
        }

    /// @brief Sets the path of the currently running script.
    /// @param path The filepath of the currently running script.
    void SetScriptFilePath(const wxString& path) { m_scriptFilePath = path; }

  private:
    static void LineHookCallback(lua_State* L, lua_Debug* ar);

    lua_State* m_L{ nullptr };
    static bool m_isRunning;
    static bool m_quitRequested;
    wxString m_scriptFilePath;
    };

    // NOLINTEND(readability-implicit-bool-conversion)
    // NOLINTEND(readability-identifier-length)

#endif // LUAINTERFACE_H
