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

#ifndef LUASCREENSHOT_H
#define LUASCREENSHOT_H

#include "lua.hpp"

// NOLINTBEGIN(readability-identifier-length)
// NOLINTBEGIN(readability-implicit-bool-conversion)
// cppcheck-suppress-begin functionConst

/// @private
enum class ProjectScreenshotCropMode
    {
    NoCrop,
    CropToSidebarBottom,
    CropToSidebarSelectedItem
    };

/// @private
/// @internal This is documented in the readability-studio-api help.
namespace LuaScripting
    {
    int ShowScriptEditor(lua_State* L);
    int SnapScreenshot(lua_State* L);
    int SnapScreenshotWithAnnotation(lua_State* L);
    int SnapScreenshotOfActiveProject(lua_State* L);
    int SnapScreenshotOfDialogWithPropertyGrid(lua_State* L);
    int SnapScreenshotOfTextWindow(lua_State* L);
    int SnapScreenshotOfListControl(lua_State* L);
    int SnapScreenshotOfRibbon(lua_State* L);
    int CropScreenshot(lua_State* L);
    int HighlightScreenshot(lua_State* L);
    int ConvertImage(lua_State* L);
    int ShowStandardProjectWizardLanguagePage(lua_State* L);
    int ShowStandardProjectWizardTextSourcePage(lua_State* L);
    int ShowStandardProjectWizardTextFromFilePage(lua_State* L);
    int SetStandardProjectWizardTextFromFilePath(lua_State* L);
    int ShowStandardProjectWizardTextEntryPage(lua_State* L);
    int ShowStandardProjectWizardTestRecommendationPage(lua_State* L);
    int ShowStandardProjectWizardTestByIndustryPage(lua_State* L);
    int ShowStandardProjectWizardTestByDocumentTypePage(lua_State* L);
    int ShowStandardProjectWizardDocumentStructurePage(lua_State* L);
    int ShowStandardProjectWizardManualTestSelectionPage(lua_State* L);
    int ShowStandardProjectWizardTestByBundlePage(lua_State* L);
    int CloseStandardProjectWizard(lua_State* L);
    int ShowBatchProjectWizardLanguagePage(lua_State* L);
    int ShowBatchProjectWizardTextSourcePage(lua_State* L);
    int ShowBatchProjectWizardTestRecommendationPage(lua_State* L);
    int ShowBatchProjectWizardTestByIndustryPage(lua_State* L);
    int ShowBatchProjectWizardTestByDocumentTypePage(lua_State* L);
    int ShowBatchProjectWizardDocumentStructurePage(lua_State* L);
    int ShowBatchProjectWizardManualTestSelectionPage(lua_State* L);
    int BatchProjectWizardTextSourcePageSetFiles(lua_State* L);
    int CloseBatchProjectWizard(lua_State* L);
    int ShowTestBundleDialog(lua_State* L);
    int CloseTestBundleDialog(lua_State* L);
    int ShowCustomTestDialogFunctionBrowser(lua_State* L);
    int ShowCustomTestDialogGeneralSettings(lua_State* L);
    int ShowCustomTestDialogFamiliarWords(lua_State* L);
    int ShowCustomTestDialogProperNounsAndNumbers(lua_State* L);
    int ShowCustomTestDialogClassification(lua_State* L);
    int SetCustomTestDialogDocumentTypes(lua_State* L);
    int SetCustomTestDialogIndustries(lua_State* L);
    int SetCustomTestDialogIncludedLists(lua_State* L);
    int CloseCustomTestDialog(lua_State* L);
    int ShowOptions(lua_State* L);
    int CloseOptions(lua_State* L);
    int ShowPrinterHeaderFooterOptions(lua_State* L);
    int ClosePrinterHeaderFooterOptions(lua_State* L);
    int ShowListExportDlg(lua_State* L);
    int CloseListExportDlg(lua_State* L);
    int CloseListViewItemDlg(lua_State* L);
    int ShowListViewItemDlg(lua_State* L);
    int ShowGetDirDlg(lua_State* L);
    int ShowGetArchiveDlg(lua_State* L);
    int CloseGetDirDlg(lua_State* L);
    int CloseGetArchiveDlg(lua_State* L);
    int ShowSelectProjectTypeDlg(lua_State* L);
    int CloseSelectProjectTypeDlg(lua_State* L);
    int ShowWebHarvesterDlg(lua_State* L);
    int CloseWebHarvesterDlg(lua_State* L);
    int ShowFilteredTextPreviewDlg(lua_State* L);
    int CloseFilteredTextPreviewDlg(lua_State* L);
    int ShowSortListDlg(lua_State* L);
    int CloseSortListDlg(lua_State* L);
    int ShowEditWordListDlg(lua_State* L);
    int CloseEditWordListDlg(lua_State* L);
    int ShowDocGroupSelectDlg(lua_State* L);
    int CloseDocGroupSelectDlg(lua_State* L);
    int ShowEditorTextDlg(lua_State* L);
    int CloseEditorTextDlg(lua_State* L);

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    static const luaL_Reg ScreenshotLib[] = {
        { "ShowEditorTextDlg", ShowEditorTextDlg },
        { "CloseEditorTextDlg", CloseEditorTextDlg },
        { "ShowEditWordListDlg", ShowEditWordListDlg },
        { "CloseEditWordListDlg", CloseEditWordListDlg },
        { "ShowDocGroupSelectDlg", ShowDocGroupSelectDlg },
        { "CloseDocGroupSelectDlg", CloseDocGroupSelectDlg },
        { "ShowSortListDlg", ShowSortListDlg },
        { "CloseSortListDlg", CloseSortListDlg },
        { "ShowFilteredTextPreviewDlg", ShowFilteredTextPreviewDlg },
        { "CloseFilteredTextPreviewDlg", CloseFilteredTextPreviewDlg },
        { "ShowWebHarvesterDlg", ShowWebHarvesterDlg },
        { "CloseWebHarvesterDlg", CloseWebHarvesterDlg },
        { "ShowSelectProjectTypeDlg", ShowSelectProjectTypeDlg },
        { "CloseSelectProjectTypeDlg", CloseSelectProjectTypeDlg },
        { "ShowGetDirDlg", ShowGetDirDlg },
        { "CloseGetDirDlg", CloseGetDirDlg },
        { "ShowGetArchiveDlg", ShowGetArchiveDlg },
        { "CloseGetArchiveDlg", CloseGetArchiveDlg },
        { "ShowListViewItemDlg", ShowListViewItemDlg },
        { "CloseListViewItemDlg", CloseListViewItemDlg },
        { "ShowListExportDlg", ShowListExportDlg },
        { "CloseListExportDlg", CloseListExportDlg },
        { "ShowPrinterHeaderFooterOptions", ShowPrinterHeaderFooterOptions },
        { "ClosePrinterHeaderFooterOptions", ClosePrinterHeaderFooterOptions },
        { "ShowStandardProjectWizardLanguagePage", ShowStandardProjectWizardLanguagePage },
        { "ShowStandardProjectWizardTextSourcePage", ShowStandardProjectWizardTextSourcePage },
        { "ShowStandardProjectWizardTextFromFilePage", ShowStandardProjectWizardTextFromFilePage },
        { "SetStandardProjectWizardTextFromFilePath", SetStandardProjectWizardTextFromFilePath },
        { "ShowStandardProjectWizardTextEntryPage", ShowStandardProjectWizardTextEntryPage },
        { "ShowStandardProjectWizardTestRecommendationPage",
          ShowStandardProjectWizardTestRecommendationPage },
        { "ShowStandardProjectWizardTestByIndustryPage",
          ShowStandardProjectWizardTestByIndustryPage },
        { "ShowStandardProjectWizardTestByDocumentTypePage",
          ShowStandardProjectWizardTestByDocumentTypePage },
        { "ShowStandardProjectWizardManualTestSelectionPage",
          ShowStandardProjectWizardManualTestSelectionPage },
        { "ShowStandardProjectWizardTestByBundlePage", ShowStandardProjectWizardTestByBundlePage },
        { "ShowStandardProjectWizardDocumentStructurePage",
          ShowStandardProjectWizardDocumentStructurePage },
        { "ShowBatchProjectWizardLanguagePage", ShowBatchProjectWizardLanguagePage },
        { "ShowBatchProjectWizardTextSourcePage", ShowBatchProjectWizardTextSourcePage },
        { "ShowBatchProjectWizardTestRecommendationPage",
          ShowBatchProjectWizardTestRecommendationPage },
        { "ShowBatchProjectWizardTestByIndustryPage", ShowBatchProjectWizardTestByIndustryPage },
        { "ShowBatchProjectWizardTestByDocumentTypePage",
          ShowBatchProjectWizardTestByDocumentTypePage },
        { "ShowBatchProjectWizardDocumentStructurePage",
          ShowBatchProjectWizardDocumentStructurePage },
        { "ShowBatchProjectWizardManualTestSelectionPage",
          ShowBatchProjectWizardManualTestSelectionPage },
        { "BatchProjectWizardTextSourcePageSetFiles", BatchProjectWizardTextSourcePageSetFiles },
        { "CloseBatchProjectWizard", CloseBatchProjectWizard },
        { "CloseStandardProjectWizard", CloseStandardProjectWizard },
        { "ShowOptions", ShowOptions },
        { "CloseOptions", CloseOptions },
        { "ShowTestBundleDialog", ShowTestBundleDialog },
        { "CloseTestBundleDialog", CloseTestBundleDialog },
        { "ShowCustomTestDialogFunctionBrowser", ShowCustomTestDialogFunctionBrowser },
        { "ShowCustomTestDialogGeneralSettings", ShowCustomTestDialogGeneralSettings },
        { "ShowCustomTestDialogProperNounsAndNumbers", ShowCustomTestDialogProperNounsAndNumbers },
        { "ShowCustomTestDialogFamiliarWords", ShowCustomTestDialogFamiliarWords },
        { "ShowCustomTestDialogClassification", ShowCustomTestDialogClassification },
        { "SetCustomTestDialogIncludedLists", SetCustomTestDialogIncludedLists },
        { "SetCustomTestDialogDocumentTypes", SetCustomTestDialogDocumentTypes },
        { "SetCustomTestDialogIndustries", SetCustomTestDialogIndustries },
        { "CloseCustomTestDialog", CloseCustomTestDialog },
        { "SnapScreenshot", SnapScreenshot },
        { "SnapScreenshotWithAnnotation", SnapScreenshotWithAnnotation },
        { "SnapScreenshotOfActiveProject", SnapScreenshotOfActiveProject },
        { "SnapScreenshotOfDialogWithPropertyGrid", SnapScreenshotOfDialogWithPropertyGrid },
        { "SnapScreenshotOfTextWindow", SnapScreenshotOfTextWindow },
        { "SnapScreenshotOfListControl", SnapScreenshotOfListControl },
        { "SnapScreenshotOfRibbon", SnapScreenshotOfRibbon },
        { "CropScreenshot", CropScreenshot },
        { "HighlightScreenshot", HighlightScreenshot },
        { "ConvertImage", ConvertImage },
        { "ShowScriptEditor", ShowScriptEditor },
        { nullptr, nullptr }
    };
    } // namespace LuaScripting

// cppcheck-suppress-end functionConst
// NOLINTEND(readability-implicit-bool-conversion)
// NOLINTEND(readability-identifier-length)

#endif // LUASCREENSHOT_H
