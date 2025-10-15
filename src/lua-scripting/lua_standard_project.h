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

#ifndef LUA_STANDARD_PROJECTS_H
#define LUA_STANDARD_PROJECTS_H

#include "lua_debug.h"
#include "luna.h"
#include <wx/wx.h>

class ProjectDoc;
class BatchProjectDoc;
class ToolsOptionsDlg;

// NOLINTBEGIN(readability-identifier-length)
// NOLINTBEGIN(readability-implicit-bool-conversion)
// cppcheck-suppress-begin functionConst

namespace LuaScripting
    {
    /// @brief Standard project interface.
    /// @private
    /// @internal This is documented in the readability-studio-api help.
    class StandardProject
        {
        ProjectDoc* m_project{ nullptr };
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
        StandardProject() = default;
        // Opens a project file.
        // File path to the project to open.
        explicit StandardProject(lua_State* L);
        StandardProject(const StandardProject&) = delete;
        StandardProject& operator=(const StandardProject&) = delete;

        void SetProject(ProjectDoc* doc) noexcept { m_project = doc; }

        static const char className[];
        static Luna<StandardProject>::FunctionType methods[];
        static Luna<StandardProject>::PropertyType properties[];

        // quneiform-suppress-begin
        // clang-format off
        // NOTE: these must all be single-line for the build script to properly create new topics from these.

        int DelayReloading(lua_State* L /*boolean delay*/); // Prevents a project from updating while settings are being changed. 
        int /*string*/ GetTitle(lua_State* L); // Returns the title of the project.
        int SetWindowSize(lua_State* L /*number width, number height*/); // Sets the size of the project window.

        // Stats functions
        int /*number*/ GetSentenceCount(lua_State* L); // Returns the number of sentences from the document.
        int /*number*/ GetIndependentClauseCount(lua_State* L); // Returns the number of independent clauses from the document.
        int /*number*/ GetNumeralCount(lua_State* L); // Returns the number of numeric words from the document.
        int /*number*/ GetProperNounCount(lua_State* L); // Returns the number of proper nouns from the document.
        int /*number*/ GetUniqueWordCount(lua_State* L); // Returns the number of unique words from the document.
        int /*number*/ GetWordCount(lua_State* L); // Returns the total number of words from the document.
        int /*number*/ GetCharacterAndPunctuationCount(lua_State* L); // Returns the total number of letters, numbers, and punctuation from the document.
        int /*number*/ GetCharacterCount(lua_State* L); // Returns the total number of characters (letters and numbers) from the document.
        int /*number*/ GetSyllableCount(lua_State* L); // Returns the total number of syllables from the document.
        int /*number*/ GetUnique3SyllablePlusWordCount(lua_State* L); // Returns the number of unique words consisting of three or more syllables from the document.
        int /*number*/ Get3SyllablePlusWordCount(lua_State* L); // Returns the number of words consisting of three or more syllables from the document.
        int /*number*/ GetUnique1SyllableWordCount(lua_State* L); // Returns the number of unique words consisting of three or more syllables from the document.
        int /*number*/ Get1SyllableWordCount(lua_State* L); // Returns the number of monosyllabic words from the document.
        int /*number*/ Get7CharacterPlusWordCount(lua_State* L); // Returns the number of words consisting of seven or more characters from the document.
        int /*number*/ GetUnique6CharPlusWordCount(lua_State* L); // Returns the number of unique words consisting of six or more characters from the document.
        int /*number*/ Get6CharacterPlusWordCount(lua_State* L); // Returns the number of words consisting of six or more characters from the document.
        int /*number*/ GetUnfamiliarSpacheWordCount(lua_State* L); // Returns the number of words unfamiliar to the Spache test from the document.
        int /*number*/ GetUnfamiliarDCWordCount(lua_State* L); // Returns the number of words unfamiliar to the Dale-Chall test from the document.
        int /*number*/ GetUnfamiliarHJWordCount(lua_State* L); // Returns the number of words unfamiliar to the Harris-Jacobson test from the document.

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

        // HIGHLIGHTED REPORTS
        int SetReportFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for highlighted reports.
        int SetExcludedTextHighlightColor(lua_State* L /*string colorName*/); // Sets the font color for excluded text in formatted reports.
        int SetDifficultTextHighlightColor(lua_State* L /*string colorName*/); // Sets the font color for difficult text in formatted reports.
        int SetGrammarIssuesHighlightColor(lua_State* L /*string colorName*/); // Sets the font color for grammar issues in formatted reports.
        int SetWordyTextHighlightColor(lua_State* L /*string colorName*/); // Sets the font color for wordy content in formatted reports.
        int SetTextHighlighting(lua_State* L /*TextHighlight highlighting*/); // Sets how to highlight content in formatted reports.
        int /*TextHighlight*/ GetTextHighlighting(lua_State* L); // Returns how to highlight content in formatted reports.
        int SetDolchConjunctionsColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch conjunctions in the formatted Dolch report.
        int SetDolchPrepositionsColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch prepositions in the formatted Dolch report.
        int SetDolchPronounsColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch pronouns in the formatted Dolch report.
        int SetDolchAdverbsColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch adverbs in the formatted Dolch report.
        int SetDolchAdjectivesColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch adjectives in the formatted Dolch report.
        int SetDolchVerbsColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch verbs in the formatted Dolch report.
        int SetDolchNounsColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch nouns in the formatted Dolch report.
        int HighlightDolchConjunctions(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch conjunctions in the formatted Dolch report.
        int /*boolean*/ IsHighlightingDolchConjunctions(lua_State* L); // Returns whether Dolch conjunctions are being highlighted in the formatted Dolch report.
        int HighlightDolchPrepositions(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch prepositions in the formatted Dolch report.
        int /*boolean*/ IsHighlightingDolchPrepositions(lua_State* L); // Returns whether Dolch prepositions are being highlighted in the formatted Dolch report.
        int HighlightDolchPronouns(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch pronouns in the formatted Dolch report.
        int /*boolean*/ IsHighlightingDolchPronouns(lua_State* L); // Returns whether Dolch pronouns are being highlighted in the formatted Dolch report.
        int HighlightDolchAdverbs(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch adverbs in the formatted Dolch report.
        int /*boolean*/ IsHighlightingDolchAdverbs(lua_State* L); // Returns whether Dolch adverbs are being highlighted in the formatted Dolch report.
        int HighlightDolchAdjectives(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch adjectives in the formatted Dolch report.
        int /*boolean*/ IsHighlightingDolchAdjectives(lua_State* L); // Returns whether Dolch adjectives are being highlighted in the formatted Dolch report.
        int HighlightDolchVerbs(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch verbs in the formatted Dolch report.
        int /*boolean*/ IsHighlightingDolchVerbs(lua_State* L); // Returns whether Dolch verbs are being highlighted in the formatted Dolch report.
        int HighlightDolchNouns(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch nouns in the formatted Dolch report.
        int /*boolean*/ IsHighlightingDolchNouns(lua_State* L); // Returns whether Dolch nouns are being highlighted in the formatted Dolch report.

        // PROJECT SETTINGS
        int SetLanguage(lua_State* L /*Language lang*/); // Sets the project language. This will affect syllable counting and which tests are available.
        int /*Language*/ GetLanguage(lua_State* L); // Returns the project's language.
        int SetReviewer(lua_State* L /*string reviewer*/); // Sets the username for the software.
        int /*string*/ GetReviewer(lua_State* L); // Returns the reviewer's name.
        int SetStatus(lua_State* L /*string status*/); // Sets the status of the project. This can be freeform text.
        int /*string*/ GetStatus(lua_State* L); // Returns the status of the project.
        int /*TextStorage*/ GetTextStorageMethod(lua_State* L); // Returns whether the project embeds its documents or links to them.
        int SetTextStorageMethod(lua_State* L /*TextStorage storageMethod*/); // Sets whether the project embeds its documents or links to them.
        int SetDocumentFilePath(lua_State* L /*string docPath*/); // Sets the path of the document being analyzed by the project.
        int /*TextSource*/ GetTextSource(lua_State* L); // Returns where a project is getting its content from.
        int SetTextSource(lua_State* L /*TextSource storageMethod*/); // Sets where a project should get its content from.
        int /*string*/ GetDocumentFilePath(lua_State* L); // Returns the path of the document being analyzed by the project.
        int SetAppendedDocumentFilePath(lua_State* L /*string filePath*/); // Sets the file path to the document being appended for analysis.
        int /*string*/ GetAppendedDocumentFilePath(lua_State* L); // Returns the file path to the document being appended for analysis.
        int UseRealTimeUpdate(lua_State* L /*boolean use*/); // Toggles whether documents are being re-analyzed as they change.
        int /*boolean*/ IsRealTimeUpdating(lua_State* L); // Returns whether documents are being re-analyzed as they change.

        // DOCUMENT INDEXING
        int /*LongSentence*/ GetLongSentenceMethod(lua_State* L); // Returns the method to determine what a long sentence is.
        int SetLongSentenceMethod(lua_State* L /*LongSentence method*/); // Sets the method to determine what a long sentence is.
        int /*number*/ GetDifficultSentenceLength(lua_State* L); // Returns the threshold for determining an overly-long sentence.
        int SetDifficultSentenceLength(lua_State* L /*number length*/); // Sets the threshold for determining an overly-long sentence.

        int SetParagraphsParsingMethod(lua_State* L /*ParagraphParse parseMethod*/); // Sets how hard returns help determine how paragraphs and sentences are detected.
        int /*ParagraphParse*/ GetParagraphsParsingMethod(lua_State* L); // Returns the method for how paragraphs are parsed.
        int IgnoreBlankLines(lua_State* L /*boolean ignore*/); // Sets whether to ignore blank lines when figuring out if we are at the end of a paragraph.
        int /*boolean*/ IsIgnoringBlankLines(lua_State* L); // Returns whether to ignore blank lines when figuring out if we are at the end of a paragraph.
        int IgnoreIndenting(lua_State* L /*boolean ignore*/); // Sets whether to ignore indenting when parsing paragraphs.
        int /*boolean*/ IsIgnoringIndenting(lua_State* L); // Returns whether to ignore indenting when parsing paragraphs.
        int SetSentenceStartMustBeUppercased(lua_State* L /*boolean caps*/); // Sets whether the first word of a sentence must be capitalized.
        int /*boolean*/ SentenceStartMustBeUppercased(lua_State* L); // Returns whether the first word of a sentence must be capitalized.

        int SetTextExclusion(lua_State* L /*TextExclusionType exclusionType*/); // Specifies how text should be excluded while parsing the source document.
        int /*TextExclusionType*/ GetTextExclusion(lua_State* L); // Returns how text should be excluded.
        int SetIncludeIncompleteTolerance(lua_State* L /*number minWordsForCompleteSentence*/); // Sets the incomplete-sentence tolerance. This is the minimum number of words that will make a sentence missing terminating punctuation be considered complete.
        int /*number*/ GetIncludeIncompleteTolerance(lua_State* L); // Sets the incomplete-sentence tolerance. This is the minimum number of words that will make a sentence missing terminating punctuation be considered complete.
        int AggressivelyExclude(lua_State* L /*boolean beAggressive*/); // Specifies whether to use aggressive text exclusion.
        int ExcludeCopyrightNotices(lua_State* L /*boolean exclude*/); // Specifies whether to exclude copyright notices.
        int ExcludeTrailingCitations(lua_State* L /*boolean exclude*/); // Specifies whether to exclude trailing citations.
        int ExcludeFileAddress(lua_State* L /*boolean exclude*/); // Specifies whether to exclude file addresses.
        int ExcludeNumerals(lua_State* L /*boolean exclude*/); // Specifies whether to exclude numerals.
        int ExcludeProperNouns(lua_State* L /*boolean exclude*/); // Specifies whether to exclude proper nouns.
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
        int /*ReadingAgeDisplay*/GetReadingAgeDisplay(lua_State* L); // Returns the age display for test scores to display.
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
        int ApplyGraphBackgroundFade(lua_State* L /*boolean useColorFade*/); // Sets whether to apply a fade to graph background colors.
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
        int /*string*/ GetWatermark(lua_State* L); // Returns the watermark drawn on graphs for.
        int SetGraphLogoImage(lua_State* L /*string imagePath*/); // Sets the logo image filepath, shown in the bottom left corner.
        int /*string*/ GetGraphLogoImage(lua_State* L); // Returns the logo image filepath.
        int SetStippleImage(lua_State* L /*string imagePath*/);// Sets the stipple image used to draw bars in graphs.
        int /*string*/ GetStippleImage(lua_State* L);// Returns the stipple image filepath used to draw bars in graphs.
        int SetStippleShape(lua_State* L /*string shapeId*/); // Sets the stipple shape used to draw bars in graphs.
        int /*string*/ GetStippleShape(lua_State* L); // Returns the stipple shape used to draw bars in graphs.
        int SetStippleShapeColor(lua_State* L /*string colorName*/); // If using stipple shapes for bars, sets the color for certain shapes.
        int SetGraphCommonImage(lua_State* L /*string imagePath*/); // Sets the common image drawn across all bars.
        int /*string*/ GetGraphCommonImage(lua_State* L); // Returns the common image drawn across all bars.
        int DisplayGraphDropShadows(lua_State* L /*boolean displayShadows*/); // Sets whether to display shadows on graphs.
        int /*boolean*/ IsDisplayingGraphDropShadows(lua_State* L); // Returns whether to display shadows on graphs.
        int ShowcaseKeyItems(lua_State* L /*boolean showcase*/); // Specifies whether important parts of certain graphs should be highlighted.
        int /*boolean*/ IsShowcasingKeyItems(lua_State* L); // Returns whether important parts of certain graphs should be highlighted.

        int SetXAxisFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' X axes.
        int SetYAxisFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' Y axes.

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

        // TEST OPTIONS
        int AddTest(lua_State* L /*Test test*/); // Adds a test to the project.
        int Reload(lua_State*); // Reanalyzes the project's document.
        int Close(lua_State* L /*boolean saveChanges*/); // Closes the project.

        int ExportAll(lua_State* L /*string outputFolder*/); // Exports all the results from the project into a folder.
        int ExportGraph(lua_State* L/*GraphType type, string outputFilePath, boolean grayScale, number width, number height*/); // Saves the specified graph as an image.
        int ExportScores(lua_State* L /*string outputFilePath*/); // Saves the project's test scores to *outputFilePath*.
        int ExportHighlightedWords(lua_State* L /*HighlightedWordReportType type, string outputFilePath*/); // Saves the specified highlighted words as an RTF or HTML report.
        int ExportReport(lua_State* L /*ReportType type, string outputFilePath*/); // Saves the specified report.
        // Saves a list from the project.
        // ListType Which list to save.
        // FilePath The file path to save the list.
        int ExportList(lua_State* L);
        // cppcheck-suppress functionConst
        int ExportFilteredText(lua_State* L /*string outputFilePath, boolean romanizeText, boolean removeEllipses, boolean removeBullets, boolean removeFilePaths, boolean stripAbbreviations*/); // Saves a copy of the project's document with excluded text (and other optional items) filtered out. Returns true if successful.
        int SortList(lua_State* L /*ListType list, number columnToSort, SortOrder order, ...*/); // Sorts the specified list using a series of columns and sorting orders.
        int SortGraph(lua_State* L /*GraphType graphType, SortOrder order*/); // Sorts the bars in the specified bar chart.
        int SelectWindow(lua_State* L /*SideBarSection section, number windowId*/); // Selects a window in the project.
        int ShowSidebar(lua_State* L /*boolean show*/); // Show or hides the project's sidebar.

        int SelectTextGrammarWindow(lua_State* L /*string contentToHighlight*/); // Selects the highlighted text window in the Grammar section. (Can also optionally find and select a string in it.)
        int SelectHighlightedWordReport(lua_State* L /*HighlightedReportType windowToSelect, string contentToHighlight*/); // Selects a highlighted text window in the Word Breakdown section. (Can also optionally find and select a string in it.)

        // HIDDEN interfaces for testing and screenshots
        /* Selects and sorts a list in the Words Breakdown section.
           WindowToSelect The list window to select items in. Refer to ListType enumeration.
           RowsToSelect Rows to select.*/
        int /*INTERNAL!!!*/ SelectRowsInWordsBreakdownList(lua_State* L);
        /* Selects and scrolls down a text window.
           WindowToSelect The text window to select. Refer to HighlightedReportType enumeration.
           ContentToHighlight The content to scroll to.*/
        int /*INTERNAL!!!*/ ScrollTextWindow(lua_State* L);
        /* Selects the Readability Results section of the project and highlights a test by index.
           TestToSelect The test to select, based on position in the list.*/
        int /*INTERNAL!!!*/ SelectReadabilityTest(lua_State* L);
        // Opens the properties dialog and the specified page
        int /*INTERNAL!!!*/ ShowProperties(lua_State* L);
        int /*INTERNAL!!!*/ CloseProperties(lua_State*);
        // clang-format on
        // quneiform-suppress-end
        };
    } // namespace LuaScripting

// cppcheck-suppress-end functionConst
// NOLINTEND(readability-implicit-bool-conversion)
// NOLINTEND(readability-identifier-length)

#endif // LUA_STANDARD_PROJECTS_H
