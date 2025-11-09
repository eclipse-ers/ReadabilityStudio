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

#ifndef READABILITY_PROJECT_TEST_H
#define READABILITY_PROJECT_TEST_H

#include "readability_test.h"
#include <bitset>
#include <utility>
#include <vector>

namespace readability
    {
    /// @brief Wrapper around a readability_test, which adds features needed for projects,
    ///     such as score data and an inclusion flag.
    /// @details This also provides a thin wrapper around readability_test objects,
    ///     so that numerous copies of a test can point to a constant readability_test;
    ///     this way the same strings inside of readability_test objects don't get
    ///     copied thousand of times.
    template<typename datasetT>
    class readability_project_test
        {
      public:
        /// @private
        readability_project_test(const readability_project_test& that) noexcept
            : m_test(that.m_test), m_included(that.m_included)
            {
            }

        /// @brief CTOR from regular test definition.
        explicit readability_project_test(const readability_test& test) noexcept : m_test(&test) {}

        /// @private
        readability_project_test() = delete;

        /// @private
        virtual ~readability_project_test() = default;

        /// @private
        void operator=(const readability_project_test& that) noexcept
            {
            m_test = that.m_test;
            m_included = that.m_included;
            }

        /// @private
        [[nodiscard]]
        bool operator<(const readability_test& that) const noexcept
            {
            return (get_test() < that);
            }

        /// @private
        [[nodiscard]]
        bool operator<(const readability_project_test& that) const noexcept
            {
            return (get_test() < that.get_test());
            }

        /// @brief Compares (case insensitively) a string value to the test's ID,
        ///     short name, and long name.
        [[nodiscard]]
        bool operator==(const readability_test& that) const noexcept
            {
            return (get_test() == that);
            }

        /// @private
        [[nodiscard]]
        bool operator==(const readability_project_test& that) const noexcept
            {
            return (get_test() == that.get_test());
            }

        /// @returns The underlying test description.
        [[nodiscard]]
        const readability_test& get_test() const noexcept
            {
            assert(m_test && "test reference not initialized in project test.");
            return *m_test;
            }

        /// @returns grade score points (for multi-document tests).
        [[nodiscard]]
        std::shared_ptr<datasetT> get_grade_point_collection()
            {
            if (m_grade_values == nullptr)
                {
                m_grade_values = std::make_shared<datasetT>();
                }
            return m_grade_values;
            }

        /// @returns index score points (for multi-document tests).
        [[nodiscard]]
        std::shared_ptr<datasetT> get_index_point_collection()
            {
            if (m_index_values == nullptr)
                {
                m_index_values = std::make_shared<datasetT>();
                }
            return m_index_values;
            }

        /// @returns cloze score points (for multi-document tests).
        [[nodiscard]]
        std::shared_ptr<datasetT> get_cloze_point_collection()
            {
            if (m_cloze_scores == nullptr)
                {
                m_cloze_scores = std::make_shared<datasetT>();
                }
            return m_cloze_scores;
            }

        /// @returns Whether the test is included in the project.
        [[nodiscard]]
        bool is_included() const noexcept
            {
            return m_included;
            }

        /// Whether to include the test in the project.
        /// @param inc True to include the test, false to remove.
        void include(const bool inc) noexcept { m_included = inc; }

        /// @brief Gives direct access to the inclusion flag.
        /// @details Used when an external source needs to edit the
        ///     inclusion status, but can't call include().
        [[nodiscard]]
        bool& get_include_flag() noexcept
            {
            return m_included;
            }

      private:
        const readability_test* m_test{ nullptr };
        // whether the test is included in the project
        bool m_included{ false };
        // batch projects use these for its multi-doc graphs
        std::shared_ptr<datasetT> m_grade_values{ nullptr };
        std::shared_ptr<datasetT> m_index_values{ nullptr };
        std::shared_ptr<datasetT> m_cloze_scores{ nullptr };
        };
    } // namespace readability

#endif // READABILITY_PROJECT_TEST_H
