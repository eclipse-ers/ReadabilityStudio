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

#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H

#include "luna.h"
#include <functional>
#include <set>
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
    LuaInterpreter() = default;
    /// @brief Initializes the Lua state and registers custom libraries.
    /// @note Must be called before running any Lua code.
    void Initialize();
    /// @brief Closes and reinitializes the Lua state, discarding all global
    ///     variables and other session state from any prior runs.
    /// @note Has no effect while a script is currently running.
    void Restart();
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

    /// @brief Sets which (1-based) script lines should pause execution when hit.
    /// @param lines The line numbers to break on.
    void SetBreakpointLines(std::set<int> lines) { m_breakpointLines = std::move(lines); }

    /// @brief Registers a callback invoked when the pause state changes.
    /// @details Called with the 1-based line number execution paused at,
    ///     or @c -1 when execution resumes or the run ends/errors/is stopped.
    /// @param callback The callback to invoke.
    void SetPauseStateChangedCallback(std::function<void(int)> callback)
        {
        m_pauseStateChangedCallback = std::move(callback);
        }

    /// @returns @c true if execution is currently paused at a breakpoint.
    [[nodiscard]]
    static bool IsPaused() noexcept
        {
        return m_isPaused;
        }

    /// @returns The 1-based line currently paused at, or @c -1 if not paused.
    [[nodiscard]]
    static int GetPausedLine() noexcept
        {
        return m_pausedLine;
        }

    /// @brief Resumes a paused script from exactly where it stopped.
    static void ContinueExecution() noexcept { m_continueRequested = true; }

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
    /** @brief Strips the Lua error header from @c errorMessage in place.
        @param[in,out] errorMessage The raw error string from the Lua interpreter.
        @param[out] lineNumber The script line where the stop was requested.
            Only meaningful when the function returns @c true.
        @returns @c true if it was an actual script error;
            @c false if the error was a user-requested stop (via @c Quit()).*/
    static bool ParseLuaError(wxString& errorMessage, long& lineNumber);

    // fixed chunk name for workbench-run code (the leading '=' tells Lua to use
    // the rest verbatim as the chunk's source name, with no added formatting).
    // This lets the debug hook tell the running script apart from any nested
    // chunks it loads via dofile()/require(), which get their own distinct
    // source names -- without it, a breakpoint's line number could coincidentally
    // match a line inside a dofile()'d file and pause there instead.
    static constexpr const char* WORKBENCH_CHUNK_NAME = "=workbench";

    lua_State* m_L{ nullptr };
    static bool m_isRunning;
    static bool m_quitRequested;
    static bool m_isPaused;
    static bool m_continueRequested;
    static int m_pausedLine;
    static std::set<int> m_breakpointLines;
    static std::function<void(int)> m_pauseStateChangedCallback;
    wxString m_scriptFilePath;
    };

    // NOLINTEND(readability-implicit-bool-conversion)
    // NOLINTEND(readability-identifier-length)

#endif // LUAINTERFACE_H
