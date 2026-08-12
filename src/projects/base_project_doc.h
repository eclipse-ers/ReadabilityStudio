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

#ifndef BASE_PROJECT_DOC_H
#define BASE_PROJECT_DOC_H

#include "../graphs/raygorgraph.h"
#include "../ui/controls/explanation_listctrl.h"
#include "base_project.h"

/// Base document layer of a project, storing features common to standard and batch projects.
class BaseProjectDoc : public BaseProject, public wxDocument
    {
  public:
    BaseProjectDoc();
    BaseProjectDoc(const BaseProjectDoc&) = delete;
    BaseProjectDoc& operator=(const BaseProjectDoc&) = delete;

    ~BaseProjectDoc() override { m_File.Close(); }

    /** Call this if you have constructed a project document
        and need to copy over all settings from another project. In turn, this would be like
        calling BaseProjectDoc's CTOR after copying over settings from another project.
        @param that The BaseProjectDoc to copy settings from.
        @param reloadImages `true` to load any images from this project's image paths, `false`
            to copy the images directly from this project.
        @note This will call BaseProject::CopySettings() along with copying document-level
       features.*/
    void CopyDocumentLevelSettings(const BaseProjectDoc& that, const bool reloadImages);

    static bool AddGlobalCustomReadabilityTest(CustomReadabilityTest& customTest);
    /// When we switched to the TinyExpr++ formula engine (circa 2020),
    /// the syntax slightly changed, so this updates custom test formulas
    /// to work with the new parser.
    static CustomReadabilityTest::string_type
    UpdateCustomReadabilityTest(const CustomReadabilityTest::string_type& formula);
    static void RemoveGlobalCustomReadabilityTest(const wxString& testName);
    static void RemoveAllGlobalCustomReadabilityTests();

    /// Removes all tests and goals from the project and applies
    /// the tests and goals from a given bundle.
    /// @param bundleName The test bundle to apply.
    /// @returns @c true if bundle was successfully applied.
    bool ApplyTestBundle(const wxString& bundleName);

    void UseRealTimeUpdate(const bool realTime) { m_realTimeUpdate = realTime; }

    [[nodiscard]]
    bool IsRealTimeUpdating() const noexcept
        {
        return m_realTimeUpdate;
        }

    // graph information
    //------------------------------
    void SetStippleImagePath(const wxString& filePath);

    [[nodiscard]]
    wxString GetStippleImagePath() const
        {
        return m_stippleImagePath;
        }

    void SetPlotBackGroundImagePath(const wxString& filePath);

    [[nodiscard]]
    wxString GetGraphCommonImagePath() const
        {
        return m_commonImagePath;
        }

    void SetGraphCommonImagePath(const wxString& filePath);

    void SetStippleShape(const wxString& shape) { m_stippleShape = shape; }

    [[nodiscard]]
    wxString GetStippleShape() const
        {
        return m_stippleShape;
        }

    [[nodiscard]]
    wxColour GetStippleShapeColor() const noexcept
        {
        return m_stippleColor;
        }

    void SetStippleShapeColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_stippleColor = color;
            }
        }

    [[nodiscard]]
    wxString GetPlotBackGroundImagePath() const
        {
        return m_plotBackGroundImagePath;
        }

    [[nodiscard]]
    wxString GetGraphColorScheme() const
        {
        return m_graphColorSchemeName;
        }

    // @note Must be the internal key (e.g., "coffeeshop"), not "Coffee Shop."
    void SetGraphColorScheme(wxString colorScheme);

    [[nodiscard]]
    wxColour GetBackGroundColor() const
        {
        return m_graphBackGroundColor;
        }

    void SetBackGroundColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_graphBackGroundColor = color;
            }
        }

    [[nodiscard]]
    wxColour GetPlotBackGroundColor() const
        {
        return m_plotBackGroundColor;
        }

    void SetPlotBackGroundColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_plotBackGroundColor = color;
            }
        }

    [[nodiscard]]
    uint8_t GetPlotBackGroundImageOpacity() const noexcept
        {
        return m_plotBackGroundImageOpacity;
        }

    void SetPlotBackGroundImageOpacity(const uint8_t opacity) noexcept
        {
        m_plotBackGroundImageOpacity = opacity;
        }

    [[nodiscard]]
    uint8_t GetPlotBackGroundColorOpacity() const noexcept
        {
        return m_plotBackGroundColorOpacity;
        }

    void SetPlotBackGroundColorOpacity(const uint8_t opacity) noexcept
        {
        m_plotBackGroundColorOpacity = opacity;
        }

    /// watermark functions
    void SetWatermark(const Wisteria::Canvas::Watermark& watermark) { m_watermark = watermark; }

    [[nodiscard]]
    Wisteria::Canvas::Watermark GetWatermark() const
        {
        return m_watermark;
        }

    void SetWatermarkLogoPath(const wxString& filePath);

    [[nodiscard]]
    wxString GetWatermarkLogoPath() const
        {
        return m_watermarkImagePath;
        }

    /// whether gradient is used for graph backgrounds
    void SetGraphBackGroundLinearGradient(const bool useGradient) noexcept
        {
        m_useGraphBackGroundImageLinearGradient = useGradient;
        }

    [[nodiscard]]
    bool GetGraphBackGroundLinearGradient() const noexcept
        {
        return m_useGraphBackGroundImageLinearGradient;
        }

    /// whether drop shadows should be shown
    void DisplayDropShadows(const bool display) noexcept { m_displayDropShadows = display; }

    [[nodiscard]]
    bool IsDisplayingDropShadows() const noexcept
        {
        return m_displayDropShadows;
        }

    // whether to draw attention to the complex word groups in syllable graphs, Fry score, etc.
    void ShowcaseKeyItems(const bool display) noexcept { m_showcaseKeyItems = display; }

    [[nodiscard]]
    bool IsShowcasingKeyItems() const noexcept
        {
        return m_showcaseKeyItems;
        }

    /// axis font colors
    [[nodiscard]]
    wxColour GetXAxisFontColor() const
        {
        return m_xAxisFontColor;
        }

    void SetXAxisFontColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_xAxisFontColor = color;
            }
        }

    [[nodiscard]]
    wxColour GetYAxisFontColor() const
        {
        return m_yAxisFontColor;
        }

    void SetYAxisFontColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_yAxisFontColor = color;
            }
        }

    /// axis fonts
    [[nodiscard]]
    wxFont GetXAxisFont() const
        {
        return m_xAxisFont;
        }

    void SetXAxisFont(const wxFont& font)
        {
        if (font.IsOk())
            {
            m_xAxisFont = font;
            }
        }

    [[nodiscard]]
    wxFont GetYAxisFont() const
        {
        return m_yAxisFont;
        }

    void SetYAxisFont(const wxFont& font)
        {
        if (font.IsOk())
            {
            m_yAxisFont = font;
            }
        }

    /// title fonts
    [[nodiscard]]
    wxColour GetGraphTopTitleFontColor() const
        {
        return m_topTitleFontColor;
        }

    void SetGraphTopTitleFontColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_topTitleFontColor = color;
            }
        }

    [[nodiscard]]
    wxFont GetGraphTopTitleFont() const
        {
        return m_topTitleFont;
        }

    void SetGraphTopTitleFont(const wxFont& font)
        {
        if (font.IsOk())
            {
            m_topTitleFont = font;
            }
        }

    [[nodiscard]]
    wxColour GetGraphBottomTitleFontColor() const
        {
        return m_bottomTitleFontColor;
        }

    void SetGraphBottomTitleFontColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_bottomTitleFontColor = color;
            }
        }

    [[nodiscard]]
    wxFont GetGraphBottomTitleFont() const
        {
        return m_bottomTitleFont;
        }

    void SetGraphBottomTitleFont(const wxFont& font)
        {
        if (font.IsOk())
            {
            m_bottomTitleFont = font;
            }
        }

    [[nodiscard]]
    wxColour GetGraphLeftTitleFontColor() const
        {
        return m_leftTitleFontColor;
        }

    void SetGraphLeftTitleFontColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_leftTitleFontColor = color;
            }
        }

    [[nodiscard]]
    wxFont GetGraphLeftTitleFont() const
        {
        return m_leftTitleFont;
        }

    void SetGraphLeftTitleFont(const wxFont& font)
        {
        if (font.IsOk())
            {
            m_leftTitleFont = font;
            }
        }

    [[nodiscard]]
    wxColour GetGraphRightTitleFontColor() const
        {
        return m_rightTitleFontColor;
        }

    void SetGraphRightTitleFontColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_rightTitleFontColor = color;
            }
        }

    [[nodiscard]]
    wxFont GetGraphRightTitleFont() const
        {
        return m_rightTitleFont;
        }

    void SetGraphRightTitleFont(const wxFont& font)
        {
        if (font.IsOk())
            {
            m_rightTitleFont = font;
            }
        }

    /// invalid area color
    [[nodiscard]]
    wxColour GetInvalidAreaColor() const
        {
        return m_graphInvalidAreaColor;
        }

    void SetInvalidAreaColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_graphInvalidAreaColor = color;
            }
        }

    // Raygor style
    [[nodiscard]]
    Wisteria::Graphs::RaygorStyle GetRaygorStyle() const noexcept
        {
        return m_raygorStyle;
        }

    void SetRaygorStyle(const Wisteria::Graphs::RaygorStyle style) noexcept
        {
        m_raygorStyle = style;
        }

    // Flesch connection lines
    void ConnectFleschPoints(const bool connect) noexcept { m_fleschChartConnectPoints = connect; }

    [[nodiscard]]
    bool IsConnectingFleschPoints() const noexcept
        {
        return m_fleschChartConnectPoints;
        }

    // Flesch document groups next to the syllable ruler
    void IncludeFleschRulerDocGroups(const bool connect) noexcept
        {
        m_fleschChartSyllableRulerDocGroups = connect;
        }

    [[nodiscard]]
    bool IsIncludingFleschRulerDocGroups() const noexcept
        {
        return m_fleschChartSyllableRulerDocGroups;
        }

    /** Sets whether to use English labels for the brackets on German Lix gauges.
        @param useEnglish True to use the translated (English) labels.*/
    void UseEnglishLabelsForGermanLix(const bool useEnglish) noexcept
        {
        m_useEnglishLabelsGermanLix = useEnglish;
        }

    /// @returns @c true if English labels are being used for the brackets on German Lix gauges.
    [[nodiscard]]
    bool IsUsingEnglishLabelsForGermanLix() const noexcept
        {
        return m_useEnglishLabelsGermanLix;
        }

    /// Histogram options
    [[nodiscard]]
    Wisteria::Graphs::Histogram::BinningMethod GetHistogramBinningMethod() const noexcept
        {
        return m_histogramBinningMethod;
        }

    void SetHistogramBinningMethod(const Wisteria::Graphs::Histogram::BinningMethod method) noexcept
        {
        m_histogramBinningMethod = method;
        }

    [[nodiscard]]
    Wisteria::BinLabelDisplay GetHistogramBinLabelDisplay() const noexcept
        {
        return m_histogramBinLabelDisplayMethod;
        }

    void SetHistogramBinLabelDisplay(const Wisteria::BinLabelDisplay display) noexcept
        {
        m_histogramBinLabelDisplayMethod = display;
        }

    [[nodiscard]]
    Wisteria::RoundingMethod GetHistogramRoundingMethod() const noexcept
        {
        return m_histogramRoundingMethod;
        }

    void SetHistogramRoundingMethod(const Wisteria::RoundingMethod rounding) noexcept
        {
        m_histogramRoundingMethod = rounding;
        }

    void
    SetHistogramIntervalDisplay(const Wisteria::Graphs::Histogram::IntervalDisplay display) noexcept
        {
        m_histogramIntervalDisplay = display;
        }

    [[nodiscard]]
    Wisteria::Graphs::Histogram::IntervalDisplay GetHistogramIntervalDisplay() const noexcept
        {
        return m_histogramIntervalDisplay;
        }

    [[nodiscard]]
    wxColour GetHistogramBarColor() const noexcept
        {
        return m_histogramBarColor;
        }

    void SetHistogramBarColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_histogramBarColor = color;
            }
        }

    [[nodiscard]]
    uint8_t GetHistogramBarOpacity() const noexcept
        {
        return m_histogramBarOpacity;
        }

    void SetHistogramBarOpacity(const uint8_t opacity) noexcept { m_histogramBarOpacity = opacity; }

    [[nodiscard]]
    Wisteria::BoxEffect GetHistogramBarEffect() const noexcept
        {
        return m_histogramBarEffect;
        }

    void SetHistogramBarEffect(const Wisteria::BoxEffect effect) noexcept
        {
        m_histogramBarEffect = effect;
        }

    // Bar chart options
    [[nodiscard]]
    wxColour GetBarChartBarColor() const noexcept
        {
        return m_barChartBarColor;
        }

    void SetBarChartBarColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_barChartBarColor = color;
            }
        }

    [[nodiscard]]
    Wisteria::Orientation GetBarChartOrientation() const noexcept
        {
        return m_barChartOrientation;
        }

    void SetBarChartOrientation(const Wisteria::Orientation orient) noexcept
        {
        m_barChartOrientation = orient;
        }

    [[nodiscard]]
    uint8_t GetGraphBarOpacity() const noexcept
        {
        return m_graphBarOpacity;
        }

    void SetGraphBarOpacity(const uint8_t opacity) noexcept { m_graphBarOpacity = opacity; }

    [[nodiscard]]
    Wisteria::BoxEffect GetGraphBarEffect() const noexcept
        {
        return m_graphBarEffect;
        }

    void SetGraphBarEffect(const Wisteria::BoxEffect effect) noexcept { m_graphBarEffect = effect; }

    void DisplayBarChartLabels(const bool display = true) noexcept { m_barDisplayLabels = display; }

    [[nodiscard]]
    bool IsDisplayingBarChartLabels() const noexcept
        {
        return m_barDisplayLabels;
        }

    // Box Plot options
    void ShowAllBoxPlotPoints(const bool show) noexcept { m_boxPlotShowAllPoints = show; }

    [[nodiscard]]
    bool IsShowingAllBoxPlotPoints() const noexcept
        {
        return m_boxPlotShowAllPoints;
        }

    void DisplayBoxPlotLabels(const bool display = true) noexcept { m_boxDisplayLabels = display; }

    [[nodiscard]]
    bool IsDisplayingBoxPlotLabels() const noexcept
        {
        return m_boxDisplayLabels;
        }

    void ConnectBoxPlotMiddlePoints(const bool connect = true) noexcept
        {
        m_boxConnectMiddlePoints = connect;
        }

    [[nodiscard]]
    bool IsConnectingBoxPlotMiddlePoints() const noexcept
        {
        return m_boxConnectMiddlePoints;
        }

    [[nodiscard]]
    wxColour GetGraphBoxColor() const noexcept
        {
        return m_graphBoxColor;
        }

    void SetGraphBoxColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_graphBoxColor = color;
            }
        }

    [[nodiscard]]
    uint8_t GetGraphBoxOpacity() const noexcept
        {
        return m_graphBoxOpacity;
        }

    void SetGraphBoxOpacity(const uint8_t opacity) noexcept { m_graphBoxOpacity = opacity; }

    [[nodiscard]]
    Wisteria::BoxEffect GetGraphBoxEffect() const noexcept
        {
        return m_graphBoxEffect;
        }

    void SetGraphBoxEffect(const Wisteria::BoxEffect effect) noexcept { m_graphBoxEffect = effect; }

    // background image options
    [[nodiscard]]
    Wisteria::ImageEffect GetPlotBackGroundImageEffect() const noexcept
        {
        return m_plotBackgroundImageEffect;
        }

    void SetPlotBackGroundImageEffect(const Wisteria::ImageEffect effect);

    [[nodiscard]]
    Wisteria::ImageFit GetPlotBackGroundImageFit() const noexcept
        {
        return m_plotBackgroundImageFit;
        }

    void SetPlotBackGroundImageFit(const Wisteria::ImageFit fit) noexcept
        {
        m_plotBackgroundImageFit = fit;
        }

    // text highlighting
    [[nodiscard]]
    TextHighlight GetTextHighlightMethod() const noexcept
        {
        return m_textHighlight;
        }

    void SetTextHighlightMethod(const TextHighlight highlight) noexcept
        {
        m_textHighlight = highlight;
        }

    // text highlighting
    [[nodiscard]]
    wxColour GetTextHighlightColor() const noexcept
        {
        return m_textViewHighlightColor;
        }

    void SetTextHighlightColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_textViewHighlightColor = color;
            }
        }

    // excluded sentences
    [[nodiscard]]
    wxColour GetExcludedTextHighlightColor() const noexcept
        {
        return m_excludedTextHighlightColor;
        }

    void SetExcludedTextHighlightColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_excludedTextHighlightColor = color;
            }
        }

    // color to highlight repeated words
    [[nodiscard]]
    wxColour GetDuplicateWordHighlightColor() const noexcept
        {
        return m_duplicateWordHighlightColor;
        }

    void SetDuplicateWordHighlightColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_duplicateWordHighlightColor = color;
            }
        }

    // color for wordy items
    [[nodiscard]]
    wxColour GetWordyPhraseHighlightColor() const noexcept
        {
        return m_wordyPhraseHighlightColor;
        }

    void SetWordyPhraseHighlightColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_wordyPhraseHighlightColor = color;
            }
        }

    // text view font and color
    [[nodiscard]]
    wxFont GetTextViewFont() const noexcept
        {
        return m_textViewFont;
        }

    void SetTextViewFont(const wxFont& font)
        {
        if (font.IsOk())
            {
            m_textViewFont = font;
            }
        }

    // whether the highlighted-text windows use the report theme's font (from its CSS)
    // instead of GetTextViewFont()
    [[nodiscard]]
    bool IsUsingStandardReportFont() const noexcept
        {
        return m_useStandardReportFont;
        }

    void UseStandardReportFont(const bool use = true) noexcept { m_useStandardReportFont = use; }

    // text color
    [[nodiscard]]
    wxColour GetTextFontColor() const noexcept
        {
        return m_fontColor;
        }

    void SetTextFontColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_fontColor = color;
            }
        }

    // dolch colors
    [[nodiscard]]
    wxColour GetDolchConjunctionsColor() const noexcept
        {
        return m_dolchConjunctionsColor;
        }

    void SetDolchConjunctionsColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_dolchConjunctionsColor = color;
            }
        }

    [[nodiscard]]
    wxColour GetDolchPrepositionsColor() const noexcept
        {
        return m_dolchPrepositionsColor;
        }

    void SetDolchPrepositionsColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_dolchPrepositionsColor = color;
            }
        }

    [[nodiscard]]
    wxColour GetDolchPronounsColor() const noexcept
        {
        return m_dolchPronounsColor;
        }

    void SetDolchPronounsColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_dolchPronounsColor = color;
            }
        }

    [[nodiscard]]
    wxColour GetDolchAdverbsColor() const noexcept
        {
        return m_dolchAdverbsColor;
        }

    void SetDolchAdverbsColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_dolchAdverbsColor = color;
            }
        }

    [[nodiscard]]
    wxColour GetDolchAdjectivesColor() const noexcept
        {
        return m_dolchAdjectivesColor;
        }

    void SetDolchAdjectivesColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_dolchAdjectivesColor = color;
            }
        }

    [[nodiscard]]
    wxColour GetDolchVerbsColor() const noexcept
        {
        return m_dolchVerbsColor;
        }

    void SetDolchVerbsColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_dolchVerbsColor = color;
            }
        }

    [[nodiscard]]
    wxColour GetDolchNounColor() const noexcept
        {
        return m_dolchNounColor;
        }

    void SetDolchNounColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_dolchNounColor = color;
            }
        }

    [[nodiscard]]
    bool IsHighlightingDolchConjunctions() const noexcept
        {
        return m_highlightDolchConjunctions;
        }

    void HighlightDolchConjunctions(const bool highlight = true) noexcept
        {
        m_highlightDolchConjunctions = highlight;
        }

    [[nodiscard]]
    bool IsHighlightingDolchPrepositions() const noexcept
        {
        return m_highlightDolchPrepositions;
        }

    void HighlightDolchPrepositions(const bool highlight = true) noexcept
        {
        m_highlightDolchPrepositions = highlight;
        }

    [[nodiscard]]
    bool IsHighlightingDolchPronouns() const noexcept
        {
        return m_highlightDolchPronouns;
        }

    void HighlightDolchPronouns(const bool highlight = true) noexcept
        {
        m_highlightDolchPronouns = highlight;
        }

    [[nodiscard]]
    bool IsHighlightingDolchAdverbs() const noexcept
        {
        return m_highlightDolchAdverbs;
        }

    void HighlightDolchAdverbs(const bool highlight = true) noexcept
        {
        m_highlightDolchAdverbs = highlight;
        }

    [[nodiscard]]
    bool IsHighlightingDolchAdjectives() const noexcept
        {
        return m_highlightDolchAdjectives;
        }

    void HighlightDolchAdjectives(const bool highlight = true) noexcept
        {
        m_highlightDolchAdjectives = highlight;
        }

    [[nodiscard]]
    bool IsHighlightingDolchVerbs() const noexcept
        {
        return m_highlightDolchVerbs;
        }

    void HighlightDolchVerbs(const bool highlight = true) noexcept
        {
        m_highlightDolchVerbs = highlight;
        }

    [[nodiscard]]
    bool IsHighlightingDolchNouns() const noexcept
        {
        return m_highlightDolchNouns;
        }

    void HighlightDolchNouns(const bool highlight = true) noexcept
        {
        m_highlightDolchNouns = highlight;
        }

    /// @returns The background color used for the text windows
    [[nodiscard]]
    wxColour GetTextReportBackgroundColor() const;

    void SetExportFolder(const wxString& folder) { m_exportFolder = folder; }

    [[nodiscard]]
    wxString GetExportFolder() const
        {
        return m_exportFolder;
        }

    void SetExportFile(const wxString& file) { m_exportFile = file; }

    [[nodiscard]]
    wxString GetExportFile() const
        {
        return m_exportFile;
        }

    static void SetExportTextViewExt(const wxString& extension) { m_exportTextViewExt = extension; }

    [[nodiscard]]
    static wxString GetExportTextViewExt()
        {
        return m_exportTextViewExt;
        }

    static void SetExportSummaryReportExt(const wxString& extension)
        {
        m_exportSummaryReportExt = extension;
        }

    [[nodiscard]]
    static wxString GetExportSummaryReportExt()
        {
        return m_exportSummaryReportExt;
        }

    static void SetExportListExt(const wxString& extension) { m_exportListExt = extension; }

    [[nodiscard]]
    static wxString GetExportListExt()
        {
        return m_exportListExt;
        }

    static void SetExportGraphExt(const wxString& extension) { m_exportGraphExt = extension; }

    [[nodiscard]]
    static wxString GetExportGraphExt()
        {
        return m_exportGraphExt;
        }

    static void ExportHardWordLists(const bool exportItem) noexcept
        {
        m_exportHardWordLists = exportItem;
        }

    [[nodiscard]]
    static bool IsExportingHardWordLists() noexcept
        {
        return m_exportHardWordLists;
        }

    static void ExportSentencesBreakdown(const bool exportItem) noexcept
        {
        m_exportSentencesBreakdown = exportItem;
        }

    [[nodiscard]]
    static bool IsExportingSentencesBreakdown() noexcept
        {
        return m_exportSentencesBreakdown;
        }

    static void ExportGraphs(const bool exportItem) noexcept { m_exportGraphs = exportItem; }

    [[nodiscard]]
    static bool IsExportingGraphs() noexcept
        {
        return m_exportGraphs;
        }

    static void ExportTestResults(const bool exportItem) noexcept
        {
        m_exportTestResults = exportItem;
        }

    [[nodiscard]]
    static bool IsExportingTestResults() noexcept
        {
        return m_exportTestResults;
        }

    static void ExportStatistics(const bool exportItem) noexcept
        {
        m_exportStatistics = exportItem;
        }

    [[nodiscard]]
    static bool IsExportingStatistics() noexcept
        {
        return m_exportStatistics;
        }

    static void ExportWordiness(const bool exportItem) noexcept { m_exportWordiness = exportItem; }

    static bool IsExportingWordiness() noexcept { return m_exportWordiness; }

    static void ExportPlainLanguageGuide(const bool exportItem) noexcept
        {
        m_exportPlainLanguageGuide = exportItem;
        }

    [[nodiscard]]
    static bool IsExportingPlainLanguageGuide() noexcept
        {
        return m_exportPlainLanguageGuide;
        }

    static void ExportSightWords(const bool exportItem) noexcept
        {
        m_exportSightWords = exportItem;
        }

    [[nodiscard]]
    static bool IsExportingSightWords() noexcept
        {
        return m_exportSightWords;
        }

    static void ExportWarnings(const bool exportItem) noexcept { m_exportWarnings = exportItem; }

    [[nodiscard]]
    static bool IsExportingWarnings() noexcept
        {
        return m_exportWarnings;
        }

    static void ExportLists(const bool exportItem) noexcept { m_exportingLists = exportItem; }

    [[nodiscard]]
    static bool IsExportingLists() noexcept
        {
        return m_exportingLists;
        }

    static void ExportTextReports(const bool exportItem) noexcept
        {
        m_exportingTextReports = exportItem;
        }

    [[nodiscard]]
    static bool IsExportingTextReports() noexcept
        {
        return m_exportingTextReports;
        }

    [[nodiscard]]
    static Wisteria::UI::ImageExportOptions& GetImageExportOptions() noexcept
        {
        return m_imageExportOptions;
        }

    void LoadSettingsFile(const wchar_t* settingsFileText);

    void UpdateGraphOptions(Wisteria::Canvas* canvas) const;
    void UpdateListOptions(Wisteria::UI::ListCtrlEx* list) const;
    void UpdateExplanationListOptions(ExplanationListCtrl* eList) const;
    /// @brief Updates printer settings for a canvas.
    /// @param window The Canvas to update.
    void UpdatePrinterSettings(Wisteria::Canvas* window) const;
    /// @brief Updates printer settings for a list control.
    /// @param window The list control to update.
    void UpdatePrinterSettings(Wisteria::UI::ListCtrlEx* window) const;
    /// @brief Updates printer settings for an explanation list control.
    /// @param window The explanation list control to update.
    void UpdatePrinterSettings(ExplanationListCtrl* window) const;

    virtual void RefreshGraphs() {}

    virtual void RefreshStatisticsReports() {}

    /// @brief Determines whether the project is safe to have its options or tests changed.
    /// @returns @c false if the project is being reloaded.
    [[nodiscard]]
    bool IsSafeToUpdate() const
        {
        if (IsProcessing())
            {
            wxMessageBox(_(L"Project still being reloaded. Please wait..."), GetTitle(),
                         wxOK | wxICON_EXCLAMATION);
            return false;
            }

        return true;
        }

    /// @returns The path part of the project file location.
    [[nodiscard]]
    wxString GetProjectDirectory() const override
        {
        return wxFileName(GetFilename()).GetPath();
        }

    /// how file paths are shown in batch projects
    void SetFilePathTruncationMode(
        const Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode TruncMode) noexcept
        {
        m_filePathTruncationMode = TruncMode;
        }

    [[nodiscard]]
    Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode
    GetFilePathTruncationMode() const noexcept
        {
        return m_filePathTruncationMode;
        }

    /// event functions
    bool OnCloseDocument() override;

    void SetModifiedFlag() override { Modify(true); }

    void SetDocumentTitle(const wxString& title) override { SetTitle(title); }

    // used for the contents of a project (zip) file
    [[nodiscard]]
    static wxString ProjectSettingsFileLabel()
        {
        return L"settings.xml";
        }

    [[nodiscard]]
    static wxString ProjectContentFileLabel()
        {
        return L"content0.txt";
        }

  protected:
    /// @returns The goal values of a test goal (if specified in the project).
    std::tuple<double, double> GetGoalsForTest(const wxString& testName) const
        {
        const auto foundTest = GetTestGoals().find(TestGoal{ testName.wc_str() });
        return (foundTest == GetTestGoals().cend()) ?
                   std::make_tuple(std::numeric_limits<double>::quiet_NaN(),
                                   std::numeric_limits<double>::quiet_NaN()) :
                   std::make_tuple(foundTest->GetMinGoal(), foundTest->GetMaxGoal());
        }

    /// @returns The goal values of a statistic goal (if specified in the project).
    std::tuple<double, double> GetGoalsForStatistic(const wxString& statName) const
        {
        const auto foundStat = GetStatGoals().find(TestGoal{ statName.wc_str() });
        return (foundStat == GetStatGoals().cend()) ?
                   std::make_tuple(std::numeric_limits<double>::quiet_NaN(),
                                   std::numeric_limits<double>::quiet_NaN()) :
                   std::make_tuple(foundStat->GetMinGoal(), foundStat->GetMaxGoal());
        }

    /// @brief Loads and image from given path, and will also search for the
    ///     file in the project directory if not found.
    /// @param[in,out] filePath The image file path. Will be updated if not found
    ///     but then found in the project folder.
    /// @param[out] img The loaded image.
    void LoadImageAndPath(wxString& filePath, wxBitmapBundle& img);

    [[nodiscard]]
    wxBitmap LoadBitmapFromProject(const wxString& fileName) const
        {
        if (!wxFile::Exists(GetFilename()) || fileName.empty())
            {
            return wxNullBitmap;
            }
        try
            {
            MemoryMappedFile sourceFile;
            sourceFile.MapFile(GetFilename(), true, true);
            const char* projectFileText = static_cast<char*>(sourceFile.GetStream());
            const Wisteria::ZipCatalog cat(projectFileText, sourceFile.GetMapSize());
            return cat.ReadBitmap(fileName, wxBITMAP_TYPE_ANY);
            }
        catch (...)
            {
            return wxNullBitmap;
            }
        }

    /// @returns the application's printer data.
    [[nodiscard]]
    static wxPrintData* GetPrintData();
    wxString FormatProjectSettings() const;
    // project status
    bool LockProjectFile();

    bool m_FileReadOnly{ false };
    wxFile m_File;

    bool m_realTimeUpdate{ false };

    // general graph information (that can apply to both standard and batch projects)
    bool m_useGraphBackGroundImageLinearGradient{ false };
    bool m_displayDropShadows{ false };
    bool m_showcaseKeyItems{ false };
    wxBitmapBundle m_plotBackgroundImage;
    wxBitmapBundle m_plotBackgroundImageWithEffect;
    wxBitmapBundle m_graphStippleImage;
    std::shared_ptr<Wisteria::Images::Schemes::ImageScheme> m_graphImageScheme{
        std::make_shared<Wisteria::Images::Schemes::ImageScheme>(
            std::vector<wxBitmapBundle>{ wxBitmapBundle{} })
    };
    std::shared_ptr<Wisteria::Brushes::Schemes::BrushScheme> m_graphBrushScheme{
        std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
            Wisteria::Colors::Schemes::Campfire{})
    };
    wxString m_graphColorSchemeName{ _DT(L"campfire") };
    wxString m_plotBackGroundImagePath;
    wxString m_stippleImagePath;
    wxString m_commonImagePath;
    wxString m_stippleShape{ DONTTRANSLATE(L"book") };
    wxColour m_stippleColor{ wxColour{ L"#6082B6" } };
    wxColour m_graphBackGroundColor;
    wxColour m_plotBackGroundColor;
    uint8_t m_plotBackGroundImageOpacity{ wxALPHA_OPAQUE };
    uint8_t m_plotBackGroundColorOpacity{ wxALPHA_OPAQUE };
    Wisteria::Canvas::Watermark m_watermark;
    wxString m_watermarkImagePath;
    wxBitmapBundle m_waterMarkImage;
    wxColour m_xAxisFontColor;
    wxFont m_xAxisFont;
    wxColour m_yAxisFontColor;
    wxFont m_yAxisFont;
    wxColour m_topTitleFontColor;
    wxFont m_topTitleFont;
    wxColour m_bottomTitleFontColor;
    wxFont m_bottomTitleFont;
    wxColour m_leftTitleFontColor;
    wxFont m_leftTitleFont;
    wxColour m_rightTitleFontColor;
    wxFont m_rightTitleFont;
    wxColour m_graphInvalidAreaColor;
    Wisteria::Graphs::RaygorStyle m_raygorStyle{ Wisteria::Graphs::RaygorStyle::BaldwinKaufman };
    bool m_fleschChartConnectPoints{ true };
    bool m_fleschChartSyllableRulerDocGroups{ false };
    bool m_useEnglishLabelsGermanLix{ false };

    // Histogram options
    Wisteria::Graphs::Histogram::BinningMethod m_histogramBinningMethod{
        Wisteria::Graphs::Histogram::BinningMethod::BinByIntegerRange
    };
    Wisteria::BinLabelDisplay m_histogramBinLabelDisplayMethod{
        Wisteria::BinLabelDisplay::BinValue
    };
    Wisteria::RoundingMethod m_histogramRoundingMethod{ Wisteria::RoundingMethod::RoundDown };
    Wisteria::Graphs::Histogram::IntervalDisplay m_histogramIntervalDisplay{
        Wisteria::Graphs::Histogram::IntervalDisplay::Cutpoints
    };
    wxColour m_histogramBarColor{ wxColour{ 182, 164, 204 } }; // lavender
    uint8_t m_histogramBarOpacity{ wxALPHA_OPAQUE };
    Wisteria::BoxEffect m_histogramBarEffect{ Wisteria::BoxEffect::Glassy };
    // Bar chart options
    wxColour m_barChartBarColor;
    Wisteria::Orientation m_barChartOrientation{ Wisteria::Orientation::Horizontal };
    uint8_t m_graphBarOpacity{ wxALPHA_OPAQUE };
    Wisteria::BoxEffect m_graphBarEffect{ Wisteria::BoxEffect::Glassy };
    bool m_barDisplayLabels{ true };
    // box plot options
    bool m_boxPlotShowAllPoints{ false };
    bool m_boxDisplayLabels{ false };
    bool m_boxConnectMiddlePoints{ false };
    wxColour m_graphBoxColor;
    uint8_t m_graphBoxOpacity{ wxALPHA_OPAQUE };
    Wisteria::BoxEffect m_graphBoxEffect{ Wisteria::BoxEffect::Glassy };
    // background image options
    Wisteria::ImageEffect m_plotBackgroundImageEffect{ Wisteria::ImageEffect::NoEffect };
    Wisteria::ImageFit m_plotBackgroundImageFit{ Wisteria::ImageFit::Shrink };

    // text highlighting options
    wxColour m_textViewHighlightColor;
    wxColour m_excludedTextHighlightColor;
    wxColour m_wordyPhraseHighlightColor;
    wxColour m_duplicateWordHighlightColor;

    TextHighlight m_textHighlight{ TextHighlight::HighlightBackground };
    wxFont m_textViewFont;
    bool m_useStandardReportFont{ true };
    wxColour m_fontColor;

    // text windows
    bool m_textReportBackgroundColorFromTheme{ true };

    // dolch colors
    wxColour m_dolchConjunctionsColor;
    wxColour m_dolchPrepositionsColor;
    wxColour m_dolchPronounsColor;
    wxColour m_dolchAdverbsColor;
    wxColour m_dolchAdjectivesColor;
    wxColour m_dolchVerbsColor;
    wxColour m_dolchNounColor;
    bool m_highlightDolchConjunctions{ true };
    bool m_highlightDolchPrepositions{ true };
    bool m_highlightDolchPronouns{ true };
    bool m_highlightDolchAdverbs{ true };
    bool m_highlightDolchAdjectives{ true };
    bool m_highlightDolchVerbs{ true };
    bool m_highlightDolchNouns{ false };

    // batch project options
    Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode m_filePathTruncationMode{
        Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames
    };

    //"Export All" options
    // different for each document
    wxString m_exportFolder;
    // different for each document
    wxString m_exportFile;
    // these should be shared for everyone
    static wxString m_exportListExt;
    static wxString m_exportTextViewExt;
    static wxString m_exportSummaryReportExt;
    static wxString m_exportGraphExt;
    static bool m_exportHardWordLists;
    static bool m_exportSentencesBreakdown;
    static bool m_exportGraphs;
    static bool m_exportTestResults;
    static bool m_exportStatistics;
    static bool m_exportWordiness;
    static bool m_exportPlainLanguageGuide;
    static bool m_exportSightWords;
    static bool m_exportWarnings;
    static bool m_exportingLists;
    static bool m_exportingTextReports;
    static Wisteria::UI::ImageExportOptions m_imageExportOptions;

    wxDECLARE_DYNAMIC_CLASS(BaseProjectDoc);
    };

#endif // BASE_PROJECT_DOC_H
