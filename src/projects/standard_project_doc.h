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

#ifndef PROJECT_DOC_H
#define PROJECT_DOC_H

#include "../app/readability_app.h"
#include "base_project_doc.h"
#include <wx/timer.h>

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

    bool AddFryTest(bool setFocus = true) final;
    bool AddRaygorTest(bool setFocus = true) final;
    bool AddGilliamPenaMountainFryTest(bool setFocus = true) final;
    bool AddFraseTest(bool setFocus = true) final;
    bool AddSchwartzTest(bool setFocus = true) final;
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
    void AddDB2Plot(const bool setFocus = true) final;
    void AddFleschChart(const bool setFocus = true) final;
    void AddLixGermanGauge(const bool setFocus) final;
    void AddLixGauge(const bool setFocus = true) final;
    void AddCrawfordGraph(const bool setFocus = true) final;
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
    /// @brief Builds the tags used to highlight words in RTF or Pango.
    /// @param highlightColor The default highlight color.\n
    ///     This is only used for RTF, not Pango.
    /// @param highlighterColors Highlight colors used for the tags when
    ///     building for Pango (not used for RTF, since that uses indices into a color table).
    /// @returns The tags used to build RTF or Pango content.
    [[nodiscard]]
    HighlighterTags
    BuildHighlighterTags([[maybe_unused]] const wxColour& highlightColor,
                         [[maybe_unused]] const HighlighterColors& highlighterColors) const;
    /// @brief Formats the main font for an RTF's header.
    std::pair<wxString, wxString> FormatRtfHeaderFont(const wxFont& textViewFont,
                                                      const size_t mainFontColorIndex);
    [[nodiscard]]
    std::pair<TextLegendLines, size_t>
    BuildLegendLines(const HighlighterTags& highlighterTags) const;
    [[nodiscard]]
    wxString BuildLegendLine(const HighlighterTags& highlighterTags, const wxString& legendStr);
    /// @brief Builds the RTF color table (used by Windows and macOS).
    /// @returns RTF-formatted header sections (used by Windows and macOS).
    std::tuple<wxString, wxString, wxString>
    BuildColorTable(const wxFont& textViewFont, const HighlighterColors& highlighterColors,
                    const wxColour& backgroundColor);
    [[nodiscard]]
    TextLegends BuildLegends(const TextLegendLines& legendLines, const wxFont& textViewFont);
    [[nodiscard]]
    wxString BuildLegend(const wxString& legendLine, const TextLegendLines& legendLines,
                         const wxFont& textViewFont);
    [[nodiscard]]
    TextHeader BuildHeader(const wxColour& backgroundColor,
                           [[maybe_unused]] const HighlighterColors& highlighterColors,
                           const wxFont& textViewFont);

    static void
    SetFormattedTextAndRestoreInsertionPoint(Wisteria::UI::FormattedTextCtrl* textWindow,
                                             const wchar_t* formattedText)
        {
        const auto cursorPos = textWindow->GetInsertionPoint();
        textWindow->SetFormattedText(formattedText);
        textWindow->SetInsertionPoint(cursorPos);
        textWindow->ShowPosition(cursorPos);
        }

    void LoadDCTextWindow(const std::wstring& mainBuffer, const std::wstring& paperBuffer);
    void LoadHJTextWindow(const std::wstring& mainBuffer, const std::wstring& paperBuffer);
    void LoadSpacheTextWindow(const std::wstring& mainBuffer, const std::wstring& paperBuffer);
    void LoadSixCharsTextWindow(const std::wstring& mainBuffer, const std::wstring& paperBuffer);
    void LoadThreeSyllTextWindow(const std::wstring& mainBuffer, const std::wstring& paperBuffer);
    Wisteria::UI::FormattedTextCtrl* LoadTextWindow(Wisteria::UI::FormattedTextCtrl* textWindow,
                                                    const int ID, const wxString& label,
                                                    const std::wstring& mainBuffer,
                                                    const std::wstring& paperBuffer);

    bool OnCreate(const wxString& path, long flags) final;

    void UpdateSourceFileModifiedTime();
    void OnRealTimeTimer([[maybe_unused]] wxTimerEvent& event);

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
    Wisteria::UI::FormattedTextCtrl* m_dcTextWindow{ nullptr };
    Wisteria::UI::FormattedTextCtrl* m_spacheTextWindow{ nullptr };
    Wisteria::UI::FormattedTextCtrl* m_hjTextWindow{ nullptr };

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
