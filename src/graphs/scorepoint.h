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

#ifndef SCORE_POINTS_H
#define SCORE_POINTS_H

#include "../Wisteria-Dataviz/src/base/graphitems.h"
#include "../Wisteria-Dataviz/src/base/icons.h"
#include <bitset>

namespace Wisteria
    {
    /// @brief A test's score, plotted as a point on a graph.
    class ScorePoint
        {
      public:
        /// @brief Constructor.
        ScorePoint() = default;

        /** @brief Constructor.
            @param wordStatistic The word statistic (e.g., number of hard words).
            @param sentenceStatistic The sentence statistic (e.g., number of sentences).*/
        ScorePoint(const double wordStatistic, const double sentenceStatistic)
            : m_wordStatistic(wordStatistic), m_sentenceStatistic(sentenceStatistic),
              m_score1(std::numeric_limits<size_t>::quiet_NaN()),
              m_score2(std::numeric_limits<size_t>::quiet_NaN())
            {
            }

        wxPoint m_scorePoint;            /// The axes position.
        double m_wordStatistic{ 0 };     /// The word count.
        double m_sentenceStatistic{ 0 }; /// The sentence count.

        /// @brief Resets the scoring info, but preserves the underlying word,
        ///     sentence, and label information used for the scoring.
        void ResetStatus() noexcept
            {
            m_scoreInfo.reset();
            m_score1 = m_score2 = std::numeric_limits<size_t>::quiet_NaN();
            }

        /// @brief Sets the score.
        /// @param score The value of the score.
        void SetScore(const size_t score) noexcept
            {
            m_score1 = score;
            m_score2 = score;
            }

        /// @returns The score's value.
        [[nodiscard]]
        size_t GetScore() const noexcept
            {
            wxASSERT_MSG(m_score1 == m_score2,
                         L"GetScore() called, but there are two different scores for this point! "
                         "Did you mean to call GetScoreRange() or GetScoreAverage()?");
            return m_score1;
            }

        /** @brief Sets the score's range.
            @param startScore The start of the score's range.
            @param endScore The end of the score's range.*/
        void SetScoreRange(const size_t startScore, const size_t endScore) noexcept
            {
            m_score1 = startScore;
            m_score2 = endScore;
            }

        /// @returns The range of scores that the results falls within for the point.\n
        ///     This is useful for tests that score something within a range of values
        ///     (rather than a single value).
        [[nodiscard]]
        std::pair<size_t, size_t> GetScoreRange() const noexcept
            {
            return std::make_pair(m_score1, m_score2);
            }

        /// @returns The middle of the score range (or simply the score if there is only one)
        ///     for the score.
        [[nodiscard]]
        double GetScoreAverage() const noexcept
            {
            return safe_divide<double>(static_cast<double>(m_score1) + m_score2, 2);
            }

        /// @brief Sets the score invalid flag.
        /// @param enable @c true to mark the score as invalid.
        void SetScoreInvalid(const bool enable) { m_scoreInfo.set(0, enable); }

        /// @returns Whether score is invalid.
        [[nodiscard]]
        bool IsScoreInvalid() const noexcept
            {
            return m_scoreInfo[0];
            }

        /// @brief Sets whether the score is out of grade range.
        /// @param enable True to mark it as out-of-range.
        void SetScoreOutOfGradeRange(const bool enable) { m_scoreInfo.set(1, enable); }

        /// @returns Whether score is out of any valid grade range.
        [[nodiscard]]
        bool IsScoreOutOfGradeRange() const noexcept
            {
            return m_scoreInfo[1];
            }

        /// @brief Sets whether the sentences are hard.
        /// @param enable True to mark the sentences as hard.
        void SetSentencesHard(const bool enable) { m_scoreInfo.set(2, enable); }

        /// @returns Whether sentences are hard.
        [[nodiscard]]
        bool IsSentencesHard() const noexcept
            {
            return m_scoreInfo[2];
            }

        /// @brief Sets whether the words are hard.
        /// @param enable True to mark the words as hard.
        void SetWordsHard(const bool enable) { m_scoreInfo.set(3, enable); }

        /// @returns Whether the words are hard.
        [[nodiscard]]
        bool IsWordsHard() const noexcept
            {
            return m_scoreInfo[3];
            }

      private:
        // invalid score, score out of grade range, etc...
        std::bitset<4> m_scoreInfo{ 0 };
        size_t m_score1{ std::numeric_limits<size_t>::quiet_NaN() };
        size_t m_score2{ std::numeric_limits<size_t>::quiet_NaN() };
        };
    } // namespace Wisteria

#endif // SCORE_POINTS_H
