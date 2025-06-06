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

#include "base_project_doc.h"
#include "../Wisteria-Dataviz/src/base/reportenumconvert.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../app/readability_app.h"

wxIMPLEMENT_DYNAMIC_CLASS(BaseProjectDoc, wxDocument)

    wxDECLARE_APP(ReadabilityApp);

using namespace lily_of_the_valley;
using namespace Wisteria;
using namespace Wisteria::Graphs;
using namespace Wisteria::Colors;
using namespace Wisteria::UI;

wxString BaseProjectDoc::m_exportTextViewExt = L"htm";
wxString BaseProjectDoc::m_exportListExt = L"htm";
wxString BaseProjectDoc::m_exportGraphExt = L"png";
bool BaseProjectDoc::m_exportHardWordLists = true;
bool BaseProjectDoc::m_exportSentencesBreakdown = true;
bool BaseProjectDoc::m_exportGraphs = true;
bool BaseProjectDoc::m_exportTestResults = true;
bool BaseProjectDoc::m_exportStatistics = true;
bool BaseProjectDoc::m_exportWordiness = true;
bool BaseProjectDoc::m_exportSightWords = true;
bool BaseProjectDoc::m_exportWarnings = true;
bool BaseProjectDoc::m_exportingLists = true;
bool BaseProjectDoc::m_exportingTextReports = true;
Wisteria::UI::ImageExportOptions BaseProjectDoc::m_imageExportOptions;

BaseProjectDoc::BaseProjectDoc()
    : m_FileReadOnly(false), m_realTimeUpdate(wxGetApp().GetAppOptions().IsRealTimeUpdating()),
      // graph options
      m_useGraphBackGroundImageLinearGradient(
          wxGetApp().GetAppOptions().GetGraphBackGroundLinearGradient()),
      m_displayDropShadows(wxGetApp().GetAppOptions().IsDisplayingDropShadows()),
      m_showcaseKeyItems(wxGetApp().GetAppOptions().IsShowcasingKeyItems()),
      m_graphColorSchemeName(wxGetApp().GetAppOptions().GetGraphColorScheme()),
      m_plotBackGroundImagePath(wxGetApp().GetAppOptions().GetPlotBackGroundImagePath()),
      m_stippleImagePath(wxGetApp().GetAppOptions().GetStippleImagePath()),
      m_commonImagePath(wxGetApp().GetAppOptions().GetGraphCommonImagePath()),
      m_stippleShape(wxGetApp().GetAppOptions().GetStippleShape()),
      m_stippleColor(wxGetApp().GetAppOptions().GetStippleShapeColor()),
      m_graphBackGroundColor(wxGetApp().GetAppOptions().GetBackGroundColor()),
      m_plotBackGroundColor(wxGetApp().GetAppOptions().GetPlotBackGroundColor()),
      m_plotBackGroundImageOpacity(wxGetApp().GetAppOptions().GetPlotBackGroundImageOpacity()),
      m_plotBackGroundColorOpacity(wxGetApp().GetAppOptions().GetPlotBackGroundColorOpacity()),
      m_watermark(wxGetApp().GetAppOptions().GetWatermark()),
      m_watermarkImagePath(wxGetApp().GetAppOptions().GetWatermarkLogo()),
      m_xAxisFontColor(wxGetApp().GetAppOptions().GetXAxisFontColor()),
      m_xAxisFont(wxGetApp().GetAppOptions().GetXAxisFont()),
      m_yAxisFontColor(wxGetApp().GetAppOptions().GetYAxisFontColor()),
      m_yAxisFont(wxGetApp().GetAppOptions().GetYAxisFont()),
      m_topTitleFontColor(wxGetApp().GetAppOptions().GetGraphTopTitleFontColor()),
      m_topTitleFont(wxGetApp().GetAppOptions().GetGraphTopTitleFont()),
      m_bottomTitleFontColor(wxGetApp().GetAppOptions().GetGraphBottomTitleFontColor()),
      m_bottomTitleFont(wxGetApp().GetAppOptions().GetGraphBottomTitleFont()),
      m_leftTitleFontColor(wxGetApp().GetAppOptions().GetGraphLeftTitleFontColor()),
      m_leftTitleFont(wxGetApp().GetAppOptions().GetGraphLeftTitleFont()),
      m_rightTitleFontColor(wxGetApp().GetAppOptions().GetGraphRightTitleFontColor()),
      m_rightTitleFont(wxGetApp().GetAppOptions().GetGraphRightTitleFont()),
      m_graphInvalidAreaColor(wxGetApp().GetAppOptions().GetInvalidAreaColor()),
      m_raygorStyle(wxGetApp().GetAppOptions().GetRaygorStyle()),
      m_fleschChartConnectPoints(wxGetApp().GetAppOptions().IsConnectingFleschPoints()),
      m_fleschChartSyllableRulerDocGroups(
          wxGetApp().GetAppOptions().IsIncludingFleschRulerDocGroups()),
      m_useEnglishLabelsGermanLix(wxGetApp().GetAppOptions().IsUsingEnglishLabelsForGermanLix()),
      // histogram options
      m_histogramBinningMethod(wxGetApp().GetAppOptions().GetHistogramBinningMethod()),
      m_histogramBinLabelDisplayMethod(wxGetApp().GetAppOptions().GetHistogramBinLabelDisplay()),
      m_histogramRoundingMethod(wxGetApp().GetAppOptions().GetHistogramRoundingMethod()),
      m_histogramIntervalDisplay(wxGetApp().GetAppOptions().GetHistogramIntervalDisplay()),
      m_histogramBarColor(wxGetApp().GetAppOptions().GetHistogramBarColor()),
      m_histogramBarOpacity(wxGetApp().GetAppOptions().GetHistogramBarOpacity()),
      m_histogramBarEffect(wxGetApp().GetAppOptions().GetHistogramBarEffect()),
      // bar chart options
      m_barChartBarColor(wxGetApp().GetAppOptions().GetBarChartBarColor()),
      m_barChartOrientation(wxGetApp().GetAppOptions().GetBarChartOrientation()),
      m_graphBarOpacity(wxGetApp().GetAppOptions().GetGraphBarOpacity()),
      m_graphBarEffect(wxGetApp().GetAppOptions().GetGraphBarEffect()),
      m_barDisplayLabels(wxGetApp().GetAppOptions().IsDisplayingBarChartLabels()),
      // box plot options
      m_boxPlotShowAllPoints(wxGetApp().GetAppOptions().IsShowingAllBoxPlotPoints()),
      m_boxDisplayLabels(wxGetApp().GetAppOptions().IsDisplayingBoxPlotLabels()),
      m_boxConnectMiddlePoints(wxGetApp().GetAppOptions().IsConnectingBoxPlotMiddlePoints()),
      m_graphBoxColor(wxGetApp().GetAppOptions().GetGraphBoxColor()),
      m_graphBoxOpacity(wxGetApp().GetAppOptions().GetGraphBoxOpacity()),
      m_graphBoxEffect(wxGetApp().GetAppOptions().GetGraphBoxEffect()),
      // background image effect
      m_plotBackgroundImageEffect(wxGetApp().GetAppOptions().GetPlotBackGroundImageEffect()),
      m_plotBackgroundImageFit(wxGetApp().GetAppOptions().GetPlotBackGroundImageFit()),
      // text highlighting options
      m_textViewHighlightColor(wxGetApp().GetAppOptions().GetTextHighlightColor()),
      m_excludedTextHighlightColor(wxGetApp().GetAppOptions().GetExcludedTextHighlightColor()),
      m_wordyPhraseHighlightColor(wxGetApp().GetAppOptions().GetWordyPhraseHighlightColor()),
      m_duplicateWordHighlightColor(wxGetApp().GetAppOptions().GetDuplicateWordHighlightColor()),
      m_textHighlight(wxGetApp().GetAppOptions().GetTextHighlightMethod()),
      m_textViewFont(wxGetApp().GetAppOptions().GetTextViewFont()),
      m_fontColor(wxGetApp().GetAppOptions().GetTextFontColor()),
      // dolch colors
      m_dolchConjunctionsColor(wxGetApp().GetAppOptions().GetDolchConjunctionsColor()),
      m_dolchPrepositionsColor(wxGetApp().GetAppOptions().GetDolchPrepositionsColor()),
      m_dolchPronounsColor(wxGetApp().GetAppOptions().GetDolchPronounsColor()),
      m_dolchAdverbsColor(wxGetApp().GetAppOptions().GetDolchAdverbsColor()),
      m_dolchAdjectivesColor(wxGetApp().GetAppOptions().GetDolchAdjectivesColor()),
      m_dolchVerbsColor(wxGetApp().GetAppOptions().GetDolchVerbsColor()),
      m_dolchNounColor(wxGetApp().GetAppOptions().GetDolchNounsColor()),

      m_highlightDolchConjunctions(wxGetApp().GetAppOptions().IsHighlightingDolchConjunctions()),
      m_highlightDolchPrepositions(wxGetApp().GetAppOptions().IsHighlightingDolchPrepositions()),
      m_highlightDolchPronouns(wxGetApp().GetAppOptions().IsHighlightingDolchPronouns()),
      m_highlightDolchAdverbs(wxGetApp().GetAppOptions().IsHighlightingDolchAdverbs()),
      m_highlightDolchAdjectives(wxGetApp().GetAppOptions().IsHighlightingDolchAdjectives()),
      m_highlightDolchVerbs(wxGetApp().GetAppOptions().IsHighlightingDolchVerbs()),
      m_highlightDolchNouns(wxGetApp().GetAppOptions().IsHighlightingDolchNouns()),
      // batch options
      m_filePathTruncationMode(wxGetApp().GetAppOptions().GetFilePathTruncationMode())
    {
    }

//------------------------------------------------
void BaseProjectDoc::CopyDocumentLevelSettings(const BaseProjectDoc& that, const bool reloadImages)
    {
    CopySettings(that);

    // load appended template file (if there is one)
    LoadAppendedDocument();

    // load the excluded phrases.
    LoadExcludePhrases();

    // not really transferred from batch to standard, but include for completeness
    m_realTimeUpdate = that.m_realTimeUpdate;
    // graph settings
    m_useGraphBackGroundImageLinearGradient = that.m_useGraphBackGroundImageLinearGradient;
    m_displayDropShadows = that.m_displayDropShadows;
    m_showcaseKeyItems = that.m_showcaseKeyItems;

    // image effect must be set before a possible call to SetPlotBackGroundImagePath(),
    // which will apply the effect
    m_plotBackgroundImageEffect = that.m_plotBackgroundImageEffect;

    if (reloadImages)
        {
        SetPlotBackGroundImagePath(that.m_plotBackGroundImagePath);
        SetStippleImagePath(that.m_stippleImagePath);
        SetWatermarkLogoPath(that.m_watermarkImagePath);
        SetGraphCommonImagePath(that.m_commonImagePath);
        }
    else
        {
        m_plotBackGroundImagePath = that.m_plotBackGroundImagePath;
        m_plotBackgroundImage = that.m_plotBackgroundImage;
        m_plotBackgroundImageWithEffect = that.m_plotBackgroundImageWithEffect;
        m_stippleImagePath = that.m_stippleImagePath;
        m_graphStippleImage = that.m_graphStippleImage;
        m_watermarkImagePath = that.m_watermarkImagePath;
        m_waterMarkImage = that.m_waterMarkImage;
        m_commonImagePath = that.m_commonImagePath;
        m_graphImageScheme = that.m_graphImageScheme;
        }

    SetGraphColorScheme(that.m_graphColorSchemeName); // needs to load a color scheme from a string
    m_stippleShape = that.m_stippleShape;
    m_stippleColor = that.m_stippleColor;
    m_graphBackGroundColor = that.m_graphBackGroundColor;
    m_plotBackGroundColor = that.m_plotBackGroundColor;
    m_plotBackGroundImageOpacity = that.m_plotBackGroundImageOpacity;
    m_plotBackgroundImageFit = that.m_plotBackgroundImageFit;
    m_plotBackGroundColorOpacity = that.m_plotBackGroundColorOpacity;
    m_watermark = that.m_watermark;
    m_xAxisFontColor = that.m_xAxisFontColor;
    m_xAxisFont = that.m_xAxisFont;
    m_yAxisFontColor = that.m_yAxisFontColor;
    m_yAxisFont = that.m_yAxisFont;
    m_topTitleFontColor = that.m_topTitleFontColor;
    m_topTitleFont = that.m_topTitleFont;
    m_bottomTitleFontColor = that.m_bottomTitleFontColor;
    m_bottomTitleFont = that.m_bottomTitleFont;
    m_leftTitleFontColor = that.m_leftTitleFontColor;
    m_leftTitleFont = that.m_leftTitleFont;
    m_rightTitleFontColor = that.m_rightTitleFontColor;
    m_rightTitleFont = that.m_rightTitleFont;
    // readability graphs
    m_graphInvalidAreaColor = that.m_graphInvalidAreaColor;
    m_raygorStyle = that.m_raygorStyle;
    m_fleschChartConnectPoints = that.m_fleschChartConnectPoints;
    m_fleschChartSyllableRulerDocGroups = that.m_fleschChartSyllableRulerDocGroups;
    m_useEnglishLabelsGermanLix = that.m_useEnglishLabelsGermanLix;
    // box plot
    m_boxPlotShowAllPoints = that.IsShowingAllBoxPlotPoints();
    m_boxDisplayLabels = that.IsDisplayingBoxPlotLabels();
    m_boxConnectMiddlePoints = that.IsConnectingBoxPlotMiddlePoints();
    m_graphBoxColor = that.GetGraphBoxColor();
    m_graphBoxOpacity = that.GetGraphBoxOpacity();
    m_graphBoxEffect = that.GetGraphBoxEffect();
    // bar chart
    m_barDisplayLabels = that.m_barDisplayLabels;
    m_graphBarEffect = that.m_graphBarEffect;
    m_graphBarOpacity = that.m_graphBarOpacity;
    m_barChartOrientation = that.m_barChartOrientation;
    m_barChartBarColor = that.m_barChartBarColor;
    }

//------------------------------------------------------
void BaseProjectDoc::SetGraphCommonImagePath(const wxString& filePath)
    {
    m_commonImagePath = filePath;
    if (filePath.empty())
        {
        m_graphImageScheme->Clear();
        }
    if (HasUI())
        {
        wxBitmapBundle bmp;
        LoadImageAndPath(m_commonImagePath, bmp);
        if (m_graphImageScheme->GetImages().size())
            {
            m_graphImageScheme->GetImages()[0] = bmp;
            }
        else
            {
            m_graphImageScheme->AddImage(bmp);
            }
        }
    else
        {
        m_graphImageScheme->Clear();
        }
    }

//------------------------------------------------------
void BaseProjectDoc::SetGraphColorScheme(wxString colorScheme)
    {
    m_graphColorSchemeName = std::move(colorScheme);

    auto foundScheme = Wisteria::ReportEnumConvert::ConvertColorScheme(m_graphColorSchemeName);
    if (foundScheme != nullptr)
        {
        m_graphBrushScheme =
            std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(*foundScheme);
        }
    }

//------------------------------------------------------
void BaseProjectDoc::SetPlotBackGroundImagePath(const wxString& filePath)
    {
    m_plotBackGroundImagePath = filePath;
    if (filePath.empty())
        {
        m_plotBackgroundImageWithEffect = m_plotBackgroundImage = wxBitmapBundle{};
        }
    if (HasUI())
        {
        if (wxFile::Exists(filePath))
            {
            const auto bmp = wxGetApp().GetResourceManager().GetBitmap(m_plotBackGroundImagePath,
                                                                       wxBITMAP_TYPE_ANY);
            if (bmp.IsOk())
                {
                m_plotBackgroundImage = wxBitmapBundle(bmp.ConvertToImage());
                }
            }
        else
            {
            // if image file not found, then try to search for it in the subdirectories from where
            // the project is
            wxString fileBySameNameInProjectDirectory;
            if (FindMissingFile(filePath, fileBySameNameInProjectDirectory))
                {
                m_plotBackGroundImagePath = fileBySameNameInProjectDirectory;
                const auto bmp = wxGetApp().GetResourceManager().GetBitmap(
                    m_plotBackGroundImagePath, wxBITMAP_TYPE_ANY);
                if (bmp.IsOk())
                    {
                    m_plotBackgroundImage = wxBitmapBundle(bmp.ConvertToImage());
                    }
                }
            else
                {
                m_plotBackgroundImage = wxBitmapBundle{};
                }
            }
        const auto bmp = m_plotBackgroundImage.GetBitmap(m_plotBackgroundImage.GetDefaultSize());
        if (bmp.IsOk())
            {
            m_plotBackgroundImageWithEffect = Wisteria::GraphItems::Image::ApplyEffect(
                GetPlotBackGroundImageEffect(), bmp.ConvertToImage());
            }
        }
    }

//------------------------------------------------------
void BaseProjectDoc::SetPlotBackGroundImageEffect(const Wisteria::ImageEffect effect)
    {
    if (HasUI())
        {
        // only update the altered image if changing the effect
        if (GetPlotBackGroundImageEffect() != effect)
            {
            m_plotBackgroundImageEffect = effect;
            const auto bmp =
                m_plotBackgroundImage.GetBitmap(m_plotBackgroundImage.GetDefaultSize());
            if (bmp.IsOk())
                {
                m_plotBackgroundImageWithEffect = Wisteria::GraphItems::Image::ApplyEffect(
                    GetPlotBackGroundImageEffect(), bmp.ConvertToImage());
                }
            }
        }
    else
        {
        m_plotBackgroundImageEffect = effect;
        }
    }

//------------------------------------------------------
void BaseProjectDoc::SetStippleImagePath(const wxString& filePath)
    {
    m_stippleImagePath = filePath;
    if (filePath.empty())
        {
        m_graphStippleImage = wxBitmapBundle{};
        }
    if (HasUI())
        {
        LoadImageAndPath(m_stippleImagePath, m_graphStippleImage);
        }
    else
        {
        m_graphStippleImage = wxBitmapBundle{};
        }
    }

//------------------------------------------------------
void BaseProjectDoc::SetWatermarkLogoPath(const wxString& filePath)
    {
    m_watermarkImagePath = filePath;
    if (filePath.empty())
        {
        m_waterMarkImage = wxBitmapBundle{};
        }
    if (HasUI())
        {
        LoadImageAndPath(m_watermarkImagePath, m_waterMarkImage);
        }
    else
        {
        m_waterMarkImage = wxBitmapBundle{};
        }
    }

//------------------------------------------------------
void BaseProjectDoc::LoadImageAndPath(wxString& filePath, wxBitmapBundle& img)
    {
    if (filePath.empty())
        {
        img = wxBitmapBundle{};
        }
    else if (wxFile::Exists(filePath))
        {
        const auto bmp = wxGetApp().GetResourceManager().GetBitmap(filePath, wxBITMAP_TYPE_ANY);
        if (bmp.IsOk())
            {
            img = wxBitmapBundle(bmp.ConvertToImage());
            }
        }
    else
        {
        // if image file not found, then try to search for it in the subdirectories from where the
        // project is
        wxString fileBySameNameInProjectDirectory;
        if (FindMissingFile(filePath, fileBySameNameInProjectDirectory))
            {
            filePath = fileBySameNameInProjectDirectory;
            const auto bmp = wxGetApp().GetResourceManager().GetBitmap(filePath, wxBITMAP_TYPE_ANY);
            if (bmp.IsOk())
                {
                img = wxBitmapBundle(bmp.ConvertToImage());
                }
            }
        else
            {
            img = wxBitmapBundle{};
            }
        }
    }

//------------------------------------------------
wxPrintData* BaseProjectDoc::GetPrintData() { return wxGetApp().GetPrintData(); }

//-------------------------------------------------------
wxColour BaseProjectDoc::GetTextReportBackgroundColor() const
    {
    if (m_textReportBackgroundColorFromTheme)
        {
        return wxSystemSettings::GetColour(wxSystemColour::wxSYS_COLOUR_WINDOW);
        }
    else
        {
        return *wxWHITE;
        }
    }

//------------------------------------------------
void BaseProjectDoc::UpdateTextWindowOptions(FormattedTextCtrl* textW) const
    {
    if (textW == nullptr)
        {
        return;
        }

    textW->SetBackgroundColour(GetTextReportBackgroundColor());

    UpdatePrinterHeaderAndFooters(textW);
    }

//------------------------------------------------
void BaseProjectDoc::UpdateExplanationListOptions(ExplanationListCtrl* eList)
    {
    if (eList == nullptr)
        {
        return;
        }

    eList->SetResources(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                        L"online/customizing-results.html");

    UpdatePrinterHeaderAndFooters(eList);
    }

//------------------------------------------------
void BaseProjectDoc::UpdatePrinterHeaderAndFooters(Wisteria::Canvas* window)
    {
    if (window == nullptr)
        {
        return;
        }
    window->SetPrinterSettings(*GetPrintData());
    window->SetLeftPrinterHeader(wxGetApp().GetAppOptions().GetLeftPrinterHeader());
    window->SetCenterPrinterHeader(wxGetApp().GetAppOptions().GetCenterPrinterHeader());
    window->SetRightPrinterHeader(wxGetApp().GetAppOptions().GetRightPrinterHeader());
    window->SetLeftPrinterFooter(wxGetApp().GetAppOptions().GetLeftPrinterFooter());
    window->SetCenterPrinterFooter(wxGetApp().GetAppOptions().GetCenterPrinterFooter());
    window->SetRightPrinterFooter(wxGetApp().GetAppOptions().GetRightPrinterFooter());
    }

//------------------------------------------------
void BaseProjectDoc::UpdatePrinterHeaderAndFooters(ExplanationListCtrl* window)
    {
    if (window == nullptr)
        {
        return;
        }
    window->SetPrinterSettings(GetPrintData());
    window->SetLeftPrinterHeader(wxGetApp().GetAppOptions().GetLeftPrinterHeader());
    window->SetCenterPrinterHeader(wxGetApp().GetAppOptions().GetCenterPrinterHeader());
    window->SetRightPrinterHeader(wxGetApp().GetAppOptions().GetRightPrinterHeader());
    window->SetLeftPrinterFooter(wxGetApp().GetAppOptions().GetLeftPrinterFooter());
    window->SetCenterPrinterFooter(wxGetApp().GetAppOptions().GetCenterPrinterFooter());
    window->SetRightPrinterFooter(wxGetApp().GetAppOptions().GetRightPrinterFooter());
    }

//------------------------------------------------
void BaseProjectDoc::UpdatePrinterHeaderAndFooters(FormattedTextCtrl* window)
    {
    if (window == nullptr)
        {
        return;
        }
    window->SetPrinterSettings(GetPrintData());
    window->SetLeftPrinterHeader(wxGetApp().GetAppOptions().GetLeftPrinterHeader());
    window->SetCenterPrinterHeader(wxGetApp().GetAppOptions().GetCenterPrinterHeader());
    window->SetRightPrinterHeader(wxGetApp().GetAppOptions().GetRightPrinterHeader());
    window->SetLeftPrinterFooter(wxGetApp().GetAppOptions().GetLeftPrinterFooter());
    window->SetCenterPrinterFooter(wxGetApp().GetAppOptions().GetCenterPrinterFooter());
    window->SetRightPrinterFooter(wxGetApp().GetAppOptions().GetRightPrinterFooter());
    }

//------------------------------------------------
void BaseProjectDoc::UpdatePrinterHeaderAndFooters(ListCtrlEx* window)
    {
    if (window == nullptr)
        {
        return;
        }
    window->SetPrinterSettings(GetPrintData());
    window->SetLeftPrinterHeader(wxGetApp().GetAppOptions().GetLeftPrinterHeader());
    window->SetCenterPrinterHeader(wxGetApp().GetAppOptions().GetCenterPrinterHeader());
    window->SetRightPrinterHeader(wxGetApp().GetAppOptions().GetRightPrinterHeader());
    window->SetLeftPrinterFooter(wxGetApp().GetAppOptions().GetLeftPrinterFooter());
    window->SetCenterPrinterFooter(wxGetApp().GetAppOptions().GetCenterPrinterFooter());
    window->SetRightPrinterFooter(wxGetApp().GetAppOptions().GetRightPrinterFooter());
    }

//------------------------------------------------
void BaseProjectDoc::UpdatePrinterHeaderAndFooters(Wisteria::UI::HtmlTableWindow* window)
    {
    if (window == nullptr)
        {
        return;
        }
    window->SetPrinterSettings(GetPrintData());
    window->SetLeftPrinterHeader(wxGetApp().GetAppOptions().GetLeftPrinterHeader());
    window->SetCenterPrinterHeader(wxGetApp().GetAppOptions().GetCenterPrinterHeader());
    window->SetRightPrinterHeader(wxGetApp().GetAppOptions().GetRightPrinterHeader());
    window->SetLeftPrinterFooter(wxGetApp().GetAppOptions().GetLeftPrinterFooter());
    window->SetCenterPrinterFooter(wxGetApp().GetAppOptions().GetCenterPrinterFooter());
    window->SetRightPrinterFooter(wxGetApp().GetAppOptions().GetRightPrinterFooter());
    }

//------------------------------------------------
void BaseProjectDoc::UpdateListOptions(ListCtrlEx* list)
    {
    if (list == nullptr)
        {
        return;
        }

    list->SetExportResources(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                             L"online/publishing.html");
    list->SetSortHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                           L"online/customizing-results.html");

    UpdatePrinterHeaderAndFooters(list);
    }

//------------------------------------------------
void BaseProjectDoc::UpdateGraphOptions(Wisteria::Canvas* canvas)
    {
    if (canvas == nullptr)
        {
        return;
        }
    canvas->SetExportResources(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                               L"online/publishing.html");

    UpdatePrinterHeaderAndFooters(canvas);

    canvas->SetBackgroundColor(GetBackGroundColor(), GetGraphBackGroundLinearGradient());

    auto graph = std::dynamic_pointer_cast<Wisteria::Graphs::Graph2D>(canvas->GetFixedObject(0, 0));
    assert(graph && L"No graph on the canvas!");

    if (m_graphBrushScheme != nullptr)
        {
        graph->SetBrushScheme(m_graphBrushScheme);
        }
    if (graph->IsKindOf(wxCLASSINFO(WordCloud)))
        {
        const auto foundColorScheme =
            std::find_if(wxGetApp().GetGraphColorSchemeMap().cbegin(),
                         wxGetApp().GetGraphColorSchemeMap().cend(), [this](const auto& colorKey)
                         { return GetGraphColorScheme() == colorKey.second; });
        if (foundColorScheme != wxGetApp().GetGraphColorSchemeMap().cend())
            {
            graph->SetColorScheme(ReportEnumConvert::ConvertColorScheme(foundColorScheme->second));
            }
        }
    graph->SetPlotBackgroundColor(Colors::ColorContrast::ChangeOpacity(
        GetPlotBackGroundColor(), GetPlotBackGroundColorOpacity()));
    graph->SetPlotBackgroundImage(m_plotBackgroundImageWithEffect, GetPlotBackGroundImageOpacity());
    graph->SetPlotBackgroundImageFit(GetPlotBackGroundImageFit());

    graph->SetStippleBrush(m_graphStippleImage);
    graph->SetImageScheme(m_graphImageScheme);
    canvas->SetWatermarkLogo(m_waterMarkImage, wxSize(100, 100));
    canvas->SetWatermark(GetWatermark());
    graph->GetBottomXAxis().GetFont() = GetXAxisFont();
    graph->GetBottomXAxis().SetFontColor(GetXAxisFontColor());
    graph->GetBottomXAxis().GetTitle().GetFont() = GetXAxisFont();
    graph->GetBottomXAxis().GetTitle().SetFontColor(GetXAxisFontColor());

    graph->GetTopXAxis().GetFont() = GetXAxisFont();
    graph->GetTopXAxis().SetFontColor(GetXAxisFontColor());
    graph->GetTopXAxis().GetTitle().GetFont() = GetXAxisFont();
    graph->GetTopXAxis().GetTitle().SetFontColor(GetXAxisFontColor());

    graph->GetLeftYAxis().GetFont() = GetYAxisFont();
    graph->GetLeftYAxis().SetFontColor(GetYAxisFontColor());
    graph->GetLeftYAxis().GetTitle().GetFont() = GetYAxisFont();
    graph->GetLeftYAxis().GetTitle().SetFontColor(GetYAxisFontColor());

    graph->GetRightYAxis().GetFont() = GetYAxisFont();
    graph->GetRightYAxis().SetFontColor(GetYAxisFontColor());
    graph->GetRightYAxis().GetTitle().GetFont() = GetYAxisFont();
    graph->GetRightYAxis().GetTitle().SetFontColor(GetYAxisFontColor());

    for (size_t i = 0; i < graph->GetCustomAxes().size(); ++i)
        {
        graph->GetCustomAxes().at(i).GetFont() = GetYAxisFont();
        graph->GetCustomAxes().at(i).SetFontColor(GetYAxisFontColor());
        }

    // update the plot title's font also
    graph->GetTitle().GetFont() = GetGraphTopTitleFont();
    graph->GetTitle().SetFontColor(GetGraphTopTitleFontColor());
    // canvas title fonts
    for (size_t i = 0; i < canvas->GetTopTitles().size(); ++i)
        {
        canvas->GetTopTitles().at(i).GetFont() = GetGraphTopTitleFont();
        canvas->GetTopTitles().at(i).SetFontColor(GetGraphTopTitleFontColor());
        }
    for (size_t i = 0; i < canvas->GetBottomTitles().size(); ++i)
        {
        canvas->GetBottomTitles().at(i).GetFont() = GetGraphBottomTitleFont();
        canvas->GetBottomTitles().at(i).SetFontColor(GetGraphBottomTitleFontColor());
        }
    for (size_t i = 0; i < canvas->GetLeftTitles().size(); ++i)
        {
        canvas->GetLeftTitles().at(i).GetFont() = GetGraphLeftTitleFont();
        canvas->GetLeftTitles().at(i).SetFontColor(GetGraphLeftTitleFontColor());
        }
    for (size_t i = 0; i < canvas->GetRightTitles().size(); ++i)
        {
        canvas->GetRightTitles().at(i).GetFont() = GetGraphRightTitleFont();
        canvas->GetRightTitles().at(i).SetFontColor(GetGraphRightTitleFontColor());
        }
    }

//------------------------------------------------
bool BaseProjectDoc::LockProjectFile()
    {
    // check for read-only permissions
    if (!wxFileName(GetFilename()).IsFileWritable())
        {
        if (WarningManager::HasWarning(_DT(L"project-open-as-read-only")))
            {
            LogMessage(*WarningManager::GetWarning(_DT(L"project-open-as-read-only")));
            }
        m_FileReadOnly = true;
        }
    else
        {
        m_FileReadOnly = false;
        }
    // open in read-only mode, actual writing to project file is done via a temp file
    if (!m_File.Open(GetFilename(), wxFile::read))
        {
        LogMessage(wxString::Format(_(L"'%s': unable to open project file."), GetFilename()),
                   _(L"Project Error"), wxOK | wxICON_EXCLAMATION);
        return false;
        }
    return true;
    }

//-------------------------------------------------------
void BaseProjectDoc::RemoveAllGlobalCustomReadabilityTests()
    {
    if (m_custom_word_tests.empty())
        {
        return;
        }

    wxArrayString testNames;
    for (CustomReadabilityTestCollection::const_iterator pos =
             BaseProject::m_custom_word_tests.cbegin();
         pos != BaseProject::m_custom_word_tests.cend(); ++pos)
        {
        testNames.Add(pos->get_name().c_str());
        }
    // remove tests from all of the projects' menus
    MainFrame* mainFrame = dynamic_cast<MainFrame*>(wxGetApp().GetMainFrame());
    if (mainFrame)
        {
        wxWindowUpdateLocker noUpdates(mainFrame);
        for (size_t i = 0; i < testNames.Count(); ++i)
            {
            mainFrame->RemoveCustomTestFromMenus(testNames[i]);
            }
        }

    // remove the tests from all of the open projects and then refresh them
    wxList docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        doc->ExcludeAllCustomTestsTests();
        doc->Modify(true);
        doc->RefreshRequired(ProjectRefresh::Minimal);
        doc->RefreshProject();
        }

    // finally, remove the global tests
    m_custom_word_tests.clear();
    }

//-------------------------------------------------------
void BaseProjectDoc::RemoveGlobalCustomReadabilityTest(const wxString& testName)
    {
    // return if the test name isn't found
    CustomReadabilityTestCollection::iterator testPos =
        std::find(m_custom_word_tests.begin(), m_custom_word_tests.end(), testName);
    if (testPos == m_custom_word_tests.end())
        {
        return;
        }

    // see if any open projects are using this test. If so, they need to have it removed.
    wxList docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        const BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc->HasCustomTest(testName))
            {
            if (wxMessageBox(_(L"This test will need to be removed from any open projects "
                               "that are currently including it.\n"
                               "Do you wish to proceed with removing this test?"),
                             _(L"Project Update"), wxYES_NO | wxICON_QUESTION) == wxNO)
                {
                return;
                }
            break;
            }
        }

    const int testId = testPos->get_interface_id();
    // remove it from the global lists of available tests
    m_custom_word_tests.erase(testPos);
    // remove it from all of the projects' menus
    MainFrame* mainFrame = dynamic_cast<MainFrame*>(wxGetApp().GetMainFrame());
    if (mainFrame)
        {
        mainFrame->RemoveCustomTestFromMenus(testName);
        }
    /* remove it from all of the open projects. If a project doesn't have it, then just
       re-sync its custom tests pointers to the global list of custom tests because one was
       removed from it.*/
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc->HasCustomTest(testName))
            {
            doc->RemoveCustomReadabilityTest(testName, testId);
            doc->SyncCustomTests();
            doc->Modify(true);
            doc->RefreshRequired(ProjectRefresh::Minimal);
            doc->RefreshProject();
            }
        else
            {
            doc->SyncCustomTests();
            }
        }
    }

//-------------------------------------------------------
CustomReadabilityTest::string_type
BaseProjectDoc::UpdateCustomReadabilityTest(const CustomReadabilityTest::string_type& testFormula)
    {
    CustomReadabilityTest::string_type formula = testFormula;
    string_util::replace_all_whole_word<decltype(formula)>(formula, L"WORDCOUNT()",
                                                           L"WordCount(Default)");
    string_util::replace_all_whole_word<decltype(formula)>(formula, L"SENTENCECOUNT()",
                                                           L"SentenceCount(Default)");
    string_util::replace_all_whole_word<decltype(formula)>(formula, L"CHARACTERCOUNT()",
                                                           L"CharacterCount(Default)");
    string_util::replace_all_whole_word<decltype(formula)>(formula, L"SYLLABLECOUNT()",
                                                           L"SyllableCount(Default)");
    string_util::replace_all_whole_word<decltype(formula)>(formula, L"THREESYLLABLEPLUSWORDCOUNT()",
                                                           L"ThreeSyllablePlusWordCount(Default)");
    string_util::replace_all_whole_word<decltype(formula)>(
        formula, L"UNIQUETHREESYLLABLEPLUSWORDCOUNT()",
        L"UniqueThreeSyllablePlusWordCount(Default)");
    string_util::replace_all_whole_word<decltype(formula)>(formula, L"COTAN(", L"COT(");
    string_util::replace_all_whole_word<decltype(formula)>(formula, L"AVG(", L"AVERAGE(");
    string_util::replace_all_whole_word<decltype(formula)>(formula, L"INTPOW(", L"POWER(");
    string_util::replace_all_whole_word<decltype(formula)>(formula, L"RANDOM(", L"RAND(");
    return formula;
    }

//-------------------------------------------------------
bool BaseProjectDoc::AddGlobalCustomReadabilityTest(CustomReadabilityTest& customTest)
    {
    customTest.set_formula(UpdateCustomReadabilityTest(customTest.get_formula()));
    // see if there is a test included already (globally) with the same name
    if (std::find(m_custom_word_tests.begin(), m_custom_word_tests.end(), customTest.get_name()) !=
        m_custom_word_tests.end())
        {
        // ..if so, then see if it is the exact same test (simply return if that is the case)
        if (std::find(m_custom_word_tests.begin(), m_custom_word_tests.end(), customTest) !=
            m_custom_word_tests.end())
            {
            return true;
            }
        // we encountered a test name conflict and the test settings are different, so we will
        // need to rename this test to something else before adding it to the system
        wxString name = customTest.get_name().c_str();
        while (true)
            {
            if (std::find(m_custom_word_tests.begin(), m_custom_word_tests.end(), name) !=
                m_custom_word_tests.end())
                {
                wxMessageDialog msDlg(
                    wxGetApp().GetParentingWindow(),
                    wxString::Format(
                        _(L"This project or settings file contains a custom test named "
                          "\"%s\" that conflicts with an existing test of the same name."),
                        name),
                    _(L"Warning"), wxYES_NO | wxICON_QUESTION);
                msDlg.SetYesNoLabels(wxString::Format(_(L"Use existing version of \"%s\""), name),
                                     _(L"Rename test being imported"));
                if (msDlg.ShowModal() == wxID_YES)
                    {
                    // just replace the test in the project with the global one
                    return true;
                    }
                name = wxGetTextFromUser(_(L"Please specify a different name:"),
                                         _(L"Enter New Test Name"), name,
                                         wxGetApp().GetParentingWindow());
                // Cancel was pressed
                if (name.empty())
                    {
                    return false;
                    }
                continue;
                }
            // name is unique now, so stop prompting for a new name and change the name in the test
            customTest.set_name(name.wc_str());
            break;
            }
        }

    // validate the formula before adding it to the system
    try
        {
        BaseProject project;
        if (!project.GetFormulaParser().compile(
                wxString(customTest.get_formula().c_str()).ToStdString()))
            {
            wxMessageBox(
                wxString::Format(_(L"Error in formula, cannot add custom test \"%s\":\n\n"),
                                 customTest.get_name().c_str()),
                _(L"Error in Formula"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        }
    catch (const std::exception& exp)
        {
        wxMessageBox(
            wxString::Format(_(L"%s\nPlease verify the syntax of the formula."), exp.what()),
            _(L"Error in Formula"), wxOK | wxICON_EXCLAMATION);
        return false;
        }
    catch (...)
        {
        wxMessageBox(wxString::Format(_(L"An unknown error occurred while validating the formula. "
                                        "Cannot add custom test \"%s\"."),
                                      customTest.get_name().c_str()),
                     _(L"Error in Formula"), wxOK | wxICON_EXCLAMATION);
        return false;
        }

    wxBusyCursor wait;

    // read in the word file--note that the file path might be changed by user,
    // so update the path in the test
    wxString fileText;
    wxString wordFilePath = customTest.get_familiar_word_list_file_path().c_str();
    // load custom word file if they are using one. If not then just load an
    // empty string into this list
    if (customTest.is_including_custom_familiar_word_list())
        {
        if (!Wisteria::TextStream::ReadFile(wordFilePath, fileText))
            {
            wxMessageBox(_(L"Unable to load word list."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        }
    customTest.set_familiar_word_list_file_path(wordFilePath.wc_str());

    customTest.load_custom_familiar_words(fileText);

    m_custom_word_tests.push_back(customTest);

    MainFrame* mainFrame = dynamic_cast<MainFrame*>(wxGetApp().GetMainFrame());
    assert(mainFrame);
    if (mainFrame)
        {
        mainFrame->AddCustomTestToMenus(customTest.get_name().c_str());
        }

    // need to update all of the document iterators into this list because it has changed
    wxList docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc)
            {
            doc->SyncCustomTests();
            }
        }

    return true;
    }

//------------------------------------------------
bool BaseProjectDoc::OnCloseDocument()
    {
    if (IsProcessing())
        {
        LogMessage(_(L"Project still being reloaded. Please wait before closing."), GetTitle(),
                   wxOK | wxICON_EXCLAMATION);
        return false;
        }
    return wxDocument::OnCloseDocument();
    }

//------------------------------------------------
void BaseProjectDoc::LoadSettingsFile(const wchar_t* settingsFileText)
    {
    wxString currentStartTag;
    wxString currentEndTag;
    const wchar_t* settingsFileTextEnd = settingsFileText + std::wcslen(settingsFileText);

    // first, get the project format version number
    currentStartTag.clear();
    currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_PROJECT_HEADER.data());
    const wchar_t* projectSection = std::wcsstr(settingsFileText, currentStartTag.wc_str());
    wxString docVersionNumber = L"1.0";
    if (projectSection)
        {
        /* See if the project is a newer format than the current version that this
           product supports. Try to be forward compatibility, but some info will be lost.*/
        const wchar_t* endTag = std::wcschr(projectSection, L'>');
        const wchar_t* version =
            std::wcsstr(projectSection, wxGetApp().GetAppOptions().XML_VERSION_W.data());
        if (version && endTag && (version < endTag))
            {
            version = std::wcschr(version, L'\"');
            if (version)
                {
                ++version;
                const wchar_t* versionEnd = std::wcschr(version, L'\"');
                if (versionEnd)
                    {
                    docVersionNumber.assign(version, (versionEnd - version));
                    double docDouble{ 0 }, appDocDouble{ 0 };
                    if (docVersionNumber.ToDouble(&docDouble) &&
                        wxGetApp().GetDocumentVersionNumber().ToDouble(&appDocDouble) &&
                        docDouble > appDocDouble)
                        {
                        LogMessage(wxString::Format(
                                       _(L"Warning: This project was saved from a newer version "
                                         "of %s. Some information may be lost."),
                                       wxGetApp().GetAppDisplayName()),
                                   _(L"Version Conflict"), wxOK | wxICON_INFORMATION);
                        }
                    }
                }
            }
        }

    // original text source (e.g., document file) information
    currentStartTag.clear();
    currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_DOCUMENT.data());
    currentEndTag.clear();
    currentEndTag.append(L"</").append(wxGetApp().GetAppOptions().XML_DOCUMENT.data()).append(L">");
    const wchar_t* docParsingSection = std::wcsstr(settingsFileText, currentStartTag);
    const wchar_t* docParsingSectionEnd = std::wcsstr(settingsFileText, currentEndTag);
    if (docParsingSection && docParsingSectionEnd && (docParsingSection < docParsingSectionEnd))
        {
        /* see if the text was from a file or manually entered
          (if bogus value is in file then assume from file instead of getting global default)*/
        SetTextSource(static_cast<TextSource>(
            XmlFormat::GetLong(docParsingSection, docParsingSectionEnd,
                               wxGetApp().GetAppOptions().XML_TEXT_SOURCE.data(),
                               static_cast<long>(wxGetApp().GetAppOptions().GetTextSource()))));
        if (GetTextSource() != TextSource::EnteredText && GetTextSource() != TextSource::FromFile)
            {
            SetTextSource(TextSource::FromFile);
            }
        // read in the file path to the (original) document
        XmlFormat::GetStringsWithExtraInfo(docParsingSection, docParsingSectionEnd,
                                           wxGetApp().GetAppOptions().XML_DOCUMENT_PATH.data(),
                                           wxGetApp().GetAppOptions().XML_DESCRIPTION.data(),
                                           GetSourceFilesInfo());

        UseRealTimeUpdate(
            XmlFormat::GetBoolean(docParsingSection, docParsingSectionEnd,
                                  wxGetApp().GetAppOptions().XML_REALTIME_UPDATE.data(),
                                  wxGetApp().GetAppOptions().IsRealTimeUpdating()));
        // reviewer and status
        SetReviewer(XmlFormat::GetString(docParsingSection, docParsingSectionEnd,
                                         wxGetApp().GetAppOptions().XML_REVIEWER.data()));
        SetStatus(XmlFormat::GetString(docParsingSection, docParsingSectionEnd,
                                       wxGetApp().GetAppOptions().XML_STATUS.data()));
        // appended doc
        SetAppendedDocumentFilePath(
            XmlFormat::GetString(docParsingSection, docParsingSectionEnd,
                                 wxGetApp().GetAppOptions().XML_APPENDED_DOC_PATH.data()));
        LoadAppendedDocument();
        // see if the original file is embedded in the project or just linked too
        SetDocumentStorageMethod(static_cast<TextStorage>(XmlFormat::GetLong(
            docParsingSection, docParsingSectionEnd,
            wxGetApp().GetAppOptions().XML_DOCUMENT_STORAGE_METHOD.data(),
            static_cast<long>(wxGetApp().GetAppOptions().GetDocumentStorageMethod()))));
        if (GetDocumentStorageMethod() != TextStorage::EmbedText &&
            GetDocumentStorageMethod() != TextStorage::NoEmbedText)
            {
            SetDocumentStorageMethod(wxGetApp().GetAppOptions().GetDocumentStorageMethod());
            }
        // Min doc size
        SetMinDocWordCountForBatch(static_cast<size_t>(XmlFormat::GetLong(
            docParsingSection, docParsingSectionEnd,
            wxGetApp().GetAppOptions().XML_MIN_DOC_SIZE_FOR_BATCH.data(),
            static_cast<long>(wxGetApp().GetAppOptions().GetMinDocWordCountForBatch()))));
        if (GetMinDocWordCountForBatch() < 1)
            {
            SetMinDocWordCountForBatch(1);
            }
        // how file paths are shown in batch projects
        long truncMode = XmlFormat::GetLong(
            docParsingSection, docParsingSectionEnd,
            wxGetApp().GetAppOptions().XML_FILE_PATH_TRUNC_MODE.data(),
            static_cast<long>(wxGetApp().GetAppOptions().GetFilePathTruncationMode()));
        if (truncMode >=
            static_cast<decltype(truncMode)>(ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::
                                                 COLUMN_FILE_PATHS_TRUNCATION_MODE_COUNT))
            {
            truncMode = static_cast<decltype(truncMode)>(
                wxGetApp().GetAppOptions().GetFilePathTruncationMode());
            }
        SetFilePathTruncationMode(
            static_cast<ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode>(truncMode));
        // The project's language (used for determining which tests can be used)
        readability::test_language projectLanguage = static_cast<readability::test_language>(
            XmlFormat::GetLong(docParsingSection, docParsingSectionEnd,
                               wxGetApp().GetAppOptions().XML_PROJECT_LANGUAGE.data(),
                               static_cast<long>(wxGetApp().GetAppOptions().GetProjectLanguage())));
        SetProjectLanguage(projectLanguage);

        // exporting file paths
        m_exportFolder =
            XmlFormat::GetString(docParsingSection, docParsingSectionEnd,
                                 wxGetApp().GetAppOptions().XML_EXPORT_FOLDER_PATH.data());
        m_exportFile = XmlFormat::GetString(docParsingSection, docParsingSectionEnd,
                                            wxGetApp().GetAppOptions().XML_EXPORT_FILE_PATH.data());
        }

    // sentences breakdown
    const wchar_t* sentencesBreakdownSection =
        html_extract_text::find_element(settingsFileText, settingsFileTextEnd,
                                        wxGetApp().GetAppOptions().XML_SENTENCES_BREAKDOWN_W, true);
    const wchar_t* sentencesBreakdownSectionEnd =
        sentencesBreakdownSection ? html_extract_text::find_closing_element(
                                        sentencesBreakdownSection, settingsFileTextEnd,
                                        wxGetApp().GetAppOptions().XML_SENTENCES_BREAKDOWN_W) :
                                    nullptr;
    if (sentencesBreakdownSection && sentencesBreakdownSectionEnd)
        {
        const wxString wordsBreakdownInfo =
            XmlFormat::GetString(sentencesBreakdownSection, sentencesBreakdownSectionEnd,
                                 wxGetApp().GetAppOptions().XML_SENTENCES_BREAKDOWN_INFO.data());
        if (wordsBreakdownInfo.length())
            {
            GetSentencesBreakdownInfo().Set(wordsBreakdownInfo);
            }
        }

    // words breakdown
    const wchar_t* wordsBreakdownSection =
        html_extract_text::find_element(settingsFileText, settingsFileTextEnd,
                                        wxGetApp().GetAppOptions().XML_WORDS_BREAKDOWN_W, true);
    const wchar_t* wordsBreakdownSectionSectionEnd =
        wordsBreakdownSection ? html_extract_text::find_closing_element(
                                    wordsBreakdownSection, settingsFileTextEnd,
                                    wxGetApp().GetAppOptions().XML_WORDS_BREAKDOWN_W) :
                                nullptr;
    if (wordsBreakdownSection && wordsBreakdownSectionSectionEnd)
        {
        const wxString wordsBreakdownInfo =
            XmlFormat::GetString(wordsBreakdownSection, wordsBreakdownSectionSectionEnd,
                                 wxGetApp().GetAppOptions().XML_WORDS_BREAKDOWN_INFO.data());
        if (wordsBreakdownInfo.length())
            {
            GetWordsBreakdownInfo().Set(wordsBreakdownInfo);
            }
        }

    // grammar
    currentStartTag.clear();
    currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_GRAMMAR.data());
    currentEndTag.clear();
    currentEndTag.append(L"</").append(wxGetApp().GetAppOptions().XML_GRAMMAR.data()).append(L">");
    const wchar_t* grammarSection = std::wcsstr(settingsFileText, currentStartTag);
    const wchar_t* grammarSectionEnd = std::wcsstr(settingsFileText, currentEndTag);
    if (grammarSection && grammarSectionEnd && (grammarSection < grammarSectionEnd))
        {
        SpellCheckIgnoreProperNouns(XmlFormat::GetBoolean(
            grammarSection, grammarSectionEnd,
            wxGetApp().GetAppOptions().XML_SPELLCHECK_IGNORE_PROPER_NOUNS.data(),
            wxGetApp().GetAppOptions().SpellCheckIsIgnoringProperNouns()));
        SpellCheckIgnoreUppercased(XmlFormat::GetBoolean(
            grammarSection, grammarSectionEnd,
            wxGetApp().GetAppOptions().XML_SPELLCHECK_IGNORE_UPPERCASED.data(),
            wxGetApp().GetAppOptions().SpellCheckIsIgnoringUppercased()));
        SpellCheckIgnoreNumerals(
            XmlFormat::GetBoolean(grammarSection, grammarSectionEnd,
                                  wxGetApp().GetAppOptions().XML_SPELLCHECK_IGNORE_NUMERALS.data(),
                                  wxGetApp().GetAppOptions().SpellCheckIsIgnoringNumerals()));
        SpellCheckIgnoreFileAddresses(XmlFormat::GetBoolean(
            grammarSection, grammarSectionEnd,
            wxGetApp().GetAppOptions().XML_SPELLCHECK_IGNORE_FILE_ADDRESSES.data(),
            wxGetApp().GetAppOptions().SpellCheckIsIgnoringFileAddresses()));
        SpellCheckIgnoreProgrammerCode(XmlFormat::GetBoolean(
            grammarSection, grammarSectionEnd,
            wxGetApp().GetAppOptions().XML_SPELLCHECK_IGNORE_PROGRAMMER_CODE.data(),
            wxGetApp().GetAppOptions().SpellCheckIsIgnoringProgrammerCode()));
        SpellCheckAllowColloquialisms(XmlFormat::GetBoolean(
            grammarSection, grammarSectionEnd,
            wxGetApp().GetAppOptions().XML_SPELLCHECK_ALLOW_COLLOQUIALISMS.data(),
            wxGetApp().GetAppOptions().SpellCheckIsAllowingColloquialisms()));
        SpellCheckIgnoreSocialMediaTags(XmlFormat::GetBoolean(
            grammarSection, grammarSectionEnd,
            wxGetApp().GetAppOptions().XML_SPELLCHECK_IGNORE_SOCIAL_MEDIA_TAGS.data(),
            wxGetApp().GetAppOptions().SpellCheckIsIgnoringSocialMediaTags()));
        const wxString grammarInfo = XmlFormat::GetString(
            grammarSection, grammarSectionEnd, wxGetApp().GetAppOptions().XML_GRAMMAR_INFO.data());
        if (grammarInfo.length())
            {
            GetGrammarInfo().Set(grammarInfo);
            }
        }

    // read in the parsing and analysis logic
    currentStartTag.clear();
    currentStartTag.append(L"<").append(
        wxGetApp().GetAppOptions().XML_DOCUMENT_ANALYSIS_LOGIC.data());
    currentEndTag.clear();
    currentEndTag.append(L"</")
        .append(wxGetApp().GetAppOptions().XML_DOCUMENT_ANALYSIS_LOGIC.data())
        .append(L">");
    const wchar_t* parsingSection = std::wcsstr(settingsFileText, currentStartTag);
    const wchar_t* parsingSectionEnd = std::wcsstr(settingsFileText, currentEndTag);
    if (parsingSection && parsingSectionEnd && (parsingSection < parsingSectionEnd))
        {
        // get the method for determining a long sentence
        SetLongSentenceMethod(static_cast<LongSentence>(XmlFormat::GetLong(
            parsingSection, parsingSectionEnd,
            wxGetApp().GetAppOptions().XML_LONG_SENTENCE_METHOD.data(),
            static_cast<long>(wxGetApp().GetAppOptions().GetLongSentenceMethod()))));
        if (GetLongSentenceMethod() != LongSentence::LongerThanSpecifiedLength &&
            GetLongSentenceMethod() != LongSentence::OutlierLength)
            {
            SetLongSentenceMethod(
                static_cast<LongSentence>(wxGetApp().GetAppOptions().GetLongSentenceMethod()));
            }
        // get the hard sentence length
        SetDifficultSentenceLength(static_cast<int>(
            XmlFormat::GetLong(parsingSection, parsingSectionEnd,
                               wxGetApp().GetAppOptions().XML_LONG_SENTENCE_LENGTH.data(),
                               static_cast<long>(GetDifficultSentenceLength()))));
        // get the method for numeral syllabizing
        SetNumeralSyllabicationMethod(static_cast<NumeralSyllabize>(XmlFormat::GetLong(
            parsingSection, parsingSectionEnd,
            wxGetApp().GetAppOptions().XML_NUMERAL_SYLLABICATION_METHOD.data(),
            static_cast<long>(wxGetApp().GetAppOptions().GetNumeralSyllabicationMethod()))));
        if (GetNumeralSyllabicationMethod() != NumeralSyllabize::WholeWordIsOneSyllable &&
            GetNumeralSyllabicationMethod() != NumeralSyllabize::SoundOutEachDigit)
            {
            SetNumeralSyllabicationMethod(
                wxGetApp().GetAppOptions().GetNumeralSyllabicationMethod());
            }
        // whether to ignore blank lines when figuring out if an
        // incomplete sentences is end of paragraph
        IgnoreBlankLinesForParagraphsParser(XmlFormat::GetBoolean(
            parsingSection, parsingSectionEnd,
            wxGetApp().GetAppOptions().XML_IGNORE_BLANK_LINES_FOR_PARAGRAPH_PARSING.data(),
            wxGetApp().GetAppOptions().IsIgnoringBlankLinesForParagraphsParser()));
        // whether we should ignore indenting when parsing paragraphs
        IgnoreIndentingForParagraphsParser(XmlFormat::GetBoolean(
            parsingSection, parsingSectionEnd,
            wxGetApp().GetAppOptions().XML_IGNORE_INDENTING_FOR_PARAGRAPH_PARSING.data(),
            wxGetApp().GetAppOptions().IsIgnoringIndentingForParagraphsParser()));
        // whether sentences must start capitalized
        SetSentenceStartMustBeUppercased(XmlFormat::GetBoolean(
            parsingSection, parsingSectionEnd,
            wxGetApp().GetAppOptions().XML_SENTENCES_MUST_START_CAPITALIZED.data(),
            wxGetApp().GetAppOptions().GetSentenceStartMustBeUppercased()));
        // File path to phrases to exclude from analysis.
        SetExcludedPhrasesPath(
            XmlFormat::GetString(parsingSection, parsingSectionEnd,
                                 wxGetApp().GetAppOptions().XML_EXCLUDED_PHRASES_PATH.data()));
        LoadExcludePhrases();
        const wchar_t* exclusionBlockTagSection = html_extract_text::find_element(
            parsingSection, parsingSectionEnd, wxGetApp().GetAppOptions().XML_EXCLUDE_BLOCK_TAGS_W,
            true);
        const wchar_t* exclusionBlockTagSectionEnd =
            exclusionBlockTagSection ? html_extract_text::find_closing_element(
                                           exclusionBlockTagSection, parsingSectionEnd,
                                           wxGetApp().GetAppOptions().XML_EXCLUDE_BLOCK_TAGS_W) :
                                       nullptr;
        if (exclusionBlockTagSection && exclusionBlockTagSectionEnd)
            {
            GetExclusionBlockTags().clear();
            const wchar_t* exclusionBlockTag = exclusionBlockTagSection;
            while (exclusionBlockTag)
                {
                exclusionBlockTag = html_extract_text::find_element(
                    exclusionBlockTag, parsingSectionEnd,
                    wxGetApp().GetAppOptions().XML_EXCLUDE_BLOCK_TAG_W, true);
                if (!exclusionBlockTag)
                    {
                    break;
                    }
                const wchar_t* exclusionBlockTagEnd = html_extract_text::find_closing_element(
                    exclusionBlockTag, parsingSectionEnd,
                    wxGetApp().GetAppOptions().XML_EXCLUDE_BLOCK_TAG_W);
                if (!exclusionBlockTagEnd)
                    {
                    break;
                    }
                const wxString blockTags =
                    XmlFormat::GetString(exclusionBlockTag, exclusionBlockTagEnd,
                                         wxGetApp().GetAppOptions().XML_VALUE.data());
                if (blockTags.length() >= 2)
                    {
                    GetExclusionBlockTags().push_back(std::make_pair(blockTags[0], blockTags[1]));
                    }
                exclusionBlockTag = exclusionBlockTagEnd;
                }
            }
        // whether the first occurrence of an excluded phrase should be included
        IncludeExcludedPhraseFirstOccurrence(XmlFormat::GetBoolean(
            parsingSection, parsingSectionEnd,
            wxGetApp().GetAppOptions().XML_EXCLUDED_PHRASES_INCLUDE_FIRST_OCCURRENCE.data(),
            wxGetApp().GetAppOptions().IsIncludingExcludedPhraseFirstOccurrence()));
        // whether to ignore proper nouns
        ExcludeProperNouns(
            XmlFormat::GetBoolean(parsingSection, parsingSectionEnd,
                                  wxGetApp().GetAppOptions().XML_IGNORE_PROPER_NOUNS.data(),
                                  wxGetApp().GetAppOptions().IsExcludingProperNouns()));
        // whether to ignore numerals
        ExcludeNumerals(XmlFormat::GetBoolean(parsingSection, parsingSectionEnd,
                                              wxGetApp().GetAppOptions().XML_IGNORE_NUMERALS.data(),
                                              wxGetApp().GetAppOptions().IsExcludingNumerals()));
        // whether to ignore file addresses
        ExcludeFileAddresses(
            XmlFormat::GetBoolean(parsingSection, parsingSectionEnd,
                                  wxGetApp().GetAppOptions().XML_IGNORE_FILE_ADDRESSES.data(),
                                  wxGetApp().GetAppOptions().IsExcludingFileAddresses()));
        // whether to ignore trailing citations
        ExcludeTrailingCitations(
            XmlFormat::GetBoolean(parsingSection, parsingSectionEnd,
                                  wxGetApp().GetAppOptions().XML_IGNORE_CITATIONS.data(),
                                  wxGetApp().GetAppOptions().IsExcludingTrailingCitations()));
        // whether to use aggressive list deduction
        AggressiveExclusion(
            XmlFormat::GetBoolean(parsingSection, parsingSectionEnd,
                                  wxGetApp().GetAppOptions().XML_AGGRESSIVE_EXCLUSION.data(),
                                  wxGetApp().GetAppOptions().IsExcludingAggressively()));
        // whether to ignore trailing copyright notices
        ExcludeTrailingCopyrightNoticeParagraphs(XmlFormat::GetBoolean(
            parsingSection, parsingSectionEnd,
            wxGetApp().GetAppOptions().XML_IGNORE_COPYRIGHT_NOTICES.data(),
            wxGetApp().GetAppOptions().IsExcludingTrailingCopyrightNoticeParagraphs()));
        // get the method for parsing paragraphs
        SetParagraphsParsingMethod(static_cast<ParagraphParse>(XmlFormat::GetLong(
            parsingSection, parsingSectionEnd,
            wxGetApp().GetAppOptions().XML_PARAGRAPH_PARSING_METHOD.data(),
            static_cast<long>(wxGetApp().GetAppOptions().GetParagraphsParsingMethod()))));
        if (GetParagraphsParsingMethod() !=
                ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs &&
            GetParagraphsParsingMethod() != ParagraphParse::EachNewLineIsAParagraph)
            {
            SetParagraphsParsingMethod(static_cast<ParagraphParse>(
                wxGetApp().GetAppOptions().GetParagraphsParsingMethod()));
            }
        // Number of words that will make an incomplete sentence actually complete
        SetIncludeIncompleteSentencesIfLongerThanValue(XmlFormat::GetLong(
            parsingSection, parsingSectionEnd,
            wxGetApp().GetAppOptions().XML_INCLUDE_INCOMPLETE_SENTENCES_LONGER_THAN.data(),
            static_cast<long>(
                wxGetApp().GetAppOptions().GetIncludeIncompleteSentencesIfLongerThanValue())));
        // get the method for handling incomplete sentences
        SetInvalidSentenceMethod(static_cast<InvalidSentence>(XmlFormat::GetLong(
            parsingSection, parsingSectionEnd,
            wxGetApp().GetAppOptions().XML_INVALID_SENTENCE_METHOD.data(),
            static_cast<long>((docVersionNumber <= L"1.0") ?
                                  InvalidSentence::IncludeAsFullSentences :
                                  wxGetApp().GetAppOptions().GetInvalidSentenceMethod()))));
        if (static_cast<int>(GetInvalidSentenceMethod()) < 0 ||
            static_cast<int>(GetInvalidSentenceMethod()) >=
                static_cast<int>(InvalidSentence::INVALID_SENTENCE_METHOD_COUNT))
            {
            SetInvalidSentenceMethod(wxGetApp().GetAppOptions().GetInvalidSentenceMethod());
            }
        }
    else
        {
        LogMessage(wxString::Format(_(L"Warning: \"%s\" section not found in project file. "
                                      "Default configurations will be used."),
                                    currentStartTag),
                   _(L"Error"), wxOK | wxICON_ERROR);
        }

    // read in the custom tests
    currentStartTag.clear();
    currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_CUSTOM_TESTS.data());
    currentEndTag.clear();
    currentEndTag.append(L"</")
        .append(wxGetApp().GetAppOptions().XML_CUSTOM_TESTS.data())
        .append(L">");
    const wchar_t* customTestSection = std::wcsstr(settingsFileText, currentStartTag);
    const wchar_t* customTestSectionEnd = std::wcsstr(settingsFileText, currentEndTag);
    if (customTestSection && customTestSectionEnd && (customTestSection < customTestSectionEnd))
        {
        currentStartTag.clear();
        currentStartTag.append(L"<").append(
            wxGetApp().GetAppOptions().XML_CUSTOM_FAMILIAR_WORD_TEST.data());
        currentEndTag.clear();
        currentEndTag.append(L"</")
            .append(wxGetApp().GetAppOptions().XML_CUSTOM_FAMILIAR_WORD_TEST.data())
            .append(L">");
        const wchar_t* customFamiliarTestSection = std::wcsstr(customTestSection, currentStartTag);
        const wchar_t* customFamiliarTestSectionEnd = std::wcsstr(customTestSection, currentEndTag);
        while (customFamiliarTestSection && customFamiliarTestSectionEnd)
            {
            wxString testName =
                XmlFormat::GetString(customFamiliarTestSection, customFamiliarTestSectionEnd,
                                     wxGetApp().GetAppOptions().XML_TEST_NAME.data());
            wxString filePath =
                XmlFormat::GetString(customFamiliarTestSection, customFamiliarTestSectionEnd,
                                     wxGetApp().GetAppOptions().XML_FAMILIAR_WORD_FILE_PATH.data());
            if (!wxFile::Exists(filePath))
                {
                // if word list file not found, then try to search for it
                // in the subdirectories from where the project is
                wxString fileBySameNameInProjectDirectory;
                if (FindMissingFile(filePath, fileBySameNameInProjectDirectory))
                    {
                    filePath = fileBySameNameInProjectDirectory;
                    }
                }
            long testType =
                XmlFormat::GetLong(customFamiliarTestSection, customFamiliarTestSectionEnd,
                                   wxGetApp().GetAppOptions().XML_TEST_TYPE.data(), 0);
            if (testType < 0 ||
                testType >= static_cast<int>(readability::readability_test_type::TEST_TYPE_COUNT))
                {
                testType = 0;
                }
            long stemmingType =
                XmlFormat::GetLong(customFamiliarTestSection, customFamiliarTestSectionEnd,
                                   wxGetApp().GetAppOptions().XML_STEMMING_TYPE.data(), 0);
            if (stemmingType < 0 ||
                stemmingType >= static_cast<int>(stemming::stemming_type::STEMMING_TYPE_COUNT))
                {
                stemmingType = 0;
                }
            long formulaType =
                XmlFormat::GetLong(customFamiliarTestSection, customFamiliarTestSectionEnd,
                                   wxGetApp().GetAppOptions().XML_TEST_FORMULA_TYPE.data(), 0);
            if (formulaType != 0 && formulaType != 1)
                {
                formulaType = 0;
                }
            wxString formula =
                XmlFormat::GetString(customFamiliarTestSection, customFamiliarTestSectionEnd,
                                     wxGetApp().GetAppOptions().XML_TEST_FORMULA.data());
            string_util::remove_blank_lines(formula);
            formula = FormulaFormat::FormatMathExpressionFromUS(formula);
            if (formula.empty())
                {
                if (formulaType == 1)
                    {
                    formula = ReadabilityFormulaParser::GetCustomSpacheSignature();
                    }
                else
                    {
                    formula = ReadabilityFormulaParser::GetCustomNewDaleChallSignature();
                    }
                }
            // goals
            const double minGoal =
                XmlFormat::GetDouble(customFamiliarTestSection, customFamiliarTestSectionEnd,
                                     wxGetApp().GetAppOptions().XML_GOAL_MIN_VAL_GOAL.data(),
                                     std::numeric_limits<double>::quiet_NaN());
            const double maxGoal =
                XmlFormat::GetDouble(customFamiliarTestSection, customFamiliarTestSectionEnd,
                                     wxGetApp().GetAppOptions().XML_GOAL_MAX_VAL_GOAL.data(),
                                     std::numeric_limits<double>::quiet_NaN());
            // include options
            const int includeProperNouns =
                XmlFormat::GetLong(customFamiliarTestSection, customFamiliarTestSectionEnd,
                                   wxGetApp().GetAppOptions().XML_INCLUDE_PROPER_NOUNS.data(), 1);
            const bool includeNumeric =
                XmlFormat::GetBoolean(customFamiliarTestSection, customFamiliarTestSectionEnd,
                                      wxGetApp().GetAppOptions().XML_INCLUDE_NUMERIC.data(), true);
            const bool includeCustomWordList = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_INCLUDE_CUSTOM_WORD_LIST.data(), true);
            const bool includeDCTest =
                XmlFormat::GetBoolean(customFamiliarTestSection, customFamiliarTestSectionEnd,
                                      wxGetApp().GetAppOptions().XML_INCLUDE_DC_LIST.data(), false);
            const bool includeSpacheTest = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_INCLUDE_SPACHE_LIST.data(), false);
            const bool includeHJList = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_INCLUDE_HARRIS_JACOBSON_LIST.data(), false);
            const bool includeStockerList = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_INCLUDE_STOCKER_LIST.data(), false);
            const bool familiar_words_must_be_on_each_included_list = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_FAMILIAR_WORDS_ALL_LISTS.data(), false);
            // industry
            const bool industryChildrensPublishingSelected = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_INDUSTRY_CHILDRENS_PUBLISHING.data(), false);
            const bool industryAdultPublishingSelected = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_INDUSTRY_ADULTPUBLISHING.data(), false);
            const bool industrySecondaryLanguageSelected = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_INDUSTRY_SECONDARY_LANGUAGE.data(), false);
            const bool industryChildrensHealthCareSelected = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_INDUSTRY_CHILDRENS_HEALTHCARE.data(), false);
            const bool industryAdultHealthCareSelected = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_INDUSTRY_ADULT_HEALTHCARE.data(), false);
            const bool industryMilitaryGovernmentSelected = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_INDUSTRY_MILITARY_GOVERNMENT.data(), false);
            const bool industryBroadcastingSelected = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_INDUSTRY_BROADCASTING.data(), false);
            // document
            const bool documentGeneralSelected = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_DOCUMENT_GENERAL.data(), false);
            const bool documentTechSelected = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_DOCUMENT_TECHNICAL.data(), false);
            const bool documentFormSelected =
                XmlFormat::GetBoolean(customFamiliarTestSection, customFamiliarTestSectionEnd,
                                      wxGetApp().GetAppOptions().XML_DOCUMENT_FORM.data(), false);
            const bool documentYoungAdultSelected = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_DOCUMENT_YOUNGADULT.data(), false);
            const bool documentChildrenSelected = XmlFormat::GetBoolean(
                customFamiliarTestSection, customFamiliarTestSectionEnd,
                wxGetApp().GetAppOptions().XML_DOCUMENT_CHILDREN_LIT.data(), false);

            CustomReadabilityTest cTest(
                testName.wc_str(), formula.wc_str(), readability::readability_test_type(testType),
                filePath.wc_str(), stemming::stemming_type(stemmingType), includeCustomWordList,
                includeDCTest, &BaseProject::m_dale_chall_word_list, includeSpacheTest,
                &BaseProject::m_spache_word_list, includeHJList,
                &BaseProject::m_harris_jacobson_word_list, includeStockerList,
                &BaseProject::m_stocker_catholic_word_list,
                familiar_words_must_be_on_each_included_list,
                static_cast<readability::proper_noun_counting_method>(includeProperNouns),
                includeNumeric, industryChildrensPublishingSelected,
                industryAdultPublishingSelected, industrySecondaryLanguageSelected,
                industryChildrensHealthCareSelected, industryAdultHealthCareSelected,
                industryMilitaryGovernmentSelected, industryBroadcastingSelected,
                documentGeneralSelected, documentTechSelected, documentFormSelected,
                documentYoungAdultSelected, documentChildrenSelected);

            // set project to dirty just in case there is a conflict
            // with this test and a global test
            if (std::find(m_custom_word_tests.begin(), m_custom_word_tests.end(), testName) !=
                m_custom_word_tests.end())
                {
                // test with the same name was found, but dirty document if it can't
                // find a test with the same name and settings
                if (std::find(m_custom_word_tests.begin(), m_custom_word_tests.end(), cTest) ==
                    m_custom_word_tests.end())
                    {
                    SetModifiedFlag();
                    }
                }
            // add test to the global list of available tests (if it's not already in there)
            if (AddGlobalCustomReadabilityTest(cTest))
                {
                AddCustomReadabilityTest(cTest.get_name().c_str());
                }
            else
                {
                LogMessage(wxString::Format(_(L"Unable to add custom readability test \"%s\"."),
                                            cTest.get_name().c_str()),
                           _(L"Error"), wxOK | wxICON_EXCLAMATION);
                }
            // add it to the goals if it has any
            if (!std::isnan(minGoal) || !std::isnan(maxGoal))
                {
                GetTestGoals().insert({ cTest.get_name(), minGoal, maxGoal });
                }
            // go to next test
            customFamiliarTestSection = std::wcsstr(++customFamiliarTestSection, currentStartTag);
            if (!customFamiliarTestSection)
                {
                break;
                }
            customFamiliarTestSectionEnd = std::wcsstr(customFamiliarTestSection, currentEndTag);
            }
        }

    // read in the graph configurations
    currentStartTag.clear();
    currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_GRAPH_SETTINGS.data());
    currentEndTag.clear();
    currentEndTag.append(L"</")
        .append(wxGetApp().GetAppOptions().XML_GRAPH_SETTINGS.data())
        .append(L">");
    const wchar_t* graphsSection = std::wcsstr(settingsFileText, currentStartTag);
    const wchar_t* graphsSectionEnd = std::wcsstr(settingsFileText, currentEndTag);
    if (graphsSection && graphsSectionEnd && (graphsSection < graphsSectionEnd))
        {
        // color scheme
        SetGraphColorScheme(
            XmlFormat::GetString(graphsSection, graphsSectionEnd,
                                 wxGetApp().GetAppOptions().XML_GRAPH_COLOR_SCHEME.data(),
                                 wxGetApp().GetAppOptions().GetGraphColorScheme()));

        // background color and images
        SetPlotBackGroundImagePath(XmlFormat::GetString(
            graphsSection, graphsSectionEnd,
            wxGetApp().GetAppOptions().XML_GRAPH_PLOT_BACKGROUND_IMAGE_PATH.data()));

        long imageEffect = XmlFormat::GetLong(
            graphsSection, graphsSectionEnd,
            wxGetApp().GetAppOptions().XML_GRAPH_PLOT_BACKGROUND_IMAGE_EFFECT.data(),
            static_cast<int>(GetPlotBackGroundImageEffect()));
        if (imageEffect < 0 ||
            imageEffect >= static_cast<decltype(imageEffect)>(ImageEffect::IMAGE_EFFECTS_COUNT))
            {
            imageEffect = static_cast<decltype(imageEffect)>(ImageEffect::NoEffect);
            }
        SetPlotBackGroundImageEffect(static_cast<ImageEffect>(imageEffect));

        long imageFit = XmlFormat::GetLong(
            graphsSection, graphsSectionEnd,
            wxGetApp().GetAppOptions().XML_GRAPH_PLOT_BACKGROUND_IMAGE_FIT.data(),
            static_cast<int>(GetPlotBackGroundImageFit()));
        if (imageFit < 0 || imageFit >= static_cast<decltype(imageFit)>(ImageFit::IMAGE_FIT_COUNT))
            {
            imageFit = static_cast<decltype(imageFit)>(ImageFit::Shrink);
            }
        SetPlotBackGroundImageFit(static_cast<ImageFit>(imageFit));

        SetBackGroundColor(
            XmlFormat::GetColor(graphsSection, graphsSectionEnd,
                                wxGetApp().GetAppOptions().XML_GRAPH_BACKGROUND_COLOR.data(),
                                wxGetApp().GetAppOptions().GetBackGroundColor()));

        SetPlotBackGroundColor(
            XmlFormat::GetColor(graphsSection, graphsSectionEnd,
                                wxGetApp().GetAppOptions().XML_GRAPH_PLOT_BACKGROUND_COLOR.data(),
                                wxGetApp().GetAppOptions().GetPlotBackGroundColor()));

        SetPlotBackGroundImageOpacity(XmlFormat::GetLong(
            graphsSection, graphsSectionEnd,
            wxGetApp().GetAppOptions().XML_GRAPH_PLOT_BACKGROUND_IMAGE_OPACITY.data(),
            wxGetApp().GetAppOptions().GetPlotBackGroundImageOpacity()));
        SetPlotBackGroundColorOpacity(XmlFormat::GetLong(
            graphsSection, graphsSectionEnd,
            wxGetApp().GetAppOptions().XML_GRAPH_PLOT_BACKGROUND_COLOR_OPACITY.data(),
            wxGetApp().GetAppOptions().GetPlotBackGroundColorOpacity()));

        SetGraphBackGroundLinearGradient(XmlFormat::GetBoolean(
            graphsSection, graphsSectionEnd,
            wxGetApp().GetAppOptions().XML_GRAPH_BACKGROUND_LINEAR_GRADIENT.data(),
            wxGetApp().GetAppOptions().GetGraphBackGroundLinearGradient()));

        SetStippleImagePath(
            XmlFormat::GetString(graphsSection, graphsSectionEnd,
                                 wxGetApp().GetAppOptions().XML_GRAPH_STIPPLE_PATH.data()));

        SetStippleShapeColor(
            XmlFormat::GetColor(graphsSection, graphsSectionEnd,
                                wxGetApp().GetAppOptions().XML_GRAPH_STIPPLE_COLOR.data(),
                                wxGetApp().GetAppOptions().GetStippleShapeColor()));

        SetStippleShape(
            XmlFormat::GetString(graphsSection, graphsSectionEnd,
                                 wxGetApp().GetAppOptions().XML_GRAPH_STIPPLE_SHAPE.data(),
                                 // don't want empty string here, it should always be something
                                 wxGetApp().GetAppOptions().GetStippleShape()));

        SetGraphCommonImagePath(
            XmlFormat::GetString(graphsSection, graphsSectionEnd,
                                 wxGetApp().GetAppOptions().XML_GRAPH_COMMON_IMAGE_PATH.data()));

        DisplayDropShadows(
            XmlFormat::GetBoolean(graphsSection, graphsSectionEnd,
                                  wxGetApp().GetAppOptions().XML_DISPLAY_DROP_SHADOW.data(),
                                  wxGetApp().GetAppOptions().IsDisplayingDropShadows()));

        ShowcaseKeyItems(
            XmlFormat::GetBoolean(graphsSection, graphsSectionEnd,
                                  wxGetApp().GetAppOptions().XML_SHOWCASE_KEY_ITEMS.data(),
                                  wxGetApp().GetAppOptions().IsShowcasingKeyItems()));

        SetWatermark(XmlFormat::GetString(graphsSection, graphsSectionEnd,
                                          wxGetApp().GetAppOptions().XML_GRAPH_WATERMARK.data()));
        SetWatermarkLogoPath(XmlFormat::GetString(
            graphsSection, graphsSectionEnd,
            wxGetApp().GetAppOptions().XML_GRAPH_WATERMARK_LOGO_IMAGE_PATH.data()));

        // Fry/Raygor settings
        SetInvalidAreaColor(
            XmlFormat::GetColor(graphsSection, graphsSectionEnd,
                                wxGetApp().GetAppOptions().XML_INVALID_AREA_COLOR.data(),
                                wxGetApp().GetAppOptions().GetInvalidAreaColor()));

        long raygorStyle = XmlFormat::GetLong(
            graphsSection, graphsSectionEnd, wxGetApp().GetAppOptions().XML_RAYGOR_STYLE.data(),
            static_cast<int>(wxGetApp().GetAppOptions().GetRaygorStyle()));
        if (raygorStyle < 0 ||
            raygorStyle >= static_cast<decltype(raygorStyle)>(
                               Wisteria::Graphs::RaygorStyle::RAYGOR_STYLE_COUNT))
            {
            raygorStyle =
                static_cast<decltype(raygorStyle)>(Wisteria::Graphs::RaygorStyle::BaldwinKaufman);
            }
        SetRaygorStyle(static_cast<Wisteria::Graphs::RaygorStyle>(raygorStyle));

        // Lix gauge
        currentStartTag.clear();
        currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_LIX_SETTINGS.data());
        currentEndTag.clear();
        currentEndTag.append(L"</")
            .append(wxGetApp().GetAppOptions().XML_LIX_SETTINGS.data())
            .append(L">");
        const wchar_t* lixGaugeSection = std::wcsstr(graphsSection, currentStartTag);
        const wchar_t* lixGaugeSectionEnd = std::wcsstr(graphsSection, currentEndTag);
        if (lixGaugeSection && lixGaugeSectionEnd && (lixGaugeSection < lixGaugeSectionEnd))
            {
            UseEnglishLabelsForGermanLix(XmlFormat::GetBoolean(
                lixGaugeSection, lixGaugeSectionEnd,
                wxGetApp().GetAppOptions().XML_USE_ENGLISH_LABELS.data(),
                wxGetApp().GetAppOptions().IsUsingEnglishLabelsForGermanLix()));
            }

        // Flesch chart
        currentStartTag.clear();
        currentStartTag.append(L"<").append(
            wxGetApp().GetAppOptions().XML_FLESCH_CHART_SETTINGS.data());
        currentEndTag.clear();
        currentEndTag.append(L"</")
            .append(wxGetApp().GetAppOptions().XML_FLESCH_CHART_SETTINGS.data())
            .append(L">");
        const wchar_t* fleschChartSection = std::wcsstr(graphsSection, currentStartTag);
        const wchar_t* fleschChartSectionEnd = std::wcsstr(graphsSection, currentEndTag);
        if (fleschChartSection && fleschChartSectionEnd &&
            (fleschChartSection < fleschChartSectionEnd))
            {
            ConnectFleschPoints(
                XmlFormat::GetBoolean(fleschChartSection, fleschChartSectionEnd,
                                      wxGetApp().GetAppOptions().XML_INCLUDE_CONNECTION_LINE.data(),
                                      wxGetApp().GetAppOptions().IsConnectingFleschPoints()));
            IncludeFleschRulerDocGroups(XmlFormat::GetBoolean(
                fleschChartSection, fleschChartSectionEnd,
                wxGetApp().GetAppOptions().XML_FLESCH_RULER_DOC_GROUPS.data(),
                wxGetApp().GetAppOptions().IsIncludingFleschRulerDocGroups()));
            }

        // box plot settings
        currentStartTag.clear();
        currentStartTag.append(L"<").append(
            wxGetApp().GetAppOptions().XML_BOX_PLOT_SETTINGS.data());
        currentEndTag.clear();
        currentEndTag.append(L"</")
            .append(wxGetApp().GetAppOptions().XML_BOX_PLOT_SETTINGS.data())
            .append(L">");
        const wchar_t* boxPlotSection = std::wcsstr(graphsSection, currentStartTag);
        const wchar_t* boxPlotSectionEnd = std::wcsstr(graphsSection, currentEndTag);
        if (boxPlotSection && boxPlotSectionEnd && (boxPlotSection < boxPlotSectionEnd))
            {
            SetGraphBoxColor(XmlFormat::GetColor(boxPlotSection, boxPlotSectionEnd,
                                                 wxGetApp().GetAppOptions().XML_GRAPH_COLOR.data(),
                                                 wxGetApp().GetAppOptions().GetGraphBoxColor()));
            long boxEffect = XmlFormat::GetLong(
                boxPlotSection, boxPlotSectionEnd, wxGetApp().GetAppOptions().XML_BOX_EFFECT.data(),
                static_cast<long>(wxGetApp().GetAppOptions().GetGraphBoxEffect()));
            if (boxEffect < 0 ||
                boxEffect >= static_cast<decltype(boxEffect)>(BoxEffect::EFFECTS_COUNT))
                {
                boxEffect = static_cast<decltype(boxEffect)>(
                    wxGetApp().GetAppOptions().GetGraphBoxEffect());
                }
            SetGraphBoxEffect(static_cast<BoxEffect>(boxEffect));
            SetGraphBoxOpacity(
                XmlFormat::GetLong(boxPlotSection, boxPlotSectionEnd,
                                   wxGetApp().GetAppOptions().XML_GRAPH_OPACITY.data(),
                                   wxGetApp().GetAppOptions().GetGraphBoxOpacity()));
            ShowAllBoxPlotPoints(XmlFormat::GetBoolean(
                boxPlotSection, boxPlotSectionEnd,
                wxGetApp().GetAppOptions().XML_BOX_PLOT_SHOW_ALL_POINTS.data(),
                wxGetApp().GetAppOptions().IsShowingAllBoxPlotPoints()));
            DisplayBoxPlotLabels(
                XmlFormat::GetBoolean(boxPlotSection, boxPlotSectionEnd,
                                      wxGetApp().GetAppOptions().XML_BOX_DISPLAY_LABELS.data(),
                                      wxGetApp().GetAppOptions().IsDisplayingBoxPlotLabels()));
            ConnectBoxPlotMiddlePoints(XmlFormat::GetBoolean(
                boxPlotSection, boxPlotSectionEnd,
                wxGetApp().GetAppOptions().XML_BOX_CONNECT_MIDDLE_POINTS.data(),
                wxGetApp().GetAppOptions().IsConnectingBoxPlotMiddlePoints()));
            }

        // histogram settings
        currentStartTag.clear();
        currentStartTag.append(L"<").append(
            wxGetApp().GetAppOptions().XML_HISTOGRAM_SETTINGS.data());
        currentEndTag.clear();
        currentEndTag.append(L"</")
            .append(wxGetApp().GetAppOptions().XML_HISTOGRAM_SETTINGS.data())
            .append(L">");
        const wchar_t* histoSection = std::wcsstr(graphsSection, currentStartTag);
        const wchar_t* histoSectionEnd = std::wcsstr(graphsSection, currentEndTag);
        if (histoSection && histoSectionEnd && (histoSection < histoSectionEnd))
            {
            long barEffect = XmlFormat::GetLong(
                histoSection, histoSectionEnd, wxGetApp().GetAppOptions().XML_BAR_EFFECT.data(),
                static_cast<long>(wxGetApp().GetAppOptions().GetHistogramBarEffect()));
            if (barEffect < 0 ||
                barEffect >= static_cast<decltype(barEffect)>(BoxEffect::EFFECTS_COUNT))
                {
                barEffect = static_cast<decltype(barEffect)>(
                    wxGetApp().GetAppOptions().GetHistogramBarEffect());
                }
            SetHistogramBarEffect(static_cast<BoxEffect>(barEffect));
            SetHistogramBarColor(XmlFormat::GetColor(
                histoSection, histoSectionEnd, wxGetApp().GetAppOptions().XML_GRAPH_COLOR.data(),
                wxGetApp().GetAppOptions().GetHistogramBarColor()));
            SetHistogramBarOpacity(XmlFormat::GetLong(
                histoSection, histoSectionEnd, wxGetApp().GetAppOptions().XML_GRAPH_OPACITY.data(),
                wxGetApp().GetAppOptions().GetHistogramBarOpacity()));
            // how values are binned
            long catMethod = XmlFormat::GetLong(
                histoSection, histoSectionEnd,
                wxGetApp().GetAppOptions().XML_GRAPH_BINNING_METHOD.data(),
                static_cast<long>(wxGetApp().GetAppOptions().GetHistogramBinningMethod()));
            if (catMethod < 0 || catMethod >= static_cast<decltype(catMethod)>(
                                                  Histogram::BinningMethod::BINNING_METHOD_COUNT))
                {
                catMethod =
                    static_cast<long>(wxGetApp().GetAppOptions().GetHistogramBinningMethod());
                }
            SetHistogramBinningMethod(static_cast<Histogram::BinningMethod>(catMethod));
            // how values are rounded
            long roundMethod = XmlFormat::GetLong(
                histoSection, histoSectionEnd,
                wxGetApp().GetAppOptions().XML_GRAPH_ROUNDING_METHOD.data(),
                static_cast<long>(wxGetApp().GetAppOptions().GetHistogramRoundingMethod()));
            if (roundMethod >=
                static_cast<decltype(roundMethod)>(RoundingMethod::ROUNDING_METHOD_COUNT))
                {
                roundMethod =
                    static_cast<long>(wxGetApp().GetAppOptions().GetHistogramRoundingMethod());
                }
            SetHistogramRoundingMethod(static_cast<RoundingMethod>(roundMethod));
            // how the intervals are displayed on the axis and bar
            long intervalDisplayMethod = XmlFormat::GetLong(
                histoSection, histoSectionEnd,
                wxGetApp().GetAppOptions().XML_GRAPH_INTERVAL_DISPLAY.data(),
                static_cast<long>(wxGetApp().GetAppOptions().GetHistogramIntervalDisplay()));
            if (intervalDisplayMethod < 0 ||
                intervalDisplayMethod >= static_cast<decltype(intervalDisplayMethod)>(
                                             Histogram::IntervalDisplay::INTERVAL_METHOD_COUNT))
                {
                intervalDisplayMethod =
                    static_cast<long>(wxGetApp().GetAppOptions().GetHistogramIntervalDisplay());
                }
            SetHistogramIntervalDisplay(
                static_cast<Histogram::IntervalDisplay>(intervalDisplayMethod));
            // how the categories are displayed on the axis and bar
            long catDisplayMethod = XmlFormat::GetLong(
                histoSection, histoSectionEnd,
                wxGetApp().GetAppOptions().XML_GRAPH_BINNING_LABEL_DISPLAY.data(),
                static_cast<long>(wxGetApp().GetAppOptions().GetHistogramBinLabelDisplay()));
            if (catDisplayMethod < 0 ||
                catDisplayMethod >= static_cast<decltype(catDisplayMethod)>(
                                        BinLabelDisplay::BIN_LABEL_DISPLAY_COUNT))
                {
                catDisplayMethod =
                    static_cast<long>(wxGetApp().GetAppOptions().GetHistogramBinLabelDisplay());
                }
            SetHistogramBinLabelDisplay(static_cast<BinLabelDisplay>(catDisplayMethod));
            }

        // bar chart settings
        currentStartTag.clear();
        currentStartTag.append(L"<").append(
            wxGetApp().GetAppOptions().XML_BAR_CHART_SETTINGS.data());
        currentEndTag.clear();
        currentEndTag.append(L"</")
            .append(wxGetApp().GetAppOptions().XML_BAR_CHART_SETTINGS.data())
            .append(L">");
        const wchar_t* barSection = std::wcsstr(graphsSection, currentStartTag);
        const wchar_t* barSectionEnd = std::wcsstr(graphsSection, currentEndTag);
        if (barSection && barSectionEnd && (barSection < barSectionEnd))
            {
            SetBarChartBarColor(XmlFormat::GetColor(
                barSection, barSectionEnd, wxGetApp().GetAppOptions().XML_GRAPH_COLOR.data(),
                wxGetApp().GetAppOptions().GetBarChartBarColor()));
            long barEffect = XmlFormat::GetLong(
                barSection, barSectionEnd, wxGetApp().GetAppOptions().XML_BAR_EFFECT.data(),
                static_cast<long>(wxGetApp().GetAppOptions().GetGraphBarEffect()));
            if (barEffect < 0 ||
                barEffect >= static_cast<decltype(barEffect)>(BoxEffect::EFFECTS_COUNT))
                {
                barEffect = static_cast<decltype(barEffect)>(
                    wxGetApp().GetAppOptions().GetGraphBarEffect());
                }
            SetGraphBarEffect(static_cast<BoxEffect>(barEffect));
            long orientation = XmlFormat::GetLong(
                barSection, barSectionEnd, wxGetApp().GetAppOptions().XML_BAR_ORIENTATION.data(),
                static_cast<decltype(orientation)>(
                    wxGetApp().GetAppOptions().GetBarChartOrientation()));
            if (orientation >=
                static_cast<decltype(orientation)>(Wisteria::Orientation::ORIENTATION_COUNT))
                {
                orientation = static_cast<decltype(orientation)>(
                    wxGetApp().GetAppOptions().GetBarChartOrientation());
                }
            DisplayBarChartLabels(XmlFormat::GetBoolean(
                barSection, barSectionEnd, wxGetApp().GetAppOptions().XML_BAR_DISPLAY_LABELS.data(),
                wxGetApp().GetAppOptions().IsDisplayingBarChartLabels()));
            SetBarChartOrientation(static_cast<Wisteria::Orientation>(orientation));
            SetGraphBarOpacity(XmlFormat::GetLong(
                barSection, barSectionEnd, wxGetApp().GetAppOptions().XML_GRAPH_OPACITY.data(),
                wxGetApp().GetAppOptions().GetGraphBarOpacity()));
            }

        // axis settings
        currentStartTag.clear();
        currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_AXIS_SETTINGS.data());
        currentEndTag.clear();
        currentEndTag.append(L"</")
            .append(wxGetApp().GetAppOptions().XML_AXIS_SETTINGS.data())
            .append(L">");
        const wchar_t* axisSection = std::wcsstr(graphsSection, currentStartTag);
        const wchar_t* axisSectionEnd = std::wcsstr(graphsSection, currentEndTag);
        if (axisSection && axisSectionEnd && (axisSection < axisSectionEnd))
            {
            // x axis
            currentStartTag.clear();
            currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_X_AXIS.data());
            currentEndTag.clear();
            currentEndTag.append(L"</")
                .append(wxGetApp().GetAppOptions().XML_X_AXIS.data())
                .append(L">");
            const wchar_t* xAxisSection = std::wcsstr(axisSection, currentStartTag);
            const wchar_t* xAxisSectionEnd = std::wcsstr(axisSection, currentEndTag);
            if (xAxisSection && xAxisSectionEnd && (xAxisSection < xAxisSectionEnd))
                {
                SetXAxisFontColor(XmlFormat::GetColor(
                    xAxisSection, xAxisSectionEnd, wxGetApp().GetAppOptions().XML_FONT_COLOR.data(),
                    wxGetApp().GetAppOptions().GetXAxisFontColor()));
                SetXAxisFont(XmlFormat::GetFont(xAxisSection, xAxisSectionEnd,
                                                wxGetApp().GetAppOptions().XML_FONT.data(),
                                                wxGetApp().GetAppOptions().GetXAxisFont()));
                }
            // y axis
            currentStartTag.clear();
            currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_Y_AXIS.data());
            currentEndTag.clear();
            currentEndTag.append(L"</")
                .append(wxGetApp().GetAppOptions().XML_Y_AXIS.data())
                .append(L">");
            const wchar_t* yAxisSection = std::wcsstr(axisSection, currentStartTag);
            const wchar_t* yAxisSectionEnd = std::wcsstr(axisSection, currentEndTag);
            if (yAxisSection && yAxisSectionEnd && (yAxisSection < yAxisSectionEnd))
                {
                SetYAxisFontColor(XmlFormat::GetColor(
                    yAxisSection, yAxisSectionEnd, wxGetApp().GetAppOptions().XML_FONT_COLOR.data(),
                    wxGetApp().GetAppOptions().GetYAxisFontColor()));
                SetYAxisFont(XmlFormat::GetFont(yAxisSection, yAxisSectionEnd,
                                                wxGetApp().GetAppOptions().XML_FONT.data(),
                                                wxGetApp().GetAppOptions().GetYAxisFont()));
                }
            }

        // title settings
        currentStartTag.clear();
        currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_TITLE_SETTINGS.data());
        currentEndTag.clear();
        currentEndTag.append(L"</")
            .append(wxGetApp().GetAppOptions().XML_TITLE_SETTINGS.data())
            .append(L">");
        const wchar_t* titleSection = std::wcsstr(graphsSection, currentStartTag);
        const wchar_t* titleSectionEnd = std::wcsstr(graphsSection, currentEndTag);
        if (titleSection && titleSectionEnd && (titleSection < titleSectionEnd))
            {
            // top title
            currentStartTag.clear();
            currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_TOP_TITLE.data());
            currentEndTag.clear();
            currentEndTag.append(L"</")
                .append(wxGetApp().GetAppOptions().XML_TOP_TITLE.data())
                .append(L">");
            const wchar_t* topTitleSection = std::wcsstr(titleSection, currentStartTag);
            const wchar_t* topTitleSectionEnd = std::wcsstr(titleSection, currentEndTag);
            if (topTitleSection && topTitleSectionEnd && (topTitleSection < topTitleSectionEnd))
                {
                SetGraphTopTitleFontColor(
                    XmlFormat::GetColor(topTitleSection, topTitleSectionEnd,
                                        wxGetApp().GetAppOptions().XML_FONT_COLOR.data(),
                                        wxGetApp().GetAppOptions().GetGraphTopTitleFontColor()));
                SetGraphTopTitleFont(XmlFormat::GetFont(
                    topTitleSection, topTitleSectionEnd, wxGetApp().GetAppOptions().XML_FONT.data(),
                    wxGetApp().GetAppOptions().GetGraphTopTitleFont()));
                }

            // bottom title
            currentStartTag.clear();
            currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_BOTTOM_TITLE.data());
            currentEndTag.clear();
            currentEndTag.append(L"</")
                .append(wxGetApp().GetAppOptions().XML_BOTTOM_TITLE.data())
                .append(L">");
            const wchar_t* bottomTitleSection = std::wcsstr(titleSection, currentStartTag);
            const wchar_t* bottomTitleSectionEnd = std::wcsstr(titleSection, currentEndTag);
            if (bottomTitleSection && bottomTitleSectionEnd &&
                (bottomTitleSection < bottomTitleSectionEnd))
                {
                SetGraphBottomTitleFontColor(
                    XmlFormat::GetColor(bottomTitleSection, bottomTitleSectionEnd,
                                        wxGetApp().GetAppOptions().XML_FONT_COLOR.data(),
                                        wxGetApp().GetAppOptions().GetGraphBottomTitleFontColor()));
                SetGraphBottomTitleFont(
                    XmlFormat::GetFont(bottomTitleSection, bottomTitleSectionEnd,
                                       wxGetApp().GetAppOptions().XML_FONT.data(),
                                       wxGetApp().GetAppOptions().GetGraphBottomTitleFont()));
                }

            // left title
            currentStartTag.clear();
            currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_LEFT_TITLE.data());
            currentEndTag.clear();
            currentEndTag.append(L"</")
                .append(wxGetApp().GetAppOptions().XML_LEFT_TITLE.data())
                .append(L">");
            const wchar_t* leftTitleSection = std::wcsstr(titleSection, currentStartTag);
            const wchar_t* leftTitleSectionEnd = std::wcsstr(titleSection, currentEndTag);
            if (leftTitleSection && leftTitleSectionEnd && (leftTitleSection < leftTitleSectionEnd))
                {
                SetGraphLeftTitleFontColor(
                    XmlFormat::GetColor(leftTitleSection, leftTitleSectionEnd,
                                        wxGetApp().GetAppOptions().XML_FONT_COLOR.data(),
                                        wxGetApp().GetAppOptions().GetGraphLeftTitleFontColor()));
                SetGraphLeftTitleFont(
                    XmlFormat::GetFont(leftTitleSection, leftTitleSectionEnd,
                                       wxGetApp().GetAppOptions().XML_FONT.data(),
                                       wxGetApp().GetAppOptions().GetGraphLeftTitleFont()));
                }

            // right title
            currentStartTag.clear();
            currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_RIGHT_TITLE.data());
            currentEndTag.clear();
            currentEndTag.append(L"</")
                .append(wxGetApp().GetAppOptions().XML_RIGHT_TITLE.data())
                .append(L">");
            const wchar_t* rightTitleSection = std::wcsstr(titleSection, currentStartTag);
            const wchar_t* rightTitleSectionEnd = std::wcsstr(titleSection, currentEndTag);
            if (rightTitleSection && rightTitleSectionEnd &&
                (rightTitleSection < rightTitleSectionEnd))
                {
                SetGraphRightTitleFontColor(
                    XmlFormat::GetColor(rightTitleSection, rightTitleSectionEnd,
                                        wxGetApp().GetAppOptions().XML_FONT_COLOR.data(),
                                        wxGetApp().GetAppOptions().GetGraphRightTitleFontColor()));
                SetGraphRightTitleFont(
                    XmlFormat::GetFont(rightTitleSection, rightTitleSectionEnd,
                                       wxGetApp().GetAppOptions().XML_FONT.data(),
                                       wxGetApp().GetAppOptions().GetGraphRightTitleFont()));
                }
            }
        }

    // read stat goals
    const wchar_t* statGoalsSection = html_extract_text::find_element(
        settingsFileText, settingsFileTextEnd, wxGetApp().GetAppOptions().XML_STAT_GOALS_W, true);
    const wchar_t* statGoalsSectionEnd =
        statGoalsSection ?
            html_extract_text::find_closing_element(statGoalsSection, settingsFileTextEnd,
                                                    wxGetApp().GetAppOptions().XML_STAT_GOALS_W) :
            nullptr;
    if (statGoalsSection && statGoalsSectionEnd)
        {
        for (const auto& statGoal : GetStatGoalLabels())
            {
            auto minGoal = XmlFormat::GetAttributeDoubleValue(
                statGoalsSection, statGoalsSectionEnd, statGoal.first.first.c_str(),
                wxGetApp().GetAppOptions().XML_GOAL_MIN_VAL_GOAL.data(),
                std::numeric_limits<double>::quiet_NaN());
            auto maxGoal = XmlFormat::GetAttributeDoubleValue(
                statGoalsSection, statGoalsSectionEnd, statGoal.first.first.c_str(),
                wxGetApp().GetAppOptions().XML_GOAL_MAX_VAL_GOAL.data(),
                std::numeric_limits<double>::quiet_NaN());
            if (!std::isnan(minGoal) || !std::isnan(maxGoal))
                {
                GetStatGoals().insert({ statGoal.first.first, minGoal, maxGoal });
                }
            }
        }

    // read in the statistics configurations
    const wchar_t* statsSection =
        html_extract_text::find_element(settingsFileText, settingsFileTextEnd,
                                        wxGetApp().GetAppOptions().XML_STATISTICS_SECTION_W, true);
    const wchar_t* statsSectionEnd = statsSection ?
                                         html_extract_text::find_closing_element(
                                             statsSection, settingsFileTextEnd,
                                             wxGetApp().GetAppOptions().XML_STATISTICS_SECTION_W) :
                                         nullptr;
    if (statsSection && statsSectionEnd)
        {
        SetVarianceMethod(static_cast<VarianceMethod>(XmlFormat::GetLong(
            statsSection, statsSectionEnd, wxGetApp().GetAppOptions().XML_VARIANCE_METHOD.data(),
            static_cast<long>(wxGetApp().GetAppOptions().GetVarianceMethod()))));
        const wxString statsInfo = XmlFormat::GetString(
            statsSection, statsSectionEnd, wxGetApp().GetAppOptions().XML_STATISTICS_REPORT.data());
        if (statsInfo.length())
            {
            GetStatisticsReportInfo().Set(statsInfo);
            }
        const wxString statsResultInfo =
            XmlFormat::GetString(statsSection, statsSectionEnd,
                                 wxGetApp().GetAppOptions().XML_STATISTICS_RESULTS.data());
        if (statsResultInfo.length())
            {
            GetStatisticsInfo().Set(statsResultInfo);
            }
        }

    // read in the readability tests' configurations
    const wchar_t* readabilityTestSection = html_extract_text::find_element(
        settingsFileText, settingsFileTextEnd,
        wxGetApp().GetAppOptions().XML_READABILITY_TESTS_SECTION_W, true);
    const wchar_t* readabilityTestSectionEnd =
        readabilityTestSection ? html_extract_text::find_closing_element(
                                     readabilityTestSection, settingsFileTextEnd,
                                     wxGetApp().GetAppOptions().XML_READABILITY_TESTS_SECTION_W) :
                                 nullptr;
    if (readabilityTestSection && readabilityTestSectionEnd)
        {
        // readability score results
        IncludeScoreSummaryReport((XmlFormat::GetBoolean(
            readabilityTestSection, readabilityTestSectionEnd,
            wxGetApp().GetAppOptions().XML_INCLUDE_SCORES_SUMMARY_REPORT.data(),
            wxGetApp().GetAppOptions().IsIncludingScoreSummaryReport())));
        // grade scale/reading age display
        GetReadabilityMessageCatalog().SetReadingAgeDisplay(
            static_cast<ReadabilityMessages::ReadingAgeDisplay>(
                XmlFormat::GetLong(readabilityTestSection, readabilityTestSectionEnd,
                                   wxGetApp().GetAppOptions().XML_READING_AGE_FORMAT.data(),
                                   static_cast<long>(wxGetApp()
                                                         .GetAppOptions()
                                                         .GetReadabilityMessageCatalog()
                                                         .GetReadingAgeDisplay()))));
        GetReadabilityMessageCatalog().SetGradeScale(
            static_cast<readability::grade_scale>(XmlFormat::GetLong(
                readabilityTestSection, readabilityTestSectionEnd,
                wxGetApp().GetAppOptions().XML_READABILITY_TEST_GRADE_SCALE_DISPLAY.data(),
                static_cast<long>(
                    wxGetApp().GetAppOptions().GetReadabilityMessageCatalog().GetGradeScale()))));
        GetReadabilityMessageCatalog().SetLongGradeScaleFormat(XmlFormat::GetBoolean(
            readabilityTestSection, readabilityTestSectionEnd,
            wxGetApp().GetAppOptions().XML_READABILITY_TEST_GRADE_SCALE_LONG_FORMAT.data(),
            wxGetApp()
                .GetAppOptions()
                .GetReadabilityMessageCatalog()
                .IsUsingLongGradeScaleFormat()));

        // which standard tests are included
        for (auto& rTest : GetReadabilityTests().get_tests())
            {
            rTest.include(XmlFormat::GetBoolean(readabilityTestSection, readabilityTestSectionEnd,
                                                rTest.get_test().get_id().c_str(), false));
            auto minGoal = XmlFormat::GetAttributeDoubleValue(
                readabilityTestSection, readabilityTestSectionEnd,
                rTest.get_test().get_id().c_str(),
                wxGetApp().GetAppOptions().XML_GOAL_MIN_VAL_GOAL.data(),
                std::numeric_limits<double>::quiet_NaN());
            auto maxGoal = XmlFormat::GetAttributeDoubleValue(
                readabilityTestSection, readabilityTestSectionEnd,
                rTest.get_test().get_id().c_str(),
                wxGetApp().GetAppOptions().XML_GOAL_MAX_VAL_GOAL.data(),
                std::numeric_limits<double>::quiet_NaN());
            if (!std::isnan(minGoal) || !std::isnan(maxGoal))
                {
                GetTestGoals().insert({ rTest.get_test().get_id(), minGoal, maxGoal });
                }
            }
        // Dolch sight words
        IncludeDolchSightWords(XmlFormat::GetBoolean(
            readabilityTestSection, readabilityTestSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_SIGHT_WORDS_TEST.data(), false));

        // test-specific options
        const wchar_t* fleschKincaidOptionsSection = html_extract_text::find_element(
            readabilityTestSection, readabilityTestSectionEnd,
            wxGetApp().GetAppOptions().XML_FLESCH_KINCAID_OPTIONS_W, true);
        const wchar_t* fleschKincaidOptionsSectionEnd =
            fleschKincaidOptionsSection ?
                html_extract_text::find_closing_element(
                    fleschKincaidOptionsSection, readabilityTestSectionEnd,
                    wxGetApp().GetAppOptions().XML_FLESCH_KINCAID_OPTIONS_W) :
                nullptr;
        if (fleschKincaidOptionsSection && fleschKincaidOptionsSectionEnd)
            {
            SetFleschKincaidNumeralSyllabizeMethod(
                static_cast<FleschKincaidNumeralSyllabize>(XmlFormat::GetLong(
                    fleschKincaidOptionsSection, fleschKincaidOptionsSectionEnd,
                    wxGetApp().GetAppOptions().XML_NUMERAL_SYLLABICATION_METHOD.data(),
                    static_cast<int>(
                        wxGetApp().GetAppOptions().GetFleschKincaidNumeralSyllabizeMethod()))));
            }

        const wchar_t* fleschOptionsSection =
            html_extract_text::find_element(readabilityTestSection, readabilityTestSectionEnd,
                                            wxGetApp().GetAppOptions().XML_FLESCH_OPTIONS_W, true);
        const wchar_t* fleschOptionsSectionEnd =
            fleschOptionsSection ? html_extract_text::find_closing_element(
                                       fleschOptionsSection, readabilityTestSectionEnd,
                                       wxGetApp().GetAppOptions().XML_FLESCH_OPTIONS_W) :
                                   nullptr;
        if (fleschOptionsSection && fleschOptionsSectionEnd)
            {
            SetFleschNumeralSyllabizeMethod(static_cast<FleschNumeralSyllabize>(XmlFormat::GetLong(
                fleschOptionsSection, fleschOptionsSectionEnd,
                wxGetApp().GetAppOptions().XML_NUMERAL_SYLLABICATION_METHOD.data(),
                static_cast<int>(wxGetApp().GetAppOptions().GetFleschNumeralSyllabizeMethod()))));
            }

        const wchar_t* fogOptionsSection = html_extract_text::find_element(
            readabilityTestSection, readabilityTestSectionEnd,
            wxGetApp().GetAppOptions().XML_GUNNING_FOG_OPTIONS_W, true);
        const wchar_t* fogOptionsSectionEnd =
            fogOptionsSection ? html_extract_text::find_closing_element(
                                    fogOptionsSection, readabilityTestSectionEnd,
                                    wxGetApp().GetAppOptions().XML_GUNNING_FOG_OPTIONS_W) :
                                nullptr;
        if (fogOptionsSection && fogOptionsSectionEnd)
            {
            FogUseSentenceUnits(
                XmlFormat::GetBoolean(fogOptionsSection, fogOptionsSectionEnd,
                                      wxGetApp().GetAppOptions().XML_USE_SENTENCE_UNITS.data(),
                                      wxGetApp().GetAppOptions().IsFogUsingSentenceUnits()));
            }

        const wchar_t* hjOptionsSection = html_extract_text::find_element(
            readabilityTestSection, readabilityTestSectionEnd,
            wxGetApp().GetAppOptions().XML_HARRIS_JACOBSON_OPTIONS_W, true);
        const wchar_t* hjOptionsSectionEnd =
            hjOptionsSection ? html_extract_text::find_closing_element(
                                   hjOptionsSection, readabilityTestSectionEnd,
                                   wxGetApp().GetAppOptions().XML_HARRIS_JACOBSON_OPTIONS_W) :
                               nullptr;
        if (hjOptionsSection && hjOptionsSectionEnd)
            {
            SetHarrisJacobsonTextExclusionMode(
                static_cast<SpecializedTestTextExclusion>(XmlFormat::GetLong(
                    hjOptionsSection, hjOptionsSectionEnd,
                    wxGetApp().GetAppOptions().XML_TEXT_EXCLUSION.data(),
                    static_cast<int>(
                        wxGetApp().GetAppOptions().GetHarrisJacobsonTextExclusionMode()))));
            }

        const wchar_t* dcOptionsSection = html_extract_text::find_element(
            readabilityTestSection, readabilityTestSectionEnd,
            wxGetApp().GetAppOptions().XML_NEW_DALE_CHALL_OPTIONS_W, true);
        const wchar_t* dcOptionsSectionEnd =
            dcOptionsSection ? html_extract_text::find_closing_element(
                                   dcOptionsSection, readabilityTestSectionEnd,
                                   wxGetApp().GetAppOptions().XML_NEW_DALE_CHALL_OPTIONS_W) :
                               nullptr;
        if (dcOptionsSection && dcOptionsSectionEnd)
            {
            IncludeStockerCatholicSupplement(XmlFormat::GetBoolean(
                dcOptionsSection, dcOptionsSectionEnd,
                wxGetApp().GetAppOptions().XML_STOCKER_LIST.data(),
                wxGetApp().GetAppOptions().IsIncludingStockerCatholicSupplement()));
            SetDaleChallTextExclusionMode(
                static_cast<SpecializedTestTextExclusion>(XmlFormat::GetLong(
                    dcOptionsSection, dcOptionsSectionEnd,
                    wxGetApp().GetAppOptions().XML_TEXT_EXCLUSION.data(),
                    static_cast<int>(wxGetApp().GetAppOptions().GetDaleChallTextExclusionMode()))));
            SetDaleChallProperNounCountingMethod(
                static_cast<readability::proper_noun_counting_method>(XmlFormat::GetLong(
                    dcOptionsSection, dcOptionsSectionEnd,
                    wxGetApp().GetAppOptions().XML_PROPER_NOUN_COUNTING_METHOD.data(),
                    static_cast<int>(
                        wxGetApp().GetAppOptions().GetDaleChallProperNounCountingMethod()))));
            }
        }
    else
        {
        LogMessage(wxString::Format(_(L"Warning: \"%s\" section not found in project file. Default "
                                      "test settings will be used."),
                                    wxGetApp().GetAppOptions().XML_READABILITY_TESTS_SECTION),
                   _(L"Error"), wxOK | wxICON_ERROR);
        }

    // read in the text view configurations
    currentStartTag.clear();
    currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_TEXT_VIEWS_SECTION.data());
    currentEndTag.clear();
    currentEndTag.append(L"</")
        .append(wxGetApp().GetAppOptions().XML_TEXT_VIEWS_SECTION.data())
        .append(L">");
    const wchar_t* textViewsSection = std::wcsstr(settingsFileText, currentStartTag);
    const wchar_t* textViewsSectionEnd = std::wcsstr(settingsFileText, currentEndTag);
    if (textViewsSection && textViewsSectionEnd && (textViewsSection < textViewsSectionEnd))
        {
        m_textHighlight = static_cast<TextHighlight>(XmlFormat::GetLong(
            textViewsSection, textViewsSectionEnd,
            wxGetApp().GetAppOptions().XML_HIGHLIGHT_METHOD.data(),
            static_cast<long>(wxGetApp().GetAppOptions().GetTextHighlightMethod())));
        m_textViewHighlightColor =
            XmlFormat::GetColor(textViewsSection, textViewsSectionEnd,
                                wxGetApp().GetAppOptions().XML_HIGHLIGHTCOLOR.data(),
                                wxGetApp().GetAppOptions().GetTextHighlightColor());
        m_excludedTextHighlightColor =
            XmlFormat::GetColor(textViewsSection, textViewsSectionEnd,
                                wxGetApp().GetAppOptions().XML_EXCLUDED_HIGHLIGHTCOLOR.data(),
                                wxGetApp().GetAppOptions().GetExcludedTextHighlightColor());
        m_duplicateWordHighlightColor =
            XmlFormat::GetColor(textViewsSection, textViewsSectionEnd,
                                wxGetApp().GetAppOptions().XML_DUP_WORD_HIGHLIGHTCOLOR.data(),
                                wxGetApp().GetAppOptions().GetDuplicateWordHighlightColor());
        m_wordyPhraseHighlightColor =
            XmlFormat::GetColor(textViewsSection, textViewsSectionEnd,
                                wxGetApp().GetAppOptions().XML_WORDY_PHRASE_HIGHLIGHTCOLOR.data(),
                                wxGetApp().GetAppOptions().GetWordyPhraseHighlightColor());
        m_fontColor =
            XmlFormat::GetColor(textViewsSection, textViewsSectionEnd,
                                wxGetApp().GetAppOptions().XML_DOCUMENT_DISPLAY_FONTCOLOR.data(),
                                wxGetApp().GetAppOptions().GetTextFontColor());
        m_textViewFont =
            XmlFormat::GetFont(textViewsSection, textViewsSectionEnd,
                               wxGetApp().GetAppOptions().XML_DOCUMENT_DISPLAY_FONT.data(),
                               wxGetApp().GetAppOptions().GetTextViewFont());
        // dolch highlighting
        m_dolchConjunctionsColor = XmlFormat::GetColorWithInclusionTag(
            textViewsSection, textViewsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_CONJUNCTIONS_HIGHLIGHTCOLOR.data(),
            m_highlightDolchConjunctions, wxGetApp().GetAppOptions().GetDolchConjunctionsColor(),
            wxGetApp().GetAppOptions().IsHighlightingDolchConjunctions());
        m_dolchPrepositionsColor = XmlFormat::GetColorWithInclusionTag(
            textViewsSection, textViewsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_PREPOSITIONS_HIGHLIGHTCOLOR.data(),
            m_highlightDolchPrepositions, wxGetApp().GetAppOptions().GetDolchPrepositionsColor(),
            wxGetApp().GetAppOptions().IsHighlightingDolchPrepositions());
        m_dolchPronounsColor = XmlFormat::GetColorWithInclusionTag(
            textViewsSection, textViewsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_PRONOUNS_HIGHLIGHTCOLOR.data(),
            m_highlightDolchPronouns, wxGetApp().GetAppOptions().GetDolchPronounsColor(),
            wxGetApp().GetAppOptions().IsHighlightingDolchPronouns());
        m_dolchAdverbsColor = XmlFormat::GetColorWithInclusionTag(
            textViewsSection, textViewsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_ADVERBS_HIGHLIGHTCOLOR.data(),
            m_highlightDolchAdverbs, wxGetApp().GetAppOptions().GetDolchAdverbsColor(),
            wxGetApp().GetAppOptions().IsHighlightingDolchAdverbs());
        m_dolchAdjectivesColor = XmlFormat::GetColorWithInclusionTag(
            textViewsSection, textViewsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_ADJECTIVES_HIGHLIGHTCOLOR.data(),
            m_highlightDolchAdjectives, wxGetApp().GetAppOptions().GetDolchAdjectivesColor(),
            wxGetApp().GetAppOptions().IsHighlightingDolchAdjectives());
        m_dolchVerbsColor = XmlFormat::GetColorWithInclusionTag(
            textViewsSection, textViewsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_VERBS_HIGHLIGHTCOLOR.data(), m_highlightDolchVerbs,
            wxGetApp().GetAppOptions().GetDolchVerbsColor(),
            wxGetApp().GetAppOptions().IsHighlightingDolchVerbs());
        m_dolchNounColor = XmlFormat::GetColorWithInclusionTag(
            textViewsSection, textViewsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_NOUNS_HIGHLIGHTCOLOR.data(), m_highlightDolchNouns,
            wxGetApp().GetAppOptions().GetDolchNounsColor(),
            wxGetApp().GetAppOptions().IsHighlightingDolchNouns());
        }
    else
        {
        LogMessage(wxString::Format(_(L"Warning: \"%s\" section not found in project file. "
                                      "No highlighted text views will be displayed."),
                                    currentStartTag),
                   _(L"Error"), wxOK | wxICON_ERROR);
        }

    // fix any issues with items loaded from this file
    Wisteria::GraphItems::Label::FixFont(m_xAxisFont);
    Wisteria::GraphItems::Label::FixFont(m_yAxisFont);
    Wisteria::GraphItems::Label::FixFont(m_topTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_bottomTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_leftTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_rightTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_textViewFont);
    }

wxString BaseProjectDoc::FormatProjectSettings() const
    {
    lily_of_the_valley::html_encode_text htmlEncode;
    wxString fileText = wxEmptyString, sectionText = wxEmptyString;
    fileText.append(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<")
        .append(wxGetApp().GetAppOptions().XML_PROJECT_HEADER.data())
        .append(_DT(L" version=\""))
        .append(wxGetApp().GetDocumentVersionNumber())
        .append(L"\">\n");
    // document info
    fileText.append(L"\t<").append(wxGetApp().GetAppOptions().XML_DOCUMENT.data()).append(L">\n");
    // the version of the product saving this project
    XmlFormat::FormatSection(sectionText, _DT(L"app-version"), wxGetApp().GetAppVersion(), 2);
    fileText += sectionText;
    // where the text actually came from originally (a file or manually entered)
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_TEXT_SOURCE.data(),
                             static_cast<int>(GetTextSource()), 2);
    fileText += sectionText;
    // path to original document and optional short description
    for (size_t i = 0; i < GetSourceFilesInfo().size(); ++i)
        {
        fileText += XmlFormat::FormatSectionWithAttribute(
            wxGetApp().GetAppOptions().XML_DOCUMENT_PATH.data(), GetSourceFilesInfo().at(i).first,
            wxGetApp().GetAppOptions().XML_DESCRIPTION.data(), GetSourceFilesInfo().at(i).second,
            2);
        }
    // storage/linking
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_DOCUMENT_STORAGE_METHOD.data(),
                             static_cast<int>(GetDocumentStorageMethod()), 2);
    fileText += sectionText;

    // min doc size
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_MIN_DOC_SIZE_FOR_BATCH.data(),
                             GetMinDocWordCountForBatch(), 2);
    fileText += sectionText;

    // how file paths are shown in batch projects
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_FILE_PATH_TRUNC_MODE.data(),
                             static_cast<int>(GetFilePathTruncationMode()), 2);
    fileText += sectionText;

    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_REALTIME_UPDATE.data(),
                             IsRealTimeUpdating(), 2);
    fileText += sectionText;

    // reviewer and status
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_REVIEWER.data(),
                             GetReviewer(), 2);
    fileText += sectionText;
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_STATUS.data(), GetStatus(),
                             2);
    fileText += sectionText;

    // appended doc
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_APPENDED_DOC_PATH.data(),
                             GetAppendedDocumentFilePath(), 2);
    fileText += sectionText;

    // Project language
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_PROJECT_LANGUAGE.data(),
                             static_cast<int>(GetProjectLanguage()), 2);
    fileText += sectionText;

    // export file paths
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_EXPORT_FOLDER_PATH.data(),
                             m_exportFolder, 2);
    fileText += sectionText;
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_EXPORT_FILE_PATH.data(),
                             m_exportFile, 2);
    fileText += sectionText;
    fileText.append(L"\t</").append(wxGetApp().GetAppOptions().XML_DOCUMENT.data()).append(L">\n");

    // sentences breakdown
    fileText.append(L"\t<")
        .append(wxGetApp().GetAppOptions().XML_SENTENCES_BREAKDOWN.data())
        .append(L">\n");
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_SENTENCES_BREAKDOWN_INFO.data(),
                             GetSentencesBreakdownInfo().ToString(), 2);
    fileText += sectionText;
    fileText.append(L"\t</")
        .append(wxGetApp().GetAppOptions().XML_SENTENCES_BREAKDOWN.data())
        .append(L">\n");

    // words breakdown
    fileText.append(L"\t<")
        .append(wxGetApp().GetAppOptions().XML_WORDS_BREAKDOWN.data())
        .append(L">\n");
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_WORDS_BREAKDOWN_INFO.data(),
                             GetWordsBreakdownInfo().ToString(), 2);
    fileText += sectionText;
    fileText.append(L"\t</")
        .append(wxGetApp().GetAppOptions().XML_WORDS_BREAKDOWN.data())
        .append(L">\n");

    // grammar
    fileText.append(L"\t<").append(wxGetApp().GetAppOptions().XML_GRAMMAR.data()).append(L">\n");

    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_SPELLCHECK_IGNORE_PROPER_NOUNS.data(),
                             SpellCheckIsIgnoringProperNouns(), 3);
    fileText += sectionText;

    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_SPELLCHECK_IGNORE_UPPERCASED.data(),
                             SpellCheckIsIgnoringUppercased(), 3);
    fileText += sectionText;

    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_SPELLCHECK_IGNORE_NUMERALS.data(),
                             SpellCheckIsIgnoringNumerals(), 3);
    fileText += sectionText;

    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_SPELLCHECK_IGNORE_FILE_ADDRESSES.data(),
                             SpellCheckIsIgnoringFileAddresses(), 3);
    fileText += sectionText;

    XmlFormat::FormatSection(
        sectionText, wxGetApp().GetAppOptions().XML_SPELLCHECK_IGNORE_PROGRAMMER_CODE.data(),
        SpellCheckIsIgnoringProgrammerCode(), 3);
    fileText += sectionText;

    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_SPELLCHECK_ALLOW_COLLOQUIALISMS.data(),
                             SpellCheckIsAllowingColloquialisms(), 3);
    fileText += sectionText;

    XmlFormat::FormatSection(
        sectionText, wxGetApp().GetAppOptions().XML_SPELLCHECK_IGNORE_SOCIAL_MEDIA_TAGS.data(),
        SpellCheckIsIgnoringSocialMediaTags(), 3);
    fileText += sectionText;

    // which grammar features are included
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_GRAMMAR_INFO.data(),
                             GetGrammarInfo().ToString(), 2);
    fileText += sectionText;

    fileText.append(L"\t</").append(wxGetApp().GetAppOptions().XML_GRAMMAR.data()).append(L">\n");

    // save the parsing analysis logic
    //---------------------------
    fileText.append(L"\t<")
        .append(wxGetApp().GetAppOptions().XML_DOCUMENT_ANALYSIS_LOGIC.data())
        .append(L">\n");
    // long sentence section
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_LONG_SENTENCES.data())
        .append(L">\n");
    // method
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_LONG_SENTENCE_METHOD.data(),
                             static_cast<int>(GetLongSentenceMethod()), 3);
    fileText += sectionText;
    // length
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_LONG_SENTENCE_LENGTH.data(),
                             GetDifficultSentenceLength(), 3);
    fileText += sectionText;
    fileText.append(L"\t\t</")
        .append(wxGetApp().GetAppOptions().XML_LONG_SENTENCES.data())
        .append(L">\n");
    // numeral syllabizing
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_NUMERAL_SYLLABICATION_METHOD.data(),
                             static_cast<int>(GetNumeralSyllabicationMethod()), 2);
    fileText += sectionText;
    // whether to ignore blank lines
    XmlFormat::FormatSection(
        sectionText, wxGetApp().GetAppOptions().XML_IGNORE_BLANK_LINES_FOR_PARAGRAPH_PARSING.data(),
        IsIgnoringBlankLinesForParagraphsParser(), 2);
    fileText += sectionText;
    // whether we should ignore indenting when parsing paragraphs
    XmlFormat::FormatSection(
        sectionText, wxGetApp().GetAppOptions().XML_IGNORE_INDENTING_FOR_PARAGRAPH_PARSING.data(),
        IsIgnoringIndentingForParagraphsParser(), 2);
    fileText += sectionText;
    // whether sentences need to start capitalized
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_SENTENCES_MUST_START_CAPITALIZED.data(),
                             GetSentenceStartMustBeUppercased(), 2);
    fileText += sectionText;
    // file path to phrases to exclude from analysis
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_EXCLUDED_PHRASES_PATH.data(),
                             GetExcludedPhrasesPath(), 2);
    fileText += sectionText;
    // text block tag exclusion
    fileText.append(L"\t<")
        .append(wxGetApp().GetAppOptions().XML_EXCLUDE_BLOCK_TAGS.data())
        .append(L">\n");
    for (auto currentExcludeTag = GetExclusionBlockTags().cbegin();
         currentExcludeTag != GetExclusionBlockTags().cend(); ++currentExcludeTag)
        {
        const wchar_t excludeTagsStr[3] = { currentExcludeTag->first, currentExcludeTag->second,
                                            0 };
        fileText.append(L"\t\t<")
            .append(wxGetApp().GetAppOptions().XML_EXCLUDE_BLOCK_TAG.data())
            .append(L">\n");
        XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_VALUE.data(),
                                 htmlEncode({ excludeTagsStr, 2 }, false), 3);
        fileText += sectionText;
        fileText.append(L"\t\t</")
            .append(wxGetApp().GetAppOptions().XML_EXCLUDE_BLOCK_TAG.data())
            .append(L">\n");
        }
    fileText.append(L"\t</")
        .append(wxGetApp().GetAppOptions().XML_EXCLUDE_BLOCK_TAGS.data())
        .append(L">\n");

    // whether the first occurrence of an excluded phrase should be included
    XmlFormat::FormatSection(
        sectionText,
        wxGetApp().GetAppOptions().XML_EXCLUDED_PHRASES_INCLUDE_FIRST_OCCURRENCE.data(),
        IsIncludingExcludedPhraseFirstOccurrence(), 2);
    fileText += sectionText;
    // whether to ignore proper nouns
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_IGNORE_PROPER_NOUNS.data(),
                             IsExcludingProperNouns(), 2);
    fileText += sectionText;
    // whether to ignore numerals
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_IGNORE_NUMERALS.data(),
                             IsExcludingNumerals(), 2);
    fileText += sectionText;
    // whether to ignore file addresses
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_IGNORE_FILE_ADDRESSES.data(),
                             IsExcludingFileAddresses(), 2);
    fileText += sectionText;
    // whether to ignore trailing citations
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_IGNORE_CITATIONS.data(),
                             IsExcludingTrailingCitations(), 2);
    fileText += sectionText;
    // whether to use aggressive list deduction
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_AGGRESSIVE_EXCLUSION.data(),
                             IsExcludingAggressively(), 2);
    fileText += sectionText;
    // whether to ignore trailing copyright notices
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_IGNORE_COPYRIGHT_NOTICES.data(),
                             IsExcludingTrailingCopyrightNoticeParagraphs(), 2);
    fileText += sectionText;
    // paragraph parsing
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_PARAGRAPH_PARSING_METHOD.data(),
                             static_cast<int>(GetParagraphsParsingMethod()), 2);
    fileText += sectionText;
    // Number of words that will make an incomplete sentence actually complete
    XmlFormat::FormatSection(
        sectionText, wxGetApp().GetAppOptions().XML_INCLUDE_INCOMPLETE_SENTENCES_LONGER_THAN.data(),
        GetIncludeIncompleteSentencesIfLongerThanValue(), 2);
    fileText += sectionText;
    // invalid sentence handling
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_INVALID_SENTENCE_METHOD.data(),
                             static_cast<int>(GetInvalidSentenceMethod()), 2);
    fileText += sectionText;
    fileText.append(L"\t</")
        .append(wxGetApp().GetAppOptions().XML_DOCUMENT_ANALYSIS_LOGIC.data())
        .append(L">\n");

    // custom tests settings
    fileText.append(L"\t<")
        .append(wxGetApp().GetAppOptions().XML_CUSTOM_TESTS.data())
        .append(L">\n");
    for (std::vector<CustomReadabilityTestInterface>::const_iterator pos =
             GetCustTestsInUse().begin();
         pos != GetCustTestsInUse().end(); ++pos)
        {
        fileText.append(L"\t\t<")
            .append(wxGetApp().GetAppOptions().XML_CUSTOM_FAMILIAR_WORD_TEST.data())
            .append(L">\n");
        // name
        const wxString testName = pos->GetIterator()->get_name().c_str();
        XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_TEST_NAME.data(),
                                 htmlEncode({ testName.wc_str(), testName.length() }, false), 3);
        fileText += sectionText;
        // file path
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_FAMILIAR_WORD_FILE_PATH.data(),
            wxString(pos->GetIterator()->get_familiar_word_list_file_path().c_str()), 3);
        fileText += sectionText;
        // test type
        XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_TEST_TYPE.data(),
                                 static_cast<int>(pos->GetIterator()->get_test_type()), 3);
        fileText += sectionText;
        // stemming type
        XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_STEMMING_TYPE.data(),
                                 static_cast<int>(pos->GetIterator()->get_stemming_type()), 3);
        fileText += sectionText;
        // formula
        const wxString formula(
            FormulaFormat::FormatMathExpressionToUS(pos->GetIterator()->get_formula().c_str()));
        XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_TEST_FORMULA.data(),
                                 htmlEncode({ formula.wc_str(), formula.length() }, false), 3);
        fileText += sectionText;
        // formula type (this is just needed for forward compatibility)
        const int formulaType =
            (wxString(pos->GetIterator()->get_formula().c_str())
                 .CmpNoCase(ReadabilityFormulaParser::GetCustomSpacheSignature()) == 0) ?
                1 :
                0;
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_TEST_FORMULA_TYPE.data(), formulaType, 3);
        fileText += sectionText;
        // goals
        const auto [minGoal, maxGoal] = GetGoalsForTest(pos->GetIterator()->get_name().c_str());
        const wxString minGoalStr =
            std::isnan(minGoal) ? wxString{} : wxString::FromCDouble(minGoal);
        const wxString maxGoalStr =
            std::isnan(maxGoal) ? wxString{} : wxString::FromCDouble(maxGoal);
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_GOAL_MIN_VAL_GOAL.data(), minGoalStr, 3);
        fileText += sectionText;
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_GOAL_MAX_VAL_GOAL.data(), maxGoalStr, 3);
        fileText += sectionText;
        // inclusion of proper nouns and numbers
        XmlFormat::FormatSection(sectionText,
                                 wxGetApp().GetAppOptions().XML_INCLUDE_PROPER_NOUNS.data(),
                                 static_cast<int>(pos->GetIterator()->get_proper_noun_method()), 3);
        fileText += sectionText;
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_INCLUDE_NUMERIC.data(),
            int_to_bool(pos->GetIterator()->is_including_numeric_as_familiar()), 3);
        fileText += sectionText;
        // include of other tests
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_INCLUDE_CUSTOM_WORD_LIST.data(),
            int_to_bool(pos->GetIterator()->is_including_custom_familiar_word_list()), 3);
        fileText += sectionText;
        XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_INCLUDE_DC_LIST.data(),
                                 int_to_bool(pos->GetIterator()->is_including_dale_chall_list()),
                                 3);
        fileText += sectionText;
        XmlFormat::FormatSection(sectionText,
                                 wxGetApp().GetAppOptions().XML_INCLUDE_SPACHE_LIST.data(),
                                 int_to_bool(pos->GetIterator()->is_including_spache_list()), 3);
        fileText += sectionText;
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_INCLUDE_HARRIS_JACOBSON_LIST.data(),
            int_to_bool(pos->GetIterator()->is_including_harris_jacobson_list()), 3);
        fileText += sectionText;
        XmlFormat::FormatSection(sectionText,
                                 wxGetApp().GetAppOptions().XML_INCLUDE_STOCKER_LIST.data(),
                                 int_to_bool(pos->GetIterator()->is_including_stocker_list()), 3);
        fileText += sectionText;
        // whether familiar words have to be on each included list
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_FAMILIAR_WORDS_ALL_LISTS.data(),
            int_to_bool(pos->GetIterator()->is_familiar_words_must_be_on_each_included_list()), 3);
        fileText += sectionText;
        // industry association
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_INDUSTRY_CHILDRENS_PUBLISHING.data(),
            int_to_bool(pos->GetIterator()->has_industry_classification(
                readability::industry_classification::childrens_publishing_industry)),
            3);
        fileText += sectionText;
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_INDUSTRY_ADULTPUBLISHING.data(),
            int_to_bool(pos->GetIterator()->has_industry_classification(
                readability::industry_classification::adult_publishing_industry)),
            3);
        fileText += sectionText;
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_INDUSTRY_SECONDARY_LANGUAGE.data(),
            int_to_bool(pos->GetIterator()->has_industry_classification(
                readability::industry_classification::sedondary_language_industry)),
            3);
        fileText += sectionText;
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_INDUSTRY_CHILDRENS_HEALTHCARE.data(),
            int_to_bool(pos->GetIterator()->has_industry_classification(
                readability::industry_classification::childrens_healthcare_industry)),
            3);
        fileText += sectionText;
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_INDUSTRY_ADULT_HEALTHCARE.data(),
            int_to_bool(pos->GetIterator()->has_industry_classification(
                readability::industry_classification::adult_healthcare_industry)),
            3);
        fileText += sectionText;
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_INDUSTRY_MILITARY_GOVERNMENT.data(),
            int_to_bool(pos->GetIterator()->has_industry_classification(
                readability::industry_classification::military_government_industry)),
            3);
        fileText += sectionText;
        XmlFormat::FormatSection(sectionText,
                                 wxGetApp().GetAppOptions().XML_INDUSTRY_BROADCASTING.data(),
                                 int_to_bool(pos->GetIterator()->has_industry_classification(
                                     readability::industry_classification::broadcasting_industry)),
                                 3);
        fileText += sectionText;
        XmlFormat::FormatSection(sectionText,
                                 wxGetApp().GetAppOptions().XML_DOCUMENT_GENERAL.data(),
                                 int_to_bool(pos->GetIterator()->has_document_classification(
                                     readability::document_classification::general_document)),
                                 3);
        fileText += sectionText;
        XmlFormat::FormatSection(sectionText,
                                 wxGetApp().GetAppOptions().XML_DOCUMENT_TECHNICAL.data(),
                                 int_to_bool(pos->GetIterator()->has_document_classification(
                                     readability::document_classification::technical_document)),
                                 3);
        fileText += sectionText;
        XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_DOCUMENT_FORM.data(),
                                 int_to_bool(pos->GetIterator()->has_document_classification(
                                     readability::document_classification::nonnarrative_document)),
                                 3);
        fileText += sectionText;
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_DOCUMENT_YOUNGADULT.data(),
            int_to_bool(pos->GetIterator()->has_document_classification(
                readability::document_classification::adult_literature_document)),
            3);
        fileText += sectionText;
        XmlFormat::FormatSection(
            sectionText, wxGetApp().GetAppOptions().XML_DOCUMENT_CHILDREN_LIT.data(),
            int_to_bool(pos->GetIterator()->has_document_classification(
                readability::document_classification::childrens_literature_document)),
            3);
        fileText += sectionText;

        fileText.append(L"\t\t</")
            .append(wxGetApp().GetAppOptions().XML_CUSTOM_FAMILIAR_WORD_TEST.data())
            .append(L">\n");
        }
    fileText.append(L"\t</")
        .append(wxGetApp().GetAppOptions().XML_CUSTOM_TESTS.data())
        .append(L">\n");

    // save the graph settings
    //----------------------------------
    fileText.append(L"\t<")
        .append(wxGetApp().GetAppOptions().XML_GRAPH_SETTINGS.data())
        .append(L">\n");
    // color scheme
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_GRAPH_COLOR_SCHEME.data())
        .append(L">");
    fileText += GetGraphColorScheme();
    fileText.append(L"</")
        .append(wxGetApp().GetAppOptions().XML_GRAPH_COLOR_SCHEME.data())
        .append(L">\n");
    // background image
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_GRAPH_PLOT_BACKGROUND_IMAGE_PATH.data())
        .append(L">");
    fileText += GetPlotBackGroundImagePath();
    fileText.append(L"</")
        .append(wxGetApp().GetAppOptions().XML_GRAPH_PLOT_BACKGROUND_IMAGE_PATH.data())
        .append(L">\n");

    XmlFormat::FormatSection(
        sectionText, wxGetApp().GetAppOptions().XML_GRAPH_PLOT_BACKGROUND_IMAGE_EFFECT.data(),
        static_cast<int>(GetPlotBackGroundImageEffect()), 3);
    fileText += sectionText;

    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_GRAPH_PLOT_BACKGROUND_IMAGE_FIT.data(),
                             static_cast<int>(GetPlotBackGroundImageFit()), 3);
    fileText += sectionText;
    // background color
    fileText.append(L"\t\t<").append(wxGetApp().GetAppOptions().XML_GRAPH_BACKGROUND_COLOR.data());
    fileText += XmlFormat::FormatColorAttributes(GetBackGroundColor());
    fileText += L"/>\n";
    // plot background color
    fileText.append(L"\t\t<").append(
        wxGetApp().GetAppOptions().XML_GRAPH_PLOT_BACKGROUND_COLOR.data());
    fileText += XmlFormat::FormatColorAttributes(GetPlotBackGroundColor());
    fileText.append(L"/>\n");
    // plot background image opacity
    XmlFormat::FormatSection(
        sectionText, wxGetApp().GetAppOptions().XML_GRAPH_PLOT_BACKGROUND_IMAGE_OPACITY.data(),
        static_cast<int>(GetPlotBackGroundImageOpacity()), 2);
    fileText += sectionText;
    // plot background color opacity
    XmlFormat::FormatSection(
        sectionText, wxGetApp().GetAppOptions().XML_GRAPH_PLOT_BACKGROUND_COLOR_OPACITY.data(),
        static_cast<int>(GetPlotBackGroundColorOpacity()), 2);
    fileText += sectionText;
    // background linear gradient
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_GRAPH_BACKGROUND_LINEAR_GRADIENT.data(),
                             GetGraphBackGroundLinearGradient(), 2);
    fileText += sectionText;
    // stipple image
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_GRAPH_STIPPLE_PATH.data(),
                             GetStippleImagePath(), 2);
    fileText += sectionText;
    // stipple shape
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_GRAPH_STIPPLE_SHAPE.data(),
                             GetStippleShape(), 2);
    fileText += sectionText;

    fileText.append(L"\t\t<").append(wxGetApp().GetAppOptions().XML_GRAPH_STIPPLE_COLOR.data());
    fileText += XmlFormat::FormatColorAttributes(GetStippleShapeColor());
    fileText += L"/>\n";
    // common image
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_GRAPH_COMMON_IMAGE_PATH.data(),
                             GetGraphCommonImagePath(), 2);
    fileText += sectionText;
    // whether drop shadows should be shown
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_DISPLAY_DROP_SHADOW.data(),
                             IsDisplayingDropShadows(), 2);
    fileText += sectionText;
    // whether to draw attention to the complex word groups in syllable graphs
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_SHOWCASE_KEY_ITEMS.data(),
                             IsShowcasingKeyItems(), 2);
    fileText += sectionText;
    // watermarks
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_GRAPH_WATERMARK.data(),
                             GetWatermark(), 2);
    fileText += sectionText;
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_GRAPH_WATERMARK_LOGO_IMAGE_PATH.data(),
                             GetWatermarkLogoPath(), 2);
    fileText += sectionText;
    // lix gauge
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_LIX_SETTINGS.data())
        .append(L">\n");
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_USE_ENGLISH_LABELS.data(),
                             IsUsingEnglishLabelsForGermanLix(), 3);
    fileText += sectionText;
    fileText.append(L"\t\t</")
        .append(wxGetApp().GetAppOptions().XML_LIX_SETTINGS.data())
        .append(L">\n");
    // flesch chart
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_FLESCH_CHART_SETTINGS.data())
        .append(L">\n");
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_INCLUDE_CONNECTION_LINE.data(),
                             IsConnectingFleschPoints(), 3);
    fileText += sectionText;
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_FLESCH_RULER_DOC_GROUPS.data(),
                             IsIncludingFleschRulerDocGroups(), 3);
    fileText += sectionText;
    fileText.append(L"\t\t</")
        .append(wxGetApp().GetAppOptions().XML_FLESCH_CHART_SETTINGS.data())
        .append(L">\n");
    // Fry/Raygor invalid area colors
    fileText.append(L"\t\t<").append(wxGetApp().GetAppOptions().XML_INVALID_AREA_COLOR.data());
    fileText += XmlFormat::FormatColorAttributes(GetInvalidAreaColor());
    fileText.append(L"/>\n");
    // Raygor style
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_RAYGOR_STYLE.data(),
                             static_cast<int>(GetRaygorStyle()), 3);
    fileText += sectionText;

    // histogram settings
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_HISTOGRAM_SETTINGS.data())
        .append(L">\n");
    // categorization method
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_GRAPH_BINNING_METHOD.data(),
                             static_cast<int>(GetHistogramBinningMethod()), 3);
    fileText += sectionText;
    // interval display method
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_GRAPH_INTERVAL_DISPLAY.data(),
                             static_cast<int>(GetHistogramIntervalDisplay()), 3);
    fileText += sectionText;
    // categorization label display method
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_GRAPH_BINNING_LABEL_DISPLAY.data(),
                             static_cast<int>(GetHistogramBinLabelDisplay()), 3);
    fileText += sectionText;
    // rounding
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_GRAPH_ROUNDING_METHOD.data(),
                             static_cast<int>(GetHistogramRoundingMethod()), 3);
    fileText += sectionText;
    // bar color
    fileText.append(L"\t\t\t<").append(wxGetApp().GetAppOptions().XML_GRAPH_COLOR.data());
    fileText += XmlFormat::FormatColorAttributes(GetHistogramBarColor());
    fileText.append(L"/>\n");
    // bar opacity
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_GRAPH_OPACITY.data(),
                             static_cast<int>(GetHistogramBarOpacity()), 3);
    fileText += sectionText;
    // bar effect
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_BAR_EFFECT.data(),
                             static_cast<int>(GetHistogramBarEffect()), 3);
    fileText += sectionText;
    fileText.append(L"\t\t</")
        .append(wxGetApp().GetAppOptions().XML_HISTOGRAM_SETTINGS.data())
        .append(L">\n");

    // bar chart settings
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_BAR_CHART_SETTINGS.data())
        .append(L">\n");
    // bar color
    fileText.append(L"\t\t\t<").append(wxGetApp().GetAppOptions().XML_GRAPH_COLOR.data());
    fileText += XmlFormat::FormatColorAttributes(GetBarChartBarColor());
    fileText.append(L"/>\n");
    // bar orientation
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_BAR_ORIENTATION.data(),
                             static_cast<int>(GetBarChartOrientation()), 3);
    fileText += sectionText;
    // bar opacity
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_GRAPH_OPACITY.data(),
                             static_cast<int>(GetGraphBarOpacity()), 3);
    fileText += sectionText;
    // bar display labels
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_BAR_DISPLAY_LABELS.data(),
                             static_cast<int>(IsDisplayingBarChartLabels()), 3);
    fileText += sectionText;
    // bar effect
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_BAR_EFFECT.data(),
                             static_cast<int>(GetGraphBarEffect()), 3);
    fileText += sectionText;
    fileText.append(L"\t\t</")
        .append(wxGetApp().GetAppOptions().XML_BAR_CHART_SETTINGS.data())
        .append(L">\n");

    // box plot settings
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_BOX_PLOT_SETTINGS.data())
        .append(L">\n");
    // box color
    fileText.append(L"\t\t\t<").append(wxGetApp().GetAppOptions().XML_GRAPH_COLOR.data());
    fileText += XmlFormat::FormatColorAttributes(GetGraphBoxColor());
    fileText.append(L"/>\n");
    // box opacity
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_GRAPH_OPACITY.data(),
                             static_cast<int>(GetGraphBoxOpacity()), 3);
    fileText += sectionText;
    // box effect
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_BOX_EFFECT.data(),
                             static_cast<int>(GetGraphBoxEffect()), 3);
    fileText += sectionText;
    // box show all points
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_BOX_PLOT_SHOW_ALL_POINTS.data(),
                             static_cast<int>(IsShowingAllBoxPlotPoints()), 3);
    fileText += sectionText;
    // box connection points
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_BOX_CONNECT_MIDDLE_POINTS.data(),
                             static_cast<int>(IsConnectingBoxPlotMiddlePoints()), 3);
    fileText += sectionText;
    // box display labels
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_BOX_DISPLAY_LABELS.data(),
                             static_cast<int>(IsDisplayingBoxPlotLabels()), 3);
    fileText += sectionText;
    fileText.append(L"\t\t</")
        .append(wxGetApp().GetAppOptions().XML_BOX_PLOT_SETTINGS.data())
        .append(L">\n");

    // axis settings
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_AXIS_SETTINGS.data())
        .append(L">\n");
    // x axis
    fileText.append(L"\t\t\t<").append(wxGetApp().GetAppOptions().XML_X_AXIS.data()).append(L">\n");
    // font color
    fileText.append(L"\t\t\t\t<").append(wxGetApp().GetAppOptions().XML_FONT_COLOR.data());
    fileText += XmlFormat::FormatColorAttributes(GetXAxisFontColor());
    fileText.append(L"/>\n");
    // font information
    fileText.append(L"\t\t\t\t<").append(wxGetApp().GetAppOptions().XML_FONT.data());
    fileText += XmlFormat::FormatFontAttributes(GetXAxisFont());
    fileText.append(L"/>\n");
    fileText.append(L"\t\t\t</")
        .append(wxGetApp().GetAppOptions().XML_X_AXIS.data())
        .append(L">\n");
    // y axis
    fileText.append(L"\t\t\t<").append(wxGetApp().GetAppOptions().XML_Y_AXIS.data()).append(L">\n");
    // font color
    fileText.append(L"\t\t\t\t<").append(wxGetApp().GetAppOptions().XML_FONT_COLOR.data());
    fileText += XmlFormat::FormatColorAttributes(GetYAxisFontColor());
    fileText.append(L"/>\n");
    // font information
    fileText.append(L"\t\t\t\t<").append(wxGetApp().GetAppOptions().XML_FONT.data());
    fileText += XmlFormat::FormatFontAttributes(GetYAxisFont());
    fileText.append(L"/>\n");
    fileText.append(L"\t\t\t</")
        .append(wxGetApp().GetAppOptions().XML_Y_AXIS.data())
        .append(L">\n");
    fileText.append(L"\t\t</")
        .append(wxGetApp().GetAppOptions().XML_AXIS_SETTINGS.data())
        .append(L">\n");

    // title settings
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_TITLE_SETTINGS.data())
        .append(L">\n");
    // top title
    fileText.append(L"\t\t\t<")
        .append(wxGetApp().GetAppOptions().XML_TOP_TITLE.data())
        .append(L">\n");
    // font color
    fileText.append(L"\t\t\t\t<").append(wxGetApp().GetAppOptions().XML_FONT_COLOR.data());
    fileText += XmlFormat::FormatColorAttributes(GetGraphTopTitleFontColor());
    fileText.append(L"/>\n");
    // font information
    fileText.append(L"\t\t\t\t<").append(wxGetApp().GetAppOptions().XML_FONT.data());
    fileText += XmlFormat::FormatFontAttributes(GetGraphTopTitleFont());
    fileText.append(L"/>\n");
    fileText.append(L"\t\t\t</")
        .append(wxGetApp().GetAppOptions().XML_TOP_TITLE.data())
        .append(L">\n");
    // bottom title
    fileText.append(L"\t\t\t<")
        .append(wxGetApp().GetAppOptions().XML_BOTTOM_TITLE.data())
        .append(L">\n");
    // font color
    fileText.append(L"\t\t\t\t<").append(wxGetApp().GetAppOptions().XML_FONT_COLOR.data());
    fileText += XmlFormat::FormatColorAttributes(GetGraphBottomTitleFontColor());
    fileText.append(L"/>\n");
    // font information
    fileText.append(L"\t\t\t\t<").append(wxGetApp().GetAppOptions().XML_FONT.data());
    fileText += XmlFormat::FormatFontAttributes(GetGraphBottomTitleFont());
    fileText.append(L"/>\n");
    fileText.append(L"\t\t\t</")
        .append(wxGetApp().GetAppOptions().XML_BOTTOM_TITLE.data())
        .append(L">\n");
    // left title
    fileText.append(L"\t\t\t<")
        .append(wxGetApp().GetAppOptions().XML_LEFT_TITLE.data())
        .append(L">\n");
    // font color
    fileText.append(L"\t\t\t\t<").append(wxGetApp().GetAppOptions().XML_FONT_COLOR.data());
    fileText += XmlFormat::FormatColorAttributes(GetGraphLeftTitleFontColor());
    fileText.append(L"/>\n");
    // font information
    fileText.append(L"\t\t\t\t<").append(wxGetApp().GetAppOptions().XML_FONT.data());
    fileText += XmlFormat::FormatFontAttributes(GetGraphLeftTitleFont());
    fileText.append(L"/>\n");
    fileText.append(L"\t\t\t</")
        .append(wxGetApp().GetAppOptions().XML_LEFT_TITLE.data())
        .append(L">\n");
    // right title
    fileText.append(L"\t\t\t<")
        .append(wxGetApp().GetAppOptions().XML_RIGHT_TITLE.data())
        .append(L">\n");
    // font color
    fileText.append(L"\t\t\t\t<").append(wxGetApp().GetAppOptions().XML_FONT_COLOR.data());
    fileText += XmlFormat::FormatColorAttributes(GetGraphRightTitleFontColor());
    fileText.append(L"/>\n");
    // font information
    fileText.append(L"\t\t\t\t<").append(wxGetApp().GetAppOptions().XML_FONT.data());
    fileText += XmlFormat::FormatFontAttributes(GetGraphRightTitleFont());
    fileText.append(L"/>\n");
    fileText.append(L"\t\t\t</")
        .append(wxGetApp().GetAppOptions().XML_RIGHT_TITLE.data())
        .append(L">\n");
    fileText.append(L"\t\t</")
        .append(wxGetApp().GetAppOptions().XML_TITLE_SETTINGS.data())
        .append(L">\n");

    fileText.append(L"\t</")
        .append(wxGetApp().GetAppOptions().XML_GRAPH_SETTINGS.data())
        .append(L">\n");

    // stats goals
    fileText.append(L"\t<").append(wxGetApp().GetAppOptions().XML_STAT_GOALS.data()).append(L">\n");
    for (const auto& statGoal : GetStatGoalLabels())
        {
        const auto [minGoal, maxGoal] = GetGoalsForStatistic(statGoal.first.first.c_str());
        const wxString minGoalStr =
            std::isnan(minGoal) ? wxString{} : wxString::FromCDouble(minGoal);
        const wxString maxGoalStr =
            std::isnan(maxGoal) ? wxString{} : wxString::FromCDouble(maxGoal);

        fileText.append(L"\t\t<")
            .append(statGoal.first.first.c_str())
            .append(L" ")
            .append(wxGetApp().GetAppOptions().XML_GOAL_MIN_VAL_GOAL.data())
            .append(L"=\"")
            .append(minGoalStr)
            .append(L"\" ")
            .append(wxGetApp().GetAppOptions().XML_GOAL_MAX_VAL_GOAL.data())
            .append(L"=\"")
            .append(maxGoalStr)
            .append(L"\"/>\n");
        }
    fileText.append(L"\t</")
        .append(wxGetApp().GetAppOptions().XML_STAT_GOALS.data())
        .append(L">\n");

    // save the statistics
    fileText.append(L"\t<")
        .append(wxGetApp().GetAppOptions().XML_STATISTICS_SECTION.data())
        .append(L">\n");
    // stats results
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_STATISTICS_RESULTS.data(),
                             GetStatisticsInfo().ToString(), 2);
    fileText += sectionText;
    // stats report
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_STATISTICS_REPORT.data(),
                             GetStatisticsReportInfo().ToString(), 2);
    fileText += sectionText;
    // variance method
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_VARIANCE_METHOD.data(),
                             static_cast<int>(GetVarianceMethod()), 2);
    fileText += sectionText;
    fileText.append(L"\t</")
        .append(wxGetApp().GetAppOptions().XML_STATISTICS_SECTION.data())
        .append(L">\n");

    fileText.append(L"\t<")
        .append(wxGetApp().GetAppOptions().XML_READABILITY_TESTS_SECTION.data())
        .append(L">\n");
    // readability score results
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_INCLUDE_SCORES_SUMMARY_REPORT.data(),
                             IsIncludingScoreSummaryReport(), 2);
    fileText += sectionText;
    // grade scale/reading age display
    XmlFormat::FormatSection(
        sectionText, wxGetApp().GetAppOptions().XML_READING_AGE_FORMAT.data(),
        static_cast<int>(GetReadabilityMessageCatalog().GetReadingAgeDisplay()), 2);
    fileText += sectionText;

    XmlFormat::FormatSection(
        sectionText, wxGetApp().GetAppOptions().XML_READABILITY_TEST_GRADE_SCALE_DISPLAY.data(),
        static_cast<int>(GetReadabilityMessageCatalog().GetGradeScale()), 2);
    fileText += sectionText;

    XmlFormat::FormatSection(
        sectionText, wxGetApp().GetAppOptions().XML_READABILITY_TEST_GRADE_SCALE_LONG_FORMAT.data(),
        GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat(), 2);
    fileText += sectionText;
    // test-specific options
    // Flesch-Kincaid
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_FLESCH_KINCAID_OPTIONS.data())
        .append(L">\n");
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_NUMERAL_SYLLABICATION_METHOD.data(),
                             static_cast<int>(GetFleschKincaidNumeralSyllabizeMethod()), 3);
    fileText += sectionText;
    fileText.append(L"\t\t</")
        .append(wxGetApp().GetAppOptions().XML_FLESCH_KINCAID_OPTIONS.data())
        .append(L">\n");
    // Flesch
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_FLESCH_OPTIONS.data())
        .append(L">\n");
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_NUMERAL_SYLLABICATION_METHOD.data(),
                             static_cast<int>(GetFleschNumeralSyllabizeMethod()), 3);
    fileText += sectionText;
    fileText.append(L"\t\t</")
        .append(wxGetApp().GetAppOptions().XML_FLESCH_OPTIONS.data())
        .append(L">\n");
    // Fog
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_GUNNING_FOG_OPTIONS.data())
        .append(L">\n");
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_USE_SENTENCE_UNITS.data(),
                             IsFogUsingSentenceUnits(), 3);
    fileText += sectionText;
    fileText.append(L"\t\t</")
        .append(wxGetApp().GetAppOptions().XML_GUNNING_FOG_OPTIONS.data())
        .append(L">\n");
    // HJ
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_HARRIS_JACOBSON_OPTIONS.data())
        .append(L">\n");
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_TEXT_EXCLUSION.data(),
                             static_cast<int>(GetHarrisJacobsonTextExclusionMode()), 3);
    fileText += sectionText;
    fileText.append(L"\t\t</")
        .append(wxGetApp().GetAppOptions().XML_HARRIS_JACOBSON_OPTIONS.data())
        .append(L">\n");
    // DC
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_NEW_DALE_CHALL_OPTIONS.data())
        .append(L">\n");
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_STOCKER_LIST.data(),
                             IsIncludingStockerCatholicSupplement(), 3);
    fileText += sectionText;
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_TEXT_EXCLUSION.data(),
                             static_cast<int>(GetDaleChallTextExclusionMode()), 3);
    fileText += sectionText;
    XmlFormat::FormatSection(sectionText,
                             wxGetApp().GetAppOptions().XML_PROPER_NOUN_COUNTING_METHOD.data(),
                             static_cast<int>(GetDaleChallProperNounCountingMethod()), 3);
    fileText += sectionText;
    fileText.append(L"\t\t</")
        .append(wxGetApp().GetAppOptions().XML_NEW_DALE_CHALL_OPTIONS.data())
        .append(L">\n");
    // save the standard readability tests
    for (const auto& rTest : GetReadabilityTests().get_tests())
        {
        const auto [minGoal, maxGoal] = GetGoalsForTest(rTest.get_test().get_id().c_str());
        const wxString minGoalStr =
            std::isnan(minGoal) ? wxString{} : wxString::FromCDouble(minGoal);
        const wxString maxGoalStr =
            std::isnan(maxGoal) ? wxString{} : wxString::FromCDouble(maxGoal);

        fileText.append(L"\t\t<")
            .append(rTest.get_test().get_id().c_str())
            .append(L" ")
            .append(wxGetApp().GetAppOptions().XML_GOAL_MIN_VAL_GOAL.data())
            .append(L"=\"")
            .append(minGoalStr)
            .append(L"\" ")
            .append(wxGetApp().GetAppOptions().XML_GOAL_MAX_VAL_GOAL.data())
            .append(L"=\"")
            .append(maxGoalStr)
            .append(L"\">");
        fileText += rTest.is_included() ? XmlFormat::GetTrue() : XmlFormat::GetFalse();
        fileText.append(L"</").append(rTest.get_test().get_id().c_str()).append(L">\n");
        }
    // Dolch sight words
    fileText.append(L"\t\t<")
        .append(wxGetApp().GetAppOptions().XML_DOLCH_SIGHT_WORDS_TEST.data())
        .append(L">");
    fileText += IsIncludingDolchSightWords() ? XmlFormat::GetTrue() : XmlFormat::GetFalse();
    fileText.append(L"</")
        .append(wxGetApp().GetAppOptions().XML_DOLCH_SIGHT_WORDS_TEST.data())
        .append(L">\n");
    fileText.append(L"\t</")
        .append(wxGetApp().GetAppOptions().XML_READABILITY_TESTS_SECTION.data())
        .append(L">\n");

    // save the text views
    //---------------------------
    fileText.append(L"\t<")
        .append(wxGetApp().GetAppOptions().XML_TEXT_VIEWS_SECTION.data())
        .append(L">\n");
    // how the text is highlighted
    XmlFormat::FormatSection(sectionText, wxGetApp().GetAppOptions().XML_HIGHLIGHT_METHOD.data(),
                             static_cast<int>(m_textHighlight), 2);
    fileText += sectionText;
    // text highlight color
    fileText.append(L"\t\t<").append(wxGetApp().GetAppOptions().XML_HIGHLIGHTCOLOR.data());
    fileText += XmlFormat::FormatColorAttributes(m_textViewHighlightColor);
    fileText.append(L"/>\n");
    // highlight for excluded sentences
    fileText.append(L"\t\t<").append(wxGetApp().GetAppOptions().XML_EXCLUDED_HIGHLIGHTCOLOR.data());
    fileText += XmlFormat::FormatColorAttributes(m_excludedTextHighlightColor);
    fileText.append(L"/>\n");
    // highlight for repeated words
    fileText.append(L"\t\t<").append(wxGetApp().GetAppOptions().XML_DUP_WORD_HIGHLIGHTCOLOR.data());
    fileText += XmlFormat::FormatColorAttributes(m_duplicateWordHighlightColor);
    fileText.append(L"/>\n");
    // highlight for dolch words
    fileText.append(L"\t\t<").append(
        wxGetApp().GetAppOptions().XML_DOLCH_CONJUNCTIONS_HIGHLIGHTCOLOR.data());
    fileText += XmlFormat::FormatColorAttributeWithInclusionTag(GetDolchConjunctionsColor(),
                                                                IsHighlightingDolchConjunctions());
    fileText.append(L"/>\n");
    fileText.append(L"\t\t<").append(
        wxGetApp().GetAppOptions().XML_DOLCH_PREPOSITIONS_HIGHLIGHTCOLOR.data());
    fileText += XmlFormat::FormatColorAttributeWithInclusionTag(GetDolchPrepositionsColor(),
                                                                IsHighlightingDolchPrepositions());
    fileText.append(L"/>\n");
    fileText.append(L"\t\t<").append(
        wxGetApp().GetAppOptions().XML_DOLCH_PRONOUNS_HIGHLIGHTCOLOR.data());
    fileText += XmlFormat::FormatColorAttributeWithInclusionTag(GetDolchPronounsColor(),
                                                                IsHighlightingDolchPronouns());
    fileText.append(L"/>\n");
    fileText.append(L"\t\t<").append(
        wxGetApp().GetAppOptions().XML_DOLCH_ADVERBS_HIGHLIGHTCOLOR.data());
    fileText += XmlFormat::FormatColorAttributeWithInclusionTag(GetDolchAdverbsColor(),
                                                                IsHighlightingDolchAdverbs());
    fileText.append(L"/>\n");
    fileText.append(L"\t\t<").append(
        wxGetApp().GetAppOptions().XML_DOLCH_ADJECTIVES_HIGHLIGHTCOLOR.data());
    fileText += XmlFormat::FormatColorAttributeWithInclusionTag(GetDolchAdjectivesColor(),
                                                                IsHighlightingDolchAdjectives());
    fileText.append(L"/>\n");
    fileText.append(L"\t\t<").append(
        wxGetApp().GetAppOptions().XML_DOLCH_VERBS_HIGHLIGHTCOLOR.data());
    fileText += XmlFormat::FormatColorAttributeWithInclusionTag(GetDolchVerbsColor(),
                                                                IsHighlightingDolchVerbs());
    fileText.append(L"/>\n");
    fileText.append(L"\t\t<").append(
        wxGetApp().GetAppOptions().XML_DOLCH_NOUNS_HIGHLIGHTCOLOR.data());
    fileText += XmlFormat::FormatColorAttributeWithInclusionTag(GetDolchNounColor(),
                                                                IsHighlightingDolchNouns());
    fileText.append(L"/>\n");
    // highlight for wordy items
    fileText.append(L"\t\t<").append(
        wxGetApp().GetAppOptions().XML_WORDY_PHRASE_HIGHLIGHTCOLOR.data());
    fileText += XmlFormat::FormatColorAttributes(m_wordyPhraseHighlightColor);
    fileText.append(L"/>\n");
    // text view font color
    fileText.append(L"\t\t<").append(
        wxGetApp().GetAppOptions().XML_DOCUMENT_DISPLAY_FONTCOLOR.data());
    fileText += XmlFormat::FormatColorAttributes(m_fontColor);
    fileText.append(L"/>\n");
    // text view font information
    fileText.append(L"\t\t<").append(wxGetApp().GetAppOptions().XML_DOCUMENT_DISPLAY_FONT.data());
    fileText += XmlFormat::FormatFontAttributes(m_textViewFont);
    fileText.append(L"/>\n");
    fileText.append(L"\t</")
        .append(wxGetApp().GetAppOptions().XML_TEXT_VIEWS_SECTION.data())
        .append(L">\n");

    fileText.append(L"</")
        .append(wxGetApp().GetAppOptions().XML_PROJECT_HEADER.data())
        .append(L">");

    return fileText;
    }

//------------------------------------------------
bool BaseProjectDoc::ApplyTestBundle(const wxString& bundleName)
    {
    const auto testBundleIter = BaseProject::m_testBundles.find(TestBundle(bundleName.wc_str()));
    if (testBundleIter == BaseProject::m_testBundles.end())
        {
        wxMessageBox(wxString::Format(_(L"Unable to find test bundle \"%s\": "
                                        "internal error, please contact software vendor."),
                                      bundleName),
                     _(L"Error"), wxOK | wxICON_ERROR);
        return false;
        }
    // Add the tests (and goals) to the project
    ExcludeAllTests();
    SetTestGoals(testBundleIter->GetTestGoals());
    SetStatGoals(testBundleIter->GetStatGoals());
    // add the tests from the bundle (test goals are reviewed as the tests are added)
    for (const auto& bundledTest : testBundleIter->GetTestGoals())
        {
        if (GetReadabilityTests().has_test(bundledTest.GetName().c_str()))
            {
            GetReadabilityTests().include_test(bundledTest.GetName().c_str(), true);
            }
        else if (bundledTest.GetName() ==
                 traits::case_insensitive_wstring_ex(ReadabilityMessages::DOLCH()))
            {
            IncludeDolchSightWords();
            }
        else if (AddCustomReadabilityTest(wxString(bundledTest.GetName().c_str())))
            {
            RefreshRequired(ProjectRefresh::FullReindexing);
            }
        }
    ReviewStatGoals();
    return true;
    }
