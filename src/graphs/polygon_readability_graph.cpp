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

#include "polygon_readability_graph.h"

wxIMPLEMENT_DYNAMIC_CLASS(Wisteria::Graphs::PolygonReadabilityGraph, Wisteria::Graphs::GroupGraph2D)

    namespace Wisteria::Graphs
    {
    //----------------------------------------------------------------
    wxString PolygonReadabilityGraph::FormatScoreAsGradeLevel(const Wisteria::ScorePoint& score)
        const
        {
        const auto [startGrade, endGrade] = score.GetScoreRange();
        if (GetMessageCatalog() == nullptr)
            {
            return (startGrade == endGrade) ? wxString::Format(L"%zu", startGrade) :
                                              wxString::Format(L"%zu-%zu", startGrade, endGrade);
            }
        return (startGrade == endGrade) ?
                   GetMessageCatalog()->GetGradeScaleLongLabel(startGrade) :
                   wxString::Format(L"%s-%s",
                                    GetMessageCatalog()->GetGradeScaleLongLabel(startGrade),
                                    GetMessageCatalog()->GetGradeScaleLongLabel(endGrade));
        }

    //----------------------------------------------------------------
    wxString PolygonReadabilityGraph::FormatScoresForAccessibility(
        const std::vector<Wisteria::ScorePoint>& scores,
        const std::function<wxString(const Wisteria::ScorePoint&)>& scoreFormatter)
        {
        std::vector<Wisteria::ScorePoint> plottedScores;
        plottedScores.reserve(scores.size());
        for (const auto& score : scores)
            {
            if (!score.IsScoreInvalid() && !score.IsScoreOutOfGradeRange())
                {
                plottedScores.push_back(score);
                }
            }
        const size_t unplottedCount{ scores.size() - plottedScores.size() };

        wxString description;
        if (plottedScores.size() == 1)
            {
            /* TRANSLATORS: Readability graph accessibility: a single score.
               %s is the score's reading level. */
            description = wxString::Format(_(L"Score: %s"), scoreFormatter(plottedScores.front()));
            // note what is influencing the text's difficulty
            if (plottedScores.front().IsWordsHard())
                {
                description += _(L", with the difficulty coming from longer words");
                }
            else if (plottedScores.front().IsSentencesHard())
                {
                description += _(L", with the difficulty coming from longer sentences");
                }
            }
        else if (plottedScores.size() > 1)
            {
            const auto [lowestScore, highestScore] = std::minmax_element(
                plottedScores.cbegin(), plottedScores.cend(),
                [](const Wisteria::ScorePoint& lhv, const Wisteria::ScorePoint& rhv) noexcept
                { return lhv.GetScoreAverage() < rhv.GetScoreAverage(); });
            /* TRANSLATORS: Readability graph accessibility: multiple scores.
               %zu is the score count, 1st %s is the lowest reading level,
               2nd %s is the highest reading level. */
            description =
                wxString::Format(_(L"%zu scores, ranging from %s to %s"), plottedScores.size(),
                                 scoreFormatter(*lowestScore), scoreFormatter(*highestScore));
            }

        if (unplottedCount > 0)
            {
            if (!description.empty())
                {
                description += L"; ";
                }
            description +=
                (unplottedCount == 1) ?
                    _(L"1 score could not be plotted (the text is too difficult to be graphed)") :
                       /* TRANSLATORS: Readability graph accessibility: unplottable scores.
                          %zu is the number of scores that fell outside of the graph. */
                       wxString::Format(_(L"%zu scores could not be plotted "
                                          "(the text is too difficult to be graphed)"),
                                        unplottedCount);
            }

        return description;
        }
    } // namespace Wisteria::Graphs
