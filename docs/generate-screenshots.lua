dofile(Application.GetLuaConstantsPath())

ScreenshotLib.ShowScriptEditor(false)

-- input
ScreenshotProjectsFolder = Application.GetAbsoluteFilePath(
            Debug.GetScriptFolder(),
            "projects/")

-- output
ImagePath = Debug.GetScriptFolder().."readability-studio-manual/images/"
FileExtension = "bmp"

-- reset settings for the screenshots
Application.RemoveAllCustomTests()
Application.RemoveAllCustomTestBundles()
Application.ResetSettings()
Application.DisableAllWarnings()
Application.SetWindowSize(1000, 700)
Application.SetReviewer("Isabelle M.")

Application.SetCenterPrintHeader("@TITLE@")
Application.SetRightPrintHeader("A. Clark")
Application.SetLeftPrintFooter("Page @PAGENUM@ of @PAGESCNT@")
Application.SetRightPrintFooter("@DATE@")

ScreenshotLib.ShowTestBundleDialog("4-F")
ScreenshotLib.SnapScreenshot(ImagePath .. "test-bundle." .. FileExtension)

ScreenshotLib.ShowTestBundleDialog("4-F", 20002, "FORCAST", "flesch-kincaid", "fry", "gunning fog")
ScreenshotLib.SnapScreenshot(ImagePath .. "test-bundle-4f." .. FileExtension)

ScreenshotLib.CloseTestBundleDialog()

ScreenshotLib.ShowCustomTestDialogFunctionBrowser(true)
ScreenshotLib.SnapScreenshot(ImagePath .. "function-browser." .. FileExtension, 7006)
ScreenshotLib.CloseCustomTestDialog()

ScreenshotLib.ShowWebHarvesterDlg()
ScreenshotLib.SnapScreenshot(ImagePath .. "webharvester." .. FileExtension)
ScreenshotLib.CloseWebHarvesterDlg()

ScreenshotLib.ShowSelectProjectTypeDlg(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "selectprojecttype." .. FileExtension)
ScreenshotLib.CloseSelectProjectTypeDlg()

ScreenshotLib.ShowGetDirDlg("/home/rdoyle/ReadabilityStudio/Topics")
ScreenshotLib.SnapScreenshot(ImagePath .. "selectdirectory." .. FileExtension)
ScreenshotLib.CloseGetDirDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Licenses\\license.rtf", "Count", "1", "Passive Voice", "\"be purchased\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchpassivevoiceviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Licenses\\license.rtf", "Count", "2", "Article Mismatches", "\"a agreement\" * 2")
ScreenshotLib.SnapScreenshot(ImagePath .. "viewitemarticlemismatch." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Dialogs\\NewProject.html", "Automated Readability Index", "12.1", "New Dale-Chall", "16+", "Flesch-Kincaid", "12.0", "New Fog Count", "8.8", "Simplified Automated Readability Index", "11.0")
ScreenshotLib.SnapScreenshot(ImagePath .. "viewitemrawscores." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\BatchProject\\Grammar\\ReviewingRepeatedWords.html", "Count", "3", "Repeated Words", "\"the the\" * 2, \"to to\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchrepeatedviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\IntroductoryOverview.html", "Count", "3", "Wordy Phrases", "\"is able to\", \"it is important to note\", \"take into account\"", "Suggestions", "\"can\", \"note\", \"consider\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "viewitemwordyphrase." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\BatchProject\\Grammar\\ReviewingRedundantPhrases.html", "Count", "7", "Redundant Phrases", "\"brief summary\" * 3, \"collaborated together\" * 2, \"join together\" * 2", "Suggestions", "\"summary\", \"collaborated\", \"join, combine\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchredundantphrasesviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\BatchProject\\Grammar\\ReviewingBatchCliches.html", "Count", "4", "Clichés", "\"off the wall\" * 4", "Explanations/Suggestions", "\"unusual\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchclichesviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\StandardProject\\Grammar\\ReviewingHighlightedText.html", "Overly-long Sentences", "9", "Longest Sentence Length", "36", "Longest Sentence", "As a special note for documents with numerous sentences over twenty words long, it is recommended to open the Project Properties dialog and click the Document Analysis icon and select the option Outside sentence-length outlier range.")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchlongestsentenceviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Program Files\\Readability Studio\\examples\\The Frog Prince.txt", "Count", "3", "Starting Word", "\"stop\", \"thy\", \"you\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchlowersentenceviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\BatchProject\\Grammar\\WebHarvester.html", "Total", "1", "Unique Count", "1", "Misspellings", "\"specifiy\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchmisspellingsviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\BatchProject\\Grammar\\LongSentenceSearching.html", "Count", "1", "Wording Errors", "\"in parenthesis\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchwordingerrorsviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowOptions(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "options-dlg." .. FileExtension, -1, -1, 1089)
ScreenshotLib.CloseOptions()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Program Files\\Readability Studio\\examples\\The Frog Prince.txt", "Count", "16", "Conjunctions", "\"And\" * 4, \"But\" * 9, \"So\" * 3")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchconjunctionviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowPrinterHeaderFooterOptions()
ScreenshotLib.SnapScreenshot(ImagePath .. "printerheadersfooters." .. FileExtension)
ScreenshotLib.ClosePrinterHeaderFooterOptions()

-- List export
ScreenshotLib.ShowListExportDlg(1031, 1, false, true)
ScreenshotLib.SnapScreenshot(ImagePath .. "exportoptions." .. FileExtension)
ScreenshotLib.CloseListExportDlg()

-- Custom test dialog
ScreenshotLib.ShowCustomTestDialogGeneralSettings()
ScreenshotLib.SnapScreenshot(ImagePath .. "add-custom-test." .. FileExtension)

ScreenshotLib.ShowCustomTestDialogFamiliarWords()
ScreenshotLib.SetCustomTestDialogIncludedLists(false, true, false, true)
ScreenshotLib.SnapScreenshotOfDialogWithPropertyGrid(ImagePath .. "custom-test-include-lists." .. FileExtension,
    7003, "", "", true)

ScreenshotLib.ShowCustomTestDialogProperNounsAndNumbers()
ScreenshotLib.SnapScreenshotOfDialogWithPropertyGrid(ImagePath .. "custom-test-treat." .. FileExtension,
    7004, "", "", true, 200)

ScreenshotLib.SetCustomTestDialogDocumentTypes(false, false, false, false, true)
ScreenshotLib.SetCustomTestDialogIndustries(false, false, true, false, false, false, false)
ScreenshotLib.ShowCustomTestDialogClassification()
ScreenshotLib.SnapScreenshotOfDialogWithPropertyGrid(ImagePath .. "custom-test-wizard-settings." .. FileExtension,
    7005, "", "", true)

ScreenshotLib.CloseCustomTestDialog()

-- Standard project wizard
ScreenshotLib.ShowStandardProjectWizardTextSourcePage(1)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizard1." .. FileExtension)
-- crop off bottom 3/4
info = Application.GetImageInfo(ImagePath .. "wizard1." .. FileExtension)
ScreenshotLib.CropScreenshot(ImagePath .. "wizard1." .. FileExtension, -1, info["Height"]/4)

ScreenshotLib.SetStandardProjectWizardTextFromFilePath("")
ScreenshotLib.SnapScreenshot(ImagePath .. "exampleselectfile." .. FileExtension, 10001)
-- crop off bottom 3/4
info = Application.GetImageInfo(ImagePath .. "exampleselectfile." .. FileExtension)
ScreenshotLib.CropScreenshot(ImagePath .. "exampleselectfile." .. FileExtension, -1, info["Height"]/4)

ScreenshotLib.SetStandardProjectWizardTextFromFilePath("https://www.libreoffice.org/about-us/who-are-we/")
ScreenshotLib.SnapScreenshot(ImagePath .. "webpath." .. FileExtension, 10002, 10001, 10001)

ScreenshotLib.ShowStandardProjectWizardTestByDocumentTypePage(4)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddoctypelitselected." .. FileExtension, -1, -1, 10028)

ScreenshotLib.ShowStandardProjectWizardDocumentStructurePage(1, false, false, true)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddocstructurehardreturns." .. FileExtension, 10020, 10021, 10032)

ScreenshotLib.ShowStandardProjectWizardDocumentStructurePage(1, false, false, false)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddocstructure." .. FileExtension)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddocstructurenarrativeselected." .. FileExtension, 10024, 10025, 10032)
-- option highlighted (but not selected) for example
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddocstructuresentencessplit." .. FileExtension, 10026, 10027, 10032)

ScreenshotLib.ShowStandardProjectWizardDocumentStructurePage(2, false, false, false)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddocstructurenonnarrative." .. FileExtension, 10022, 10023, 10032)

ScreenshotLib.ShowStandardProjectWizardTestByIndustryPage(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardindustry." .. FileExtension, -1, -1, 10031)

ScreenshotLib.ShowStandardProjectWizardTextEntryPage("   ")
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardentertext." .. FileExtension)
-- crop off bottom half
info = Application.GetImageInfo(ImagePath .. "wizardentertext." .. FileExtension)
ScreenshotLib.CropScreenshot(ImagePath .. "wizardentertext." .. FileExtension, -1, info["Height"]/2)

ScreenshotLib.ShowStandardProjectWizardManualTestSelectionPage()
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardtests." .. FileExtension)

ScreenshotLib.ShowStandardProjectWizardTestByBundlePage("Patient Consent Forms")
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbundles." .. FileExtension, -1, -1, 10030)

ScreenshotLib.ShowStandardProjectWizardTestRecommendationPage(1)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardreadabilitymethods." .. FileExtension, -1, -1, 10028)

ScreenshotLib.CloseStandardProjectWizard()

-- DDX is really weird with this page for some reason on Vista, so we need to get this separately
ScreenshotLib.ShowStandardProjectWizardLanguagePage()
ScreenshotLib.ShowStandardProjectWizardTestByDocumentTypePage(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddoctypetechselected." .. FileExtension, -1, -1, 10028)
ScreenshotLib.CloseStandardProjectWizard()

-- Batch project wizard
ScreenshotLib.ShowBatchProjectWizardTextSourcePage(ScreenshotProjectsFolder .. "Topics2008")
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchselectfilesloaded." .. FileExtension)
ScreenshotLib.BatchProjectWizardTextSourcePageSetFiles()
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchselectfiles." .. FileExtension)
ScreenshotLib.SnapScreenshot(ImagePath .. "randomsample." .. FileExtension, 10011, 10012, 10029)

ScreenshotLib.ShowBatchProjectWizardTestByDocumentTypePage(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchdoctype." .. FileExtension, -1, -1, 10028)

ScreenshotLib.ShowBatchProjectWizardTestByIndustryPage(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchindustry." .. FileExtension, -1, -1, 10031)

ScreenshotLib.ShowBatchProjectWizardTestRecommendationPage(1)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchreadabilitymethods." .. FileExtension, -1, -1, 10028)

ScreenshotLib.ShowBatchProjectWizardManualTestSelectionPage()
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchtests." .. FileExtension)

ScreenshotLib.CloseBatchProjectWizard()

-- Batch project
Application.EnableWarning("document-less-than-100-words")
bp = BatchProject(ScreenshotProjectsFolder .. "Topics2008.rsbp")
bp:SetSpellCheckerOptions(true,true,true,true,true)
bp:Reload()
bp:ExportGraph(SideBarSection.ReadabilityScores, GraphType.Fry, ImagePath .. "multiple-fry." .. FileExtension)

bp:SelectWindow(SideBarSection.Grammar, ListType.Cliches)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "batchclichemultiple." .. FileExtension,
    ListType.Cliches, 1, 3, 2, 3, 3)

bp:SelectWindow(SideBarSection.SentencesBreakdown, ListType.LongSentences)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchlongsentences." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.OverusedWordsBySentence)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchoverusedwordsbysentence." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.MisspelledWords)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchmisspellings." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.WordingErrors)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchwordingerrors." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.RedundantPhrases)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchredundantphrases." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.Wordiness)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchwordyphrases." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.PassiveVoice)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchpassivevoice." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.WordsBreakdown)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchdifficultwordsverbose." .. FileExtension, 1)

bp:SelectWindow(SideBarSection.Warnings)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "warnings." .. FileExtension, 1)

bp:SelectWindow(SideBarSection.SentencesBreakdown, ListType.LongSentences)
bp:SortList(ListType.LongSentences, 3, SortOrder.SortDescending)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchlongestsentence." .. FileExtension, 1)

bp:Close()
Application.DisableAllWarnings()

bp = BatchProject(ScreenshotProjectsFolder .. "Topics2008(Improved) Bundle.rsbp")
bp:SelectWindow(SideBarSection.ReadabilityScores, ListType.Goals)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "batch-project-goals." .. FileExtension,
    ListType.Goals, -1, 3)
bp:Close()

Application.RemoveAllCustomTests()
Application.DisableAllWarnings()

-- Batch project (examples)
bp = BatchProject(ScreenshotProjectsFolder .. "examples.rsbp")
bp:SelectWindow(SideBarSection.Grammar, ListType.Cliches)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchcliches." .. FileExtension, 2)
bp:SelectWindow(SideBarSection.Grammar, ListType.LowercasedSentences)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchlowersentences." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.ConjunctionStartingSentences)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchconjunctionstart." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Dolch, DolchResultType.DolchCoverageBatchSummary)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchdolchcoverage." .. FileExtension, 1)

bp:SelectWindow(SideBarSection.Dolch, DolchResultType.DolchWordsBatchSummary)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchdolchwords." .. FileExtension, 1)

bp:SelectWindow(SideBarSection.Dolch, DolchResultType.NonDolchWordsBatchSummary)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchdolchnonwords." .. FileExtension, 1)

bp:Close()
Application.RemoveAllCustomTests()

-- Batch project
bp = BatchProject(ScreenshotProjectsFolder .. "Topics2008More.rsbp")

bp:ShowSidebar(false)

bp:SelectWindow(SideBarSection.ReadabilityScores, BatchScoreResultType.RawScores)
ScreenshotLib.SnapScreenshot(ImagePath .. "batchrawscores." .. FileExtension)

bp:SelectWindow(SideBarSection.ReadabilityScores, BatchScoreResultType.AggregatedScoreStatistics)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchscorestats." .. FileExtension, 1)
bp:Close()
Application.RemoveAllCustomTests()

-- Batch project
bp = BatchProject(ScreenshotProjectsFolder .. "Topics2008(Less Tests).rsbp")

bp:ExportGraph(SideBarSection.Histograms, Application.GetTestId("flesch-kincaid-test"), ImagePath .. "histogrades." .. FileExtension)
bp:ExportGraph(SideBarSection.Histograms, Application.GetTestId("flesch-test"), ImagePath .. "histoindex." .. FileExtension)

bp:ExportGraph(SideBarSection.BoxPlots, Application.GetTestId("flesch-test"), ImagePath .. "boxindex." .. FileExtension)

bp:ShowSidebar(false)
bp:SelectWindow(SideBarSection.ReadabilityScores, BatchScoreResultType.AggregatedGradeScoresByDocument)
ScreenshotLib.SnapScreenshot(ImagePath .. "batch-grade-scores-by-document." .. FileExtension)

bp:SelectWindow(SideBarSection.ReadabilityScores, BatchScoreResultType.AggregatedClozeScoresByDocument)
ScreenshotLib.SnapScreenshot(ImagePath .. "batch-cloze-scores-by-document." .. FileExtension)

bp:SelectWindow(SideBarSection.WordsBreakdown)
ScreenshotLib.SnapScreenshot(ImagePath .. "batchdifficultwords." .. FileExtension)
bp:Close()

Application.RemoveAllCustomTests()

bp = BatchProject(ScreenshotProjectsFolder .. "Topics2008(Improved).rsbp")
bp:ExportGraph(SideBarSection.Histograms, Application.GetTestId("flesch-kincaid-test"), ImagePath .. "histoimproved." .. FileExtension)
bp:Close()

-- Standard project
-------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Cocoa Desserts.rsp")
sp:ScrollTextWindow(HighlightedReportType.GrammarHighlightedIssues, "Sift together one-half cup of the flour")
ScreenshotLib.SnapScreenshot(ImagePath .. "longsentences." .. FileExtension) 
sp:SelectHighlightedWordReport(HighlightedReportType.ThreePlusSyllableHighlightedWords, "Consequently")
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "difficultwordselected." .. FileExtension, 1)
sp:SelectWindow(SideBarSection.WordsBreakdown, ListType.ThreePlusSyllableWords)
sp:SortList(ListType.ThreePlusSyllableWords, 2, SortOrder.SortDescending)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "exampledifficultwords." .. FileExtension, 1)
sp:SelectWindow(SideBarSection.SentencesBreakdown, ListType.LongSentences)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "examplelongsentences." .. FileExtension, 1)
sp:SelectWindow(SideBarSection.Statistics, ReportType.StatisticsSummaryReport)
ScreenshotLib.SnapScreenshot(ImagePath .. "examplestats." .. FileExtension)
sp:SelectWindow(SideBarSection.ReadabilityScores, ReportType.ReadabilityScoresTabularReport)
ScreenshotLib.SnapScreenshot(ImagePath .. "testscores." .. FileExtension)
sp:SelectWindow(SideBarSection.Grammar, ReportType.StatisticsSummaryReport)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "longsentenceselected." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true,
    "Put the cocoa in a small saucepan; add the cold water and stir until perfectly smooth; then the hot water, and cook for one or two minutes, add vanilla and a speck of salt, then stir in enough sugar to make it stiff enough to spread nicely.")

sp:DelayReloading(true)
sp:SetDocumentFilePath("/home/mcrane/Cocoa Desserts.rtf") -- use generic filepath
sp:SetReviewer("Maris")
sp:ShowProperties(OptionsPageType.ProjectSettings)
ScreenshotLib.SnapScreenshot(ImagePath .. "projectproperties." .. FileExtension, -1, -1, 1083)
ScreenshotLib.SnapScreenshot(ImagePath .. "linktofile." .. FileExtension, 1085, -1, 1083)
sp:CloseProperties()

sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (Frog Prince)
---------------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "The Frog Prince.rsp")
sp:SelectWindow(SideBarSection.Grammar, ListType.ConjunctionStartingSentences)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "conjunctionsent." .. FileExtension, 1)

sp:SelectWindow(SideBarSection.Dolch, DolchResultType.HighlightedNonDolchWordsReport)
ScreenshotLib.SnapScreenshot(ImagePath .. "dolchhighlightednonwords." .. FileExtension)

sp:SelectWindow(SideBarSection.Dolch, DolchResultType.HighlightedDolchWordsReport)
ScreenshotLib.SnapScreenshot(ImagePath .. "dolchhighlightedwords." .. FileExtension)

sp:SelectWindow(SideBarSection.Dolch, DolchResultType.NonDolchWordsList)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "dolchnonwordslist." .. FileExtension, 1)

sp:SelectWindow(SideBarSection.Dolch, DolchResultType.DolchStatisticsSummary)
ScreenshotLib.SnapScreenshot(ImagePath .. "dolchsummary." .. FileExtension)

sp:SelectWindow(SideBarSection.Dolch, DolchResultType.UnusedDolchWordsList)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "dolchunusedwordslist." .. FileExtension, 1)

sp:SelectWindow(SideBarSection.Dolch, DolchResultType.DolchWordsList)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "dolchwordslist." .. FileExtension, 1)
sp:SortList(DolchResultType.DolchWordsList, 2, SortOrder.SortDescending)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "columnheader." .. FileExtension,
    DolchResultType.DolchWordsList, -1, -1, -1, -1, 4)
sp:ExportGraph(GraphType.DolchWordBarChart, ImagePath .. "dolch-word-breakdown." .. FileExtension)
sp:ExportGraph(GraphType.DolchCoverageChart, ImagePath .. "dolch-coverage-chart." .. FileExtension)

sp:ExportGraph(GraphType.SentenceHeatmap, ImagePath .. "sentences-heatmap." .. FileExtension)

sp:Close()
Application.RemoveAllCustomTests()

sp = StandardProject(ScreenshotProjectsFolder .. "Consent.rsp")
sp:SelectWindow(SideBarSection.ReadabilityScores, ListType.Goals)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "standard-project-goals." .. FileExtension,
    ListType.Goals)
sp:Close()

-- Standard project (A Christmas Carol)
sp = StandardProject(ScreenshotProjectsFolder .. "A Christmas Carol.rsp")
sp:SelectWindow(SideBarSection.Grammar, ListType.LowercasedSentences)
ScreenshotLib.SnapScreenshot(ImagePath .. "lowercasedsent." .. FileExtension)
sp:SelectWindow(SideBarSection.Grammar, ListType.OverusedWordsBySentence)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "overusedwordsbysentence." .. FileExtension, 2)

sp:SelectWindow(SideBarSection.Grammar, ListType.Wordiness)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "wordylist." .. FileExtension, 2)

sp:SelectWindow(SideBarSection.Grammar, ListType.RedundantPhrases)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "redundantphrases." .. FileExtension, 2)

sp:SelectWindow(SideBarSection.Grammar, ListType.Cliches)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "clichelist." .. FileExtension, 2)
sp:SelectWindow(SideBarSection.Grammar, ListType.PassiveVoice)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "passivevoice." .. FileExtension, 2)

sp:SelectReadabilityTest(4)
ScreenshotLib.SnapScreenshot(ImagePath .. "scores." .. FileExtension)
sp:SelectWindow(SideBarSection.WordsBreakdown, ListType.ThreePlusSyllableWords)
sp:SortList(ListType.ThreePlusSyllableWords,2,SortOrder.SortDescending,3,SortOrder.SortDescending)
ScreenshotLib.SnapScreenshot(ImagePath .. "difficultwords." .. FileExtension)

sp:SelectRowsInWordsBreakdownList(ListType.ThreePlusSyllableWords, 1, 3, 4, 5, 7, 8, 9, 10, 12, 13, 15,
                                  16, 18, 19, 22, 23, 25, 26, 27, 30, 31, 32, 33, 34, 35, 36, 37, 40)
ScreenshotLib.SnapScreenshot(ImagePath .. "difficultwords2." .. FileExtension, 500)

sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "graph-bars-rectangular." .. FileExtension)
sp:SetStippleImage(ScreenshotProjectsFolder .. "Book.png")
sp:SetBarChartBarEffect(BoxEffect.StippleImage)
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "graph-custom-brush." .. FileExtension)

sp:SetStippleShapeColor("SmokyBlack")
sp:SetStippleShape("newspaper")
sp:SetBarChartBarEffect(BoxEffect.StippleShape)
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "graph-stipple-shape." .. FileExtension)
sp:SetStippleShape("car")
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "graph-stipple-shape-car." .. FileExtension)

sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (Features)
sp = StandardProject(ScreenshotProjectsFolder .. "Features.rsp")
sp:SelectReadabilityTest(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "featuresscores." .. FileExtension)

sp:SelectWindow(SideBarSection.SentencesBreakdown, ListType.LongSentences)
sp:SortList(ListType.LongSentences, 2, SortOrder.SortAscending)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "featuressentencelist." .. FileExtension, 1)

sp:SelectWindow(SideBarSection.Statistics, ReportType.StatisticsSummaryReport)
ScreenshotLib.SnapScreenshot(ImagePath .. "featuresstats." .. FileExtension)
sp:SelectWindow(SideBarSection.SentencesBreakdown, ListType.LongSentences)
sp:SortList(ListType.LongSentences, 2, SortOrder.SortDescending)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "featuressentencelistsorted." .. FileExtension, 1)

sp:ShowProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "featuressentoptions." .. FileExtension, 1040, 1041, 1086)
sp:CloseProperties()

sp:ScrollTextWindow(HighlightedReportType.GrammarHighlightedIssues, "Experts can manually select the tests that they want to")
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "featuressentences." .. FileExtension, 1)
sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (Features with sentence outlier)
sp = StandardProject(ScreenshotProjectsFolder .. "Features(Outlier).rsp")
sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "featuressentencesoutliers." .. FileExtension,
  HighlightedReportType.GrammarHighlightedIssues, true, "(45)", "(48)")

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "featuressentences25." .. FileExtension,
  HighlightedReportType.GrammarHighlightedIssues, true, "(25)", "(25)")

sp:SelectWindow(SideBarSection.Statistics, ReportType.StatisticsSummaryReport)
ScreenshotLib.SnapScreenshot(ImagePath .. "featuressentences43." .. FileExtension)
sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (improved Cocoa Desserts)
sp = StandardProject(ScreenshotProjectsFolder .. "Cocoa Desserts (Improved).rsp")
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "ribbon-edit-document-options." .. FileExtension, 1)
sp:SelectReadabilityTest(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "improvedscores." .. FileExtension)
sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (for wording errors)
sp = StandardProject(ScreenshotProjectsFolder .. "LongSentenceSearching.rsp")
sp:SelectWindow(SideBarSection.Grammar, ListType.WordingErrors)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "wordingerrors." .. FileExtension, 1)
sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (for misspellings)
sp = StandardProject(ScreenshotProjectsFolder .. "WebHarvester.rsp")
sp:SelectWindow(SideBarSection.Grammar, ListType.WordingErrors)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "misspellings." .. FileExtension, 1)
sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (License)
sp = StandardProject(ScreenshotProjectsFolder .. "license.rsp")
sp:SelectWindow(SideBarSection.Grammar, ListType.RepeatedWords)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "repeatedwords." .. FileExtension, 2)
sp:Close()
Application.RemoveAllCustomTests()

-- Batch project
bp = BatchProject(ScreenshotProjectsFolder .. "Distribution Documents.rsbp")
bp:SelectWindow(SideBarSection.Grammar, ListType.RepeatedWords)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchrepeatedwords." .. FileExtension, 2)
bp:Close()
Application.RemoveAllCustomTests()

-- Batch showing multiple items in lists
bp = BatchProject(ScreenshotProjectsFolder .. "Topics2020ForScreenshots.rsbp")
bp:SetSpellCheckerOptions(true,true,true,true,true)
bp:SetWindowSize(1200, 700)

bp:SelectWindow(SideBarSection.Grammar, ListType.Cliches)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "batchclichemultiple." .. FileExtension,
    ListType.Cliches, 1, 4, 2, 4)

bp:SelectWindow(SideBarSection.Grammar, ListType.RedundantPhrases)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "batchredundantphrasesmultiple." .. FileExtension,
    ListType.RedundantPhrases, 3, 4, 4, 4)

bp:SelectWindow(SideBarSection.Grammar, ListType.RepeatedWords)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "batchrepeatedmultiple." .. FileExtension,
    ListType.RepeatedWords, 2, 4, 2, 4)

bp:SelectWindow(SideBarSection.Grammar, ListType.Wordiness)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "batchwordyphrasesmultiple." .. FileExtension,
    ListType.Wordiness, 2, 4, 6, 4, 10)

bp:Close()

-- Standard project (Peter Rabbit)
----------------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "The Tale of Peter Rabbit.rsp")
sp:ShowProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "ignore-blank-lines." .. FileExtension, 1042, 1043, 1086)
sp:CloseProperties()

sp:SelectWindow(SideBarSection.WordsBreakdown, HighlightedReportType.ThreePlusSyllableHighlightedWords)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "peter-rabbit-chained-sentences." .. FileExtension,
  HighlightedReportType.ThreePlusSyllableHighlightedWords, true,
  "First he ate some lettuces and some French beans And then He Ate Some Radishes And then, feeling rather sick, he went to look for some parsley.")

sp:Close()
Application.RemoveAllCustomTests()

sp = StandardProject(ScreenshotProjectsFolder .. "The Tale of Peter Rabbit(Split Sentences).rsp")
sp:ScrollTextWindow(HighlightedReportType.ThreePlusSyllableHighlightedWords, "First he ate some lettuces and some French beans")
ScreenshotLib.SnapScreenshot(ImagePath .. "peter-rabbit-split-sentences." .. FileExtension)

sp:Close()
Application.RemoveAllCustomTests()

sp = StandardProject(ScreenshotProjectsFolder .. "Crawford Sample.rsp")
sp:ExportGraph(GraphType.Crawford, ImagePath .. "crawford-graph." .. FileExtension)
sp:Close()
Application.RemoveAllCustomTests()

sp = StandardProject(ScreenshotProjectsFolder .. "turkey brining.rsp")

sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "barchart." .. FileExtension)
sp:ExportGraph(GraphType.SentenceBoxPlox, ImagePath .. "sentences-boxplot." .. FileExtension)
sp:ExportGraph(GraphType.SentenceHistogram, ImagePath .. "sentences-histogram." .. FileExtension)
sp:SelectWindow(SideBarSection.SentencesBreakdown, GraphType.SentenceBoxPlox)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "ribbon-edit-graphs." .. FileExtension, 0, RibbonButtonBar.Edit)
sp:SelectWindow(SideBarSection.WordsBreakdown, GraphType.WordBarChart)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "ribbon-edit-bar-charts." .. FileExtension, 0, RibbonButtonBar.Edit)

sp:ExportGraph(GraphType.SyllableHistogram, ImagePath .. "syllable-count-histogram." .. FileExtension)
sp:ExportGraph(GraphType.SyllablePieChart, ImagePath .. "syllables-pie-chart." .. FileExtension)
sp:AddTest(Test.DanielsonBryan2)
sp:ExportGraph(GraphType.DanielsonBryan2, ImagePath .. "db2-plot." .. FileExtension)
sp:ExportGraph(GraphType.Fry, ImagePath .. "frygraph." .. FileExtension)
sp:ExportGraph(GraphType.Raygor, ImagePath .. "raygorgraph." .. FileExtension)
sp:ExportGraph(GraphType.Flesch, ImagePath .. "flesch-chart." .. FileExtension)
sp:ExportGraph(GraphType.Lix, ImagePath .. "lix-gauge." .. FileExtension)
sp:Close()

-- German graph
sp = StandardProject(ScreenshotProjectsFolder .. "turkey brining.rtf")
sp:SetLanguage(Language.German)
sp:AddTest(Test.Schwartz)
sp:AddTest(Test.LixGermanTechnical)
sp:ExportGraph(GraphType.Schwartz, ImagePath .. "schwartz." .. FileExtension)
sp:ExportGraph(GraphType.GermanLix, ImagePath .. "german-lix-gauge." .. FileExtension)
sp:Close()

sp = StandardProject(ScreenshotProjectsFolder .. "BadRaygorScore.txt")
sp:AddTest("raygor-test")
sp:ExportGraph(GraphType.Raygor, ImagePath .. "raygorbadgrade." .. FileExtension)
sp:Close()
Application.RemoveAllCustomTests()

sp = StandardProject(ScreenshotProjectsFolder .. "GPM graph.rsp")
sp:ExportGraph(GraphType.GpmFry, ImagePath .. "gpm-fry." .. FileExtension)
sp:Close()
Application.RemoveAllCustomTests()

-- Custom test example images
ScreenshotLib.ShowCustomTestDialogGeneralSettings("Flesch-Kincaid (unclamped)", "(11.8*(B/W)) + (.39*(W/S)) - 15.59", TestType.GradeLevel)
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-testfk-unclamped." .. FileExtension, 7000, 7000)
-- crop off bottom half
info = Application.GetImageInfo(ImagePath .. "custom-testfk-unclamped." .. FileExtension)
ScreenshotLib.CropScreenshot(ImagePath .. "custom-testfk-unclamped." .. FileExtension, -1, info["Height"]/2)

ScreenshotLib.ShowCustomTestDialogGeneralSettings("Flesch (IC)", "ROUND(206.835 - (84.6*(B/W)) - (1.015*(W/S)))", TestType.IndexValue)
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-test-example1-test-name." .. FileExtension, 7000, 7000)
-- crop off bottom half
info = Application.GetImageInfo(ImagePath .. "custom-test-example1-test-name." .. FileExtension)
ScreenshotLib.CropScreenshot(ImagePath .. "custom-test-example1-test-name." .. FileExtension, -1, info["Height"]/2)

ScreenshotLib.ShowCustomTestDialogGeneralSettings("Flesch (IC)", "ROUND(206.835 -\n(84.6*(SyllableCount()/WordCount())) -\n(1.015*(WordCount()/IndependentClauseCount())))", TestType.IndexValue)
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-test-example1-finished." .. FileExtension)
-- crop off bottom half
info = Application.GetImageInfo(ImagePath .. "custom-test-example1-finished." .. FileExtension)
ScreenshotLib.CropScreenshot(ImagePath .. "custom-test-example1-finished." .. FileExtension, -1, info["Height"]/2)

ScreenshotLib.CloseCustomTestDialog()

sp = StandardProject(ScreenshotProjectsFolder .. "FleschIC.rsp")
sp:SelectReadabilityTest(1)
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-test-example1-score." .. FileExtension)
sp:Close()
Application.RemoveAllCustomTests()

-- article mismating
sp = StandardProject(ScreenshotProjectsFolder .. "license.rtf")
sp:SelectWindow(SideBarSection.Grammar, ListType.ArticleMismatch)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "articlemismatches." .. FileExtension, 1)
sp:Close()

Application.RemoveAllCustomTests()
bp = BatchProject(ScreenshotProjectsFolder .. "Eula.rsbp")
bp:SelectWindow(SideBarSection.Grammar, ListType.ArticleMismatch)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batcharticlemismatches." .. FileExtension, 2)
bp:Close()

Application.RemoveAllCustomTests()

-- Custom test example 2
------------------------
ScreenshotLib.ShowCustomTestDialogGeneralSettings("New Dale-Chall (Baking)")
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-test-example2-name." .. FileExtension, 7000, 7000, 7001)

ScreenshotLib.ShowCustomTestDialogGeneralSettings("New Dale-Chall (Baking)", "CustomNewDaleChall()")
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-test-example2-formula-finished." .. FileExtension, 7002)

ScreenshotLib.ShowCustomTestDialogFamiliarWords("/home/Niles/CookingWords.txt")
ScreenshotLib.SnapScreenshotOfDialogWithPropertyGrid(ImagePath .. "custom-test-example2-words-loaded." .. FileExtension,
    7003, "File containing familiar words", "", true)

ScreenshotLib.ShowCustomTestDialogFamiliarWords("/home/Niles/CookingWords.txt", StemmingType.English, true)
ScreenshotLib.SnapScreenshotOfDialogWithPropertyGrid(ImagePath .. "custom-test-example2-word-settings-finished." .. FileExtension,
    7003, "Include New Dale-Chall word list", "", true)

ScreenshotLib.CloseCustomTestDialog()

-- Custom index Test example
----------------------------
ScreenshotLib.ShowCustomTestDialogGeneralSettings("Buzz Index")
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-index-test-example-name." .. FileExtension, 7000, 7000, 7001)

ScreenshotLib.ShowCustomTestDialogGeneralSettings("Buzz Index", "", TestType.IndexValue)
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-index-test-example-test-type." .. FileExtension, 7001, 7001, 7001)

ScreenshotLib.ShowCustomTestDialogGeneralSettings("Buzz Index", "IF((FamiliarWordCount()/WordCount())*100 > 15, 3,\n  IF((FamiliarWordCount()/WordCount())*100 > 5, 2, 1) )", TestType.IndexValue)
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-index-test-example-formula." .. FileExtension, 7002)

ScreenshotLib.ShowCustomTestDialogFamiliarWords("C:\\Users\\Roz\\WordLists\\Buzz Words.txt")
ScreenshotLib.SnapScreenshotOfDialogWithPropertyGrid(ImagePath .. "custom-index-test-example-words-loaded." .. FileExtension,
    7003, "File containing familiar words", "", true)

ScreenshotLib.ShowCustomTestDialogFamiliarWords("C:\\Users\\Roz\\WordLists\\Buzz Words.txt", StemmingType.English)
ScreenshotLib.SnapScreenshotOfDialogWithPropertyGrid(ImagePath .. "custom-index-test-example-word-settings-finished." .. FileExtension,
    7003, "Use stemming to search for similar words", "", true)

ScreenshotLib.ShowCustomTestDialogProperNounsAndNumbers(0, false)
ScreenshotLib.SnapScreenshotOfDialogWithPropertyGrid(ImagePath .. "custom-index-test-example-proper-and-numerals-finished." .. FileExtension,
    7004, "Treat numerals as familiar", "", true, 200)

ScreenshotLib.CloseCustomTestDialog()

sp = StandardProject(ScreenshotProjectsFolder .. "Press Release.rsp")
sp:SelectReadabilityTest(1)
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-index-test-example-score." .. FileExtension)
sp:Close()

-- Custom word example 2
------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Chocolate Eclairs.rsp")
sp:SelectReadabilityTest(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-test-example2-score." .. FileExtension)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "ribbon-edit-scores." .. FileExtension, 0, RibbonButtonBar.Edit)

sp:SelectHighlightedWordReport(Application.GetTestId("New Dale-Chall (Baking)"))
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-test-example2-custom-text." .. FileExtension)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "ribbon-edit-text-window." .. FileExtension, 0, RibbonButtonBar.Edit)

sp:SelectWindow(SideBarSection.WordsBreakdown, ListType.DaleChallUnfamiliarWords)
sp:SortList(ListType.DaleChallUnfamiliarWords, 1, SortOrder.SortAscending)
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-test-example2-dc-word-list." .. FileExtension)

sp:SelectWindow(SideBarSection.WordsBreakdown, Application.GetTestId("New Dale-Chall (Baking)"))
sp:SortList(Application.GetTestId("New Dale-Chall (Baking)"), 1, SortOrder.SortAscending)
ScreenshotLib.SnapScreenshot(ImagePath .. "custom-test-example2-custom-word-list." .. FileExtension)

sp:SelectWindow(SideBarSection.Statistics, ReportType.StatisticsSummaryReport)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "ribbon-edit-stats." .. FileExtension, 0, RibbonButtonBar.Edit)

ScreenshotLib.ShowFilteredTextPreviewDlg()
ScreenshotLib.SnapScreenshot(ImagePath .. "filter-text-preview." .. FileExtension)
ScreenshotLib.CloseFilteredTextPreviewDlg()

sp:Close()
Application.RemoveAllCustomTests()

sp = StandardProject(ScreenshotProjectsFolder .. "Frase.rsp")
sp:ExportGraph(GraphType.Frase, ImagePath .. "frase-graph." .. FileExtension)
sp:ExportGraph(GraphType.Inflesz, ImagePath .. "inflesz-graph." .. FileExtension)
sp:Close()
Application.RemoveAllCustomTests()

ScreenshotLib.ShowSortListDlg("Word", "Syllable Count", "Frequency")
ScreenshotLib.SnapScreenshot(ImagePath .. "sortcolumnsdialog." .. FileExtension)
ScreenshotLib.CloseSortListDlg()

-- Text exclusion example
-------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Danielson-Bryan.html")

sp:ExcludeFileAddress(false)
sp:ExcludeProperNouns(false)
sp:AggressivelyExclude(false)
sp:ExcludeCopyrightNotices(false)
sp:ExcludeTrailingCitations(false)
sp:ExcludeNumerals(false)

sp:Reload()

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-example-header-excluded." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true, "Danielson-Bryan 1 Test")

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-example-partial-excluded." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true,
    "Where:", "RP Number of characters, in this context being letters, numbers, and punctuation (except for sentence-ending punctuation, such as periods)")

sp:ShowProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "exclusion-example-all-excluded." .. FileExtension, 1051, 1058, 1058)
sp:CloseProperties()

sp:AggressivelyExclude(true)

sp:Reload()

sp:ShowProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "exclusion-example-aggressive-list." .. FileExtension, 1051, 1051, 1058)
sp:CloseProperties()

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-example-where-excluded." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true, "Where:")

sp:SetIncludeIncompleteTolerance(20)

sp:Reload()

sp:ShowProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "exclusion-example-incomp-length-value." .. FileExtension, 1076, 1077, 1077)
sp:CloseProperties()

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-example-long-sentence-excluded." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true,
    "RP Number of characters, in this context being letters, numbers, and punctuation (except for sentence-ending punctuation, such as periods)")

sp:ScrollTextWindow(HighlightedReportType.GrammarHighlightedIssues, "Danielson, Wayne A., and")
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-example-copyright-included." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true,
    "©2011 Oleander Software, all rights reserved.")

sp:ExcludeCopyrightNotices(true)

sp:Reload()

sp:ShowProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "exclusion-example-copyright-excluded." .. FileExtension, 1054, 1054, 1058)
sp:CloseProperties()

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-example-copyright-excluded-now." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true,
    "©2011 Oleander Software, all rights reserved.")

sp:ExcludeFileAddress(true)

sp:Reload()

sp:ShowProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "exclusion-example-url-excluded." .. FileExtension, 1056, 1056, 1058)
sp:CloseProperties()

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-example-url-excluded-now." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true,
    "http://www.jstor.org/")

sp:ExcludeProperNouns(true)
sp:ExcludeNumerals(true)

sp:Reload()

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-example-numbers-excluded-now." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true, "206", "1105")

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-example-proper-excluded-now." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true,
    "Danielson-Bryan", "Farr-Jenkins-Paterson", "UNIVAC")

sp:ShowProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "exclusion-example-edit-list-button." .. FileExtension, 1052, 1052, 1052)
sp:CloseProperties()

-- Exclusion word list
----------------------
ScreenshotLib.ShowEditWordListDlg(ScreenshotProjectsFolder .. "ExclusionWords.txt")
ScreenshotLib.SnapScreenshotWithAnnotation(ImagePath .. "exclusion-example-custom-words." .. FileExtension, "ExclusionWords.txt", 9000)
ScreenshotLib.CloseEditWordListDlg()

sp:SetPhraseExclusionList(ScreenshotProjectsFolder .. "ExclusionWords.txt")

sp:Reload()

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-example-custom-words-excluded-now." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true, "sentence length", "punctuation")

sp:ExcludeTrailingCitations(true)

sp:Reload()

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-example-citation-excluded." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true,
    "Danielson, Wayne A., and Sam Dunn Bryan.  “Computer Automation of Two Readability Formulas.”  Journalism Quarterly.  40 (1963): 201-206.  Print.")

sp:SelectWindow(SideBarSection.WordsBreakdown, ListType.ThreePlusSyllableWords)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "ribbon-edit-list-options-with-exclude-selected." .. FileExtension, 0, RibbonButtonBar.Edit)

sp:Close()

-- excluded tags example
------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Press Release.rtf")

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-tags-example-list-excluded." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true,
   "<Marketing representative name>", "<Company>", "<Company slogan>", "<Copyright info>")

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-tags-example-not-excluding." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true,
    "<Platforms>",
    "<Note that this is not intended for general distribution. (9)  This is a preliminary draft, final approval pending>")

sp:SetBlockExclusionTags("<>")
sp:Reload()

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "exclusion-tags-example-excluding." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true,
    "<Platforms>", "<Note that this is not intended for general distribution.  This is a preliminary draft, final approval pending>")

sp:ShowProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "exclusion-tags-example-options." .. FileExtension, 1078, 1079, 1078)
sp:CloseProperties()

sp:Close(false)

-- sentence map example
-----------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Importing and Exporting Fixed-Width Data with R.docx")
sp:SetParagraphsParsingMethod(ParagraphParse.EachNewLineIsAParagraph)
sp:ExportGraph(GraphType.SentenceHeatmap, ImagePath .. "fixed-with-r-sentencemap." .. FileExtension)
sp:Close()

sp = StandardProject(ScreenshotProjectsFolder .. "Instructional Disclaimer.odt")
sp:SetParagraphsParsingMethod(ParagraphParse.EachNewLineIsAParagraph)
sp:ExportGraph(GraphType.SentenceHeatmap, ImagePath .. "disclaimer-sentencemap." .. FileExtension)
sp:Close()

sp = StandardProject(ScreenshotProjectsFolder .. "A Christmas Carol.txt")
sp:SetParagraphsParsingMethod(ParagraphParse.EachNewLineIsAParagraph)
sp:ExportGraph(GraphType.SentenceHeatmap, ImagePath .. "christmas-carol-sentencemap." .. FileExtension)
sp:Close()

-- hard returns example
-----------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Job Posting.odt")
sp:SetParagraphsParsingMethod(ParagraphParse.EachNewLineIsAParagraph)
sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "job-posting-highlighted-text." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true,
    "Title: Data Analyst", "Department: Admin Office", "Pay Grade: A2",
    "Job Posting Close Date: Dec. 14th 2022", "Start Date: Jan. 1st 2022")
sp:Close(false)

-- Flyer example
----------------
sp = StandardProject(ScreenshotProjectsFolder .. "Summer Code Camp.odt")
sp:SetTextExclusion(TextExclusionType.DoNotExcludeAnyText)
sp:AddTest(Test.Forcast)
sp:SelectWindow(SideBarSection.WordsBreakdown, HighlightedReportType.ThreePlusSyllableHighlightedWords)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "summer-code-camp-upper-half1." .. FileExtension,
    HighlightedReportType.ThreePlusSyllableHighlightedWords, true,
    "Proudly presented by Northwoods Software Testing Services")
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "summer-code-camp-upper-half2." .. FileExtension,
    HighlightedReportType.ThreePlusSyllableHighlightedWords, true,
    "No previous computer programming experience required!")
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "summer-code-camp-lower-half." .. FileExtension,
    HighlightedReportType.ThreePlusSyllableHighlightedWords, true,
    "Please fill out and return no later than May 25th")
sp:Close(false)

-- addendum example
-------------------
ScreenshotLib.ShowStandardProjectWizardManualTestSelectionPage("Flesch Reading Ease")
ScreenshotLib.SnapScreenshot(ImagePath .. "addendum-wizard-manual-tests." .. FileExtension)
ScreenshotLib.CloseStandardProjectWizard()

sp = StandardProject(ScreenshotProjectsFolder .. "YA Enterprise Software Symposium.odt")
sp:SetTextExclusion(TextExclusionType.ExcludeIncompleteSentences)
sp:AddTest(Test.Flesch)
sp:ExportGraph(GraphType.Flesch, ImagePath .. "addendum-flesch1." .. FileExtension)
sp:SelectWindow(SideBarSection.WordsBreakdown, HighlightedReportType.ThreePlusSyllableHighlightedWords)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "addendum-highlighted-text1." .. FileExtension,
    HighlightedReportType.ThreePlusSyllableHighlightedWords, true,
    "Preparing young minds for an exciting career* in enterprise-level software development and deployment.")
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "addendum-highlighted-text2." .. FileExtension,
    HighlightedReportType.ThreePlusSyllableHighlightedWords, true,
    "(Previous computer experience recommended).",
    "Participants are entirely responsible for disclosing all food allergies and special dietary needs prior to enrollment.")

sp:SetAppendedDocumentFilePath(ScreenshotProjectsFolder .. "Instructional Disclaimer.odt")

sp:ExportGraph(GraphType.Flesch, ImagePath .. "addendum-flesch2." .. FileExtension)
sp:SelectWindow(SideBarSection.WordsBreakdown, HighlightedReportType.ThreePlusSyllableHighlightedWords)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "addendum-highlighted-text3." .. FileExtension,
    HighlightedReportType.ThreePlusSyllableHighlightedWords, true,
    "Any instruction, advice, or products provided by Company do not include any sort of warranty or guarantee of Customer's perceived quality.  Although Company has provided quality instruction in the past, this performance being represented is historical; past performance is not a reliable indicator of current or future results and Customer's expectations of quality is in no way guaranteed.  The quality of instruction may fluctuate and Company cannot be held responsible for lower level of quality that Customer may experience.  Customer hereby releases Company and Owners from any perceived obligation or expectations related to Customer's subjective measurements of instructional quality.  Customer also releases Company and Owners from any legal responsibilities as a result of interaction with Company's products, services, advice, or third-party catering services (e.g., allergic reactions, inaccurate educational materials, Customer “disappointment”).")
sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "addendum-highlighted-text4." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true,
    "Customer also releases Company and Owners from any legal responsibilities as a result of interaction with Company's products, services, advice, or third-party catering services (e.g., allergic reactions, inaccurate educational materials, Customer “disappointment”).")

-- show a generic set of file paths in the properties dialog and get a screenshot of that
sp:DelayReloading(true)
sp:SetReviewer("Daphne")
sp:SetDocumentFilePath("/home/dmoon/YA Enterprise Software Symposium.odt")
sp:SetAppendedDocumentFilePath("/home/dmoon/Instructional Disclaimer.odt")
sp:ShowProperties(OptionsPageType.ProjectSettings)
ScreenshotLib.SnapScreenshot(ImagePath .. "addendum-properties." .. FileExtension, 1083, 1080, 1080)
sp:CloseProperties()
sp:Close()

-- batch labeling examples
--------------------------
bp = BatchProject(ScreenshotProjectsFolder .. "Statistics Manual Ch. 1.rsbp")
bp:ShowSidebar(false)
bp:SelectWindow(SideBarSection.ReadabilityScores, ListType.BatchRawScores)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "batch-desc-labels-scores." .. FileExtension,
    ListType.BatchRawScores, -1, 2, -1, 2)
bp:Close()

bp = BatchProject(ScreenshotProjectsFolder .. "Topics(Grouped).rsbp")
bp:ShowSidebar(false)
bp:ExportGraph(SideBarSection.ReadabilityScores, GraphType.Fry, ImagePath .. "fry-grouped." .. FileExtension)
bp:ExportGraph(SideBarSection.BoxPlots, Application.GetTestId("fry-test"), ImagePath .. "boxgrouped." .. FileExtension)
bp:SelectWindow(SideBarSection.ReadabilityScores, ListType.BatchRawScores)
bp:SortList(ListType.BatchRawScores, 3, SortOrder.SortAscending)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "batch-group-labels-scores." .. FileExtension,
    ListType.BatchRawScores, -1, 2, -1, 4)
bp:Close()

ScreenshotLib.ShowBatchProjectWizardTextSourcePage(
            Application.GetAbsoluteFilePath(
            Debug.GetScriptFolder(),
            ScreenshotProjectsFolder .. "Statistics Manual Ch. 1"))
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchselectfilessamenames." .. FileExtension, 10009)
-- crop off bottom half
info = Application.GetImageInfo(ImagePath .. "wizardbatchselectfilessamenames." .. FileExtension)
ScreenshotLib.CropScreenshot(ImagePath .. "wizardbatchselectfilessamenames." .. FileExtension, -1, info["Height"]/2)
ScreenshotLib.CloseBatchProjectWizard()

ScreenshotLib.ShowDocGroupSelectDlg(1)
ScreenshotLib.SnapScreenshot(ImagePath .. "group-label-descselected." .. FileExtension)
ScreenshotLib.CloseDocGroupSelectDlg()

ScreenshotLib.ShowDocGroupSelectDlg(2, "Examples")
ScreenshotLib.SnapScreenshot(ImagePath .. "group-label-examples." .. FileExtension)
ScreenshotLib.CloseDocGroupSelectDlg()

ScreenshotLib.ShowDocGroupSelectDlg(2, "Notes")
ScreenshotLib.SnapScreenshot(ImagePath .. "group-label-notes." .. FileExtension)
ScreenshotLib.CloseDocGroupSelectDlg()

-- Difficult words example
--------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Instructional Disclaimer.odt")
sp:AddTest(Test.Fry)
sp:ExportGraph(GraphType.WordCloud, ImagePath .. "example-difficult-words-wordcloud." .. FileExtension)
sp:ExportGraph(GraphType.SyllableHistogram, ImagePath .. "example-difficult-syllables-histogram." .. FileExtension)
sp:ShowcaseKeyItems(true)
sp:ExportGraph(GraphType.SyllableHistogram, ImagePath .. "example-difficult-syllables-histogram-showcased." .. FileExtension)
sp:ExportGraph(GraphType.SyllablePieChart, ImagePath .. "example-difficult-syllables-pie-chart." .. FileExtension)
sp:ExportGraph(GraphType.SentenceHeatmap, ImagePath .. "example-difficult-sentences-heatmap." .. FileExtension)
sp:ExportGraph(GraphType.Fry, ImagePath .. "example-difficult-fry-showcased." .. FileExtension)
sp:Close()

-- "Retro" looking graph example
--------------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Chocolate Eclairs.txt")
sp:SelectWindow(SideBarSection.WordsBreakdown, GraphType.WordBarChart)
sp:DelayReloading(true)
sp:DisplayBarChartLabels(false)
sp:SetGraphTopTitleFont("MONOSPACE")
sp:SetXAxisFont("MONOSPACE")
sp:SetYAxisFont("MONOSPACE")
sp:SetBarChartBarColor("Timberwolf")
sp:SetBarChartBarEffect(BoxEffect.Solid)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-graph-retro." .. FileExtension)
sp:Close()

-- dark mode graph example
--------------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Chocolate Eclairs.txt")
sp:AddTest(Test.Raygor)
sp:SelectWindow(SideBarSection.ReadabilityScores, GraphType.Raygor)
sp:SetGraphBackgroundColor("black")
sp:ExportGraph(GraphType.Raygor, ImagePath .. "example-dark-graph." .. FileExtension)
sp:Close()

-- Graph editing example (selective colorization)
-------------------------------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Chocolate Eclairs.txt")

sp:SelectWindow(SideBarSection.WordsBreakdown, GraphType.WordBarChart)

-- remove the bar labels
sp:DisplayBarChartLabels(false)

-- change bar chart effects
sp:SetGraphCommonImage(ScreenshotProjectsFolder .. "BookStore.png")
sp:SetBarChartBarEffect(BoxEffect.CommonImage)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-barchart-common-image." .. FileExtension)

-- change the background to an image
sp:SetPlotBackgroundImage(ScreenshotProjectsFolder .. "BookStore.png")
sp:SetPlotBackgroundImageFit(ImageFit.CropAndCenter)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-graph-background-image-for-common-image." .. FileExtension)

-- apply an effect to the background image
sp:SetPlotBackgroundImageEffect(ImageEffect.Grayscale)
sp:SetPlotBackgroundImageOpacity(100)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-graph-background-image-effect-grayscaled." .. FileExtension)

sp:Close()

-- Graph editing example
------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Chocolate Eclairs.txt")

sp:SelectWindow(SideBarSection.WordsBreakdown, GraphType.WordBarChart)
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-graph-default." .. FileExtension)

-- sort the graph
sp:SortGraph(GraphType.WordBarChart, SortOrder.SortAscending)
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-graph-sorted." .. FileExtension)

-- change orientation
sp:SetBarChartOrientation(Orientation.Vertical)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-barchart-orientation-changed." .. FileExtension)

-- change bar chart effects
sp:SetBarChartBarEffect(BoxEffect.ThickWatercolor)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-barchart-effects-changed." .. FileExtension)

-- remove the bar labels
sp:DisplayBarChartLabels(false)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-barchart-no-labels-changed." .. FileExtension)

-- change color of background
sp:SetGraphBackgroundColor("AntiqueWhite")
sp:ApplyGraphBackgroundFade(true)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-graph-background-color-changed." .. FileExtension)

-- change the background to an image
sp:SetPlotBackgroundImage(ScreenshotProjectsFolder .. "bookstore.png")
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-graph-background-image-changed." .. FileExtension)

-- apply an effect to the background image
sp:SetPlotBackgroundImageEffect(ImageEffect.OilPainting)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-graph-background-image-effect-changed." .. FileExtension)

-- add watermark
sp:SetWatermark("INTERNAL USE ONLY\nProcessed on @DATE@")
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-watermark-changed." .. FileExtension)

-- add logo
sp:SetGraphLogoImage(ScreenshotProjectsFolder .. "CoffeeLogo.png")
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "example-logo-changed." .. FileExtension)

sp:Close()

-- Editor and realtime examples
-------------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Termination Checklist.rsp")
sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "termchecklistgrammar." .. FileExtension)
ScreenshotLib.ShowEditorTextDlg(1000, 300)
ScreenshotLib.SnapScreenshot(ImagePath .. "editorwithcontent." .. FileExtension)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "editorsaveribbon." .. FileExtension, 0, RibbonButtonBar.Document)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "editorviewribbon." .. FileExtension, 0, RibbonButtonBar.View)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "editorclipboardandeditribbon." .. FileExtension, 0, RibbonButtonBar.Clipboard, RibbonButtonBar.Edit)
ScreenshotLib.CloseEditorTextDlg()

sp:SetTextStorageMethod(TextStorage.LoadFromExternalDocument)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "termchecklist-realtime." .. FileExtension, 0, RibbonButtonBar.Project)

sp:DelayReloading(true)
sp:SetDocumentFilePath("/home/LSternin/Termination Checklist.odt") -- use generic filepath
sp:SetReviewer("Lilith")
sp:SetStatus("Draft")
sp:ShowProperties(OptionsPageType.ProjectSettings)
ScreenshotLib.SnapScreenshot(ImagePath .. "realtime-options." .. FileExtension, 1085, 1087, 1088)
sp:CloseProperties()
sp.Close()

-- Website example
------------------
sp = StandardProject(ScreenshotProjectsFolder .. "LibreOffice.rsp")
sp:ScrollTextWindow(HighlightedReportType.ThreePlusSyllableHighlightedWords, "processed as indicated in the YouTube privacy policy")
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "incompletesentences." .. FileExtension, 1)

sp:DelayReloading(true) -- prevent reloading website in case we are offline
sp:SetTextStorageMethod(TextStorage.LoadFromExternalDocument)
sp:ShowProperties(OptionsPageType.ProjectSettings)
ScreenshotLib.SnapScreenshot(ImagePath .. "reloadwebpage." .. FileExtension, 1085, -1, 1083)
sp:CloseProperties()

sp:ShowProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "excludeincompletesentences." .. FileExtension, 1036, -1, 1084)
sp:CloseProperties()

sp:SetTextExclusion(TextExclusionType.DoNotExcludeAnyText)
sp:ShowProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "donotexcludeincompletesentences." .. FileExtension, 1036, -1, 1084)
sp:CloseProperties()

sp:Close()
Application.RemoveAllCustomTests()

-- rescale images and convert from BMP to PNG
bitmaps = Application.FindFiles(ImagePath, "*." .. FileExtension)

for i,v in ipairs(bitmaps)
do
    ScreenshotLib.ConvertImage(v, 1000, 1000)
end

ScreenshotLib.ShowScriptEditor()
