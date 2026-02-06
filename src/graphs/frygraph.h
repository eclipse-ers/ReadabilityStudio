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

#ifndef FRY_GRAPH_H
#define FRY_GRAPH_H

#include "polygon_readability_graph.h"

namespace Wisteria::Graphs
    {
    /** @brief Fry readability graph.

        @par %Data:
         This plot accepts a Data::Dataset where there are continuous columns for
         the word, syllable, and sentences counts for a sample.
         The ID column's labels will be associated with each point,
         so it is recommended to fill this column with the documents' (or samples') names.

         A categorical column can also optionally be used as a grouping variable.

        @par Missing Data:
         - Scores that are missing data will not be plotted.
         - Blank IDs will apply blank selection labels to their respective points.
         - Blank group labels will be lumped into a "[NO GROUP]" category.

        @par Citation:
            Fry, Edward.
            “A Readability Formula That Saves Time.” *Journal of Reading*, vol. 11, no. 7, 1968,
            pp. 513-16, 575-78.

            Gilliam, Bettye, Sylvia C. Peña, and Lee Mountain.
            “The Fry Graph Applied to Spanish Readability.”
            *Reading Teacher*, vol. 33, 1980, pp. 426-30.*/
    class FryGraph final : public PolygonReadabilityGraph
        {
        wxDECLARE_DYNAMIC_CLASS(FryGraph);
        FryGraph() = default;

      public:
        /// @brief Which type of Fry graph.
        enum class FryGraphType
            {
            /// @brief English Fry graph (the original).
            Traditional,
            /// @brief Gilliam-Peña-Mountain variation (for Spanish text).
            GPM
            };
        /** @brief Constructor.
            @param fType Which type of Fry graph to use.
            @param canvas The parent canvas to render on.
            @param colors The color scheme to apply to the points.
               Leave as null to use the default theme.
            @param shapes The shape scheme to use for the points.
               Leave as null to use the standard shapes.*/
        FryGraph(Wisteria::Canvas* canvas, const FryGraphType fType,
                 const std::shared_ptr<Wisteria::Colors::Schemes::ColorScheme>& colors = nullptr,
                 const std::shared_ptr<Wisteria::Icons::Schemes::IconScheme>& shapes = nullptr);

        /** @brief Sets the data.
            @param data The data to use.
            @param numberOfWordsColumnName The column containing the number of words
                in the sample (a continuous column).
            @param numberOfSyllablesColumnName The column containing the number of syllables
                in the sample (a continuous column).
            @param numberOfSentencesColumnName The column containing the number of sentences
                in the sample (a continuous column).
            @param groupColumnName The (optional) categorical column to use for grouping.
            @note Pass in the raw numbers of the sample, this function will normalize it
                to a 100-word sample for you.
            @throws std::runtime_error If any columns can't be found, throws an exception.\n
                The exception's @c what() message is UTF-8 encoded, so pass it to
                @c wxString::FromUTF8() when formatting it for an error message.*/
        void SetData(const std::shared_ptr<Wisteria::Data::Dataset>& data,
                     const wxString& numberOfWordsColumnName,
                     const wxString& numberOfSyllablesColumnName,
                     const wxString& numberOfSentencesColumnName,
                     const std::optional<wxString>& groupColumnName = std::nullopt);

        /// @returns The results calculated from the last call to SetData().
        /// @note These scores will be aligned with the order of the data from
        ///     when it was initially passed to SetData().
        [[nodiscard]]
        const std::vector<Wisteria::ScorePoint>& GetScores() const noexcept
            {
            return m_results;
            }

      protected:
        void RecalcSizes(wxDC& dc) final;
        void CalculateScorePositions(wxDC& dc) final;
        /// @brief Adds grade level labels to the graph regions.
        void AddLevelLabels();
        /// @brief Adds bracket decorations around the graph title.
        void AddBrackets();

        /// @returns The syllable axis offset used for language variations.
        [[nodiscard]]
        uint16_t GetSyllableAxisOffset() const noexcept
            {
            return m_syllableAxisOffset;
            }

      private:
        void BuildBackscreen();
        /// @brief Set as a Spanish variation of the Fry graph.
        void SetAsGilliamPenaMountainGraph();
        FryGraphType m_fryGraphType{ FryGraphType::Traditional };
        // used for adjusting the axis for other languages
        uint16_t m_syllableAxisOffset{ 0 };

        std::array<wxPoint, 37> m_gradeLinePoints;
        std::array<wxPoint, 3> m_longSentencesPoints;
        std::array<wxPoint, 9> m_longWordPoints;
        std::array<wxPoint, 12> m_dividerLinePoints;

        wxString m_numberOfWordsColumn;
        wxString m_numberOfSyllablesColumn;
        wxString m_numberOfSentencesColumn;
        std::vector<Wisteria::ScorePoint> m_results;

        // used for consistent polygon calculations
        std::unique_ptr<FryGraph> m_backscreen{ nullptr };
        };
    } // namespace Wisteria::Graphs

#endif // FRY_GRAPH_H
