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

#include "schwartzgraph.h"

wxIMPLEMENT_DYNAMIC_CLASS(Wisteria::Graphs::SchwartzGraph,
                          Wisteria::Graphs::PolygonReadabilityGraph)

    namespace Wisteria::Graphs
    {
    //----------------------------------------------------------------
    SchwartzGraph::SchwartzGraph(
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

        GetCanvas()->SetLabel(_(L"Schwartz Graph"));
        GetCanvas()->SetName(_(L"Schwartz Graph"));

        // number of sentences axis goes down, unlike the Fry Graph that goes upwards
        GetLeftYAxis().Reverse();
        GetLeftYAxis().SetRange(2.4, 20, 1, 0.4, 1);
        GetLeftYAxis().ShowOuterLabels(true);
        GetLeftYAxis().EnableAutoStacking(true);
        if (GetLeftYAxis().GetGridlinePen().IsOk())
            {
            GetLeftYAxis().GetGridlinePen().SetColour(wxColour(211, 211, 211, 75));
            }

        GetBottomXAxis().SetRange(125, 189, 0, 1, 1);
        // The axis points are displayed with the 100 chopped off of it
        // (so 125 is displayed as 25).
        // This is an "optimization" to fit more labels on the axis.
        // (Not my design, that's how the published graph is designed).
        for (const auto& label : GetBottomXAxis().GetAxisPoints())
            {
            GetBottomXAxis().SetCustomLabel(
                label.GetValue(),
                GraphItems::Label(wxString::FromDouble(label.GetValue() - 100, 0)));
            }
        GetBottomXAxis().ShowOuterLabels(true);
        GetBottomXAxis().GetFooter().SetText(_DT(L"100+", DTExplanation::Constant));
        GetBottomXAxis().GetFooter().SetTextAlignment(TextAlignment::FlushRight);
        GetBottomXAxis().GetFooter().SetRelativeAlignment(Wisteria::RelativeAlignment::FlushBottom);
        GetBottomXAxis().GetFooter().GetPen() = wxNullPen;
        GetBottomXAxis().GetFooter().SetLabelStyle(LabelStyle::RightArrowLinedPaperWithMargins);
        GetBottomXAxis().GetFooter().SetPadding(0, 25, 5, 5);
        if (GetBottomXAxis().GetGridlinePen().IsOk())
            {
            GetBottomXAxis().GetGridlinePen().SetColour(wxColour(211, 211, 211, 75));
            }

        // add the labels for the grades
        AddLevelLabel(LevelLabel(129, 11, _DT(L"1"), 1, 1));
        AddLevelLabel(LevelLabel(135, 5.6, L"2", 2, 2));
        AddLevelLabel(LevelLabel(143, 3.2, L"3/4", 3, 4));
        AddLevelLabel(LevelLabel(156, 2.5, L"5/6", 5, 6));
        AddLevelLabel(LevelLabel(169, 2.5, L"7/8", 7, 7.9)); // really like 7th to lower 8th grade
        AddLevelLabel(LevelLabel(181, 2.5, L"8+", 8, 8));    // advanced 8th grade and beyond
        }

    //----------------------------------------------------------------
    void SchwartzGraph::SetData(const std::shared_ptr<Wisteria::Data::Dataset>& data,
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
    void SchwartzGraph::BuildBackscreen()
        {
        if (m_backscreen == nullptr)
            {
            // Shape and color schemes are irrelevant, as this is used
            // only for the base polygon calculations.
            m_backscreen = std::make_unique<SchwartzGraph>(GetCanvas());
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
    void SchwartzGraph::RecalcSizes(wxDC & dc)
        {
        // axes footers maintain their own font, so adjust this one
        GetBottomXAxis().GetFooter().GetFont() = GetBottomXAxis().GetFont();
        GetBottomXAxis().GetFooter().SetFontColor(GetBottomXAxis().GetFontColor());

        Graph2D::RecalcSizes(dc);

        const wxColour labelFontColor{ GetLeftYAxis().GetFontColor() };

        assert((!m_backscreen || m_backscreen->GetBoundingBox(dc).GetWidth() ==
                                     Canvas::GetDefaultCanvasWidthDIPs()) &&
               L"Invalid backscreen size!");
        assert((!m_backscreen || m_backscreen->GetBoundingBox(dc).GetHeight() ==
                                     Canvas::GetDefaultCanvasHeightDIPs()) &&
               L"Invalid backscreen size!");

        // long sentence region
        GetPhysicalCoordinates(125, 10.8, m_longSentencesPoints[0]);
        GetPhysicalCoordinates(126.2, 9.6, m_longSentencesPoints[1]);
        GetPhysicalCoordinates(127.2, 8.6, m_longSentencesPoints[2]);
        GetPhysicalCoordinates(128.2, 7.6, m_longSentencesPoints[3]);
        GetPhysicalCoordinates(129.25, 6.8, m_longSentencesPoints[4]);
        GetPhysicalCoordinates(130.3, 6.15, m_longSentencesPoints[5]);
        GetPhysicalCoordinates(131, 5.75, m_longSentencesPoints[6]);
        GetPhysicalCoordinates(132.1, 5.2, m_longSentencesPoints[7]);
        GetPhysicalCoordinates(132.95, 4.8, m_longSentencesPoints[8]);
        GetPhysicalCoordinates(133.9, 4.4, m_longSentencesPoints[9]);
        GetPhysicalCoordinates(135, 4, m_longSentencesPoints[10]);
        GetPhysicalCoordinates(136.85, 3.45, m_longSentencesPoints[11]);
        GetPhysicalCoordinates(137.85, 3.2, m_longSentencesPoints[12]);
        GetPhysicalCoordinates(139.8, 2.7, m_longSentencesPoints[13]);
        GetPhysicalCoordinates(141, 2.4, m_longSentencesPoints[14]);
        GetPhysicalCoordinates(125, 2.4, m_longSentencesPoints[15]);

        auto invalidSentenceArea = std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxNullPen)
                .Brush(wxBrush(GetInvalidAreaColor(), wxBRUSHSTYLE_FDIAGONAL_HATCH))
                .Text(_(L"Invalid region: sentences are too long")),
            m_longSentencesPoints);
        invalidSentenceArea->SetBackgroundFill(
            Colors::GradientFill(Colors::ColorBrewer::GetColor(Colors::Color::White)));
        AddObject(std::move(invalidSentenceArea));

        // long word region
        GetPhysicalCoordinates(160.5, 20.0, m_longWordPoints[0]);
        GetPhysicalCoordinates(161, 19.2, m_longWordPoints[1]);
        GetPhysicalCoordinates(162, 17.6, m_longWordPoints[2]);
        GetPhysicalCoordinates(163.1, 16.45, m_longWordPoints[3]);
        GetPhysicalCoordinates(164, 15.6, m_longWordPoints[4]);
        GetPhysicalCoordinates(165.1, 14.8, m_longWordPoints[5]);
        GetPhysicalCoordinates(166.1, 14.2, m_longWordPoints[6]);
        GetPhysicalCoordinates(167, 13.7, m_longWordPoints[7]);
        GetPhysicalCoordinates(168, 13.3, m_longWordPoints[8]);
        GetPhysicalCoordinates(169, 12.9, m_longWordPoints[9]);
        GetPhysicalCoordinates(170.1, 12.5, m_longWordPoints[10]);
        GetPhysicalCoordinates(171, 12.2, m_longWordPoints[11]);
        GetPhysicalCoordinates(172, 11.9, m_longWordPoints[12]);
        GetPhysicalCoordinates(173, 11.64, m_longWordPoints[13]);
        GetPhysicalCoordinates(174, 11.4, m_longWordPoints[14]);
        GetPhysicalCoordinates(174.9, 11.22, m_longWordPoints[15]);
        GetPhysicalCoordinates(176, 11.0, m_longWordPoints[16]);
        GetPhysicalCoordinates(177, 10.825, m_longWordPoints[17]);
        GetPhysicalCoordinates(178, 10.7, m_longWordPoints[18]);
        GetPhysicalCoordinates(178.8, 10.6, m_longWordPoints[19]);
        GetPhysicalCoordinates(180, 10.45, m_longWordPoints[20]);
        GetPhysicalCoordinates(181, 10.4, m_longWordPoints[21]);
        GetPhysicalCoordinates(182, 10.35, m_longWordPoints[22]);
        GetPhysicalCoordinates(183, 10.32, m_longWordPoints[23]);
        GetPhysicalCoordinates(184, 10.3, m_longWordPoints[24]);
        GetPhysicalCoordinates(185, 10.28, m_longWordPoints[25]);
        GetPhysicalCoordinates(186, 10.26, m_longWordPoints[26]);
        GetPhysicalCoordinates(187, 10.26, m_longWordPoints[27]);
        GetPhysicalCoordinates(188, 10.25, m_longWordPoints[28]);
        GetPhysicalCoordinates(189, 10.24, m_longWordPoints[29]);
        GetPhysicalCoordinates(189, 20.0, m_longWordPoints[30]);

        auto invalidWordArea = std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxNullPen)
                .Brush(wxBrush(GetInvalidAreaColor(), wxBRUSHSTYLE_FDIAGONAL_HATCH))
                .Text(_(L"Invalid region: words are too long")),
            m_longWordPoints);
        invalidWordArea->SetBackgroundFill(
            Colors::GradientFill(Colors::ColorBrewer::GetColor(Colors::Color::White)));
        AddObject(std::move(invalidWordArea));

        // divider line
        GetPhysicalCoordinates(139, 20, m_dividerLinePoints[0]); // not drawn, used for point region
        GetPhysicalCoordinates(144, 15.6, m_dividerLinePoints[1]);
        GetPhysicalCoordinates(152.5, 10.4, m_dividerLinePoints[2]);
        GetPhysicalCoordinates(158, 7.8, m_dividerLinePoints[3]);
        GetPhysicalCoordinates(161, 6.7, m_dividerLinePoints[4]);
        GetPhysicalCoordinates(166, 5.6, m_dividerLinePoints[5]);
        GetPhysicalCoordinates(173, 5.2, m_dividerLinePoints[6]);
        // the following are not drawn, just used for calculating information
        GetPhysicalCoordinates(189, 5.2, m_dividerLinePoints[7]);
        GetPhysicalCoordinates(189, 20, m_dividerLinePoints[8]);

        // light grey on selection
        const wxBrush selectionBrush = wxBrush(wxColour(192, 192, 192, 100));

        // grade lines
        // 1st grade
        GetPhysicalCoordinates(125, 10.8, m_gradeLinePoints[0]);
        GetPhysicalCoordinates(125, 20.0, m_gradeLinePoints[1]);
        GetPhysicalCoordinates(160.5, 20.0, m_gradeLinePoints[2]);
        GetPhysicalCoordinates(162.45, 17.1, m_gradeLinePoints[3]);
        GetPhysicalCoordinates(128.2, 7.6, m_gradeLinePoints[4]);

        // 2nd grade
        GetPhysicalCoordinates(168.5, 13.1, m_gradeLinePoints[5]);
        GetPhysicalCoordinates(135, 4, m_gradeLinePoints[6]);

        // 3rd-4th grade
        GetPhysicalCoordinates(148, 2.4, m_gradeLinePoints[7]);
        GetPhysicalCoordinates(170.1, 12.5, m_gradeLinePoints[8]);

        // 5th-6th grade
        GetPhysicalCoordinates(172, 11.9, m_gradeLinePoints[9]);
        GetPhysicalCoordinates(165, 2.4, m_gradeLinePoints[10]);

        // 7th-8th grade
        GetPhysicalCoordinates(177.5, 2.4, m_gradeLinePoints[11]);
        GetPhysicalCoordinates(180, 10.45, m_gradeLinePoints[12]);

        // 8th grade+
        GetPhysicalCoordinates(189, 10.24, m_gradeLinePoints[13]);
        GetPhysicalCoordinates(189, 2.4, m_gradeLinePoints[14]);

        assert(GetMessageCatalog() && L"Label manager not set in Schwartz Graph!");

        // 1st grade polygon
        GetPhysicalCoordinates(125, 20.0, m_grade1Polygon[0]);
        GetPhysicalCoordinates(160.5, 20.0, m_grade1Polygon[1]);
        GetPhysicalCoordinates(161, 19.2, m_grade1Polygon[2]);
        GetPhysicalCoordinates(162, 17.6, m_grade1Polygon[3]);
        GetPhysicalCoordinates(162.45, 17.1, m_grade1Polygon[4]);
        GetPhysicalCoordinates(128.2, 7.6, m_grade1Polygon[5]);
        GetPhysicalCoordinates(126.2, 9.6, m_grade1Polygon[6]);
        GetPhysicalCoordinates(127.2, 8.6, m_grade1Polygon[7]);
        GetPhysicalCoordinates(125, 10.8, m_grade1Polygon[8]);
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo(GetMessageCatalog()->GetGradeScaleLongLabel(1))
                .Pen(wxNullPen)
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            m_grade1Polygon));
        // 2nd grade polygon
        GetPhysicalCoordinates(162.45, 17.1, m_grade2Polygon[0]);
        GetPhysicalCoordinates(163.1, 16.45, m_grade2Polygon[1]);
        GetPhysicalCoordinates(164, 15.6, m_grade2Polygon[2]);
        GetPhysicalCoordinates(165.1, 14.8, m_grade2Polygon[3]);
        GetPhysicalCoordinates(166.1, 14.2, m_grade2Polygon[4]);
        GetPhysicalCoordinates(167, 13.7, m_grade2Polygon[5]);
        GetPhysicalCoordinates(168, 13.3, m_grade2Polygon[6]);
        GetPhysicalCoordinates(168.5, 13.1, m_grade2Polygon[7]);
        GetPhysicalCoordinates(135, 4, m_grade2Polygon[8]);
        GetPhysicalCoordinates(133.9, 4.4, m_grade2Polygon[9]);
        GetPhysicalCoordinates(132.95, 4.8, m_grade2Polygon[10]);
        GetPhysicalCoordinates(132.1, 5.2, m_grade2Polygon[11]);
        GetPhysicalCoordinates(131, 5.75, m_grade2Polygon[12]);
        GetPhysicalCoordinates(130.3, 6.15, m_grade2Polygon[13]);
        GetPhysicalCoordinates(129.25, 6.8, m_grade2Polygon[14]);
        GetPhysicalCoordinates(128.2, 7.6, m_grade2Polygon[15]);
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo(GetMessageCatalog()->GetGradeScaleLongLabel(2))
                .Pen(wxNullPen)
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            m_grade2Polygon));
        // 3rd-4th grade polygon
        GetPhysicalCoordinates(168.5, 13.1, m_grade3to4Polygon[0]);
        GetPhysicalCoordinates(169, 12.9, m_grade3to4Polygon[1]);
        GetPhysicalCoordinates(170.1, 12.5, m_grade3to4Polygon[2]);
        GetPhysicalCoordinates(148, 2.4, m_grade3to4Polygon[3]);
        GetPhysicalCoordinates(141, 2.4, m_grade3to4Polygon[4]);
        GetPhysicalCoordinates(139.8, 2.7, m_grade3to4Polygon[5]);
        GetPhysicalCoordinates(137.85, 3.2, m_grade3to4Polygon[6]);
        GetPhysicalCoordinates(136.85, 3.45, m_grade3to4Polygon[7]);
        GetPhysicalCoordinates(135, 4, m_grade3to4Polygon[8]);
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo(
                wxString::Format(L"%s-%s", GetMessageCatalog()->GetGradeScaleLongLabel(3),
                                 GetMessageCatalog()->GetGradeScaleLongLabel(4)))
                .Pen(wxNullPen)
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            m_grade3to4Polygon));
        // 5th-6th grade polygon
        GetPhysicalCoordinates(170.1, 12.5, m_grade5to6Polygon[0]);
        GetPhysicalCoordinates(171, 12.2, m_grade5to6Polygon[1]);
        GetPhysicalCoordinates(172, 11.9, m_grade5to6Polygon[2]);
        GetPhysicalCoordinates(165, 2.4, m_grade5to6Polygon[3]);
        GetPhysicalCoordinates(148, 2.4, m_grade5to6Polygon[4]);
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo(
                wxString::Format(L"%s-%s", GetMessageCatalog()->GetGradeScaleLongLabel(5),
                                 GetMessageCatalog()->GetGradeScaleLongLabel(6)))
                .Pen(wxNullPen)
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            m_grade5to6Polygon));
        // 7th-8th grade polygon
        GetPhysicalCoordinates(172, 11.9, m_grade7to8Polygon[0]);
        GetPhysicalCoordinates(173, 11.64, m_grade7to8Polygon[1]);
        GetPhysicalCoordinates(174, 11.4, m_grade7to8Polygon[2]);
        GetPhysicalCoordinates(174.9, 11.22, m_grade7to8Polygon[3]);
        GetPhysicalCoordinates(176, 11.0, m_grade7to8Polygon[4]);
        GetPhysicalCoordinates(177, 10.825, m_grade7to8Polygon[5]);
        GetPhysicalCoordinates(178, 10.7, m_grade7to8Polygon[6]);
        GetPhysicalCoordinates(178.8, 10.6, m_grade7to8Polygon[7]);
        GetPhysicalCoordinates(180, 10.5, m_grade7to8Polygon[8]);
        GetPhysicalCoordinates(177.5, 2.4, m_grade7to8Polygon[9]);
        GetPhysicalCoordinates(165, 2.4, m_grade7to8Polygon[10]);
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo(
                wxString::Format(L"%s-%s", GetMessageCatalog()->GetGradeScaleLongLabel(7),
                                 GetMessageCatalog()->GetGradeScaleLongLabel(8)))
                .Pen(wxNullPen)
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            m_grade7to8Polygon));
        // 8+ polygon
        GetPhysicalCoordinates(180, 10.5, m_gradeOver8Polygon[0]);
        GetPhysicalCoordinates(181, 10.4, m_gradeOver8Polygon[1]);
        GetPhysicalCoordinates(182, 10.35, m_gradeOver8Polygon[2]);
        GetPhysicalCoordinates(183, 10.32, m_gradeOver8Polygon[3]);
        GetPhysicalCoordinates(184, 10.3, m_gradeOver8Polygon[4]);
        GetPhysicalCoordinates(185, 10.28, m_gradeOver8Polygon[5]);
        GetPhysicalCoordinates(186, 10.26, m_gradeOver8Polygon[6]);
        GetPhysicalCoordinates(187, 10.26, m_gradeOver8Polygon[7]);
        GetPhysicalCoordinates(188, 10.25, m_gradeOver8Polygon[8]);
        GetPhysicalCoordinates(189, 10.24, m_gradeOver8Polygon[9]);
        GetPhysicalCoordinates(189, 2.4, m_gradeOver8Polygon[10]);
        GetPhysicalCoordinates(177.5, 2.4, m_gradeOver8Polygon[11]);
        AddObject(std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo(GetMessageCatalog()->GetGradeScaleLongLabel(8) + _DT(L"+"))
                .Pen(wxNullPen)
                .Brush(wxNullBrush)
                .SelectionBrush(selectionBrush),
            m_gradeOver8Polygon));

        // middle points on the separator line
        wxColour separatorColor{ Colors::ColorContrast::ChangeOpacity(
            Wisteria::Colors::ColorBrewer::GetColor(Wisteria::Colors::Color::Black), 200) };
        separatorColor =
            Colors::ColorContrast::ShadeOrTintIfClose(separatorColor, GetPlotOrCanvasColor());
        auto linePoints = std::make_unique<GraphItems::Points2D>(wxNullPen);
        linePoints->SetScaling(GetScaling());
        linePoints->SetDPIScaleFactor(GetDPIScaleFactor());
        linePoints->AddPoint(GraphItems::Point2D(GraphItems::GraphItemInfo{}
                                                     .AnchorPoint(m_dividerLinePoints[1])
                                                     .Brush(separatorColor),
                                                 Settings::GetPointRadius(),
                                                 Icons::IconShape::Circle),
                             dc);
        linePoints->AddPoint(GraphItems::Point2D(GraphItems::GraphItemInfo{}
                                                     .AnchorPoint(m_dividerLinePoints[2])
                                                     .Brush(separatorColor),
                                                 Settings::GetPointRadius(),
                                                 Icons::IconShape::Circle),
                             dc);
        linePoints->AddPoint(GraphItems::Point2D(GraphItems::GraphItemInfo{}
                                                     .AnchorPoint(m_dividerLinePoints[3])
                                                     .Brush(separatorColor),
                                                 Settings::GetPointRadius(),
                                                 Icons::IconShape::Circle),
                             dc);
        linePoints->AddPoint(GraphItems::Point2D(GraphItems::GraphItemInfo{}
                                                     .AnchorPoint(m_dividerLinePoints[4])
                                                     .Brush(separatorColor),
                                                 Settings::GetPointRadius(),
                                                 Icons::IconShape::Circle),
                             dc);
        // index [5] is just added to the spline to curve it better,
        // not used for a middle point
        linePoints->AddPoint(GraphItems::Point2D(GraphItems::GraphItemInfo{}
                                                     .AnchorPoint(m_dividerLinePoints[6])
                                                     .Brush(separatorColor),
                                                 Settings::GetPointRadius(),
                                                 Icons::IconShape::Circle),
                             dc);
        AddObject(std::move(linePoints));

        // separator line
        auto levelsSpline = std::make_unique<GraphItems::Polygon>(
            GraphItems::GraphItemInfo{}
                .Pen(wxPen{ separatorColor })
                .Brush(wxBrush{ separatorColor })
                .Selectable(false)
                .Scaling(GetScaling())
                .DPIScaling(GetDPIScaleFactor()),
            std::span(std::next(m_dividerLinePoints.cbegin(), 1), m_dividerLinePoints.size() - 3));
        levelsSpline->SetShape(GraphItems::Polygon::PolygonShape::Spline);
        AddObject(std::move(levelsSpline));

        // inner lines of polygons
        // (can't draw outline around full polygon because it overlaps the axis)
        auto polySeparatorLineWords =
            std::make_unique<GraphItems::Lines>(wxPen{ L"#ECECEC" }, GetScaling());
        for (size_t i = 0; i < m_longWordPoints.size() - 2; ++i)
            {
            polySeparatorLineWords->AddLine(m_longWordPoints[i], m_longWordPoints[i + 1]);
            }
        AddObject(std::move(polySeparatorLineWords));

        auto polySeparatorLineWordsAxis =
            std::make_unique<GraphItems::Lines>(GetLeftYAxis().GetPen(), GetScaling());
        polySeparatorLineWordsAxis->AddLine(m_longWordPoints[m_longWordPoints.size() - 2],
                                            m_longWordPoints[m_longWordPoints.size() - 1]);
        polySeparatorLineWordsAxis->AddLine(m_longWordPoints[m_longWordPoints.size() - 1],
                                            m_longWordPoints[0]);
        AddObject(std::move(polySeparatorLineWordsAxis));

        auto polySeparatorLineSentences =
            std::make_unique<GraphItems::Lines>(wxPen{ L"#ECECEC" }, GetScaling());
        for (size_t i = 0; i < m_longSentencesPoints.size() - 2; ++i)
            {
            polySeparatorLineSentences->AddLine(m_longSentencesPoints[i],
                                                m_longSentencesPoints[i + 1]);
            }
        AddObject(std::move(polySeparatorLineSentences));

        auto polySeparatorLineSentencesAxis =
            std::make_unique<GraphItems::Lines>(GetTopXAxis().GetPen(), GetScaling());
        polySeparatorLineSentencesAxis->AddLine(
            m_longSentencesPoints[m_longSentencesPoints.size() - 2],
            m_longSentencesPoints[m_longSentencesPoints.size() - 1]);
        polySeparatorLineSentencesAxis->AddLine(
            m_longSentencesPoints[m_longSentencesPoints.size() - 1], m_longSentencesPoints[0]);
        AddObject(std::move(polySeparatorLineSentencesAxis));

        // the explanatory legends (embedded on the graph)
        const wxFont legendFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(),
                                wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false,
                                GetFancyFontFaceName());
        // main label on graph
        wxPoint textCoordinate;
        GetPhysicalCoordinates(126, 19.6, textCoordinate);
        auto mainLabel = std::make_unique<GraphItems::Label>(
            GraphItems::GraphItemInfo(_(L"German\nReadability Graph"))
                .Pen(Wisteria::Colors::ColorBrewer::GetColor(Wisteria::Colors::Color::Black))
                .FontBackgroundColor(Colors::ColorBrewer::GetColor(Colors::Color::White))
                .Scaling(GetScaling())
                .DPIScaling(GetDPIScaleFactor())
                .Font(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize() * 1.5,
                             wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false,
                             GetFancyFontFaceName()))
                .LabelAlignment(TextAlignment::Centered)
                .Anchoring(Anchoring::BottomLeftCorner)
                .Padding(4, 4, 4, 4)
                .AnchorPoint(textCoordinate));
        mainLabel->SetBoxCorners(BoxCorners::Straight);

            {
            wxGCDC measureDC;
            auto legend = std::make_unique<GraphItems::Label>(
                GraphItems::GraphItemInfo(_(L"Average line\nAverage, each zone"))
                    .Scaling(GetScaling())
                    .DPIScaling(GetDPIScaleFactor())
                    .Pen(Wisteria::Colors::ColorBrewer::GetColor(Wisteria::Colors::Color::Black))
                    .FontBackgroundColor(Colors::ColorBrewer::GetColor(Colors::Color::White))
                    .Font(legendFont)
                    .LabelAlignment(TextAlignment::FlushLeft)
                    .Anchoring(Anchoring::BottomLeftCorner)
                    .Padding(4, 4, 4, GraphItems::Label::GetMinLegendWidthDIPs())
                    .AnchorPoint(mainLabel->GetBoundingBox(measureDC).GetTopLeft() -
                                 wxPoint(0, ScaleToScreenAndCanvas(2))));
            legend->GetLegendIcons().emplace_back(Icons::IconShape::HorizontalLine, separatorColor,
                                                  separatorColor);
            legend->GetLegendIcons().emplace_back(Icons::IconShape::Circle, separatorColor,
                                                  separatorColor);
            legend->SetBoxCorners(BoxCorners::Straight);
            AddObject(std::move(legend));
            }

        AddObject(std::move(mainLabel));

        CalculateScorePositions(dc);

        // Add the grade labels to the regions
        // (and highlight in heavy bold and a different color the one where the score lies)
        const wxFont numberFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize() *
                                    3,
                                wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL,
                                false, GetFancyFontFaceName());
        for (const auto& level : GetLevelLabels())
            {
            wxPoint point;
            GetPhysicalCoordinates(level.GetX(), level.GetY(), point);

            auto levelLabel =
                std::make_unique<GraphItems::Label>(GraphItems::GraphItemInfo(level.GetLabel())
                                                        .Scaling(GetScaling())
                                                        .DPIScaling(GetDPIScaleFactor())
                                                        .Pen(wxNullPen)
                                                        .Font(numberFont)
                                                        .FontColor(labelFontColor)
                                                        .AnchorPoint(point));
            levelLabel->SetAnchoring(Anchoring::TopLeftCorner);
            if (GetScores().size() == 1)
                {
                // ghost non-scored sections
                if (IsShowcasingScore() &&
                    level != std::floor(GetScores().front().GetScoreAverage()))
                    {
                    levelLabel->SetFontColor(Colors::ColorContrast::ChangeOpacity(
                        labelFontColor, Settings::GHOST_OPACITY));
                    }
                else
                    {
                    // If showcasing, then we will leave the scored section the same
                    // (this area will be shaded)...
                    levelLabel->GetFont().SetWeight(wxFontWeight::wxFONTWEIGHT_EXTRAHEAVY);
                    // ...but if not showcasing and this section is scored, then make it bold and
                    // change the color.
                    if (!IsShowcasingScore() &&
                        level == std::floor(GetScores().front().GetScoreAverage()))
                        {
                        levelLabel->SetFontColor(GetColorScheme()->GetColor(0));
                        }
                    }
                }
            AddObject(std::move(levelLabel));
            }
        }

    //----------------------------------------------------------------
    void SchwartzGraph::CalculateScorePositions(wxDC & dc)
        {
        const wxColour gradeLineColor{ Colors::ColorContrast::IsDark(GetPlotOrCanvasColor()) ?
                                           Colors::ColorBrewer::GetColor(Colors::Color::BondiBlue) :
                                           Colors::ColorBrewer::GetColor(Colors::Color::Blue) };

        if (GetDataset() == nullptr)
            {
            // draw regular grade lines and return since there are no points to plot
            for (size_t i = 3; i < std::size(m_gradeLinePoints); i += 2)
                {
                AddObject(std::make_unique<GraphItems::Polygon>(
                    GraphItems::GraphItemInfo{}
                        .Pen(wxPen{ Colors::ColorContrast::ChangeOpacity(gradeLineColor, 200) })
                        .Brush(gradeLineColor)
                        .Scaling(GetScaling()),
                    std::span(std::next(m_gradeLinePoints.cbegin(), i), 2)));
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
                std::clamp<double>(normalizationFactor * numberOfSyllablesColumn->GetValue(i), 125,
                                   189),
                std::clamp<double>(normalizationFactor * numberOfSentencesColumn->GetValue(i), 2.4,
                                   20));

            m_results[i].ResetStatus();

            const auto calcScoreFromPolygons =
                [](const std::unique_ptr<SchwartzGraph>& graph, ScorePoint& scorePoint)
            {
                // see where the point is
                if (!graph->GetPhysicalCoordinates(scorePoint.m_wordStatistic,
                                                   scorePoint.m_sentenceStatistic,
                                                   scorePoint.m_scorePoint))
                    {
                    scorePoint.SetScoreInvalid(true);
                    return false;
                    }

                if (IsScoreInsideRegion(scorePoint.m_scorePoint, graph->m_gradeOver8Polygon, 1, -1))
                    {
                    scorePoint.SetScore(8);
                    }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint, graph->m_grade7to8Polygon, 1,
                                             -1))
                    {
                    scorePoint.SetScoreRange(7, 8);
                    }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint, graph->m_grade5to6Polygon, 1,
                                             -1))
                    {
                    scorePoint.SetScoreRange(5, 6);
                    }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint, graph->m_grade3to4Polygon, 1,
                                             -1))
                    {
                    scorePoint.SetScoreRange(3, 4);
                    }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint, graph->m_grade2Polygon, 1,
                                             -1))
                    {
                    scorePoint.SetScore(2);
                    }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint, graph->m_grade1Polygon, 1,
                                             -1))
                    {
                    scorePoint.SetScore(1);
                    }
                else
                    {
                    scorePoint.SetScoreOutOfGradeRange(true);
                    }

                // if in a valid grade area see if it leans towards having harder sentences or words
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

            if (IsShowcasingScore() && GetScores().size() == 1)
                {
                if (GetScores().at(0).GetScoreRange().first == 8)
                    {
                    std::ranges::copy(std::as_const(m_gradeOver8Polygon),
                                      std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScoreRange().first == 7)
                    {
                    std::ranges::copy(std::as_const(m_grade7to8Polygon),
                                      std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScoreRange().first == 5)
                    {
                    std::ranges::copy(std::as_const(m_grade5to6Polygon),
                                      std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScoreRange().first == 3)
                    {
                    std::ranges::copy(std::as_const(m_grade3to4Polygon),
                                      std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScoreRange().first == 2)
                    {
                    std::ranges::copy(std::as_const(m_grade2Polygon),
                                      std::back_inserter(highlightedGradeLinePoints));
                    }
                else if (GetScores().at(0).GetScoreRange().first == 1)
                    {
                    std::ranges::copy(std::as_const(m_grade1Polygon),
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
        for (size_t i = 3; i < std::size(m_gradeLinePoints); i += 2)
            {
            auto foundHighlightLine =
                std::ranges::find(std::as_const(highlightedGradeLinePoints), m_gradeLinePoints[i]);
            const uint8_t opacityLevel = (foundHighlightLine == highlightedGradeLinePoints.cend() &&
                                          IsShowcasingScore() && GetScores().size() == 1) ?
                                             Settings::GHOST_OPACITY :
                                             200;
            AddObject(std::make_unique<GraphItems::Polygon>(
                GraphItems::GraphItemInfo{}
                    .Pen(
                        wxPen{ Colors::ColorContrast::ChangeOpacity(gradeLineColor, opacityLevel) })
                    .Brush(gradeLineColor)
                    .Scaling(GetScaling()),
                std::span(std::next(m_gradeLinePoints.cbegin(), i), 2)));
            }

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
    } // namespace Wisteria::Graphs
