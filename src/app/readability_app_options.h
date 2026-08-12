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

#ifndef READABILITY_APP_OPTIONS_H
#define READABILITY_APP_OPTIONS_H

#include "../OleanderStemmingLibrary/src/stemming.h"
#include "../Wisteria-Dataviz/src/base/image.h"
#include "../Wisteria-Dataviz/src/data/dataset.h"
#include "../Wisteria-Dataviz/src/graphs/barchart.h"
#include "../Wisteria-Dataviz/src/graphs/boxplot.h"
#include "../Wisteria-Dataviz/src/graphs/histogram.h"
#include "../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../Wisteria-Dataviz/src/util/warningmanager.h"
#include "../Wisteria-Dataviz/src/util/xml_format.h"
#include "../graphs/raygorgraph.h"
#include "../readability/readability.h"
#include "../readability/readability_project_test.h"
#include "../readability/readability_test.h"
#include "../results-format/readability_messages.h"
#include "../tinyxml2/tinyxml2.h"
#include "optionenums.h"
#include <wx/colourdata.h>

/// @brief Class for managing what is included in the statistics section.
class StatisticsInfo
    {
  public:
    StatisticsInfo() { Reset(); }

    void Reset() noexcept { EnableAll(); }

    void EnableAll() noexcept { m_statItemsToInclude.set(); }

    void Set(const wxString& val)
        {
        try
            {
            m_statItemsToInclude = std::bitset<64>(val.ToStdString());
            }
        catch (const std::exception&)
            {
            wxLogError(L"Invalid statistics info settings value.");
            m_statItemsToInclude = 0;
            }
        }

    [[nodiscard]]
    wxString ToString() const
        {
        return wxString{ m_statItemsToInclude.to_string() };
        }

    void EnableReport(const bool enable) { m_statItemsToInclude.set(0, enable); }

    [[nodiscard]]
    bool IsReportEnabled() const
        {
        return m_statItemsToInclude[0];
        }

    void EnableTable(const bool enable) { m_statItemsToInclude.set(1, enable); }

    [[nodiscard]]
    bool IsTableEnabled() const
        {
        return m_statItemsToInclude[1];
        }

  private:
    std::bitset<64> m_statItemsToInclude{ 0 };
    };

/// Class for managing what is included in the statistics reports.
/// By default, all statistics are included (except additional notes).
class StatisticsReportInfo
    {
  public:
    StatisticsReportInfo() { Reset(); }

    void Reset()
        {
        // enable all flags by default, except for additional notes
        EnableAll();
        EnableExtendedInformation(false);
        }

    void EnableAll() noexcept { m_statReportItemsToInclude.set(); }

    void Set(const wxString& val)
        {
        try
            {
            m_statReportItemsToInclude = std::bitset<10>(val.ToStdString());
            }
        catch (const std::exception&)
            {
            wxLogError(L"Invalid statistics report settings value.");
            m_statReportItemsToInclude = 0;
            }
        }

    [[nodiscard]]
    wxString ToString() const
        {
        return m_statReportItemsToInclude.to_string().c_str();
        }

    void EnableParagraph(const bool enable) { m_statReportItemsToInclude.set(0, enable); }

    [[nodiscard]]
    bool IsParagraphEnabled() const
        {
        return m_statReportItemsToInclude[0];
        }

    void EnableSentences(const bool enable) { m_statReportItemsToInclude.set(1, enable); }

    [[nodiscard]]
    bool IsSentencesEnabled() const
        {
        return m_statReportItemsToInclude[1];
        }

    void EnableWords(const bool enable) { m_statReportItemsToInclude.set(2, enable); }

    [[nodiscard]]
    bool IsWordsEnabled() const
        {
        return m_statReportItemsToInclude[2];
        }

    void EnableExtendedWords(const bool enable) { m_statReportItemsToInclude.set(3, enable); }

    [[nodiscard]]
    bool IsExtendedWordsEnabled() const
        {
        return m_statReportItemsToInclude[3];
        }

    void EnableGrammar(const bool enable) { m_statReportItemsToInclude.set(4, enable); }

    [[nodiscard]]
    bool IsGrammarEnabled() const
        {
        return m_statReportItemsToInclude[4];
        }

    void EnableNotes(const bool enable) { m_statReportItemsToInclude.set(5, enable); }

    [[nodiscard]]
    bool IsNotesEnabled() const
        {
        return m_statReportItemsToInclude[5];
        }

    void EnableExtendedInformation(const bool enable) { m_statReportItemsToInclude.set(6, enable); }

    [[nodiscard]]
    bool IsExtendedInformationEnabled() const
        {
        return m_statReportItemsToInclude[6];
        }

    void EnableDolchCoverage(const bool enable) { m_statReportItemsToInclude.set(7, enable); }

    [[nodiscard]]
    bool IsDolchCoverageEnabled() const
        {
        return m_statReportItemsToInclude[7];
        }

    void EnableDolchWords(const bool enable) { m_statReportItemsToInclude.set(8, enable); }

    [[nodiscard]]
    bool IsDolchWordsEnabled() const
        {
        return m_statReportItemsToInclude[8];
        }

    void EnableDolchExplanation(const bool enable) { m_statReportItemsToInclude.set(9, enable); }

    [[nodiscard]]
    bool IsDolchExplanationEnabled() const
        {
        return m_statReportItemsToInclude[9];
        }

    /// @returns Whether anything in the standard statistics are enabled.
    [[nodiscard]]
    bool HasStatisticsEnabled() const
        {
        return (IsParagraphEnabled() || IsSentencesEnabled() || IsWordsEnabled() ||
                IsExtendedWordsEnabled() || IsGrammarEnabled() || IsExtendedInformationEnabled());
        }

    /// @returns Whether any Dolch statistics are enabled.
    [[nodiscard]]
    bool HasDolchStatisticsEnabled() const
        {
        return (IsDolchCoverageEnabled() || IsDolchWordsEnabled() || IsDolchExplanationEnabled());
        }

  private:
    std::bitset<10> m_statReportItemsToInclude{ 0 };
    };

class GrammarInfo
    {
  public:
    GrammarInfo() { EnableAll(); } // enable all flags by default

    void EnableAll() noexcept { m_grammarItemsToInclude.set(); }

    void Set(const wxString& val)
        {
        try
            {
            m_grammarItemsToInclude = std::bitset<64>(val.ToStdString());
            }
        catch (const std::exception&)
            {
            wxLogError(L"Invalid grammar info settings value.");
            m_grammarItemsToInclude = 0;
            }
        }

    [[nodiscard]]
    wxString ToString() const
        {
        return wxString{ m_grammarItemsToInclude.to_string() };
        }

    void EnableHighlightedReport(const bool enable) { m_grammarItemsToInclude.set(0, enable); }

    [[nodiscard]]
    bool IsHighlightedReportEnabled() const
        {
        return m_grammarItemsToInclude[0];
        }

    void EnableMisspellings(const bool enable) { m_grammarItemsToInclude.set(1, enable); }

    [[nodiscard]]
    bool IsMisspellingsEnabled() const
        {
        return m_grammarItemsToInclude[1];
        }

    void EnableRepeatedWords(const bool enable) { m_grammarItemsToInclude.set(2, enable); }

    [[nodiscard]]
    bool IsRepeatedWordsEnabled() const
        {
        return m_grammarItemsToInclude[2];
        }

    void EnableArticleMismatches(const bool enable) { m_grammarItemsToInclude.set(3, enable); }

    [[nodiscard]]
    bool IsArticleMismatchesEnabled() const
        {
        return m_grammarItemsToInclude[3];
        }

    void EnableWordingErrors(const bool enable) { m_grammarItemsToInclude.set(4, enable); }

    [[nodiscard]]
    bool IsWordingErrorsEnabled() const
        {
        return m_grammarItemsToInclude[4];
        }

    void EnableRedundantPhrases(const bool enable) { m_grammarItemsToInclude.set(5, enable); }

    [[nodiscard]]
    bool IsRedundantPhrasesEnabled() const
        {
        return m_grammarItemsToInclude[5];
        }

    void EnableOverUsedWordsBySentence(const bool enable)
        {
        m_grammarItemsToInclude.set(6, enable);
        }

    [[nodiscard]]
    bool IsOverUsedWordsBySentenceEnabled() const
        {
        return m_grammarItemsToInclude[6];
        }

    void EnableWordyPhrases(const bool enable) { m_grammarItemsToInclude.set(7, enable); }

    [[nodiscard]]
    bool IsWordyPhrasesEnabled() const
        {
        return m_grammarItemsToInclude[7];
        }

    void EnableCliches(const bool enable) { m_grammarItemsToInclude.set(8, enable); }

    [[nodiscard]]
    bool IsClichesEnabled() const
        {
        return m_grammarItemsToInclude[8];
        }

    void EnablePassiveVoice(const bool enable) { m_grammarItemsToInclude.set(9, enable); }

    [[nodiscard]]
    bool IsPassiveVoiceEnabled() const
        {
        return m_grammarItemsToInclude[9];
        }

    void EnableConjunctionStartingSentences(const bool enable)
        {
        m_grammarItemsToInclude.set(10, enable);
        }

    [[nodiscard]]
    bool IsConjunctionStartingSentencesEnabled() const
        {
        return m_grammarItemsToInclude[10];
        }

    void EnableLowercaseSentences(const bool enable) { m_grammarItemsToInclude.set(11, enable); }

    [[nodiscard]]
    bool IsLowercaseSentencesEnabled() const
        {
        return m_grammarItemsToInclude[11];
        }

    [[nodiscard]]
    bool IsAnyFeatureEnabled() const
        {
        return (IsHighlightedReportEnabled() || IsMisspellingsEnabled() ||
                IsRepeatedWordsEnabled() || IsArticleMismatchesEnabled() ||
                IsWordingErrorsEnabled() || IsRedundantPhrasesEnabled() ||
                IsOverUsedWordsBySentenceEnabled() || IsWordyPhrasesEnabled() ||
                IsClichesEnabled() || IsPassiveVoiceEnabled() ||
                IsConjunctionStartingSentencesEnabled() || IsLowercaseSentencesEnabled());
        }

  private:
    std::bitset<64> m_grammarItemsToInclude{ 0 };
    };

/// @brief Class for managing what is included in the words breakdown section.
class WordsBreakdownInfo
    {
  public:
    WordsBreakdownInfo() { EnableAll(); }

    void EnableAll() noexcept { m_wordsBreakdownItemsToInclude.set(); }

    void Set(const wxString& val)
        {
        try
            {
            m_wordsBreakdownItemsToInclude = std::bitset<64>(val.ToStdString());
            }
        catch (const std::exception&)
            {
            wxLogError(L"Invalid words breakdown settings value.");
            m_wordsBreakdownItemsToInclude = 0;
            }
        }

    [[nodiscard]]
    wxString ToString() const
        {
        return wxString{ m_wordsBreakdownItemsToInclude.to_string() };
        }

    void EnableWordBarchart(const bool enable) { m_wordsBreakdownItemsToInclude.set(0, enable); }

    [[nodiscard]]
    bool IsWordBarchartEnabled() const
        {
        return m_wordsBreakdownItemsToInclude[0];
        }

    void EnableSyllableHistogram(const bool enable)
        {
        m_wordsBreakdownItemsToInclude.set(1, enable);
        }

    [[nodiscard]]
    bool IsSyllableGraphsEnabled() const
        {
        return m_wordsBreakdownItemsToInclude[1];
        }

    void Enable3PlusSyllables(const bool enable) { m_wordsBreakdownItemsToInclude.set(2, enable); }

    [[nodiscard]]
    bool Is3PlusSyllablesEnabled() const
        {
        return m_wordsBreakdownItemsToInclude[2];
        }

    void Enable6PlusCharacter(const bool enable) { m_wordsBreakdownItemsToInclude.set(3, enable); }

    [[nodiscard]]
    bool Is6PlusCharacterEnabled() const
        {
        return m_wordsBreakdownItemsToInclude[3];
        }

    void EnableDCUnfamiliar(const bool enable) { m_wordsBreakdownItemsToInclude.set(4, enable); }

    [[nodiscard]]
    bool IsDCUnfamiliarEnabled() const
        {
        return m_wordsBreakdownItemsToInclude[4];
        }

    void EnableSpacheUnfamiliar(const bool enable)
        {
        m_wordsBreakdownItemsToInclude.set(5, enable);
        }

    [[nodiscard]]
    bool IsSpacheUnfamiliarEnabled() const
        {
        return m_wordsBreakdownItemsToInclude[5];
        }

    void EnableHarrisJacobsonUnfamiliar(const bool enable)
        {
        m_wordsBreakdownItemsToInclude.set(6, enable);
        }

    [[nodiscard]]
    bool IsHarrisJacobsonUnfamiliarEnabled() const
        {
        return m_wordsBreakdownItemsToInclude[6];
        }

    void EnableCustomTestsUnfamiliar(const bool enable)
        {
        m_wordsBreakdownItemsToInclude.set(7, enable);
        }

    [[nodiscard]]
    bool IsCustomTestsUnfamiliarEnabled() const
        {
        return m_wordsBreakdownItemsToInclude[7];
        }

    void EnableAllWords(const bool enable) { m_wordsBreakdownItemsToInclude.set(8, enable); }

    [[nodiscard]]
    bool IsAllWordsEnabled() const
        {
        return m_wordsBreakdownItemsToInclude[8];
        }

    void EnableKeyWords(const bool enable) { m_wordsBreakdownItemsToInclude.set(9, enable); }

    [[nodiscard]]
    bool IsKeyWordsEnabled() const
        {
        return m_wordsBreakdownItemsToInclude[9];
        }

    void EnableProperNouns(const bool enable) { m_wordsBreakdownItemsToInclude.set(10, enable); }

    [[nodiscard]]
    bool IsProperNounsEnabled() const
        {
        return m_wordsBreakdownItemsToInclude[10];
        }

    void EnableContractions(const bool enable) { m_wordsBreakdownItemsToInclude.set(11, enable); }

    [[nodiscard]]
    bool IsContractionsEnabled() const
        {
        return m_wordsBreakdownItemsToInclude[11];
        }

    void EnableWordCloud(const bool enable) { m_wordsBreakdownItemsToInclude.set(12, enable); }

    [[nodiscard]]
    bool IsWordCloudEnabled() const
        {
        return m_wordsBreakdownItemsToInclude[12];
        }

  private:
    std::bitset<64> m_wordsBreakdownItemsToInclude{ 0 };
    };

/// @brief Class for managing what is included in the sentences section.
class SentencesBreakdownInfo
    {
  public:
    SentencesBreakdownInfo() { Reset(); }

    void Reset() noexcept { EnableAll(); }

    void EnableAll() noexcept { m_sentenceBreakdownItemsToInclude.set(); }

    void Set(const wxString& val)
        {
        try
            {
            m_sentenceBreakdownItemsToInclude = std::bitset<64>(val.ToStdString());
            }
        catch (const std::exception&)
            {
            wxLogError(L"Invalid sentences breakdown settings value.");
            m_sentenceBreakdownItemsToInclude = 0;
            }
        }

    [[nodiscard]]
    wxString ToString() const
        {
        return wxString{ m_sentenceBreakdownItemsToInclude.to_string() };
        }

    void EnableLongSentences(const bool enable)
        {
        m_sentenceBreakdownItemsToInclude.set(0, enable);
        }

    [[nodiscard]]
    bool IsLongSentencesEnabled() const
        {
        return m_sentenceBreakdownItemsToInclude[0];
        }

    void EnableLengthsBoxPlot(const bool enable)
        {
        m_sentenceBreakdownItemsToInclude.set(1, enable);
        }

    [[nodiscard]]
    bool IsLengthsBoxPlotEnabled() const
        {
        return m_sentenceBreakdownItemsToInclude[1];
        }

    void EnableLengthsHeatmap(const bool enable)
        {
        m_sentenceBreakdownItemsToInclude.set(2, enable);
        }

    [[nodiscard]]
    bool IsLengthsHeatmapEnabled() const
        {
        return m_sentenceBreakdownItemsToInclude[2];
        }

    void EnableLengthsHistogram(const bool enable)
        {
        m_sentenceBreakdownItemsToInclude.set(3, enable);
        }

    [[nodiscard]]
    bool IsLengthsHistogramEnabled() const
        {
        return m_sentenceBreakdownItemsToInclude[3];
        }

  private:
    std::bitset<64> m_sentenceBreakdownItemsToInclude{ 0 };
    };

/// @brief Options that need to be loaded from the settings file before
///     the application is fully constructed.
/// @details ReadabilityAppOptions cannot be constructed until the `wxApp::OnInit()`
///     is called and the top-level window constructed.
///     These are the options needed prior to that.
class PreAppInitOptions
    {
  public:
    bool LoadOptionsFile(wxString optionsFile);

    bool m_appWindowMaximized{ true };
    int m_appWindowWidth{ 800 };
    int m_appWindowHeight{ 700 };
    UiLanguage m_uiLanguage{ UiLanguage::Default };
    bool m_logAppendDailyLog{ false };
    wxString m_userName;
    };

class ReadabilityAppOptions
    {
  public:
    /// @private
    using TestCollectionType = readability::readability_test_collection<
        readability::readability_project_test<Wisteria::Data::Dataset>>;
    ReadabilityAppOptions();
    /// set the third parameter to false if you only want to load the settings from this file and
    /// write back to a different settings file
    bool LoadOptionsFile(wxString optionsFile, const bool loadOnlyGeneralOptions = false,
                         const bool writeChangesBackToThisFile = true);
    // saves options file, defaults to the current project file
    bool SaveOptionsFile(const wxString& optionsFile = wxEmptyString);
    /// Resets settings back to the factory default.
    void ResetSettings();

    /// Resets color theming to the system colors.
    void SetColorsFromSystem();
    void SetFonts();

    // editor
    [[nodiscard]]
    wxFont GetEditorFont() const
        {
        return m_editorFont;
        }

    void SetEditorFont(const wxFont& font)
        {
        if (font.IsOk())
            {
            m_editorFont = font;
            }
        }

    [[nodiscard]]
    bool IsEditorIndenting() const noexcept
        {
        return m_editorIndent;
        }

    void IndentEditor(const bool indent = true) noexcept { m_editorIndent = indent; }

    [[nodiscard]]
    bool IsEditorShowSpaceAfterParagraph() const noexcept
        {
        return m_editorSpaceAfterNewlines;
        }

    void AddParagraphSpaceInEditor(const bool spaces = true) noexcept
        {
        m_editorSpaceAfterNewlines = spaces;
        }

    [[nodiscard]]
    wxTextAttrAlignment GetEditorTextAlignment() const noexcept
        {
        return m_editorTextAlignment;
        }

    void SetEditorTextAlignment(wxTextAttrAlignment align) { m_editorTextAlignment = align; }

    [[nodiscard]]
    wxTextAttrLineSpacing GetEditorLineSpacing() const noexcept
        {
        return m_editorLineSpacing;
        }

    void SetEditorLineSpacing(wxTextAttrLineSpacing spacing) { m_editorLineSpacing = spacing; }

    [[nodiscard]]
    TextHighlight GetTextHighlightMethod() const noexcept
        {
        return m_textHighlight;
        }

    [[nodiscard]]
    bool IsAppendingDailyLog() const noexcept
        {
        return m_logAppendDailyLog;
        }

    void AppendDailyLog(const bool append) noexcept { m_logAppendDailyLog = append; }

    /// @returns @c true if Lua unsafe mode is enabled (os, io, debug libraries available).
    /// @warning Enabling this allows scripts to execute system commands and access files.
    [[nodiscard]]
    bool IsLuaUnsafeModeEnabled() const noexcept
        {
        return m_luaUnsafeMode;
        }

    /// @brief Enables or disables Lua unsafe mode.
    /// @param enable @c true to enable os, io, and debug libraries in Lua scripts.
    /// @warning Enabling this allows scripts to execute system commands and access files.
    void EnableLuaUnsafeMode(const bool enable) noexcept { m_luaUnsafeMode = enable; }

    /// @returns @c true if the Developer ribbon tab is shown.
    [[nodiscard]]
    bool IsShowingDeveloperTab() const noexcept
        {
        return m_showDeveloperTab;
        }

    /// @brief Shows or hides the Developer ribbon tab.
    void ShowDeveloperTab(const bool show) noexcept { m_showDeveloperTab = show; }

    /// @returns @c true if the Log ribbon tab is shown.
    [[nodiscard]]
    bool IsShowingLogTab() const noexcept
        {
        return m_showLogTab;
        }

    /// @brief Shows or hides the Log ribbon tab.
    void ShowLogTab(const bool show) noexcept { m_showLogTab = show; }

    /// @returns @c true if the Log tab auto-refresh is enabled.
    [[nodiscard]]
    bool IsLogAutoRefresh() const noexcept
        {
        return m_logAutoRefresh;
        }

    /// @brief Enables or disables Log tab auto-refresh.
    void SetLogAutoRefresh(const bool refresh) noexcept { m_logAutoRefresh = refresh; }

    /// @returns @c true if GPU acceleration is disabled for reports.
    [[nodiscard]]
    bool IsGpuAccelerationDisabled() const noexcept
        {
        return m_disableGpuAcceleration;
        }

    /// @brief Disables (or re-enables) GPU acceleration for reports.
    /// @param disable @c true to disable GPU acceleration.
    /// @note This takes effect after the program is restarted.
    void DisableGpuAcceleration(const bool disable) noexcept { m_disableGpuAcceleration = disable; }

    /// @returns The CSS theme overlaid on top of "default.css" for reports.
    [[nodiscard]]
    wxString GetReportTheme() const
        {
        return m_reportTheme;
        }

    /// @brief Sets the CSS theme overlaid on top of "default.css" for reports.
    void SetReportTheme(const wxString& theme) { m_reportTheme = theme; }

    void SetTextHighlightMethod(const TextHighlight highlight) noexcept
        {
        m_textHighlight = highlight;
        }

    // color to highlight difficult words
    [[nodiscard]]
    wxColour GetTextHighlightColor() const
        {
        return m_textHighlightColor;
        }

    void SetTextHighlightColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_textHighlightColor = color;
            }
        }

    // color to highlight ignored sentences
    [[nodiscard]]
    wxColour GetExcludedTextHighlightColor() const
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
    wxColour GetDuplicateWordHighlightColor() const
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
    wxColour GetWordyPhraseHighlightColor() const
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

    // font functions for the text windows
    [[nodiscard]]
    wxFont GetTextViewFont() const
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

    [[nodiscard]]
    wxColour GetTextFontColor() const
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
    wxColour GetDolchConjunctionsColor() const
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
    wxColour GetDolchPrepositionsColor() const
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
    wxColour GetDolchPronounsColor() const
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
    wxColour GetDolchAdverbsColor() const
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
    wxColour GetDolchAdjectivesColor() const
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
    wxColour GetDolchVerbsColor() const
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
    wxColour GetDolchNounsColor() const
        {
        return m_dolchNounsColor;
        }

    void SetDolchNounsColor(const wxColour& color)
        {
        if (color.IsOk())
            {
            m_dolchNounsColor = color;
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

    // method to determine what a long sentence is
    [[nodiscard]]
    LongSentence GetLongSentenceMethod() const noexcept
        {
        return m_longSentenceMethod;
        }

    void SetLongSentenceMethod(const LongSentence method) noexcept
        {
        m_longSentenceMethod = method;
        }

    [[nodiscard]]
    int GetDifficultSentenceLength() const noexcept
        {
        return m_difficultSentenceLength;
        }

    void SetDifficultSentenceLength(const int length) noexcept
        {
        m_difficultSentenceLength = length;
        }

    // method to determine how to syllabize numerals
    [[nodiscard]]
    NumeralSyllabize GetNumeralSyllabicationMethod() const noexcept
        {
        return m_numeralSyllabicationMethod;
        }

    void SetNumeralSyllabicationMethod(const NumeralSyllabize method) noexcept
        {
        m_numeralSyllabicationMethod = method;
        }

    // whether to ignore blank lines when figuring out if we are at the end of a paragraph
    [[nodiscard]]
    bool IsIgnoringBlankLinesForParagraphsParser() const noexcept
        {
        return m_ignoreBlankLinesForParagraphsParser;
        }

    void IgnoreBlankLinesForParagraphsParser(const bool ignore) noexcept
        {
        m_ignoreBlankLinesForParagraphsParser = ignore;
        }

    // whether we should ignore indenting when parsing paragraphs
    [[nodiscard]]
    bool IsIgnoringIndentingForParagraphsParser() const noexcept
        {
        return m_ignoreIndentingForParagraphsParser;
        }

    void IgnoreIndentingForParagraphsParser(const bool ignore) noexcept
        {
        m_ignoreIndentingForParagraphsParser = ignore;
        }

    // whether the first word of a sentence must be capitalized
    [[nodiscard]]
    bool GetSentenceStartMustBeUppercased() const noexcept
        {
        return m_sentenceStartMustBeUppercased;
        }

    void SetSentenceStartMustBeUppercased(const bool uppercased) noexcept
        {
        m_sentenceStartMustBeUppercased = uppercased;
        }

    // whether to use aggressive exclusion
    [[nodiscard]]
    bool IsExcludingAggressively() const noexcept
        {
        return m_aggressiveExclusion;
        }

    void AggressiveExclusion(const bool aggressive = true) noexcept
        {
        m_aggressiveExclusion = aggressive;
        }

    // whether trailing copyright/trademark paragraphs are getting ignored
    [[nodiscard]]
    bool IsExcludingTrailingCopyrightNoticeParagraphs() const noexcept
        {
        return m_excludeTrailingCopyrightNoticeParagraphs;
        }

    void ExcludeTrailingCopyrightNoticeParagraphs(const bool ignore = true) noexcept
        {
        m_excludeTrailingCopyrightNoticeParagraphs = ignore;
        }

    // whether trailing citation paragraphs are getting ignored
    [[nodiscard]]
    bool IsExcludingTrailingCitations() const noexcept
        {
        return m_excludeTrailingCitations;
        }

    void ExcludeTrailingCitations(const bool ignore = true) noexcept
        {
        m_excludeTrailingCitations = ignore;
        }

    // whether file addresses are getting ignored
    [[nodiscard]]
    bool IsExcludingFileAddresses() const noexcept
        {
        return m_excludeFileAddresses;
        }

    void ExcludeFileAddresses(const bool ignore = true) noexcept
        {
        m_excludeFileAddresses = ignore;
        }

    // whether numerals are getting ignored
    [[nodiscard]]
    bool IsExcludingNumerals() const noexcept
        {
        return m_excludeNumerals;
        }

    void ExcludeNumerals(const bool ignore = true) noexcept { m_excludeNumerals = ignore; }

    // whether Proper Nouns are getting ignored
    [[nodiscard]]
    bool IsExcludingProperNouns() const noexcept
        {
        return m_excludeProperNouns;
        }

    void ExcludeProperNouns(const bool ignore = true) noexcept { m_excludeProperNouns = ignore; }

    // file path to phrases to exclude from analysis
    [[nodiscard]]
    wxString GetExcludedPhrasesPath() const
        {
        return m_excludedPhrasesPath;
        }

    void SetExcludedPhrasesPath(const wxString& path) { m_excludedPhrasesPath = path; }

    // bundled Plain Language Guide phrase list filename (default for new standard
    // projects); empty means the feature is disabled ("None")
    [[nodiscard]]
    wxString GetPlainLanguageGuideListName() const
        {
        return m_plainLanguageGuideListName;
        }

    void SetPlainLanguageGuideListName(const wxString& name)
        {
        m_plainLanguageGuideListName = name;
        }

    // Tags for excluding blocks of text
    [[nodiscard]]
    const std::vector<std::pair<wchar_t, wchar_t>>& GetExclusionBlockTags() const noexcept
        {
        return m_exclusionBlockTags;
        }

    std::vector<std::pair<wchar_t, wchar_t>>& GetExclusionBlockTags() noexcept
        {
        return m_exclusionBlockTags;
        }

    void SetExclusionBlockTags(const std::vector<std::pair<wchar_t, wchar_t>>& tags)
        {
        m_exclusionBlockTags = tags;
        }

    // whether the first occurrence of an excluded phrase should be included
    [[nodiscard]]
    bool IsIncludingExcludedPhraseFirstOccurrence() const noexcept
        {
        return m_includeExcludedPhraseFirstOccurrence;
        }

    void IncludeExcludedPhraseFirstOccurrence(const bool include) noexcept
        {
        m_includeExcludedPhraseFirstOccurrence = include;
        }

    // method to determine how to parse text into paragraphs
    [[nodiscard]]
    ParagraphParse GetParagraphsParsingMethod() const noexcept
        {
        return m_paragraphsParsingMethod;
        }

    void SetParagraphsParsingMethod(const ParagraphParse method) noexcept
        {
        m_paragraphsParsingMethod = method;
        }

    // method to determine how to handle headers and lists
    [[nodiscard]]
    InvalidSentence GetInvalidSentenceMethod() const noexcept
        {
        return m_invalidSentenceMethod;
        }

    void SetInvalidSentenceMethod(const InvalidSentence method) noexcept
        {
        m_invalidSentenceMethod = method;
        }

    /// Number of words that will make an incomplete sentence actually complete
    [[nodiscard]]
    size_t GetIncludeIncompleteSentencesIfLongerThanValue() const noexcept
        {
        return m_includeIncompleteSentencesIfLongerThan;
        }

    void SetIncludeIncompleteSentencesIfLongerThanValue(const size_t wordCount) noexcept
        {
        m_includeIncompleteSentencesIfLongerThan = wordCount;
        }

    // grammar
    [[nodiscard]]
    bool SpellCheckIsIgnoringProperNouns() const noexcept
        {
        return m_spellcheck_ignore_proper_nouns;
        }

    void SpellCheckIgnoreProperNouns(const bool ignore) noexcept
        {
        m_spellcheck_ignore_proper_nouns = ignore;
        }

    [[nodiscard]]
    bool SpellCheckIsIgnoringUppercased() const noexcept
        {
        return m_spellcheck_ignore_uppercased;
        }

    void SpellCheckIgnoreUppercased(const bool ignore) noexcept
        {
        m_spellcheck_ignore_uppercased = ignore;
        }

    [[nodiscard]]
    bool SpellCheckIsIgnoringNumerals() const noexcept
        {
        return m_spellcheck_ignore_numerals;
        }

    void SpellCheckIgnoreNumerals(const bool ignore) noexcept
        {
        m_spellcheck_ignore_numerals = ignore;
        }

    [[nodiscard]]
    bool SpellCheckIsIgnoringFileAddresses() const noexcept
        {
        return m_spellcheck_ignore_file_addresses;
        }

    void SpellCheckIgnoreFileAddresses(const bool ignore) noexcept
        {
        m_spellcheck_ignore_file_addresses = ignore;
        }

    [[nodiscard]]
    bool SpellCheckIsIgnoringProgrammerCode() const noexcept
        {
        return m_spellcheck_ignore_programmer_code;
        }

    void SpellCheckIgnoreProgrammerCode(const bool ignore) noexcept
        {
        m_spellcheck_ignore_programmer_code = ignore;
        }

    [[nodiscard]]
    bool SpellCheckIsAllowingColloquialisms() const noexcept
        {
        return m_allow_colloquialisms;
        }

    void SpellCheckAllowColloquialisms(const bool allow) noexcept
        {
        m_allow_colloquialisms = allow;
        }

    [[nodiscard]]
    bool SpellCheckIsIgnoringSocialMediaTags() const noexcept
        {
        return m_spellcheck_ignore_social_media_tags;
        }

    void SpellCheckIgnoreSocialMediaTags(const bool ignore) noexcept
        {
        m_spellcheck_ignore_social_media_tags = ignore;
        }

    // readability get functions
    [[nodiscard]]
    bool IsDolchSelected() const noexcept
        {
        return m_includeDolchSightWords;
        }

    [[nodiscard]]
    TestCollectionType& GetReadabilityTests() noexcept
        {
        return m_readabilityTests;
        }

    [[nodiscard]]
    std::vector<wxString>& GetIncludedCustomTests() noexcept
        {
        return m_includedCustomTests;
        }

    // readability set functions
    void SetDolch(const bool value) noexcept { m_includeDolchSightWords = value; }

    void SetReadabilityTests(const TestCollectionType& tests) { m_readabilityTests = tests; }

    // Test Source
    [[nodiscard]]
    TextSource GetTextSource() const noexcept
        {
        return m_textSource;
        }

    void SetTextSource(const TextSource value) noexcept { m_textSource = value; }

    // Test Recommendation
    [[nodiscard]]
    TestRecommendation GetTestRecommendation() const noexcept
        {
        return m_testRecommendation;
        }

    void SetTestRecommendation(const TestRecommendation value) noexcept
        {
        m_testRecommendation = value;
        }

    // Tests by Industry
    [[nodiscard]]
    readability::industry_classification GetTestByIndustry() const noexcept
        {
        return m_testsByIndustry;
        }

    void SetTestByIndustry(const readability::industry_classification value) noexcept
        {
        m_testsByIndustry = value;
        }

    // Tests by DocumentType
    [[nodiscard]]
    readability::document_classification GetTestByDocumentType() const noexcept
        {
        return m_testsByDocumentType;
        }

    void SetTestByDocumentType(const readability::document_classification value) noexcept
        {
        m_testsByDocumentType = value;
        }

    // tests by bundle
    [[nodiscard]]
    const wxString& GetSelectedTestBundle() const noexcept
        {
        return m_selectedTestBundle;
        }

    void SetSelectedTestBundle(const wxString& bundleName) { m_selectedTestBundle = bundleName; }

    // document storage/linking information
    [[nodiscard]]
    TextStorage GetDocumentStorageMethod() const noexcept
        {
        return m_documentStorageMethod;
        }

    void SetDocumentStorageMethod(const TextStorage method) noexcept
        {
        m_documentStorageMethod = method;
        }

    // how documents are grouped
    [[nodiscard]]
    int GetBatchGroupMethod() const noexcept
        {
        return m_batchGroupDefault;
        }

    void SetBatchGroupMethod(int group) { m_batchGroupDefault = group; }

    // Window information
    [[nodiscard]]
    bool IsAppWindowMaximized() const noexcept
        {
        return m_appWindowMaximized;
        }

    void SetAppWindowMaximized(const bool maximized) noexcept { m_appWindowMaximized = maximized; }

    [[nodiscard]]
    int GetAppWindowWidth() const noexcept
        {
        return m_appWindowWidth;
        }

    [[nodiscard]]
    int GetAppWindowHeight() const noexcept
        {
        return m_appWindowHeight;
        }

    void SetAppWindowWidth(const int width) noexcept { m_appWindowWidth = width; }

    void SetAppWindowHeight(const int height) noexcept { m_appWindowHeight = height; }

    void SetUiLanguage(const UiLanguage lang) noexcept { m_uiLanguage = lang; }

    [[nodiscard]]
    UiLanguage GetUiLanguage() const noexcept
        {
        return m_uiLanguage;
        }

    // last opened file locations
    [[nodiscard]]
    wxString GetImagePath() const
        {
        return m_imagePath;
        }

    void SetImagePath(const wxString& path) { m_imagePath = path; }

    [[nodiscard]]
    wxString GetDownloadsPath() const
        {
        return m_downloadsPath;
        }

    void SetDownloadsPath(const wxString& path) { m_downloadsPath = path; }

    [[nodiscard]]
    wxString GetProjectPath() const
        {
        return m_projectPath;
        }

    void SetProjectPath(const wxString& path) { m_projectPath = path; }

    [[nodiscard]]
    wxString GetWordListPath() const
        {
        return m_wordlistPath;
        }

    void SetWordListPath(const wxString& path) { m_wordlistPath = path; }

    // internet
    [[nodiscard]]
    const wxString& GetUserAgent() const
        {
        return m_userAgent;
        }

    void SetUserAgent(const wxString& path) { m_userAgent = path; }

    /** @brief Disable SSL certificate verification.
        @details This can be used to connect to self-signed servers or
            other invalid SSL connections.\n
            Disabling verification makes the communication insecure.
        @param disable @c true to disable SSL certificate verification.*/
    void DisablePeerVerify(const bool disable) noexcept { m_disablePeerVerify = disable; }

    /// @returns Returns @c true if peer verification has been disabled.
    [[nodiscard]]
    bool IsPeerVerifyDisabled() const noexcept
        {
        return m_disablePeerVerify;
        }

    /** @brief If @c true, will read any cookies being set via JavaScript when
            reading or downloading a page. If any cookies are found,
            then the page will be reconnected to with these cookies being sent.
        @details This is useful when connecting to pages that won't load as expected unless
            cookies being set via JavaScript are sent back to the server.
        @warning This will result in an additional call to read each webpage and is
            only recommended if JavaScript is being used to block headless connections.
        @param useCookies @c true to reconnect with cookies in the JavaScript.*/
    void UseJavaScriptCookies(const bool useCookies) { m_useJsCookies = useCookies; }

    /// @returns Whether cookies should be extracted from JS code and sent back to the
    ///     server when connected to them.
    [[nodiscard]]
    bool IsUsingJavaScriptCookies() const noexcept
        {
        return m_useJsCookies;
        }

    /** @brief If using JavaScript cookies, store and send all encountered cookies for
            all sites being crawled during a given session.
        @param persistCookies @c true to reuse cookies.*/
    void PersistJavaScriptCookies(const bool persistCookies)
        {
        m_persistJsCookies = persistCookies;
        }

    /// @returns If using JavaScript cookies, store and send all encountered cookies for
    ///     all sites being crawled during a given session.
    [[nodiscard]]
    bool IsPersistingJavaScriptCookies() const noexcept
        {
        return m_persistJsCookies;
        }

    // graph information
    //------------------------------
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

    void DisplayBarChartLabels(const bool display = true) noexcept { m_barDisplayLabels = display; }

    [[nodiscard]]
    bool IsDisplayingBarChartLabels() const noexcept
        {
        return m_barDisplayLabels;
        }

    // whether gradient is used for graph backgrounds
    void SetGraphBackGroundLinearGradient(const bool useGradient) noexcept
        {
        m_useGraphBackGroundColorLinearGradient = useGradient;
        }

    [[nodiscard]]
    bool GetGraphBackGroundLinearGradient() const noexcept
        {
        return m_useGraphBackGroundColorLinearGradient;
        }

    // whether drop shadows should be shown
    void DisplayDropShadows(const bool display) noexcept { m_displayDropShadows = display; }

    [[nodiscard]]
    bool IsDisplayingDropShadows() const noexcept
        {
        return m_displayDropShadows;
        }

    // whether to draw attention to the complex word groups in syllable graphs
    void ShowcaseKeyItems(const bool display) noexcept { m_showcaseKeyItems = display; }

    [[nodiscard]]
    bool IsShowcasingKeyItems() const noexcept
        {
        return m_showcaseKeyItems;
        }

    // graph background image
    void SetPlotBackGroundImagePath(const wxString& filePath)
        {
        m_plotBackGroundImagePath = filePath;
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

    void SetGraphColorScheme(const wxString& colorScheme) { m_graphColorSchemeName = colorScheme; }

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

    /// water mark functions
    void SetWatermark(const Wisteria::Canvas::Watermark& watermark) { m_watermark = watermark; }

    [[nodiscard]]
    Wisteria::Canvas::Watermark GetWatermark() const
        {
        return m_watermark;
        }

    void SetWatermarkLogo(const wxString& watermarkImg) { m_watermarkImg = watermarkImg; }

    [[nodiscard]]
    wxString GetWatermarkLogo() const
        {
        return m_watermarkImg;
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

    /// Bar chart options
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

    void SetGraphBarEffect(const Wisteria::BoxEffect effect) noexcept { m_graphBarEffect = effect; }

    [[nodiscard]]
    Wisteria::BoxEffect GetGraphBarEffect() const noexcept
        {
        return m_graphBarEffect;
        }

    void SetStippleImagePath(const wxString& path) { m_stippleImagePath = path; }

    [[nodiscard]]
    wxString GetStippleImagePath() const
        {
        return m_stippleImagePath;
        }

    void SetGraphCommonImagePath(const wxString& path) { m_commonImagePath = path; }

    [[nodiscard]]
    wxString GetGraphCommonImagePath() const
        {
        return m_commonImagePath;
        }

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

    // Box Plot options
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

    void SetPlotBackGroundImageEffect(const Wisteria::ImageEffect effect) noexcept
        {
        m_plotBackgroundImageEffect = effect;
        }

    [[nodiscard]]
    Wisteria::ImageFit GetPlotBackGroundImageFit() const noexcept
        {
        return m_plotBackgroundImageFit;
        }

    void SetPlotBackGroundImageFit(const Wisteria::ImageFit fit) noexcept
        {
        m_plotBackgroundImageFit = fit;
        }

    // axis font colors
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

    // axis fonts
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

    // title fonts
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

    // invalid area color
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

    // variance method
    [[nodiscard]]
    VarianceMethod GetVarianceMethod() const noexcept
        {
        return m_varianceMethod;
        }

    void SetVarianceMethod(const VarianceMethod method) noexcept { m_varianceMethod = method; }

    // minimum doc size
    void SetMinDocWordCountForBatch(const size_t docSize) noexcept
        {
        m_minDocWordCountForBatch = docSize;
        }

    [[nodiscard]]
    size_t GetMinDocWordCountForBatch() const noexcept
        {
        return m_minDocWordCountForBatch;
        }

    // random sampling size
    void SetBatchRandomSamplingSize(const size_t size) noexcept
        {
        m_randomSampleSizeForBatch = size;
        }

    [[nodiscard]]
    size_t GetBatchRandomSamplingSize() const noexcept
        {
        return m_randomSampleSizeForBatch;
        }

    [[nodiscard]]
    bool IsRandomSampling() const noexcept
        {
        return m_randomSampling;
        }

    void EnableRandomSampling(const bool enable) noexcept { m_randomSampling = enable; }

    // how file paths are shown in batch projects
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

    // project test language
    void SetProjectLanguage(const readability::test_language lang) noexcept { m_language = lang; }

    [[nodiscard]]
    readability::test_language GetProjectLanguage() const noexcept
        {
        return m_language;
        }

    /// @returns The file path to the document being appended for analysis (optional)
    [[nodiscard]]
    wxString GetAppendedDocumentFilePath() const
        {
        return m_appendedDocumentFilePath;
        }

    /// Sets the file path to the document being appended for analysis (optional).
    /// @param path The file path to the document.
    void SetAppendedDocumentFilePath(const wxString& path) { m_appendedDocumentFilePath = path; }

    void SetReviewer(const wxString& reviewer) { m_reviewer = reviewer; }

    [[nodiscard]]
    wxString GetReviewer() const
        {
        return m_reviewer;
        }

    void UseRealTimeUpdate(const bool realTime) { m_realTimeUpdate = realTime; }

    [[nodiscard]]
    bool IsRealTimeUpdating() const noexcept
        {
        return m_realTimeUpdate;
        }

    // printer settings
    // page setup
    void SetPaperId(const wxPaperSize Id) noexcept { m_paperId = Id; }

    [[nodiscard]]
    wxPaperSize GetPaperId() const noexcept
        {
        return m_paperId;
        }

    void SetPaperOrientation(const wxPrintOrientation orient) noexcept
        {
        m_paperOrientation = orient;
        }

    [[nodiscard]]
    wxPrintOrientation GetPaperOrientation() const noexcept
        {
        return m_paperOrientation;
        }

    // printer header functions
    void SetLeftPrinterHeader(const wxString& header) { m_leftPrinterHeader = header; }

    [[nodiscard]]
    wxString GetLeftPrinterHeader() const
        {
        return m_leftPrinterHeader;
        }

    void SetCenterPrinterHeader(const wxString& header) { m_centerPrinterHeader = header; }

    [[nodiscard]]
    wxString GetCenterPrinterHeader() const
        {
        return m_centerPrinterHeader;
        }

    void SetRightPrinterHeader(const wxString& header) { m_rightPrinterHeader = header; }

    [[nodiscard]]
    wxString GetRightPrinterHeader() const
        {
        return m_rightPrinterHeader;
        }

    // printer footer functions
    void SetLeftPrinterFooter(const wxString& header) { m_leftPrinterFooter = header; }

    [[nodiscard]]
    wxString GetLeftPrinterFooter() const
        {
        return m_leftPrinterFooter;
        }

    void SetCenterPrinterFooter(const wxString& header) { m_centerPrinterFooter = header; }

    [[nodiscard]]
    wxString GetCenterPrinterFooter() const
        {
        return m_centerPrinterFooter;
        }

    void SetRightPrinterFooter(const wxString& header) { m_rightPrinterFooter = header; }

    [[nodiscard]]
    wxString GetRightPrinterFooter() const
        {
        return m_rightPrinterFooter;
        }

    // test options
    [[nodiscard]]
    SpecializedTestTextExclusion GetDaleChallTextExclusionMode() const noexcept
        {
        return m_dcTextExclusion;
        }

    void SetDaleChallTextExclusionMode(const SpecializedTestTextExclusion mode) noexcept
        {
        m_dcTextExclusion = mode;
        }

    void IncludeStockerCatholicSupplement(const bool includeSupplement) noexcept
        {
        m_includeStockerCatholicDCSupplement = includeSupplement;
        }

    [[nodiscard]]
    bool IsIncludingStockerCatholicSupplement() const noexcept
        {
        return m_includeStockerCatholicDCSupplement;
        }

    [[nodiscard]]
    readability::proper_noun_counting_method GetDaleChallProperNounCountingMethod() const noexcept
        {
        return m_dcProperNounCountingMethod;
        }

    void SetDaleChallProperNounCountingMethod(
        const readability::proper_noun_counting_method mode) noexcept
        {
        m_dcProperNounCountingMethod = mode;
        }

    [[nodiscard]]
    SpecializedTestTextExclusion GetHarrisJacobsonTextExclusionMode() const noexcept
        {
        return m_hjTextExclusion;
        }

    void SetHarrisJacobsonTextExclusionMode(const SpecializedTestTextExclusion mode) noexcept
        {
        m_hjTextExclusion = mode;
        }

    [[nodiscard]]
    bool IsFogUsingSentenceUnits() const noexcept
        {
        return m_fogUseSentenceUnits;
        }

    void FogUseSentenceUnits(const bool useUnits) noexcept { m_fogUseSentenceUnits = useUnits; }

    [[nodiscard]]
    FleschNumeralSyllabize GetFleschNumeralSyllabizeMethod() const noexcept
        {
        return m_fleschNumeralSyllabizeMethod;
        }

    void SetFleschNumeralSyllabizeMethod(const FleschNumeralSyllabize method) noexcept
        {
        m_fleschNumeralSyllabizeMethod = method;
        }

    [[nodiscard]]
    FleschKincaidNumeralSyllabize GetFleschKincaidNumeralSyllabizeMethod() const noexcept
        {
        return m_fleschKincaidNumeralSyllabizeMethod;
        }

    void SetFleschKincaidNumeralSyllabizeMethod(const FleschKincaidNumeralSyllabize method) noexcept
        {
        m_fleschKincaidNumeralSyllabizeMethod = method;
        }

    [[nodiscard]]
    bool IsIncludingScoreSummaryReport() const noexcept
        {
        return m_includeScoreSummaryReport;
        }

    void IncludeScoreSummaryReport(const bool include) noexcept
        {
        m_includeScoreSummaryReport = include;
        }

    [[nodiscard]]
    std::vector<wxColour>& GetCustomColors() noexcept
        {
        return m_customColors;
        }

    void CopyCustomColorsToColorData(wxColourData& colorData)
        {
        GetCustomColors().resize(MAX_CUSTOM_COLORS);
        for (size_t i = 0; i < MAX_CUSTOM_COLORS; ++i)
            {
            colorData.SetCustomColour(i, GetCustomColors().at(i));
            }
        }

    void CopyColorDataToCustomColors(const wxColourData& colorData)
        {
        GetCustomColors().clear();
        for (size_t i = 0; i < MAX_CUSTOM_COLORS; ++i)
            {
            GetCustomColors().push_back(colorData.GetCustomColour(i));
            }
        }

    [[nodiscard]]
    StatisticsInfo& GetStatisticsInfo() noexcept
        {
        return m_statsInfo;
        }

    [[nodiscard]]
    StatisticsReportInfo& GetStatisticsReportInfo() noexcept
        {
        return m_statsReportInfo;
        }

    [[nodiscard]]
    GrammarInfo& GetGrammarInfo() noexcept
        {
        return m_grammarInfo;
        }

    [[nodiscard]]
    WordsBreakdownInfo& GetWordsBreakdownInfo() noexcept
        {
        return m_wordsBreakdownInfo;
        }

    [[nodiscard]]
    SentencesBreakdownInfo& GetSentencesBreakdownInfo() noexcept
        {
        return m_sentencesBreakdownInfo;
        }

    void UpdateGraphOptions(Wisteria::Canvas* graphCanvas);

    /// @returns The catalog of labels to show on readability graphs
    ///     (mapped to various grade levels).
    [[nodiscard]]
    ReadabilityMessages& GetReadabilityMessageCatalog() noexcept
        {
        return m_readMessages;
        }

    /// @returns The file filter string for opening our supported document types.
    [[nodiscard]]
    static wxString GetDocumentFilter();

    [[nodiscard]]
    static wxString TiXmlNodeAttributeToString(const tinyxml2::XMLNode* node,
                                               const wxString& tagToRead,
                                               const wxString& fallbackValue = wxString{});

  private:
    void LoadThemeNode(tinyxml2::XMLElement* appearanceNode);
    void LoadInternetNode(tinyxml2::XMLElement* configRootNode);
    void LoadWarningsNode(tinyxml2::XMLElement* configRootNode);
    void LoadExportNode(tinyxml2::XMLElement* configRootNode);
    void LoadTestBundlesNode(tinyxml2::XMLElement* projectSettings);
    void LoadCustomTestsNode(tinyxml2::XMLElement* projectSettings);
    void LoadGraphsNode(tinyxml2::XMLElement* projectSettings);
    void LoadStatsNode(tinyxml2::XMLElement* projectSettings);
    void LoadDocAnalysisNode(tinyxml2::XMLElement* projectSettings);
    void LoadReadabilityTestsNode(tinyxml2::XMLElement* projectSettings);
    /// @returns The value from the specified attribute from @c node as a double, or NaN on failure.
    /// @note This assumes the double is written in US format (and no thousands separator).
    [[nodiscard]]
    static double TiXmlNodeToDouble(const tinyxml2::XMLNode* colorNode, const wxString& tagToRead);
    [[nodiscard]]
    static wxColour TiXmlNodeToColor(const tinyxml2::XMLNode* node,
                                     const wxColour& defaultColor = wxColour{ 0, 0, 0 });

    // embedded text editor
    wxFont m_editorFont;
    bool m_editorIndent{ false };
    bool m_editorSpaceAfterNewlines{ false };
    wxTextAttrAlignment m_editorTextAlignment{ wxTextAttrAlignment::wxTEXT_ALIGNMENT_JUSTIFIED };
    wxTextAttrLineSpacing m_editorLineSpacing{
        wxTextAttrLineSpacing::wxTEXT_ATTR_LINE_SPACING_NORMAL
    };

    bool m_logAppendDailyLog{ false };

    // Lua scripting - enables os, io, debug libraries (security risk)
    bool m_luaUnsafeMode{ false };
    bool m_showDeveloperTab{ true };
    bool m_showLogTab{ false };
    bool m_logAutoRefresh{ false };
    bool m_disableGpuAcceleration{ false };
    wxString m_reportTheme{ _DT(L"emerald-isles.css") };

    wxColour m_dolchConjunctionsColor{ wxColour{ 255, 255, 0 } };
    wxColour m_dolchPrepositionsColor{ wxColour{ 0, 245, 255 } };
    wxColour m_dolchPronounsColor{ wxColour{ 198, 226, 255 } };
    wxColour m_dolchAdverbsColor{ wxColour{ 0, 250, 154 } };
    wxColour m_dolchAdjectivesColor{ wxColour{ 221, 160, 221 } };
    wxColour m_dolchVerbsColor{ wxColour{ 254, 208, 112 } };
    wxColour m_dolchNounsColor{ wxColour{ 255, 182, 193 } };
    bool m_highlightDolchConjunctions{ true };
    bool m_highlightDolchPrepositions{ true };
    bool m_highlightDolchPronouns{ true };
    bool m_highlightDolchAdverbs{ true };
    bool m_highlightDolchAdjectives{ true };
    bool m_highlightDolchVerbs{ true };
    bool m_highlightDolchNouns{ false };

    StatisticsInfo m_statsInfo;
    StatisticsReportInfo m_statsReportInfo;
    GrammarInfo m_grammarInfo;
    WordsBreakdownInfo m_wordsBreakdownInfo;
    SentencesBreakdownInfo m_sentencesBreakdownInfo;

    wxString m_optionsFile;
    wxColour m_wordyPhraseHighlightColor{ wxColour{ 0, 255, 255 } };
    TextHighlight m_textHighlight{ TextHighlight::HighlightBackground };
    wxColour m_textHighlightColor{ wxColour{ 152, 251, 152 } };
    wxColour m_excludedTextHighlightColor{ wxColour{ 175, 175, 175 } };
    wxColour m_duplicateWordHighlightColor{ wxColour{ 255, 128, 128 } };
    wxColour m_fontColor{ wxColour{ 0, 0, 0 } };
    wxFont m_textViewFont;
    bool m_useStandardReportFont{ true };
    LongSentence m_longSentenceMethod{ LongSentence::LongerThanSpecifiedLength };
    int m_difficultSentenceLength{ 22 };
    NumeralSyllabize m_numeralSyllabicationMethod{ NumeralSyllabize::WholeWordIsOneSyllable };
    ParagraphParse m_paragraphsParsingMethod{
        ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs
    };
    bool m_ignoreBlankLinesForParagraphsParser{ false };
    bool m_ignoreIndentingForParagraphsParser{ false };
    bool m_sentenceStartMustBeUppercased{ false };
    bool m_aggressiveExclusion{ false };
    bool m_excludeTrailingCopyrightNoticeParagraphs{ true };
    bool m_excludeTrailingCitations{ true };
    bool m_excludeFileAddresses{ false };
    bool m_excludeNumerals{ false };
    bool m_excludeProperNouns{ false };
    bool m_includeExcludedPhraseFirstOccurrence{ false };
    wxString m_excludedPhrasesPath;
    wxString m_plainLanguageGuideListName;
    std::vector<std::pair<wchar_t, wchar_t>> m_exclusionBlockTags;
    InvalidSentence m_invalidSentenceMethod{ InvalidSentence::ExcludeFromAnalysis };
    size_t m_includeIncompleteSentencesIfLongerThan{ 15 };
    // grammar
    bool m_spellcheck_ignore_proper_nouns{ false };
    bool m_spellcheck_ignore_uppercased{ true };
    bool m_spellcheck_ignore_numerals{ true };
    bool m_spellcheck_ignore_file_addresses{ true };
    bool m_spellcheck_ignore_programmer_code{ false };
    bool m_allow_colloquialisms{ true };
    bool m_spellcheck_ignore_social_media_tags{ true };
    // readability tests
    bool m_includeDolchSightWords{ false };
    // stores the settings for which tests are selected in the wizard
    readability::readability_test_collection<
        readability::readability_project_test<Wisteria::Data::Dataset>>
        m_readabilityTests;
    std::vector<wxString> m_includedCustomTests;
    ReadabilityMessages m_readMessages;
    // Test Recommendation
    TestRecommendation m_testRecommendation{ TestRecommendation::BasedOnDocumentType };
    // Text Source
    TextSource m_textSource{ TextSource::FromFile };
    // Tests by Industry
    readability::industry_classification m_testsByIndustry{
        readability::industry_classification::adult_publishing_industry
    };
    // Tests By Document Type
    readability::document_classification m_testsByDocumentType{
        readability::document_classification::adult_literature_document
    };
    // tests by test bundle (this will be the selected bundle name)
    wxString m_selectedTestBundle;
    int m_batchGroupDefault{ 2 };
    // document storage/linking information
    TextStorage m_documentStorageMethod{ TextStorage::NoEmbedText };
    // Window information
    bool m_appWindowMaximized{ true };
    int m_appWindowWidth{ 800 };
    int m_appWindowHeight{ 700 };
    UiLanguage m_uiLanguage{ UiLanguage::Default };
    // last opened file locations
    wxString m_imagePath;
    wxString m_projectPath;
    wxString m_wordlistPath;
    wxString m_downloadsPath;
    // internet features
    // Note that we call this a "WebLion". Using words like "harvester," "crawler,"
    // "scraper," and even "browser" will result in a forbidden response from some sites,
    // so avoid using those words.
    wxString m_userAgent{ _DT(L"Mozilla/5.0 (") + wxGetOsDescription() +
                          _DT(L") WebKit/12.0 WebLion") };
    bool m_disablePeerVerify{ false };
    bool m_useJsCookies{ false };
    bool m_persistJsCookies{ false };
    // graph information
    bool m_boxPlotShowAllPoints{ false };
    bool m_boxDisplayLabels{ false };
    bool m_boxConnectMiddlePoints{ true };
    bool m_useGraphBackGroundColorLinearGradient{ false };
    bool m_displayDropShadows{ false };
    bool m_showcaseKeyItems{ false };
    wxString m_graphColorSchemeName{ _DT(L"campfire") };
    wxString m_plotBackGroundImagePath;
    Wisteria::Canvas::Watermark m_watermark;
    wxString m_watermarkImg;
    wxColour m_graphBackGroundColor{ wxColour{ 255, 255, 255 } };
    wxColour m_plotBackGroundColor{ wxColour{ 255, 255, 255 } };
    uint8_t m_plotBackGroundImageOpacity{ wxALPHA_OPAQUE };
    uint8_t m_plotBackGroundColorOpacity{ wxALPHA_TRANSPARENT };
    wxColour m_xAxisFontColor{ wxColour{ 0, 0, 0 } };
    wxFont m_xAxisFont;
    wxColour m_yAxisFontColor{ wxColour{ 0, 0, 0 } };
    wxFont m_yAxisFont;
    wxColour m_topTitleFontColor{ wxColour{ 0, 0, 0 } };
    wxFont m_topTitleFont;
    wxColour m_bottomTitleFontColor{ wxColour{ 0, 0, 0 } };
    wxFont m_bottomTitleFont;
    wxColour m_leftTitleFontColor{ wxColour{ 0, 0, 0 } };
    wxFont m_leftTitleFont;
    wxColour m_rightTitleFontColor{ wxColour{ 0, 0, 0 } };
    wxFont m_rightTitleFont;
    // a "rainy" look for the readability graphs
    wxColour m_graphInvalidAreaColor{ wxColour{ 193, 205, 193 } }; // honeydew
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
    Wisteria::BoxEffect m_histogramBarEffect{ Wisteria::BoxEffect::Solid };
    // Bar chart options
    bool m_barDisplayLabels{ true };
    wxColour m_barChartBarColor{ wxColour{ 107, 183, 196 } }; // rain color
    Wisteria::Orientation m_barChartOrientation{ Wisteria::Orientation::Horizontal };
    uint8_t m_graphBarOpacity{ wxALPHA_OPAQUE };
    Wisteria::BoxEffect m_graphBarEffect{ Wisteria::BoxEffect::Solid };
    wxString m_stippleImagePath;
    wxString m_commonImagePath;
    // Note that this is not stored as an icon enum because there are many shapes
    // that are not relevant for stippling. Instead, we store as a string and convert
    // that to an enum value.
    wxString m_stippleShape{ DONTTRANSLATE(L"book") };
    wxColour m_stippleColor{ wxColour{ L"#6082B6" } };
    wxColour m_graphBoxColor{ wxColour{ 0, 128, 64 } };
    uint8_t m_graphBoxOpacity{ wxALPHA_OPAQUE };
    Wisteria::BoxEffect m_graphBoxEffect{ Wisteria::BoxEffect::Solid };
    // background image options
    Wisteria::ImageEffect m_plotBackgroundImageEffect{ Wisteria::ImageEffect::NoEffect };
    Wisteria::ImageFit m_plotBackgroundImageFit{ Wisteria::ImageFit::Shrink };
    // project settings
    VarianceMethod m_varianceMethod{ VarianceMethod::PopulationVariance };
    size_t m_minDocWordCountForBatch{ 50 };
    size_t m_randomSampleSizeForBatch{ 15 };
    bool m_randomSampling{ false };
    Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode m_filePathTruncationMode{
        Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames
    };
    wxString m_reviewer;
    wxString m_appendedDocumentFilePath;
    readability::test_language m_language{ readability::test_language::english_test };
    bool m_realTimeUpdate{ false };
    // printing settings
    // page setup
    wxPaperSize m_paperId{ wxPAPER_LETTER };
    wxPrintOrientation m_paperOrientation{ wxLANDSCAPE };
    // headers
    wxString m_leftPrinterHeader;
    wxString m_centerPrinterHeader;
    wxString m_rightPrinterHeader;
    // footers
    wxString m_leftPrinterFooter;
    wxString m_centerPrinterFooter;
    wxString m_rightPrinterFooter;
    // readability scores options
    bool m_includeScoreSummaryReport{ true };
    // test options
    SpecializedTestTextExclusion m_dcTextExclusion{
        SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings
    };
    SpecializedTestTextExclusion m_hjTextExclusion{
        SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings
    };
    readability::proper_noun_counting_method m_dcProperNounCountingMethod{
        readability::proper_noun_counting_method::
            only_count_first_instance_of_proper_noun_as_unfamiliar
    };
    bool m_includeStockerCatholicDCSupplement{ false };
    bool m_fogUseSentenceUnits{ true };
    FleschNumeralSyllabize m_fleschNumeralSyllabizeMethod{
        FleschNumeralSyllabize::NumeralIsOneSyllable
    };
    FleschKincaidNumeralSyllabize m_fleschKincaidNumeralSyllabizeMethod{
        FleschKincaidNumeralSyllabize::FleschKincaidNumeralSoundOutEachDigit
    };
    Wisteria::Graphs::RaygorStyle m_raygorStyle{ Wisteria::Graphs::RaygorStyle::BaldwinKaufman };
    // custom colors
    std::vector<wxColour> m_customColors;
    // images used for blank graphs
    wxBitmapBundle m_graphBackgroundImage;
    wxBitmapBundle m_waterMarkImage;
    wxBitmapBundle m_graphStippleImage;
    std::shared_ptr<Wisteria::Images::Schemes::ImageScheme> m_graphImageScheme{
        std::make_shared<Wisteria::Images::Schemes::ImageScheme>(
            std::vector<wxBitmapBundle>{ wxBitmapBundle{} })
    };

    constexpr static int MAX_CUSTOM_COLORS{ 16 };

  public:
    inline constexpr static std::string_view XML_EDITOR{ _DT("editor") };
    inline constexpr static std::string_view XML_EDITOR_FONT{ _DT("editor-font") };
    inline constexpr static std::string_view XML_EDITOR_INDENT{ _DT("editor-indent") };
    inline constexpr static std::string_view XML_EDITOR_SPACE_AFTER_PARAGRAPH{ _DT(
        "editor-space-after-paragraph") };
    inline constexpr static std::string_view XML_EDITOR_TEXT_ALIGNMENT{ _DT(
        "editor-text-alignment") };
    inline constexpr static std::string_view XML_EDITOR_LINE_SPACING{ _DT("editor-line-spacing") };
    inline constexpr static std::string_view XML_LOG_SETTINGS{ _DT("log-setting") };
    inline constexpr static std::string_view XML_LOG_VERBOSE{ _DT("log-verbose") };
    inline constexpr static std::string_view XML_LOG_APPEND_DAILY{ _DT("log-append-daily") };
    inline constexpr static std::string_view XML_PROJECT_HEADER{ _DT(
        "oleander-readability-studio-project") };
    inline constexpr static std::string_view XML_DOCUMENT{ _DT("document") };
    inline constexpr static std::string_view XML_TEXT_SOURCE{ _DT("text-source") };
    inline constexpr static std::string_view XML_DOCUMENT_PATH{ _DT("file-path") };
    inline constexpr static std::string_view XML_DESCRIPTION{ _DT("description") };
    inline constexpr static std::string_view XML_DOCUMENT_ANALYSIS_LOGIC{ _DT(
        "document-analysis-logic") };
    inline constexpr static std::string_view XML_EXPORT_FOLDER_PATH{ _DT("export-folder-path") };
    inline constexpr static std::string_view XML_EXPORT_FILE_PATH{ _DT("export-file-path") };
    inline constexpr static std::string_view XML_INCLUDE{ _DT("include") };
    inline constexpr static std::string_view XML_CONFIGURATIONS{ _DT("configurations") };
    inline constexpr static std::string_view XML_VERSION{ _DT("version") };
    inline constexpr static std::wstring_view XML_VERSION_W{ _DT(L"version") };
    inline constexpr static std::string_view XML_HIGHLIGHT_METHOD{ _DT("highlight-method") };
    inline constexpr static std::string_view XML_HIGHLIGHTCOLOR{ _DT("highlight-color") };
    inline constexpr static std::string_view XML_EXCLUDED_HIGHLIGHTCOLOR{ _DT(
        "excluded-highlight-color") };
    inline constexpr static std::string_view XML_DUP_WORD_HIGHLIGHTCOLOR{ _DT(
        "duplicate-word-highlight-color") };
    inline constexpr static std::string_view XML_WORDY_PHRASE_HIGHLIGHTCOLOR{ _DT(
        "wordy-phrase-highlight-color") };
    inline constexpr static std::string_view XML_LONG_SENTENCES{ _DT("long-sentences") };
    inline constexpr static std::string_view XML_LONG_SENTENCE_METHOD{ _DT(
        "long-sentence-method") };
    inline constexpr static std::string_view XML_LONG_SENTENCE_LENGTH{ _DT(
        "long-sentence-length") };
    inline constexpr static std::string_view XML_NUMERAL_SYLLABICATION_METHOD{ _DT(
        "numeral-syllabication-method") };
    inline constexpr static std::string_view XML_PARAGRAPH_PARSING_METHOD{ _DT(
        "paragraph-parsing-method") };
    inline constexpr static std::string_view XML_IGNORE_BLANK_LINES_FOR_PARAGRAPH_PARSING{ _DT(
        "ignore-blank-lines-for-paragraphs") };
    inline constexpr static std::string_view XML_IGNORE_INDENTING_FOR_PARAGRAPH_PARSING{ _DT(
        "ignore-indenting-for-paragraphs") };
    inline constexpr static std::string_view XML_SENTENCES_MUST_START_CAPITALIZED{ _DT(
        "sentences-must-start-capitalized") };
    inline constexpr static std::string_view XML_AGGRESSIVE_EXCLUSION{ _DT(
        "aggressively-deduce-lists") };
    inline constexpr static std::string_view XML_IGNORE_COPYRIGHT_NOTICES{ _DT(
        "ignore-trailing-copyright-notices") };
    inline constexpr static std::string_view XML_IGNORE_CITATIONS{ _DT(
        "ignore-trailing-citations") };
    inline constexpr static std::string_view XML_IGNORE_FILE_ADDRESSES{ _DT(
        "ignore-file-addresses") };
    inline constexpr static std::string_view XML_IGNORE_NUMERALS{ _DT("ignore-numerals") };
    inline constexpr static std::string_view XML_IGNORE_PROPER_NOUNS{ _DT("ignore-proper-nouns") };
    inline constexpr static std::string_view XML_LUA_UNSAFE_MODE{ _DT("lua-unsafe-mode") };
    inline constexpr static std::string_view XML_SHOW_DEVELOPER_TAB{ _DT("show-developer-tab") };
    inline constexpr static std::string_view XML_SHOW_LOG_TAB{ _DT("show-log-tab") };
    inline constexpr static std::string_view XML_LOG_AUTO_REFRESH{ _DT("log-auto-refresh") };
    inline constexpr static std::string_view XML_DISABLE_GPU_ACCELERATION{ _DT(
        "disable-gpu-acceleration") };
    inline constexpr static std::string_view XML_REPORT_THEME{ _DT("report-theme") };
    inline constexpr static std::string_view XML_EXCLUDED_PHRASES_PATH{ _DT(
        "excluded-phrases-filepath") };
    inline constexpr static std::string_view XML_PLAIN_LANGUAGE_GUIDE_LIST{ _DT(
        "plain-language-guide-list") };
    inline constexpr static std::string_view XML_EXCLUDED_PHRASES_INCLUDE_FIRST_OCCURRENCE{ _DT(
        "excluded-phrases-include-first-occurrence") };
    inline constexpr static std::string_view XML_EXCLUDE_BLOCK_TAGS{ _DT("exclude-block-tags") };
    inline constexpr static std::wstring_view XML_EXCLUDE_BLOCK_TAGS_W{ _DT(
        L"exclude-block-tags") };
    inline constexpr static std::string_view XML_EXCLUDE_BLOCK_TAG{ _DT("exclude-block-tag") };
    inline constexpr static std::wstring_view XML_EXCLUDE_BLOCK_TAG_W{ _DT(L"exclude-block-tag") };
    inline constexpr static std::string_view XML_INVALID_SENTENCE_METHOD{ _DT(
        "invalid-sentence-method") };
    inline constexpr static std::string_view XML_METHOD{ _DT("method") };
    inline constexpr static std::string_view XML_VALUE{ _DT("value") };
    inline constexpr static std::string_view XML_DISPLAY{ _DT("display") };
    inline constexpr static std::string_view XML_CONFIG_HEADER{ _DT(
        "oleander-readability-studio-configuration") };
    inline constexpr static std::string_view XML_WIZARD_PAGES_SETTINGS{ _DT(
        "wizard-page-defaults") };
    inline constexpr static std::string_view XML_WIZARD_BATCH_GROUP{ _DT("batch-group-method") };
    inline constexpr static std::string_view XML_PROJECT_LANGUAGE{ _DT("project-language") };
    // test settings
    inline constexpr static std::string_view XML_READABILITY_TEST_GRADE_SCALE_DISPLAY{ _DT(
        "readability-test-grade-scale-display") };
    inline constexpr static std::string_view XML_READABILITY_TEST_GRADE_SCALE_LONG_FORMAT{ _DT(
        "readability-test-grade-scale-long-format") };
    inline constexpr static std::string_view XML_NEW_DALE_CHALL_OPTIONS{ _DT(
        "dale-chall-options") };
    inline constexpr static std::wstring_view XML_NEW_DALE_CHALL_OPTIONS_W{ _DT(
        L"dale-chall-options") };
    inline constexpr static std::string_view XML_STOCKER_LIST{ _DT(
        "include-stocker-catholic-supplement") };
    inline constexpr static std::string_view XML_HARRIS_JACOBSON_OPTIONS{ _DT(
        "harris-jacobson-options") };
    inline constexpr static std::wstring_view XML_HARRIS_JACOBSON_OPTIONS_W{ _DT(
        L"harris-jacobson-options") };
    inline constexpr static std::string_view XML_GUNNING_FOG_OPTIONS{ _DT("gunning-fog-options") };
    inline constexpr static std::wstring_view XML_GUNNING_FOG_OPTIONS_W{ _DT(
        L"gunning-fog-options") };
    inline constexpr static std::string_view XML_TEXT_EXCLUSION{ _DT("text-exclusion-mode") };
    inline constexpr static std::string_view XML_INCLUDE_INCOMPLETE_SENTENCES_LONGER_THAN{ _DT(
        "include-incomplete-sentences-longer-than") };
    inline constexpr static std::string_view XML_USE_SENTENCE_UNITS{ _DT("use-sentence-units") };
    inline constexpr static std::string_view XML_USE_HIGH_PRECISION{ _DT("use-precision") };
    inline constexpr static std::string_view XML_PROPER_NOUN_COUNTING_METHOD{ _DT(
        "proper-noun-counting-method") };
    inline constexpr static std::string_view XML_FLESCH_OPTIONS{ _DT("flesch-options") };
    inline constexpr static std::wstring_view XML_FLESCH_OPTIONS_W{ _DT(L"flesch-options") };
    inline constexpr static std::string_view XML_FLESCH_KINCAID_OPTIONS{ _DT(
        "flesch-kincaid-options") };
    inline constexpr static std::wstring_view XML_FLESCH_KINCAID_OPTIONS_W{ _DT(
        L"flesch-kincaid-options") };
    inline constexpr static std::string_view XML_RAYGOR_STYLE{ _DT("raygor-style") };
    // custom test settings
    inline constexpr static std::string_view XML_TEST_BUNDLES{ _DT("test-bundles") };
    inline constexpr static std::string_view XML_TEST_BUNDLE{ _DT("test-bundle") };
    inline constexpr static std::string_view XML_TEST_BUNDLE_NAME{ _DT("test-bundle-name") };
    inline constexpr static std::string_view XML_TEST_BUNDLE_DESCRIPTION{ _DT(
        "test-bundle-description") };
    inline constexpr static std::string_view XML_CUSTOM_TESTS{ _DT("custom-tests") };
    inline constexpr static std::string_view XML_CUSTOM_TEST{ _DT("custom-test") };
    inline constexpr static std::string_view XML_CUSTOM_FAMILIAR_WORD_TEST{ _DT(
        "custom-familiar-word-test") };
    inline constexpr static std::string_view XML_BUNDLE_STATISTICS{ _DT("bundle-statistics") };
    inline constexpr static std::string_view XML_BUNDLE_STATISTIC{ _DT("bundle-statistic") };
    inline constexpr static std::string_view XML_TEST_NAMES{ _DT("test-names") };
    inline constexpr static std::string_view XML_TEST_NAME{ _DT("test-name") };
    inline constexpr static std::string_view XML_TEST_TYPE{ _DT("test-type") };
    inline constexpr static std::string_view XML_FAMILIAR_WORD_FILE_PATH{ _DT(
        "familiar-word-file-path") };
    inline constexpr static std::string_view XML_TEST_FORMULA_TYPE{ _DT("test-formula-type") };
    inline constexpr static std::string_view XML_TEST_FORMULA{ _DT("test-formula") };
    inline constexpr static std::string_view XML_STEMMING_TYPE{ _DT("stemming-type") };
    inline constexpr static std::string_view XML_INCLUDE_CUSTOM_WORD_LIST{ _DT(
        "include-custom-word-list") };
    inline constexpr static std::string_view XML_INCLUDE_DC_LIST{ _DT("include-dale-chall-list") };
    inline constexpr static std::string_view XML_INCLUDE_SPACHE_LIST{ _DT("include-spache-list") };
    inline constexpr static std::string_view XML_INCLUDE_HARRIS_JACOBSON_LIST{ _DT(
        "include-harris-jacobson-list") };
    inline constexpr static std::string_view XML_INCLUDE_STOCKER_LIST{ _DT(
        "include-stocker-list") };
    inline constexpr static std::string_view XML_FAMILIAR_WORDS_ALL_LISTS{ _DT(
        "familiar-words-all-lists") };
    inline constexpr static std::string_view XML_INCLUDE_PROPER_NOUNS{ _DT(
        "include-proper-nouns") };
    inline constexpr static std::string_view XML_INCLUDE_NUMERIC{ _DT("include-numeric") };
    // graph settings
    inline constexpr static std::string_view XML_GRAPH_SETTINGS{ _DT("graph-settings") };
    inline constexpr static std::string_view XML_GRAPH_COLOR_SCHEME{ _DT("graph-color-scheme") };
    inline constexpr static std::string_view XML_GRAPH_BACKGROUND_COLOR{ _DT(
        "graph-background-color") };
    inline constexpr static std::string_view XML_GRAPH_PLOT_BACKGROUND_COLOR{ _DT(
        "graph-plot-background-color") };
    inline constexpr static std::string_view XML_GRAPH_PLOT_BACKGROUND_IMAGE_PATH{ _DT(
        "graph-background-image") };
    inline constexpr static std::string_view XML_GRAPH_PLOT_BACKGROUND_IMAGE_OPACITY{ _DT(
        "graph-background-opacity") };
    inline constexpr static std::string_view XML_GRAPH_PLOT_BACKGROUND_IMAGE_EFFECT{ _DT(
        "graph-background-image-effect") };
    inline constexpr static std::string_view XML_GRAPH_PLOT_BACKGROUND_IMAGE_FIT{ _DT(
        "graph-background-image-fit") };
    inline constexpr static std::string_view XML_GRAPH_PLOT_BACKGROUND_COLOR_OPACITY{ _DT(
        "graph-plot-background-color-opacity") };
    inline constexpr static std::string_view XML_GRAPH_BACKGROUND_LINEAR_GRADIENT{ _DT(
        "graph-background-linear-gradient") };
    inline constexpr static std::string_view XML_GRAPH_WATERMARK{ _DT("watermark") };
    inline constexpr static std::string_view XML_GRAPH_WATERMARK_LOGO_IMAGE_PATH{ _DT(
        "watermark-logo") };
    inline constexpr static std::string_view XML_GRAPH_COMMON_IMAGE_PATH{ _DT("common-image") };
    inline constexpr static std::string_view XML_DISPLAY_DROP_SHADOW{ _DT("display-drop-shadow") };
    inline constexpr static std::string_view XML_SHOWCASE_KEY_ITEMS{ _DT("showcase-key-items") };
    inline constexpr static std::string_view XML_AXIS_SETTINGS{ _DT("axis-settings") };
    inline constexpr static std::string_view XML_FRY_RAYGOR_SETTINGS{ _DT("fry-raygor-settings") };
    inline constexpr static std::string_view XML_INVALID_AREA_COLOR{ _DT("invalid-area-color-1") };
    inline constexpr static std::string_view XML_FLESCH_CHART_SETTINGS{ _DT(
        "flesch-chart-settings") };
    inline constexpr static std::string_view XML_INCLUDE_CONNECTION_LINE{ _DT(
        "include-connection-line") };
    inline constexpr static std::string_view XML_FLESCH_RULER_DOC_GROUPS{ _DT(
        "flesch-ruler-doc-groups") };
    inline constexpr static std::string_view XML_LIX_SETTINGS{ _DT("lix-settings") };
    inline constexpr static std::string_view XML_USE_ENGLISH_LABELS{ _DT("use-english-labels") };
    inline constexpr static std::string_view XML_X_AXIS{ _DT("x-axis") };
    inline constexpr static std::string_view XML_Y_AXIS{ _DT("y-axis") };
    inline constexpr static std::string_view XML_FONT_COLOR{ _DT("font-color") };
    inline constexpr static std::string_view XML_FONT{ _DT("font") };
    inline constexpr static std::string_view XML_TITLE_SETTINGS{ _DT("title-settings") };
    inline constexpr static std::string_view XML_TOP_TITLE{ _DT("top-title") };
    inline constexpr static std::string_view XML_BOTTOM_TITLE{ _DT("bottom-title") };
    inline constexpr static std::string_view XML_LEFT_TITLE{ _DT("left-title") };
    inline constexpr static std::string_view XML_RIGHT_TITLE{ _DT("right-title") };
    inline constexpr static std::string_view XML_HISTOGRAM_SETTINGS{ _DT("histogram-settings") };
    inline constexpr static std::string_view XML_GRAPH_BINNING_METHOD{ _DT("binning-method") };
    inline constexpr static std::string_view XML_GRAPH_ROUNDING_METHOD{ _DT("rounding-method") };
    inline constexpr static std::string_view XML_GRAPH_INTERVAL_DISPLAY{ _DT("interval-display") };
    inline constexpr static std::string_view XML_GRAPH_BINNING_LABEL_DISPLAY{ _DT(
        "bin-label-display") };
    inline constexpr static std::string_view XML_BAR_CHART_SETTINGS{ _DT("bar-chart-settings") };
    inline constexpr static std::string_view XML_GRAPH_OPACITY{ _DT("opacity") };
    inline constexpr static std::string_view XML_GRAPH_COLOR{ _DT("color") };
    inline constexpr static std::string_view XML_BAR_ORIENTATION{ _DT("bar-orientation") };
    inline constexpr static std::string_view XML_BAR_EFFECT{ _DT("bar-effect") };
    inline constexpr static std::string_view XML_BAR_DISPLAY_LABELS{ _DT("bar-display-labels") };
    inline constexpr static std::string_view XML_GRAPH_STIPPLE_PATH{ _DT("stipple-image-path") };
    inline constexpr static std::string_view XML_GRAPH_STIPPLE_SHAPE{ _DT("stipple-shape") };
    inline constexpr static std::string_view XML_GRAPH_STIPPLE_COLOR{ _DT("stipple-color") };
    inline constexpr static std::string_view XML_BOX_PLOT_SETTINGS{ _DT("box-plot-settings") };
    inline constexpr static std::string_view XML_BOX_EFFECT{ _DT("box-effect") };
    inline constexpr static std::string_view XML_BOX_DISPLAY_LABELS{ _DT("box-display-labels") };
    inline constexpr static std::string_view XML_BOX_CONNECT_MIDDLE_POINTS{ _DT(
        "box-connect-middle-points") };
    inline constexpr static std::string_view XML_BOX_PLOT_SHOW_ALL_POINTS{ _DT(
        "box-plot-show-all-points") };
    // printer setting tags
    inline constexpr static std::string_view XML_PRINTER_SETTINGS{ _DT("printer-settings") };
    inline constexpr static std::string_view XML_PRINTER_ID{ _DT("paper-id") };
    inline constexpr static std::string_view XML_PRINTER_ORIENTATION{ _DT("paper-orientation") };
    inline constexpr static std::string_view XML_PRINTER_LEFT_HEADER{ _DT("printer-left-header") };
    inline constexpr static std::string_view XML_PRINTER_CENTER_HEADER{ _DT(
        "printer-center-header") };
    inline constexpr static std::string_view XML_PRINTER_RIGHT_HEADER{ _DT(
        "printer-right-header") };
    inline constexpr static std::string_view XML_PRINTER_LEFT_FOOTER{ _DT("printer-left-footer") };
    inline constexpr static std::string_view XML_PRINTER_CENTER_FOOTER{ _DT(
        "printer-center-footer") };
    inline constexpr static std::string_view XML_PRINTER_RIGHT_FOOTER{ _DT(
        "printer-right-footer") };
    // stats section
    inline constexpr static std::string_view XML_STATISTICS_SECTION{ _DT("statistics") };
    inline constexpr static std::wstring_view XML_STATISTICS_SECTION_W{ _DT(L"statistics") };
    inline constexpr static std::string_view XML_VARIANCE_METHOD{ _DT("variance-calculation") };
    // tests section
    inline constexpr static std::string_view XML_PROJECT_SETTINGS{ _DT("project-settings") };
    inline constexpr static std::string_view XML_READABILITY_TESTS_SECTION{ _DT(
        "readability-tests") };
    inline constexpr static std::wstring_view XML_READABILITY_TESTS_SECTION_W{ _DT(
        L"readability-tests") };
    inline constexpr static std::string_view XML_READING_AGE_FORMAT{ _DT("reading-age-format") };
    inline constexpr static std::string_view XML_INCLUDE_SCORES_SUMMARY_REPORT{ _DT(
        "include-score-summary-report") };
    inline constexpr static std::string_view XML_DOLCH_SUITE{ _DT("dolch-suite") };
    inline constexpr static std::string_view XML_DOLCH_SIGHT_WORDS_TEST{ _DT("dolch-sight-words") };
    inline constexpr static std::string_view XML_TEST_RECOMMENDATION{ _DT("test-recommendation") };
    inline constexpr static std::string_view XML_TEST_BY_INDUSTRY{ _DT("tests-by-industry") };
    inline constexpr static std::string_view XML_TEST_BY_DOCUMENT_TYPE{ _DT(
        "tests-by-document-type") };
    inline constexpr static std::string_view XML_SELECTED_TEST_BUNDLE{ _DT(
        "selected-test-bundle") };
    inline constexpr static std::string_view XML_INDUSTRY_CHILDRENS_PUBLISHING{ _DT(
        "industry-childrens-publishing") };
    inline constexpr static std::string_view XML_INDUSTRY_ADULTPUBLISHING{ _DT(
        "industry-adult-publishing") };
    inline constexpr static std::string_view XML_INDUSTRY_SECONDARY_LANGUAGE{ _DT(
        "industry-secondary-language") };
    inline constexpr static std::string_view XML_INDUSTRY_CHILDRENS_HEALTHCARE{ _DT(
        "industry-childrens-healthcare") };
    inline constexpr static std::string_view XML_INDUSTRY_ADULT_HEALTHCARE{ _DT(
        "industry-healthcare") };
    inline constexpr static std::string_view XML_INDUSTRY_MILITARY_GOVERNMENT{ _DT(
        "industry-military-government") };
    inline constexpr static std::string_view XML_INDUSTRY_BROADCASTING{ _DT(
        "industry-broadcasting") };
    inline constexpr static std::string_view XML_DOCUMENT_GENERAL{ _DT("document-general") };
    inline constexpr static std::string_view XML_DOCUMENT_TECHNICAL{ _DT("document-technical") };
    inline constexpr static std::string_view XML_DOCUMENT_FORM{ _DT("document-form") };
    inline constexpr static std::string_view XML_DOCUMENT_YOUNGADULT{ _DT("document-young-adult") };
    inline constexpr static std::string_view XML_DOCUMENT_CHILDREN_LIT{ _DT(
        "document-children-literature") };
    inline constexpr static std::string_view XML_STAT_GOALS{ _DT("statistics-goals") };
    inline constexpr static std::wstring_view XML_STAT_GOALS_W{ _DT(L"statistics-goals") };
    inline constexpr static std::string_view XML_GOAL_MIN_VAL_GOAL{ _DT("min-value-goal") };
    inline constexpr static std::string_view XML_GOAL_MAX_VAL_GOAL{ _DT("max-value-goal") };
    // text view constants
    inline constexpr static std::string_view XML_TEXT_VIEWS_SECTION{ _DT("text-views") };
    inline constexpr static std::string_view XML_DOCUMENT_DISPLAY_FONTCOLOR{ _DT(
        "document-display-font-color") };
    inline constexpr static std::string_view XML_DOCUMENT_DISPLAY_FONT{ _DT(
        "document-display-font") };
    inline constexpr static std::string_view XML_USE_STANDARD_REPORT_FONT{ _DT(
        "use-standard-report-font") };
    inline constexpr static std::string_view XML_DOLCH_CONJUNCTIONS_HIGHLIGHTCOLOR{ _DT(
        "dolch-conjunction-font-color") };
    inline constexpr static std::string_view XML_DOLCH_PREPOSITIONS_HIGHLIGHTCOLOR{ _DT(
        "dolch-preposition-font-color") };
    inline constexpr static std::string_view XML_DOLCH_PRONOUNS_HIGHLIGHTCOLOR{ _DT(
        "dolch-pronoun-font-color") };
    inline constexpr static std::string_view XML_DOLCH_ADVERBS_HIGHLIGHTCOLOR{ _DT(
        "dolch-adverb-font-color") };
    inline constexpr static std::string_view XML_DOLCH_ADJECTIVES_HIGHLIGHTCOLOR{ _DT(
        "dolch-adjective-font-color") };
    inline constexpr static std::string_view XML_DOLCH_VERBS_HIGHLIGHTCOLOR{ _DT(
        "dolch-verb-font-color") };
    inline constexpr static std::string_view XML_DOLCH_NOUNS_HIGHLIGHTCOLOR{ _DT(
        "dolch-noun-font-color") };
    // general options
    inline constexpr static std::string_view XML_APPEARANCE{ _DT("appearance") };
    inline constexpr static std::string_view XML_WINDOW_MAXIMIZED{ _DT("app-window-maximized") };
    inline constexpr static std::string_view XML_WINDOW_WIDTH{ _DT("app-window-width") };
    inline constexpr static std::string_view XML_WINDOW_HEIGHT{ _DT("app-window-height") };
    inline constexpr static std::string_view XML_UI_LANGUAGE{ _DT("ui-language") };
    // web harvester options
    inline constexpr static std::string_view XML_USER_AGENT{ _DT("user-agent") };
    inline constexpr static std::string_view XML_DISABLE_PEER_VERIFY{ _DT("disable-peer-verify") };
    inline constexpr static std::string_view XML_USE_JS_COOKIES{ _DT("use-javascript-cookies") };
    inline constexpr static std::string_view XML_PERSIST_COOKIES{ _DT("persist-cookies") };
    inline constexpr static std::string_view XML_DOWNLOAD_MIN_FILESIZE{ _DT(
        "download-min-file-size") };
    inline constexpr static std::string_view XML_DOWNLOAD_KEEP_FOLDER_STRUCTURE{ _DT(
        "download-keep-web-folder-structure") };
    inline constexpr static std::string_view XML_DOWNLOAD_REPLACE_EXISTING{ _DT(
        "download-replace-existing-files") };
    // project options
    inline constexpr static std::string_view XML_REVIEWER{ _DT("project-reviewer") };
    inline constexpr static std::string_view XML_STATUS{ _DT("project-status") };
    inline constexpr static std::string_view XML_REALTIME_UPDATE{ _DT("realtime-refresh") };
    inline constexpr static std::string_view XML_APPENDED_DOC_PATH{ _DT("appended-doc-path") };
    // document linking information
    inline constexpr static std::string_view XML_DOCUMENT_STORAGE_METHOD{ _DT(
        "document-storage-method") };
    // stats information
    inline constexpr static std::string_view XML_STATISTICS_RESULTS{ _DT("statistics-results") };
    inline constexpr static std::string_view XML_STATISTICS_REPORT{ _DT("statistics-report") };
    // Min doc size
    inline constexpr static std::string_view XML_MIN_DOC_SIZE_FOR_BATCH{ _DT(
        "min-doc-size-for-batch") };
    inline constexpr static std::string_view XML_RANDOM_SAMPLE_SIZE{ _DT("random-sample-size") };
    inline constexpr static std::string_view XML_RANDOM_SAMPLE_ENABLED{ _DT(
        "random-sample-size-enabled") };
    inline constexpr static std::string_view XML_FILE_PATH_TRUNC_MODE{ _DT(
        "filepath-truncation-mode") };
    // export options
    inline constexpr static std::string_view XML_EXPORT{ _DT("export-settings") };
    inline constexpr static std::string_view XML_EXPORT_LIST_EXT{ _DT("export-list-extension") };
    inline constexpr static std::string_view XML_EXPORT_TEXT_EXT{ _DT("export-text-extension") };
    inline constexpr static std::string_view XML_EXPORT_SUMMARY_REPORT_EXT{ _DT(
        "export-summary-report-extension") };
    inline constexpr static std::string_view XML_EXPORT_GRAPH_EXT{ _DT("export-graph-extension") };
    inline constexpr static std::string_view XML_EXPORT_LISTS{ _DT("export-lists") };
    inline constexpr static std::string_view XML_EXPORT_SENTENCES_BREAKDOWN{ _DT(
        "export-sentence-breakdown") };
    inline constexpr static std::string_view XML_EXPORT_GRAPHS{ _DT("export-graphs") };
    inline constexpr static std::string_view XML_EXPORT_TEST_RESULTS{ _DT("export-test-results") };
    inline constexpr static std::string_view XML_EXPORT_STATS{ _DT("export-statistics") };
    inline constexpr static std::string_view XML_EXPORT_GRAMMAR{ _DT("export-grammar") };
    inline constexpr static std::string_view XML_EXPORT_DOLCH_WORDS{ _DT("export-dolch-words") };
    inline constexpr static std::string_view XML_EXPORT_WARNINGS{ _DT("export-warnings") };
    // warning settings
    inline constexpr static std::string_view XML_WARNING_MESSAGE_SETTINGS{ _DT(
        "warning-message-settings") };
    inline constexpr static std::string_view XML_WARNING_MESSAGE{ _DT("warning-message") };
    inline constexpr static std::string_view XML_PREVIOUS_RESPONSE{ _DT("previous-response") };
    // general strings
    inline constexpr static std::wstring_view ALL_DOCUMENTS_WILDCARD{
        _DT(LR"(*.txt;*.htm;*.html;*.xhtml;*.sgml;*.php;*.php3;*.php4;*.aspx;*.asp;*.rtf;*.doc;*.docx;*.docm;*.pptx;*.pptm;*.dot;*.wri;*.odt;*.ott;*.odp;*.otp;*.ps;*.idl;*.cpp;*.c;*.h;*.md;*.qmd;*.rmd)")
    };
    inline constexpr static std::wstring_view ALL_IMAGES_WILDCARD{ _DT(
        LR"(*.bmp;*.jpg;*.jpeg;*.jpe;*.png;*.gif;*.tga;*.tif;*.tiff;*.pcx)") };
    // last opened file locations
    inline constexpr static std::string_view XML_FILE_OPEN_PATHS{ _DT("file-open-paths") };
    inline constexpr static std::string_view XML_FILE_OPEN_IMAGE_PATH{ _DT("image-path") };
    inline constexpr static std::string_view XML_DOWNLOADS_PATH{ _DT("downloads") };
    inline constexpr static std::string_view XML_FILE_OPEN_PROJECT_PATH{ _DT("project-path") };
    inline constexpr static std::string_view XML_FILE_OPEN_WORDLIST_PATH{ _DT("wordlist-path") };
    // grammar
    inline constexpr static std::string_view XML_GRAMMAR{ _DT("grammar") };
    inline constexpr static std::string_view XML_SPELLCHECK_IGNORE_PROPER_NOUNS{ _DT(
        "spellcheck-ignore-proper-nouns") };
    inline constexpr static std::string_view XML_SPELLCHECK_IGNORE_UPPERCASED{ _DT(
        "spellcheck-ignore-uppercased") };
    inline constexpr static std::string_view XML_SPELLCHECK_IGNORE_NUMERALS{ _DT(
        "spellcheck-ignore-numerals") };
    inline constexpr static std::string_view XML_SPELLCHECK_IGNORE_FILE_ADDRESSES{ _DT(
        "spellcheck-ignore-file-address") };
    inline constexpr static std::string_view XML_SPELLCHECK_IGNORE_PROGRAMMER_CODE{ _DT(
        "spellcheck-ignore-programmer-code") };
    inline constexpr static std::string_view XML_SPELLCHECK_ALLOW_COLLOQUIALISMS{ _DT(
        "spellcheck-allow-colloquialisms") };
    inline constexpr static std::string_view XML_SPELLCHECK_IGNORE_SOCIAL_MEDIA_TAGS{ _DT(
        "spellcheck-ignore-social-media-tags") };
    inline constexpr static std::string_view XML_GRAMMAR_INFO{ _DT("grammar-features") };
    // words breakdown
    inline constexpr static std::string_view XML_WORDS_BREAKDOWN{ _DT("words-breakdown") };
    inline constexpr static std::wstring_view XML_WORDS_BREAKDOWN_W{ _DT(L"words-breakdown") };
    inline constexpr static std::string_view XML_WORDS_BREAKDOWN_INFO{ _DT(
        "words-breakdown-features") };
    // sentences breakdown
    inline constexpr static std::string_view XML_SENTENCES_BREAKDOWN{ _DT("sentences-breakdown") };
    inline constexpr static std::wstring_view XML_SENTENCES_BREAKDOWN_W{ _DT(
        L"sentences-breakdown") };
    inline constexpr static std::string_view XML_SENTENCES_BREAKDOWN_INFO{ _DT(
        "sentences-breakdown-features") };
    // custom colors
    inline constexpr static std::string_view XML_CUSTOM_COLORS{ _DT("custom-colors") };
    };

#endif // READABILITY_APP_OPTIONS_H
