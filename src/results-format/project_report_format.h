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

#ifndef PROJECT_REPORT_FORMAT_H
#define PROJECT_REPORT_FORMAT_H

#include "../projects/base_project.h"

/// @brief Class to format a project's information into reports.
class ProjectReportFormat
    {
  public:
    /** @returns HTML text with its trailing &lt;br /&gt; removed.
        @param text The HTML text to trim.*/
    [[nodiscard]]
    static wxString TrimTrailingBreaks(const wxString& text);
    /// @returns The html/head/body start sections for a report.
    /// @param title The page's title.
    /// @note This will include various (internal) CSS styling necessary for the HTML.
    [[nodiscard]]
    static wxString FormatHtmlReportStart(const wxString& title = wxEmptyString);
    /// @returns The html/head/body start sections for a report.
    [[nodiscard]]
    static wxString FormatHtmlReportEnd();
    /** @brief Formats a message into a fancy "Note" tip (in the form of an HTML table).
        @param note The message to format.
        @returns The note formatted in HTML.*/
    [[nodiscard]]
    static wxString FormatHtmlNoteSection(const wxString& note);
    /** @brief Formats a message into a fancy "Warning" (in the form of an HTML table).
        @param note The message to format.
        @returns The note formatted in HTML.*/
    [[nodiscard]]
    static wxString FormatHtmlWarningSection(const wxString& note);
    /** @returns A test's results, description, and notes formatted into an HTML table.
        @param score The calculated score.
        @param theTest The test to format.
        @param note An optional note to include.*/
    [[nodiscard]]
    static wxString FormatTestResult(const wxString& score,
                                     const readability::readability_test& theTest,
                                     const wxString& note = wxEmptyString);
    /** @returns Project statistics information from a project.
        @param project The project to analyze statistics from.
        @param statsInfo Information about which statistics to include.
        @param attentionColor Color to highlight important details.
        @param[out] listData An optional data grid to store tabular results.\n
            Pass in null to ignore this parameter.*/
    [[nodiscard]]
    static wxString
    FormatStatisticsInfo(const BaseProject* project, const StatisticsReportInfo& statsInfo,
                         const wxColour& attentionColor,
                         const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData);
    /** @returns Dolch statistics information from a project.
        @param project The project to analyze Dolch statistics from.
        @param statsInfo Information about which statistics to include.
        @param includeExplanation True to include detailed explanations in the report.
        @param attentionColor Color to highlight important details.
        @param[out] listData An optional data grid to store tabular results.\n
               Pass in null to ignore this parameter.\n
               Caller is responsible for clearing this data grid before calling this function
               because rows will be appended to what was already in there.
               This is because FormatStatisticsInfo() uses this function to append\n
               Dolch statistics to its own @c listData argument.*/
    [[nodiscard]]
    static wxString FormatDolchStatisticsInfo(
        const BaseProject* project, const StatisticsReportInfo& statsInfo, bool includeExplanation,
        const wxColour& attentionColor,
        const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData);
    /** @brief Formats a full sentence from a project's sentence information structure.
        @param project The project containing the sentence and words.
        @param sentence The iterator to the sentence structure.
        @param[out] punctStart The current position of the punctuation info for the document.
            This is updated (i.e., moved) by this function so that in the next call it can
            start off from the same place. This prevents having to scan through the entire
            collection of punctuation to "catch up" to the current sentence every time
            this function needs to be called.
        @param punctEnd The end of the punctuation info.
        @returns The formatted sentence.*/
    [[nodiscard]]
    static wxString
    FormatSentence(const BaseProject* project, const grammar::sentence_info& sentence,
                   std::vector<punctuation::punctuation_mark>::const_iterator& punctStart,
                   const std::vector<punctuation::punctuation_mark>::const_iterator& punctEnd);

    /// @returns The `<span>` start for something in the report that should be in red.
    [[nodiscard]]
    static wxString GetIssueSpanStart()
        {
        return wxSystemSettings::GetAppearance().IsDark() ? L"<span style=\"color:#ED5C7A\">" :
                                                            L"<span style=\"color:#FF0000\">";
        }

    /// @returns The header color for a report table.
    [[nodiscard]]
    static wxColour GetReportHeaderColor()
        {
        return { L"#C3D7D7" };
        }

    /// @returns The header color for a report table.
    [[nodiscard]]
    static wxColour GetReportHeaderFontColor()
        {
        return wxColour{ 0, 0, 0 };
        }

    /** @returns A formula formatted into HTML.
        @param formula The formula to format.*/
    [[nodiscard]]
    static wxString FormatFormulaToHtml(const wxString& formula);
    /** @returns The display name of a stemming language (e.g., English).
        @param stemType The stemmer to review.*/
    [[nodiscard]]
    static wxString GetStemmingDisplayName(const stemming::stemming_type stemType);
    /// maximum number of summary statistics
    constexpr static int MAX_SUMMARY_STAT_ROWS = 500;
    constexpr static int MAX_DOLCH_CONJUNCTION_WORDS = 6;
    constexpr static int MAX_DOLCH_PREPOSITION_WORDS = 16;
    constexpr static int MAX_DOLCH_PRONOUN_WORDS = 26;
    constexpr static int MAX_DOLCH_ADVERB_WORDS = 34;
    constexpr static int MAX_DOLCH_ADJECTIVE_WORDS = 46;
    constexpr static int MAX_DOLCH_VERBS = 92;
    /// should be 95, but we count "Santa" and "Claus" as two words instead of one.
    constexpr static int MAX_DOLCH_NOUNS = 96;

  private:
    [[nodiscard]]
    static wxColour GetReportNoteHeaderColor()
        {
        return { L"#55A8E6" };
        }

    [[nodiscard]]
    static wxString FormatDolchHeader(const wxString& label)
        {
        return wxString::Format(L"\n<tr class='report-column-header' style='background:%s;'>"
                                "<td colspan='3'><span style='color:%s;'>%s</span></td></tr>",
                                GetReportHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
                                GetReportHeaderFontColor().GetAsString(wxC2S_HTML_SYNTAX), label);
        }

    [[nodiscard]]
    static wxString FormatDolchRow(const long labelColumnWidth, const long numberColumnWidth,
                                   const wxString& label, const wxString& value,
                                   const wxString& percent)
        {
        return wxString::Format(L"\n<tr><td style='min-width:%ldpx; width:40%%;'>%s</td>"
                                "<td style='text-align:right; width:%ldpx;'>%s</td>"
                                "<td style='text-align:left;'>%s</td></tr>",
                                labelColumnWidth, label, numberColumnWidth, value, percent);
        }

    /** @returns A test's factors formatted into an HTML table.
        @param test The test to format.*/
    [[nodiscard]]
    static wxString FormatTestFactors(const readability::readability_test& test);
    };

#endif // PROJECT_REPORT_FORMAT_H
