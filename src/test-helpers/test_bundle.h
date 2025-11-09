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

#ifndef TEST_BUNDLE_H
#define TEST_BUNDLE_H

#include "../readability/readability_test.h"
#include <bitset>
#include <limits>
#include <set>
#include <string>

/// @brief A test (and its optional goals) included in a bundle.
class Goal
    {
  public:
    /// @brief String type for test names.
    using string_type = traits::case_insensitive_wstring_ex;

    /// @brief Constructor.
    /// @param name The name of the goal (test name or statistic name).
    explicit Goal(string_type name) : m_name(std::move(name)) { m_passFails.set(); }

    /// @brief Constructor.
    /// @param name The name of the goal (test name or statistic name).
    /// @param minGoal The minimum value of the goal.
    /// @param maxGoal The maximum value of the goal.
    /// @note Set either the min or max goal to NaN to not use them
    Goal(string_type name, const double minGoal, const double maxGoal)
        : m_name(std::move(name)), m_minGoal(minGoal), m_maxGoal(maxGoal)
        {
        m_passFails.set();
        }

    /// @brief Compares goals by name.
    /// @param that The goal to compare against.
    /// @returns @c true if this goal's name is less than the other one.
    [[nodiscard]]
    bool operator<(const Goal& that) const noexcept
        {
        return GetName() < that.GetName();
        }

    /// @returns The test/statistic's name.
    [[nodiscard]]
    const string_type& GetName() const noexcept
        {
        return m_name;
        }

    /// @returns The minimum score that a test/statistic should have.
    /// @note Normally, you would be more interested in setting a max score, rather than this.
    [[nodiscard]]
    double GetMinGoal() const noexcept
        {
        return m_minGoal;
        }

    /// @returns The maximum score that a test/statistic should have.
    [[nodiscard]]
    double GetMaxGoal() const noexcept
        {
        return m_maxGoal;
        }

    /// @returns The flags used to indicate if a test/statistic is within its constraints.
    [[nodiscard]]
    const std::bitset<32>& GetPassFailFlags() const noexcept
        {
        return m_passFails;
        }

    /// @returns The flags used to indicate if a test/statistic is within its constraints.
    [[nodiscard]]
    std::bitset<32>& GetPassFailFlags() noexcept
        {
        return m_passFails;
        }

    /// @returns @c true if there is at least one goal enabled for this test/statistic.
    [[nodiscard]]
    bool HasGoals() const noexcept
        {
        return (!std::isnan(GetMinGoal()) || !std::isnan(GetMaxGoal()));
        }

  private:
    string_type m_name;
    double m_minGoal{ std::numeric_limits<double>::quiet_NaN() };
    double m_maxGoal{ std::numeric_limits<double>::quiet_NaN() };
    std::bitset<32> m_passFails{ 0 };
    };

/// @brief A test goal (really, just typedef of Goal).
using TestGoal = Goal;
/// @brief A statistic goal (really, just typedef of Goal).
using StatGoal = Goal;

/// @brief A collection of tests (and optionally, their recommended goals).
class TestBundle
    {
  public:
    /// @brief String type for test names.
    using string_type = traits::case_insensitive_wstring_ex;

    /// @brief Constructor.
    /// @param name The bundle's name.
    explicit TestBundle(string_type name) : m_name(std::move(name)) {}

    /// @private
    TestBundle() = default;

    /// @brief Compares bundles by name.
    /// @param that The bundle to compare against.
    /// @returns @c true if this bundle's name is less than the other one.
    [[nodiscard]]
    bool operator<(const TestBundle& that) const noexcept
        {
        return GetName() < that.GetName();
        }

    /// @returns The bundle's name.
    [[nodiscard]]
    const string_type& GetName() const noexcept
        {
        return m_name;
        }

    void SetName(const string_type& name) { m_name = name; }

    /// @returns The bundle's description.
    [[nodiscard]]
    const string_type& GetDescription() const noexcept
        {
        return m_description;
        }

    /// @brief Sets the bundle's description.
    /// @param desc The description to use.
    void SetDescription(const string_type& desc) { m_description = desc; }

    /// @returns The bundle's language.
    [[nodiscard]]
    readability::test_language GetLanguage() const noexcept
        {
        return m_language;
        }

    /// @brief Sets the bundle's language.
    /// @param lang The language to use.
    void SetLanguage(const readability::test_language lang) noexcept { m_language = lang; }

    /// @returns The bundle's test goals
    [[nodiscard]]
    std::set<TestGoal>& GetTestGoals() noexcept
        {
        return m_includedTests;
        }

    /// @private
    [[nodiscard]]
    const std::set<TestGoal>& GetTestGoals() const noexcept
        {
        return m_includedTests;
        }

    /// @returns The bundle's statistic goals.
    [[nodiscard]]
    std::set<StatGoal>& GetStatGoals() noexcept
        {
        return m_includedStatGoals;
        }

    /// @private
    [[nodiscard]]
    const std::set<StatGoal>& GetStatGoals() const noexcept
        {
        return m_includedStatGoals;
        }

    /// @brief Locks or unlocks the bundle.
    /// @details A locked bundle cannot be edited by the user through the interface.\n
    ///     This is useful for creating system bundles based on published articles
    ///     that should not be changed.
    void Lock(const bool lock = true) noexcept { m_locked = lock; }

    /// @returns Whether the bundle can be edited by the user.
    [[nodiscard]]
    bool IsLocked() const noexcept
        {
        return m_locked;
        }

  private:
    string_type m_name;
    string_type m_description;
    std::set<TestGoal> m_includedTests;
    std::set<StatGoal> m_includedStatGoals;
    bool m_locked{ false };
    readability::test_language m_language{ readability::test_language::unknown_language };
    };

#endif // TEST_BUNDLE_H
