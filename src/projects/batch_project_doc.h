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

#ifndef BATCH_PROJECT_DOC_H
#define BATCH_PROJECT_DOC_H

#include "../Wisteria-Dataviz/src/data/dataset.h"
#include "base_project_doc.h"
#include "base_project_view.h"
#include <vector>
#include <wx/wx.h>

class BatchProjectDoc final : public BaseProjectDoc
    {
  public:
    BatchProjectDoc()
        {
        // batches don't use manually entered text, so just set this to reflect that
        SetTextSource(TextSource::FromFile);
        }

    BatchProjectDoc(const BatchProjectDoc&) = delete;
    BatchProjectDoc& operator=(const BatchProjectDoc&) = delete;

    ~BatchProjectDoc() override
        {
        for (auto& doc : m_docs)
            {
            wxDELETE(doc);
            }
        DeleteExcludedPhrases();
        }

    bool OnNewDocument() final;
    bool OnCreate(const wxString& path, long flags) final;
    bool OnSaveDocument(const wxString& filename) final;
    bool OnOpenDocument(const wxString& filename) final;

    void RefreshProject() final;
    /// Only refresh the graphs, this assumes that no windows are being added
    /// or removed from the project.
    void RefreshGraphs() final;
    // only refresh statistics reports
    void RefreshStatisticsReports() final;

    /// @returns A const reference to the batch's documents.
    [[nodiscard]]
    const std::vector<BaseProject*>& GetDocuments() const noexcept
        {
        return m_docs;
        }

    /// @returns A reference to the batch's documents.
    [[nodiscard]]
    std::vector<BaseProject*>& GetDocuments() noexcept
        {
        return m_docs;
        }

    /// @returns The unique list of labels connected to the documents.
    [[nodiscard]]
    const std::map<traits::case_insensitive_wstring_ex, Wisteria::Data::GroupIdType>&
    GetDocumentLabels() const noexcept
        {
        return m_docLabels;
        }

    /// @returns The maximum number of groups that the data can be broken into for graphs.
    [[nodiscard]]
    size_t GetMaxGroupCount() const noexcept
        {
        return m_maxGroupCount;
        }

    /// Sets the maximum number of groups allows to bin the documents into (based on their
    /// description/group label).
    void SetMaxGroupCount(const size_t maxCount) noexcept { m_maxGroupCount = maxCount; }

    /** @returns The document from the batch by name.
            If document name isn't found in the batch, then null is returned.
        @param docName The full name (including filepath) of the document.*/
    [[nodiscard]]
    const BaseProject* GetDocument(const wxString& docName) const
        {
        for (auto* doc : GetDocuments())
            {
            if (doc != nullptr &&
                CompareFilePaths(doc->GetOriginalDocumentFilePath(), docName) == 0)
                {
                return doc;
                }
            }
        return nullptr;
        }

    void RemoveDocument(const wxString& docName);
    void RemoveMisspellings(const wxArrayString& misspellingsToRemove) final;

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetMisspelledWordData() const noexcept
        {
        return m_misspelledWordData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetRepeatedWordData() const noexcept
        {
        return m_dupWordData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetIncorrectArticleData() const noexcept
        {
        return m_incorrectArticleData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetOverusedWordBySentenceData() const noexcept
        {
        return m_overusedWordBySentenceData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetPassiveVoiceData() const noexcept
        {
        return m_passiveVoiceData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetWordyItemsData() const noexcept
        {
        return m_wordyPhraseData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetRedundantPhrasesData() const noexcept
        {
        return m_redundantPhraseData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetWordingErrorsData() const noexcept
        {
        return m_wordingErrorData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetClicheData() const noexcept
        {
        return m_clichePhraseData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetAllWordsBatchData() const noexcept
        {
        return m_allWordsBatchData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetKeyWordsBatchData() const noexcept
        {
        return m_keyWordsBatchData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetLongSentencesData() const noexcept
        {
        return m_overlyLongSentenceData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetConjunctionStartingSentencesData() const noexcept
        {
        return m_sentenceStartingWithConjunctionsData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>&
    GetLowerCasedSentencesData() const noexcept
        {
        return m_sentenceStartingWithLowercaseData;
        }

  private:
    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetMisspelledWordData() noexcept
        {
        return m_misspelledWordData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetRepeatedWordData() noexcept
        {
        return m_dupWordData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetAllWordsBatchData() noexcept
        {
        return m_allWordsBatchData;
        }

    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetKeyWordsBatchData() noexcept
        {
        return m_keyWordsBatchData;
        }

    void DisplayFleschChart();
    void DisplayDB2Plot();
    void DisplayCrawfordGraph();
    void DisplayInfleszGraph();
    void DisplayLixGauge();
    void DisplayGermanLixGauge();
    constexpr static size_t CUMULATIVE_STATS_COUNT = 13;
    void LoadProjectFile(const char* projectFileText, const size_t textLength);
    bool RunProjectWizard(const wxString& path);
    bool LoadDocuments(wxProgressDialog& progressDlg);
    void LoadScoresSection();
    void LoadSummaryStatsSection();
    void LoadWarningsSection();
    void LoadHardWordsSection();
    void LoadDolchSection();
    void DisplayReadabilityGraphs();
    void DisplayHistograms();
    void DisplayHistogram(const wxString& name, const wxWindowID Id, const wxString& topLabel,
                          const wxString& bottomLabel,
                          const std::shared_ptr<const Wisteria::Data::Dataset>& data,
                          const bool includeTest, const bool isTestGradeLevel,
                          const bool startAtOne);
    void DisplayBoxPlots();
    void DisplayScores();
    void DisplayWarnings();
    void DisplayGrammar();
    void DisplaySentencesBreakdown();
    void DisplayHardWords();
    void DisplaySummaryStats();
    void DisplaySightWords();

    void InitializeDocuments();
    /** @brief Build a list of unique labels from the documents.
        @details If there is a low number of labels, then these should be treated
            like grouping labels (and used as grouping for the
            graphs). Otherwise, they are probably just document descriptions
            (pulled from their metadata).*/
    void LoadGroupingLabelsFromDocumentsInfo();
    void SyncFilePathsWithDocuments();

    void ShowQueuedMessages() final;

    /** @brief Removes documents that failed to be loaded (usually because they couldn't be found
            or if they didn't contain enough text).
        @details Along with removing the loaded document, it will remove the file
            from the project settings.
        @returns @c true if the user requested to removed failed documents from the project.*/
    bool CheckForFailedDocuments();
    void RemoveFailedDocuments();

    /** @brief Fills a row in a data grid with statistics.
        @param dataGrid The data grid to write to.
        @param rowName The label for the first column. Usually a test name or a document name.
        @param optionalDescription An optional description for the second column.
            If empty, then won't be used for the second column.
            This parameter only makes sense for adding a description next to a document.
        @param rowNum The row index to write to inside @c dataGrid.
        @param data The data to analyze.
        @param decimalSize The amount of floating-point precision to use.
        @param varianceMethod The variance method to calculate with (population or sample).
        @param allowCustomFormatting Whether custom formatting should be used for number formatting
            in the @c dataGrid.*/
    static void
    SetScoreStatsRow(const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& dataGrid,
                     const wxString& rowName, const wxString& optionalDescription,
                     const long rowNum, const std::vector<double>& data, const int decimalSize,
                     const VarianceMethod varianceMethod, const bool allowCustomFormatting);
    /** @brief Fills a score list control with statistics.
        @param windowName The name of the list control.
        @param windowId The window ID of the list control.
        @param data The data grid used for the list control (will be written to).
        @param firstColumnName The label for the first column in @c data.
        @param optionalSecondColumnName An optional label for the second column.
            If empty, will not be used for the second column.\n
            This parameter only makes sense for adding a description next to a document.
        @param multiSelectable @c true to make the list control multi-item selectable.*/
    void DisplayScoreStatisticsWindow(
        const wxString& windowName, const int windowId,
        const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& data,
        const wxString& firstColumnName, const wxString& optionalSecondColumnName,
        const bool multiSelectable);

    /// @returns `true` if there is more than one document label, but not more than
    ///     the max number of groups for the legends.
    [[nodiscard]]
    bool IsShowingGroupLegends() const noexcept
        {
        return (GetDocumentLabels().size() <= GetMaxGroupCount() && GetDocumentLabels().size() > 1);
        }

    [[nodiscard]]
    static wxString GetScoreColumnName()
        {
        return _DT(L"SCORE");
        }

    [[nodiscard]]
    static wxString GetGroupColumnName()
        {
        return _DT(L"GROUP");
        }

    bool m_adjustParagraphParserForDocFiles{ false };
    std::vector<std::shared_ptr<Wisteria::Data::Dataset>> m_customTestScores;

    size_t m_maxGroupCount{ 10 };

    Wisteria::Colors::Schemes::EarthTones m_legendScheme;
    Wisteria::Icons::Schemes::StandardShapes m_iconScheme;

    std::vector<BaseProject*> m_docs;
    std::map<traits::case_insensitive_wstring_ex, Wisteria::Data::GroupIdType> m_docLabels;
    Wisteria::Data::ColumnWithStringTable::StringTableType m_groupStringTable;
    // score list data
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_scoreRawData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_goalsData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_scoreStatsData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_aggregatedGradeScoresData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_aggregatedClozeScoresData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    // grammar list data
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_dupWordData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_incorrectArticleData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_passiveVoiceData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_misspelledWordData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_wordyPhraseData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_redundantPhraseData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_wordingErrorData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_overusedWordBySentenceData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_clichePhraseData{
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
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_allWordsBatchData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_keyWordsBatchData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    // difficult words list data
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_hardWordsData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    // summary stats data
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_summaryStatsData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::vector<wxString> m_summaryStatsColumnNames;
    // warnings list data
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_warnings{
        std::make_shared<Wisteria::UI::ListCtrlExDataProvider>()
    };
    // dolch list data
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_dolchCompletionData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_dolchWordsBatchData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_NonDolchWordsData{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };

    wxDECLARE_DYNAMIC_CLASS(BatchProjectDoc);
    };

#endif // BATCH_PROJECT_DOC_H
