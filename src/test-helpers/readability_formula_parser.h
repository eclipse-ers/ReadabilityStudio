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

#ifndef READABILITY_FORMULA_PARSER_H
#define READABILITY_FORMULA_PARSER_H

#include "../tinyexpr-plusplus/tinyexpr.h"
#include "wx/string.h"

/// @private
class BaseProject;

/// @brief Connects a formula parser and a project.
class FormulaProject : public te_expr
    {
  public:
    /// @brief Constructor.
    /// @param project The project to connect the formula parser to.
    explicit FormulaProject(const BaseProject* project) noexcept : m_project(project) {}

    /// @returns The target project.
    [[nodiscard]]
    const BaseProject* GetProject() const noexcept
        {
        return m_project;
        }

  private:
    const BaseProject* m_project{ nullptr };
    };

/// @brief Readability formula parser.
class ReadabilityFormulaParser : public te_parser
    {
  public:
    ReadabilityFormulaParser(const BaseProject* project, const wchar_t decimalSeparator,
                             const wchar_t listSeparator);
    /// @private
    ReadabilityFormulaParser() = delete;
    /// @private
    ReadabilityFormulaParser(const ReadabilityFormulaParser& that) = delete;
    /// @private
    ReadabilityFormulaParser& operator=(const ReadabilityFormulaParser& that) = delete;
    /// @brief Refreshes the project's statistics that are mapped to variables
    ///     in the formula parser.
    void UpdateVariables();

    /// @brief Parses a signature to find the function name in it.
    /// @param signature The signature to parse.
    /// @returns The name of the function from the signature.
    [[nodiscard]]
    static wxString SignatureToFunctionName(const wxString& signature)
        {
        const auto paren = signature.find(L'(');
        return ((paren == wxString::npos) ? signature : signature.substr(0, paren));
        }

    /// @returns The signature of the custom DC test.
    [[nodiscard]]
    static wxString GetCustomNewDaleChallSignature()
        {
        return L"CustomNewDaleChall()";
        }

    /// @returns The signature of the custom Spache test.
    [[nodiscard]]
    static wxString GetCustomSpacheSignature()
        {
        return L"CustomSpache()";
        }

    /// @returns The signature of the custom HJ test.
    [[nodiscard]]
    static wxString GetCustomHarrisJacobsonSignature()
        {
        return L"CustomHarrisJacobson()";
        }

  private:
    FormulaProject m_formulaProject;
    };

#endif // READABILITY_FORMULA_PARSER_H
