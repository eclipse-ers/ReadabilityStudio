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

#ifndef RAYGOR_GRAPH_H
#define RAYGOR_GRAPH_H

#include "polygon_readability_graph.h"

namespace Wisteria::Graphs
    {
    enum class RaygorStyle
        {
        /// @brief Sans serif font for graph and region labels, abbreviated labels,
        ///     no polygons for the too-difficult regions,
        ///     and place the grade levels closer together.
        Original,
        /// @brief Same as Original, except includes too-difficult regions' polygons.
        BaldwinKaufman,
        /// @brief Same as BaldwinKaufman, except uses the default font for graph and region labels,
        ///     says "College" and "Professor", adds "+" and "-" to the axes,
        ///     and places grade labels further apart. This makes the
        ///     graph look closer to a Fry graph.
        Modern,
        /// @private
        RAYGOR_STYLE_COUNT
        };

    /// @brief Raygor readability graph.
    class RaygorGraph final : public PolygonReadabilityGraph
        {
        wxDECLARE_DYNAMIC_CLASS(RaygorGraph);
        RaygorGraph() = default;

      public:
        /** @brief Constructor.
            @param canvas The parent canvas to render on.
            @param colors The color scheme to apply to the points.
               Leave as null to use the default theme.
            @param shapes The shape scheme to use for the points.
               Leave as null to use the standard shapes.*/
        explicit RaygorGraph(
            Wisteria::Canvas* canvas,
            const std::shared_ptr<Wisteria::Colors::Schemes::ColorScheme>& colors = nullptr,
            const std::shared_ptr<Wisteria::Icons::Schemes::IconScheme>& shapes = nullptr);

        /** @brief Sets the data.
            @param data The data to use.
            @param numberOfWordsColumnName The column containing the number of words
                (excluding numerals) in the sample (a continuous column).
            @param numberOf6PlusCharWordsColumnName The column containing the number of 6+
                character words (excluding numerals) in the sample (a continuous column).
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
                     const wxString& numberOf6PlusCharWordsColumnName,
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

        /// @returns The style (appearance) of the graph.
        [[nodiscard]]
        RaygorStyle GetRaygorStyle() const noexcept
            {
            return m_raygorStyle;
            }

        /** @brief Sets the style for the graph.
            @param style The appearance to use for the graph.*/
        void SetRaygorStyle(const RaygorStyle style);

      private:
        void RecalcSizes(wxDC& dc) final;
        void CalculateScorePositions(wxDC& dc) final;
        void BuildBackscreen();

        std::array<wxPoint, 27> m_gradeLinePoints;
        std::array<wxPoint, 8> m_longSentencesPoints;
        std::array<wxPoint, 10> m_longWordPoints;
        std::array<wxPoint, 10> m_dividerLinePoints;

        wxString m_numberOfWordsColumn;
        wxString m_numberOf6PlusCharWordsColumn;
        wxString m_numberOfSentencesColumn;
        std::vector<Wisteria::ScorePoint> m_results;

        RaygorStyle m_raygorStyle{ RaygorStyle::BaldwinKaufman };

        // used for consistent polygon calculations
        std::unique_ptr<RaygorGraph> m_backscreen{ nullptr };
        };
    } // namespace Wisteria::Graphs

#endif // RAYGOR_GRAPH_H
