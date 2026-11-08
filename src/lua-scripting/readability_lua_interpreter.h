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

#ifndef READABILITY_LUA_INTERPRETER_H
#define READABILITY_LUA_INTERPRETER_H

#include "../Wisteria-Dataviz/src/lua-scripting/lua_interface.h"

// NOLINTBEGIN(readability-identifier-length)

/// @brief Connects the Lua interpreter to Readability Studio's API and options.
class ReadabilityLuaInterpreter final : public Wisteria::UI::LuaInterpreter
    {
  protected:
    /// @private
    void RegisterLibraries(lua_State* L) override;
    /// @private
    [[nodiscard]]
    bool IsUnsafeModeEnabled() const override;
    /// @private
    [[nodiscard]]
    wxString GetUnsafeModeHint() const override;
    /// @private
    void OnLuaCodeFinished() override;
    /// @private
    void DescribeUserData(LuaVariableInfo& info, int idx) const override;
    };

    // NOLINTEND(readability-identifier-length)

#endif // READABILITY_LUA_INTERPRETER_H
