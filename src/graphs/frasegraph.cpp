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

#include "frasegraph.h"

wxIMPLEMENT_DYNAMIC_CLASS(Wisteria::Graphs::FraseGraph, Wisteria::Graphs::PolygonReadabilityGraph)

    namespace Wisteria::Graphs
    {
    //----------------------------------------------------------------
    FraseGraph::FraseGraph(
        Wisteria::Canvas * canvas,
        const std::shared_ptr<Colors::Schemes::ColorScheme>& colors /*= nullptr*/,
        const std::shared_ptr<Icons::Schemes::IconScheme>& shapes /*= nullptr*/)
        : PolygonReadabilityGraph(canvas)
        {
        SetColorScheme(colors != nullptr ? colors :
                                           std::make_shared<Colors::Schemes::ColorScheme>(
                                               Settings::GetDefaultColorScheme()));
        SetShapeScheme(shapes != nullptr ? shapes :
                                           std::make_shared<Icons::Schemes::IconScheme>(
                                               Icons::Schemes::StandardShapes()));

        GetCanvas()->SetLabel(_(L"FRASE Graph"));
        GetCanvas()->SetName(_(L"FRASE Graph"));
        GetLeftYAxis().GetTitle().SetText(_(L"Average number of sentences per 100 words"));
        GetBottomXAxis().GetTitle().SetText(_(L"Average number of syllables per 100 words"));
        GetTitle() = GraphItems::Label(
            GraphItems::GraphItemInfo(
                _(L"FRASE (Fry Readability Adaptation for Spanish Evaluation) Graph"))
                .Pen(wxNullPen));

        // set up the x-axis
        GetBottomXAxis().SetRange(182, 234, 0, 1, 1);
        GetBottomXAxis().SetDisplayInterval(4, 2);
        GetBottomXAxis().GetGridlinePen() = wxNullPen;
        GetBottomXAxis().SetTickMarkDisplay(GraphItems::Axis::TickMark::DisplayType::Inner);
        GetBottomXAxis().SetTickMarkInterval(2);
        GetBottomXAxis().SetMajorTickMarkLength(10);
        GetBottomXAxis().SetMinorTickMarkLength(10);

        // set up the y-axis
        GetLeftYAxis().SetRange(0, 15, 0, 1, 1);
        GetLeftYAxis().ShowOuterLabels(false);
        GetLeftYAxis().GetGridlinePen() = wxNullPen;
        GetLeftYAxis().SetTickMarkDisplay(GraphItems::Axis::TickMark::DisplayType::Inner);
        GetLeftYAxis().SetTickMarkInterval(1);
        GetLeftYAxis().SetMajorTickMarkLength(10);
        GetLeftYAxis().SetMinorTickMarkLength(10);

        // add the labels for the grades
        AddLevelLabel(LevelLabel(204, 13, _(L"BEGINNING\nLEVEL\nI"), 1, 1));
        AddLevelLabel(LevelLabel(215.5, 10, _(L"INTERMEDIATE\nLEVEL\nII"), 2, 2));
        AddLevelLabel(LevelLabel(223.5, 8,
                                 // TRANSLATORS: Uppercasing is just for display
                                 _(L"ADVANCED\nINTERMEDIATE\nLEVEL\nIII"), 3, 3));
        AddLevelLabel(LevelLabel(230, 6, _(L"ADVANCED\nLEVEL\nIV"), 4, 4));
        }

    //----------------------------------------------------------------
    void FraseGraph::SetData(const std::shared_ptr<Wisteria::Data::Dataset>& data,
                             const wxString& numberOfWordsColumnName,
                             const wxString& numberOfSyllablesColumnName,
                             const wxString& numberOfSentencesColumnName,
                             const std::optional<wxString>& groupColumnName /*= std::nullopt*/)
        {
        SetDataset(data);
        ResetGrouping();
        m_results.clear();
        GetSelectedIds().clear();

        if (GetDataset() == nullptr)
            {
            return;
            }

        SetGroupColumn(groupColumnName);
        m_numberOfWordsColumn = numberOfWordsColumnName;
        m_numberOfSyllablesColumn = numberOfSyllablesColumnName;
        m_numberOfSentencesColumn = numberOfSentencesColumnName;

        // if grouping, build the list of group IDs, sorted by their respective labels
        if (IsUsingGrouping())
            {
            BuildGroupIdMap();
            }

        BuildBackscreen();
        }

    //----------------------------------------------------------------
    void FraseGraph::BuildBackscreen()
        {
        if (m_backscreen == nullptr)
            {
            // Shape and color schemes are irrelevant, as this is used
            // only for the base polygon calculations.
            m_backscreen = std::make_unique<FraseGraph>(GetCanvas());
            m_backscreen->SetMessageCatalog(GetMessageCatalog());
            // Also, leave the data as null, as that isn't used either for
            // simply creating polygons. All scaling should just be 1.0.
            m_backscreen->SetDataset(nullptr);
            m_backscreen->SetScaling(1.0);
            m_backscreen->SetDPIScaleFactor(1.0);
            wxGCDC dc(GetCanvas());
            // Just use this size as-is, don't adjust it from DIP -> DPI under HiDPI.
            // We are just interested in having a 700x500 size window to do polygon
            // collision within; this isn't actually being presented. This is also why
            // this canvas's DPI scale factor is hard coded to 1.9 (above).
            m_backscreen->SetBoundingBox(
                wxSize(Canvas::GetDefaultCanvasWidthDIPs(), Canvas::GetDefaultCanvasHeightDIPs()),
                dc, 1.0);
            m_backscreen->RecalcSizes(dc);
            }
        }

    //----------------------------------------------------------------
    void FraseGraph::RecalcSizes(wxDC & dc)
        {
        Graph2D::RecalcSizes(dc);

        wxASSERT_MSG((!m_backscreen || m_backscreen->GetBoundingBox(dc).GetWidth() ==
                                           Wisteria::Canvas::GetDefaultCanvasWidthDIPs()),
                     L"Invalid backscreen size!");
        wxASSERT_MSG((!m_backscreen || m_backscreen->GetBoundingBox(dc).GetHeight() ==
                                           Wisteria::Canvas::GetDefaultCanvasHeightDIPs()),
                     L"Invalid backscreen size!");

        const wxColour labelFontColor{ GetLeftYAxis().GetFontColor() };

        // divider line
        GetPhysicalCoordinates(188, 15, m_dividerLinePoints[0]);
        GetPhysicalCoordinates(196, 12, m_dividerLinePoints[1]);
        GetPhysicalCoordinates(204, 9, m_dividerLinePoints[2]);
        GetPhysicalCoordinates(210, 7, m_dividerLinePoints[3]);
        GetPhysicalCoordinates(217, 5, m_dividerLinePoints[4]);
        GetPhysicalCoordinates(224, 3.8, m_dividerLinePoints[5]);
        GetPhysicalCoordinates(230, 3.5, m_dividerLinePoints[6]);
        GetPhysicalCoordinates(234, 3.75, m_dividerLinePoints[7]);

        // I
        GetPhysicalCoordinates(182, 3.1, m_levelLinePoints[0]);
        GetPhysicalCoordinates(182, 15, m_levelLinePoints[1]);
        GetPhysicalCoordinates(224, 15, m_levelLinePoints[2]);
        // II
        GetPhysicalCoordinates(224, 15, m_levelLinePoints[3]);
        GetPhysicalCoordinates(182, 3.1, m_levelLinePoints[4]);
        GetPhysicalCoordinates(182, 0, m_levelLinePoints[5]);
        // III
        GetPhysicalCoordinates(196, 0, m_levelLinePoints[6]);
        GetPhysicalCoordinates(234, 15, m_levelLinePoints[7]);
        // IV
        GetPhysicalCoordinates(234, 10.75, m_levelLinePoints[8]);
        GetPhysicalCoordinates(210.5, 0, m_levelLinePoints[9]);
        GetPhysicalCoordinates(234, 0, m_levelLinePoints[10]);

        const wxBrush selectionBrush = wxBrush(Colors::ColorContrast::ChangeOpacity(
            Colors::ColorBrewer::GetColor(Colors::Color::LightGray), 100));

        // I
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxNullPen)
                .Text(_(L"Beginner Level"))
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            std::span(m_levelLinePoints.cbegin(), std::next(m_levelLinePoints.cbegin(), 3))));
        // II
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxNullPen)
                .Text(_(L"Intermediate Level"))
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            std::span(std::next(m_levelLinePoints.cbegin(), 3), 5)));
        // III
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxNullPen)
                .Text(_(L"Advanced Intermediate Level"))
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            std::span(std::next(m_levelLinePoints.cbegin(), 6), 4)));
        // IV
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxNullPen)
                .Text(_(L"Advanced Level"))
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            std::span(std::next(m_levelLinePoints.cbegin(), 8), 3)));

        // separator line
        wxColour separatorColor{ Colors::ColorContrast::ChangeOpacity(
            Wisteria::Colors::ColorBrewer::GetColor(Wisteria::Colors::Color::Black), 200) };
        separatorColor =
            Colors::ColorContrast::ShadeOrTintIfClose(separatorColor, GetPlotOrCanvasColor());
        auto levelsSpline =
            std::make_unique<GraphItems::Polygon>(GraphItems::GraphItemInfo{}
                                                      .Pen(wxPen{ separatorColor })
                                                      .Brush(separatorColor)
                                                      .Selectable(false)
                                                      .Scaling(GetScaling())
                                                      .DPIScaling(GetDPIScaleFactor()),
                                                  m_dividerLinePoints);
        levelsSpline->SetShape(GraphItems::Polygon::PolygonShape::Spline);
        AddObject(std::move(levelsSpline));

        // draw the region lines
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxPen{ separatorColor })
                .Brush(separatorColor)
                .SelectionBrush(selectionBrush)
                .Scaling(GetScaling()),
            std::span(std::next(m_levelLinePoints.cbegin(), 3), 2)));
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxPen{ separatorColor })
                .Brush(separatorColor)
                .SelectionBrush(selectionBrush)
                .Scaling(GetScaling()),
            std::span(std::next(m_levelLinePoints.cbegin(), 6), 2)));
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxPen{ separatorColor })
                .Brush(separatorColor)
                .SelectionBrush(selectionBrush)
                .Scaling(GetScaling()),
            std::span(std::next(m_levelLinePoints.cbegin(), 8), 2)));

        CalculateScorePositions(dc);

        // draw the quadrant labels
        // (and highlight in heavy bold and a different color the one where the score lies)
        wxPoint pt1;
        const wxFont quadLabelFont(wxFont(
            wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFractionalPointSize() * 1.25,
            wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false,
            GraphItems::Label::GetFirstAvailableMonospaceFont()));
        for (const auto& level : GetLevelLabels())
            {
            GetPhysicalCoordinates(level.GetX(), level.GetY(), pt1);
            auto levelLabel =
                std::make_unique<GraphItems::Label>(GraphItems::GraphItemInfo(level.GetLabel())
                                                        .Scaling(GetScaling())
                                                        .Pen(wxNullPen)
                                                        .Font(quadLabelFont)
                                                        .FontColor(labelFontColor)
                                                        .AnchorPoint(pt1));
            levelLabel->SetTextAlignment(Wisteria::TextAlignment::Centered);
            if (GetScores().size() == 1 && level == GetScores().front().GetScore())
                {
                levelLabel->GetFont().SetWeight(wxFontWeight::wxFONTWEIGHT_EXTRAHEAVY);
                levelLabel->SetFontColor(GetColorScheme()->GetColor(0));
                }
            AddObject(std::move(levelLabel));
            }
        }

    //----------------------------------------------------------------
    void FraseGraph::CalculateScorePositions(wxDC & dc)
        {
        if (GetDataset() == nullptr)
            {
            return;
            }
        const auto numberOfWordsColumn = GetContinuousColumn(m_numberOfWordsColumn);
        const auto numberOfSyllablesColumn = GetContinuousColumn(m_numberOfSyllablesColumn);
        const auto numberOfSentencesColumn = GetContinuousColumn(m_numberOfSentencesColumn);

        wxASSERT_MSG(m_backscreen, L"Backscreen not set!");
        wxASSERT_MSG(m_backscreen->GetBoundingBox(dc).GetWidth() ==
                         Canvas::GetDefaultCanvasWidthDIPs(),
                     L"Invalid backscreen size!");
        wxASSERT_MSG(m_backscreen->GetBoundingBox(dc).GetHeight() ==
                         Canvas::GetDefaultCanvasHeightDIPs(),
                     L"Invalid backscreen size!");

        auto points = std::make_unique<GraphItems::Points2D>(wxNullPen);
        points->SetScaling(GetScaling());
        points->SetDPIScaleFactor(GetDPIScaleFactor());
        points->Reserve(GetDataset()->GetRowCount());
        // these will all be filled with something, even if NaN
        m_results.resize(GetDataset()->GetRowCount());
        for (size_t i = 0; i < GetDataset()->GetRowCount(); ++i)
            {
            if (std::isnan(numberOfWordsColumn->GetValue(i)))
                {
                m_results[i].SetScoreInvalid(true);
                continue;
                }

            const auto normalizationFactor =
                safe_divide<double>(100, numberOfWordsColumn->GetValue(i));

            // add the score to the grouped data
            m_results[i] = Wisteria::ScorePoint(
                std::clamp<double>(normalizationFactor * numberOfSyllablesColumn->GetValue(i), 182,
                                   234),
                std::clamp<double>(normalizationFactor * numberOfSentencesColumn->GetValue(i), 0,
                                   15));

            m_results[i].ResetStatus();

            const auto calcScoreFromPolygons =
                [](const std::unique_ptr<FraseGraph>& graph, ScorePoint& scorePoint)
            {
                // see where the point is
                if (!graph->GetPhysicalCoordinates(scorePoint.m_wordStatistic,
                                                   scorePoint.m_sentenceStatistic,
                                                   scorePoint.m_scorePoint))
                    {
                    scorePoint.SetScoreInvalid(true);
                    return false;
                    }

                if (IsScoreInsideRegion(
                        scorePoint.m_scorePoint,
                        std::span(std::next(graph->m_levelLinePoints.cbegin(), 8), 3), 1, 1))
                    {
                    scorePoint.SetScore(4);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_levelLinePoints.cbegin(), 6), 4), 1, 1))
                    {
                    scorePoint.SetScore(3);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_levelLinePoints.cbegin(), 3), 5), 1, 1))
                    {
                    scorePoint.SetScore(2);
                    }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint,
                                             std::span(graph->m_levelLinePoints.cbegin(), 3), 1, 1))
                    {
                    scorePoint.SetScore(1);
                    }
                else
                    {
                    scorePoint.SetScoreOutOfGradeRange(true);
                    scorePoint.SetScoreInvalid(true);
                    }

                // if in a valid grade area see if it leans towards having harder sentences or words
                if (!scorePoint.IsScoreInvalid())
                    {
                    if (IsScoreInsideRegion(scorePoint.m_scorePoint, graph->m_dividerLinePoints, 1,
                                            1))
                        {
                        scorePoint.SetSentencesHard(false);
                        scorePoint.SetWordsHard(true);
                        }
                    else
                        {
                        scorePoint.SetWordsHard(false);
                        scorePoint.SetSentencesHard(true);
                        }
                    }

                return true;
            };

            calcScoreFromPolygons(m_backscreen, m_results[i]);

            // Convert group ID into color scheme index
            // (index is ordered by labels alphabetically).
            // Note that this will be zero if grouping is not in use.
            const size_t colorIndex =
                IsUsingGrouping() ? GetSchemeIndexFromGroupId(GetGroupColumn()->GetValue(i)) : 0;

            // see where the point is on this graph (not the backscreen) and
            // add it to be physically plotted
            if (GetPhysicalCoordinates(m_results[i].m_wordStatistic,
                                       m_results[i].m_sentenceStatistic, m_results[i].m_scorePoint))
                {
                points->AddPoint(
                    GraphItems::Point2D(
                        GraphItems::GraphItemInfo(GetDataset()->GetIdColumn().GetValue(i))
                            .AnchorPoint(m_results[i].m_scorePoint)
                            .Pen(
                                Colors::ColorContrast::BlackOrWhiteContrast(GetPlotOrCanvasColor()))
                            .Brush(GetColorScheme()->GetColor(colorIndex)),
                        Settings::GetPointRadius(), GetShapeScheme()->GetShape(colorIndex)),
                    dc);
                }
            else
                {
                wxFAIL_MSG(wxString::Format(
                    L"Point plotted on backscreen, but failed on main graph!\n"
                    "%.2f, %.2f",
                    m_results[i].m_wordStatistic, m_results[i].m_sentenceStatistic));
                }
            }
        AddObject(std::move(points));

        if (m_results.size() == 1 && m_results.front().IsScoreInvalid())
            {
            wxFont labelFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
            labelFont.SetPointSize(12);
            const wxPoint textCoordinate(
                GetPlotAreaBoundingBox().GetX() + (GetPlotAreaBoundingBox().GetWidth() / 2),
                GetPlotAreaBoundingBox().GetY() + (GetPlotAreaBoundingBox().GetHeight() / 2));
            AddObject(std::make_unique<GraphItems::Label>(
                GraphItems::GraphItemInfo(_(L"Invalid score: text is too difficult to be plotted"))
                    .Scaling(GetScaling())
                    .Pen(Wisteria::Colors::ColorBrewer::GetColor(Wisteria::Colors::Color::Black))
                    .Font(labelFont)
                    .AnchorPoint(textCoordinate)));
            }
        }

    //----------------------------------------------------------------
    void FraseGraph::SetAutoAccessibilityAttributes()
        {
        if (GetDataset() == nullptr || GetScores().empty())
            {
            return;
            }

        wxString label = _(L"A FRASE graph");
        AddAccessibilityAttribute(label, GetTitle().GetText(), L": ");
        AddAccessibilityAttribute(label, GetSubtitle().GetText(), L", ");

        // the scores are levels here, matching the labels on the graph's regions
        const auto formatScore = [](const ScorePoint& score) -> wxString
        {
            switch (score.GetScoreRange().first)
                {
            case 1:
                return _(L"Beginner Level");
            case 2:
                return _(L"Intermediate Level");
            case 3:
                return _(L"Advanced Intermediate Level");
            case 4:
                return _(L"Advanced Level");
            default:
                return wxString{};
                }
        };

        AddAccessibilityAttribute(label, FormatScoresForAccessibility(GetScores(), formatScore),
                                  L". ");

        AddAccessibilityAttribute(label, GetCaption().GetText(), L". ");

        if (!label.EndsWith(L"."))
            {
            label += L".";
            }

        GetAutoAccessibilityAttributes() = wxSVGAttributes{}.Role(_DT(L"img")).AriaLabel(label);
        }
    } // namespace Wisteria::Graphs
