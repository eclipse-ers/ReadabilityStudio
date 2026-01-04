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

#ifndef LUA_BATCH_PROJECTS_H
#define LUA_BATCH_PROJECTS_H

#include "lua_debug.h"
#include "luna.h"
#include <wx/wx.h>

class ProjectDoc;
class BatchProjectDoc;
class ToolsOptionsDlg;

// NOLINTBEGIN(readability-identifier-length)
// NOLINTBEGIN(readability-implicit-bool-conversion)
// cppcheck-suppress-file [functionConst]

namespace LuaScripting
    {
    /// @brief Batch project interface.
    /// @private
    /// @internal This is documented in the readability-studio-api help.
    class BatchProject
        {
        BatchProjectDoc* m_project{ nullptr };
        ToolsOptionsDlg* m_settingsDlg{ nullptr };

        [[nodiscard]]
        bool VerifyProjectIsOpen(const wxString& functionName) const
            {
            if (m_project == nullptr)
                {
                wxMessageBox(wxString::Format(_(L"%s: accessing project that is already closed."),
                                              functionName),
                             _(L"Warning"), wxOK | wxICON_INFORMATION);
                return false;
                }
            return true;
            }

        /// @note This takes into account the boolean parameter in the front
        ///     passed to Lunar objects, so @c minParameterCount should be the actual
        ///     expected number of parameters.
        [[nodiscard]]
        static bool VerifyParameterCount(lua_State* L, const int minParameterCount,
                                         const wxString& functionName)
            {
            assert(L);
            assert(minParameterCount >= 0);
            // skip over the boolean param passed in the front indicating success of
            // routing the function to the class object.
            if ((lua_gettop(L) - 1) < minParameterCount)
                {
                wxMessageBox(
                    wxString::Format(
                        // TRANSLATORS: %s is a function name that failed from a script
                        _(L"%s: invalid number of arguments.\n\n%d expected, %d provided."),
                        functionName, minParameterCount, (lua_gettop(L) - 1)),
                    _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                return false;
                }
            return true;
            }

        bool ReloadIfNotDelayed();
        bool ReloadIfNotDelayedSimple();
        bool m_delayReloading{ false };

      public:
        // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
        static const char className[];
        static Luna<BatchProject>::FunctionType methods[];
        static Luna<BatchProject>::PropertyType properties[];
        // NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)

        BatchProject() = default;
        // Opens a project file.
        // File path to the project to open.
        explicit BatchProject(lua_State* L);
        BatchProject(const BatchProject&) = delete;
        BatchProject& operator=(const BatchProject&) = delete;

        void SetProject(BatchProjectDoc* doc) noexcept { m_project = doc; }

        // quneiform-suppress-begin
        // clang-format off
        // NOTE: these must all be single-line for the build script to properly create new topics from these.

        int LoadFolder(lua_State* L /*string folderPath, boolean recursiveSearch*/); // Analyses all supported documents from the provided folder.
        int LoadFiles(lua_State* L /*table files*/); // Analyzes a list of provided file paths.

        int /*string*/ GetTitle(lua_State* L); // Returns the title of the project.
        int SetWindowSize(lua_State* L /*number width, number height*/); // Sets the size of the project window.
        int /*table*/ GetWindowSize(lua_State* L); // Returns the current size of the project window (table containing width and height).

        int DelayReloading(lua_State* L /*boolean delay*/); // Prevents a project from updating while settings are being changed.

        // SUMMARY STATS
        int SetSummaryStatsResultsOptions(lua_State* L /*boolean includeFormattedReport, boolean TabularReport*/); // Sets which results in the summary statistics section should be included.
        int SetSummaryStatsReportOptions(lua_State* L /*boolean includeParagraphs, boolean includeSentences, boolean includeWords, boolean includeExtendedWords, boolean includeGrammar, boolean includeNotes, boolean includeExtendedInfo*/); // Sets which results in the summary statistics reports should be included.
        int SetSummaryStatsDolchReportOptions(lua_State* L /*boolean includeCoverage, boolean includeWords, boolean includeExplanation*/); // Sets which results in the Dolch summary report should be included.

        // WORDS BREAKDOWN
        int SetWordsBreakdownResultsOptions(lua_State* L /*boolean includeWordCounts, boolean includeSyllableCounts, boolean include3PlusSyllables, boolean include6PlusChars, boolean includeKeyWordCloud, boolean includeDC, boolean includeSpache, boolean includeHJ, boolean includeCustomTests, boolean includeAllWords, boolean includeKeyWords*/); // Sets which results in the words breakdown section should be included.

        // SENTENCES BREAKDOWN
        int SetSentenceBreakdownResultsOptions(lua_State* L /*boolean includeLongSentences, boolean includeLengthsSpread, boolean includeLengthsDistribution, boolean includeLengthsDensity*/); // Sets which results in the sentences breakdown section should be included.

        // GRAMMAR
        int SetSpellCheckerOptions(lua_State* L /*boolean ignoreProperNouns, boolean ignoreUppercased, boolean ignoreNumerals, boolean ignoreFileAddresses, boolean ignoreProgrammerCode, boolean ignoreSocialMediaTags, boolean allowColloquialisms*/); // Sets spell-checker options.
        int SetGrammarResultsOptions(lua_State* L /*boolean includeHighlightedReport, boolean includeErrors, boolean includePossibleMisspellings, boolean includeRepeatedWords, boolean includeArticleMismatches, boolean includeRedundantPhrases, boolean includeOverusedWords, boolean includeWordiness, boolean includeCliches, boolean includePassiveVoice, boolean includeConjunctionStartingSentences, boolean includeLowercasedSentences*/); // Sets which grammar results should be included.

        // PROJECT SETTINGS
        int SetLanguage(lua_State* L /*Language lang*/); // Sets the project language. This will affect syllable counting and which tests are available.
        int /*Language*/ GetLanguage(lua_State* L); // Returns the project's language.
        int SetReviewer(lua_State* L /*string reviewer*/); // Sets the username for the software.
        int /*string*/ GetReviewer(lua_State* L); // Returns the reviewer's name.
        int SetStatus(lua_State* L /*string status*/); // Sets the status of the project. This can be freeform text.
        int /*string*/ GetStatus(lua_State* L); // Returns the status of the project.
        int /*TextSource*/ GetTextSource(lua_State* L); // Returns where a project is getting its content from.
        int SetTextSource(lua_State* L /*TextSource storageMethod*/); // Sets where a project should get its content from.
        int /*TextStorage*/ GetTextStorageMethod(lua_State* L); // Returns whether the project embeds its documents or links to them.
        int SetTextStorageMethod(lua_State* L /*TextStorage storageMethod*/); // Sets whether the project embeds its documents or links to them.
        int SetMinDocWordCountForBatch(lua_State* L /*number wordCount*/); // Sets the minimum number of words a document must have to be included in the project.
        int /*number*/ GetMinDocWordCountForBatch(lua_State* L); // Returns the minimum number of words a document must have to be included in the project.
        int SetFilePathDisplayMode(lua_State* L /*FilePathDisplayMode displayMode*/); // Sets how filepaths are displayed.
        int /*FilePathDisplayMode*/ GetFilePathDisplayMode(lua_State* L); // Returns how filepaths are displayed for new batch projects.
        int SetAppendedDocumentFilePath(lua_State* L/*string filePath*/); // Sets the file path to the document being appended for analysis.
        int /*string*/ GetAppendedDocumentFilePath(lua_State* L); // Returns the file path to the document being appended for analysis.
        int UseRealTimeUpdate(lua_State* L /*boolean use*/); // Toggles whether documents are being re-analyzed as they change.
        int /*boolean*/ IsRealTimeUpdating(lua_State* L); // Returns whether documents are being re-analyzed as they change.

        // DOCUMENT INDEXING
        int /*LongSentence*/ GetLongSentenceMethod(lua_State* L); // Returns the method to determine what a long sentence is.
        int SetLongSentenceMethod(lua_State* L /*LongSentence method*/); // Sets the method to determine what a long sentence is.
        int /*number*/ GetDifficultSentenceLength(lua_State* L); // Returns the threshold for determining an overly-long sentence.
        int SetDifficultSentenceLength(lua_State* L /*number length*/); // Sets the threshold for determining an overly-long sentence.

        int /*ParagraphParse*/ GetParagraphsParsingMethod(lua_State* L); // Returns the method for how paragraphs are parsed.
        int SetParagraphsParsingMethod(lua_State* L /*ParagraphParse parseMethod*/); // Sets how hard returns help determine how paragraphs and sentences are detected.
        int IgnoreBlankLines(lua_State* L /*boolean ignore*/); // Sets whether to ignore blank lines when figuring out if we are at the end of a paragraph.
        int /*boolean*/ IsIgnoringBlankLines(lua_State* L); // Returns whether to ignore blank lines when figuring out if we are at the end of a paragraph.
        int IgnoreIndenting(lua_State* L /*boolean ignore*/); // Sets whether to ignore indenting when parsing paragraphs.
        int /*boolean*/ IsIgnoringIndenting(lua_State* L); // Returns whether to ignore indenting when parsing paragraphs.
        int SetSentenceStartMustBeUppercased(lua_State* L /*boolean caps*/); // Sets whether the first word of a sentence must be capitalized.
        int /*boolean*/ SentenceStartMustBeUppercased(lua_State* L); // Returns whether the first word of a sentence must be capitalized.

        int SetTextExclusion(lua_State* L/*TextExclusionType exclusionType*/); // Specifies how text should be excluded while parsing the source document.
        int /*TextExclusionType*/ GetTextExclusion(lua_State* L); // Returns how text should be excluded.
        int SetIncludeIncompleteTolerance(lua_State* L /*number minWordsForCompleteSentence*/); // Sets the incomplete-sentence tolerance. This is the minimum number of words that will make a sentence missing terminating punctuation be considered complete.
        int /*number*/ GetIncludeIncompleteTolerance(lua_State* L); // Sets the incomplete-sentence tolerance. This is the minimum number of words that will make a sentence missing terminating punctuation be considered complete.
        int AggressivelyExclude(lua_State* L/*boolean beAggressive*/); // Specifies whether to use aggressive text exclusion.
        int ExcludeCopyrightNotices(lua_State* L/*boolean exclude*/); // Specifies whether to exclude copyright notices.
        int ExcludeTrailingCitations(lua_State* L/*boolean exclude*/); // Specifies whether to exclude trailing citations.
        int ExcludeFileAddress(lua_State* L/*boolean exclude*/); // Specifies whether to exclude file addresses.
        int ExcludeNumerals(lua_State* L/*boolean exclude*/); // Specifies whether to exclude numerals.
        int ExcludeProperNouns(lua_State* L/*boolean exclude*/); // Specifies whether to exclude proper nouns.
        int /*boolean*/ IsExcludingAggressively(lua_State* L); // Returns whether to aggressively exclude.
        int /*boolean*/ IsExcludingCopyrightNotices(lua_State* L); // Returns whether to exclude copyright notices.
        int /*boolean*/ IsExcludingTrailingCitations(lua_State* L); // Returns whether to exclude trailing citations.
        int /*boolean*/ IsExcludingFileAddresses(lua_State* L); // Returns whether to exclude file addresses.
        int /*boolean*/ IsExcludingNumerals(lua_State* L); // Returns whether to exclude numerals.
        int /*boolean*/ IsExcludingProperNouns(lua_State* L); // Returns whether to exclude proper nouns.
        int SetPhraseExclusionList(lua_State* L /*string exclusionListPath*/); // Sets the filepath to the phrase exclusion list.
        int /*string*/ GetPhraseExclusionList(lua_State* L); // Returns the filepath to the phrase exclusion list.
        int IncludeExcludedPhraseFirstOccurrence(lua_State* L /*boolean includeFirstOccurrent*/ ); // Sets whether the first occurrence of an excluded phrase should be included.
        int /*boolean*/ IsIncludingExcludedPhraseFirstOccurrence(lua_State* L); // Returns whether the first occurrence of an excluded phrase should be included.
        int SetBlockExclusionTags(lua_State* L /*string tagString*/); // Sets the text exclusion tags. This should be a two-character string containing a pair of exclusion tags.
        int /*string*/ GetBlockExclusionTags(lua_State* L); // Returns the text exclusion tags.

        int SetNumeralSyllabication(lua_State* L /*NumeralSyllabize method*/); // Sets the method to determine how to syllabize numerals.
        int /*NumeralSyllabize*/ GetNumeralSyllabication(lua_State* L); // Returns method to determine how to syllabize numerals.

        // READABILITY SCORES
        int /*boolean*/ IsFogUsingSentenceUnits(lua_State* L); // Returns whether to count independent clauses when calculating Gunning Fog.
        int FogUseSentenceUnits(lua_State* L /*boolean use*/); // Sets whether independent clauses are being counted when calculating Gunning Fog.
        int IncludeStockerCatholicSupplement(lua_State* L /*boolean include*/); // Sets whether to include additional Catholic words with the Dale-Chall test.
        int /*boolean*/ IsIncludingStockerCatholicSupplement(lua_State* L); // Returns whether additional Catholic words are being included with the Dale-Chall test.
        int IncludeScoreSummaryReport(lua_State* L /*boolean include*/); // Sets whether to include the test score summary report.
        int /*boolean*/ IsIncludingScoreSummaryReport(lua_State* L); // Returns whether the test score summary report is being included.
        int SetLongGradeScaleFormat(lua_State* L /*boolean longFormat*/); // Sets whether to display test scores in long format.
        int /*boolean*/ IsUsingLongGradeScaleFormat(lua_State* L); // Returns whether test scores are being shown in long format.
        int /*ReadingAgeDisplay*/ GetReadingAgeDisplay(lua_State* L); // Returns the age display for test scores to display.
        int SetReadingAgeDisplay(lua_State* L /*ReadingAgeDisplay ageFormat*/); // Sets the age display for test scores to display.
        int /*GradeScale*/ GetGradeScale(lua_State* L); // Returns the grade scales used to display test scores.
        int SetGradeScale(lua_State* L /*GradeScale scale*/); // Sets the grade scales used to display test scores.
        int /*FleschNumeralSyllabize*/ GetFleschNumeralSyllabizeMethod(lua_State* L); // Returns how numerals' syllables are counted for the Flesch Reading Ease test.
        int SetFleschNumeralSyllabizeMethod(lua_State* L /*FleschNumeralSyllabize method*/); // Sets how numerals' syllables are counted for the Flesch Reading Ease test.
        int /*FleschKincaidNumeralSyllabize*/ GetFleschKincaidNumeralSyllabizeMethod(lua_State* L); // Returns how numerals' syllables are counted for the Flesch-Kincaid test.
        int SetFleschKincaidNumeralSyllabizeMethod(lua_State* L /*FleschKincaidNumeralSyllabize method*/); // Sets how numerals' syllables are counted for the Flesch-Kincaid test.
        int /*SpecializedTestTextExclusion*/ GetHarrisJacobsonTextExclusionMode(lua_State* L); // Returns how text is excluded for the Harris-Jacobson test.
        int SetHarrisJacobsonTextExclusionMode(lua_State* L /*SpecializedTestTextExclusion method*/); // Sets how text is excluded for the Harris-Jacobson test.
        int /*SpecializedTestTextExclusion*/ GetDaleChallTextExclusionMode(lua_State* L); // Returns how text is excluded for the Dale-Chall test.
        int SetDaleChallTextExclusionMode(lua_State* L /*SpecializedTestTextExclusion method*/); // Sets how text is excluded for the Dale-Chall test.
        int /*ProperNounCountingMethod*/ GetDaleChallProperNounCountingMethod(lua_State* L); // Returns how Dale-Chall counts proper nouns.
        int SetDaleChallProperNounCountingMethod(lua_State* L /*ProperNounCountingMethod method*/); // Sets how Dale-Chall counts proper nouns.

        // GRAPH OPTIONS
        int SetGraphColorScheme(lua_State* L /*string colorScheme*/); // Sets the graph color scheme.
        int /*string*/ GetGraphColorScheme(lua_State* L); // Returns the graph color scheme.
        int SetGraphBackgroundColor(lua_State* L /*string colorName*/); // Sets the graph background color.
        int ApplyGraphBackgroundFade(lua_State* L/*boolean useColorFade*/); // Sets whether to apply a fade to graph background colors.
        int /*boolean*/ IsApplyingGraphBackgroundFade(lua_State* L); // Returns whether to apply a fade to graph background colors.
        int SetPlotBackgroundColor(lua_State* L /*string colorName*/); // Sets the graph background (plot area) color.
        int SetPlotBackgroundColorOpacity(lua_State* L /*number opacity*/); // Sets the graph background color opacity.
        int /*number*/ GetPlotBackgroundColorOpacity(lua_State* L); // Returns the graph background (plot area) color opacity.
        int SetPlotBackgroundImage(lua_State* L /*string imagePath*/); // Sets the graph background (plot area) image.
        int /*string*/ GetPlotBackgroundImage(lua_State* L); // Returns the graph background (plot area) image.
        int SetPlotBackgroundImageEffect(lua_State* L /*ImageEffect effect*/); // Sets the effect applied to an image when drawn as a graph's background.
        int /*ImageEffect*/ GetPlotBackgroundImageEffect(lua_State* L); // Returns the effect applied to an image when drawn as a graph's background.
        int SetPlotBackgroundImageFit(lua_State* L /*ImageFit fitType*/); // Specifies how to adjust an image to fit within a graph's background.
        int /*ImageFit*/ GetPlotBackgroundImageFit(lua_State* L); // Returns how to adjust an image to fit within a graph's background.
        int SetPlotBackgroundImageOpacity(lua_State* L /*number opacity*/); // Sets the graph background (plot area) image opacity.
        int /*number*/ GetPlotBackgroundImageOpacity(lua_State* L); // Returns the graph background (plot area) image opacity.
        int SetWatermark(lua_State* L /*string watermark*/); // Sets the watermark drawn on graphs.
        int /*string*/ GetWatermark(lua_State* L); // Returns the watermark drawn on graphs.
        int SetGraphLogoImage(lua_State* L /*string imagePath*/); // Sets the logo image filepath, shown in the bottom left corner.
        int /*string*/ GetGraphLogoImage(lua_State* L); // Returns the logo image filepath.
        int SetStippleImage(lua_State* L /*string imagePath*/);// Sets the stipple image used to draw bars in graphs.
        int /*string*/ GetStippleImage(lua_State* L);// Returns the stipple image filepath used to draw bars in graphs.
        int SetStippleShape(lua_State* L /*string shapeId*/); // Sets the stipple shape used to draw bars in graphs.
        int /*string*/ GetStippleShape(lua_State* L); // Returns the stipple shape used to draw bars in graphs.
        int SetStippleShapeColor(lua_State* L /*string colorName*/); // If using stipple shapes for bars, sets the color for certain shapes.
        int DisplayGraphDropShadows(lua_State* L /*boolean displayShadows*/); // Sets whether to display shadows on graphs.
        int /*boolean*/ IsDisplayingGraphDropShadows(lua_State* L); // Returns whether to display shadows on graphs.
        int SetGraphCommonImage(lua_State* L /*string imagePath*/); // Sets the common image drawn across all bars.
        int /*string*/ GetGraphCommonImage(lua_State* L); // Returns the common image drawn across all bars.

        int SetXAxisFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' x-axes.
        int SetYAxisFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' y-axes.

        int SetGraphTopTitleFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' top titles.
        int SetGraphBottomTitleFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' bottom titles.
        int SetGraphLeftTitleFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' left titles.
        int SetGraphRightTitleFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' right titles.

        int SetGraphInvalidRegionColor(lua_State* L /*string colorName*/); // Sets the color for the invalid score regions for Fry-like graphs.
        int SetRaygorStyle(lua_State* L /*RaygorStyle style*/); // Sets the visual style for Raygor graphs.
        int /*RaygorStyle*/ GetRaygorStyle(lua_State* L); // Returns the visual style for Raygor graphs.
        int ConnectFleschPoints(lua_State* L /*boolean connect*/); // Sets whether connection lines should be drawn between the three rulers in Flesch charts.
        int /*boolean*/ IsConnectingFleschPoints(lua_State* L); // Returns whether connection lines should be drawn between the three rulers in Flesch charts.
        int IncludeFleschRulerDocGroups(lua_State* L /*boolean include*/); // Sets whether document group labels are drawn next to the syllable ruler on Flesch charts.
        int /*boolean*/ IsIncludingFleschRulerDocGroups(lua_State* L); // Returns document group labels are drawn next to the syllable ruler on Flesch charts.
        int UseEnglishLabelsForGermanLix(lua_State* L /*boolean useEnglish*/); // Sets whether English labels are being used for the brackets on German Lix gauges.
        int /*boolean*/ IsUsingEnglishLabelsForGermanLix(lua_State* L); // Returns whether English labels are being used for the brackets on German Lix gauges.

        int SetBarChartBarColor(lua_State* L /*string colorName*/); // If the bar charts' effect is to use a single color, sets the color to draw the bars with.
        int SetBarChartBarEffect(lua_State* L /*BoxEffect effect*/); // Sets how bars should be drawn within the various bar charts.
        int /*BoxEffect*/ GetBarChartBarEffect(lua_State* L); // Returns how bars should be drawn within the various bar charts.
        int SetBarChartBarOpacity(lua_State* L /*number opacity*/); // Sets the opacity of the bars within the various bar charts.
        int /*number*/ GetBarChartBarOpacity(lua_State* L); // Returns the opacity of the bars within the various bar charts.
        int SetBarChartOrientation(lua_State* L /*Orientation orientation*/); // Sets whether the bar charts should be drawn vertically or horizontally.
        int /*Orientation*/ GetBarChartOrientation(lua_State* L); // Returns whether the bar charts should be drawn vertically or horizontally.
        int DisplayBarChartLabels(lua_State* L /*boolean display*/); // Specifies whether to display labels above each bar in a bar chart.
        int /*boolean*/ IsDisplayingBarChartLabels(lua_State* L); // Returns whether to display labels above each bar in a bar chart.

        int SetHistogramBarColor(lua_State* L /*string colorName*/); // Sets bar color (in histograms).
        int SetHistogramBarEffect(lua_State* L /*BoxEffect barEffect*/); // Sets bar appearance (in histograms).
        int /*BoxEffect*/ GetHistogramBarEffect(lua_State* L); // Returns how bars should be drawn within the various histograms.
        int SetHistogramBarOpacity(lua_State* L /*number opacity*/); // Sets bar opacity (in histograms).
        int /*number*/ GetHistogramBarOpacity(lua_State* L); // Returns the opacity of the bars within the various histograms.
        int SetHistogramBinning(lua_State* L /*BinningMethod method*/); // Sets how data are binned in histograms.
        int /*BinningMethod*/ GetHistogramBinning(lua_State* L); // Returns how data are binned in histograms.
        int SetHistogramRounding(lua_State* L /*Rounding method*/); // Sets how data are rounded (during binning) in histograms.
        int /*Rounding*/ GetHistogramRounding(lua_State* L); // Returns how data are rounded (during binning) in histograms.
        int SetHistogramIntervalDisplay(lua_State* L /*IntervalDisplay display*/); // Sets how bin (axis) labels are displayed on histograms.
        int /*IntervalDisplay*/ GetHistogramIntervalDisplay(lua_State* L); // Returns how bin (axis) labels are displayed on histograms.
        int SetHistogramBinLabelDisplay(lua_State* L /*BinLabelDisplay display*/); // Sets how bar labels are displayed on histograms.
        int /*BinLabelDisplay*/ GetHistogramBinLabelDisplay(lua_State* L); // Returns how bar labels are displayed on histograms.

        int SetBoxPlotColor(lua_State* L /*string colorName*/); // Sets box color (in box plots).
        int SetBoxPlotEffect(lua_State* L /*BoxEffect barEffect*/); // Sets box appearance (in box plots).
        int /*BoxEffect*/ GetBoxPlotEffect(lua_State* L); // Returns how boxes should be drawn within the various box plots.
        int SetBoxPlotOpacity(lua_State* L /*number opacity*/); // Sets box opacity (in box plots).
        int /*number*/ GetBoxPlotOpacity(lua_State* L); // Returns the opacity of the boxes within the various box plots.
        int DisplayBoxPlotLabels(lua_State* L /*boolean display*/); // Specifies whether to display labels on box plots.
        int /*boolean*/ IsDisplayingBoxPlotLabels(lua_State* L); // Returns whether to display labels on box plots.
        int DisplayAllBoxPlotPoints(lua_State* L /*boolean display*/); // Specifies whether to display all points (not just outliers) on box plots.
        int /*boolean*/ IsDisplayingAllBoxPlotPoints(lua_State* L); // Returns whether to display all points (not just outliers) on box plots.
        int ConnectBoxPlotMiddlePoints(lua_State* L /*boolean display*/); // Specifies whether to connect the medians between box plots.
        int /*boolean*/ IsConnectingBoxPlotMiddlePoints(lua_State* L); // Returns whether to connect the medians between box plots.

        int AddTest(lua_State* L/*Test test*/); // Adds a test to the project.
        int Reload(lua_State* L); // Reanalyzes the project's document.
        int Close(lua_State* L /*boolean saveChanges*/); // Closes the project.
        int ExportAll(lua_State* L/*string outputFolder*/); // Exports all the results from the project into a folder.
        int ExportList(lua_State* L /*ListType type, string outputFilePath, number fromRow, number toRow, number fromColumn, number toColumn, boolean includeHeaders, boolean includePageBreaks*/); // Saves the specified list as an HTML, text, or LaTeX file to *outputFilePath*.
        int ExportGraph(lua_State* L /*SideBarSection section, GraphType type, string outputFilePath, boolean grayScale, number width, number height*/); // Saves the specified graph as an image.
        int SelectWindow(lua_State* L /*SideBarSection section, number windowId*/); // Selects a window in the project.
        int ShowSidebar(lua_State* L /*boolean show*/); // Show or hides the project's sidebar.
        int SortList(lua_State* L /*ListType list, number columnToSort, SortOrder order, ...*/); // Sorts the specified list using a series of columns and sorting orders.

        // hidden functions just used for screenshots
        int /*INTERNAL!!!*/ ShowProperties(lua_State* L);
        int /*INTERNAL!!!*/ CloseProperties(lua_State* L);
        // clang-format on
        // quneiform-suppress-end
        };
    } // namespace LuaScripting

// NOLINTEND(readability-implicit-bool-conversion)
// NOLINTEND(readability-identifier-length)

#endif // LUA_BATCH_PROJECTS_H
