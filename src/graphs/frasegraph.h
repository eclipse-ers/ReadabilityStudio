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

#ifndef FRASE_GRAPH_H
#define FRASE_GRAPH_H

#include "polygon_readability_graph.h"

namespace Wisteria::Graphs
    {
    /** @brief Readability graph for Spanish (as a second language).

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
            Vari-Cartier, Patricia. “Development and Validation of a New Instrument to
            Assess the Readability of Spanish Prose.” *The Modern Language Journal*,
            vol. 65, no. 2, 1981, pp. 141-48.*/
    class FraseGraph final : public PolygonReadabilityGraph
        {
        wxDECLARE_DYNAMIC_CLASS(FraseGraph);
        FraseGraph() = default;

      public:
        /** @brief Constructor.
            @param canvas The parent canvas to render on.
            @param colors The color scheme to apply to the points.
               Leave as null to use the default theme.
            @param shapes The shape scheme to use for the points.
               Leave as null to use the standard shapes.*/
        explicit FraseGraph(
            Wisteria::Canvas* canvas,
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

      private:
        void RecalcSizes(wxDC& dc) final;
        void CalculateScorePositions(wxDC& dc) final;
        void BuildBackscreen();

        std::array<wxPoint, 11> m_levelLinePoints;
        std::array<wxPoint, 8> m_dividerLinePoints;

        wxString m_numberOfWordsColumn;
        wxString m_numberOfSyllablesColumn;
        wxString m_numberOfSentencesColumn;
        std::vector<Wisteria::ScorePoint> m_results;

        // used for consistent polygon calculations
        std::unique_ptr<FraseGraph> m_backscreen{ nullptr };
        };
    } // namespace Wisteria::Graphs

#endif // FRASE_GRAPH_H
