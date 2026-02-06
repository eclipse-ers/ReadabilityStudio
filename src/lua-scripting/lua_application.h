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

#ifndef LUAAPPLICATION_H
#define LUAAPPLICATION_H

#include "../Wisteria-Dataviz/src/util/donttranslate.h"
#include "lua.hpp"
#include "lua_batch_project.h"
#include "lua_debug.h"
#include "lua_standard_project.h"
#include <map>
#include <wx/wx.h>

// NOLINTBEGIN(readability-identifier-length)
// NOLINTBEGIN(readability-implicit-bool-conversion)
// cppcheck-suppress-begin functionConst

namespace LuaScripting
    {
    void VerifyLink(const wchar_t* link, const size_t length, const bool isImage,
                    const wxString& currentFile, std::multimap<wxString, wxString>& badLinks,
                    std::multimap<wxString, wxString>& badImageSizes,
                    const bool IncludeExternalLinks);

    // quneiform-suppress-begin
    // clang-format off
    // NOTE: these must all be single-line for the build script to properly create new topics from these.

    // Helper functions
    int MsgBox(lua_State* L /*string message*/); // Displays a message dialog.
    int /*string*/ GetLuaConstantsPath(lua_State* L); // Returns the path of the file containing the Lua data types used by the program.
    int /*string*/ GetProgramPath(lua_State* L); // Returns the program's folder path.
    int /*string*/ GetExamplesFolder(lua_State* L); // Returns the program's example documents' folder path.
    int /*string*/ GetUserFolder(lua_State* L /*UserPath path*/); // Returns the path of a given folder in the user's directory.
    int /*string*/ GetAbsoluteFilePath(lua_State* L /*string filePath, string baseFilePath*/); // Returns the absolute filepath for the first path, relative to the second.

    // WEB HARVESTER
    int /*string*/ DownloadFile(lua_State* L /*string Url, string downloadPath*/); // Downloads a webpage to the provided local path.
    int /*string*/ GetDownloadFolder(lua_State* L); // Returns the directory path where the web harvester will download files.
    int SetDownloadFolder(lua_State* L /*string path*/); // Sets the directory path where the web harvester will download files.
    int /*boolean*/ IsUsingWebsitesFolderStructure(lua_State* L); // Returns whether to mirror websites' folder structure when downloading their files.
    int UseWebsitesFolderStructure(lua_State* L /*boolean use*/); // Sets whether to mirror websites' folder structure when downloading their files.
    int /*boolean*/ IsSearchingForBrokenLinks(lua_State* L); // Returns whether a list of broken links are being catalogued while web harvesting.
    int SearchForBrokenLinks(lua_State* L /*boolean use*/); // Sets whether to build a list of broken links while crawling.
    int /*boolean*/ IsReplacingExistingFiles(lua_State* L); // Returns whether files being downloaded will replace existing ones.
    int ReplaceExistingFiles(lua_State* L /*boolean use*/); // Sets whether files being downloaded can overwrite each other if they have the same path.
    int /*number*/ GetMinimumDownloadFileSizeInKilobytes(lua_State* L); // Returns whether files being downloaded will replace existing ones.
    int SetMinimumDownloadFileSizeInKilobytes(lua_State* L /*number fileSize*/); // Sets the minimum size a file must be to download.
    int /*number*/ GetDepthLevel(lua_State* L); // Returns the depth level to crawl from the base URL.
    int SetDepthLevel(lua_State* L /*number depthLevel*/); // Sets the depth level to crawl from the base URL.
    int /*DomainRestriction*/ GetDomainRestriction(lua_State* L); // Returns the domains restrictions that the web harvester is using while crawling.
    int SetDomainRestriction(lua_State* L /*DomainRestriction restriction*/); // Sets the domains restrictions that the web harvester is using while crawling.
    int AddAllowableDomain(lua_State* L /*string domain*/); // Adds a user-defined web path (domain/folder structure) to restrict harvesting to.
    int SetWebHarvesterFileFilter(lua_State* L /*string filter*/); // Sets the file types that the web harvester will download. This should be a semicolon separated list of file wildcards (e.g., "*.html;*.png").
    int SetWebHarvesterWebsite(lua_State* L /*string Url*/); // Sets the website to harvest next.
    int /*boolean*/ WebHarvest(lua_State* L); // Begins crawling the web harvester's currently loaded website.

    int /*table*/ FindFiles(lua_State* L /*string directory, string filePattern, boolean recursive*/); // Returns a list of all files from a folder matching the provided file pattern.
    int Close(lua_State* L); // Closes the program.
    int /*boolean*/ ExportLogReport(lua_State* L /*string outPath*/); // Saves the program's log report to a tab-delimited text file.
    int LogMessage(lua_State* L /*string message*/); // Sends a message to the program's log report.
    int LogError(lua_State* L /*string errorMessage*/); // Sends a warning message to the program's log report.
    int /*boolean*/ WriteToFile(lua_State* L /*string outputFilePath, string content*/); // Writes a string to a file.
    int RemoveAllCustomTests(lua_State* L); // Clears all custom tests from the program.
    int RemoveAllCustomTestBundles(lua_State* L); // Clears all custom test bundles from the program.
    int /*boolean*/ MergeWordLists(lua_State* L /*string inputFile1, string inputFile2, ..., string outputFile*/); // Creates a new word list file from a list of other word list files.
    int /*boolean*/ MergePhraseLists(lua_State* L /*string inputFile1, string inputFile2, ..., string outputFile*/); // Creates a new phrase list file from a list of other phrase list files.
    // Append given suffixes to words in a list.
    // Note that these are internal functions used for expanding our proper noun list.
    int /*boolean*/ /*INTERNAL!!!*/ ExpandWordList(lua_State* L /*string inputWordList, string outputFile, ... suffixesToAddToEachWord*/); // INTERNAL FUNCTION!!! SHOULD NOT BE DOCUMENTED.
    int /*boolean*/ /*INTERNAL!!!*/ PhraseListToWordList(lua_State* L /*string inputPhraseList, string outputFile*/); // INTERNAL FUNCTION!!! SHOULD NOT BE DOCUMENTED.
    int /*boolean*/ CrossReferencePhraseLists(lua_State* L /*string phraseList, string otherPhraseList, string outputFile*/); // Compares two phrase lists and creates a new one that contains only the words that appear in both of them.
    int SetWindowSize(lua_State* L /*number width, number height*/); // Sets the size of the program's main window.
    int /*table*/ GetWindowSize(lua_State* L); // Returns the current size of the application window (table containing width and height).
    int /*number*/ GetTestId(lua_State* L /*string testName*/); // Converts a test name (a string) into it's enumeration equivalent.
    int /*boolean*/ SplashScreen(lua_State* L /*number imageIndex*/); // Displays the splashscreen, based on the provided splashscreen's index.
    int /*boolean*/ CheckHtmlLinks(lua_State* L /*string path, boolean includeExternalLinks*/); // Checks for broken links in folder of HTML documents. (All broken links will be sent to the log report.)
    // Internal testing functions
    int /*boolean*/ QAVerify /*INTERNAL!!!*/ (lua_State* L); // INTERNAL FUNCTION!!! SHOULD NOT BE DOCUMENTED.
    int /*number*/ GetFileCheckSum /*INTERNAL!!!*/ (lua_State* L /*string filePath*/); // INTERNAL FUNCTION!!! SHOULD NOT BE DOCUMENTED.

    // GENERAL SETTINGS
    int /*boolean*/ ImportSettings(lua_State* L /*string filePath*/); // Loads the program's settings from a configuration file.
    int /*boolean*/ ExportSettings(lua_State* L /*string outputFilePath*/); // Saves the program's settings to a configuration file.
    int ResetSettings(lua_State* L); // Resets the program's settings to the factory default.
    int DisableAllWarnings(lua_State* L); // Suppresses all warnings that can appear in the program.
    int EnableAllWarnings(lua_State* L); // Enables all warnings that can appear in the program.
    int EnableWarning(lua_State* L /*string warningId*/); // Enables a specific warning that can appear in the program.
    int DisableWarning(lua_State* L /*string warningId*/); // Disables a specific warning that can appear in the program.
    int SetUserAgent(lua_State* L /*string userAgent*/); // Sets the user agent used by the web harvester.
    int /*string*/ GetUserAgent(lua_State* L); // Returns the user agent used by the web harvester.
    int DisableSSLVerification(lua_State* L /*boolean disable*/); // Toggles whether SSL verification used by the web harvester is disabled.
    int /*boolean*/ IsSSLVerificationDisabled(lua_State* L); // Returns whether SSL verification used by the web harvester is disabled.
    int UseJavaScriptCookies(lua_State* L /*boolean useJSCookies*/); // Toggles whether the web harvester parses JavaScript for cookies needed by the website.
    int /*boolean*/ IsUsingJavaScriptCookies(lua_State* L); // Returns whether the web harvester parses JavaScript for cookies needed by the website.
    int PersistCookies(lua_State* L /*boolean keepCookies*/); // Toggles whether the web harvester uses keeps cookies read from JavaScript between harvests.
    int /*boolean*/ IsPersistingCookies(lua_State* L); // Returns whether the web harvester uses keeps cookies read from JavaScript between harvests.
    int EnableVerboseLogging(lua_State* L /*boolean enable*/); // Toggles whether verbose logging is enabled.
    int /*boolean*/ IsLoggingVerbose(lua_State* L); // Returns whether verbose logging is enabled.
    int AppendDailyLog(lua_State* L /*boolean append*/); // Toggles whether the current log file should be appended when the program restarts.
    int /*boolean*/ IsAppendingDailyLog(lua_State* L); // Returns whether the current log file is being appended when the program restarts.

    // PROJECT SETTINGS
    int SetReviewer(lua_State* L /*string reviewer*/); // Sets the reviewer's name for new projects.
    int /*string*/ GetReviewer(lua_State* L); // Returns the reviewer's name.
    int SetProjectLanguage(lua_State* L /*Language lang*/); // Sets the default language for new projects. This will affect syllable counting and which tests are available.
    int /*Language*/ GetProjectLanguage(lua_State* L); // Returns the default language for new projects.
    int /*TextStorage*/ GetTextStorageMethod(lua_State* L); // Returns whether the project embeds its documents or links to them for new projects.
    int SetTextStorageMethod(lua_State* L /*TextStorage storageMethod*/); // Sets whether new projects embed their documents or link to them.
    int SetMinDocWordCountForBatch(lua_State* L /*number minWordCount*/); // Sets the minimum number of words a document must have for new batch projects.
    int /*number*/ GetMinDocWordCountForBatch(lua_State* L); // Returns the minimum number of words a document must have for new batch projects.
    int SetFilePathDisplayMode(lua_State* L /*FilePathDisplayMode displayMode*/); // Sets how filepaths are displayed for new batch projects.
    int /*FilePathDisplayMode*/ GetFilePathDisplayMode(lua_State* L); // Returns how filepaths are displayed for new batch projects.
    int SetAppendedDocumentFilePath(lua_State* L /*string filePath*/); // Sets the file path to the document being appended for analysis.
    int /*string*/ GetAppendedDocumentFilePath(lua_State* L); // Returns the file path to the document being appended for analysis.
    int UseRealTimeUpdate(lua_State* L /*boolean use*/); // Toggles whether documents are being re-analyzed as they change.
    int /*boolean*/ IsRealTimeUpdating(lua_State* L); // Returns whether documents are being re-analyzed as they change.

    // DOCUMENT INDEXING
    int /*LongSentence*/ GetLongSentenceMethod(lua_State* L); // Returns the method to determine what a long sentence is for new projects.
    int SetLongSentenceMethod(lua_State* L /*LongSentence method*/); // Sets the method to determine what a long sentence is for new projects.
    int /*number*/ GetDifficultSentenceLength(lua_State* L); // Returns the threshold for determining an overly-long sentence for new projects.
    int SetDifficultSentenceLength(lua_State* L /*number length*/); // Sets the threshold for determining an overly-long sentence for new projects.

    int /*ParagraphParse*/ GetParagraphsParsingMethod(lua_State* L); // Returns the default method for how paragraphs are parsed for new projects.
    int SetParagraphsParsingMethod(lua_State* L /*ParagraphParse parseMethod*/); // Sets how hard returns help determine how paragraphs and sentences are detected for new projects.
    int IgnoreBlankLines(lua_State* L /*boolean ignore*/); // Sets whether to ignore blank lines when figuring out if we are at the end of a paragraph for new projects.
    int /*boolean*/ IsIgnoringBlankLines(lua_State* L); // Returns whether to ignore blank lines when figuring out if we are at the end of a paragraph for new projects.
    int IgnoreIndenting(lua_State* L /*boolean ignore*/); // Sets whether to ignore indenting when parsing paragraphs for new projects.
    int /*boolean*/ IsIgnoringIndenting(lua_State* L); // Returns whether to ignore indenting when parsing paragraphs for new projects.
    int SetSentenceStartMustBeUppercased(lua_State* L /*boolean caps*/); // Sets whether the first word of a sentence must be capitalized for new projects.
    int /*boolean*/ SentenceStartMustBeUppercased(lua_State* L); // Returns whether the first word of a sentence must be capitalized for new projects.

    int SetTextExclusion(lua_State* L /*TextExclusionType exclusionType*/); // Sets how text should be excluded for new projects.
    int /*TextExclusionType*/ GetTextExclusion(lua_State* L); // Returns how text should be excluded for new projects.
    int SetIncludeIncompleteTolerance(lua_State* L /*number minWordsForCompleteSentence*/); // Sets the incomplete-sentence tolerance for new projects. This is the minimum number of words that will make a sentence missing terminating punctuation be considered complete.
    int /*number*/ GetIncludeIncompleteTolerance(lua_State* L); // Sets the incomplete-sentence tolerance for new projects. This is the minimum number of words that will make a sentence missing terminating punctuation be considered complete.
    int AggressivelyExclude(lua_State* L /*boolean beAggressive*/); // Sets whether to aggressively exclude for new projects.
    int /*boolean*/ IsExcludingAggressively(lua_State* L); // Returns whether to aggressively exclude for new projects.
    int ExcludeCopyrightNotices(lua_State* L /*boolean exclude*/); // Sets whether to exclude copyright notices for new projects.
    int /*boolean*/ IsExcludingCopyrightNotices(lua_State* L); // Returns whether to exclude copyright notices for new projects.
    int ExcludeTrailingCitations(lua_State* L /*boolean exclude*/); // Sets whether to exclude trailing citations for new projects.
    int /*boolean*/ IsExcludingTrailingCitations(lua_State* L); // Returns whether to exclude trailing citations for new projects.
    int ExcludeFileAddress(lua_State* L /*boolean exclude*/); // Sets whether to exclude file addresses for new projects.
    int /*boolean*/ IsExcludingFileAddresses(lua_State* L); // Returns whether to exclude file addresses for new projects.
    int ExcludeNumerals(lua_State* L /*boolean exclude*/); // Sets whether to exclude numerals for new projects.
    int /*boolean*/ IsExcludingNumerals(lua_State* L); // Returns whether to exclude numerals for new projects.
    int ExcludeProperNouns(lua_State* L /*boolean exclude*/); // Sets whether to exclude proper nouns for new projects.
    int /*boolean*/ IsExcludingProperNouns(lua_State* L); // Returns whether to exclude proper nouns for new projects.
    int SetPhraseExclusionList(lua_State* L /*string exclusionListPath*/); // Sets the filepath to the phrase exclusion list for new projects.
    int /*string*/ GetPhraseExclusionList(lua_State* L); // Returns the filepath to the phrase exclusion list for new projects.
    int IncludeExcludedPhraseFirstOccurrence(lua_State* L /*boolean includeFirstOccurrent*/ ); // Sets whether the first occurrence of an excluded phrase should be included for new projects.
    int /*boolean*/ IsIncludingExcludedPhraseFirstOccurrence(lua_State* L); // Returns whether the first occurrence of an excluded phrase should be included for new projects.
    int SetBlockExclusionTags(lua_State* L /*string tagString*/); // Sets the text exclusion tags for new projects. This should be a two-character string containing a pair of exclusion tags.
    int /*string*/ GetBlockExclusionTags(lua_State* L); // Returns the text exclusion tags for new projects.

    int SetNumeralSyllabication(lua_State* L /*NumeralSyllabize method*/); // Sets the method to determine how to syllabize numerals for new projects.
    int /*NumeralSyllabize*/ GetNumeralSyllabication(lua_State* L); // Returns method to determine how to syllabize numerals for new projects.

    // READABILITY SCORES
    int /*boolean*/ IsFogUsingSentenceUnits(lua_State* L); // Returns whether to count independent clauses when calculating Gunning Fog for new projects.
    int FogUseSentenceUnits(lua_State* L /*boolean use*/); // Sets whether independent clauses are being counted when calculating Gunning Fog for new projects.
    int IncludeStockerCatholicSupplement(lua_State* L /*boolean include*/); // Sets whether to include additional Catholic words with the Dale-Chall test for new projects.
    int /*boolean*/ IsIncludingStockerCatholicSupplement(lua_State* L); // Returns whether additional Catholic words are being included with the Dale-Chall test for new projects.
    int IncludeScoreSummaryReport(lua_State* L /*boolean include*/); // Sets whether to include the test score summary report for new projects.
    int /*boolean*/ IsIncludingScoreSummaryReport(lua_State* L); // Returns whether the test score summary report is being included for new projects.
    int SetLongGradeScaleFormat(lua_State* L /*boolean longFormat*/); // Sets whether to display test scores in long format for new projects.
    int /*boolean*/ IsUsingLongGradeScaleFormat(lua_State* L); // Returns whether test scores are being shown in long format for new projects.
    int /*ReadingAgeDisplay*/ GetReadingAgeDisplay(lua_State* L); // Returns the age display for test scores to display for new projects.
    int SetReadingAgeDisplay(lua_State* L /*ReadingAgeDisplay ageFormat*/); // Sets the age display for test scores to display for new projects.
    int /*GradeScale*/ GetGradeScale(lua_State* L); // Returns the grade scales used to display test scores for new projects.
    int SetGradeScale(lua_State* L /*GradeScale scale*/); // Sets the grade scales used to display test scores for new projects.
    int /*FleschNumeralSyllabize*/ GetFleschNumeralSyllabizeMethod(lua_State* L); // Returns how numerals' syllables are counted for the Flesch Reading Ease test for new projects.
    int SetFleschNumeralSyllabizeMethod(lua_State* L /*FleschNumeralSyllabize method*/); // Sets how numerals' syllables are counted for the Flesch Reading Ease test for new projects.
    int /*FleschKincaidNumeralSyllabize*/ GetFleschKincaidNumeralSyllabizeMethod(lua_State* L); // Returns how numerals' syllables are counted for the Flesch-Kincaid test for new projects.
    int SetFleschKincaidNumeralSyllabizeMethod(lua_State* L /*FleschKincaidNumeralSyllabize method*/); // Sets how numerals' syllables are counted for the Flesch-Kincaid test for new projects.
    int /*SpecializedTestTextExclusion*/ GetHarrisJacobsonTextExclusionMode(lua_State* L); // Returns how text is excluded for the Harris-Jacobson test for new projects.
    int SetHarrisJacobsonTextExclusionMode(lua_State* L /*SpecializedTestTextExclusion method*/); // Sets how text is excluded for the Harris-Jacobson test for new projects.
    int /*SpecializedTestTextExclusion*/ GetDaleChallTextExclusionMode(lua_State* L); // Returns how text is excluded for the Dale-Chall test for new projects.
    int SetDaleChallTextExclusionMode(lua_State* L /*SpecializedTestTextExclusion method*/); // Sets how text is excluded for the Dale-Chall test for new projects.
    int /*ProperNounCountingMethod*/ GetDaleChallProperNounCountingMethod(lua_State* L); // Returns how Dale-Chall counts proper nouns for new projects.
    int SetDaleChallProperNounCountingMethod(lua_State* L /*ProperNounCountingMethod method*/); // Sets how Dale-Chall counts proper nouns for new projects.

    // SUMMARY STATS
    int SetSummaryStatsResultsOptions(lua_State* L /*boolean includeFormattedReport, boolean TabularReport*/); // Sets which results in the summary statistics section should be included for new projects.
    int SetSummaryStatsReportOptions(lua_State* L /*boolean includeParagraphs, boolean includeSentences, boolean includeWords, boolean includeExtendedWords, boolean includeGrammar, boolean includeNotes, boolean includeExtendedInfo*/); // Sets which results in the summary statistics reports should be included for new projects.
    int SetSummaryStatsDolchReportOptions(lua_State* L /*boolean includeCoverage, boolean includeWords, boolean includeExplanation*/); // Sets which results in the Dolch summary report should be included for new projects.

    // WORDS BREAKDOWN
    int SetWordsBreakdownResultsOptions(lua_State* L /*boolean includeWordCounts, boolean includeSyllableCounts, boolean include3PlusSyllables, boolean include6PlusChars, boolean includeKeyWordCloud, boolean includeDC, boolean includeSpache, boolean includeHJ, boolean includeCustomTests, boolean includeAllWords, boolean includeKeyWords*/); // Sets which results in the words breakdown section should be included for new projects.

    // SENTENCES BREAKDOWN
    int SetSentenceBreakdownResultsOptions(lua_State* L /*boolean includeLongSentences, boolean includeLengthsSpread, boolean includeLengthsDistribution, boolean includeLengthsDensity*/); // Sets which results in the sentences breakdown section should be included for new projects.

    // GRAMMAR
    int SetSpellCheckerOptions(lua_State* L /*boolean ignoreProperNouns, boolean ignoreUppercased, boolean ignoreNumerals, boolean ignoreFileAddresses, boolean ignoreProgrammerCode, boolean ignoreSocialMediaTags, boolean allowColloquialisms*/); // Sets spell-checker options for new projects.
    int SetGrammarResultsOptions(lua_State* L /*boolean includeHighlightedReport, boolean includeErrors, boolean includePossibleMisspellings, boolean includeRepeatedWords, boolean includeArticleMismatches, boolean includeRedundantPhrases, boolean includeOverusedWords, boolean includeWordiness, boolean includeCliches, boolean includePassiveVoice, boolean includeConjunctionStartingSentences, boolean includeLowercasedSentences*/); // Sets which grammar results should be included for new projects.

    // HIGHLIGHTED REPORTS
    int SetReportFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for highlighted reports for new projects.
    int SetExcludedTextHighlightColor(lua_State* L /*number red, number green, number blue*/); // Sets the font color for excluded text in formatted reports for new projects.
    int SetDifficultTextHighlightColor(lua_State* L /*string colorName*/); // Sets the font color for difficult text in formatted reports for new projects.
    int SetGrammarIssuesHighlightColor(lua_State* L /*string colorName*/); // Sets the font color for grammar issues in formatted reports for new projects.
    int SetWordyTextHighlightColor(lua_State* L /*string colorName*/); // Sets the font color for wordy content in formatted reports for new projects.
    int SetTextHighlighting(lua_State* L /*TextHighlight highlighting*/); // Sets how to highlight content in formatted reports for new projects.
    int /*TextHighlight*/ GetTextHighlighting(lua_State* L); // Returns how to highlight content in formatted reports for new projects.
    int SetDolchConjunctionsColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch conjunctions in the formatted Dolch report for new projects.
    int SetDolchPrepositionsColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch prepositions in the formatted Dolch report for new projects.
    int SetDolchPronounsColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch pronouns in the formatted Dolch report for new projects.
    int SetDolchAdverbsColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch adverbs in the formatted Dolch report for new projects.
    int SetDolchAdjectivesColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch adjectives in the formatted Dolch report for new projects.
    int SetDolchVerbsColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch verbs in the formatted Dolch report for new projects.
    int SetDolchNounsColor(lua_State* L /*string colorName*/); // Sets the font color for Dolch nouns in the formatted Dolch report for new projects.
    int HighlightDolchConjunctions(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch conjunctions in the formatted Dolch report for new projects.
    int /*boolean*/ IsHighlightingDolchConjunctions(lua_State* L); // Returns whether Dolch conjunctions are being highlighted in the formatted Dolch report for new projects.
    int HighlightDolchPrepositions(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch prepositions in the formatted Dolch report for new projects.
    int /*boolean*/ IsHighlightingDolchPrepositions(lua_State* L); // Returns whether Dolch prepositions are being highlighted in the formatted Dolch report for new projects.
    int HighlightDolchPronouns(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch pronouns in the formatted Dolch report for new projects.
    int /*boolean*/ IsHighlightingDolchPronouns(lua_State* L); // Returns whether Dolch pronouns are being highlighted in the formatted Dolch report for new projects.
    int HighlightDolchAdverbs(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch adverbs in the formatted Dolch report for new projects.
    int /*boolean*/ IsHighlightingDolchAdverbs(lua_State* L); // Returns whether Dolch adverbs are being highlighted in the formatted Dolch report for new projects.
    int HighlightDolchAdjectives(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch adjectives in the formatted Dolch report for new projects.
    int /*boolean*/ IsHighlightingDolchAdjectives(lua_State* L); // Returns whether Dolch adjectives are being highlighted in the formatted Dolch report for new projects.
    int HighlightDolchVerbs(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch verbs in the formatted Dolch report for new projects.
    int /*boolean*/ IsHighlightingDolchVerbs(lua_State* L); // Returns whether Dolch verbs are being highlighted in the formatted Dolch report for new projects.
    int HighlightDolchNouns(lua_State* L /*boolean highlight*/); // Sets whether to highlight Dolch nouns in the formatted Dolch report for new projects.
    int /*boolean*/ IsHighlightingDolchNouns(lua_State* L); // Returns whether Dolch nouns are being highlighted in the formatted Dolch report for new projects.

    // GRAPH OPTIONS
    int SetGraphColorScheme(lua_State* L /*string colorScheme*/); // Sets the graph color scheme for new projects.
    int /*string*/ GetGraphColorScheme(lua_State* L); // Returns the graph color scheme for new projects.
    int SetGraphBackgroundColor(lua_State* L /*string colorName*/); // Sets the graph background color for new projects.
    int ApplyGraphBackgroundFade(lua_State* L /*boolean applyFade*/); // Sets whether to apply a fade to graph background colors for new projects.
    int /*boolean*/ IsApplyingGraphBackgroundFade(lua_State* L); // Returns whether to apply a fade to graph background colors for new projects.
    int SetPlotBackgroundColor(lua_State* L /*string colorName*/); // Sets the graph background (plot area) color for new projects.
    int SetPlotBackgroundColorOpacity(lua_State* L /*number opacity*/); // Sets the graph background (plot area) color opacity for new projects.
    int /*number*/ GetPlotBackgroundColorOpacity(lua_State* L); // Returns the graph background (plot area) color opacity for new projects.
    int SetPlotBackgroundImage(lua_State* L /*string imagePath*/); // Sets the graph background (plot area) image for new projects.
    int /*string*/ GetPlotBackgroundImage(lua_State* L); // Returns the graph background (plot area) image for new projects.
    int SetPlotBackgroundImageEffect(lua_State* L /*ImageEffect effect*/); // Sets the effect applied to an image when drawn as a graph's background for new projects.
    int /*ImageEffect*/ GetPlotBackgroundImageEffect(lua_State* L); // Returns the effect applied to an image when drawn as a graph's background for new projects.
    int SetPlotBackgroundImageOpacity(lua_State* L /*number opacity*/); // Sets the graph background (plot area) image opacity for new projects.
    int /*number*/ GetPlotBackgroundImageOpacity(lua_State* L); // Returns the graph background (plot area) image opacity for new projects.
    int SetPlotBackgroundImageFit(lua_State* L /*ImageFit fitType*/); // Specifies how to adjust an image to fit within a graph's background for new projects.
    int /*ImageFit*/ GetPlotBackgroundImageFit(lua_State* L); // Returns how to adjust an image to fit within a graph's background for new projects.
    int SetWatermark(lua_State* L /*string watermark*/); // Sets the watermark drawn on graphs for new projects.
    int /*string*/ GetWatermark(lua_State* L); // Returns the watermark drawn on graphs for new projects.
    int SetGraphLogoImage(lua_State* L /*string imagePath*/); // Sets the logo image filepath for new projects.
    int /*string*/ GetGraphLogoImage(lua_State* L); // Returns the logo image filepath for new projects.
    int SetStippleImage(lua_State* L /*string imagePath*/);// Sets the stipple image filepath used to draw bars in graphs for new projects.
    int /*string*/ GetStippleImage(lua_State* L);// Returns the stipple image filepath used to draw bars in graphs for new projects.
    int SetStippleShape(lua_State* L /*string shapeId*/); // Sets the stipple shape used to draw bars in graphs for new projects.
    int /*string*/ GetStippleShape(lua_State* L); // Returns the stipple shape used to draw bars in graphs for new projects.
    int SetStippleShapeColor(lua_State* L /*string colorName*/); // If using stipple shapes for bars, sets the color for certain shapes for new projects.
    int SetGraphCommonImage(lua_State* L /*string imagePath*/); // Sets the common image drawn across all bars for new projects.
    int /*string*/ GetGraphCommonImage(lua_State* L); // Returns the common image drawn across all bars for new projects.
    int DisplayGraphDropShadows(lua_State* L /*boolean displayShadows*/); // Sets whether to display shadows on graphs for new projects.
    int /*boolean*/ IsDisplayingGraphDropShadows(lua_State* L); // Returns whether to display shadows on graphs for new projects.
    int ShowcaseKeyItems(lua_State* L /*boolean showcase*/); // Specifies whether important parts of certain graphs should be highlighted for new projects.
    int /*boolean*/ IsShowcasingKeyItems(lua_State* L); // Returns whether important parts of certain graphs should be highlighted for new projects.

    int SetXAxisFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' x-axes for new projects.
    int SetYAxisFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' y-axes for new projects.

    int SetGraphTopTitleFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' top titles for new projects.
    int SetGraphBottomTitleFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' bottom titles.
    int SetGraphLeftTitleFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' left titles.
    int SetGraphRightTitleFont(lua_State* L /*string fontName, number pointSize, FontWeight weight, string color*/); // Sets the font for the graphs' right titles.

    int SetGraphInvalidRegionColor(lua_State* L /*string colorName*/); // Sets the color for the invalid score regions for Fry-like graphs for new projects.
    int SetRaygorStyle(lua_State* L /*RaygorStyle style*/); // Sets the visual style for Raygor graphs for new projects.
    int /*RaygorStyle*/ GetRaygorStyle(lua_State* L); // Returns the visual style for Raygor graphs for new projects.
    int ConnectFleschPoints(lua_State* L /*boolean connect*/); // Sets whether connection lines should be drawn between the three rulers in Flesch charts for new projects.
    int /*boolean*/ IsConnectingFleschPoints(lua_State* L); // Returns whether connection lines should be drawn between the three rulers in Flesch charts for new projects.
    int IncludeFleschRulerDocGroups(lua_State* L /*boolean include*/); // Sets whether document group labels are drawn next to the syllable ruler on Flesch charts for new projects.
    int /*boolean*/ IsIncludingFleschRulerDocGroups(lua_State* L); // Returns whether document group labels are drawn next to the syllable ruler on Flesch charts for new projects.
    int UseEnglishLabelsForGermanLix(lua_State* L /*boolean useEnglish*/); // Sets whether English labels are being used for the brackets on German Lix gauges for new projects.
    int /*boolean*/ IsUsingEnglishLabelsForGermanLix(lua_State* L); // Returns whether English labels are being used for the brackets on German Lix gauges for new projects.

    int SetBarChartBarColor(lua_State* L /*string colorName*/); // Sets bar color (in bar charts) for new projects.
    int SetBarChartBarEffect(lua_State* L /*BoxEffect barEffect*/); // Sets bar appearance (in bar charts) for new projects.
    int /*BoxEffect*/ GetBarChartBarEffect(lua_State* L); // Returns how bars should be drawn within the various bar charts for new projects.
    int SetBarChartBarOpacity(lua_State* L /*number opacity*/); // Sets bar opacity (in bar charts) for new projects.
    int /*number*/ GetBarChartBarOpacity(lua_State* L); // Returns the opacity of the bars within the various bar charts for new projects.
    int SetBarChartOrientation(lua_State* L /*Orientation barOrientation*/); // Sets the orientation for bars for new projects.
    int /*Orientation*/ GetBarChartOrientation(lua_State* L); // Returns whether the bar charts should be drawn vertically or horizontally for new projects.
    int DisplayBarChartLabels(lua_State* L /*boolean display*/); // Specifies whether to display labels above each bar in a bar chart for new projects.
    int /*boolean*/ IsDisplayingBarChartLabels(lua_State* L); // Returns whether to display labels above each bar in a bar chart for new projects.

    int SetHistogramBarColor(lua_State* L /*string colorName*/); // Sets bar color (in histograms) for new projects.
    int SetHistogramBarEffect(lua_State* L /*BoxEffect barEffect*/); // Sets bar appearance (in histograms) for new projects.
    int /*BoxEffect*/ GetHistogramBarEffect(lua_State* L); // Returns how bars should be drawn within the various histograms for new projects.
    int SetHistogramBarOpacity(lua_State* L /*number opacity*/); // Sets bar opacity (in histograms) for new projects.
    int /*number*/ GetHistogramBarOpacity(lua_State* L); // Returns the opacity of the bars within the various histograms for new projects.
    int SetHistogramBinning(lua_State* L /*BinningMethod method*/); // Sets how data are binned in histograms for new projects.
    int /*BinningMethod*/ GetHistogramBinning(lua_State* L); // Returns how data are binned in histograms for new projects.
    int SetHistogramRounding(lua_State* L /*Rounding method*/); // Sets how data are rounded (during binning) in histograms for new projects.
    int /*Rounding*/ GetHistogramRounding(lua_State* L); // Returns how data are rounded (during binning) in histograms for new projects.
    int SetHistogramIntervalDisplay(lua_State* L /*IntervalDisplay display*/); // Sets how bin (axis) labels are displayed on histograms for new projects.
    int /*IntervalDisplay*/ GetHistogramIntervalDisplay(lua_State* L); // Returns how bin (axis) labels are displayed on histograms for new projects.
    int SetHistogramBinLabelDisplay(lua_State* L /*BinLabelDisplay display*/); // Sets how bar labels are displayed on histograms for new projects.
    int /*BinLabelDisplay*/ GetHistogramBinLabelDisplay(lua_State* L); // Returns how bar labels are displayed on histograms for new projects.

    int SetBoxPlotColor(lua_State* L /*string colorName*/); // Sets box color (in box plots) for new projects.
    int SetBoxPlotEffect(lua_State* L /*BoxEffect barEffect*/); // Sets box appearance (in box plots) for new projects.
    int /*BoxEffect*/ GetBoxPlotEffect(lua_State* L); // Returns how boxes should be drawn within the various box plots for new projects.
    int SetBoxPlotOpacity(lua_State* L /*number opacity*/); // Sets box opacity (in box plots) for new projects.
    int /*number*/ GetBoxPlotOpacity(lua_State* L); // Returns the opacity of the boxes within the various box plots for new projects.
    int DisplayBoxPlotLabels(lua_State* L /*boolean display*/); // Specifies whether to display labels on box plots for new projects.
    int /*boolean*/ IsDisplayingBoxPlotLabels(lua_State* L); // Returns whether to display labels on box plots for new projects.
    int DisplayAllBoxPlotPoints(lua_State* L /*boolean display*/); // Specifies whether to display all points (not just outliers) on box plots for new projects.
    int /*boolean*/ IsDisplayingAllBoxPlotPoints(lua_State* L); // Returns whether to display all points (not just outliers) on box plots for new projects.
    int ConnectBoxPlotMiddlePoints(lua_State* L /*boolean display*/); // Specifies whether to connect the medians between box plots for new projects.
    int /*boolean*/ IsConnectingBoxPlotMiddlePoints(lua_State* L); // Returns whether to connect the medians between box plots for new projects.
     
    int /*table*/ GetImageInfo(lua_State* /*string imagePath*/); // Returns width and height for an image.
    int /*boolean*/ ApplyImageEffect(lua_State* /*string inputImagePath, string outputImagePath, ImageEffect effect*/); // Applies an effect to an image and saves the result to another image file. Returns true if image was successfully saved.
    int /*boolean*/ MergeImages(lua_State* /*string inputImage1, ..., string outputImagePath, Orientation direction*/); // Combines a list of images vertically or horizontally. Returns true if image was successfully saved.

    // Gets the active projects
    int /*StandardProject*/ GetActiveStandardProject(lua_State* L); // Returns the active standard project.
    int /*BatchProject*/ GetActiveBatchProject(lua_State* L); // Returns the active batch project.

    // printing
    int SetPaperOrientation(lua_State* L /*Orientation orient*/); // Sets the printer paper orientation. Orientation.Vertical is portrait and Orientation.Horizontal is landscape.
    int /*Orientation*/ GetPaperOrientation(lua_State* L); // Returns the printer paper orientation.
    int SetLeftPrintHeader(lua_State* L /*string label*/); // Sets the left print header.
    int SetCenterPrintHeader(lua_State* L /*string label*/); // Sets the center print header.
    int SetRightPrintHeader(lua_State* L /*string label*/); // Sets the right print header.
    int SetLeftPrintFooter(lua_State* L /*string label*/); // Sets the left print footer.
    int SetCenterPrintFooter(lua_State* L /*string label*/); // Sets the center print footer.
    int SetRightPrintFooter(lua_State* L /*string label*/); // Sets the right print footer.
    int /*string*/ GetLeftPrintHeader(lua_State* L); // Returns the left print header.
    int /*string*/ GetCenterPrintHeader(lua_State* L); // Returns the center print header.
    int /*string*/ GetRightPrintHeader(lua_State* L); // Returns the right print header.
    int /*string*/ GetLeftPrintFooter(lua_State* L); // Returns the left print footer.
    int /*string*/ GetCenterPrintFooter(lua_State* L); // Returns the center print footer.
    int /*string*/ GetRightPrintFooter(lua_State* L); // Returns the right print footer.
    // clang-format on
    // quneiform-suppress-end

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    static const luaL_Reg ApplicationLib[] = {
        { _DT("Close"), Close },
        { "GetLuaConstantsPath", GetLuaConstantsPath },
        { "GetProgramPath", GetProgramPath },
        { "GetExamplesFolder", GetExamplesFolder },
        { "GetUserFolder", GetUserFolder },
        { "GetAbsoluteFilePath", GetAbsoluteFilePath },
        { "GetActiveStandardProject", GetActiveStandardProject },
        { "GetActiveBatchProject", GetActiveBatchProject },
        { "LogMessage", LogMessage },
        { "ExportLogReport", ExportLogReport },
        { "LogError", LogError },
        { "MsgBox", MsgBox },
        { "DownloadFile", DownloadFile },
        { "GetDownloadFolder", GetDownloadFolder },
        { "SetDownloadFolder", SetDownloadFolder },
        { "UseWebsitesFolderStructure", UseWebsitesFolderStructure },
        { "IsUsingWebsitesFolderStructure", IsUsingWebsitesFolderStructure },
        { "SearchForBrokenLinks", SearchForBrokenLinks },
        { "IsSearchingForBrokenLinks", IsSearchingForBrokenLinks },
        { "ReplaceExistingFiles", ReplaceExistingFiles },
        { "IsReplacingExistingFiles", IsReplacingExistingFiles },
        { "SetMinimumDownloadFileSizeInKilobytes", SetMinimumDownloadFileSizeInKilobytes },
        { "GetMinimumDownloadFileSizeInKilobytes", GetMinimumDownloadFileSizeInKilobytes },
        { "GetDepthLevel", GetDepthLevel },
        { "SetDepthLevel", SetDepthLevel },
        { "GetDomainRestriction", GetDomainRestriction },
        { "SetDomainRestriction", SetDomainRestriction },
        { "AddAllowableDomain", AddAllowableDomain },
        { "SetWebHarvesterFileFilter", SetWebHarvesterFileFilter },
        { "SetWebHarvesterWebsite", SetWebHarvesterWebsite },
        { "WebHarvest", WebHarvest },
        { "FindFiles", FindFiles },
        { "GetTestId", GetTestId },
        { "GetFileCheckSum", GetFileCheckSum },
        { "SetProjectLanguage", SetProjectLanguage },
        { "GetProjectLanguage", GetProjectLanguage },
        { "SetReviewer", SetReviewer },
        { "GetReviewer", GetReviewer },
        { "GetTextStorageMethod", GetTextStorageMethod },
        { "SetTextStorageMethod", SetTextStorageMethod },
        { "GetParagraphsParsingMethod", GetParagraphsParsingMethod },
        { "SetParagraphsParsingMethod", SetParagraphsParsingMethod },
        { "GetLongSentenceMethod", GetLongSentenceMethod },
        { "SetLongSentenceMethod", SetLongSentenceMethod },
        { "GetDifficultSentenceLength", GetDifficultSentenceLength },
        { "SetDifficultSentenceLength", SetDifficultSentenceLength },
        { "IgnoreBlankLines", IgnoreBlankLines },
        { "IsIgnoringBlankLines", IsIgnoringBlankLines },
        { "IgnoreIndenting", IgnoreIndenting },
        { "IsIgnoringIndenting", IsIgnoringIndenting },
        { "SetSentenceStartMustBeUppercased", SetSentenceStartMustBeUppercased },
        { "SentenceStartMustBeUppercased", SentenceStartMustBeUppercased },
        { "SetMinDocWordCountForBatch", SetMinDocWordCountForBatch },
        { "GetMinDocWordCountForBatch", GetMinDocWordCountForBatch },
        { "SetFilePathDisplayMode", SetFilePathDisplayMode },
        { "GetFilePathDisplayMode", GetFilePathDisplayMode },
        { "ImportSettings", ImportSettings },
        { "ExportSettings", ExportSettings },
        { "ResetSettings", ResetSettings },
        { "DisableAllWarnings", DisableAllWarnings },
        { "EnableAllWarnings", EnableAllWarnings },
        { "EnableWarning", EnableWarning },
        { "DisableWarning", DisableWarning },
        { "SetUserAgent", SetUserAgent },
        { "GetUserAgent", GetUserAgent },
        { "DisableSSLVerification", DisableSSLVerification },
        { "IsSSLVerificationDisabled", IsSSLVerificationDisabled },
        { "UseJavaScriptCookies", UseJavaScriptCookies },
        { "IsUsingJavaScriptCookies", IsUsingJavaScriptCookies },
        { "PersistCookies", PersistCookies },
        { "IsPersistingCookies", IsPersistingCookies },
        { "EnableVerboseLogging", EnableVerboseLogging },
        { "IsLoggingVerbose", IsLoggingVerbose },
        { "AppendDailyLog", AppendDailyLog },
        { "IsAppendingDailyLog", IsAppendingDailyLog },
        { "SetTextExclusion", SetTextExclusion },
        { "GetTextExclusion", GetTextExclusion },
        { "SetIncludeIncompleteTolerance", SetIncludeIncompleteTolerance },
        { "GetIncludeIncompleteTolerance", GetIncludeIncompleteTolerance },
        { "AggressivelyExclude", AggressivelyExclude },
        { "ExcludeCopyrightNotices", ExcludeCopyrightNotices },
        { "ExcludeTrailingCitations", ExcludeTrailingCitations },
        { "ExcludeFileAddress", ExcludeFileAddress },
        { "ExcludeNumerals", ExcludeNumerals },
        { "ExcludeProperNouns", ExcludeProperNouns },
        { "IsExcludingAggressively", IsExcludingAggressively },
        { "IsExcludingCopyrightNotices", IsExcludingCopyrightNotices },
        { "IsExcludingTrailingCitations", IsExcludingTrailingCitations },
        { "IsExcludingFileAddresses", IsExcludingFileAddresses },
        { "IsExcludingNumerals", IsExcludingNumerals },
        { "IsExcludingProperNouns", IsExcludingProperNouns },
        { "SetPhraseExclusionList", SetPhraseExclusionList },
        { "GetPhraseExclusionList", GetPhraseExclusionList },
        { "SetBlockExclusionTags", SetBlockExclusionTags },
        { "GetBlockExclusionTags", GetBlockExclusionTags },
        { "GetNumeralSyllabication", GetNumeralSyllabication },
        { "SetNumeralSyllabication", SetNumeralSyllabication },
        { "IsFogUsingSentenceUnits", IsFogUsingSentenceUnits },
        { "FogUseSentenceUnits", FogUseSentenceUnits },
        { "IncludeStockerCatholicSupplement", IncludeStockerCatholicSupplement },
        { "IsIncludingStockerCatholicSupplement", IsIncludingStockerCatholicSupplement },
        { "IncludeScoreSummaryReport", IncludeScoreSummaryReport },
        { "IsIncludingScoreSummaryReport", IsIncludingScoreSummaryReport },
        { "SetLongGradeScaleFormat", SetLongGradeScaleFormat },
        { "IsUsingLongGradeScaleFormat", IsUsingLongGradeScaleFormat },
        { "GetReadingAgeDisplay", GetReadingAgeDisplay },
        { "SetReadingAgeDisplay", SetReadingAgeDisplay },
        { "GetGradeScale", GetGradeScale },
        { "SetGradeScale", SetGradeScale },
        { "GetFleschNumeralSyllabizeMethod", GetFleschNumeralSyllabizeMethod },
        { "SetFleschNumeralSyllabizeMethod", SetFleschNumeralSyllabizeMethod },
        { "GetFleschKincaidNumeralSyllabizeMethod", GetFleschKincaidNumeralSyllabizeMethod },
        { "SetFleschKincaidNumeralSyllabizeMethod", SetFleschKincaidNumeralSyllabizeMethod },
        { "GetHarrisJacobsonTextExclusionMode", GetHarrisJacobsonTextExclusionMode },
        { "SetHarrisJacobsonTextExclusionMode", SetHarrisJacobsonTextExclusionMode },
        { "GetDaleChallTextExclusionMode", GetDaleChallTextExclusionMode },
        { "SetDaleChallTextExclusionMode", SetDaleChallTextExclusionMode },
        { "GetDaleChallProperNounCountingMethod", GetDaleChallProperNounCountingMethod },
        { "SetDaleChallProperNounCountingMethod", SetDaleChallProperNounCountingMethod },
        { "IncludeExcludedPhraseFirstOccurrence", IncludeExcludedPhraseFirstOccurrence },
        { "IsIncludingExcludedPhraseFirstOccurrence", IsIncludingExcludedPhraseFirstOccurrence },
        { "SetAppendedDocumentFilePath", SetAppendedDocumentFilePath },
        { "GetAppendedDocumentFilePath", GetAppendedDocumentFilePath },
        { "UseRealTimeUpdate", UseRealTimeUpdate },
        { "IsRealTimeUpdating", IsRealTimeUpdating },
        { "SetGraphColorScheme", SetGraphColorScheme },
        { "GetGraphColorScheme", GetGraphColorScheme },
        { "SetGraphBackgroundColor", SetGraphBackgroundColor },
        { "ApplyGraphBackgroundFade", ApplyGraphBackgroundFade },
        { "IsApplyingGraphBackgroundFade", IsApplyingGraphBackgroundFade },
        { "SetGraphCommonImage", SetGraphCommonImage },
        { "GetGraphCommonImage", GetGraphCommonImage },
        { "SetPlotBackgroundImage", SetPlotBackgroundImage },
        { "GetPlotBackgroundImage", GetPlotBackgroundImage },
        { "SetPlotBackgroundImageEffect", SetPlotBackgroundImageEffect },
        { "GetPlotBackgroundImageEffect", GetPlotBackgroundImageEffect },
        { "SetPlotBackgroundImageFit", SetPlotBackgroundImageFit },
        { "GetPlotBackgroundImageFit", GetPlotBackgroundImageFit },
        { "SetPlotBackgroundImageOpacity", SetPlotBackgroundImageOpacity },
        { "GetPlotBackgroundImageOpacity", GetPlotBackgroundImageOpacity },
        { "SetPlotBackgroundColor", SetPlotBackgroundColor },
        { "SetPlotBackgroundColorOpacity", SetPlotBackgroundColorOpacity },
        { "GetPlotBackgroundColorOpacity", GetPlotBackgroundColorOpacity },
        { "SetWatermark", SetWatermark },
        { "GetWatermark", GetWatermark },
        { "SetGraphLogoImage", SetGraphLogoImage },
        { "GetGraphLogoImage", GetGraphLogoImage },
        { "SetStippleImage", SetStippleImage },
        { "GetStippleImage", GetStippleImage },
        { "SetStippleShape", SetStippleShape },
        { "GetStippleShape", GetStippleShape },
        { "SetXAxisFont", SetXAxisFont },
        { "SetYAxisFont", SetYAxisFont },
        { "SetGraphTopTitleFont", SetGraphTopTitleFont },
        { "SetGraphBottomTitleFont", SetGraphBottomTitleFont },
        { "SetGraphLeftTitleFont", SetGraphLeftTitleFont },
        { "SetGraphRightTitleFont", SetGraphRightTitleFont },
        { "DisplayBarChartLabels", DisplayBarChartLabels },
        { "DisplayGraphDropShadows", DisplayGraphDropShadows },
        { "IsDisplayingGraphDropShadows", IsDisplayingGraphDropShadows },
        { "SetBarChartBarColor", SetBarChartBarColor },
        { "SetBarChartBarOpacity", SetBarChartBarOpacity },
        { "SetBarChartBarEffect", SetBarChartBarEffect },
        { "GetBarChartBarEffect", GetBarChartBarEffect },
        { "GetBarChartBarOpacity", GetBarChartBarOpacity },
        { "GetBarChartOrientation", GetBarChartOrientation },
        { "IsDisplayingBarChartLabels", IsDisplayingBarChartLabels },
        { "SetBarChartOrientation", SetBarChartOrientation },
        { "SetHistogramBarColor", SetHistogramBarColor },
        { "SetHistogramBarEffect", SetHistogramBarEffect },
        { "GetHistogramBarEffect", GetHistogramBarEffect },
        { "SetHistogramBarOpacity", SetHistogramBarOpacity },
        { "GetHistogramBarOpacity", GetHistogramBarOpacity },
        { "SetHistogramBinning", SetHistogramBinning },
        { "GetHistogramBinning", GetHistogramBinning },
        { "SetHistogramRounding", SetHistogramRounding },
        { "GetHistogramRounding", GetHistogramRounding },
        { "SetHistogramIntervalDisplay", SetHistogramIntervalDisplay },
        { "GetHistogramIntervalDisplay", GetHistogramIntervalDisplay },
        { "SetHistogramBinLabelDisplay", SetHistogramBinLabelDisplay },
        { "GetHistogramBinLabelDisplay", GetHistogramBinLabelDisplay },
        { "SetBoxPlotColor", SetBoxPlotColor },
        { "SetBoxPlotEffect", SetBoxPlotEffect },
        { "GetBoxPlotEffect", GetBoxPlotEffect },
        { "SetBoxPlotOpacity", SetBoxPlotOpacity },
        { "GetBoxPlotOpacity", GetBoxPlotOpacity },
        { "DisplayBoxPlotLabels", DisplayBoxPlotLabels },
        { "IsDisplayingBoxPlotLabels", IsDisplayingBoxPlotLabels },
        { "DisplayAllBoxPlotPoints", DisplayAllBoxPlotPoints },
        { "IsDisplayingAllBoxPlotPoints", IsDisplayingAllBoxPlotPoints },
        { "ConnectBoxPlotMiddlePoints", ConnectBoxPlotMiddlePoints },
        { "IsConnectingBoxPlotMiddlePoints", IsConnectingBoxPlotMiddlePoints },
        { "SetGraphInvalidRegionColor", SetGraphInvalidRegionColor },
        { "SetRaygorStyle", SetRaygorStyle },
        { "GetRaygorStyle", GetRaygorStyle },
        { "ConnectFleschPoints", ConnectFleschPoints },
        { "IsConnectingFleschPoints", IsConnectingFleschPoints },
        { "IncludeFleschRulerDocGroups", IncludeFleschRulerDocGroups },
        { "IsIncludingFleschRulerDocGroups", IsIncludingFleschRulerDocGroups },
        { "UseEnglishLabelsForGermanLix", UseEnglishLabelsForGermanLix },
        { "IsUsingEnglishLabelsForGermanLix", IsUsingEnglishLabelsForGermanLix },
        { "SetStippleShapeColor", SetStippleShapeColor },
        { "ShowcaseKeyItems", ShowcaseKeyItems },
        { "IsShowcasingKeyItems", IsShowcasingKeyItems },
        { "SetSpellCheckerOptions", SetSpellCheckerOptions },
        { "SetSummaryStatsResultsOptions", SetSummaryStatsResultsOptions },
        { "SetSummaryStatsReportOptions", SetSummaryStatsReportOptions },
        { "SetWordsBreakdownResultsOptions", SetWordsBreakdownResultsOptions },
        { "SetSummaryStatsDolchReportOptions", SetSummaryStatsDolchReportOptions },
        { "SetSentenceBreakdownResultsOptions", SetSentenceBreakdownResultsOptions },
        { "SetGrammarResultsOptions", SetGrammarResultsOptions },
        { "SetReportFont", SetReportFont },
        { "SetExcludedTextHighlightColor", SetExcludedTextHighlightColor },
        { "SetDifficultTextHighlightColor", SetDifficultTextHighlightColor },
        { "SetGrammarIssuesHighlightColor", SetGrammarIssuesHighlightColor },
        { "SetWordyTextHighlightColor", SetWordyTextHighlightColor },
        { "SetTextHighlighting", SetTextHighlighting },
        { "GetTextHighlighting", GetTextHighlighting },
        { "SetDolchConjunctionsColor", SetDolchConjunctionsColor },
        { "SetDolchPrepositionsColor", SetDolchPrepositionsColor },
        { "SetDolchPronounsColor", SetDolchPronounsColor },
        { "SetDolchAdverbsColor", SetDolchAdverbsColor },
        { "SetDolchAdjectivesColor", SetDolchAdjectivesColor },
        { "SetDolchVerbsColor", SetDolchVerbsColor },
        { "SetDolchNounsColor", SetDolchNounsColor },
        { "HighlightDolchConjunctions", HighlightDolchConjunctions },
        { "IsHighlightingDolchConjunctions", IsHighlightingDolchConjunctions },
        { "HighlightDolchPrepositions", HighlightDolchPrepositions },
        { "IsHighlightingDolchPrepositions", IsHighlightingDolchPrepositions },
        { "HighlightDolchPronouns", HighlightDolchPronouns },
        { "IsHighlightingDolchPronouns", IsHighlightingDolchPronouns },
        { "HighlightDolchAdverbs", HighlightDolchAdverbs },
        { "IsHighlightingDolchAdverbs", IsHighlightingDolchAdverbs },
        { "HighlightDolchAdjectives", HighlightDolchAdjectives },
        { "IsHighlightingDolchAdjectives", IsHighlightingDolchAdjectives },
        { "HighlightDolchVerbs", HighlightDolchVerbs },
        { "IsHighlightingDolchVerbs", IsHighlightingDolchVerbs },
        { "HighlightDolchNouns", HighlightDolchNouns },
        { "IsHighlightingDolchNouns", IsHighlightingDolchNouns },
        { "SetWindowSize", SetWindowSize },
        { "GetWindowSize", GetWindowSize },
        { "RemoveAllCustomTests", RemoveAllCustomTests },
        { "RemoveAllCustomTestBundles", RemoveAllCustomTestBundles },
        { "WriteToFile", WriteToFile },
        { "ExpandWordList", ExpandWordList },
        { "PhraseListToWordList", PhraseListToWordList },
        { "CrossReferencePhraseLists", CrossReferencePhraseLists },
        { "SplashScreen", SplashScreen },
        { "MergeWordLists", MergeWordLists },
        { "MergePhraseLists", MergePhraseLists },
        { "QAVerify", QAVerify },
        { "CheckHtmlLinks", CheckHtmlLinks },
        { "GetPaperOrientation", GetPaperOrientation },
        { "SetPaperOrientation", SetPaperOrientation },
        { "GetLeftPrintHeader", GetLeftPrintHeader },
        { "SetLeftPrintHeader", SetLeftPrintHeader },
        { "GetCenterPrintHeader", GetCenterPrintHeader },
        { "SetCenterPrintHeader", SetCenterPrintHeader },
        { "GetRightPrintHeader", GetRightPrintHeader },
        { "SetRightPrintHeader", SetRightPrintHeader },
        { "GetLeftPrintFooter", GetLeftPrintFooter },
        { "SetLeftPrintFooter", SetLeftPrintFooter },
        { "GetCenterPrintFooter", GetCenterPrintFooter },
        { "SetCenterPrintFooter", SetCenterPrintFooter },
        { "GetRightPrintFooter", GetRightPrintFooter },
        { "SetRightPrintFooter", SetRightPrintFooter },
        { "GetImageInfo", GetImageInfo },
        { "ApplyImageEffect", ApplyImageEffect },
        { "MergeImages", MergeImages },
        { nullptr, nullptr }
    };
    } // namespace LuaScripting

// cppcheck-suppress-end functionConst
// NOLINTEND(readability-implicit-bool-conversion)
// NOLINTEND(readability-identifier-length)

#endif // LUAAPPLICATION_H
