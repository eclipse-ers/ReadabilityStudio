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

#include "frygraph.h"

wxIMPLEMENT_DYNAMIC_CLASS(Wisteria::Graphs::FryGraph, Wisteria::Graphs::PolygonReadabilityGraph)

    namespace Wisteria::Graphs
    {
    //----------------------------------------------------------------
    FryGraph::FryGraph(Wisteria::Canvas * canvas, const FryGraphType fType,
                       const std::shared_ptr<Colors::Schemes::ColorScheme>& colors /*= nullptr*/,
                       const std::shared_ptr<Icons::Schemes::IconScheme>& shapes /*= nullptr*/)
        : PolygonReadabilityGraph(canvas), m_fryGraphType(fType)
        {
        SetColorScheme(colors != nullptr ? colors :
                                           std::make_shared<Colors::Schemes::ColorScheme>(
                                               Settings::GetDefaultColorScheme()));
        SetShapeScheme(shapes != nullptr ? shapes :
                                           std::make_unique<Icons::Schemes::IconScheme>(
                                               Icons::Schemes::StandardShapes()));

        // TRANSLATORS: "Fry" is a test name, don't translate it.
        GetCanvas()->SetLabel(_(L"Fry Graph"));
        GetCanvas()->SetName(_(L"Fry Graph"));
        GetTitle() =
            GraphItems::Label(GraphItems::GraphItemInfo(
                                  _(L"GRAPH FOR ESTIMATING READABILITY"
                                    "—EXTENDED\nby Edward Fry, Rutgers University Reading Center, "
                                    "New Brunswick, NJ 08904"))
                                  .Scaling(GetScaling())
                                  .Pen(wxNullPen)
                                  .LabelAlignment(TextAlignment::Centered));
        GetTitle().GetFont().MakeSmaller();
        GetLeftYAxis().GetTitle().SetText(_(L"Average number of sentences per 100 words"));
        GetLeftYAxis().GetTitle().GetFont().MakeSmaller();
        GetTopXAxis().GetTitle().SetText(_(L"Average number of syllables per 100 words"));
        GetTopXAxis().GetTitle().GetFont().MakeSmaller();

        for (auto i = 108 + GetSyllableAxisOffset(); i <= 180 + GetSyllableAxisOffset(); i += 2)
            {
            GetBottomXAxis().AddUnevenAxisPoint(
                i, wxNumberFormatter::ToString(i, 0,
                                               wxNumberFormatter::Style::Style_NoTrailingZeroes));
            }
        GetBottomXAxis().SetCustomLabel(
            108 + GetSyllableAxisOffset(),
            GraphItems::Label(
                wxNumberFormatter::ToString(108 + GetSyllableAxisOffset(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                _DT(L"-")));
        GetBottomXAxis().AddUnevenAxisPoint(
            181 + GetSyllableAxisOffset(),
            wxNumberFormatter::ToString(181 + GetSyllableAxisOffset(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes));
        GetBottomXAxis().AddUnevenAxisPoint(
            182 + GetSyllableAxisOffset(),
            wxNumberFormatter::ToString(182 + GetSyllableAxisOffset(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                _DT(L"+"));
        GetBottomXAxis().AdjustRangeToLabels();
        GetBottomXAxis().SetDisplayInterval(2);

        MirrorXAxis(true);
        MirrorYAxis(true);

        GetLeftYAxis().AddUnevenAxisPoint(
            2.0,
            wxNumberFormatter::ToString(2.0, 1, wxNumberFormatter::Style::Style_None) + _DT(L"-"));
        GetLeftYAxis().AddUnevenAxisPoint(
            2.5, wxNumberFormatter::ToString(2.5, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            3.0, wxNumberFormatter::ToString(3.0, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            3.3, wxNumberFormatter::ToString(3.3, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            3.5, wxNumberFormatter::ToString(3.5, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            3.6, wxNumberFormatter::ToString(3.6, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            3.7, wxNumberFormatter::ToString(3.7, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            3.8, wxNumberFormatter::ToString(3.8, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            4.0, wxNumberFormatter::ToString(4.0, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            4.2, wxNumberFormatter::ToString(4.2, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            4.3, wxNumberFormatter::ToString(4.3, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            4.5, wxNumberFormatter::ToString(4.5, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            4.8, wxNumberFormatter::ToString(4.8, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            5.0, wxNumberFormatter::ToString(5.0, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            5.2, wxNumberFormatter::ToString(5.2, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            5.6, wxNumberFormatter::ToString(5.6, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            5.9, wxNumberFormatter::ToString(5.9, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            6.3, wxNumberFormatter::ToString(6.3, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            6.7, wxNumberFormatter::ToString(6.7, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            7.1, wxNumberFormatter::ToString(7.1, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            7.7, wxNumberFormatter::ToString(7.7, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            8.3, wxNumberFormatter::ToString(8.3, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            9.1, wxNumberFormatter::ToString(9.1, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            10.0, wxNumberFormatter::ToString(10.0, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            11.1, wxNumberFormatter::ToString(11.1, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            12.5, wxNumberFormatter::ToString(12.5, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            14.3, wxNumberFormatter::ToString(14.3, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            16.7, wxNumberFormatter::ToString(16.7, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            20.0, wxNumberFormatter::ToString(20.0, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            25.0,
            wxNumberFormatter::ToString(25.0, 1, wxNumberFormatter::Style::Style_None) + _DT(L"+"));
        GetLeftYAxis().AdjustRangeToLabels();

        AddLevelLabels();
        AddBrackets();

        if (m_fryGraphType == FryGraphType::GPM)
            {
            SetAsGilliamPenaMountainGraph();
            }
        }

    //----------------------------------------------------------------
    void FryGraph::SetData(const std::shared_ptr<Wisteria::Data::Dataset>& data,
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

        m_numberOfWordsColumn = numberOfWordsColumnName;
        m_numberOfSyllablesColumn = numberOfSyllablesColumnName;
        m_numberOfSentencesColumn = numberOfSentencesColumnName;
        SetGroupColumn(groupColumnName);

        // if grouping, build the list of group IDs, sorted by their respective labels
        if (IsUsingGrouping())
            {
            BuildGroupIdMap();
            }

        BuildBackscreen();
        }

    //----------------------------------------------------------------
    void FryGraph::BuildBackscreen()
        {
        if (m_backscreen == nullptr)
            {
            // Shape and color schemes are irrelevant, as this is used
            // only for the base polygon calculations.
            m_backscreen = std::make_unique<FryGraph>(GetCanvas(), m_fryGraphType);
            m_backscreen->SetMessageCatalog(GetMessageCatalog());
            // Also, leave the data as null, as that isn't used either for
            // simply creating polygons. All scaling should just be 1.0 (even DPI, see below).
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
    void FryGraph::RecalcSizes(wxDC & dc)
        {
        Graph2D::RecalcSizes(dc);

        wxASSERT_MSG((!m_backscreen || m_backscreen->GetBoundingBox(dc).GetWidth() ==
                                           Canvas::GetDefaultCanvasWidthDIPs()),
                     L"Invalid backscreen size!");
        wxASSERT_MSG((!m_backscreen || m_backscreen->GetBoundingBox(dc).GetHeight() ==
                                           Canvas::GetDefaultCanvasHeightDIPs()),
                     L"Invalid backscreen size!");

        const wxColour labelFontColor{ GetLeftYAxis().GetFontColor() };

        // long sentence danger area
        GetPhysicalCoordinates(108 + GetSyllableAxisOffset(), 2.0, m_longSentencesPoints[0]);
        GetPhysicalCoordinates(124 + GetSyllableAxisOffset(), 2.0, m_longSentencesPoints[1]);
        GetPhysicalCoordinates(108 + GetSyllableAxisOffset(), 4.2, m_longSentencesPoints[2]);
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxNullPen)
                .Text(_(L"Invalid region: sentences are too long"))
                .Brush(wxBrush(Colors::ColorContrast::ChangeOpacity(GetInvalidAreaColor(), 100))),
            m_longSentencesPoints));
        // long word danger area
        GetPhysicalCoordinates(141 + GetSyllableAxisOffset(), 25.0, m_longWordPoints[0]);
        GetPhysicalCoordinates(148 + GetSyllableAxisOffset(), 14.3, m_longWordPoints[1]);
        GetPhysicalCoordinates(154 + GetSyllableAxisOffset(), 11.1, m_longWordPoints[2]);
        GetPhysicalCoordinates(158 + GetSyllableAxisOffset(), 10.0, m_longWordPoints[3]);
        GetPhysicalCoordinates(162 + GetSyllableAxisOffset(), 9.1, m_longWordPoints[4]);
        GetPhysicalCoordinates(168 + GetSyllableAxisOffset(), 8.3, m_longWordPoints[5]);
        GetPhysicalCoordinates(175 + GetSyllableAxisOffset(), 7.6, m_longWordPoints[6]);
        GetPhysicalCoordinates(182 + GetSyllableAxisOffset(), 7.3, m_longWordPoints[7]);
        GetPhysicalCoordinates(182 + GetSyllableAxisOffset(), 25.0, m_longWordPoints[8]);
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxNullPen)
                .Text(_(L"Invalid region: too many complex words"))
                .Brush(wxBrush(Colors::ColorContrast::ChangeOpacity(GetInvalidAreaColor(), 100))),
            m_longWordPoints));
        // divider line
        GetPhysicalCoordinates(119.5 + GetSyllableAxisOffset(), 15.8, m_dividerLinePoints[0]);
        GetPhysicalCoordinates(120.3 + GetSyllableAxisOffset(), 11.1, m_dividerLinePoints[1]);
        GetPhysicalCoordinates(121.3 + GetSyllableAxisOffset(), 9.1, m_dividerLinePoints[2]);
        GetPhysicalCoordinates(122 + GetSyllableAxisOffset(), 8.3, m_dividerLinePoints[3]);
        GetPhysicalCoordinates(124.2 + GetSyllableAxisOffset(), 7.1, m_dividerLinePoints[4]);
        GetPhysicalCoordinates(128 + GetSyllableAxisOffset(), 6.1, m_dividerLinePoints[5]);
        GetPhysicalCoordinates(134.1 + GetSyllableAxisOffset(), 5.2, m_dividerLinePoints[6]);
        GetPhysicalCoordinates(146 + GetSyllableAxisOffset(), 4.5, m_dividerLinePoints[7]);
        GetPhysicalCoordinates(167.2 + GetSyllableAxisOffset(), 4.0, m_dividerLinePoints[8]);
        GetPhysicalCoordinates(182 + GetSyllableAxisOffset(), 3.75, m_dividerLinePoints[9]);
        // the following are not drawn, just used for calculating information
        GetPhysicalCoordinates(182 + GetSyllableAxisOffset(), 25.0, m_dividerLinePoints[10]);
        GetPhysicalCoordinates(119 + GetSyllableAxisOffset(), 25.0, m_dividerLinePoints[11]);
        // grade lines
        // lower area
        GetPhysicalCoordinates(108 + GetSyllableAxisOffset(), 11.1, m_gradeLinePoints[0]);
        GetPhysicalCoordinates(108 + GetSyllableAxisOffset(), 25, m_gradeLinePoints[1]);
        GetPhysicalCoordinates(132 + GetSyllableAxisOffset(), 25, m_gradeLinePoints[2]);
        // 1st grade
        GetPhysicalCoordinates(132.2 + GetSyllableAxisOffset(), 23.0, m_gradeLinePoints[3]);
        GetPhysicalCoordinates(109 + GetSyllableAxisOffset(), 10.0, m_gradeLinePoints[4]);
        // 2nd grade
        GetPhysicalCoordinates(109 + GetSyllableAxisOffset(), 7.9, m_gradeLinePoints[5]);
        GetPhysicalCoordinates(140.2 + GetSyllableAxisOffset(), 23.0, m_gradeLinePoints[6]);
        // 3rd grade
        GetPhysicalCoordinates(142 + GetSyllableAxisOffset(), 18.0, m_gradeLinePoints[7]);
        GetPhysicalCoordinates(109 + GetSyllableAxisOffset(), 6.5, m_gradeLinePoints[8]);
        // 4th grade
        GetPhysicalCoordinates(109.55 + GetSyllableAxisOffset(), 5.8, m_gradeLinePoints[9]);
        GetPhysicalCoordinates(144.3 + GetSyllableAxisOffset(), 14.3, m_gradeLinePoints[10]);
        // 5th grade
        GetPhysicalCoordinates(146 + GetSyllableAxisOffset(), 12.5, m_gradeLinePoints[11]);
        GetPhysicalCoordinates(109.55 + GetSyllableAxisOffset(), 5.15, m_gradeLinePoints[12]);
        // 6th grade
        GetPhysicalCoordinates(112.0 + GetSyllableAxisOffset(), 4.2, m_gradeLinePoints[13]);
        GetPhysicalCoordinates(149 + GetSyllableAxisOffset(), 11.2, m_gradeLinePoints[14]);
        // 7th grade
        GetPhysicalCoordinates(153 + GetSyllableAxisOffset(), 9.1, m_gradeLinePoints[15]);
        GetPhysicalCoordinates(120 + GetSyllableAxisOffset(), 3.58, m_gradeLinePoints[16]);
        // 8th grade
        GetPhysicalCoordinates(127 + GetSyllableAxisOffset(), 3.2, m_gradeLinePoints[17]);
        GetPhysicalCoordinates(157 + GetSyllableAxisOffset(), 8.6, m_gradeLinePoints[18]);
        // 9th grade
        GetPhysicalCoordinates(159.5 + GetSyllableAxisOffset(), 8.2, m_gradeLinePoints[19]);
        GetPhysicalCoordinates(137.2 + GetSyllableAxisOffset(), 3.1, m_gradeLinePoints[20]);
        // 10th grade
        GetPhysicalCoordinates(144.1 + GetSyllableAxisOffset(), 2.5, m_gradeLinePoints[21]);
        GetPhysicalCoordinates(164 + GetSyllableAxisOffset(), 7.6, m_gradeLinePoints[22]);
        // 11th grade
        GetPhysicalCoordinates(168 + GetSyllableAxisOffset(), 7.2, m_gradeLinePoints[23]);
        GetPhysicalCoordinates(149 + GetSyllableAxisOffset(), 2.5, m_gradeLinePoints[24]);
        // 12th grade
        GetPhysicalCoordinates(155.5 + GetSyllableAxisOffset(), 2.4, m_gradeLinePoints[25]);
        GetPhysicalCoordinates(170.2 + GetSyllableAxisOffset(), 7.1, m_gradeLinePoints[26]);
        // 13th grade
        GetPhysicalCoordinates(172.2 + GetSyllableAxisOffset(), 7.0, m_gradeLinePoints[27]);
        GetPhysicalCoordinates(162 + GetSyllableAxisOffset(), 2.4, m_gradeLinePoints[28]);
        // 14th grade
        GetPhysicalCoordinates(168.5 + GetSyllableAxisOffset(), 2.4, m_gradeLinePoints[29]);
        GetPhysicalCoordinates(174.2 + GetSyllableAxisOffset(), 6.8, m_gradeLinePoints[30]);
        // 15th grade
        GetPhysicalCoordinates(176.2 + GetSyllableAxisOffset(), 6.8, m_gradeLinePoints[31]);
        GetPhysicalCoordinates(173.8 + GetSyllableAxisOffset(), 2.4, m_gradeLinePoints[32]);
        // 16th grade
        GetPhysicalCoordinates(179 + GetSyllableAxisOffset(), 2.4, m_gradeLinePoints[33]);
        GetPhysicalCoordinates(180.2 + GetSyllableAxisOffset(), 6.8, m_gradeLinePoints[34]);
        // beyond 16th grade
        GetPhysicalCoordinates(182 + GetSyllableAxisOffset(), 6.8, m_gradeLinePoints[35]);
        GetPhysicalCoordinates(182 + GetSyllableAxisOffset(), 2.4, m_gradeLinePoints[36]);

        const wxBrush selectionBrush = wxBrush(Colors::ColorContrast::ChangeOpacity(
            Colors::ColorBrewer::GetColor(Colors::Color::LightGray), 100));
        wxColour separatorColor{ Colors::ColorContrast::ChangeOpacity(
            Wisteria::Colors::ColorBrewer::GetColor(Wisteria::Colors::Color::Black), 200) };
        separatorColor =
            Colors::ColorContrast::ShadeOrTintIfClose(separatorColor, GetPlotOrCanvasColor());

        // the separator line
        auto levelsSpline = std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxPen{ separatorColor })
                .Brush(wxBrush{ separatorColor })
                .Selectable(false)
                .Scaling(GetScaling())
                .DPIScaling(GetDPIScaleFactor()),
            std::span(m_dividerLinePoints.cbegin(), m_dividerLinePoints.size() - 2));
        levelsSpline->SetShape(GraphItems::Polygon::PolygonShape::Spline);
        AddObject(std::move(levelsSpline));

        wxASSERT_MSG(GetMessageCatalog(), L"Label manager not set in Fry Graph!");

        // draw the grade areas (for selecting)
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo(GetMessageCatalog()->GetGradeScaleLongLabel(1))
                .Pen(wxNullPen)
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            std::span(m_gradeLinePoints.cbegin(), 5)));
        // the rest of the grade areas
        for (size_t i = 2, pointIter = 5; i <= 16; ++i, pointIter += 2)
            {
            AddObject(std::make_unique<GraphItems::Polygon>(
                GraphItems::GraphItemInfo(GetMessageCatalog()->GetGradeScaleLongLabel(i))
                    .Pen(wxNullPen)
                    .Brush(wxNullBrush)
                    .SelectionBrush(selectionBrush),
                std::span(std::next(m_gradeLinePoints.cbegin(), (pointIter - 2)), 4)));
            }
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo(_(L"Post-graduate+"))
                .Pen(wxNullPen)
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            std::span(std::next(m_gradeLinePoints.cbegin(), 33), 4)));

        CalculateScorePositions(dc);

        // add the grade labels to the regions
        // (and highlight in heavy bold and a different color the one where the score lies)
        const wxFont labelFont =
            wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize() * 1.25,
                   wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false,
                   GetFancyFontFaceName());
        for (const auto& level : GetLevelLabels())
            {
            wxPoint point;
            GetPhysicalCoordinates(level.GetX(), level.GetY(), point);

            auto levelLabel =
                std::make_unique<GraphItems::Label>(GraphItems::GraphItemInfo(level.GetLabel())
                                                        .Scaling(GetScaling())
                                                        .Pen(wxNullPen)
                                                        .Font(labelFont)
                                                        .FontColor(labelFontColor)
                                                        .AnchorPoint(point));
            levelLabel->SetTextAlignment(TextAlignment::Centered);
            if (GetScores().size() == 1)
                {
                // ghost non-scored sections
                if (IsShowcasingScore() && level != GetScores().front().GetScore())
                    {
                    levelLabel->SetFontColor(Colors::ColorContrast::ChangeOpacity(
                        labelFontColor, Wisteria::Settings::GHOST_OPACITY));
                    }
                else
                    {
                    // If showcasing, then we will leave the scored section the same
                    // (this area will be shaded)...
                    levelLabel->GetFont().SetWeight(wxFontWeight::wxFONTWEIGHT_EXTRAHEAVY);
                    // ...but if not showcasing and this section is scored, then make it bold and
                    // change the color.
                    if (!IsShowcasingScore() && level == GetScores().front().GetScore())
                        {
                        levelLabel->SetFontColor(GetColorScheme()->GetColor(0));
                        }
                    }
                }
            AddObject(std::move(levelLabel));
            }
        }

    //----------------------------------------------------------------
    void FryGraph::CalculateScorePositions(wxDC & dc)
        {
        const wxColour gradeLineColor{ Colors::ColorContrast::IsDark(GetPlotOrCanvasColor()) ?
                                           Colors::ColorBrewer::GetColor(Colors::Color::BondiBlue) :
                                           Colors::ColorBrewer::GetColor(Colors::Color::Blue) };

        if (GetDataset() == nullptr)
            {
            // draw regular grade lines and return since there are no points to plot
            constexpr uint8_t OPACITY_LEVEL{ 200 };
            AddObject(std::make_unique<GraphItems::Polygon>(
                GraphItems::GraphItemInfo{}
                    .Pen(wxPen{
                        Colors::ColorContrast::ChangeOpacity(gradeLineColor, OPACITY_LEVEL) })
                    .Brush(gradeLineColor)
                    .Scaling(GetScaling()),
                std::span(std::next(m_gradeLinePoints.cbegin(), 3), 2)));
            // the rest of grade region lines
            for (size_t i = 2, pointIter = 5; i <= 16; ++i, pointIter += 2)
                {
                AddObject(std::make_unique<GraphItems::Polygon>(
                    GraphItems::GraphItemInfo{}
                        .Pen(wxPen{
                            Colors::ColorContrast::ChangeOpacity(gradeLineColor, OPACITY_LEVEL) })
                        .Brush(gradeLineColor)
                        .Scaling(GetScaling()),
                    std::span(std::next(m_gradeLinePoints.cbegin(), pointIter), 2)));
                }
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

        std::vector<wxPoint> highlightedGradeLinePoints;

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
                std::clamp<double>(normalizationFactor * numberOfSyllablesColumn->GetValue(i),
                                   108 + static_cast<double>(GetSyllableAxisOffset()),
                                   182 + static_cast<double>(GetSyllableAxisOffset())),
                std::clamp<double>(normalizationFactor * numberOfSentencesColumn->GetValue(i), 2,
                                   25));

            m_results[i].ResetStatus();

            const auto calcScoreFromPolygons =
                [](const std::unique_ptr<FryGraph>& graph, ScorePoint& scorePoint)
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
                        std::span(std::next(graph->m_gradeLinePoints.cbegin(), 33), 4), 1, 1))
                    {
                    scorePoint.SetScore(17);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 31), 4), 1, 1))
                    {
                    scorePoint.SetScore(16);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 29), 4), 1, 1))
                    {
                    scorePoint.SetScore(15);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 27), 4), 1, 1))
                    {
                    scorePoint.SetScore(14);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 25), 4), 1, 1))
                    {
                    scorePoint.SetScore(13);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 23), 4), 1, 1))
                    {
                    scorePoint.SetScore(12);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 21), 4), 1, 1))
                    {
                    scorePoint.SetScore(11);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 19), 4), 1, 1))
                    {
                    scorePoint.SetScore(10);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 17), 4), 1, 1))
                    {
                    scorePoint.SetScore(9);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 15), 4), 1, 1))
                    {
                    scorePoint.SetScore(8);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 13), 4), 1, 1))
                    {
                    scorePoint.SetScore(7);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 11), 4), 1, 1))
                    {
                    scorePoint.SetScore(6);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 9), 4), 1, 1))
                    {
                    scorePoint.SetScore(5);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 7), 4), 1, 1))
                    {
                    scorePoint.SetScore(4);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 5), 4), 1, 1))
                    {
                    scorePoint.SetScore(3);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 3), 4), 1, 1))
                    {
                    scorePoint.SetScore(2);
                    }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint,
                                             std::span(graph->m_gradeLinePoints.cbegin(), 5), 1, 1))
                    {
                    scorePoint.SetScore(1);
                    }
                else
                    {
                    scorePoint.SetScoreOutOfGradeRange(true);
                    }

                // if in a valid grade area see if it
                // leans towards having harder sentences or words
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

            if (IsShowcasingScore() && GetScores().size() == 1)
                {
                if (GetScores().at(0).GetScore() == 17)
                    {
                    std::copy(&m_gradeLinePoints[33], &m_gradeLinePoints[36],
                              std::back_inserter(highlightedGradeLinePoints));
                    highlightedGradeLinePoints.push_back(m_gradeLinePoints[36]);
                    }
                else if (GetScores().at(0).GetScore() == 16)
                    {
                    std::copy(&m_gradeLinePoints[31], &m_gradeLinePoints[35],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 15)
                    {
                    std::copy(&m_gradeLinePoints[29], &m_gradeLinePoints[33],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 14)
                    {
                    std::copy(&m_gradeLinePoints[27], &m_gradeLinePoints[31],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 13)
                    {
                    std::copy(&m_gradeLinePoints[25], &m_gradeLinePoints[29],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 12)
                    {
                    std::copy(&m_gradeLinePoints[23], &m_gradeLinePoints[27],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 11)
                    {
                    std::copy(&m_gradeLinePoints[21], &m_gradeLinePoints[25],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 10)
                    {
                    std::copy(&m_gradeLinePoints[19], &m_gradeLinePoints[23],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 9)
                    {
                    std::copy(&m_gradeLinePoints[17], &m_gradeLinePoints[21],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 8)
                    {
                    std::copy(&m_gradeLinePoints[15], &m_gradeLinePoints[19],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 7)
                    {
                    std::copy(&m_gradeLinePoints[13], &m_gradeLinePoints[17],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 6)
                    {
                    std::copy(&m_gradeLinePoints[11], &m_gradeLinePoints[15],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 5)
                    {
                    std::copy(&m_gradeLinePoints[9], &m_gradeLinePoints[13],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 4)
                    {
                    std::copy(&m_gradeLinePoints[7], &m_gradeLinePoints[11],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 3)
                    {
                    std::copy(&m_gradeLinePoints[5], &m_gradeLinePoints[9],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 2)
                    {
                    std::copy(&m_gradeLinePoints[3], &m_gradeLinePoints[7],
                              std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScore() == 1)
                    {
                    std::copy(m_gradeLinePoints.data(), &m_gradeLinePoints[5],
                              std::back_inserter(highlightedGradeLinePoints));
                    }

                if (!highlightedGradeLinePoints.empty())
                    {
                    AddObject(std::make_unique<GraphItems::Polygon>(
                        GraphItems::GraphItemInfo{}
                            .Pen(Colors::ColorContrast::ChangeOpacity(
                                Colors::ColorBrewer::GetColor(Colors::Color::BondiBlue), 100))
                            .Brush(Colors::ColorContrast::ChangeOpacity(
                                Colors::ColorBrewer::GetColor(Colors::Color::BondiBlue), 100))
                            .Scaling(GetScaling()),
                        highlightedGradeLinePoints));
                    }
                }
            }

        // draw the grade lines
        auto foundHighlightLine =
            std::ranges::find(std::as_const(highlightedGradeLinePoints), m_gradeLinePoints[3]);
        uint8_t opacityLevel = (foundHighlightLine == highlightedGradeLinePoints.cend() &&
                                IsShowcasingScore() && GetScores().size() == 1) ?
                                   Wisteria::Settings::GHOST_OPACITY :
                                   200;
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxPen{ Colors::ColorContrast::ChangeOpacity(gradeLineColor, opacityLevel) })
                .Brush(gradeLineColor)
                .Scaling(GetScaling()),
            std::span(std::next(m_gradeLinePoints.cbegin(), 3), 2)));
        // the rest of grade region lines
        for (size_t i = 2, pointIter = 5; i <= 16; ++i, pointIter += 2)
            {
            foundHighlightLine = std::ranges::find(std::as_const(highlightedGradeLinePoints),
                                                   m_gradeLinePoints[pointIter]);
            opacityLevel = (foundHighlightLine == highlightedGradeLinePoints.cend() &&
                            IsShowcasingScore() && GetScores().size() == 1) ?
                               Wisteria::Settings::GHOST_OPACITY :
                               200;
            AddObject(std::make_unique<GraphItems::Polygon>(
                GraphItems::GraphItemInfo{}
                    .Pen(
                        wxPen{ Colors::ColorContrast::ChangeOpacity(gradeLineColor, opacityLevel) })
                    .Brush(gradeLineColor)
                    .Scaling(GetScaling()),
                std::span(std::next(m_gradeLinePoints.cbegin(), pointIter), 2)));
            }

        // draw the description label
        const wxColour labelFontColor{ GetLeftYAxis().GetFontColor() };
        wxPoint pt1, pt2;
        GetPhysicalCoordinates(112 + GetSyllableAxisOffset(), 5.0, pt1);
        GetPhysicalCoordinates(128 + GetSyllableAxisOffset(), 3.3, pt2);
        auto gradeLevelLabel = std::make_unique<GraphItems::Label>(
            // TRANSLATORS: Extra spaces are used to make uppercased text easier to read.
            GraphItems::GraphItemInfo(_(L"APPROXIMATE  GRADE  LEVEL"))
                .Pen(wxNullPen)
                .Selectable(false)
                .FontColor(labelFontColor)
                // overriding scaling with a hard-coded font
                // size returned from CalcDiagonalFontSize()
                .Scaling(1)
                .DPIScaling(1)
                .AnchorPoint(pt1)
                .Anchoring(Anchoring::TopLeftCorner));
        gradeLevelLabel->Tilt(-45);
        // make it fit inside the graph
        const wxRect gradeLabelArea{ pt1, pt2 };
        wxFont labelFont(wxFontInfo().FaceName(GetFancyFontFaceName()));
        labelFont.SetPointSize(GraphItems::Label::CalcDiagonalFontSize(
            dc, labelFont, gradeLabelArea, 45, gradeLevelLabel->GetText()));
        gradeLevelLabel->SetFont(labelFont);
        if constexpr (Settings::IsDebugFlagEnabled(DebugSettings::DrawExtraInformation))
            {
            AddObject(std::make_unique<GraphItems::Polygon>(
                GraphItems::GraphItemInfo{}
                    .Pen(Colors::ColorBrewer::GetColor(Colors::Color::Red))
                    .Brush(wxNullBrush),
                std::vector<wxPoint>{ gradeLabelArea.GetTopLeft(), gradeLabelArea.GetTopRight(),
                                      gradeLabelArea.GetBottomRight(),
                                      gradeLabelArea.GetBottomLeft() }));
            }

        AddObject(std::move(gradeLevelLabel));

        // plot the scores
        auto points = std::make_unique<GraphItems::Points2D>(wxNullPen);
        points->SetScaling(GetScaling());
        points->SetDPIScaleFactor(GetDPIScaleFactor());
        points->Reserve(GetDataset()->GetRowCount());
        for (size_t i = 0; i < GetDataset()->GetRowCount(); ++i)
            {
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

        if (GetScores().size() == 1 && GetScores().front().IsScoreInvalid())
            {
            labelFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
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
    void FryGraph::SetAutoAccessibilityAttributes()
        {
        if (GetDataset() == nullptr || GetScores().empty())
            {
            return;
            }

        wxString label = (m_fryGraphType == FryGraphType::GPM) ?
                             _(L"A Gilliam-Peña-Mountain graph") :
                             // TRANSLATORS: "Fry" is a test name, don't translate it.
                             _(L"A Fry graph");
        AddAccessibilityAttribute(label, GetTitle().GetText(), L": ");
        AddAccessibilityAttribute(label, GetSubtitle().GetText(), L", ");

        // the highest region covers everything beyond the last grade
        const auto formatScore = [this](const ScorePoint& score) -> wxString
        {
            return (score.GetScoreRange().first == 17) ? _(L"Post-graduate+") :
                                                         FormatScoreAsGradeLevel(score);
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

    //----------------------------------------------------------------
    void FryGraph::SetAsGilliamPenaMountainGraph()
        {
        m_syllableAxisOffset = 67;
        GetTitle().SetText(_(L"Gilliam-Peña-Mountain Graph"));
        GetCanvas()->SetLabel(_(L"Gilliam-Peña-Mountain Graph"));
        GetCanvas()->SetName(_(L"Gilliam-Peña-Mountain Graph"));

        GetBottomXAxis().Reset();
        for (auto i = 108 + GetSyllableAxisOffset(); i <= 180 + GetSyllableAxisOffset(); i += 2)
            {
            GetBottomXAxis().AddUnevenAxisPoint(
                i, wxNumberFormatter::ToString(i, 0,
                                               wxNumberFormatter::Style::Style_NoTrailingZeroes));
            }
        GetBottomXAxis().SetCustomLabel(
            108 + GetSyllableAxisOffset(),
            GraphItems::Label(
                wxNumberFormatter::ToString(108 + GetSyllableAxisOffset(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                L"-"));
        GetBottomXAxis().AddUnevenAxisPoint(
            181 + GetSyllableAxisOffset(),
            wxNumberFormatter::ToString(181 + GetSyllableAxisOffset(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes));
        GetBottomXAxis().AddUnevenAxisPoint(
            182 + GetSyllableAxisOffset(),
            wxNumberFormatter::ToString(182 + GetSyllableAxisOffset(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                L"+");
        GetBottomXAxis().AdjustRangeToLabels();
        GetBottomXAxis().SetDisplayInterval(2);

        AddLevelLabels();
        AddBrackets();
        }

    //----------------------------------------------------------------
    void FryGraph::AddBrackets()
        {
        GetLeftYAxis().ClearBrackets();
        GraphItems::Axis::AxisBracket yBracketLong(2, 5.6, 3.6, _(L"Long sentences"));
        yBracketLong.SetBracketLineStyle(Wisteria::BracketLineStyle::NoConnectionLines);
        yBracketLong.GetLabel().SetTextOrientation(Orientation::Vertical);
        yBracketLong.SetPadding(0);
        yBracketLong.GetLabel().SetFont(GetTopXAxis().GetFont());
        GetLeftYAxis().AddBracket(yBracketLong);

        GraphItems::Axis::AxisBracket yBracketShort(5.6, 25, 12.5, _(L"Short sentences"));
        yBracketShort.SetBracketLineStyle(Wisteria::BracketLineStyle::NoConnectionLines);
        yBracketShort.GetLabel().SetTextOrientation(Orientation::Vertical);
        yBracketShort.SetPadding(0);
        yBracketShort.GetLabel().SetFont(GetTopXAxis().GetFont());
        GetLeftYAxis().AddBracket(yBracketShort);

        GetLeftYAxis().EnableAutoStacking(false);

        GetTopXAxis().ClearBrackets();
        GraphItems::Axis::AxisBracket xBracketLong(140 + GetSyllableAxisOffset(),
                                                   182 + GetSyllableAxisOffset(),
                                                   164 + GetSyllableAxisOffset(), _(L"Long words"));
        xBracketLong.SetBracketLineStyle(Wisteria::BracketLineStyle::NoConnectionLines);
        xBracketLong.GetLabel().SetTextOrientation(Orientation::Horizontal);
        xBracketLong.SetPadding(0);
        xBracketLong.GetLabel().SetFont(GetLeftYAxis().GetFont());
        GetTopXAxis().AddBracket(xBracketLong);

        GraphItems::Axis::AxisBracket xBracketShort(
            108 + GetSyllableAxisOffset(), 140 + GetSyllableAxisOffset(),
            126 + GetSyllableAxisOffset(), _(L"Short words"));
        xBracketShort.SetBracketLineStyle(Wisteria::BracketLineStyle::NoConnectionLines);
        xBracketShort.GetLabel().SetTextOrientation(Orientation::Horizontal);
        xBracketShort.SetPadding(0);
        xBracketShort.GetLabel().SetFont(GetLeftYAxis().GetFont());
        GetTopXAxis().AddBracket(xBracketShort);
        }

    //----------------------------------------------------------------
    void FryGraph::AddLevelLabels()
        {
        GetLevelLabels().clear();
        AddLevelLabel(LevelLabel(114.6 + GetSyllableAxisOffset(), 14.5, _DT(L"1"), 1, 1));
        AddLevelLabel(LevelLabel(115.5 + GetSyllableAxisOffset(), 10.6, L"2", 2, 2));
        AddLevelLabel(LevelLabel(116.8 + GetSyllableAxisOffset(), 8.6, L"3", 3, 3));
        AddLevelLabel(LevelLabel(118.4 + GetSyllableAxisOffset(), 7.3, L"4", 4, 4));
        AddLevelLabel(LevelLabel(119.7 + GetSyllableAxisOffset(), 6.6, L"5", 5, 5));
        AddLevelLabel(LevelLabel(121.8 + GetSyllableAxisOffset(), 5.7, L"6", 6, 6));
        AddLevelLabel(LevelLabel(127.2 + GetSyllableAxisOffset(), 4.8, L"7", 7, 7));
        AddLevelLabel(LevelLabel(134 + GetSyllableAxisOffset(), 4.25, L"8", 8, 8));
        AddLevelLabel(LevelLabel(140 + GetSyllableAxisOffset(), 4.0, L"9", 9, 9));
        AddLevelLabel(LevelLabel(146 + GetSyllableAxisOffset(), 3.75, L"10", 10, 10));
        AddLevelLabel(LevelLabel(151.3 + GetSyllableAxisOffset(), 3.65, L"11", 11, 11));
        AddLevelLabel(LevelLabel(155.8 + GetSyllableAxisOffset(), 3.6, L"12", 12, 12));
        AddLevelLabel(LevelLabel(161.5 + GetSyllableAxisOffset(), 3.55, L"13", 13, 13));
        AddLevelLabel(LevelLabel(167.2 + GetSyllableAxisOffset(), 3.52, L"14", 14, 14));
        AddLevelLabel(LevelLabel(172.2 + GetSyllableAxisOffset(), 3.5, L"15", 15, 15));
        AddLevelLabel(LevelLabel(176.5 + GetSyllableAxisOffset(), 3.48, L"16", 16, 16));
        AddLevelLabel(LevelLabel(180.8 + GetSyllableAxisOffset(), 3.48, L"17+", 17, 17));
        }
    } // namespace Wisteria::Graphs
