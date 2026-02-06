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

#ifndef BASE_PROJECT_H
#define BASE_PROJECT_H

#include "../Wisteria-Dataviz/src/data/dataset.h"
#include "../Wisteria-Dataviz/src/import/doc_extract_text.h"
#include "../Wisteria-Dataviz/src/import/rtf_extract_text.h"
#include "../Wisteria-Dataviz/src/import/xlsx_extract_text.h"
#include "../Wisteria-Dataviz/src/math/mathematics.h"
#include "../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../Wisteria-Dataviz/src/util/formulaformat.h"
#include "../Wisteria-Dataviz/src/util/frequencymap.h"
#include "../Wisteria-Dataviz/src/util/memorymappedfile.h"
#include "../Wisteria-Dataviz/src/util/textstream.h"
#include "../Wisteria-Dataviz/src/util/xml_format.h"
#include "../Wisteria-Dataviz/src/util/zipcatalog.h"
#include "../app/readability_app_options.h"
#include "../graphs/frasegraph.h"
#include "../indexing/german_syllabize.h"
#include "../indexing/phrase.h"
#include "../indexing/spanish_syllabize.h"
#include "../indexing/word_collection.h"
#include "../readability/custom_readability_test.h"
#include "../readability/dolch.h"
#include "../readability/readability_project_test.h"
#include "../readability/readability_test.h"
#include "../readability/spanish_readability.h"
#include "../results-format/readability_messages.h"
#include "../test-helpers/readability_formula_parser.h"
#include "../test-helpers/test_bundle.h"
#include "../test-helpers/tests_functional.h"
#include "../tinyxml2/tinyxml2.h"
#include "../webharvester/filepathresolver.h"
#include "../webharvester/webharvester.h"
#include "project_refresh.h"
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <ranges>
#include <set>
#include <unordered_map>
#include <vector>
#include <wx/docview.h>
#include <wx/ffile.h>
#include <wx/file.h>
#include <wx/string.h>
#include <wx/wizard.h>
#include <wx/wx.h>
#include <wx/zstream.h>

/** Using a set would be ideal so that we would be forced only have unique custom tests in here
    (based on the test's ID). However, using a set requires copy constructing some large vectors of
    tests every time we add or edit a test, so it's more optimal to use a vector and just handle
    checking for duplicate test IDs ourselves.*/
using CustomReadabilityTestCollection = std::vector<CustomReadabilityTest>;

/// @brief Helper structure for handling an Excel (XLSX) file.
struct ExcelFile
    {
    explicit ExcelFile(const wxString& filePath) : m_zip(filePath) {}

    lily_of_the_valley::xlsx_extract_text m_xlsx_extract{ false };
    using Workbook = std::map<wxString, lily_of_the_valley::xlsx_extract_text::worksheet>;
    Workbook m_worksheets;
    Wisteria::ZipCatalog m_zip;
    };

/** @brief Stores the unique IDs in here of the tests being used.
    @details The system will look these up from the custom tests stored globally*/
class CustomReadabilityTestInterface
    {
  public:
    CustomReadabilityTestInterface() = delete;

    explicit CustomReadabilityTestInterface(wxString testName) : m_testName(std::move(testName)) {}
    CustomReadabilityTestInterface(const CustomReadabilityTestInterface& that)
        : m_formulasFlags(that.m_formulasFlags), m_testName(that.m_testName),
          m_uniqueUnfamiliarWordCount(that.m_uniqueUnfamiliarWordCount),
          m_unfamiliarWordCount(that.m_unfamiliarWordCount), m_iter(that.m_iter)
        // don't copy over list view data, but that should be empty anyway. This
        // will be filled in later after the next recalculation.
        {
        }

    CustomReadabilityTestInterface& operator=(const CustomReadabilityTestInterface& that)
        {
        if (this != &that)
            {
            m_formulasFlags = that.m_formulasFlags;
            m_testName = that.m_testName;
            m_uniqueUnfamiliarWordCount = that.m_uniqueUnfamiliarWordCount;
            m_unfamiliarWordCount = that.m_unfamiliarWordCount;
            m_iter = that.m_iter;
            }
        // don't copy over list view data, but that should be empty anyway. This
        // will be filled in later after the next recalculation.
        return *this;
        }

    [[nodiscard]]
    bool operator<(const CustomReadabilityTestInterface& that) const
        {
        return m_testName < that.m_testName;
        }

    [[nodiscard]]
    bool operator==(const CustomReadabilityTestInterface& that) const
        {
        return m_testName == that.m_testName;
        }

    [[nodiscard]]
    bool operator==(const wxString& testName) const
        {
        return m_testName == testName;
        }

    [[nodiscard]]
    bool operator()(const word_case_insensitive_no_stem& theWord) const
        {
        return m_iter->is_word_familiar(theWord);
        }

    [[nodiscard]]
    const double& GetUniqueUnfamiliarWordCount() const noexcept
        {
        return m_uniqueUnfamiliarWordCount;
        }

    [[nodiscard]]
    const double& GetUnfamiliarWordCount() const noexcept
        {
        return m_unfamiliarWordCount;
        }

    void SetUniqueUnfamiliarWordCount(const double wordCount) noexcept
        {
        m_uniqueUnfamiliarWordCount = wordCount;
        }

    void SetUnfamiliarWordCount(const double wordCount) noexcept
        {
        m_unfamiliarWordCount = wordCount;
        }

    void IncrementUniqueUnfamiliarWordCount() noexcept { ++m_uniqueUnfamiliarWordCount; }

    void IncrementUnfamiliarWordCount(const double size) noexcept { m_unfamiliarWordCount += size; }

    [[nodiscard]]
    const wxString& GetTestName() const noexcept
        {
        return m_testName;
        }

    /// Flushes out recorded values. Call this before re-calculating the test.
    void Reset()
        {
        GetListViewData()->DeleteAllItems();
        m_uniqueUnfamiliarWordCount = 0;
        m_unfamiliarWordCount = 0;
        m_iter->reset();
        }

    void ResetIterator() { m_iter->reset(); }

    void SetIterator(const CustomReadabilityTestCollection::iterator& iter) noexcept
        {
        m_iter = iter;
        }

    [[nodiscard]]
    CustomReadabilityTestCollection::const_iterator GetIterator() const noexcept
        {
        return m_iter;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetListViewData()
        {
        if (m_listViewData == nullptr)
            {
            m_listViewData = std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        return m_listViewData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetListViewData() const
        {
        assert(m_listViewData);
        return m_listViewData;
        }

    // flags indicating that the formula is a specialized familiar word test
    void SetIsDaleChallFormula(const bool enableFlag) { m_formulasFlags.set(0, enableFlag); }

    [[nodiscard]]
    bool IsDaleChallFormula() const
        {
        return m_formulasFlags[0];
        }

    void SetIsHarrisJacobsonFormula(const bool enableFlag) { m_formulasFlags.set(1, enableFlag); }

    [[nodiscard]]
    bool IsHarrisJacobsonFormula() const
        {
        return m_formulasFlags[1];
        }

  private:
    // DC and HJ have their own explicit, unique logic for how numerals and proper nouns are
    // handled, so we include flags for custom test to use those behaviors
    std::bitset<2> m_formulasFlags{ 0 }; // Dale-chall = 0, Harris-Jacobson = 1
    wxString m_testName;
    double m_uniqueUnfamiliarWordCount{ 0 };
    double m_unfamiliarWordCount{ 0 };
    CustomReadabilityTestCollection::iterator m_iter;
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_listViewData{ nullptr };
    };

/** @brief The base class for readability projects. */
class BaseProject : public ProjectRefresh
    {
  public:
    /// @private
    using ProjectTestType = readability::readability_project_test<Wisteria::Data::Dataset>;
    /// @private
    using TestCollectionType = readability::readability_test_collection<ProjectTestType>;

    BaseProject();
    BaseProject(const BaseProject&) = delete;
    BaseProject& operator==(const BaseProject&) = delete;

    virtual ~BaseProject() = default;

    [[nodiscard]]
    const wxString& GetCurrentCustomTest() const
        {
        return m_currentCustTest;
        }

    /// @brief Gets (and constructs, if necessary) the formula parser.
    /// @note This is created on demand on the heap as we don't create this in a
    ///     thin client unless really needed.
    [[nodiscard]]
    ReadabilityFormulaParser& GetFormulaParser()
        {
        if (m_formulaParser == nullptr)
            {
            m_formulaParser = std::make_shared<ReadabilityFormulaParser>(
                this, wxNumberFormatter::GetDecimalSeparator(), FormulaFormat::GetListSeparator());
            }
        return *m_formulaParser;
        }

    void CopySettings(const BaseProject& that);
    void FormatFilteredText(std::wstring& text, const bool romanizeText, const bool removeEllipses,
                            const bool removeBullets, const bool removeFilePaths,
                            const bool stripAbbreviations, const bool narrowFullWithText) const;
    /** @brief Loads a "thin" project where only the statistics, scores, and hard word lists are
       loaded.
        @param path The document's file path.
        @param text The document's text to load.\n
            If this is blank, the @c path will be loaded and analyzed. If this has content,
            then this will override @c path and be used instead.
        @param minWordCount The minimum number of words to be acceptable for this to be a legitimate
            document.\n If the document has fewer words than this then
            LoadingOriginalTextSucceeded() will return @c false and an error will be issued.*/
    bool LoadDocumentAsSubProject(
        const wxString& path,
        const std::wstring&
            text /*if text is already loaded; otherwise, just pass in empty string*/,
        const size_t minWordCount);

    /** @brief Sets the project to "dirty" (i.e., it contains unsaved changes).
        @details This should be implemented by derived documents.\n
            The non UI version of this class simply ignores this.*/
    virtual void SetModifiedFlag() {}

    /** @brief Sets the project's title.
        @details This should be implemented by derived documents.\n
            The non UI version of this class simply ignores this.
        @param title The document title.*/
    virtual void SetDocumentTitle([[maybe_unused]] const wxString& title) {}

    /// If an appended document is being included, then this loads that into the buffer.
    bool LoadAppendedDocument();

    /// loads the extracted text into the indexing engine
    void LoadDocument()
        {
        if (!GetOriginalDocumentFilePath().empty())
            {
            wxLogMessage(L"Analyzing %s", GetOriginalDocumentFilePath());
            }
        CreateWords();
        if (!GetAppendedDocumentText().empty())
            {
            const std::wstring concatenatedText =
                GetDocumentText() + L"\n\f\n" + GetAppendedDocumentText();
            SetTextSize(concatenatedText.length());
            GetWords()->load(concatenatedText.c_str(), concatenatedText.length());
            }
        else
            {
            SetTextSize(GetDocumentText().length());
            GetWords()->load(GetDocumentText().c_str(), GetDocumentText().length());
            }
        /// @todo when we have support for other language spell checkers, then change this
        if (GetProjectLanguage() != readability::test_language::english_test)
            {
            GetWords()->clear_misspelled_words();
            }
        }

    void DeleteWords() { m_words = nullptr; }

    void DeleteUniqueWordMap() { m_word_frequency_map.reset(); }

    [[nodiscard]]
    const std::shared_ptr<CaseInSensitiveNonStemmingDocument>& GetWords() const noexcept
        {
        return m_words;
        }

    [[nodiscard]]
    std::shared_ptr<CaseInSensitiveNonStemmingDocument>& GetWords() noexcept
        {
        return m_words;
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

    [[nodiscard]]
    size_t GetDuplicateWordCount() const noexcept
        {
        return m_duplicateWordCount;
        }

    void SetDuplicateWordCount(const size_t dupCount) noexcept { m_duplicateWordCount = dupCount; }

    [[nodiscard]]
    size_t GetMismatchedArticleCount() const noexcept
        {
        return m_mismatchedArticleCount;
        }

    void SetMismatchedArticleCount(const size_t martCount) noexcept
        {
        m_mismatchedArticleCount = martCount;
        }

    [[nodiscard]]
    size_t GetOverusedWordsBySentenceCount() const noexcept
        {
        return m_overusedWordsBySentenceCount;
        }

    void SetOverusedWordsBySentenceCount(const size_t ovCount) noexcept
        {
        m_overusedWordsBySentenceCount = ovCount;
        }

    [[nodiscard]]
    size_t GetPassiveVoicesCount() const noexcept
        {
        return m_passiveVoiceCount;
        }

    void SetPassiveVoicesCount(const size_t passiveVoiceCount) noexcept
        {
        m_passiveVoiceCount = passiveVoiceCount;
        }

    [[nodiscard]]
    size_t GetMisspelledWordCount() const noexcept
        {
        return m_misspelledWordCount;
        }

    void SetMisspelledWordCount(const size_t MisspelledCount) noexcept
        {
        m_misspelledWordCount = MisspelledCount;
        }

    [[nodiscard]]
    size_t GetWordyPhraseCount() const noexcept
        {
        return m_wordyPhraseCount;
        }

    void SetWordyPhraseCount(const size_t wordyPhraseCount) noexcept
        {
        m_wordyPhraseCount = wordyPhraseCount;
        }

    [[nodiscard]]
    size_t GetRedundantPhraseCount() const noexcept
        {
        return m_redundantPhraseCount;
        }

    void SetRedundantPhraseCount(const size_t count) noexcept { m_redundantPhraseCount = count; }

    [[nodiscard]]
    size_t GetWordingErrorCount() const noexcept
        {
        return m_wordingErrorCount;
        }

    void SetWordingErrorCount(const size_t count) noexcept { m_wordingErrorCount = count; }

    [[nodiscard]]
    size_t GetClicheCount() const noexcept
        {
        return m_clicheCount;
        }

    void SetClicheCount(const size_t clicheCount) noexcept { m_clicheCount = clicheCount; }

    [[nodiscard]]
    size_t GetSentenceStartingWithConjunctionsCount() const noexcept
        {
        return m_sentenceStartingWithConjunctionsCount;
        }

    void SetSentenceStartingWithConjunctionsCount(const size_t count) noexcept
        {
        m_sentenceStartingWithConjunctionsCount = count;
        }

    [[nodiscard]]
    size_t GetSentenceStartingWithLowercaseCount() const noexcept
        {
        return m_sentenceStartingWithLowercaseCount;
        }

    void SetSentenceStartingWithLowercaseCount(const size_t count) noexcept
        {
        m_sentenceStartingWithLowercaseCount = count;
        }

    /** Determines if any test that uses the DC word list is included in the project.*/
    [[nodiscard]]
    bool IsDaleChallLikeTestIncluded() const
        {
        return (GetReadabilityTests().is_test_included(ReadabilityMessages::DALE_CHALL()) ||
                GetReadabilityTests().is_test_included(ReadabilityMessages::PSK_DALE_CHALL()) ||
                GetReadabilityTests().is_test_included(ReadabilityMessages::BORMUTH_CLOZE_MEAN()) ||
                GetReadabilityTests().is_test_included(
                    ReadabilityMessages::BORMUTH_GRADE_PLACEMENT_35()) ||
                GetReadabilityTests().is_test_included(
                    ReadabilityMessages::DEGREES_OF_READING_POWER()) ||
                GetReadabilityTests().is_test_included(
                    ReadabilityMessages::DEGREES_OF_READING_POWER_GE()));
        }

    /** Determines if any SMOG-related test is included in the project.*/
    [[nodiscard]]
    bool IsSmogLikeTestIncluded() const
        {
        return (
            GetReadabilityTests().is_test_included(ReadabilityMessages::SMOG()) ||
            GetReadabilityTests().is_test_included(ReadabilityMessages::SMOG_SIMPLIFIED()) ||
            GetReadabilityTests().is_test_included(ReadabilityMessages::MODIFIED_SMOG()) ||
            GetReadabilityTests().is_test_included(ReadabilityMessages::SMOG_BAMBERGER_VANECEK()));
        }

    /// analysis options
    [[nodiscard]]
    LongSentence GetLongSentenceMethod() const noexcept
        {
        return m_longSentenceMethod;
        }

    void SetLongSentenceMethod(const LongSentence longSentenceMethod) noexcept
        {
        m_longSentenceMethod = longSentenceMethod;
        }

    /// number method
    [[nodiscard]]
    NumeralSyllabize GetNumeralSyllabicationMethod() const noexcept
        {
        return m_numeralSyllabicationMethod;
        }

    void SetNumeralSyllabicationMethod(const NumeralSyllabize numberMethod) noexcept
        {
        m_numeralSyllabicationMethod = numberMethod;
        }

    /// whether to ignore blank lines when figuring out if we are at the end of a paragraph
    [[nodiscard]]
    bool IsIgnoringBlankLinesForParagraphsParser() const noexcept
        {
        return m_ignoreBlankLinesForParagraphsParser;
        }

    void IgnoreBlankLinesForParagraphsParser(const bool ignore) noexcept
        {
        m_ignoreBlankLinesForParagraphsParser = ignore;
        }

    /// whether we should ignore indenting when parsing paragraphs
    [[nodiscard]]
    bool IsIgnoringIndentingForParagraphsParser() const noexcept
        {
        return m_ignoreIndentingForParagraphsParser;
        }

    void IgnoreIndentingForParagraphsParser(const bool ignore) noexcept
        {
        m_ignoreIndentingForParagraphsParser = ignore;
        }

    /// whether the first word of a sentence must be capitalized
    [[nodiscard]]
    bool GetSentenceStartMustBeUppercased() const noexcept
        {
        return m_sentenceStartMustBeUppercased;
        }

    void SetSentenceStartMustBeUppercased(const bool uppercased) noexcept
        {
        m_sentenceStartMustBeUppercased = uppercased;
        }

    /// whether trailing citation paragraphs are getting ignored
    [[nodiscard]]
    bool IsExcludingTrailingCitations() const noexcept
        {
        return m_excludeTrailingCitations;
        }

    void ExcludeTrailingCitations(const bool ignore = true) noexcept
        {
        m_excludeTrailingCitations = ignore;
        }

    /// whether numerals are getting ignored
    [[nodiscard]]
    bool IsExcludingNumerals() const noexcept
        {
        return m_excludeNumerals;
        }

    void ExcludeNumerals(const bool ignore = true) noexcept { m_excludeNumerals = ignore; }

    /// whether Proper Nouns are getting ignored
    [[nodiscard]]
    bool IsExcludingProperNouns() const noexcept
        {
        return m_excludeProperNouns;
        }

    void ExcludeProperNouns(const bool ignore = true) noexcept { m_excludeProperNouns = ignore; }

    /// whether file addresses are getting ignored
    [[nodiscard]]
    bool IsExcludingFileAddresses() const noexcept
        {
        return m_excludeFileAddresses;
        }

    void ExcludeFileAddresses(const bool ignore = true) noexcept
        {
        m_excludeFileAddresses = ignore;
        }

    /// whether to use aggressive list deduction
    [[nodiscard]]
    bool IsExcludingAggressively() const noexcept
        {
        return m_aggressiveExclusion;
        }

    void AggressiveExclusion(const bool aggressive = true) noexcept
        {
        m_aggressiveExclusion = aggressive;
        }

    /// whether trailing copyright/trademark paragraphs are getting ignored
    [[nodiscard]]
    bool IsExcludingTrailingCopyrightNoticeParagraphs() const noexcept
        {
        return m_excludeTrailingCopyrightNoticeParagraphs;
        }

    void ExcludeTrailingCopyrightNoticeParagraphs(const bool ignore = true) noexcept
        {
        m_excludeTrailingCopyrightNoticeParagraphs = ignore;
        }

    /// paragraph parsing
    [[nodiscard]]
    ParagraphParse GetParagraphsParsingMethod() const noexcept
        {
        return m_paragraphsParsingMethod;
        }

    void SetParagraphsParsingMethod(const ParagraphParse parsingMethod) noexcept
        {
        m_paragraphsParsingMethod = parsingMethod;
        }

    /// method to determine how to handle headers and lists
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

    /// document storage/linking information
    [[nodiscard]]
    TextStorage GetDocumentStorageMethod() const noexcept
        {
        return m_documentStorageMethod;
        }

    void SetDocumentStorageMethod(const TextStorage method) noexcept
        {
        m_documentStorageMethod = method;
        }

    /// grammar
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
    bool SpellCheckIsIgnoringSocialMediaTags() const noexcept
        {
        return m_spellcheck_ignore_social_media_tags;
        }

    void SpellCheckIgnoreSocialMediaTags(const bool ignore) noexcept
        {
        m_spellcheck_ignore_social_media_tags = ignore;
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

    /// @returns The text source (was the text from a document or manually entered?).
    [[nodiscard]]
    TextSource GetTextSource() const noexcept
        {
        return m_textSource;
        }

    /// @brief Sets where the text being analyzed is coming from.
    /// @param ts The text source methods.
    void SetTextSource(const TextSource ts) noexcept { m_textSource = ts; }

    /** @returns The original document file path. Not the project path, but the file being analyzed.
        @param index The index into the list of documents.*/
    [[nodiscard]]
    const wxString& GetOriginalDocumentFilePath(const size_t index = 0) const
        {
        if (m_sourceFilePaths.size() <= index)
            {
            // note that we are returning a reference, so that is why we are using
            // an already existing (and persistent) empty string instead of constructing
            // a new one.
            return m_emptyString;
            }
        return m_sourceFilePaths[index].first;
        }

    /** @brief Sets the document path, at a given index.
        @details Standard projects just has one of these (index 0),
            but batch projects will have a list of these.
        @param filePath The path to the document being analyzed.
        @param index The index in the document list to set to this path.
            Document list will be resized if this index is larger than the size of the list.*/
    void SetOriginalDocumentFilePath(const wxString& filePath, const size_t index = 0)
        {
        if (m_sourceFilePaths.size() <= index)
            {
            m_sourceFilePaths.resize(index + 1);
            m_sourceFilePaths[index].first = filePath;
            }
        else
            {
            m_sourceFilePaths[index].first = filePath;
            }
        }

    /** @returns The original document description.
        @param index The index into the list of documents.*/
    [[nodiscard]]
    const wxString& GetOriginalDocumentDescription(const size_t index = 0) const
        {
        if (m_sourceFilePaths.size() <= index)
            {
            return m_emptyString;
            }
        return m_sourceFilePaths[index].second;
        }

    /** Sets the document description, at a given index. Standard projects just has one of these
            (index 0), but batch projects will have a list of these.
        @param description The document description.
        @param index The index in the document list to set to this path.
            Document list will be resized if this index is larger than the size of the list.*/
    void SetOriginalDocumentDescription(const wxString& description, const size_t index = 0)
        {
        if (m_sourceFilePaths.size() <= index)
            {
            m_sourceFilePaths.resize(index + 1);
            m_sourceFilePaths[index].second = description;
            }
        else
            {
            m_sourceFilePaths[index].second = description;
            }
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

    /// @returns The size of the text streamed from the file (or manually entered).
    [[nodiscard]]
    size_t GetTextSize() const noexcept
        {
        return m_textSize;
        }

    void SetTextSize(const size_t size) noexcept { m_textSize = size; }

    [[nodiscard]]
    bool LoadingOriginalTextSucceeded() const noexcept
        {
        return m_loadingOriginalTextSucceeded;
        }

    void SetLoadingOriginalTextSucceeded(const bool succeeded) noexcept
        {
        m_loadingOriginalTextSucceeded = succeeded;
        }

    /// @returns variance method
    [[nodiscard]]
    VarianceMethod GetVarianceMethod() const noexcept
        {
        return m_varianceMethod;
        }

    void SetVarianceMethod(const VarianceMethod method) noexcept { m_varianceMethod = method; }

    /// min doc size
    void SetMinDocWordCountForBatch(const size_t docSize) noexcept
        {
        m_minDocWordCountForBatch = docSize;
        }

    [[nodiscard]]
    size_t GetMinDocWordCountForBatch() const noexcept
        {
        return m_minDocWordCountForBatch;
        }

    [[nodiscard]]
    const double& GetTotalSyllables() const noexcept
        {
        return m_totalSyllables;
        }

    [[nodiscard]]
    const double& GetTotalSyllablesNumeralsFullySyllabized() const noexcept
        {
        return m_totalSyllablesNumeralsFullySyllabized;
        }

    [[nodiscard]]
    const double& GetTotalSyllablesNumeralsOneSyllable() const noexcept
        {
        return m_totalSyllablesNumeralsOneSyllable;
        }

    [[nodiscard]]
    const double& GetTotalCharacters() const noexcept
        {
        return m_totalCharacters;
        }

    [[nodiscard]]
    const double& GetTotalCharactersPlusPunctuation() const noexcept
        {
        return m_totalCharactersPlusPunctuation;
        }

    [[nodiscard]]
    const double& GetTotal3PlusSyllabicWords() const noexcept
        {
        return m_total3plusSyllableWords;
        }

    [[nodiscard]]
    const double& GetTotalMonoSyllabicWords() const noexcept
        {
        return m_totalMonoSyllabic;
        }

    [[nodiscard]]
    const double& GetTotalHardWordsSpache() const noexcept
        {
        return m_totalHardWordsSpache;
        }

    [[nodiscard]]
    const double& GetTotalHardWordsDaleChall() const noexcept
        {
        return m_totalHardWordsDaleChall;
        }

    [[nodiscard]]
    const double& GetTotalHardWordsHarrisJacobson() const noexcept
        {
        return m_totalHardWordsHarrisJacobson;
        }

    [[nodiscard]]
    const double& GetTotalHardWordsFog() const noexcept
        {
        return m_totalHardWordsFog;
        }

    [[nodiscard]]
    const double& GetTotal3PlusSyllabicWordsNumeralsFullySyllabized() const noexcept
        {
        return m_total3PlusSyllabicWordsNumeralsFullySyllabized;
        }

    [[nodiscard]]
    const double& GetTotalHardWordsSol() const noexcept
        {
        return m_totalHardWordsSol;
        }

    [[nodiscard]]
    const double& GetTotalLongWords() const noexcept
        {
        return m_totalLongWords;
        }

    [[nodiscard]]
    const double& GetTotalSixPlusCharacterWordsIgnoringNumerals() const noexcept
        {
        return m_totalSixPlusCharacterWordsIgnoringNumerals;
        }

    [[nodiscard]]
    const double& GetTotalHardLixRixWords() const noexcept
        {
        return m_totalHardWordsLixRix;
        }

    [[nodiscard]]
    const double& GetTotalMiniWords() const noexcept
        {
        return m_totalMiniWords;
        }

    [[nodiscard]]
    const double& GetTotalWords() const noexcept
        {
        return m_totalWords;
        }

    [[nodiscard]]
    const double& GetTotalNumerals() const noexcept
        {
        return m_totalNumerals;
        }

    [[nodiscard]]
    const double& GetTotalProperNouns() const noexcept
        {
        return m_totalProperNouns;
        }

    [[nodiscard]]
    const double& GetTotalOverlyLongSentences() const noexcept
        {
        return m_totalOverlyLongSentences;
        }

    [[nodiscard]]
    const double& GetTotalInterrogativeSentences() const noexcept
        {
        return m_totalInterrogativeSentences;
        }

    [[nodiscard]]
    const double& GetTotalExclamatorySentences() const noexcept
        {
        return m_totalExclamatorySentences;
        }

    [[nodiscard]]
    const double& GetLongestSentence() const noexcept
        {
        return m_longestSentence;
        }

    [[nodiscard]]
    const double& GetLongestSentenceIndex() const noexcept
        {
        return m_longestSentenceIndex;
        }

    [[nodiscard]]
    const double& GetTotalSyllablesIgnoringNumeralsAndProperNouns() const noexcept
        {
        return m_totalSyllablesIgnoringNumeralsAndProperNouns;
        }

    // supplementary stat functions needed for some tests
    [[nodiscard]]
    const double& GetTotalWordsFromCompleteSentencesAndHeaders() const noexcept
        {
        return m_totalWordsFromCompleteSentencesAndHeaders;
        }

    void SetTotalWordsFromCompleteSentencesAndHeaders(const double val) noexcept
        {
        m_totalWordsFromCompleteSentencesAndHeaders = val;
        }

    [[nodiscard]]
    const double& GetTotalSentencesFromCompleteSentencesAndHeaders() const noexcept
        {
        return m_totalSentencesFromCompleteSentencesAndHeaders;
        }

    void SetTotalSentencesFromCompleteSentencesAndHeaders(const double val) noexcept
        {
        m_totalSentencesFromCompleteSentencesAndHeaders = val;
        }

    [[nodiscard]]
    const double& GetTotalNumeralsFromCompleteSentencesAndHeaders() const noexcept
        {
        return m_totalNumeralsFromCompleteSentencesAndHeaders;
        }

    void SetTotalNumeralsFromCompleteSentencesAndHeaders(const double val) noexcept
        {
        m_totalNumeralsFromCompleteSentencesAndHeaders = val;
        }

    [[nodiscard]]
    const double& GetTotalCharactersFromCompleteSentencesAndHeaders() const noexcept
        {
        return m_totalCharactersFromCompleteSentencesAndHeaders;
        }

    void SetTotalCharactersFromCompleteSentencesAndHeaders(const double val) noexcept
        {
        m_totalCharactersFromCompleteSentencesAndHeaders = val;
        }

    // unique value get/set functions
    [[nodiscard]]
    const double& GetTotalUniqueWords() const noexcept
        {
        return m_uniqueWords;
        }

    void SetTotalUniqueWords(const double val) noexcept { m_uniqueWords = val; }

    [[nodiscard]]
    const double& GetTotalUniqueHardWordsSpache() const noexcept
        {
        return m_uniqueSpacheHardWords;
        }

    void SetTotalUniqueHardWordsSpache(const double val) noexcept { m_uniqueSpacheHardWords = val; }

    [[nodiscard]]
    const double& GetTotalUniqueMonoSyllabicWords() const noexcept
        {
        return m_uniqueMonoSyllabicWords;
        }

    void SetTotalUniqueMonoSyllabicWords(const double val) noexcept
        {
        m_uniqueMonoSyllabicWords = val;
        }

    [[nodiscard]]
    const double& GetTotalUnique6CharsPlusWords() const noexcept
        {
        return m_unique6CharsPlusWords;
        }

    void SetTotalUnique6CharsPlusWords(const double val) noexcept { m_unique6CharsPlusWords = val; }

    [[nodiscard]]
    const double& GetTotalUnique3PlusSyllableWords() const noexcept
        {
        return m_unique3PlusSyllableWords;
        }

    void SetTotalUnique3PlusSyllableWords(const double val) noexcept
        {
        m_unique3PlusSyllableWords = val;
        }

    [[nodiscard]]
    const double& GetUnique3PlusSyllabicWordsNumeralsFullySyllabized() const noexcept
        {
        return m_unique3PlusSyllabicWordsNumeralsFullySyllabized;
        }

    void SetUnique3PlusSyllabicWordsNumeralsFullySyllabized(const double val) noexcept
        {
        m_unique3PlusSyllabicWordsNumeralsFullySyllabized = val;
        }

    [[nodiscard]]
    const double& GetTotalUniqueDCHardWords() const noexcept
        {
        return m_uniqueDCHardWords;
        }

    void SetTotalUniqueDCHardWords(const double val) noexcept { m_uniqueDCHardWords = val; }

    [[nodiscard]]
    const double& GetTotalUniqueHarrisJacobsonHardWords() const noexcept
        {
        return m_uniqueHarrisJacobsonHardWords;
        }

    void SetTotalUniqueHarrisJacobsonHardWords(const double val) noexcept
        {
        m_uniqueHarrisJacobsonHardWords = val;
        }

    [[nodiscard]]
    const double& GetTotalUniqueMiniWords() const noexcept
        {
        return m_uniqueMiniWords;
        }

    void SetTotalUniqueMiniWords(const double val) noexcept { m_uniqueMiniWords = val; }

    [[nodiscard]]
    const double& GetTotalUniqueHardFogWords() const noexcept
        {
        return m_uniqueHardFogWords;
        }

    void SetTotalUniqueHardFogWords(const double val) noexcept { m_uniqueHardFogWords = val; }

    [[nodiscard]]
    const double& GetTotalSentences() const noexcept
        {
        return m_totalSentences;
        }

    [[nodiscard]]
    const double& GetTotalSentenceUnits() const noexcept
        {
        return m_totalSentenceUnits;
        }

    [[nodiscard]]
    const double& GetTotalParagraphs() const noexcept
        {
        return m_totalParagraphs;
        }

    [[nodiscard]]
    wxString GetLastTestName() const
        {
        return m_testName;
        }

    [[nodiscard]]
    wxString GetLastGradeLevel() const
        {
        return m_gradeLevel;
        }

    [[nodiscard]]
    wxString GetLastReaderAge() const
        {
        return m_readerAge;
        }

    [[nodiscard]]
    double GetLastIndexScore() const noexcept
        {
        return m_indexScore;
        }

    [[nodiscard]]
    double GetLastClozeScore() const noexcept
        {
        return m_clozeScore;
        }

    /// functions to calculate and include tests
    /// Graphical tests need to be virtual because projects add these differently
    bool AddStandardReadabilityTest(const wxString& id, const bool setFocus = true);
    using AddTestFunction = bool (BaseProject::*)(const bool);
    std::unordered_map<int, AddTestFunction> m_standardTestFunctions;
    bool AddNewDaleChallTest(const bool setFocus = true);
    bool AddSmogTest(const bool setFocus = true);
    bool AddModifiedSmogTest(const bool setFocus = true);
    bool AddSmogSimplifiedTest(const bool setFocus = true);
    bool AddColemanLiauTest(const bool setFocus = true);
    bool AddSpacheTest(const bool setFocus = true);
    bool AddFogTest(const bool setFocus = true);
    bool AddNewFogCountTest(const bool setFocus = true);
    bool AddForcastTest(const bool setFocus = true);
    bool AddFleschTest(const bool setFocus = true);
    bool AddFleschKincaidTest(const bool setFocus = true);
    bool AddFleschKincaidSimplifiedTest(const bool setFocus = true);
    bool AddPskFleschTest(const bool setFocus = true);
    bool AddAriTest(const bool setFocus = true);
    bool AddSimplifiedAriTest(const bool setFocus = true);
    bool AddNewAriTest(const bool setFocus = true);
    bool AddLixTest(const bool setFocus = true);
    bool AddEflawTest(const bool setFocus = true);
    bool AddRixTest(const bool setFocus = true);
    bool AddHarrisJacobsonTest(const bool setFocus = true);
    bool AddPskDaleChallTest(const bool setFocus = true);
    bool AddBormuthClozeMeanTest(const bool setFocus = true);
    bool AddBormuthGradePlacement35Test(const bool setFocus = true);
    bool AddPskFogTest(const bool setFocus = true);
    bool AddFarrJenkinsPatersonTest(const bool setFocus = true);
    bool AddNewFarrJenkinsPatersonTest(const bool setFocus = true);
    bool AddPskFarrJenkinsPatersonTest(const bool setFocus = true);
    bool AddWheelerSmithTest(const bool setFocus = true);
    bool AddCrawfordTest(const bool setFocus = true);
    bool AddSolSpanishTest(const bool setFocus = true);
    bool AddDegreesOfReadingPowerTest(const bool setFocus = true);
    bool AddDegreesOfReadingPowerGeTest(const bool setFocus = true);
    bool AddAmstadTest(const bool setFocus = true);
    bool AddSmogBambergerVanecekTest(const bool setFocus = true);
    bool AddQuBambergerVanecekTest(const bool setFocus = true);
    bool AddWheelerSmithBambergerVanecekTest(const bool setFocus = true);
    bool AddNeueWienerSachtextformel1(const bool setFocus = true);
    bool AddNeueWienerSachtextformel2(const bool setFocus = true);
    bool AddNeueWienerSachtextformel3(const bool setFocus = true);
    bool AddLixGermanChildrensLiterature(const bool setFocus = true);
    bool AddLixGermanTechnical(const bool setFocus = true);
    bool AddRixGermanFiction(const bool setFocus = true);
    bool AddRixGermanNonFiction(const bool setFocus = true);
    bool AddElfTest(const bool setFocus = true);
    bool AddDanielsonBryan1Test(const bool setFocus = true);
    bool AddDanielsonBryan2Test(const bool setFocus = true);
    bool AddInfleszTest(const bool setFocus = true);

    virtual bool AddFryTest(const bool /*setFocus = true, not used here*/)
        {
        return GetReadabilityTests().include_test(ReadabilityMessages::FRY(), true);
        }

    virtual bool AddRaygorTest(const bool /*setFocus = true, not used here*/)
        {
        return GetReadabilityTests().include_test(ReadabilityMessages::RAYGOR(), true);
        }

    virtual bool AddGilliamPenaMountainFryTest(const bool /*setFocus = true, not used here*/)
        {
        return GetReadabilityTests().include_test(ReadabilityMessages::GPM_FRY(), true);
        }

    virtual bool AddFraseTest(const bool /*setFocus = true, not used here*/)
        {
        return GetReadabilityTests().include_test(ReadabilityMessages::FRASE(), true);
        }

    virtual bool AddSchwartzTest(const bool /*setFocus = true, not used here*/)
        {
        return GetReadabilityTests().include_test(ReadabilityMessages::SCHWARTZ(), true);
        }

    /// by default, will not actually calculate the test, it just includes it
    bool AddCustomReadabilityTest(const wxString& name, const bool calculate = false);
    /// this will calculate all the included custom tests
    void AddCustomReadabilityTests();

    /// functions to simply include a test (will not calculate it)
    void IncludeDolchSightWords(const bool include = true) noexcept
        {
        m_includeDolchSightWords = include;
        }

    [[nodiscard]]
    bool IsIncludingDolchSightWords() const noexcept
        {
        return m_includeDolchSightWords;
        }

    /// Will not remove the tests, simply removes their inclusion flag.
    void ExcludeAllTests()
        {
        for (auto& rTest : GetReadabilityTests().get_tests())
            {
            rTest.include(false);
            }
        m_includeDolchSightWords = false;
        ExcludeAllCustomTestsTests();
        }

    /// Will not remove the custom tests, simply removes their inclusion flag.
    virtual void ExcludeAllCustomTestsTests() { m_customTestsInUse.clear(); }

    bool RemoveTest(const wxString& name);

    /// reload functions
    [[nodiscard]]
    bool IsProcessing() const noexcept
        {
        return m_isRefreshing;
        }

    void SetProcessing(const bool processing = true) noexcept { m_isRefreshing = processing; }

    virtual void RefreshProject() {}

    void SetUIMode(const bool hasUI) noexcept { m_hasUI = hasUI; }

    [[nodiscard]]
    bool HasUI() const noexcept
        {
        return m_hasUI;
        }

    /// @brief Logs a message to multiple outputs.
    /// @details If there is no UI attached to the project,
    ///     then queue a message to be handled by the caller later.
    ///     If there is a UI, then display the message in a message box right away.\n
    ///     The message will also be sent to the logging system, using the icon to
    ///     determine which logging level to use.
    /// @param message The message to show to the user.
    /// @param title The title for the message box (only used if there is a UI involved).
    /// @param icon The icon to be shown on the message box.
    ///     If no UI, this can be used to show the severity of the message (e.g.,
    ///     wxICON_EXCLAMATION).
    /// @param messageId If using the WarningMessage system,
    ///     then this parameter should specify the ID of the warning to look up.
    /// @param queue Whether the message should be queued for showing later.
    ///     It will not be shown until ShowQueuedMessages() is called.
    void LogMessage(wxString message, const wxString& title, const int icon,
                    const wxString& messageId = wxString{}, const bool queue = false);

    void LogMessage(const WarningMessage& message, const bool queue = false)
        {
        // in case this is something that the user asked to be suppressed
        if (message.ShouldBeShown())
            {
            LogMessage(message.GetMessage(), message.GetTitle(), message.GetFlags(),
                       message.GetId(), queue);
            }
        }

    /// @brief Clear the queued list of error/warning messages encountered while loading.
    void ClearSubProjectMessages() { m_messages.clear(); }

    /// @returns The collection of messages queued while loading documents and whatnot.
    [[nodiscard]]
    const std::vector<WarningMessage>& GetSubProjectMessages() const
        {
        return m_messages;
        }

    /// Adds a message that will be added to a list, where the caller will be responsible
    /// for showing them via GetMessages().
    /// Normally, LogMessage should be called unless you need to explicitly
    /// not show the message right away.
    void AddQuietSubProjectMessage(const wxString& message, const int icon)
        {
        m_messages.emplace_back(wxString{}, message, wxString{}, wxString{}, icon);
        }

    /// @return the messages that won't be shown until client asks from them to be shown.
    [[nodiscard]]
    const std::vector<WarningMessage>& GetQueuedMessages() const noexcept
        {
        return m_queuedMessages;
        }

    /// @brief Saves a message to be shown later, when ShowQueuedMessages() is called.
    /// @param message The message to queue.
    void AddQueuedMessage(const WarningMessage& message) { m_queuedMessages.push_back(message); }

    /// @brief Clears the queued-up messages.
    void ClearQueuedMessages() noexcept { m_queuedMessages.clear(); }

    /// @brief Shows all the queued-up messages and then clears them.
    /// @note Should be implemented by caller, as this subsystem has no UI.
    virtual void ShowQueuedMessages() {}

    [[nodiscard]]
    bool HasCustomTest(const wxString& testName) const
        {
        return std::ranges::any_of(m_customTestsInUse,
                                   [&](const auto& pos)
                                   {
                                       // Can't use the interface's internal iterator's comparison
                                       // because it might be out of sync when this is called.
                                       // We need to do the comparison ourselves, so just make
                                       // sure that this is the same type of comparison done by the
                                       // iterator.
                                       return pos.GetTestName().CmpNoCase(testName) == 0;
                                   });
        }

    /// @note Call HasCustomTest() before calling this to verify that the test will be available.
    [[nodiscard]]
    std::vector<CustomReadabilityTestInterface>::iterator GetCustomTest(const wxString& testName)
        {
        for (auto pos = m_customTestsInUse.begin(); pos != m_customTestsInUse.end(); ++pos)
            {
            // can't use the interface's internal iterator's comparison because it might be out of
            // sync when this is called. We need to do the comparison ourselves, so just make be
            // sure that this is the same type of comparison done by the iterator.
            if (pos->GetTestName().CmpNoCase(testName) == 0)
                {
                return pos;
                }
            }
        return m_customTestsInUse.end();
        }

    /// @note Call HasCustomTest() before calling this to verify that the test will be available.
    [[nodiscard]]
    std::vector<CustomReadabilityTestInterface>::const_iterator
    GetCustomTest(const wxString& testName) const
        {
        for (auto pos = m_customTestsInUse.cbegin(); pos != m_customTestsInUse.cend(); ++pos)
            {
            // can't use the interface's internal iterator's comparison because it might be out of
            // sync when this is called. We need to do the comparison ourselves, so just make be
            // sure that this is the same type of comparison done by the iterator.
            if (pos->GetTestName().CmpNoCase(testName) == 0)
                {
                return pos;
                }
            }
        return m_customTestsInUse.end();
        }

    void SyncCustomTests();

    [[nodiscard]]
    const std::vector<CustomReadabilityTestInterface>& GetCustTestsInUse() const noexcept
        {
        return m_customTestsInUse;
        }

    [[nodiscard]]
    std::vector<CustomReadabilityTestInterface>& GetCustTestsInUse() noexcept
        {
        return m_customTestsInUse;
        }

    /// subclass might want to implement its own version of this to remove test from its interface
    virtual std::vector<CustomReadabilityTestInterface>::iterator
    RemoveCustomReadabilityTest(const wxString& testName, [[maybe_unused]] const int Id);

    void SetDocumentText(std::wstring text) { m_documentContent = std::move(text); }

    [[nodiscard]]
    const std::wstring& GetDocumentText() const noexcept
        {
        return m_documentContent;
        }

    [[nodiscard]]
    std::wstring& GetDocumentText()
        {
        return m_documentContent;
        }

    void FreeDocumentText()
        {
        m_documentContent.clear();
        m_documentContent.shrink_to_fit();
        }

    void SetAppendedDocumentText(std::wstring text) { m_appendedDocumentContent = std::move(text); }

    [[nodiscard]]
    const std::wstring& GetAppendedDocumentText() const noexcept
        {
        return m_appendedDocumentContent;
        }

    /** @returns A @c true value and raw/encoded text converted into a
            filtered string on success, @c false and empty string otherwise.
        @param sourceFileText The encoded text to filter.
        @param fileExtension The file's extension.*/
    [[nodiscard]]
    std::pair<bool, std::wstring> ExtractRawText(std::string_view sourceFileText,
                                                 const wxString& fileExtension);

    [[nodiscard]]
    const double& GetUnusedDolchConjunctions() const noexcept
        {
        return m_unusedDolchConjunctions;
        }

    void SetUnusedDolchConjunctions(const double val) noexcept { m_unusedDolchConjunctions = val; }

    [[nodiscard]]
    const double& GetUnusedDolchPrepositions() const noexcept
        {
        return m_unusedDolchPrepositions;
        }

    void SetUnusedDolchPrepositions(const double val) noexcept { m_unusedDolchPrepositions = val; }

    [[nodiscard]]
    const double& GetUnusedDolchPronouns() const noexcept
        {
        return m_unusedDolchPronouns;
        }

    void SetUnusedDolchPronouns(const double val) noexcept { m_unusedDolchPronouns = val; }

    [[nodiscard]]
    const double& GetUnusedDolchAdverbs() const noexcept
        {
        return m_unusedDolchAdverbs;
        }

    void SetUnusedDolchAdverbs(const double val) noexcept { m_unusedDolchAdverbs = val; }

    [[nodiscard]]
    const double& GetUnusedDolchAdjectives() const noexcept
        {
        return m_unusedDolchAdjectives;
        }

    void SetUnusedDolchAdjectives(const double val) noexcept { m_unusedDolchAdjectives = val; }

    [[nodiscard]]
    const double& GetUnusedDolchVerbs() const noexcept
        {
        return m_unusedDolchVerbs;
        }

    void SetUnusedDolchVerbs(const double val) noexcept { m_unusedDolchVerbs = val; }

    [[nodiscard]]
    const double& GetUnusedDolchNouns() const noexcept
        {
        return m_unusedDolchNounsWords;
        }

    void SetUnusedDolchNouns(const double val) noexcept { m_unusedDolchNounsWords = val; }

    [[nodiscard]]
    std::pair<size_t, size_t> GetDolchConjunctionCounts() const noexcept
        {
        return m_dolchConjunctionCounts;
        }

    [[nodiscard]]
    std::pair<size_t, size_t> GetDolchPrepositionWordCounts() const noexcept
        {
        return m_dolchPrepositionCounts;
        }

    [[nodiscard]]
    std::pair<size_t, size_t> GetDolchPronounCounts() const noexcept
        {
        return m_dolchPronounCounts;
        }

    [[nodiscard]]
    std::pair<size_t, size_t> GetDolchAdverbCounts() const noexcept
        {
        return m_dolchAdverbCounts;
        }

    [[nodiscard]]
    std::pair<size_t, size_t> GetDolchAdjectiveCounts() const noexcept
        {
        return m_dolchAdjectiveCounts;
        }

    [[nodiscard]]
    std::pair<size_t, size_t> GetDolchVerbsCounts() const noexcept
        {
        return m_dolchVerbCounts;
        }

    [[nodiscard]]
    std::pair<size_t, size_t> GetDolchNounCounts() const noexcept
        {
        return m_dolchNounCounts;
        }

    /// @returns The total number of standard tests (and Dolch) available in the system.
    [[nodiscard]]
    size_t GetStandardTestCount() const noexcept
        {
        return GetReadabilityTests().get_tests().size() + 1 /*Dolch*/;
        }

    /// @returns The number of custom tests.
    [[nodiscard]]
    static size_t GetCustomTestCount() noexcept
        {
        return m_custom_word_tests.size();
        }

    /// @returns The number of standard tests that can have multiple results
    ///     (e.g., a grade level and predicted cloze score).
    [[nodiscard]]
    size_t GetMultiResultTestCount() const noexcept
        {
        size_t testCount(0);
        for (const auto& test : GetReadabilityTests().get_tests())
            {
            if (test.get_test().get_test_type() ==
                    readability::readability_test_type::index_value_and_grade_level ||
                test.get_test().get_test_type() ==
                    readability::readability_test_type::grade_level_and_predicted_cloze_score)
                {
                ++testCount;
                }
            }
        return testCount;
        }

    [[nodiscard]]
    const std::vector<comparable_first_pair<wxString, wxString>>&
    GetSourceFilesInfo() const noexcept
        {
        return m_sourceFilePaths;
        }

    [[nodiscard]]
    std::vector<comparable_first_pair<wxString, wxString>>& GetSourceFilesInfo() noexcept
        {
        return m_sourceFilePaths;
        }

    void SetReadabilityTests(const TestCollectionType& that) { m_readabilityTests = that; }

    [[nodiscard]]
    TestCollectionType& GetReadabilityTests() noexcept
        {
        return m_readabilityTests;
        }

    [[nodiscard]]
    const TestCollectionType& GetReadabilityTests() const noexcept
        {
        return m_readabilityTests;
        }

    static void InitializeStandardReadabilityTests();
    static void InitializeStandardEnglishFamiliarWordReadabilityTests();
    static void InitializeStandardEnglishGraphReadabilityTests();
    static void InitializeStandardEnglishRegressionReadabilityTests();
    static void InitializeStandardSpanishReadabilityTests();
    static void InitializeStandardGermanReadabilityTests();
    static void ResetStandardReadabilityTests(TestCollectionType& readabilityTests);

    [[nodiscard]]
    static const readability::readability_test_collection<readability::readability_test>&
    GetDefaultReadabilityTestsTemplate() noexcept
        {
        return m_defaultReadabilityTestsTemplate;
        }

    [[nodiscard]]
    const ReadabilityMessages& GetReadabilityMessageCatalog() const noexcept
        {
        return m_readMessages;
        }

    [[nodiscard]]
    ReadabilityMessages& GetReadabilityMessageCatalog() noexcept
        {
        return m_readMessages;
        }

    [[nodiscard]]
    ReadabilityMessages* GetReadabilityMessageCatalogPtr() noexcept
        {
        return &m_readMessages;
        }

    // test options
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
    SpecializedTestTextExclusion GetDaleChallTextExclusionMode() const noexcept
        {
        return m_dcTextExclusion;
        }

    void SetDaleChallTextExclusionMode(const SpecializedTestTextExclusion mode) noexcept
        {
        m_dcTextExclusion = mode;
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

    void SetProjectLanguage(const readability::test_language lang) noexcept { m_language = lang; }

    [[nodiscard]]
    readability::test_language GetProjectLanguage() const noexcept
        {
        return m_language;
        }

    /// @returns The test goals in the project.
    [[nodiscard]]
    const std::set<TestGoal>& GetTestGoals() const noexcept
        {
        return m_testGoals;
        }

    /// @returns The test goals in the project.
    [[nodiscard]]
    std::set<TestGoal>& GetTestGoals() noexcept
        {
        return m_testGoals;
        }

    /// Sets the test goals.
    /// @param goals The goals to copy overs.
    void SetTestGoals(const std::set<TestGoal>& goals)
        {
        m_testGoals.clear();
        // just copy in test goals that are valid
        for (const auto& goal : goals)
            {
            if (goal.HasGoals())
                {
                m_testGoals.emplace(goal);
                }
            }
        }

    /// @returns The stat goals in the project.
    [[nodiscard]]
    const std::set<StatGoal>& GetStatGoals() const noexcept
        {
        return m_statGoals;
        }

    /// @returns The stat goals in the project.
    [[nodiscard]]
    std::set<StatGoal>& GetStatGoals() noexcept
        {
        return m_statGoals;
        }

    /// Sets the stat goals.
    /// @param goals The goals to copy overs.
    void SetStatGoals(const std::set<StatGoal>& goals)
        {
        m_statGoals.clear();
        // just copy in stat goals that are valid
        for (const auto& goal : goals)
            {
            if (goal.HasGoals())
                {
                m_statGoals.emplace(goal);
                }
            }
        }

    /// @returns The list of statistics goals available to choose from.
    [[nodiscard]]
    static const auto& GetStatGoalLabels() noexcept
        {
        return m_statGoalLabels;
        }

    /// Sees if a given test has goal constraints specified, and if so,
    /// whether the test's result meets those constraints.
    /// @param testName The name of the test to review.
    /// @param score The test's score, which will be compared to its goals.
    /// @returns @c true if the test was found to have a goal.
    bool ReviewTestGoal(const wxString& testName, const double score);

    /// Sees if a given statistic has goal constraints specified, and if so,
    /// whether the statistic's result meets those constraints.
    /// @param statName The name of the statistic to review.
    /// @param value The statistic's value, which will be compared to its goals.
    /// @returns @c true if the statistic was found to have a goal.
    bool ReviewStatGoal(const wxString& statName, const double value);

    /// @brief Reviews all possible stats goals in the project.
    void ReviewStatGoals();

    void SetReviewer(const wxString& reviewer) { m_reviewer = reviewer; }

    [[nodiscard]]
    const wxString& GetReviewer() const noexcept
        {
        return m_reviewer;
        }

    void SetStatus(const wxString& status) { m_status = status; }

    /// @returns The status label of the project.
    [[nodiscard]]
    const wxString& GetStatus() const noexcept
        {
        return m_status;
        }

    [[nodiscard]]
    const std::vector<double>& GetAggregatedGradeScores() const noexcept
        {
        return m_aggregatedGradeScores;
        }

    [[nodiscard]]
    std::vector<double>& GetAggregatedGradeScores() noexcept
        {
        return m_aggregatedGradeScores;
        }

    [[nodiscard]]
    const std::vector<double>& GetAggregatedClozeScores() const noexcept
        {
        return m_aggregatedClozeScores;
        }

    [[nodiscard]]
    std::vector<double>& GetAggregatedClozeScores() noexcept
        {
        return m_aggregatedClozeScores;
        }

    /// @returns Whether a test is included which includes a cloze score.
    [[nodiscard]]
    bool IsIncludingClozeTest() const;
    /// @returns Whether a test is included which includes a grade-level score.
    [[nodiscard]]
    bool IsIncludingGradeTest() const;

    virtual void RemoveMisspellings([[maybe_unused]] const wxArrayString& misspellings) {}

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& Get3SyllablePlusData() const
        {
        return m_3SybPlusData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& Get3SyllablePlusData()
        {
        return m_3SybPlusData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    Get6CharacterPlusData() const
        {
        return m_6CharPlusData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& Get6CharacterPlusData()
        {
        return m_6CharPlusData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetDaleChallHardWordData() const
        {
        return m_DCHardWordsData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetDaleChallHardWordData()
        {
        return m_DCHardWordsData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetSpacheHardWordData() const
        {
        return m_SpacheHardWordsData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetSpacheHardWordData()
        {
        return m_SpacheHardWordsData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetHarrisJacobsonHardWordDataData() const
        {
        return m_harrisJacobsonHardWordsData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetHarrisJacobsonHardWordDataData()
        {
        return m_harrisJacobsonHardWordsData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider>& GetUnusedDolchWordData() const
        {
        return m_unusedDolchWordsData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider>& GetUnusedDolchWordData()
        {
        return m_unusedDolchWordsData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetDolchWordData() const
        {
        return m_dolchWordsData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetDolchWordData()
        {
        return m_dolchWordsData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetNonDolchWordData() const
        {
        return m_nonDolchWordsData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetNonDolchWordData()
        {
        return m_nonDolchWordsData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetKeyWordsBaseData()
        {
        return m_keyWordsBaseData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetKeyWordsBaseData() const
        {
        return m_keyWordsBaseData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetAllWordsBaseData()
        {
        return m_AllWordsBaseData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetAllWordsBaseData() const
        {
        return m_AllWordsBaseData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetProperNounsData()
        {
        return m_ProperNounsData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetProperNounsData() const
        {
        return m_ProperNounsData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetContractionsData()
        {
        return m_contractionsData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetContractionsData() const
        {
        return m_contractionsData;
        }

    [[nodiscard]]
    wxString GetExcludedPhrasesPath() const
        {
        return m_excludedPhrasesPath;
        }

    void SetExcludedPhrasesPath(const wxString& path) { m_excludedPhrasesPath = path; }

    /** Reload the excluded phrases. Note that this should not be done from
        SetExcludedPhrasesPath(), that function should only set the path, you need
        to call this function explicitly after setting the new path. This is because
        a Batch project shares its phrase list with its subprojects and subprojects
        should not call this function. Instead, they simply have their filepath to the
        phrase list set and the parent batch project shares its phrase list.*/
    void LoadExcludePhrases()
        {
        DeleteExcludedPhrases();
        m_excluded_phrases = std::make_shared<grammar::phrase_collection>();
        if (!GetExcludedPhrasesPath().empty())
            {
            wxString phrases, filePath(GetExcludedPhrasesPath()), fileBySameNameInProjectDirectory;
            // if file not found, then try to search for it in the subdirectories
            // from where the project is
            if (!wxFile::Exists(filePath) &&
                FindMissingFile(filePath, fileBySameNameInProjectDirectory))
                {
                filePath = fileBySameNameInProjectDirectory;
                }
            if (Wisteria::TextStream::ReadFile(filePath, phrases))
                {
                // path may have been changed from ReadFile()
                if (CompareFilePaths(GetExcludedPhrasesPath(), filePath) != 0)
                    {
                    SetModifiedFlag();
                    SetExcludedPhrasesPath(filePath);
                    }
                m_excluded_phrases->load_phrases(phrases, true, false);
                }
            else
                {
                LogMessage(wxString::Format(_(L"Exclusion phrase list not found:\n\n%s\n\nList "
                                              "will not be included in this project."),
                                            filePath),
                           _(L"Warning"), wxOK | wxICON_EXCLAMATION);
                SetModifiedFlag();
                SetExcludedPhrasesPath(wxString{});
                }
            }
        }

    void ShareExcludePhrases(const BaseProject& that) noexcept
        {
        m_excluded_phrases = that.m_excluded_phrases;
        }

    // Tags for excluding blocks of text
    [[nodiscard]]
    const std::vector<std::pair<wchar_t, wchar_t>>& GetExclusionBlockTags() const noexcept
        {
        return m_exclusionBlockTags;
        }

    [[nodiscard]]
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

    [[nodiscard]]
    StatisticsReportInfo& GetStatisticsReportInfo() noexcept
        {
        return m_statsReportInfo;
        }

    [[nodiscard]]
    const StatisticsReportInfo& GetStatisticsReportInfo() const noexcept
        {
        return m_statsReportInfo;
        }

    [[nodiscard]]
    StatisticsInfo& GetStatisticsInfo() noexcept
        {
        return m_statsInfo;
        }

    [[nodiscard]]
    const StatisticsInfo& GetStatisticsInfo() const noexcept
        {
        return m_statsInfo;
        }

    [[nodiscard]]
    GrammarInfo& GetGrammarInfo() noexcept
        {
        return m_grammarInfo;
        }

    [[nodiscard]]
    const GrammarInfo& GetGrammarInfo() const noexcept
        {
        return m_grammarInfo;
        }

    [[nodiscard]]
    WordsBreakdownInfo& GetWordsBreakdownInfo() noexcept
        {
        return m_wordsBreakdownInfo;
        }

    [[nodiscard]]
    const WordsBreakdownInfo& GetWordsBreakdownInfo() const noexcept
        {
        return m_wordsBreakdownInfo;
        }

    [[nodiscard]]
    SentencesBreakdownInfo& GetSentencesBreakdownInfo() noexcept
        {
        return m_sentencesBreakdownInfo;
        }

    [[nodiscard]]
    const SentencesBreakdownInfo& GetSentencesBreakdownInfo() const noexcept
        {
        return m_sentencesBreakdownInfo;
        }

    [[nodiscard]]
    const std::shared_ptr<double_frequency_set<word_case_insensitive_no_stem>>&
    GetWordsWithFrequencies() const noexcept
        {
        return m_word_frequency_map;
        }

    /// @returns The stop list, based on the project's language.
    [[nodiscard]]
    const word_list& GetStopList() const noexcept
        {
        return (GetProjectLanguage() == readability::test_language::spanish_test) ?
                   spanish_stoplist :
               (GetProjectLanguage() == readability::test_language::german_test) ? german_stoplist :
                                                                                   english_stoplist;
        }

    static grammar::phrase_collection m_englishWordyPhrases;
    static grammar::phrase_collection m_spanishWordyPhrases;
    static grammar::phrase_collection m_germanWordyPhrases;
    static grammar::phrase_collection m_copyrightNoticePhrases;
    static grammar::phrase_collection m_citationPhrases;
    static word_list m_knownProperNouns;
    static word_list m_knownPersonalNouns;
    static word_list known_english_spellings;
    static word_list known_programming_spellings;
    static word_list known_custom_english_spellings;
    static word_list known_spanish_spellings;
    static word_list known_custom_spanish_spellings;
    static word_list known_german_spellings;
    static word_list known_custom_german_spellings;
    static word_list m_dale_chall_word_list;
    static word_list m_dale_chall_plus_stocker_catholic_word_list;
    static word_list m_stocker_catholic_word_list;
    static word_list m_spache_word_list;
    static word_list m_harris_jacobson_word_list;
    static word_list english_stoplist;
    static word_list spanish_stoplist;
    static word_list german_stoplist;
    static word_list_with_replacements dale_chall_replacement_list;
    static word_list_with_replacements spache_replacement_list;
    static word_list_with_replacements harris_jacobson_replacement_list;
    static word_list_with_replacements difficult_word_replacement_list;
    static readability::dolch_word_list m_dolch_word_list;
    static std::set<TestBundle> m_testBundles;
    static CustomReadabilityTestCollection m_custom_word_tests;

  private:
    void CreateWords()
        {
        DeleteWords();
        // UpdateDocumentSettings() will set these parameters properly
        m_words = std::make_shared<CaseInSensitiveNonStemmingDocument>(
            L"", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
            nullptr, nullptr, nullptr);
        UpdateDocumentSettings();
        }

    void UpdateDocumentSettings();

    [[nodiscard]]
    std::pair<bool, std::wstring> ExtractRtfRawText(std::string_view sourceFileText);
    [[nodiscard]]
    std::pair<bool, std::wstring> ExtractPowerPointRawText(std::string_view sourceFileText);
    [[nodiscard]]
    std::pair<bool, std::wstring> ExtractDocxRawText(std::string_view sourceFileText);
    [[nodiscard]]
    std::pair<bool, std::wstring> ExtractDocRawText(std::string_view sourceFileText);
    [[nodiscard]]
    std::pair<bool, std::wstring> ExtractOpenDocumentRawText(std::string_view sourceFileText);
    [[nodiscard]]
    std::pair<bool, std::wstring> ExtractPostscriptRawText(std::string_view sourceFileText);
    [[nodiscard]]
    std::pair<bool, std::wstring> ExtractHtmlRawText(std::string_view sourceFileText,
                                                     const WebPageExtension& isHtmlExtension,
                                                     const wxString& fileExtension);
    [[nodiscard]]
    std::pair<bool, std::wstring> ExtractWorkshopRawText(std::string_view sourceFileText);
    [[nodiscard]]
    std::pair<bool, std::wstring> ExtractIdlRawText(std::string_view sourceFileText);
    [[nodiscard]]
    std::pair<bool, std::wstring> ExtractCppRawText(std::string_view sourceFileText);
    [[nodiscard]]
    std::pair<bool, std::wstring> ExtractMarkdownRawText(std::string_view sourceFileText);

  protected:
    [[nodiscard]]
    std::shared_ptr<stemming::stem<traits::case_insensitive_wstring_ex>> CreateStemmer() const
        {
        if (GetProjectLanguage() == readability::test_language::english_test)
            {
            return std::make_shared<stemming::english_stem<traits::case_insensitive_wstring_ex>>();
            }
        if (GetProjectLanguage() == readability::test_language::spanish_test)
            {
            return std::make_shared<stemming::spanish_stem<traits::case_insensitive_wstring_ex>>();
            }
        if (GetProjectLanguage() == readability::test_language::german_test)
            {
            return std::make_shared<stemming::german_stem<traits::case_insensitive_wstring_ex>>();
            }
        return std::make_shared<stemming::no_op_stem<traits::case_insensitive_wstring_ex>>();
        }

    void SetCurrentCustomTest(const wxString& test) { m_currentCustTest = test; }

    void DeleteExcludedPhrases() { m_excluded_phrases.reset(); }

    [[nodiscard]]
    bool VerifyTestBeforeAdding(
        const std::pair<std::vector<ProjectTestType>::const_iterator, bool>& theTest);

    [[nodiscard]]
    bool VerifyTotalWordsForTest(const wxString& currentTestKey)
        {
        if (GetTotalWords() == 0)
            {
            LogMessage(
                wxString::Format(
                    _(L"Unable to calculate %s: at least one word must be present in document."),
                    GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
                _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
            GetReadabilityTests().include_test(currentTestKey, false);
            return false;
            }
        return true;
        }

    [[nodiscard]]
    bool VerifyTotalSentencesForTest(const wxString& currentTestKey)
        {
        if (GetTotalSentences() == 0)
            {
            LogMessage(
                wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present "
                                   L"in document."),
                                 GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
                _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
            GetReadabilityTests().include_test(currentTestKey, false);
            return false;
            }
        return true;
        }

    void HandleFailedTestCalculation(const wxString& testName);

    [[nodiscard]]
    bool FindMissingFile(const wxString& filePath, wxString& fileBySameNameInProjectDirectory);

    // only AddDB2Test should call this
    virtual void AddDB2Plot([[maybe_unused]] const bool setFocus) {}

    // only AddLixGermanTest should call this
    virtual void AddLixGermanGauge([[maybe_unused]] const bool setFocus) {}

    // only AddLixTest should call this
    virtual void AddLixGauge([[maybe_unused]] const bool setFocus) {}

    // only AddFleschTest should call this
    virtual void AddFleschChart([[maybe_unused]] const bool setFocus) {}

    // only AddCrawfordTest should call this
    virtual void AddCrawfordGraph([[maybe_unused]] const bool setFocus) {}

    // only AddInfleszTest should call this
    virtual void AddInfleszGraph([[maybe_unused]] const bool setFocus) {}

    /** @returns @c true value and raw/encoded text (usually HTML) converted into a filtered string
            on success, @c false and empty string otherwise.
        @param sourceFileText The encoded text to filter.
        @param fileExtension The file's extension.
        @param fileName The pathway to the file (only used for error logging).
        @param[out] label A descriptive label from the HTML file (e.g., title, subject, keywords).*/
    [[nodiscard]]
    static std::pair<bool, std::wstring>
    ExtractRawTextWithEncoding(const std::wstring& sourceFileText, const wxString& fileExtension,
                               const wxFileName& fileName, wxString& label);
    void CalculateStatistics();
    void CalculateStatisticsIgnoringInvalidSentences();
    void LoadHardWords();
    [[nodiscard]]
    bool LoadExternalDocument();

    void ClearReadabilityTestResult()
        {
        m_testName.clear();
        m_gradeLevel.clear();
        m_readerAge.clear();
        m_indexScore = std::numeric_limits<double>::quiet_NaN();
        m_clozeScore = std::numeric_limits<double>::quiet_NaN();
        }

    virtual void SetReadabilityTestResult(const wxString& testId, const wxString& testName,
                                          [[maybe_unused]] const wxString& description,
                                          // score and display label
                                          const std::pair<double, wxString>& USGradeLevel,
                                          const wxString& readerAge, const double indexScore,
                                          const double clozeScore,
                                          [[maybe_unused]] const bool setFocus)
        {
        m_testName = testName;
        m_gradeLevel = USGradeLevel.second;
        m_readerAge = readerAge;
        m_indexScore = round_decimal_place(indexScore, 10);
        m_clozeScore = round_to_integer(clozeScore);

        // see if the test has any goal issues
        ReviewTestGoal(testId, !std::isnan(USGradeLevel.first) ? USGradeLevel.first :
                               !std::isnan(indexScore)         ? indexScore :
                               !std::isnan(clozeScore)         ? clozeScore :
                                                         std::numeric_limits<double>::quiet_NaN());
        }

    /// @returns The directory of the saved project. Must be overridden by derived classes.
    [[nodiscard]]
    virtual wxString GetProjectDirectory() const
        {
        return m_projectDirectory;
        }

    void SetProjectDirectory(const wxString& projectDir) { m_projectDirectory = projectDir; }

    [[nodiscard]]
    static wxString GetWordsColumnName()
        {
        return _DT(L"WORDS");
        }

    [[nodiscard]]
    static wxString GetWordsCountsColumnName()
        {
        return L"WORD_COUNTS";
        }

    // dataset for word cloud
    std::shared_ptr<Wisteria::Data::Dataset> m_keyWordsDataset{ nullptr };

    // unique word count and total count for each category
    std::pair<size_t, size_t> m_dolchConjunctionCounts{ 0, 0 };
    std::pair<size_t, size_t> m_dolchPrepositionCounts{ 0, 0 };
    std::pair<size_t, size_t> m_dolchPronounCounts{ 0, 0 };
    std::pair<size_t, size_t> m_dolchAdverbCounts{ 0, 0 };
    std::pair<size_t, size_t> m_dolchAdjectiveCounts{ 0, 0 };
    std::pair<size_t, size_t> m_dolchVerbCounts{ 0, 0 };
    std::pair<size_t, size_t> m_dolchNounCounts{ 0, 0 };

    // Statistics values
    // totals
    double m_totalWords{ 0 };
    double m_totalSentences{ 0 };
    double m_totalSentenceUnits{ 0 };
    double m_totalParagraphs{ 0 };
    double m_totalSyllables{ 0 };
    double m_totalSyllablesNumeralsFullySyllabized{ 0 };
    double m_totalSyllablesNumeralsOneSyllable{ 0 };
    double m_totalCharacters{ 0 };
    double m_totalCharactersPlusPunctuation{ 0 };
    double m_totalMonoSyllabic{ 0 };
    double m_total3plusSyllableWords{ 0 };
    double m_total3PlusSyllabicWordsNumeralsFullySyllabized{ 0 };
    double m_totalHardWordsSpache{ 0 };
    double m_totalHardWordsDaleChall{ 0 };
    double m_totalHardWordsHarrisJacobson{ 0 };
    double m_totalHardWordsFog{ 0 };
    double m_totalHardWordsSol{ 0 };
    double m_totalHardWordsLixRix{ 0 };
    double m_totalSyllablesIgnoringNumerals{ 0 };
    double m_totalSyllablesIgnoringNumeralsAndProperNouns{ 0 };
    double m_totalLongWords{ 0 };
    double m_totalSixPlusCharacterWordsIgnoringNumerals{ 0 };
    double m_totalMiniWords{ 0 };
    double m_totalNumerals{ 0 };
    double m_totalProperNouns{ 0 };
    double m_totalOverlyLongSentences{ 0 };
    double m_totalInterrogativeSentences{ 0 };
    double m_totalExclamatorySentences{ 0 };
    double m_longestSentence{ 0 };
    double m_longestSentenceIndex{ 0 };

  private:
    // extra stats needed for some tests
    double m_totalWordsFromCompleteSentencesAndHeaders{ 0 };
    double m_totalSentencesFromCompleteSentencesAndHeaders{ 0 };
    double m_totalNumeralsFromCompleteSentencesAndHeaders{ 0 };
    double m_totalCharactersFromCompleteSentencesAndHeaders{ 0 };

    // Dolch stats
    double m_unusedDolchConjunctions{ 0 };
    double m_unusedDolchPrepositions{ 0 };
    double m_unusedDolchPronouns{ 0 };
    double m_unusedDolchAdverbs{ 0 };
    double m_unusedDolchAdjectives{ 0 };
    double m_unusedDolchVerbs{ 0 };
    double m_unusedDolchNounsWords{ 0 };

    // grammar stats
    double m_duplicateWordCount{ 0 };
    double m_mismatchedArticleCount{ 0 };
    double m_passiveVoiceCount{ 0 };
    double m_misspelledWordCount{ 0 };
    double m_wordyPhraseCount{ 0 };
    double m_redundantPhraseCount{ 0 };
    double m_overusedWordsBySentenceCount{ 0 };
    double m_wordingErrorCount{ 0 };
    double m_clicheCount{ 0 };
    double m_sentenceStartingWithConjunctionsCount{ 0 };
    double m_sentenceStartingWithLowercaseCount{ 0 };

    // (unique) totals
    double m_uniqueWords{ 0 };
    double m_uniqueMonoSyllabicWords{ 0 };
    double m_unique6CharsPlusWords{ 0 };
    double m_unique3PlusSyllableWords{ 0 };
    double m_uniqueDCHardWords{ 0 };
    double m_uniqueSpacheHardWords{ 0 };
    double m_uniqueHarrisJacobsonHardWords{ 0 };
    double m_uniqueMiniWords{ 0 };
    double m_uniqueHardFogWords{ 0 };
    double m_unique3PlusSyllabicWordsNumeralsFullySyllabized{ 0 };

    // options
    double m_minDocWordCountForBatch{ 0 };
    double m_includeIncompleteSentencesIfLongerThan{ 0 };
    int m_difficultSentenceLength{ 0 };

    // analysis options
    NumeralSyllabize m_numeralSyllabicationMethod{ NumeralSyllabize::WholeWordIsOneSyllable };
    LongSentence m_longSentenceMethod{ LongSentence::LongerThanSpecifiedLength };
    ParagraphParse m_paragraphsParsingMethod{
        ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs
    };
    InvalidSentence m_invalidSentenceMethod{ InvalidSentence::ExcludeFromAnalysis };
    TextSource m_textSource{ TextSource::FromFile };
    TextStorage m_documentStorageMethod{ TextStorage::NoEmbedText };
    VarianceMethod m_varianceMethod{ VarianceMethod::PopulationVariance };

    // readability scores options
    bool m_includeScoreSummaryReport{ false };
    // test options
    SpecializedTestTextExclusion m_hjTextExclusion;
    SpecializedTestTextExclusion m_dcTextExclusion;
    readability::proper_noun_counting_method m_dcProperNounCountingMethod;
    FleschNumeralSyllabize m_fleschNumeralSyllabizeMethod;
    FleschKincaidNumeralSyllabize m_fleschKincaidNumeralSyllabizeMethod;

    std::set<TestGoal> m_testGoals;
    std::set<StatGoal> m_statGoals;
    static std::map<comparable_first_pair<Goal::string_type, Goal::string_type>,
                    std::function<double(const BaseProject*)>>
        m_statGoalLabels;

    readability::test_language m_language{ readability::test_language::english_test };
    wxString m_reviewer;
    wxString m_status;

    bool m_includeDolchSightWords{ false };
    bool m_hasUI{ true };

    // grammar options
    bool m_spellcheck_ignore_proper_nouns{ false };
    bool m_spellcheck_ignore_uppercased{ false };
    bool m_spellcheck_ignore_numerals{ false };
    bool m_spellcheck_ignore_file_addresses{ false };
    bool m_spellcheck_ignore_programmer_code{ false };
    bool m_allow_colloquialisms{ false };
    bool m_spellcheck_ignore_social_media_tags{ false };

    // analysis options
    bool m_ignoreBlankLinesForParagraphsParser{ false };
    bool m_ignoreIndentingForParagraphsParser{ false };
    bool m_sentenceStartMustBeUppercased{ false };
    bool m_aggressiveExclusion{ false };
    bool m_excludeTrailingCopyrightNoticeParagraphs{ false };
    bool m_excludeTrailingCitations{ false };
    bool m_excludeFileAddresses{ false };
    bool m_excludeNumerals{ false };
    bool m_excludeProperNouns{ false };
    bool m_includeExcludedPhraseFirstOccurrence{ false };

    // flag indicating that loading the original text failed or succeeded when opening the project
    bool m_loadingOriginalTextSucceeded{ true };
    bool m_isRefreshing{ false };
    // test options
    bool m_fogUseSentenceUnits{ false };
    bool m_includeStockerCatholicDCSupplement{ false };

    double m_indexScore{ 0 };
    // this is actually an integer that gets rounded,
    // but stored as double so that it can be set to NaN if invalid.
    double m_clozeScore{ 0 };

    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_3SybPlusData{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_6CharPlusData{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_DCHardWordsData{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_SpacheHardWordsData{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_harrisJacobsonHardWordsData{
        nullptr
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_unusedDolchWordsData{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_dolchWordsData{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_nonDolchWordsData{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_ProperNounsData{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_contractionsData{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_keyWordsBaseData{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_AllWordsBaseData{ nullptr };

    std::shared_ptr<CaseInSensitiveNonStemmingDocument> m_words{ nullptr };
    std::shared_ptr<double_frequency_set<word_case_insensitive_no_stem>> m_word_frequency_map{
        nullptr
    };
    mutable std::vector<WarningMessage> m_messages;
    std::shared_ptr<ReadabilityFormulaParser> m_formulaParser{ nullptr };

    // these can vary from project to project
    std::shared_ptr<grammar::phrase_collection> m_excluded_phrases{ nullptr };

    StatisticsInfo m_statsInfo;
    StatisticsReportInfo m_statsReportInfo;
    GrammarInfo m_grammarInfo;
    WordsBreakdownInfo m_wordsBreakdownInfo;
    SentencesBreakdownInfo m_sentencesBreakdownInfo;

    // stores document text
    std::wstring m_documentContent;
    size_t m_textSize{ 0 };

    std::wstring m_appendedDocumentContent;

    wxString m_testName;
    wxString m_gradeLevel;
    wxString m_readerAge;

    // phrases and word to exclude entirely from the document
    wxString m_excludedPhrasesPath;
    std::vector<std::pair<wchar_t, wchar_t>> m_exclusionBlockTags;

    wxString m_projectDirectory;
    wxString m_currentCustTest;

    // file name and optional short name description
    std::vector<comparable_first_pair<wxString, wxString>> m_sourceFilePaths;
    wxString m_appendedDocumentFilePath;
    std::vector<double> m_aggregatedGradeScores;
    // these are actually integers that get rounded,
    // but stored as doubles so that they can be set to NaN if invalid.
    std::vector<double> m_aggregatedClozeScores;

    stemming::english_stem<> m_english_stem;
    stemming::spanish_stem<> m_spanish_stem;
    stemming::german_stem<> m_german_stem;
    grammar::english_syllabize m_english_syllabize;
    grammar::spanish_syllabize m_spanish_syllabize;
    grammar::german_syllabize m_german_syllabize;
    grammar::is_incorrect_english_article m_english_mismatched_article;
    grammar::is_english_coordinating_conjunction m_english_conjunction;
    grammar::is_spanish_coordinating_conjunction m_spanish_conjunction;
    grammar::is_german_coordinating_conjunction m_german_conjunction;

    ReadabilityMessages m_readMessages;
    TestCollectionType m_readabilityTests;
    std::vector<CustomReadabilityTestInterface> m_customTestsInUse;

    std::vector<WarningMessage> m_queuedMessages;

    static readability::readability_test_collection<readability::readability_test>
        m_defaultReadabilityTestsTemplate;
    static std::map<wxString, int> m_testIdMap;

    // just used to return a reference to an empty string
    const wxString m_emptyString;
    };

/// Used to mark a project as being loaded or refreshed.
/// Will automatically unlock project when it goes out of scope.
class BaseProjectProcessingLock
    {
  public:
    explicit BaseProjectProcessingLock(BaseProject* project) : m_project(project)
        {
        assert(m_project);
        m_project->SetProcessing(true);
        }

    BaseProjectProcessingLock() = delete;
    BaseProjectProcessingLock(const BaseProjectProcessingLock&) = delete;
    BaseProjectProcessingLock& operator=(const BaseProjectProcessingLock&) = delete;

    ~BaseProjectProcessingLock() { m_project->SetProcessing(false); }

  private:
    BaseProject* m_project{ nullptr };
    };

#endif // BASE_PROJECT_H
