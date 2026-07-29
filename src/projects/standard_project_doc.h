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

#ifndef PROJECT_DOC_H
#define PROJECT_DOC_H

#include "../app/readability_app.h"
#include "base_project_doc.h"
#include "highlighted_text_buffers.h"
#include <wx/timer.h>
#include <wx/webview.h>

/// @brief Standard project document.
class ProjectDoc final : public BaseProjectDoc
    {
  public:
    /// @brief Constructor.
    ProjectDoc() : m_realTimeTimer(this) { Bind(wxEVT_TIMER, &ProjectDoc::OnRealTimeTimer, this); }

    /// @private
    ProjectDoc(const ProjectDoc&) = delete;
    /// @private
    ProjectDoc& operator=(const ProjectDoc&) = delete;

    /// @private
    ~ProjectDoc() override { DeleteExcludedPhrases(); }

    /// @returns The registry of HTML/RTF buffers for the highlighted-text windows.
    [[nodiscard]]
    HighlightedTextBufferMap& GetHighlightedTextBuffers() noexcept
        {
        return m_highlightedTextBuffers;
        }

    /// @private
    [[nodiscard]]
    const HighlightedTextBufferMap& GetHighlightedTextBuffers() const noexcept
        {
        return m_highlightedTextBuffers;
        }

    /// @private
    bool OnOpenDocument(const wxString& filename) final;
    /// @private
    bool OnNewDocument() final;
    /// @private
    bool OnSaveDocument(const wxString& filename) final;

    // refresh functions
    void RefreshProject() final;
    // only refresh the graphs, this assumes that no windows are being added or
    // removed from the project
    void RefreshGraphs() final;
    // only refresh statistics reports
    void RefreshStatisticsReports() final;

    void ShowQueuedMessages() final;

    // Name is all that is really needed if we know that the test is already loaded globally
    std::vector<CustomReadabilityTestInterface>::iterator
    RemoveCustomReadabilityTest(const wxString& testName, const int Id) final;

    bool AddFryTest(bool setFocus) final;
    bool AddRaygorTest(bool setFocus) final;
    bool AddGilliamPenaMountainFryTest(bool setFocus) final;
    bool AddFraseTest(bool setFocus) final;
    bool AddSchwartzTest(bool setFocus) final;
    bool AddDolchSightWords();

    void ExcludeAllCustomTestsTests() final;

    bool RunProjectWizard(const wxString& path);
    void RemoveMisspellings([[maybe_unused]] const wxArrayString& misspellingsToRemove) final;
    void DisplayReadabilityScores(const bool setFocus = true);
    void DisplayHighlightedText(const wxColour& highlightColor, const wxFont& textViewFont);

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetMisspelledWordData() const noexcept
        {
        return m_misspelledWordData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetOverusedWordsBySentenceData() const noexcept
        {
        return m_overusedWordsBySentenceData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetPassiveVoiceData() const noexcept
        {
        return m_passiveVoiceData;
        }

    void StopRealtimeUpdate() { m_realTimeTimer.Stop(); }

    void RestartRealtimeUpdate()
        {
        if (IsRealTimeUpdating())
            {
            m_realTimeTimer.Start(REALTIME_UPDATE_INTERVAL);
            }
        }

  private:
    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetOverusedWordsBySentenceData() noexcept
        {
        return m_overusedWordsBySentenceData;
        }

    void CalculateGraphData();
    void AddDB2Plot(const bool setFocus) final;
    void AddFleschChart(const bool setFocus) final;
    void AddLixGermanGauge(const bool setFocus) final;
    void AddLixGauge(const bool setFocus) final;
    void AddCrawfordGraph(const bool setFocus) final;
    void AddInfleszGraph(const bool setFocus) final;
    void LoadManuallyEnteredText();
    bool LoadProjectFile(const char* projectFileText, const size_t textLength);
    void DisplayStatistics();
    void DisplaySentencesBreakdown();
    void DisplayWordCharts();
    void DisplaySentenceCharts();
    void DisplayReadabilityGraphs();
    void DisplayWordsBreakdown();
    /// @brief Simply adds/removes word-list text windows when a word-list test is removed.
    /// @details This won't reformat anything, it just shows or hides a window if its
    ///     respective test is added or removed.
    void UpdateHighlightedTextWindows();
    void DisplayGrammar();
    void DisplayOverlyLongSentences();
    void DisplaySightWords();
    void SetReadabilityTestResult(const wxString& testId, const wxString& testName,
                                  const wxString& description,
                                  // score and display label
                                  const std::pair<double, wxString>& USGradeLevel,
                                  const wxString& readerAge, const double indexScore,
                                  const double clozeScore, const bool setFocus) final;

    // Text window formatting helpers
    //-------------------------------

    /// @brief The markup flavor to build highlighted-text content for.
    /// @details The highlighted-text windows render @c Html.\n
    ///     RTF is built only as a paper-white export format.
    ///     Windows and macOS both support RTF, but their RTF readers interpret
    ///     background-highlight control words differently;
    ///     hence the two separate RTF variants.
    enum class MarkupFormat
        {
        RtfWindows,
        RtfMacOS,
        Html
        };

    /// @returns @c true if @c format is one of the RTF variants (as opposed to HTML).
    [[nodiscard]]
    static constexpr bool IsRtf(const MarkupFormat format) noexcept
        {
        return format == MarkupFormat::RtfWindows || format == MarkupFormat::RtfMacOS;
        }

    struct HighlighterColors
        {
        wxColour highlightColor;
        wxColour errorHighlightColor;
        wxColour styleHighlightColor;
        wxColour excludedTextHighlightColor;
        wxColour dolchConjunctionsTextHighlightColor;
        wxColour dolchPrepositionsTextHighlightColor;
        wxColour dolchPronounsTextHighlightColor;
        wxColour dolchAdverbsTextHighlightColor;
        wxColour dolchAdjectivesTextHighlightColor;
        wxColour dolchVerbsTextHighlightColor;
        wxColour dolchNounTextHighlightColor;
        };

    struct HighlighterTags
        {
        wxString HIGHLIGHT_BEGIN;
        wxString HIGHLIGHT_END;
        wxString ERROR_HIGHLIGHT_BEGIN;
        wxString PHRASE_HIGHLIGHT_BEGIN;
        wxString IGNORE_HIGHLIGHT_BEGIN;
        wxString DOLCH_CONJUNCTION_BEGIN;
        wxString DOLCH_PREPOSITIONS_BEGIN;
        wxString DOLCH_PRONOUN_BEGIN;
        wxString DOLCH_ADVERB_BEGIN;
        wxString DOLCH_ADJECTIVE_BEGIN;
        wxString DOLCH_VERB_BEGIN;
        wxString DOLCH_NOUN_BEGIN;
        wxString HIGHLIGHT_BEGIN_LEGEND;
        wxString HIGHLIGHT_END_LEGEND;
        wxString DUPLICATE_HIGHLIGHT_BEGIN_LEGEND;
        wxString PHRASE_HIGHLIGHT_BEGIN_LEGEND;
        wxString IGNORE_HIGHLIGHT_BEGIN_LEGEND;
        wxString DOLCH_CONJUNCTION_BEGIN_LEGEND;
        wxString DOLCH_PREPOSITIONS_BEGIN_LEGEND;
        wxString DOLCH_PRONOUN_BEGIN_LEGEND;
        wxString DOLCH_ADVERB_BEGIN_LEGEND;
        wxString DOLCH_ADJECTIVE_BEGIN_LEGEND;
        wxString DOLCH_VERB_BEGIN_LEGEND;
        wxString DOLCH_NOUN_BEGIN_LEGEND;
        std::wstring BOLD_BEGIN;
        std::wstring BOLD_END;
        std::wstring TAB_SYMBOL;
        std::wstring CRLF;
        MarkupFormat format{ MarkupFormat::RtfWindows };
        };

    struct TextLegendLines
        {
        wxString ignoredSentencesLegendLine;
        wxString hardWordsLegendLine;
        wxString longWordsLegendLine;
        wxString unfamiliarDCWordsLegendLine;
        wxString unfamiliarHarrisJacobsonWordsLegendLine;
        wxString unfamiliarSpacheWordsLegendLine;
        wxString longSentencesLegendLine;
        wxString grammarIssuesLegendLine;
        wxString writingStyleLegendLine;
        wxString dolch1WordsLegendLine;
        wxString dolch2WordsLegendLine;
        wxString dolch3WordsLegendLine;
        wxString dolch4WordsLegendLine;
        wxString dolch5WordsLegendLine;
        wxString dolchVerbsLegendLine;
        wxString dolchNounsLegendLine;
        wxString nonDolchWordsLegendLine;
        };

    struct TextLegends
        {
        wxString plaintTextWindowLegend;
        wxString hardWordsLegend;
        wxString longWordsLegend;
        wxString unfamiliarDCWordsLegend;
        wxString unfamiliarSpacheWordsLegend;
        wxString unfamiliarHarrisJacobsonWordsLegend;
        wxString dolchWindowLegend;
        wxString nonDolchWordsLegend;
        wxString wordinessWindowLegend;
        };

    struct TextHeader
        {
        wxString header;
        wxString mainFontHeader;
        wxString colorTable;
        wxString endSection;
        };

    /// @brief Builds the colors and RTF color table (used by Windows and macOS).
    [[nodiscard]]
    HighlighterColors BuildReportColors(const wxColour& highlightColor,
                                        const wxColour& backgroundColor) const;
    /// @brief Builds the tags used to highlight words in RTF or HTML.
    /// @param format The markup flavor to build the tags for.
    /// @param highlightColor The default highlight color.\n
    ///     This is only used for RTF, not HTML.
    /// @returns The tags used to build RTF or HTML content.
    [[nodiscard]]
    HighlighterTags BuildHighlighterTags(const MarkupFormat format,
                                         const wxColour& highlightColor) const;
    /// @brief Formats the main font for an RTF's header.
    static std::pair<wxString, wxString> FormatRtfHeaderFont(const wxFont& textViewFont,
                                                             const size_t mainFontColorIndex);
    /// @brief Builds the highlight @c <style> block for an HTML highlighted-text window.
    /// @details Defines the `.hl-*` highlight classes from the user's own highlight
    ///     colors. Each class colors either the text (foreground highlighting) or its background,
    ///     and the CSS applies light/dark-mode handling to those colors per highlight mode so they
    ///     stay legible in either theme. The only thing used from the report theme is the legend
    ///     card's translucent background and side accent bar.
    /// @details Emitted as a self-contained block so the combined-report export can lift the
    ///     highlight rules straight out of the window's page source; it deliberately excludes the
    ///     body font and theme CSS, which live in a separate block.
    /// @returns The `<style>`...`</style>` block.
    [[nodiscard]]
    wxString BuildStyleSheet() const;
    /// @brief Encodes a legend label for embedding into RTF or HTML content.
    [[nodiscard]]
    static wxString EncodeLegendLabel(const wxString& label, const MarkupFormat format);
    [[nodiscard]]
    TextLegendLines BuildLegendLines(const HighlighterTags& highlighterTags) const;
    [[nodiscard]]
    static wxString BuildLegendLine(const HighlighterTags& highlighterTags,
                                    const wxString& legendStr);
    /// @brief Collapses the redundant line breaks left behind by concatenating legend lines.
    /// @details Each legend line is wrapped in a @c <br /> at both ends, so joining them
    ///     leaves a leading break, a trailing break, and a doubled break between every entry.
    ///     Removing those keeps the HTML legend card from being padded out with blank lines.
    ///     Entry spacing then comes from the line height and the card's own padding.
    [[nodiscard]]
    static wxString TidyHtmlLegendBreaks(wxString content);
    /// @brief Builds the RTF color table (used by Windows and macOS).
    /// @returns RTF-formatted header sections (used by Windows and macOS).
    static std::tuple<wxString, wxString, wxString>
    BuildColorTable(const wxFont& textViewFont, const HighlighterColors& highlighterColors,
                    const wxColour& backgroundColor);
    [[nodiscard]]
    static TextLegends BuildLegends(const MarkupFormat format, const TextLegendLines& legendLines,
                                    const wxFont& textViewFont);
    [[nodiscard]]
    static wxString BuildLegend(const MarkupFormat format, const wxString& legendLine,
                                const TextLegendLines& legendLines, const wxFont& textViewFont);
    [[nodiscard]]
    TextHeader BuildHeader(const MarkupFormat format, const wxColour& backgroundColor,
                           const HighlighterColors& highlighterColors, const wxFont& textViewFont);

    /// @brief Creates a read-only @c wxWebView for a highlighted-text window.
    /// @param parent The parent window.
    /// @param ID The window's ID.
    /// @param label The window's name/caption.
    /// @returns The created window, or @c nullptr if no webview backend is available.
    [[nodiscard]]
    wxWebView* CreateHighlightedTextWindow(wxWindow* parent, const int ID, const wxString& label);

    void LoadDCTextWindow(const std::wstring& htmlBuffer, const std::wstring& rtfBuffer);
    void LoadHJTextWindow(const std::wstring& htmlBuffer, const std::wstring& rtfBuffer);
    void LoadSpacheTextWindow(const std::wstring& htmlBuffer, const std::wstring& rtfBuffer);
    void LoadSixCharsTextWindow(const std::wstring& htmlBuffer, const std::wstring& rtfBuffer);
    void LoadThreeSyllTextWindow(const std::wstring& htmlBuffer, const std::wstring& rtfBuffer);
    wxWebView* LoadTextWindow(wxWebView* textWindow, const int ID, const wxString& label,
                              const std::wstring& htmlBuffer, const std::wstring& rtfBuffer);

    bool OnCreate(const wxString& path, long flags) final;

    void UpdateSourceFileModifiedTime();
    void OnRealTimeTimer([[maybe_unused]] wxTimerEvent& event);
    // restores a highlighted-text window's scroll offset once its refreshed page loads
    void OnHighlightedTextLoaded(wxWebViewEvent& event);

    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_dupWordData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_misspelledWordData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_incorrectArticleData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_passiveVoiceData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_wordyPhraseData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_overusedWordsBySentenceData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_clichePhraseData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_redundantPhraseData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_wordingErrorData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_overlyLongSentenceData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>
        m_sentenceStartingWithConjunctionsData{
            std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
        };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>
        m_sentenceStartingWithLowercaseData{
            std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
        };
    wxWebView* m_dcTextWindow{ nullptr };
    wxWebView* m_spacheTextWindow{ nullptr };
    wxWebView* m_hjTextWindow{ nullptr };

    // HTML (display/export) and paper-white RTF (export) buffers for the
    // highlighted-text windows, keyed by window ID
    HighlightedTextBufferMap m_highlightedTextBuffers;
    // pending scroll offsets to restore after a highlighted-text refresh, keyed by window ID
    std::map<wxWindowID, int> m_highlightedScrollOffsets;

    wxDateTime m_sourceFileLastModified;
    constexpr static int REALTIME_UPDATE_INTERVAL{ 5000 }; // in milliseconds
    wxTimer m_realTimeTimer;

    static wxString GetSentenceWordCountsColumnName() { return _DT(L"SENTENCE_WORD_COUNTS"); }

    static wxString GetSentenceIndicesColumnName() { return _DT(L"SENTENCE_INDICES"); }

    static wxString GetSyllableCountsColumnName() { return _DT(L"SYLLABLE_COUNTS"); }

    static wxString GetWordTypeGroupColumnName() { return _DT(L"WORD_TYPE"); }

    static wxString GetGroupColumnName() { return _DT(L"GROUP"); }

    std::shared_ptr<Wisteria::Data::Dataset> m_sentenceWordLengths{
        std::make_shared<Wisteria::Data::Dataset>()
    };
    std::shared_ptr<Wisteria::Data::Dataset> m_syllableCounts{
        std::make_shared<Wisteria::Data::Dataset>()
    };

  public:
    wxDECLARE_DYNAMIC_CLASS(ProjectDoc);
    };

#endif // PROJECT_DOC_H
