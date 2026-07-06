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
    /// @returns The contents of a theme CSS file from the report-themes resource directory.
    /// @param fileName The CSS file to load (defaults to "default.css").
    /// @param overrideFileName An optional CSS file to load after @p fileName,
    ///     allowing its declarations to override the base theme via the cascade.
    [[nodiscard]]
    static wxString GetThemeCss(const wxString& fileName = _DT(L"default.css"),
                                const wxString& overrideFileName = wxEmptyString);
    /// @returns The html/head/body start sections for a report.
    /// @param title The page's title.
    /// @param overrideCssFile An optional CSS file to load after the base theme.
    /// @note This will include various (internal) CSS styling necessary for the HTML.
    [[nodiscard]]
    static wxString FormatHtmlReportStart(const wxString& title = wxEmptyString,
                                          const wxString& overrideCssFile = wxEmptyString);
    /// @returns The html/head/body start sections for a report.
    [[nodiscard]]
    static wxString FormatHtmlReportEnd();
    /** @brief Formats a report banner with a title and subtitle.
        @param title The main title.
        @param subtitle An optional subtitle (e.g., project name).
        @returns The banner formatted in HTML.*/
    [[nodiscard]]
    static wxString FormatReportBanner(const wxString& title,
                                       const wxString& subtitle = wxEmptyString);
    /** @brief Formats a message into a "Note" callout.
        @param note The message to format.
        @returns The note formatted in HTML.*/
    [[nodiscard]]
    static wxString FormatHtmlNoteSection(const wxString& note);
    /** @brief Formats a message into a "Warning" callout.
        @param note The message to format.
        @returns The note formatted in HTML.*/
    [[nodiscard]]
    static wxString FormatHtmlWarningSection(const wxString& note);
    /** @returns A test's results, description, and notes formatted into HTML.
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
        @param[out] listData An optional data grid to store tabular results.\n
            Pass in null to ignore this parameter.*/
    [[nodiscard]]
    static wxString
    FormatStatisticsInfo(const BaseProject* project, const StatisticsReportInfo& statsInfo,
                         const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData);
    /** @returns Dolch statistics information from a project.
        @param project The project to analyze Dolch statistics from.
        @param statsInfo Information about which statistics to include.
        @param includeExplanation True to include detailed explanations in the report.
        @param[out] listData An optional data grid to store tabular results.\n
               Pass in null to ignore this parameter.\n
               Caller is responsible for clearing this data grid before calling this function
               because rows will be appended to what was already in there.
               This is because FormatStatisticsInfo() uses this function to append\n
               Dolch statistics to its own @c listData argument.*/
    [[nodiscard]]
    static wxString FormatDolchStatisticsInfo(
        const BaseProject* project, const StatisticsReportInfo& statsInfo, bool includeExplanation,
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
    static wxString FormatDolchHeader(const wxString& label)
        {
        return wxString::Format(L"\n<div class='explanation-card-header'>%s</div>"
                                "<div class='explanation-card-body'>",
                                label);
        }

    [[nodiscard]]
    static wxString FormatDolchRow(const wxString& label, const wxString& value,
                                   const wxString& percent)
        {
        return wxString::Format(L"\n<div class='data-row'>"
                                "<span>%s</span>"
                                "<span>%s %s</span></div>",
                                label, value, percent);
        }

    /** @brief Formats a Dolch coverage row, highlighting the value when the
            percentage is high enough to warrant attention.
        @returns HTML for the row, and updates listData if provided.
        @param rowLabel The label for the row.
        @param count The number of words found.
        @param percentage The percentage of the category.
        @param percentText The already-localized "(NN% of all Dolch X)" text for the HTML row.
        @param listPercentText The already-localized "NN% of all Dolch X" text for the list data.
        @param listDataLabel The label for the list data item.
        @param listDataItemCount The current list data item count (updated).
        @param listData The list data provider.*/
    [[nodiscard]]
    static wxString FormatDolchCoverageRow(
        const wxString& rowLabel, size_t count, double percentage, const wxString& percentText,
        const wxString& listPercentText, const wxString& listDataLabel, size_t& listDataItemCount,
        const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData);

    /** @brief Formats a Dolch word count row with total and unique counts.
        @returns HTML for the rows, and updates listData if provided.
        @param rowLabel The label for the total row.
        @param count The total count for the category.
        @param totalWords The total word count in the document.
        @param uniqueRowLabel The label for the unique row.
        @param uniqueCount The unique count for the category.
        @param listDataCountLabel The label for the total list data item.
        @param listDataUniqueLabel The label for the unique list data item.
        @param listDataItemCount The current list data item count (updated).
        @param listData The list data provider.*/
    [[nodiscard]]
    static wxString
    FormatDolchWordsRow(const wxString& rowLabel, size_t count, size_t totalWords,
                        const wxString& uniqueRowLabel, size_t uniqueCount,
                        const wxString& listDataCountLabel, const wxString& listDataUniqueLabel,
                        size_t& listDataItemCount,
                        const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData);

    /** @returns A test's factors formatted into an HTML table.
        @param test The test to format.*/
    [[nodiscard]]
    static wxString FormatTestFactors(const readability::readability_test& test);

    /** @brief Populates a two-column list data row (label + value).
        @param label The label for the row.
        @param value The formatted value string.
        @param listDataItemCount The current row index (incremented).
        @param listData The list data provider.*/
    static void
    PopulateListRow(const wxString& label, const wxString& value, size_t& listDataItemCount,
                    const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData);

    /** @brief Populates a three-column list data row (label + value + percent).
        @param label The label for the row.
        @param value The formatted value string.
        @param percent The formatted percentage string.
        @param listDataItemCount The current row index (incremented).
        @param listData The list data provider.*/
    static void
    PopulateListRow(const wxString& label, const wxString& value, const wxString& percent,
                    size_t& listDataItemCount,
                    const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData);
    };

#endif // PROJECT_REPORT_FORMAT_H
