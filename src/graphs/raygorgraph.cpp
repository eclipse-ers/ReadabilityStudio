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

#include "raygorgraph.h"

wxIMPLEMENT_DYNAMIC_CLASS(Wisteria::Graphs::RaygorGraph, Wisteria::Graphs::PolygonReadabilityGraph)

    namespace Wisteria::Graphs
    {
    //----------------------------------------------------------------
    RaygorGraph::RaygorGraph(
        Wisteria::Canvas * canvas,
        const std::shared_ptr<Colors::Schemes::ColorScheme>& colors /*= nullptr*/,
        const std::shared_ptr<Icons::Schemes::IconScheme>& shapes /*= nullptr*/)
        : PolygonReadabilityGraph(canvas)
        {
        SetColorScheme(colors != nullptr ? colors :
                                           std::make_shared<Colors::Schemes::ColorScheme>(
                                               Settings::GetDefaultColorScheme()));
        SetShapeScheme(shapes != nullptr ? shapes :
                                           std::make_unique<Icons::Schemes::IconScheme>(
                                               Icons::Schemes::StandardShapes()));

        GetCanvas()->SetLabel(_(L"Raygor Estimate"));
        GetCanvas()->SetName(_(L"Raygor Estimate"));

        // build the axes and area labels based on the graph's style
        SetRaygorStyle(GetRaygorStyle());
        }

    //----------------------------------------------------------------
    void RaygorGraph::SetRaygorStyle(const RaygorStyle style)
        {
        m_raygorStyle = style;

        GetLeftYAxis().Reset();
        GetBottomXAxis().Reset();
        GetLevelLabels().clear();

        GetLeftYAxis().GetTitle().SetText((GetRaygorStyle() == RaygorStyle::Modern) ?
                                              _(L"SENTENCES\n(per 100 words)") :
                                              // TRANSLATORS: Uppercasing is just for display
                                              _(L"SENTENCES"));
        GetLeftYAxis().GetTitle().SetTextAlignment(TextAlignment::Centered);
        GetBottomXAxis().GetTitle().SetText((GetRaygorStyle() == RaygorStyle::Modern) ?
                                                _(L"LONG WORDS\n(per 100 words)") :
                                                _(L"LONG WORDS"));
        GetBottomXAxis().GetTitle().SetTextAlignment(TextAlignment::Centered);

        GetTitle().SetText(_(L"THE RAYGOR READABILITY ESTIMATE\nALTON L. RAYGOR"
                             "\U00002014UNIVERSITY OF MINNESOTA"));
        GetTitle()
            .GetGraphItemInfo()
            .Scaling(GetScaling())
            .Pen(wxNullPen)
            .LabelAlignment(TextAlignment::Centered)
            .ChildAlignment(RelativeAlignment::Centered);
        GetTitle()
            .GetHeaderInfo()
            .Enable(true)
            .LabelAlignment(Wisteria::TextAlignment::Centered)
            .Font(GetTitle().GetFont())
            .FontColor(GetTitle().GetFontColor())
            .GetFont()
            .MakeBold();

        GetBottomXAxis().SetRange(6, 44, 0, 2, 1);
        GetBottomXAxis().SetCustomLabel(
            6, GraphItems::Label(
                   wxNumberFormatter::ToString(6, 0,
                                               wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                   ((GetRaygorStyle() == RaygorStyle::Modern) ? wxString{ L"(-)" } : wxString{})));
        GetBottomXAxis().SetCustomLabel(
            44, GraphItems::Label(
                    wxNumberFormatter::ToString(44, 0,
                                                wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    ((GetRaygorStyle() == RaygorStyle::Modern) ? wxString{ L"(+)" } : wxString{})));
        GetBottomXAxis().SetDisplayInterval(2, 1);
        GetBottomXAxis().ShowOuterLabels(true);

        // number of sentences axis goes down, unlike the Fry Graph that goes upwards
        GetLeftYAxis().Reverse();
        GetLeftYAxis().AddUnevenAxisPoint(
            3.2, wxNumberFormatter::ToString(3.2, 1, wxNumberFormatter::Style::Style_None) +
                     ((GetRaygorStyle() == RaygorStyle::Modern) ? wxString{ L"(-)" } : wxString{}));
        GetLeftYAxis().AddUnevenAxisPoint(
            3.4, wxNumberFormatter::ToString(3.4, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            3.6, wxNumberFormatter::ToString(3.6, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            3.8, wxNumberFormatter::ToString(3.8, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            4.0, wxNumberFormatter::ToString(4.0, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            4.3, wxNumberFormatter::ToString(4.3, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            4.6, wxNumberFormatter::ToString(4.6, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            4.9, wxNumberFormatter::ToString(4.9, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            5.2, wxNumberFormatter::ToString(5.2, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            5.7, wxNumberFormatter::ToString(5.7, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            6.3, wxNumberFormatter::ToString(6.3, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            6.9, wxNumberFormatter::ToString(6.9, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            7.7, wxNumberFormatter::ToString(7.7, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            9.0, wxNumberFormatter::ToString(9.0, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            10.2, wxNumberFormatter::ToString(10.2, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            12.0, wxNumberFormatter::ToString(12.0, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            13.5, wxNumberFormatter::ToString(13.5, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            16.0, wxNumberFormatter::ToString(16.0, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            19.0, wxNumberFormatter::ToString(19.0, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            23.0, wxNumberFormatter::ToString(23.0, 1, wxNumberFormatter::Style::Style_None));
        GetLeftYAxis().AddUnevenAxisPoint(
            28.0,
            wxNumberFormatter::ToString(28.0, 1, wxNumberFormatter::Style::Style_None) +
                ((GetRaygorStyle() == RaygorStyle::Modern) ? wxString{ L"(+)" } : wxString{}));
        GetLeftYAxis().AdjustRangeToLabels();

        GetLeftYAxis().EnableAutoStacking(false);

        // add the labels for the grades
        if (GetRaygorStyle() == RaygorStyle::Modern)
            {
            AddLevelLabel(LevelLabel(8.4, 7.0, L"3", 3, 3));
            AddLevelLabel(LevelLabel(10.7, 6.0, L"4", 4, 4));
            AddLevelLabel(LevelLabel(12.4, 5.4, L"5", 5, 5));
            AddLevelLabel(LevelLabel(14.4, 5.0, L"6", 6, 6));
            AddLevelLabel(LevelLabel(17.2, 4.575, L"7", 7, 7));
            AddLevelLabel(LevelLabel(21.3, 4, L"8", 8, 8));
            AddLevelLabel(LevelLabel(24, 3.8, L"9", 9, 9));
            AddLevelLabel(LevelLabel(26.4, 3.675, L"10", 10, 10));
            AddLevelLabel(LevelLabel(28.8, 3.575, L"11", 11, 11));
            AddLevelLabel(LevelLabel(31.2, 3.5, L"12", 12, 12));
            AddLevelLabel(LevelLabel(34.5, 3.45, _(L"College"), 13, 16));
            AddLevelLabel(LevelLabel(40, 3.4, _(L"Professor"), 17, 17));
            }
        else
            {
            AddLevelLabel(LevelLabel(17, 9.725, L"3", 3, 3));
            AddLevelLabel(LevelLabel(19, 8.4, L"4", 4, 4));
            AddLevelLabel(LevelLabel(20.5, 7.6, L"5", 5, 5));
            AddLevelLabel(LevelLabel(22, 6.9, L"6", 6, 6));
            AddLevelLabel(LevelLabel(24.5, 6.2, L"7", 7, 7));
            AddLevelLabel(LevelLabel(27, 5.55, L"8", 8, 8));
            AddLevelLabel(LevelLabel(29, 5.2, L"9", 9, 9));
            AddLevelLabel(LevelLabel(30.75, 5, L"10", 10, 10));
            AddLevelLabel(LevelLabel(32.5, 4.825, L"11", 11, 11));
            AddLevelLabel(LevelLabel(34.5, 4.65, L"12", 12, 12));
            AddLevelLabel(LevelLabel(37, 4.45,
                                     /* TRANSLATORS: Abbreviation for 'College' level of reading */
                                     _(L"COL"), 13, 16));
            AddLevelLabel(LevelLabel(42, 4.2,
                                     /* TRANSLATORS: Abbreviation for 'Professor', referring to post
                                        college level of reading */
                                     _(L"PROF"), 17, 17));
            }
        }

    //----------------------------------------------------------------
    void RaygorGraph::SetData(const std::shared_ptr<Wisteria::Data::Dataset>& data,
                              const wxString& numberOfWordsColumnName,
                              const wxString& numberOf6PlusCharWordsColumnName,
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
        m_numberOf6PlusCharWordsColumn = numberOf6PlusCharWordsColumnName;
        m_numberOfSentencesColumn = numberOfSentencesColumnName;

        // if grouping, build the list of group IDs, sorted by their respective labels
        if (IsUsingGrouping())
            {
            BuildGroupIdMap();
            }

        BuildBackscreen();
        }

    //----------------------------------------------------------------
    void RaygorGraph::BuildBackscreen()
        {
        if (m_backscreen == nullptr)
            {
            // Shape and color schemes are irrelevant, as this is used
            // only for the base polygon calculations.
            m_backscreen = std::make_unique<RaygorGraph>(GetCanvas());
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
    void RaygorGraph::RecalcSizes(wxDC & dc)
        {
        Graph2D::RecalcSizes(dc);

        assert((!m_backscreen || m_backscreen->GetBoundingBox(dc).GetWidth() ==
                                     Canvas::GetDefaultCanvasWidthDIPs()) &&
               L"Invalid backscreen size!");
        assert((!m_backscreen || m_backscreen->GetBoundingBox(dc).GetHeight() ==
                                     Canvas::GetDefaultCanvasHeightDIPs()) &&
               L"Invalid backscreen size!");

        const wxColour labelFontColor{ GetLeftYAxis().GetFontColor() };

        // long sentence regions
        GetPhysicalCoordinates(6, 5.5, m_longSentencesPoints[0]);
        GetPhysicalCoordinates(6.8, 5.3, m_longSentencesPoints[1]);
        GetPhysicalCoordinates(9, 4.9, m_longSentencesPoints[2]);
        GetPhysicalCoordinates(11, 4.6, m_longSentencesPoints[3]);
        GetPhysicalCoordinates(15.8, 4.0, m_longSentencesPoints[4]);
        GetPhysicalCoordinates(20.4, 3.6, m_longSentencesPoints[5]);
        GetPhysicalCoordinates(27, 3.2, m_longSentencesPoints[6]);
        GetPhysicalCoordinates(6, 3.2, m_longSentencesPoints[7]);
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo(_(L"Invalid region: sentences are too long"))
                .Pen(wxNullPen)
                .Brush(wxBrush(Colors::ColorContrast::ChangeOpacity(GetInvalidAreaColor(), 100)))
                .Show((GetRaygorStyle() != RaygorStyle::Original)),
            m_longSentencesPoints));
        // long word region
        GetPhysicalCoordinates(18, 28.0, m_longWordPoints[0]);
        GetPhysicalCoordinates(20, 22.6, m_longWordPoints[1]);
        GetPhysicalCoordinates(22, 18.6, m_longWordPoints[2]);
        GetPhysicalCoordinates(24, 15.9, m_longWordPoints[3]);
        GetPhysicalCoordinates(26, 13.8, m_longWordPoints[4]);
        GetPhysicalCoordinates(32, 10.4, m_longWordPoints[5]);
        GetPhysicalCoordinates(36, 9.0, m_longWordPoints[6]);
        GetPhysicalCoordinates(40, 7.9, m_longWordPoints[7]);
        GetPhysicalCoordinates(44, 7.3, m_longWordPoints[8]);
        GetPhysicalCoordinates(44, 28.0, m_longWordPoints[9]);
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo(_(L"Invalid region: too many long words"))
                .Pen(wxNullPen)
                .Brush(wxBrush(Colors::ColorContrast::ChangeOpacity(GetInvalidAreaColor(), 100)))
                .Show((GetRaygorStyle() != RaygorStyle::Original)),
            m_longWordPoints));
        // divider line
        GetPhysicalCoordinates(6, 28, m_dividerLinePoints[0]); // not drawn, used for point region
        GetPhysicalCoordinates(16, 9.6, m_dividerLinePoints[1]);
        GetPhysicalCoordinates(20, 7.0, m_dividerLinePoints[2]);
        GetPhysicalCoordinates(24, 5.8, m_dividerLinePoints[3]);
        GetPhysicalCoordinates(28, 5.05, m_dividerLinePoints[4]);
        GetPhysicalCoordinates(32, 4.6, m_dividerLinePoints[5]);
        GetPhysicalCoordinates(36, 4.3, m_dividerLinePoints[6]);
        GetPhysicalCoordinates(40, 4.05, m_dividerLinePoints[7]);
        GetPhysicalCoordinates(44, 3.9, m_dividerLinePoints[8]);
        // the following are not drawn, just used for calculating information
        GetPhysicalCoordinates(44, 28, m_dividerLinePoints[9]);

        // grade lines
        //------------
        // lower area
        GetPhysicalCoordinates(6, 6.3, m_gradeLinePoints[0]);
        GetPhysicalCoordinates(6, 28.0, m_gradeLinePoints[1]);
        GetPhysicalCoordinates(16, 28.0, m_gradeLinePoints[2]);
        // 3rd grade (end of region)
        GetPhysicalCoordinates(24, 12.5, m_gradeLinePoints[3]);
        GetPhysicalCoordinates(7.5, 5.8, m_gradeLinePoints[4]);
        // 4th grade
        GetPhysicalCoordinates(9.8, 5.3, m_gradeLinePoints[5]);
        GetPhysicalCoordinates(26, 11.7, m_gradeLinePoints[6]);
        // 5th grade
        GetPhysicalCoordinates(26.8, 10.3, m_gradeLinePoints[7]);
        GetPhysicalCoordinates(11.5, 4.95, m_gradeLinePoints[8]);
        // 6th grade
        GetPhysicalCoordinates(13.5, 4.6, m_gradeLinePoints[9]);
        GetPhysicalCoordinates(30, 9.8, m_gradeLinePoints[10]);
        // 7th grade
        GetPhysicalCoordinates(31.5, 9.0, m_gradeLinePoints[11]);
        GetPhysicalCoordinates(18.4, 4, m_gradeLinePoints[12]);
        // 8th grade
        GetPhysicalCoordinates(22.8, 3.8, m_gradeLinePoints[13]);
        GetPhysicalCoordinates(33.2, 8.5, m_gradeLinePoints[14]);
        // 9th grade
        GetPhysicalCoordinates(34.3, 8.0, m_gradeLinePoints[15]);
        GetPhysicalCoordinates(24.8, 3.7, m_gradeLinePoints[16]);
        // 10th grade
        GetPhysicalCoordinates(26.8, 3.5, m_gradeLinePoints[17]);
        GetPhysicalCoordinates(36, 7.7, m_gradeLinePoints[18]);
        // 11th grade
        GetPhysicalCoordinates(38, 7.6, m_gradeLinePoints[19]);
        GetPhysicalCoordinates(29.5, 3.4, m_gradeLinePoints[20]);
        // 12th grade
        GetPhysicalCoordinates(32, 3.3, m_gradeLinePoints[21]);
        GetPhysicalCoordinates(39.4, 7.2, m_gradeLinePoints[22]);
        // 13th grade
        GetPhysicalCoordinates(42, 6.6, m_gradeLinePoints[23]);
        GetPhysicalCoordinates(36.1, 3.2, m_gradeLinePoints[24]);
        // beyond 13th region
        GetPhysicalCoordinates(44, 3.2, m_gradeLinePoints[25]);
        GetPhysicalCoordinates(44, 6.3, m_gradeLinePoints[26]);

        assert(GetMessageCatalog() && L"Label manager not set in Raygor Graph!");

        // draw the labels in the "danger areas"
        wxPoint topCorner;
        const wxFont dangerAreaFont =
            (GetRaygorStyle() == RaygorStyle::Modern) ?
                wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFractionalPointSize() *
                           1.75,
                       wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false,
                       GetFancyFontFaceName()) :
                wxFont(wxFontInfo().Family(wxFONTFAMILY_SWISS).Bold()).MakeLarger().MakeLarger();
            // draw the "Long Words" area label
            {
            GetPhysicalCoordinates(31, 14.5, topCorner);
            auto invalidLabel = std::make_unique<GraphItems::Label>(
                GraphItems::GraphItemInfo(
                    /* TRANSLATORS: Uppercasing is just for display */ _(L"INVALID"))
                    .Scaling(GetScaling())
                    .Pen(wxNullPen)
                    .Font(dangerAreaFont)
                    .FontColor(labelFontColor)
                    .AnchorPoint(topCorner));
            invalidLabel->SetAnchoring(Wisteria::Anchoring::TopLeftCorner);
            AddObject(std::move(invalidLabel));
            }

            // draw the "Long Sentences" area label
            {
            GetPhysicalCoordinates(7, 3.5, topCorner);
            auto invalidLabel = std::make_unique<GraphItems::Label>(
                GraphItems::GraphItemInfo(
                    /* TRANSLATORS: Uppercasing is just for display */ _(L"INVALID"))
                    .Scaling(GetScaling())
                    .Pen(wxNullPen)
                    .Font(dangerAreaFont)
                    .FontColor(labelFontColor)
                    .AnchorPoint(topCorner));
            invalidLabel->SetAnchoring(Wisteria::Anchoring::TopLeftCorner);
            AddObject(std::move(invalidLabel));
            }

        // middle points on the separator line
        wxColour separatorColor{ Colors::ColorContrast::ChangeOpacity(
            Wisteria::Colors::ColorBrewer::GetColor(Wisteria::Colors::Color::Black), 200) };
        separatorColor =
            Colors::ColorContrast::ShadeOrTintIfClose(separatorColor, GetPlotOrCanvasColor());
        wxPoint dividerPt;
        auto splinePoints = std::make_unique<GraphItems::Points2D>(wxNullPen);
        splinePoints->SetScaling(GetScaling());
        splinePoints->SetDPIScaleFactor(GetDPIScaleFactor());
        GetPhysicalCoordinates(18, 8.1, dividerPt);
        splinePoints->AddPoint(
            GraphItems::Point2D(
                GraphItems::GraphItemInfo{}
                    .AnchorPoint(dividerPt)
                    .Brush(separatorColor)
                    .Pen(Colors::ColorContrast::BlackOrWhiteContrast(GetPlotOrCanvasColor())),
                Settings::GetPointRadius() / 2, Icons::IconShape::Circle),
            dc);

        GetPhysicalCoordinates(19.5, 7.28, dividerPt);
        splinePoints->AddPoint(
            GraphItems::Point2D(
                GraphItems::GraphItemInfo{}
                    .AnchorPoint(dividerPt)
                    .Brush(separatorColor)
                    .Pen(Colors::ColorContrast::BlackOrWhiteContrast(GetPlotOrCanvasColor())),
                Settings::GetPointRadius() / 2, Icons::IconShape::Circle),
            dc);

        GetPhysicalCoordinates(21, 6.69, dividerPt);
        splinePoints->AddPoint(
            GraphItems::Point2D(
                GraphItems::GraphItemInfo{}
                    .AnchorPoint(dividerPt)
                    .Brush(separatorColor)
                    .Pen(Colors::ColorContrast::BlackOrWhiteContrast(GetPlotOrCanvasColor())),
                Settings::GetPointRadius() / 2, Icons::IconShape::Circle),
            dc);

        GetPhysicalCoordinates(23.7, 5.91, dividerPt);
        splinePoints->AddPoint(
            GraphItems::Point2D(
                GraphItems::GraphItemInfo{}
                    .AnchorPoint(dividerPt)
                    .Scaling(GetScaling())
                    .Brush(separatorColor)
                    .Pen(Colors::ColorContrast::BlackOrWhiteContrast(GetPlotOrCanvasColor())),
                Settings::GetPointRadius() / 2, Icons::IconShape::Circle),
            dc);

        GetPhysicalCoordinates(26.5, 5.26, dividerPt);
        splinePoints->AddPoint(
            GraphItems::Point2D(
                GraphItems::GraphItemInfo{}
                    .AnchorPoint(dividerPt)
                    .Brush(separatorColor)
                    .Pen(Colors::ColorContrast::BlackOrWhiteContrast(GetPlotOrCanvasColor())),
                Settings::GetPointRadius() / 2, Icons::IconShape::Circle),
            dc);

        GetPhysicalCoordinates(28.5, 5, dividerPt);
        splinePoints->AddPoint(
            GraphItems::Point2D(
                GraphItems::GraphItemInfo{}
                    .AnchorPoint(dividerPt)
                    .Brush(separatorColor)
                    .Pen(Colors::ColorContrast::BlackOrWhiteContrast(GetPlotOrCanvasColor())),
                Settings::GetPointRadius() / 2, Icons::IconShape::Circle),
            dc);

        GetPhysicalCoordinates(30.1, 4.82, dividerPt);
        splinePoints->AddPoint(
            GraphItems::Point2D(
                GraphItems::GraphItemInfo{}
                    .AnchorPoint(dividerPt)
                    .Brush(separatorColor)
                    .Pen(Colors::ColorContrast::BlackOrWhiteContrast(GetPlotOrCanvasColor())),
                Settings::GetPointRadius() / 2, Icons::IconShape::Circle),
            dc);

        GetPhysicalCoordinates(32.2, 4.6, dividerPt);
        splinePoints->AddPoint(
            GraphItems::Point2D(
                GraphItems::GraphItemInfo{}
                    .AnchorPoint(dividerPt)
                    .Brush(separatorColor)
                    .Pen(Colors::ColorContrast::BlackOrWhiteContrast(GetPlotOrCanvasColor())),
                Settings::GetPointRadius() / 2, Icons::IconShape::Circle),
            dc);

        GetPhysicalCoordinates(34.2, 4.44, dividerPt);
        splinePoints->AddPoint(
            GraphItems::Point2D(
                GraphItems::GraphItemInfo{}
                    .AnchorPoint(dividerPt)
                    .Brush(separatorColor)
                    .Pen(Colors::ColorContrast::BlackOrWhiteContrast(GetPlotOrCanvasColor())),
                Settings::GetPointRadius() / 2, Icons::IconShape::Circle),
            dc);

        GetPhysicalCoordinates(37, 4.25, dividerPt);
        splinePoints->AddPoint(
            GraphItems::Point2D(
                GraphItems::GraphItemInfo{}
                    .AnchorPoint(dividerPt)
                    .Brush(separatorColor)
                    .Pen(Colors::ColorContrast::BlackOrWhiteContrast(GetPlotOrCanvasColor())),
                Settings::GetPointRadius() / 2, Icons::IconShape::Circle),
            dc);

        AddObject(std::move(splinePoints));

        auto levelsSpline = std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxPen{ separatorColor })
                .Brush(wxBrush{ separatorColor })
                .Selectable(false)
                .Scaling(GetScaling())
                .DPIScaling(GetDPIScaleFactor()),
            std::span(std::next(m_dividerLinePoints.cbegin(), 1), m_dividerLinePoints.size() - 2));
        levelsSpline->SetShape(GraphItems::Polygon::PolygonShape::Spline);
        AddObject(std::move(levelsSpline));

        // Draw the grade lines
        //---------------------
        // 3rd grade
        wxColour gradeLineColor{ Colors::ColorContrast::IsDark(GetPlotOrCanvasColor()) ?
                                     Colors::ColorBrewer::GetColor(Colors::Color::BondiBlue) :
                                     Colors::ColorBrewer::GetColor(Colors::Color::Blue) };
        gradeLineColor = Colors::ColorContrast::ChangeOpacity(gradeLineColor, 200);
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(gradeLineColor)
                .Brush(gradeLineColor)
                .Scaling(GetScaling()),
            std::span(std::next(m_gradeLinePoints.cbegin(), 3), 2)));
        // the rest of the grade lines
        for (size_t i = 4, pointIter = 5; i <= 12; ++i, pointIter += 2)
            {
            AddObject(std::make_unique<GraphItems::Polygon>(
                GraphItems::GraphItemInfo{}
                    .Pen(gradeLineColor)
                    .Brush(gradeLineColor)
                    .Scaling(GetScaling()),
                std::span(std::next(m_gradeLinePoints.cbegin(), pointIter), 2)));
            }
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(gradeLineColor)
                .Brush(gradeLineColor)
                .Scaling(GetScaling()),
            std::span(std::next(m_gradeLinePoints.cbegin(), 23), 2)));

        const wxBrush selectionBrush = wxBrush(Colors::ColorContrast::ChangeOpacity(
            Colors::ColorBrewer::GetColor(Colors::Color::LightGray), 100));

        // draw the grade areas (for selecting)
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo(GetMessageCatalog()->GetGradeScaleLongLabel(3))
                .Pen(wxNullPen)
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            std::span(m_gradeLinePoints.cbegin(), 5)));
        // the rest of the grade areas
        for (size_t i = 4, pointIter = 5; i <= 12; ++i, pointIter += 2)
            {
            AddObject(std::make_unique<GraphItems::Polygon>(
                GraphItems::GraphItemInfo(GetMessageCatalog()->GetGradeScaleLongLabel(i))
                    .Pen(wxNullPen)
                    .Brush(wxNullBrush)
                    .SelectionBrush(selectionBrush),
                std::span(std::next(m_gradeLinePoints.cbegin(), (pointIter - 2)), 4)));
            }
        // last labels are a little special because they are more vast
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo(_(L"College"))
                .Pen(wxNullPen)
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            std::span(std::next(m_gradeLinePoints.cbegin(), 21), 4)));
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo(_(L"Post College (e.g., Professor)"))
                .Pen(wxNullPen)
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            std::span(std::next(m_gradeLinePoints.cbegin(), 23), 4)));

        CalculateScorePositions(dc);

        // Add the grade labels to the regions
        // (and highlight in heavy bold and a different color the one where the score lies)
        const wxFont labelFont =
            (GetRaygorStyle() == RaygorStyle::Modern) ?
                wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFractionalPointSize() *
                           1.25,
                       wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false,
                       GetFancyFontFaceName()) :
                wxFont(wxFontInfo().Family(wxFONTFAMILY_SWISS)).MakeLarger();
        for (const auto& level : GetLevelLabels())
            {
            wxPoint point;
            GetPhysicalCoordinates(level.GetX(), level.GetY(), point);

            auto levelLabel = std::make_unique<GraphItems::Label>(
                GraphItems::GraphItemInfo(level.GetLabel())
                    .Scaling(GetScaling())
                    .Pen(wxNullPen)
                    .Font((GetRaygorStyle() == RaygorStyle::Modern) ? labelFont :
                                                                      // make "COL" and "PROF" bold
                              (level.GetLabel().length() > 2) ? labelFont.Bold() :
                                                                labelFont)
                    .FontColor(labelFontColor)
                    .AnchorPoint(point));
            levelLabel->SetTextAlignment(TextAlignment::Centered);
            if (GetRaygorStyle() == RaygorStyle::Modern && GetScores().size() == 1 &&
                level == GetScores().front().GetScore())
                {
                levelLabel->GetFont().SetWeight(wxFontWeight::wxFONTWEIGHT_EXTRAHEAVY);
                levelLabel->SetFontColor(GetColorScheme()->GetColor(0));
                }
            AddObject(std::move(levelLabel));
            }
        }

    //----------------------------------------------------------------
    void RaygorGraph::CalculateScorePositions(wxDC & dc)
        {
        if (GetDataset() == nullptr)
            {
            return;
            }
        const auto numberOfWordsColumn = GetContinuousColumn(m_numberOfWordsColumn);
        const auto numberOf6PlusCharWordsColumn =
            GetContinuousColumn(m_numberOf6PlusCharWordsColumn);
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
                std::clamp<double>(normalizationFactor * numberOf6PlusCharWordsColumn->GetValue(i),
                                   6, 44),
                std::clamp<double>(normalizationFactor * numberOfSentencesColumn->GetValue(i), 3.2,
                                   28));

            m_results[i].ResetStatus();

            const auto calcScoreFromPolygons =
                [](const std::unique_ptr<RaygorGraph>& graph, ScorePoint& scorePoint)
            {
                // see where the point is
                if (!graph->GetPhysicalCoordinates(
                        round_decimal_place(scorePoint.m_wordStatistic, 100),
                        round_decimal_place(scorePoint.m_sentenceStatistic, 100),
                        scorePoint.m_scorePoint))
                    {
                    scorePoint.SetScoreInvalid(true);
                    return false;
                    }

                if (IsScoreInsideRegion(
                        scorePoint.m_scorePoint,
                        std::span(std::next(graph->m_gradeLinePoints.cbegin(), 23), 4), 1, -1))
                    {
                    scorePoint.SetScore(17);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 21), 4), 1, -1))
                    {
                    scorePoint.SetScore(13);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 19), 4), 1, -1))
                    {
                    scorePoint.SetScore(12);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 17), 4), 1, -1))
                    {
                    scorePoint.SetScore(11);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 15), 4), 1, -1))
                    {
                    scorePoint.SetScore(10);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 13), 4), 1, -1))
                    {
                    scorePoint.SetScore(9);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 11), 4), 1, -1))
                    {
                    scorePoint.SetScore(8);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 9), 4), 1, -1))
                    {
                    scorePoint.SetScore(7);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 7), 4), 1, -1))
                    {
                    scorePoint.SetScore(6);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 5), 4), 1, -1))
                    {
                    scorePoint.SetScore(5);
                    }
                else if (IsScoreInsideRegion(
                             scorePoint.m_scorePoint,
                             std::span(std::next(graph->m_gradeLinePoints.cbegin(), 3), 4), 1, -1))
                    {
                    scorePoint.SetScore(4);
                    }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint,
                                             std::span(graph->m_gradeLinePoints.cbegin(), 5), 1,
                                             -1))
                    {
                    scorePoint.SetScore(3);
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
                                            -1))
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
            if (GetPhysicalCoordinates(round_decimal_place(m_results[i].m_wordStatistic, 100),
                                       round_decimal_place(m_results[i].m_sentenceStatistic, 100),
                                       m_results[i].m_scorePoint))
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
    } // namespace Wisteria::Graphs
