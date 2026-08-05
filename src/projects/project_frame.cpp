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

#include "project_frame.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/imagemergedlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/opacitydlg.h"
#include "../app/readability_app.h"
#include "../ui/dialogs/edit_word_list_dlg.h"
#include "../ui/dialogs/tools_options_dlg.h"
#include "base_project.h"
#include "base_project_view.h"
#include "batch_project_doc.h"
#include "standard_project_doc.h"
#include "wx/generic/numdlgg.h"

wxDECLARE_APP(ReadabilityApp);

//---------------------------------------------------
ProjectDocChildFrame::ProjectDocChildFrame(wxDocument* doc, wxView* view, wxFrame* parent,
                                           wxWindowID id, const wxString& title,
                                           const wxPoint& pos /*= wxDefaultPosition*/,
                                           const wxSize& size /*= wxDefaultSize*/,
                                           long style /*= wxDEFAULT_FRAME_STYLE*/,
                                           const wxString& name /*= wxASCII_STR(wxFrameNameStr)*/)
    : wxDocChildFrame(doc, view, parent, id, title, pos, size, style, name)
    {
    // ribbon drop buttons (that will pop up a menu)
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnHistoBarsLabelsDropdown,
         this, XRCID("ID_EDIT_HISTOBAR_LABELS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnExclusionTagsDropdown,
         this, XRCID("ID_EXCLUSION_TAGS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED,
         &ProjectDocChildFrame::OnNumeralSyllabicationDropdown, this,
         XRCID("ID_NUMERAL_SYLLABICATION"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnLineEndsDropdown, this,
         XRCID("ID_LINE_ENDS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnLongSentencesDropdown,
         this, XRCID("ID_SENTENCE_LENGTHS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnOpenDropdown, this,
         wxID_OPEN);
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnSaveDropdown, this,
         XRCID("ID_SAVE_PROJECT"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnExportDropdown, this,
         XRCID("ID_SAVE_ITEM"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnZoomButtonDropdown, this,
         wxID_ZOOM_IN);
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED,
         &ProjectDocChildFrame::OnEditGraphBackgroundDropdown, this,
         XRCID("ID_EDIT_GRAPH_BACKGROUND"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnHistoBarStyleDropdown,
         this, XRCID("ID_EDIT_HISTOGRAM_BAR_STYLE"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnBarOrientationDropdown,
         this, XRCID("ID_EDIT_BAR_ORIENTATION"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnBarStyleDropdown, this,
         XRCID("ID_EDIT_BAR_STYLE"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnBoxStyleDropdown, this,
         XRCID("ID_EDIT_BOX_STYLE"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnEditGraphFontsDropdown,
         this, XRCID("ID_EDIT_GRAPH_FONTS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnWordListDropdown, this,
         XRCID("ID_WORD_LISTS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnBlankGraphDropdown, this,
         XRCID("ID_BLANK_GRAPHS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnCopyDropdown, this,
         wxID_COPY);
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnGradeScaleDropdown, this,
         XRCID("ID_GRADE_SCALES"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnGraphSortDropdown, this,
         XRCID("ID_GRAPH_SORT"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnPrintDropdown, this,
         wxID_PRINT);
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnCustomTestsDropdown, this,
         XRCID("ID_CUSTOM_TESTS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnTestBundlesDropdown, this,
         XRCID("ID_TEST_BUNDLES"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnExampleDropdown, this,
         XRCID("ID_EXAMPLES"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnDictionaryDropdown, this,
         XRCID("ID_EDIT_DICTIONARY"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnPrimaryAgeTestsDropdown,
         this, XRCID("ID_PRIMARY_AGE_TESTS_BUTTON"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnSecondaryAgeTestsDropdown,
         this, XRCID("ID_SECONDARY_AGE_TESTS_BUTTON"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnAdultTestsDropdown, this,
         XRCID("ID_ADULT_TESTS_BUTTON"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED,
         &ProjectDocChildFrame::OnSecondLanguageTestsDropdown, this,
         XRCID("ID_SECOND_LANGUAGE_TESTS_BUTTON"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &ProjectDocChildFrame::OnTextExclusionDropdown,
         this, XRCID("ID_TEXT_EXCLUSION"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED,
         &ProjectDocChildFrame::OnEditGraphRaygorStyleDropdown, this,
         XRCID("ID_EDIT_GRAPH_RAYGOR_STYLE"));
    // ribbon buttons
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnEditDictionaryButton, this,
         XRCID("ID_EDIT_DICTIONARY"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnProjectSettings, this,
         wxID_PROPERTIES);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnToolsOptions, this,
         wxID_PREFERENCES);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnEditStatsReportButton, this,
         XRCID("ID_EDIT_STATS_REPORT"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnBoxPlotShowAllPointsButton, this,
         XRCID("ID_BOX_PLOT_DISPLAY_ALL_POINTS"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnBoxPlotShowLabelsButton, this,
         XRCID("ID_BOX_PLOT_DISPLAY_LABELS"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnIgnoreBlankLines, this,
         XRCID("ID_IGNORE_BLANK_LINES"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnIgnoreIndenting, this,
         XRCID("ID_IGNORE_INDENTING"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnStrictCapitalization, this,
         XRCID("ID_SENTENCES_CAPITALIZED"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnIgnoreCitations, this,
         XRCID("ID_EXCLUDE_TRAILING_CITATIONS"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnIgnoreFileAddresses, this,
         XRCID("ID_EXCLUDE_FILE_ADDRESSES"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnIgnoreNumerals, this,
         XRCID("ID_EXCLUDE_NUMERALS"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnAggressivelyExclude, this,
         XRCID("ID_EXCLUDE_AGGRESSIVELY"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnIgnoreCopyrights, this,
         XRCID("ID_EXCLUDE_COPYRIGHT_NOTICES"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnIgnoreProperNouns, this,
         XRCID("ID_EXCLUDE_PROPER_NOUNS"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnExcludeWordsList, this,
         XRCID("ID_EXCLUDE_WORD_LIST"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnIncompleteThreshold, this,
         XRCID("ID_INCOMPLETE_THRESHOLD"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnDropShadow, this,
         XRCID("ID_DROP_SHADOW"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnGraphLogo, this,
         XRCID("ID_EDIT_LOGO"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnEnglishLabels, this,
         XRCID("ID_USE_ENGLISH_LABELS"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnFleschConnectLinesButton, this,
         XRCID("ID_FLESCH_DISPLAY_LINES"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnBarLabelsButton, this,
         XRCID("ID_EDIT_BAR_LABELS"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnDocumentRefresh, this,
         XRCID("ID_DOCUMENT_REFRESH"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnEditGraphColorScheme, this,
         XRCID("ID_EDIT_GRAPH_COLOR_SCHEME"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnInvalidRegionColor, this,
         XRCID("ID_INVALID_REGION_COLOR"));
    // menus
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnPrintWatermark, this, XRCID("ID_EDIT_WATERMARK"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnRaygorStyleSelected, this,
         XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnRaygorStyleSelected, this,
         XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnRaygorStyleSelected, this,
         XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphColor, this, XRCID("ID_EDIT_BAR_COLOR"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphColor, this,
         XRCID("ID_EDIT_HISTOGRAM_BAR_COLOR"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphColor, this, XRCID("ID_EDIT_BOX_COLOR"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphColor, this, XRCID("ID_EDIT_PLOT_BKCOLOR"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphColor, this, XRCID("ID_EDIT_GRAPH_BKCOLOR"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnDocumentRefreshMenu, this,
         XRCID("ID_DOCUMENT_REFRESH"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnLongSentencesOptions, this,
         XRCID("ID_LS_LONGER_THAN"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnLongSentencesOptions, this,
         XRCID("ID_LS_OUTLIER_RANGE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnNumeralSyllabicationOptions, this,
         XRCID("ID_NUMSYL_ONE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnNumeralSyllabicationOptions, this,
         XRCID("ID_NUMSYL_EACH_DIGIT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTextExclusionOptions, this,
         XRCID("ID_TE_ALL_INCOMPLETE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTextExclusionOptions, this,
         XRCID("ID_TE_NO_EXCLUDE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTextExclusionOptions, this,
         XRCID("ID_TE_ALL_INCOMPLETE_EXCEPT_HEADERS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnExclusionTagsOptions, this,
         XRCID("ID_EXCLUSION_TAGS_NOT_ENABLED"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnExclusionTagsOptions, this,
         XRCID("ID_EXCLUSION_TAGS_CAROTS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnExclusionTagsOptions, this,
         XRCID("ID_EXCLUSION_TAGS_ANGLES"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnExclusionTagsOptions, this,
         XRCID("ID_EXCLUSION_TAGS_BRACES"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnExclusionTagsOptions, this,
         XRCID("ID_EXCLUSION_TAGS_CURLIES"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnExclusionTagsOptions, this,
         XRCID("ID_EXCLUSION_TAGS_PARANS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnLineEndOptions, this,
         XRCID("ID_LE_ONLY_AFTER_VALID_SENTENCE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnLineEndOptions, this,
         XRCID("ID_LE_ALWAYS_NEW_PARAGRAPH"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditDictionary, this, XRCID("ID_EDIT_DICTIONARY"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditEnglishDictionary, this,
         XRCID("ID_EDIT_ENGLISH_DICTIONARY"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphOpacity, this, XRCID("ID_EDIT_BAR_OPACITY"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarOrientationSelected, this,
         XRCID("ID_BAR_HORIZONTAL"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarOrientationSelected, this,
         XRCID("ID_BAR_VERTICAL"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarLabelSelected, this,
         XRCID("ID_HISTOBAR_LABELS_PERCENTAGE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarLabelSelected, this,
         XRCID("ID_HISTOBAR_LABELS_COUNT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarLabelSelected, this,
         XRCID("ID_HISTOBAR_LABELS_COUNT_AND_PERCENT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarLabelSelected, this,
         XRCID("ID_HISTOBAR_NO_LABELS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphOpacity, this,
         XRCID("ID_EDIT_HISTOBAR_OPACITY"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphOpacity, this, XRCID("ID_EDIT_BOX_OPACITY"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphOpacity, this,
         XRCID("ID_EDIT_PLOT_BKIMAGE_OPACITY"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphOpacity, this,
         XRCID("ID_EDIT_PLOT_BKCOLOR_OPACITY"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditPlotBackgroundImage, this,
         XRCID("ID_EDIT_PLOT_BKIMAGE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMergePlotBackgroundImages, this,
         XRCID("ID_MERGE_PLOT_BKIMAGES"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphFont, this, XRCID("ID_EDIT_Y_AXIS_FONT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphFont, this, XRCID("ID_EDIT_X_AXIS_FONT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphFont, this,
         XRCID("ID_EDIT_TOP_TITLES_FONT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphFont, this,
         XRCID("ID_EDIT_BOTTOM_TITLES_FONT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphFont, this,
         XRCID("ID_EDIT_LEFT_TITLES_FONT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditGraphFont, this,
         XRCID("ID_EDIT_RIGHT_TITLES_FONT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditDictionaryProjectSettings, this,
         XRCID("ID_EDIT_DICTIONARY_PROJECT_SETTINGS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnAddCustomTestBundle, this,
         XRCID("ID_ADD_CUSTOM_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditCustomTestBundle, this,
         XRCID("ID_EDIT_CUSTOM_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnRemoveCustomTestBundle, this,
         XRCID("ID_REMOVE_CUSTOM_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTestBundle, this,
         XRCID("ID_ADD_CHILDRENS_PUBLISHING_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTestBundle, this,
         XRCID("ID_ADD_ADULT_PUBLISHING_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTestBundle, this,
         XRCID("ID_ADD_CHILDRENS_HEALTHCARE_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTestBundle, this,
         XRCID("ID_ADD_ADULT_HEALTHCARE_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTestBundle, this,
         XRCID("ID_ADD_MILITARY_COVERNMENT_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTestBundle, this,
         XRCID("ID_ADD_SECOND_LANGUAGE_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTestBundle, this,
         XRCID("ID_ADD_BROADCASTING_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTestBundle, this,
         XRCID("ID_GENERAL_DOCUMENT_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTestBundle, this,
         XRCID("ID_TECHNICAL_DOCUMENT_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTestBundle, this,
         XRCID("ID_NONNARRATIVE_DOCUMENT_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTestBundle, this,
         XRCID("ID_ADULT_LITERATURE_DOCUMENT_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnTestBundle, this,
         XRCID("ID_CHILDRENS_LITERATURE_DOCUMENT_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnAddCustomTest, this, XRCID("ID_ADD_CUSTOM_TEST"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnAddCustomTest, this,
         XRCID("ID_ADD_CUSTOM_NEW_DALE_CHALL_TEST"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnAddCustomTest, this,
         XRCID("ID_ADD_CUSTOM_SPACHE_TEST"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnAddCustomTest, this,
         XRCID("ID_ADD_CUSTOM_HARRIS_JACOBSON_TEST"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditCustomTest, this, XRCID("ID_EDIT_CUSTOM_TEST"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnRemoveCustomTest, this,
         XRCID("ID_REMOVE_CUSTOM_TEST"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnFindMenu, this, wxID_FIND);
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnFindNext, this, XRCID("ID_FIND_NEXT"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectDocChildFrame::OnShowcaseKeyItems, this,
         XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarStyleSelected, this, XRCID("ID_BAR_STYLE_SOLID"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarStyleSelected, this, XRCID("ID_BAR_STYLE_GLASS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarStyleSelected, this, XRCID("ID_BAR_STYLE_BTOT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarStyleSelected, this, XRCID("ID_BAR_STYLE_TTOB"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarStyleSelected, this,
         XRCID("ID_BAR_STYLE_STIPPLE_IMAGE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarStyleSelected, this,
         XRCID("ID_BAR_STYLE_STIPPLE_SHAPE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarStyleSelected, this,
         XRCID("ID_BAR_STYLE_WATERCOLOR"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarStyleSelected, this,
         XRCID("ID_BAR_STYLE_THICK_WATERCOLOR"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarStyleSelected, this, XRCID("ID_BAR_STYLE_MARKER"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarStyleSelected, this,
         XRCID("ID_BAR_STYLE_COMMON_IMAGE"));

    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarStyleSelected, this,
         XRCID("ID_HISTOGRAM_BAR_STYLE_SOLID"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarStyleSelected, this,
         XRCID("ID_HISTOGRAM_BAR_STYLE_GLASS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarStyleSelected, this,
         XRCID("ID_HISTOGRAM_BAR_STYLE_BTOT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarStyleSelected, this,
         XRCID("ID_HISTOGRAM_BAR_STYLE_TTOB"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarStyleSelected, this,
         XRCID("ID_HISTOGRAM_BAR_STYLE_STIPPLE_IMAGE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarStyleSelected, this,
         XRCID("ID_HISTOGRAM_BAR_STYLE_STIPPLE_SHAPE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarStyleSelected, this,
         XRCID("ID_HISTOGRAM_BAR_STYLE_WATERCOLOR"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarStyleSelected, this,
         XRCID("ID_HISTOGRAM_BAR_STYLE_THICK_WATERCOLOR"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarStyleSelected, this,
         XRCID("ID_HISTOGRAM_BAR_STYLE_MARKER"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarStyleSelected, this,
         XRCID("ID_HISTOGRAM_BAR_STYLE_COMMON_IMAGE"));

    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBoxStyleSelected, this, XRCID("ID_BOX_STYLE_SOLID"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBoxStyleSelected, this, XRCID("ID_BOX_STYLE_GLASS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBoxStyleSelected, this, XRCID("ID_BOX_STYLE_LTOR"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBoxStyleSelected, this, XRCID("ID_BOX_STYLE_RTOL"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBoxStyleSelected, this,
         XRCID("ID_BOX_STYLE_STIPPLE_IMAGE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBoxStyleSelected, this,
         XRCID("ID_BOX_STYLE_STIPPLE_SHAPE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBoxStyleSelected, this,
         XRCID("ID_BOX_STYLE_WATERCOLOR"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBoxStyleSelected, this,
         XRCID("ID_BOX_STYLE_THICK_WATERCOLOR"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBoxStyleSelected, this, XRCID("ID_BOX_STYLE_MARKER"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBoxStyleSelected, this,
         XRCID("ID_BOX_STYLE_COMMON_IMAGE"));

    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBoxSelectStippleBrush, this,
         XRCID("ID_BOX_SELECT_STIPPLE_IMAGE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBoxSelectStippleShape, this,
         XRCID("ID_BOX_BAR_SELECT_STIPPLE_SHAPE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBoxSelectCommonImage, this,
         XRCID("ID_BOX_SELECT_COMMON_IMAGE"));

    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarSelectStippleBrush, this,
         XRCID("ID_HISTOGRAM_BAR_SELECT_BRUSH"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarSelectStippleShape, this,
         XRCID("ID_HISTOGRAM_BAR_SELECT_STIPPLE_SHAPE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnHistoBarSelectCommonImage, this,
         XRCID("ID_HISTOGRAM_BAR_SELECT_COMMON_IMAGE"));

    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarSelectStippleBrush, this,
         XRCID("ID_BAR_SELECT_STIPPLE_IMAGE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarSelectStippleShape, this,
         XRCID("ID_BAR_SELECT_STIPPLE_SHAPE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnBarSelectCommonImage, this,
         XRCID("ID_BAR_SELECT_COMMON_IMAGE"));

    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnGraphColorFade, this, XRCID("ID_GRAPH_BKCOLOR_FADE"));

    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditPlotBackgroundImageEffect, this,
         XRCID("ID_PLOT_BKIMAGE_EFFECT_NO_EFFECT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditPlotBackgroundImageEffect, this,
         XRCID("ID_PLOT_BKIMAGE_EFFECT_GRAYSCALE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditPlotBackgroundImageEffect, this,
         XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_HORIZONTALLY"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditPlotBackgroundImageEffect, this,
         XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_VERTICALLY"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditPlotBackgroundImageEffect, this,
         XRCID("ID_PLOT_BKIMAGE_EFFECT_SEPIA"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditPlotBackgroundImageEffect, this,
         XRCID("ID_PLOT_BKIMAGE_EFFECT_FROSTED_GLASS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditPlotBackgroundImageEffect, this,
         XRCID("ID_PLOT_BKIMAGE_EFFECT_OIL_PAINTING"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditPlotBackgroundImageEffect, this,
         XRCID("ID_PLOT_BKIMAGE_EFFECT_COLOR_BALANCE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditPlotBackgroundImageEffect, this,
         XRCID("ID_PLOT_BKIMAGE_EFFECT_DESPECKLE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditPlotBackgroundImageEffect, this,
         XRCID("ID_PLOT_BKIMAGE_EFFECT_SHARPEN"));

    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditPlotBackgroundImageFit, this,
         XRCID("ID_PLOT_BKIMAGE_FIT_CROP_AND_CENTER"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnEditPlotBackgroundImageFit, this,
         XRCID("ID_PLOT_BKIMAGE_FIT_SHRINK"));

    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_K12_US"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_K12_NEWFOUNDLAND"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_K12_BC"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_K12_NEW_BRUNSWICK"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_K12_NOVA_SCOTIA"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_K12_ONTARIO"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_K12_SASKATCHEWAN"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_K12_PE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_K12_MANITOBA"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_K12_NT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_K12_ALBERTA"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_K12_NUNAVUT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_QUEBEC"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_ENGLAND"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_REMOVE_TEST"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, wxID_SELECTALL);
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, wxID_COPY);
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, wxID_PREVIEW);
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, wxID_PRINT);
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_PRINT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_SAVE_PROJECT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_SAVE_PROJECT_AS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, wxID_ZOOM_IN);
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, wxID_ZOOM_OUT);
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, wxID_ZOOM_FIT);
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_SORT_ASCENDING"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_SORT_DESCENDING"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_EXCLUDE_SELECTED"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_SAVE_ITEM"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_COPY_ALL"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this,
         XRCID("ID_COPY_WITH_COLUMN_HEADERS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_COPY_FIRST_COLUMN"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_VIEW_ITEM"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_LIST_SORT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_EXPORT_ALL"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this,
         XRCID("ID_EXPORT_SCORES_AND_STATISTICS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_EXPORT_STATISTICS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this,
         XRCID("ID_SEND_TO_STANDARD_PROJECT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_LAUNCH_SOURCE_FILE"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this,
         XRCID("ID_EXPORT_FILTERED_DOCUMENT"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this, XRCID("ID_DOLCH"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this,
         XRCID("ID_BATCH_EXPORT_FILTERED_DOCUMENTS"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this,
         XRCID("ID_ADD_ITEM_TO_DICTIONARY"));
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnMenuCapture, this,
         XRCID("ID_ADD_ITEM_TO_DICTIONARY"));

    // bind the test menu items to their handlers
    if (doc->IsKindOf(wxCLASSINFO(BatchProjectDoc)) &&
        view->IsKindOf(wxCLASSINFO(BatchProjectView)))
        {
        for (const auto& rTest :
             dynamic_cast<const BatchProjectDoc*>(doc)->GetReadabilityTests().get_tests())
            {
            Bind(wxEVT_MENU, &BatchProjectView::OnAddTest, dynamic_cast<BatchProjectView*>(view),
                 rTest.get_test().get_interface_id());
            }
        }
    else if (doc->IsKindOf(wxCLASSINFO(ProjectDoc)) && view->IsKindOf(wxCLASSINFO(ProjectView)))
        {
        for (const auto& rTest :
             dynamic_cast<const ProjectDoc*>(doc)->GetReadabilityTests().get_tests())
            {
            Bind(wxEVT_MENU, &ProjectView::OnAddTest, dynamic_cast<ProjectView*>(view),
                 rTest.get_test().get_interface_id());
            }
        }

    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnProjectSettings(event);
        },
        wxID_PROPERTIES);

    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnToolsOptions(event);
        },
        wxID_PREFERENCES);

    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            auto* projDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
            wxASSERT_MSG(projDoc, L"Failed to get document!");
            if (projDoc == nullptr)
                {
                return;
                }

            projDoc->SetPlotBackGroundImagePath(wxString{});
            projDoc->RefreshRequired(ProjectRefresh::Minimal);
            projDoc->RefreshGraphs();
        },
        XRCID("ID_EDIT_PLOT_BKIMAGE_REMOVE"));

    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            auto* projView = dynamic_cast<BaseProjectView*>(GetView());
            wxASSERT(projView);
            if (projView != nullptr)
                {
                projView->ShowSideBar(!projView->IsSideBarShown());
                }
        },
        XRCID("ID_SHOW_SIDEBAR"));

    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            auto* projView = dynamic_cast<BaseProjectView*>(GetView());
            wxASSERT(projView);
            if (projView != nullptr)
                {
                projView->ShowSideBar(!projView->IsSideBarShown());
                }
        },
        XRCID("ID_SHOW_SIDEBAR"));

    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            auto* projView = dynamic_cast<BaseProjectView*>(GetView());
            wxASSERT(projView);
            if (projView != nullptr && projView->GetRibbon() != nullptr)
                {
                projView->GetRibbon()->ShowPanels(projView->GetRibbon()->GetDisplayMode() ==
                                                          wxRIBBON_BAR_MINIMIZED ?
                                                      wxRIBBON_BAR_EXPANDED :
                                                      wxRIBBON_BAR_MINIMIZED);
                }
        },
        XRCID("ID_TOGGLE_RIBBON"));

    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnCustomTest, this,
         wxGetApp().GetMainFrameEx()->CUSTOM_TEST_RANGE.GetFirstId(),
         wxGetApp().GetMainFrameEx()->CUSTOM_TEST_RANGE.GetLastId());
    Bind(wxEVT_MENU, &ProjectDocChildFrame::OnCustomTestBundle, this,
         wxGetApp().GetMainFrameEx()->TEST_BUNDLE_RANGE.GetFirstId(),
         wxGetApp().GetMainFrameEx()->TEST_BUNDLE_RANGE.GetLastId());
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnMenuCapture(wxCommandEvent& event)
    {
    auto* activeView = dynamic_cast<BaseProjectView*>(GetView());
    wxASSERT(activeView);
    if (activeView == nullptr)
        {
        return;
        }

    if (activeView->IsKindOf(wxCLASSINFO(ProjectView)))
        {
        wxASSERT(dynamic_cast<ProjectView*>(activeView));
        dynamic_cast<ProjectView*>(activeView)->ProcessEvent(event);
        }
    else if (activeView->IsKindOf(wxCLASSINFO(BatchProjectView)))
        {
        wxASSERT(dynamic_cast<BatchProjectView*>(activeView));
        dynamic_cast<BatchProjectView*>(activeView)->ProcessEvent(event);
        }
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnInvalidRegionColor(wxRibbonButtonBarEvent& event)
    {
    wxCommandEvent cmd(wxEVT_NULL, event.GetId());
    OnEditGraphColor(cmd);
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnDocumentRefresh([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* activeProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    if ((activeProject == nullptr) || !activeProject->IsSafeToUpdate())
        {
        return;
        }
    // if a standard project with manually entered text then there is no file to link to
    if (activeProject->IsKindOf(wxCLASSINFO(ProjectDoc)) &&
        activeProject->GetTextSource() == TextSource::EnteredText)
        {
        wxMessageBox(
            _(L"Only projects that originated from a file can be reloaded. "
              "This project had its text manually typed in and cannot be linked to a file."),
            wxGetApp().GetAppName(), wxOK | wxICON_INFORMATION);
        return;
        }
    // only refresh if we are linking to an external document.
    // if the documents were embedded, then ask if they want to change this to link to the files.
    if (activeProject->GetDocumentStorageMethod() == TextStorage::EmbedText)
        {
        if (wxMessageBox(
                _(L"Only projects linked to its source document can be reloaded. "
                  "This project currently has the original document's text embedded in it. "
                  "Do you wish to directly link to the source document?"),
                wxGetApp().GetAppName(), wxYES_NO | wxICON_QUESTION) == wxNO)
            {
            return;
            }
        if (activeProject->IsKindOf(wxCLASSINFO(ProjectDoc)))
            {
            const FilePathResolver resolvePath(activeProject->GetOriginalDocumentFilePath(), true);
            if (resolvePath.IsInvalidFile() ||
                (resolvePath.IsLocalOrNetworkFile() &&
                 !wxFile::Exists(activeProject->GetOriginalDocumentFilePath())))
                {
                wxMessageBox(wxString::Format(_(L"%s: file not found."),
                                              activeProject->GetOriginalDocumentFilePath()),
                             wxGetApp().GetAppName(), wxOK | wxICON_INFORMATION);
                return;
                }
            }
        activeProject->SetDocumentStorageMethod(TextStorage::NoEmbedText);
        }
    // project will need to do a full re-indexing
    activeProject->RefreshRequired(ProjectRefresh::FullReindexing);
    activeProject->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnEditGraphFont(wxCommandEvent& event)
    {
    wxFontData data;
    if (event.GetId() == XRCID("ID_EDIT_X_AXIS_FONT"))
        {
        data.SetInitialFont(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetXAxisFont());
        data.SetColour(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetXAxisFontColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_Y_AXIS_FONT"))
        {
        data.SetInitialFont(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetYAxisFont());
        data.SetColour(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetYAxisFontColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_TOP_TITLES_FONT"))
        {
        data.SetInitialFont(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphTopTitleFont());
        data.SetColour(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphTopTitleFontColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_BOTTOM_TITLES_FONT"))
        {
        data.SetInitialFont(
            dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphBottomTitleFont());
        data.SetColour(
            dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphBottomTitleFontColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_LEFT_TITLES_FONT"))
        {
        data.SetInitialFont(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphLeftTitleFont());
        data.SetColour(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphLeftTitleFontColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_RIGHT_TITLES_FONT"))
        {
        data.SetInitialFont(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphRightTitleFont());
        data.SetColour(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphRightTitleFontColor());
        }

    wxFontDialog dialog(this, data);
    if (dialog.ShowModal() == wxID_OK)
        {
        if (event.GetId() == XRCID("ID_EDIT_X_AXIS_FONT"))
            {
            dynamic_cast<BaseProjectDoc*>(GetDocument())
                ->SetXAxisFont(dialog.GetFontData().GetChosenFont());
            dynamic_cast<BaseProjectDoc*>(GetDocument())
                ->SetXAxisFontColor(dialog.GetFontData().GetColour());
            }
        else if (event.GetId() == XRCID("ID_EDIT_Y_AXIS_FONT"))
            {
            dynamic_cast<BaseProjectDoc*>(GetDocument())
                ->SetYAxisFont(dialog.GetFontData().GetChosenFont());
            dynamic_cast<BaseProjectDoc*>(GetDocument())
                ->SetYAxisFontColor(dialog.GetFontData().GetColour());
            }
        else if (event.GetId() == XRCID("ID_EDIT_TOP_TITLES_FONT"))
            {
            dynamic_cast<BaseProjectDoc*>(GetDocument())
                ->SetGraphTopTitleFont(dialog.GetFontData().GetChosenFont());
            dynamic_cast<BaseProjectDoc*>(GetDocument())
                ->SetGraphTopTitleFontColor(dialog.GetFontData().GetColour());
            }
        else if (event.GetId() == XRCID("ID_EDIT_BOTTOM_TITLES_FONT"))
            {
            dynamic_cast<BaseProjectDoc*>(GetDocument())
                ->SetGraphBottomTitleFont(dialog.GetFontData().GetChosenFont());
            dynamic_cast<BaseProjectDoc*>(GetDocument())
                ->SetGraphBottomTitleFontColor(dialog.GetFontData().GetColour());
            }
        else if (event.GetId() == XRCID("ID_EDIT_LEFT_TITLES_FONT"))
            {
            dynamic_cast<BaseProjectDoc*>(GetDocument())
                ->SetGraphLeftTitleFont(dialog.GetFontData().GetChosenFont());
            dynamic_cast<BaseProjectDoc*>(GetDocument())
                ->SetGraphLeftTitleFontColor(dialog.GetFontData().GetColour());
            }
        else if (event.GetId() == XRCID("ID_EDIT_RIGHT_TITLES_FONT"))
            {
            dynamic_cast<BaseProjectDoc*>(GetDocument())
                ->SetGraphRightTitleFont(dialog.GetFontData().GetChosenFont());
            dynamic_cast<BaseProjectDoc*>(GetDocument())
                ->SetGraphRightTitleFontColor(dialog.GetFontData().GetColour());
            }
        dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
        dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
        }
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnTestBundle(wxCommandEvent& event)
    {
    // see what sort of bundle this is
    bool basedOnIndustry = true;
    readability::industry_classification selectedIndustry =
        readability::industry_classification::adult_publishing_industry;
    readability::document_classification selectedDocument =
        readability::document_classification::adult_literature_document;
    if (event.GetId() == XRCID("ID_ADD_CHILDRENS_PUBLISHING_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::childrens_publishing_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_ADD_ADULT_PUBLISHING_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::adult_publishing_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_ADD_CHILDRENS_HEALTHCARE_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::childrens_healthcare_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_ADD_ADULT_HEALTHCARE_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::adult_healthcare_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_ADD_MILITARY_COVERNMENT_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::military_government_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_ADD_SECOND_LANGUAGE_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::secondary_language_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_ADD_BROADCASTING_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::broadcasting_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_GENERAL_DOCUMENT_TEST_BUNDLE"))
        {
        selectedDocument = readability::document_classification::general_document;
        basedOnIndustry = false;
        }
    else if (event.GetId() == XRCID("ID_TECHNICAL_DOCUMENT_TEST_BUNDLE"))
        {
        selectedDocument = readability::document_classification::technical_document;
        basedOnIndustry = false;
        }
    else if (event.GetId() == XRCID("ID_NONNARRATIVE_DOCUMENT_TEST_BUNDLE"))
        {
        selectedDocument = readability::document_classification::nonnarrative_document;
        basedOnIndustry = false;
        }
    else if (event.GetId() == XRCID("ID_ADULT_LITERATURE_DOCUMENT_TEST_BUNDLE"))
        {
        selectedDocument = readability::document_classification::adult_literature_document;
        basedOnIndustry = false;
        }
    else if (event.GetId() == XRCID("ID_CHILDRENS_LITERATURE_DOCUMENT_TEST_BUNDLE"))
        {
        selectedDocument = readability::document_classification::childrens_literature_document;
        basedOnIndustry = false;
        }

    // Standard project
    if ((GetDocument() != nullptr) && GetDocument()->IsKindOf(wxCLASSINFO(ProjectDoc)))
        {
        auto* doc = dynamic_cast<ProjectDoc*>(GetDocument());
        if ((doc != nullptr) && doc->IsSafeToUpdate())
            {
            const bool hadDolchSightWords = doc->IsIncludingDolchSightWords();
            doc->ExcludeAllTests();
            // standard tests
            for (auto rTest = doc->GetReadabilityTests().get_tests().begin();
                 rTest != doc->GetReadabilityTests().get_tests().end(); ++rTest)
                {
                const bool matchesClassification =
                    basedOnIndustry ?
                        rTest->get_test().has_industry_classification(selectedIndustry) :
                        rTest->get_test().has_document_classification(selectedDocument);
                rTest->include(matchesClassification &&
                               rTest->get_test().has_language(doc->GetProjectLanguage()));
                }
            // custom tests
            for (auto& customWordTest : ProjectDoc::m_custom_word_tests)
                {
                if ((basedOnIndustry &&
                     customWordTest.has_industry_classification(selectedIndustry)) ||
                    customWordTest.has_document_classification(selectedDocument))
                    {
                    doc->RefreshRequired(ProjectRefresh::FullReindexing);
                    doc->AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            // see if Dolch section should be added
            if (((basedOnIndustry &&
                  selectedIndustry ==
                      readability::industry_classification::childrens_publishing_industry) ||
                 (basedOnIndustry &&
                  selectedIndustry ==
                      readability::industry_classification::secondary_language_industry) ||
                 (!basedOnIndustry &&
                  selectedDocument ==
                      readability::document_classification::childrens_literature_document)) &&
                (doc->GetProjectLanguage() == readability::test_language::english_test))
                {
                doc->IncludeDolchSightWords();
                }
            // if Dolch was added or removed then refresh
            if ((!hadDolchSightWords && doc->IsIncludingDolchSightWords()) ||
                (hadDolchSightWords && !doc->IsIncludingDolchSightWords()))
                {
                doc->RefreshRequired(ProjectRefresh::FullReindexing);
                }
            doc->RefreshRequired(ProjectRefresh::Minimal);
            doc->RefreshProject();
            }
        }
    // Batch project
    if ((GetDocument() != nullptr) && GetDocument()->IsKindOf(wxCLASSINFO(BatchProjectDoc)))
        {
        auto* doc = dynamic_cast<BatchProjectDoc*>(GetDocument());
        if ((doc != nullptr) && doc->IsSafeToUpdate())
            {
            doc->ExcludeAllTests();
            // standard tests
            for (auto rTest = doc->GetReadabilityTests().get_tests().begin();
                 rTest != doc->GetReadabilityTests().get_tests().end(); ++rTest)
                {
                const bool matchesClassification =
                    basedOnIndustry ?
                        rTest->get_test().has_industry_classification(selectedIndustry) :
                        rTest->get_test().has_document_classification(selectedDocument);
                rTest->include(matchesClassification &&
                               rTest->get_test().has_language(doc->GetProjectLanguage()));
                }
            // custom tests
            for (auto& customWordTest : BatchProjectDoc::m_custom_word_tests)
                {
                if ((basedOnIndustry &&
                     customWordTest.has_industry_classification(selectedIndustry)) ||
                    customWordTest.has_document_classification(selectedDocument))
                    {
                    doc->RefreshRequired(ProjectRefresh::FullReindexing);
                    doc->AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            // see if Dolch section should be added...
            if (((basedOnIndustry &&
                  selectedIndustry ==
                      readability::industry_classification::childrens_publishing_industry) ||
                 (basedOnIndustry &&
                  selectedIndustry ==
                      readability::industry_classification::secondary_language_industry) ||
                 (!basedOnIndustry &&
                  selectedDocument ==
                      readability::document_classification::childrens_literature_document)) &&
                (doc->GetProjectLanguage() == readability::test_language::english_test))
                {
                doc->IncludeDolchSightWords();
                }
            // ...or removed
            else if (doc->IsIncludingDolchSightWords())
                {
                doc->IncludeDolchSightWords(false);
                }
            // need to do a full refresh (except document re-indexing) for a batch project
            // because so many sections in it involve test scores.
            doc->RefreshRequired(ProjectRefresh::Minimal);
            doc->RefreshProject();
            }
        }
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnLongSentencesOptions(wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    if (event.GetId() == XRCID("ID_LS_LONGER_THAN"))
        {
        wxNumberEntryDialog dialog(this, _(L"Consider sentences overly long if longer than:"),
                                   wxString{}, _(L"Long Sentences"),
                                   doc->GetDifficultSentenceLength(), 0,
                                   std::numeric_limits<int>::max());
        if (dialog.ShowModal() != wxID_OK)
            {
            return;
            }
        const long len = dialog.GetValue();
        doc->SetDifficultSentenceLength(len);
        }
    doc->SetLongSentenceMethod((event.GetId() == XRCID("ID_LS_LONGER_THAN")) ?
                                   LongSentence::LongerThanSpecifiedLength :
                                   LongSentence::OutlierLength);

    for (size_t i = 0; i < m_longSentencesMenu.GetMenuItemCount(); ++i)
        {
        m_longSentencesMenu.FindItemByPosition(i)->Check(false);
        }
    wxMenuItem* item = m_longSentencesMenu.FindItem(
        (doc->GetLongSentenceMethod() == LongSentence::LongerThanSpecifiedLength) ?
            XRCID("ID_LS_LONGER_THAN") :
            XRCID("ID_LS_OUTLIER_RANGE"));
    if (item != nullptr)
        {
        item->Check(true);
        }

    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnEditGraphColor(wxCommandEvent& event)
    {
    wxColourData data;
    wxGetApp().GetAppOptions()->CopyCustomColorsToColorData(data);
    data.SetChooseFull(true);
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(doc, L"Invalid document when editing graph colors!");
    if (event.GetId() == XRCID("ID_EDIT_GRAPH_BKCOLOR"))
        {
        data.SetColour(doc->GetBackGroundColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_BOX_COLOR"))
        {
        data.SetColour(doc->GetGraphBoxColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_HISTOGRAM_BAR_COLOR"))
        {
        data.SetColour(doc->GetHistogramBarColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_BAR_COLOR"))
        {
        data.SetColour(doc->GetBarChartBarColor());
        }
    else if (event.GetId() == XRCID("ID_INVALID_REGION_COLOR"))
        {
        data.SetColour(doc->GetInvalidAreaColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_PLOT_BKCOLOR"))
        {
        data.SetColour(doc->GetPlotBackGroundColor());
        }

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        wxGetApp().GetAppOptions()->CopyColorDataToCustomColors(dialog.GetColourData());
        const wxColour color = dialog.GetColourData().GetColour();
        if (event.GetId() == XRCID("ID_EDIT_GRAPH_BKCOLOR"))
            {
            doc->SetBackGroundColor(color);
            }
        else if (event.GetId() == XRCID("ID_EDIT_BOX_COLOR"))
            {
            doc->SetGraphBoxColor(color);
            }
        else if (event.GetId() == XRCID("ID_EDIT_HISTOGRAM_BAR_COLOR"))
            {
            doc->SetHistogramBarColor(color);
            }
        else if (event.GetId() == XRCID("ID_EDIT_BAR_COLOR"))
            {
            doc->SetBarChartBarColor(color);
            }
        else if (event.GetId() == XRCID("ID_INVALID_REGION_COLOR"))
            {
            doc->SetInvalidAreaColor(color);
            }
        else if (event.GetId() == XRCID("ID_EDIT_PLOT_BKCOLOR"))
            {
            doc->SetPlotBackGroundColor(color);
            if (doc->GetPlotBackGroundColorOpacity() == wxALPHA_TRANSPARENT)
                {
                if (wxMessageBox(_(L"Plot background is currently transparent; this color will "
                                   "have no effect. Do you wish to edit the opacity?"),
                                 wxGetApp().GetAppName(), wxYES_NO | wxICON_QUESTION) == wxYES)
                    {
                    wxCommandEvent evt{ wxEVT_NULL, XRCID("ID_EDIT_PLOT_BKCOLOR_OPACITY") };
                    OnEditGraphOpacity(evt);
                    }
                }
            }

        doc->RefreshRequired(ProjectRefresh::Minimal);
        doc->RefreshGraphs();
        }
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnEditGraphColorScheme([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(baseDoc, L"Failed to get document!");
    if (baseDoc == nullptr)
        {
        return;
        }

    // load the list of color schemes and select the one currently in use
    const wxString selectedStr{ baseDoc->GetGraphColorScheme() };
    int selectedItem{ 0 };
    wxArrayString choices;
    for (auto colorSchemeIter = wxGetApp().GetGraphColorSchemeMap().cbegin();
         colorSchemeIter != wxGetApp().GetGraphColorSchemeMap().cend(); ++colorSchemeIter)
        {
        choices.push_back(colorSchemeIter->first);
        if (colorSchemeIter->second == selectedStr)
            {
            selectedItem = static_cast<int>(
                std::distance(wxGetApp().GetGraphColorSchemeMap().cbegin(), colorSchemeIter));
            }
        }

    wxSingleChoiceDialog colorSchemeDlg(this, _("Select a color scheme:"), _("Select Color Scheme"),
                                        choices);
    colorSchemeDlg.SetSelection(selectedItem);
    colorSchemeDlg.SetSize(FromDIP(wxSize{ 200, 400 }));
    colorSchemeDlg.Center();
    if (colorSchemeDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    const auto foundColorScheme =
        wxGetApp().GetGraphColorSchemeMap().find(colorSchemeDlg.GetStringSelection());
    if (foundColorScheme != wxGetApp().GetGraphColorSchemeMap().cend())
        {
        baseDoc->SetGraphColorScheme(foundColorScheme->second);
        }

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnEditStatsReportButton([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    ToolsOptionsDlg optionsDlg(this, theProject, ToolsOptionsDlg::Statistics);
    optionsDlg.SelectPage(ToolsOptionsDlg::ANALYSIS_STATISTICS_PAGE);
    if (optionsDlg.ShowModal() == wxID_OK)
        {
        theProject->RefreshStatisticsReports();
        }
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnBarOrientationSelected(wxCommandEvent& event)
    {
    if (event.GetId() == XRCID("ID_BAR_HORIZONTAL"))
        {
        dynamic_cast<BaseProjectDoc*>(GetDocument())
            ->SetBarChartOrientation(Wisteria::Orientation::Horizontal);
        }
    else if (event.GetId() == XRCID("ID_BAR_VERTICAL"))
        {
        dynamic_cast<BaseProjectDoc*>(GetDocument())
            ->SetBarChartOrientation(Wisteria::Orientation::Vertical);
        }

    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnBarStyleSelected(wxCommandEvent& event)
    {
    auto* baseDoc{ dynamic_cast<BaseProjectDoc*>(GetDocument()) };
    wxASSERT_MSG(baseDoc, L"Invalid document!");

    if (event.GetId() == XRCID("ID_BAR_STYLE_SOLID"))
        {
        baseDoc->SetGraphBarEffect(Wisteria::BoxEffect::Solid);
        }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_GLASS"))
        {
        baseDoc->SetGraphBarEffect(Wisteria::BoxEffect::Glassy);
        }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_BTOT"))
        {
        baseDoc->SetGraphBarEffect(Wisteria::BoxEffect::FadeFromBottomToTop);
        }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_TTOB"))
        {
        baseDoc->SetGraphBarEffect(Wisteria::BoxEffect::FadeFromTopToBottom);
        }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_STIPPLE_IMAGE"))
        {
        if (!wxFile::Exists(baseDoc->GetStippleImagePath()))
            {
            wxFileDialog fd(this, _(L"Select Stipple Image"),
                            wxGetApp().GetAppOptions()->GetImagePath(), wxString{},
                            Wisteria::GraphItems::Image::GetImageFileFilter(),
                            wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                {
                return;
                }
            wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
            baseDoc->SetStippleImagePath(fd.GetPath());
            }
        baseDoc->SetGraphBarEffect(Wisteria::BoxEffect::StippleImage);
        }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_STIPPLE_SHAPE"))
        {
        baseDoc->SetGraphBarEffect(Wisteria::BoxEffect::StippleShape);
        }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_WATERCOLOR"))
        {
        baseDoc->SetGraphBarEffect(Wisteria::BoxEffect::WaterColor);
        }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_THICK_WATERCOLOR"))
        {
        baseDoc->SetGraphBarEffect(Wisteria::BoxEffect::ThickWaterColor);
        }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_MARKER"))
        {
        baseDoc->SetGraphBarEffect(Wisteria::BoxEffect::Marker);
        }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_COMMON_IMAGE"))
        {
        if (!wxFile::Exists(baseDoc->GetGraphCommonImagePath()))
            {
            wxFileDialog fd(this, _(L"Select Common Image"),
                            wxGetApp().GetAppOptions()->GetImagePath(), wxString{},
                            Wisteria::GraphItems::Image::GetImageFileFilter(),
                            wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                {
                return;
                }
            wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
            baseDoc->SetGraphCommonImagePath(fd.GetPath());
            }
        baseDoc->SetGraphBarEffect(Wisteria::BoxEffect::CommonImage);
        }
    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnHistoBarStyleSelected(wxCommandEvent& event)
    {
    auto* baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(baseDoc, L"Failed to get document!");
    if (baseDoc == nullptr)
        {
        return;
        }

    if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_SOLID"))
        {
        baseDoc->SetHistogramBarEffect(Wisteria::BoxEffect::Solid);
        }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_GLASS"))
        {
        baseDoc->SetHistogramBarEffect(Wisteria::BoxEffect::Glassy);
        }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_BTOT"))
        {
        baseDoc->SetHistogramBarEffect(Wisteria::BoxEffect::FadeFromBottomToTop);
        }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_TTOB"))
        {
        baseDoc->SetHistogramBarEffect(Wisteria::BoxEffect::FadeFromTopToBottom);
        }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_STIPPLE_IMAGE"))
        {
        if (!wxFile::Exists(baseDoc->GetStippleImagePath()))
            {
            wxFileDialog fd(this, _(L"Select Stipple Image"),
                            wxGetApp().GetAppOptions()->GetImagePath(), wxString{},
                            Wisteria::GraphItems::Image::GetImageFileFilter(),
                            wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                {
                return;
                }
            wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
            baseDoc->SetStippleImagePath(fd.GetPath());
            }
        baseDoc->SetHistogramBarEffect(Wisteria::BoxEffect::StippleImage);
        }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_STIPPLE_SHAPE"))
        {
        baseDoc->SetHistogramBarEffect(Wisteria::BoxEffect::StippleShape);
        }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_WATERCOLOR"))
        {
        baseDoc->SetHistogramBarEffect(Wisteria::BoxEffect::WaterColor);
        }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_THICK_WATERCOLOR"))
        {
        baseDoc->SetHistogramBarEffect(Wisteria::BoxEffect::ThickWaterColor);
        }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_MARKER"))
        {
        baseDoc->SetHistogramBarEffect(Wisteria::BoxEffect::Marker);
        }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_COMMON_IMAGE"))
        {
        if (!wxFile::Exists(baseDoc->GetGraphCommonImagePath()))
            {
            wxFileDialog fd(this, _(L"Select Common Image"),
                            wxGetApp().GetAppOptions()->GetImagePath(), wxString{},
                            Wisteria::GraphItems::Image::GetImageFileFilter(),
                            wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                {
                return;
                }
            wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
            baseDoc->SetGraphCommonImagePath(fd.GetPath());
            }
        baseDoc->SetHistogramBarEffect(Wisteria::BoxEffect::CommonImage);
        }
    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnHistoBarSelectStippleBrush([[maybe_unused]] wxCommandEvent& event)
    {
    auto* baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(baseDoc, L"Failed to get document!");
    if (baseDoc == nullptr)
        {
        return;
        }

    wxFileDialog fd(this, _(L"Select Stipple Image"), wxGetApp().GetAppOptions()->GetImagePath(),
                    wxString{}, Wisteria::GraphItems::Image::GetImageFileFilter(),
                    wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        {
        return;
        }
    wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
    baseDoc->SetStippleImagePath(fd.GetPath());

    baseDoc->SetHistogramBarEffect(Wisteria::BoxEffect::StippleImage);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnHistoBarSelectStippleShape([[maybe_unused]] wxCommandEvent& event)
    {
    auto* baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(baseDoc, L"Failed to get document!");
    if (baseDoc == nullptr)
        {
        return;
        }

    wxArrayString choices;
    for (const auto& shape : wxGetApp().GetShapeMap())
        {
        choices.push_back(shape.first);
        }

    wxSingleChoiceDialog shapesDlg(this, _("Select a shape to draw across your bars:"),
                                   _("Select Stipple Shape"), choices);
    shapesDlg.SetSize(FromDIP(wxSize{ 200, 400 }));
    shapesDlg.Center();
    if (shapesDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    const auto foundShape = wxGetApp().GetShapeMap().find(shapesDlg.GetStringSelection());
    if (foundShape != wxGetApp().GetShapeMap().cend())
        {
        baseDoc->SetStippleShape(foundShape->second);
        }

    baseDoc->SetHistogramBarEffect(Wisteria::BoxEffect::StippleShape);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnBarSelectStippleShape([[maybe_unused]] wxCommandEvent& event)
    {
    auto* baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(baseDoc, L"Failed to get document!");
    if (baseDoc == nullptr)
        {
        return;
        }

    wxArrayString choices;
    for (const auto& shape : wxGetApp().GetShapeMap())
        {
        choices.push_back(shape.first);
        }

    wxSingleChoiceDialog shapesDlg(this, _("Select a shape to draw across your bars:"),
                                   _("Select Stipple Shape"), choices);
    shapesDlg.SetSize(FromDIP(wxSize{ 200, 400 }));
    shapesDlg.Center();
    if (shapesDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    const auto foundShape = wxGetApp().GetShapeMap().find(shapesDlg.GetStringSelection());
    if (foundShape != wxGetApp().GetShapeMap().cend())
        {
        baseDoc->SetStippleShape(foundShape->second);
        }

    baseDoc->SetGraphBarEffect(Wisteria::BoxEffect::StippleShape);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnBoxSelectStippleShape([[maybe_unused]] wxCommandEvent& event)
    {
    auto* baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(baseDoc, L"Failed to get document!");
    if (baseDoc == nullptr)
        {
        return;
        }

    wxArrayString choices;
    for (const auto& shape : wxGetApp().GetShapeMap())
        {
        choices.push_back(shape.first);
        }

    wxSingleChoiceDialog shapesDlg(this, _("Select a shape to draw across your boxes:"),
                                   _("Select Stipple Shape"), choices);
    shapesDlg.SetSize(FromDIP(wxSize{ 200, 400 }));
    shapesDlg.Center();
    if (shapesDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    const auto foundShape = wxGetApp().GetShapeMap().find(shapesDlg.GetStringSelection());
    if (foundShape != wxGetApp().GetShapeMap().cend())
        {
        baseDoc->SetStippleShape(foundShape->second);
        }

    baseDoc->SetGraphBoxEffect(Wisteria::BoxEffect::StippleShape);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnHistoBarSelectCommonImage([[maybe_unused]] wxCommandEvent& event)
    {
    auto* baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(baseDoc, L"Failed to get document!");
    if (baseDoc == nullptr)
        {
        return;
        }

    wxFileDialog fd(this, _(L"Select Common Image"), wxGetApp().GetAppOptions()->GetImagePath(),
                    wxString{}, Wisteria::GraphItems::Image::GetImageFileFilter(),
                    wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        {
        return;
        }
    wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
    baseDoc->SetGraphCommonImagePath(fd.GetPath());

    baseDoc->SetHistogramBarEffect(Wisteria::BoxEffect::CommonImage);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnBoxSelectCommonImage([[maybe_unused]] wxCommandEvent& event)
    {
    auto* baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(baseDoc, L"Failed to get document!");
    if (baseDoc == nullptr)
        {
        return;
        }

    wxFileDialog fd(this, _(L"Select Common Image"), wxGetApp().GetAppOptions()->GetImagePath(),
                    wxString{}, Wisteria::GraphItems::Image::GetImageFileFilter(),
                    wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        {
        return;
        }
    wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
    baseDoc->SetGraphCommonImagePath(fd.GetPath());

    baseDoc->SetGraphBoxEffect(Wisteria::BoxEffect::CommonImage);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnBoxSelectStippleBrush([[maybe_unused]] wxCommandEvent& event)
    {
    auto* baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(baseDoc, L"Failed to get document!");
    if (baseDoc == nullptr)
        {
        return;
        }

    wxFileDialog fd(this, _(L"Select Stipple Image"), wxGetApp().GetAppOptions()->GetImagePath(),
                    wxString{}, Wisteria::GraphItems::Image::GetImageFileFilter(),
                    wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        {
        return;
        }
    wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
    baseDoc->SetStippleImagePath(fd.GetPath());

    baseDoc->SetGraphBoxEffect(Wisteria::BoxEffect::StippleImage);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnBarSelectStippleBrush([[maybe_unused]] wxCommandEvent& event)
    {
    auto* baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(baseDoc, L"Failed to get document!");
    if (baseDoc == nullptr)
        {
        return;
        }

    wxFileDialog fd(this, _(L"Select Stipple Image"), wxGetApp().GetAppOptions()->GetImagePath(),
                    wxString{}, Wisteria::GraphItems::Image::GetImageFileFilter(),
                    wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        {
        return;
        }
    wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
    baseDoc->SetStippleImagePath(fd.GetPath());

    baseDoc->SetGraphBarEffect(Wisteria::BoxEffect::StippleImage);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnBarSelectCommonImage([[maybe_unused]] wxCommandEvent& event)
    {
    auto* baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(baseDoc, L"Failed to get document!");
    if (baseDoc == nullptr)
        {
        return;
        }

    wxFileDialog fd(this, _(L"Select Common Image"), wxGetApp().GetAppOptions()->GetImagePath(),
                    wxString{}, Wisteria::GraphItems::Image::GetImageFileFilter(),
                    wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        {
        return;
        }
    wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
    baseDoc->SetGraphCommonImagePath(fd.GetPath());

    baseDoc->SetGraphBarEffect(Wisteria::BoxEffect::CommonImage);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnBoxStyleSelected(wxCommandEvent& event)
    {
    auto* baseDoc{ dynamic_cast<BaseProjectDoc*>(GetDocument()) };
    wxASSERT_MSG(baseDoc, L"Invalid document!");
    if (event.GetId() == XRCID("ID_BOX_STYLE_SOLID"))
        {
        baseDoc->SetGraphBoxEffect(Wisteria::BoxEffect::Solid);
        }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_GLASS"))
        {
        baseDoc->SetGraphBoxEffect(Wisteria::BoxEffect::Glassy);
        }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_LTOR"))
        {
        baseDoc->SetGraphBoxEffect(Wisteria::BoxEffect::FadeFromLeftToRight);
        }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_RTOL"))
        {
        baseDoc->SetGraphBoxEffect(Wisteria::BoxEffect::FadeFromRightToLeft);
        }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_STIPPLE_IMAGE"))
        {
        if (!wxFile::Exists(baseDoc->GetStippleImagePath()))
            {
            wxFileDialog fd(this, _(L"Select Stipple Image"),
                            wxGetApp().GetAppOptions()->GetImagePath(), wxString{},
                            Wisteria::GraphItems::Image::GetImageFileFilter(),
                            wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                {
                return;
                }
            wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
            baseDoc->SetStippleImagePath(fd.GetPath());
            }
        baseDoc->SetGraphBoxEffect(Wisteria::BoxEffect::StippleImage);
        }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_STIPPLE_SHAPE"))
        {
        baseDoc->SetGraphBoxEffect(Wisteria::BoxEffect::StippleShape);
        }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_WATERCOLOR"))
        {
        baseDoc->SetGraphBoxEffect(Wisteria::BoxEffect::WaterColor);
        }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_THICK_WATERCOLOR"))
        {
        baseDoc->SetGraphBoxEffect(Wisteria::BoxEffect::ThickWaterColor);
        }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_MARKER"))
        {
        baseDoc->SetGraphBoxEffect(Wisteria::BoxEffect::Marker);
        }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_COMMON_IMAGE"))
        {
        if (!wxFile::Exists(baseDoc->GetGraphCommonImagePath()))
            {
            wxFileDialog fd(this, _(L"Select Common Image"),
                            wxGetApp().GetAppOptions()->GetImagePath(), wxString{},
                            Wisteria::GraphItems::Image::GetImageFileFilter(),
                            wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                {
                return;
                }
            wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
            baseDoc->SetGraphCommonImagePath(fd.GetPath());
            }
        baseDoc->SetGraphBoxEffect(Wisteria::BoxEffect::CommonImage);
        }
    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnBoxPlotShowAllPointsButton(
    [[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    dynamic_cast<BaseProjectDoc*>(GetDocument())
        ->ShowAllBoxPlotPoints(
            !dynamic_cast<BaseProjectDoc*>(GetDocument())->IsShowingAllBoxPlotPoints());
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnBoxPlotShowLabelsButton([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    dynamic_cast<BaseProjectDoc*>(GetDocument())
        ->DisplayBoxPlotLabels(
            !dynamic_cast<BaseProjectDoc*>(GetDocument())->IsDisplayingBoxPlotLabels());
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnBarLabelsButton([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(doc, L"Failed to get document!");
    if (doc == nullptr)
        {
        return;
        }

    doc->DisplayBarChartLabels(!doc->IsDisplayingBarChartLabels());
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnFleschConnectLinesButton(
    [[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    dynamic_cast<BaseProjectDoc*>(GetDocument())
        ->ConnectFleschPoints(
            !dynamic_cast<BaseProjectDoc*>(GetDocument())->IsConnectingFleschPoints());
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnEnglishLabels([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(doc, L"Failed to get document!");
    if (doc == nullptr)
        {
        return;
        }

    doc->UseEnglishLabelsForGermanLix(!doc->IsUsingEnglishLabelsForGermanLix());
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnGraphLogo([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(doc, L"Failed to get document!");
    if (doc == nullptr)
        {
        return;
        }

    wxFileDialog fd(this, _(L"Select Logo Image"),
                    !doc->GetWatermarkLogoPath().empty() ?
                        wxString{} :
                        wxGetApp().GetAppOptions()->GetImagePath(),
                    doc->GetWatermarkLogoPath(), Wisteria::GraphItems::Image::GetImageFileFilter(),
                    wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (fd.ShowModal() == wxID_OK)
        {
        wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
        doc->SetWatermarkLogoPath(fd.GetPath());
        doc->RefreshRequired(ProjectRefresh::Minimal);
        doc->RefreshGraphs();
        }
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnPrintWatermark([[maybe_unused]] wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    if (doc != nullptr)
        {
        wxTextEntryDialog textDlg(this,
                                  _(L"Enter watermark:\n\n(Note that the tags @DATETIME@, @DATE@, "
                                    "and @TIME@ can be used\n"
                                    "to dynamically expand into the current date and time.)"),
                                  _(L"Watermark"), doc->GetWatermark().m_label,
                                  wxTextEntryDialogStyle | wxTE_MULTILINE);
        if (textDlg.ShowModal() == wxID_OK)
            {
            auto watermark = doc->GetWatermark();
            watermark.m_label = textDlg.GetValue();
            doc->SetWatermark(watermark);
            doc->RefreshRequired(ProjectRefresh::Minimal);
            doc->RefreshGraphs();
            }
        }
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnDropShadow([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    dynamic_cast<BaseProjectDoc*>(GetDocument())
        ->DisplayDropShadows(
            !dynamic_cast<BaseProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnShowcaseKeyItems([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(doc, L"Failed to get document!");
    if (doc == nullptr)
        {
        return;
        }

    doc->ShowcaseKeyItems(!doc->IsShowcasingKeyItems());
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnRaygorStyleSelected([[maybe_unused]] wxCommandEvent& event)
    {
    if (event.GetId() == XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"))
        {
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"), true);
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"), false);
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"), false);
        dynamic_cast<BaseProjectDoc*>(GetDocument())
            ->SetRaygorStyle(Wisteria::Graphs::RaygorStyle::Original);
        }
    else if (event.GetId() == XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"))
        {
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"), false);
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"), true);
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"), false);
        dynamic_cast<BaseProjectDoc*>(GetDocument())
            ->SetRaygorStyle(Wisteria::Graphs::RaygorStyle::BaldwinKaufman);
        }
    else if (event.GetId() == XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"))
        {
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"), false);
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"), false);
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"), true);
        dynamic_cast<BaseProjectDoc*>(GetDocument())
            ->SetRaygorStyle(Wisteria::Graphs::RaygorStyle::Modern);
        }
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnGraphColorFade([[maybe_unused]] wxCommandEvent& event)
    {
    dynamic_cast<BaseProjectDoc*>(GetDocument())
        ->SetGraphBackGroundLinearGradient(
            !dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphBackGroundLinearGradient());
    wxMenuItem* fadeOption = m_graphBackgroundMenu.FindItem(XRCID("ID_GRAPH_BKCOLOR_FADE"));
    if (fadeOption != nullptr)
        {
        fadeOption->Check(
            dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphBackGroundLinearGradient());
        }
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnEditPlotBackgroundImageFit(wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    // uncheck all the options
    if (auto* tempMenuItem =
            m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_FIT_CROP_AND_CENTER"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(false);
        }
    if (auto* tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_FIT_SHRINK"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(false);
        }
    wxMenuItem* menuItem{ nullptr };
    if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_FIT_CROP_AND_CENTER"))
        {
        doc->SetPlotBackGroundImageFit(Wisteria::ImageFit::CropAndCenter);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_FIT_CROP_AND_CENTER"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_FIT_SHRINK"))
        {
        doc->SetPlotBackGroundImageFit(Wisteria::ImageFit::Shrink);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_FIT_SHRINK"));
        }

    if (menuItem != nullptr)
        {
        menuItem->Check(true);
        }
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnEditPlotBackgroundImageEffect(wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    // uncheck all the options
    if (auto* tempMenuItem =
            m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_NO_EFFECT"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(false);
        }
    if (auto* tempMenuItem =
            m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_GRAYSCALE"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(false);
        }
    if (auto* tempMenuItem =
            m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_HORIZONTALLY"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(false);
        }
    if (auto* tempMenuItem =
            m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_VERTICALLY"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(false);
        }
    if (auto* tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_SEPIA"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(false);
        }
    if (auto* tempMenuItem =
            m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_FROSTED_GLASS"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(false);
        }
    if (auto* tempMenuItem =
            m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_OIL_PAINTING"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(false);
        }
    if (auto* tempMenuItem =
            m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_COLOR_BALANCE"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(false);
        }
    if (auto* tempMenuItem =
            m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_DESPECKLE"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(false);
        }
    if (auto* tempMenuItem =
            m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_SHARPEN"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(false);
        }
    wxMenuItem* menuItem{ nullptr };
    if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_NO_EFFECT"))
        {
        doc->SetPlotBackGroundImageEffect(Wisteria::ImageEffect::NoEffect);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_NO_EFFECT"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_GRAYSCALE"))
        {
        doc->SetPlotBackGroundImageEffect(Wisteria::ImageEffect::Grayscale);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_GRAYSCALE"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_HORIZONTALLY"))
        {
        doc->SetPlotBackGroundImageEffect(Wisteria::ImageEffect::BlurHorizontal);
        menuItem =
            m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_HORIZONTALLY"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_VERTICALLY"))
        {
        doc->SetPlotBackGroundImageEffect(Wisteria::ImageEffect::BlurVertical);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_VERTICALLY"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_SEPIA"))
        {
        doc->SetPlotBackGroundImageEffect(Wisteria::ImageEffect::Sepia);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_SEPIA"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_FROSTED_GLASS"))
        {
        doc->SetPlotBackGroundImageEffect(Wisteria::ImageEffect::FrostedGlass);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_FROSTED_GLASS"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_OIL_PAINTING"))
        {
        doc->SetPlotBackGroundImageEffect(Wisteria::ImageEffect::OilPainting);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_OIL_PAINTING"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_COLOR_BALANCE"))
        {
        doc->SetPlotBackGroundImageEffect(Wisteria::ImageEffect::ColorBalance);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_COLOR_BALANCE"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_DESPECKLE"))
        {
        doc->SetPlotBackGroundImageEffect(Wisteria::ImageEffect::Despeckle);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_DESPECKLE"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_SHARPEN"))
        {
        doc->SetPlotBackGroundImageEffect(Wisteria::ImageEffect::Sharpen);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_SHARPEN"));
        }

    if (menuItem != nullptr)
        {
        menuItem->Check(true);
        }
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnMergePlotBackgroundImages([[maybe_unused]] wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(doc, L"Failed to get document!");
    if (doc == nullptr)
        {
        return;
        }

    wxFileDialog fd(this, _(L"Select Images"), wxGetApp().GetAppOptions()->GetImagePath(),
                    wxString{}, Wisteria::GraphItems::Image::GetImageFileFilter(),
                    wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE | wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        {
        return;
        }
    wxArrayString imgPaths;
    fd.GetPaths(imgPaths);
    if (imgPaths.empty())
        {
        return;
        }

    Wisteria::UI::ImageMergeDlg imgDlg(this, imgPaths, wxHORIZONTAL);
    if (imgDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    wxGetApp().GetAppOptions()->SetImagePath(imgDlg.GetMergedFilePath());
    doc->SetPlotBackGroundImagePath(imgDlg.GetMergedFilePath());
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnEditPlotBackgroundImage([[maybe_unused]] wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(doc, L"Failed to get document!");
    if (doc == nullptr)
        {
        return;
        }

    wxFileDialog fd(
        this, _(L"Select Plot Background Image"),
        !doc->GetPlotBackGroundImagePath().empty() ? wxString{} :
                                                     wxGetApp().GetAppOptions()->GetImagePath(),
        doc->GetPlotBackGroundImagePath(), Wisteria::GraphItems::Image::GetImageFileFilter(),
        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        {
        return;
        }
    wxGetApp().GetAppOptions()->SetImagePath(fd.GetPath());
    doc->SetPlotBackGroundImagePath(fd.GetPath());
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnEditGraphOpacity(wxCommandEvent& event)
    {
    wxBitmap bmp(wxGetApp().GetMainFrame()->FromDIP(wxSize{ 300, 300 }));
    uint8_t opacity = wxALPHA_OPAQUE;

    const auto fillSquare = [](wxBitmap& theBmp, const wxColour& opacityColor)
    {
        wxMemoryDC memDC(theBmp);
        memDC.SetBrush(wxBrush{ opacityColor });
        memDC.SetPen(wxColour{ 0, 0, 0 });
        memDC.Clear();
        memDC.DrawRectangle(0, 0, theBmp.GetWidth(), theBmp.GetHeight());
        memDC.SelectObject(wxNullBitmap);
    };

    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT_MSG(doc, L"Failed to get document!");
    if (doc == nullptr)
        {
        return;
        }

    if (event.GetId() == XRCID("ID_EDIT_PLOT_BKIMAGE_OPACITY"))
        {
        auto img = Wisteria::GraphItems::Image::LoadFile(doc->GetPlotBackGroundImagePath());
        if (!img.IsOk())
            {
            wxFileDialog fd(this, _(L"Select Plot Background Image"),
                            !doc->GetPlotBackGroundImagePath().empty() ?
                                wxString{} :
                                wxGetApp().GetAppOptions()->GetImagePath(),
                            doc->GetPlotBackGroundImagePath(),
                            Wisteria::GraphItems::Image::GetImageFileFilter(),
                            wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                {
                return;
                }
            wxGetApp().GetAppOptions()->SetImagePath(wxFileName(fd.GetPath()).GetPath());
            doc->SetPlotBackGroundImagePath(fd.GetPath());
            img = Wisteria::GraphItems::Image::LoadFile(doc->GetPlotBackGroundImagePath());
            if (!img.IsOk())
                {
                return;
                }

            bmp = img;
            }
        else
            {
            bmp = img;
            }
        opacity = doc->GetPlotBackGroundImageOpacity();
        }
    else if (event.GetId() == XRCID("ID_EDIT_BOX_OPACITY"))
        {
        fillSquare(bmp, doc->GetGraphBoxColor());
        opacity = doc->GetGraphBoxOpacity();
        }
    else if (event.GetId() == XRCID("ID_EDIT_BAR_OPACITY"))
        {
        fillSquare(bmp, doc->GetBarChartBarColor());
        opacity = doc->GetGraphBarOpacity();
        }
    else if (event.GetId() == XRCID("ID_EDIT_HISTOBAR_OPACITY"))
        {
        fillSquare(bmp, doc->GetHistogramBarColor());
        opacity = doc->GetHistogramBarOpacity();
        }
    else if (event.GetId() == XRCID("ID_EDIT_PLOT_BKCOLOR_OPACITY"))
        {
        fillSquare(bmp, doc->GetPlotBackGroundColor());
        opacity = doc->GetPlotBackGroundColorOpacity();
        }

    Wisteria::UI::OpacityDlg dlg(this, opacity, bmp);
    if (dlg.ShowModal() == wxID_OK)
        {
        if (event.GetId() == XRCID("ID_EDIT_PLOT_BKIMAGE_OPACITY"))
            {
            doc->SetPlotBackGroundImageOpacity(dlg.GetOpacity());
            }
        else if (event.GetId() == XRCID("ID_EDIT_BOX_OPACITY"))
            {
            doc->SetGraphBoxOpacity(dlg.GetOpacity());
            }
        else if (event.GetId() == XRCID("ID_EDIT_BAR_OPACITY"))
            {
            doc->SetGraphBarOpacity(dlg.GetOpacity());
            }
        else if (event.GetId() == XRCID("ID_EDIT_HISTOBAR_OPACITY"))
            {
            doc->SetHistogramBarOpacity(dlg.GetOpacity());
            }
        else if (event.GetId() == XRCID("ID_EDIT_PLOT_BKCOLOR_OPACITY"))
            {
            doc->SetPlotBackGroundColorOpacity(dlg.GetOpacity());
            }
        doc->RefreshRequired(ProjectRefresh::Minimal);
        doc->RefreshGraphs();
        }
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnHistoBarLabelSelected(wxCommandEvent& event)
    {
    for (size_t i = 0; i < m_histoBarLabelsMenu.GetMenuItemCount(); ++i)
        {
        m_histoBarLabelsMenu.FindItemByPosition(i)->Check(false);
        }
    if (event.GetId() == XRCID("ID_HISTOBAR_LABELS_PERCENTAGE"))
        {
        dynamic_cast<BaseProjectDoc*>(GetDocument())
            ->SetHistogramBinLabelDisplay(Wisteria::BinLabelDisplay::BinPercentage);
        wxMenuItem* item = m_histoBarLabelsMenu.FindItem(XRCID("ID_HISTOBAR_LABELS_PERCENTAGE"));
        if (item != nullptr)
            {
            item->Check(true);
            }
        }
    else if (event.GetId() == XRCID("ID_HISTOBAR_LABELS_COUNT"))
        {
        dynamic_cast<BaseProjectDoc*>(GetDocument())
            ->SetHistogramBinLabelDisplay(Wisteria::BinLabelDisplay::BinValue);
        wxMenuItem* item = m_histoBarLabelsMenu.FindItem(XRCID("ID_HISTOBAR_LABELS_COUNT"));
        if (item != nullptr)
            {
            item->Check(true);
            }
        }
    else if (event.GetId() == XRCID("ID_HISTOBAR_LABELS_COUNT_AND_PERCENT"))
        {
        dynamic_cast<BaseProjectDoc*>(GetDocument())
            ->SetHistogramBinLabelDisplay(Wisteria::BinLabelDisplay::BinValueAndPercentage);
        wxMenuItem* item =
            m_histoBarLabelsMenu.FindItem(XRCID("ID_HISTOBAR_LABELS_COUNT_AND_PERCENT"));
        if (item != nullptr)
            {
            item->Check(true);
            }
        }
    else if (event.GetId() == XRCID("ID_HISTOBAR_NO_LABELS"))
        {
        dynamic_cast<BaseProjectDoc*>(GetDocument())
            ->SetHistogramBinLabelDisplay(Wisteria::BinLabelDisplay::NoDisplay);
        wxMenuItem* item = m_histoBarLabelsMenu.FindItem(XRCID("ID_HISTOBAR_NO_LABELS"));
        if (item != nullptr)
            {
            item->Check(true);
            }
        }
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnExcludeWordsList([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());

    EditWordListDlg editDlg(this, wxID_ANY, _(L"Edit Words/Phrases To Exclude"));
    editDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                         L"online/program-options.html");
    editDlg.SetPhraseFileMode(true);
    editDlg.SetFilePath(doc->GetExcludedPhrasesPath());
    if (editDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    doc->SetExcludedPhrasesPath(editDlg.GetFilePath());
    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnIncompleteThreshold([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    const long len = wxGetNumberFromUser(_(L"Include incomplete sentences containing more than:"),
                                         wxString{}, _(L"Incomplete Sentence Threshold"),
                                         doc->GetIncludeIncompleteSentencesIfLongerThanValue(), 0,
                                         std::numeric_limits<int>::max());
    if (len == -1)
        {
        return;
        }
    doc->SetIncludeIncompleteSentencesIfLongerThanValue(len);
    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnNumeralSyllabicationOptions(wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());

    doc->SetNumeralSyllabicationMethod((event.GetId() == XRCID("ID_NUMSYL_ONE")) ?
                                           NumeralSyllabize::WholeWordIsOneSyllable :
                                           NumeralSyllabize::SoundOutEachDigit);

    for (size_t i = 0; i < m_numeralSyllabicationMenu.GetMenuItemCount(); ++i)
        {
        m_numeralSyllabicationMenu.FindItemByPosition(i)->Check(false);
        }
    wxMenuItem* item = m_numeralSyllabicationMenu.FindItem(
        (doc->GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable) ?
            XRCID("ID_NUMSYL_ONE") :
            XRCID("ID_NUMSYL_EACH_DIGIT"));
    if (item != nullptr)
        {
        item->Check(true);
        }

    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnTextExclusionOptions(wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    doc->SetInvalidSentenceMethod(
        (event.GetId() == XRCID("ID_TE_ALL_INCOMPLETE")) ? InvalidSentence::ExcludeFromAnalysis :
        (event.GetId() == XRCID("ID_TE_NO_EXCLUDE"))     ? InvalidSentence::IncludeAsFullSentences :
                                                           InvalidSentence::ExcludeExceptForHeadings);

    for (size_t i = 0; i < m_textExclusionMenu.GetMenuItemCount(); ++i)
        {
        m_textExclusionMenu.FindItemByPosition(i)->Check(false);
        }
    wxMenuItem* item = m_textExclusionMenu.FindItem(
        (doc->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis) ?
            XRCID("ID_TE_ALL_INCOMPLETE") :
        (doc->GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ?
            XRCID("ID_TE_NO_EXCLUDE") :
            XRCID("ID_TE_ALL_INCOMPLETE_EXCEPT_HEADERS"));
    if (item != nullptr)
        {
        item->Check(true);
        }
    auto* view = dynamic_cast<BaseProjectView*>(GetView());
    if (view != nullptr && view->GetRibbon() != nullptr)
        {
        wxWindow* exclusionButtonBar =
            view->GetRibbon()->FindWindow(MainFrame::ID_TEXT_EXCLUSION_RIBBON_BUTTON_BAR);
        if ((exclusionButtonBar != nullptr) &&
            exclusionButtonBar->IsKindOf(wxCLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_AGGRESSIVELY"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_COPYRIGHT_NOTICES"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_TRAILING_CITATIONS"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_FILE_ADDRESSES"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_NUMERALS"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_PROPER_NOUNS"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_WORD_LIST"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            }
        wxWindow* numeralButtonBar =
            view->GetRibbon()->FindWindow(MainFrame::ID_NUMERALS_RIBBON_BUTTON_BAR);
        if ((numeralButtonBar != nullptr) &&
            numeralButtonBar->IsKindOf(wxCLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(numeralButtonBar)
                ->EnableButton(
                    XRCID("ID_NUMERAL_SYLLABICATION"),
                    (doc->GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ?
                        true :
                        !doc->IsExcludingNumerals());
            }
        }
    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnExclusionTagsOptions(wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());

    doc->GetExclusionBlockTags().clear();
    if (event.GetId() == XRCID("ID_EXCLUSION_TAGS_CAROTS"))
        {
        doc->GetExclusionBlockTags().emplace_back(L'^', L'^');
        }
    else if (event.GetId() == XRCID("ID_EXCLUSION_TAGS_ANGLES"))
        {
        doc->GetExclusionBlockTags().emplace_back(L'<', L'>');
        }
    else if (event.GetId() == XRCID("ID_EXCLUSION_TAGS_BRACES"))
        {
        doc->GetExclusionBlockTags().emplace_back(L'[', L']');
        }
    else if (event.GetId() == XRCID("ID_EXCLUSION_TAGS_CURLIES"))
        {
        doc->GetExclusionBlockTags().emplace_back(L'{', L'}');
        }
    else if (event.GetId() == XRCID("ID_EXCLUSION_TAGS_PARANS"))
        {
        doc->GetExclusionBlockTags().emplace_back(L'(', L')');
        }

    for (size_t i = 0; i < m_exclusionTagsMenu.GetMenuItemCount(); ++i)
        {
        m_exclusionTagsMenu.FindItemByPosition(i)->Check(false);
        }
    wxMenuItem* item = m_exclusionTagsMenu.FindItem(
        (doc->GetExclusionBlockTags().empty()) ? XRCID("ID_EXCLUSION_TAGS_NOT_ENABLED") :
        (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'^', L'^')) ?
                                                 XRCID("ID_EXCLUSION_TAGS_CAROTS") :
        (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'<', L'>')) ?
                                                 XRCID("ID_EXCLUSION_TAGS_ANGLES") :
        (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'[', L']')) ?
                                                 XRCID("ID_EXCLUSION_TAGS_BRACES") :
        (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'{', L'}')) ?
                                                 XRCID("ID_EXCLUSION_TAGS_CURLIES") :
        (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'(', L')')) ?
                                                 XRCID("ID_EXCLUSION_TAGS_PARANS") :
                                                 XRCID("ID_EXCLUSION_TAGS_NOT_ENABLED"));
    if (item != nullptr)
        {
        item->Check(true);
        }

    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnLineEndOptions(wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    doc->SetParagraphsParsingMethod(
        (event.GetId() == XRCID("ID_LE_ONLY_AFTER_VALID_SENTENCE")) ?
            ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs :
            ParagraphParse::EachNewLineIsAParagraph);

    for (size_t i = 0; i < m_lineEndsMenu.GetMenuItemCount(); ++i)
        {
        m_lineEndsMenu.FindItemByPosition(i)->Check(false);
        }
    wxMenuItem* item =
        m_lineEndsMenu.FindItem((doc->GetParagraphsParsingMethod() ==
                                 ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs) ?
                                    XRCID("ID_LE_ONLY_AFTER_VALID_SENTENCE") :
                                    XRCID("ID_LE_ALWAYS_NEW_PARAGRAPH"));
    if (item != nullptr)
        {
        item->Check(true);
        }

    auto* view = dynamic_cast<BaseProjectView*>(GetView());
    if (view != nullptr && view->GetRibbon() != nullptr)
        {
        wxWindow* deductionButtonBar =
            view->GetRibbon()->FindWindow(MainFrame::ID_PARAGRAPH_DEDUCTION_RIBBON_BUTTON_BAR);
        if ((deductionButtonBar != nullptr) &&
            deductionButtonBar->IsKindOf(wxCLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)
                ->EnableButton(XRCID("ID_IGNORE_BLANK_LINES"),
                               (doc->GetParagraphsParsingMethod() ==
                                ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs));
            dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)
                ->EnableButton(XRCID("ID_IGNORE_INDENTING"),
                               (doc->GetParagraphsParsingMethod() ==
                                ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs));
            }
        }
    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnIgnoreBlankLines([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->IgnoreBlankLinesForParagraphsParser(
        !theProject->IsIgnoringBlankLinesForParagraphsParser());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnIgnoreIndenting([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->IgnoreIndentingForParagraphsParser(
        !theProject->IsIgnoringIndentingForParagraphsParser());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnStrictCapitalization([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->SetSentenceStartMustBeUppercased(!theProject->GetSentenceStartMustBeUppercased());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnAggressivelyExclude([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->AggressiveExclusion(!theProject->IsExcludingAggressively());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnIgnoreCopyrights([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->ExcludeTrailingCopyrightNoticeParagraphs(
        !theProject->IsExcludingTrailingCopyrightNoticeParagraphs());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnIgnoreCitations([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->ExcludeTrailingCitations(!theProject->IsExcludingTrailingCitations());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnIgnoreFileAddresses([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->ExcludeFileAddresses(!theProject->IsExcludingFileAddresses());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnIgnoreNumerals([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->ExcludeNumerals(!theProject->IsExcludingNumerals());
    auto* view = dynamic_cast<BaseProjectView*>(GetView());
    wxASSERT(view);
    if (view != nullptr && view->GetRibbon() != nullptr)
        {
        wxWindow* numeralButtonBar =
            view->GetRibbon()->FindWindow(MainFrame::ID_NUMERALS_RIBBON_BUTTON_BAR);
        if ((numeralButtonBar != nullptr) &&
            numeralButtonBar->IsKindOf(wxCLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(numeralButtonBar)
                ->EnableButton(XRCID("ID_NUMERAL_SYLLABICATION"),
                               !theProject->IsExcludingNumerals());
            }
        }
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnIgnoreProperNouns([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->ExcludeProperNouns(!theProject->IsExcludingProperNouns());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnEditDictionaryButton([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    wxGetApp().EditDictionary(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetProjectLanguage());
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnEditEnglishDictionary([[maybe_unused]] wxCommandEvent& event)
    {
    wxGetApp().EditDictionary(readability::test_language::english_test);
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnEditDictionaryProjectSettings([[maybe_unused]] wxCommandEvent& event)
    {
    auto* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    ToolsOptionsDlg optionsDlg(this, theProject, ToolsOptionsDlg::Grammar);
    optionsDlg.SelectPage(ToolsOptionsDlg::GRAMMAR_PAGE);
    if (optionsDlg.ShowModal() == wxID_OK)
        {
        theProject->RefreshProject();
        }
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnEditDictionary([[maybe_unused]] wxCommandEvent& event)
    {
    wxGetApp().EditDictionary(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetProjectLanguage());
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnEditCustomTestBundle(wxCommandEvent& event)
    {
    wxGetApp().GetMainFrameEx()->OnEditCustomTestBundle(event);
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnRemoveCustomTestBundle(wxCommandEvent& event)
    {
    wxGetApp().GetMainFrameEx()->OnRemoveCustomTestBundle(event);
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnCustomTestBundle(wxCommandEvent& event)
    {
    const int menuId = event.GetId();
    auto pos = MainFrame::GetTestBundleMenuIds().find(menuId);
    if (pos == MainFrame::GetTestBundleMenuIds().end())
        {
        wxMessageBox(
            _(L"Unable to find test bundle: internal error, please contact software vendor."),
            _(L"Error"), wxOK | wxICON_ERROR);
        return;
        }
    // Add the tests & goals to the project
    if (GetDocument() != nullptr)
        {
        auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
        wxASSERT(doc);
        if ((doc != nullptr) && doc->IsSafeToUpdate())
            {
            const bool hadDolchSightWords = doc->IsIncludingDolchSightWords();
            doc->ApplyTestBundle(pos->second);
            if (doc->IsKindOf(wxCLASSINFO(ProjectDoc)))
                {
                // if Dolch was added or removed then refresh
                if ((!hadDolchSightWords && doc->IsIncludingDolchSightWords()) ||
                    (hadDolchSightWords && !doc->IsIncludingDolchSightWords()))
                    {
                    doc->RefreshRequired(ProjectRefresh::FullReindexing);
                    }
                // refresh
                doc->RefreshRequired(ProjectRefresh::Minimal);
                doc->RefreshProject();
                }
            else if (doc->IsKindOf(wxCLASSINFO(BatchProjectDoc)))
                {
                doc->RefreshRequired(ProjectRefresh::Minimal);
                doc->RefreshProject();
                }
            doc->Modify(true);
            }
        }
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnSaveDropdown(wxRibbonButtonBarEvent& evt)
    {
    evt.PopupMenu(&m_saveMenu);
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnDictionaryDropdown(wxRibbonButtonBarEvent& evt)
    {
    wxMenu menu;
    menu.Append(XRCID("ID_EDIT_ENGLISH_DICTIONARY"), _(L"Edit Custom Dictionary..."));
    menu.Append(XRCID("ID_ADD_ITEM_TO_DICTIONARY"), _(L"Add Selected Words"));
    menu.Append(XRCID("ID_EDIT_DICTIONARY_PROJECT_SETTINGS"), _(L"Settings (Current Project)..."));
    menu.Append(XRCID("ID_EDIT_DICTIONARY_SETTINGS"), _(L"Settings (Globally)..."));
    evt.PopupMenu(&menu);
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnPrimaryAgeTestsDropdown(wxRibbonButtonBarEvent& evt)
    {
    if (m_primaryAgeTestsMenu.GetMenuItemCount() == 0)
        {
        wxMessageBox(_(L"No tests are available in this category."), wxGetApp().GetAppDisplayName(),
                     wxOK | wxICON_INFORMATION);
        }
    else
        {
        evt.PopupMenu(&m_primaryAgeTestsMenu);
        }
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnSecondaryAgeTestsDropdown(wxRibbonButtonBarEvent& evt)
    {
    if (m_secondaryAgeTestsMenu.GetMenuItemCount() == 0)
        {
        wxMessageBox(_(L"No tests are available in this category."), wxGetApp().GetAppDisplayName(),
                     wxOK | wxICON_INFORMATION);
        }
    else
        {
        evt.PopupMenu(&m_secondaryAgeTestsMenu);
        }
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnAdultTestsDropdown(wxRibbonButtonBarEvent& evt)
    {
    if (m_adultTestsMenu.GetMenuItemCount() == 0)
        {
        wxMessageBox(_(L"No tests are available in this category."), wxGetApp().GetAppDisplayName(),
                     wxOK | wxICON_INFORMATION);
        }
    else
        {
        evt.PopupMenu(&m_adultTestsMenu);
        }
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnSecondLanguageTestsDropdown(wxRibbonButtonBarEvent& evt)
    {
    if (m_secondLanguageTestsMenu.GetMenuItemCount() == 0)
        {
        wxMessageBox(_(L"No tests are available in this category."), wxGetApp().GetAppDisplayName(),
                     wxOK | wxICON_INFORMATION);
        }
    else
        {
        evt.PopupMenu(&m_secondLanguageTestsMenu);
        }
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnRemoveCustomTest(wxCommandEvent& event)
    {
    wxGetApp().GetMainFrameEx()->OnRemoveCustomTest(event);
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnAddCustomTest(wxCommandEvent& event)
    {
    wxGetApp().GetMainFrameEx()->OnAddCustomTest(event);
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnEditCustomTest(wxCommandEvent& event)
    {
    wxGetApp().GetMainFrameEx()->OnEditCustomTest(event);
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnCustomTest(wxCommandEvent& event)
    {
    const int menuId = event.GetId();
    auto pos = MainFrame::GetCustomTestMenuIds().find(menuId);
    if (pos == MainFrame::GetCustomTestMenuIds().end())
        {
        wxMessageBox(
            _(L"Unable to find custom test: internal error, please contact software vendor."),
            _(L"Error"), wxOK | wxICON_ERROR);
        return;
        }
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    if ((doc != nullptr) && doc->IsSafeToUpdate())
        {
        doc->AddCustomReadabilityTest(pos->second);
        // projects will need to do a full re-indexing
        doc->RefreshRequired(ProjectRefresh::FullReindexing);
        doc->RefreshProject();
        }
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnFindMenu([[maybe_unused]] wxCommandEvent& event)
    {
    auto* view = dynamic_cast<BaseProjectView*>(GetView());
    if (view != nullptr && view->GetSearchPanel() != nullptr)
        {
        view->GetSearchPanel()->Activate();
        }
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnFindNext([[maybe_unused]] wxCommandEvent& event)
    {
    auto* view = dynamic_cast<BaseProjectView*>(GetView());
    if (view != nullptr && view->GetSearchPanel() != nullptr)
        {
        const wxCommandEvent cmd(wxEVT_NULL);
        view->GetSearchPanel()->OnSearch(cmd);
        }
    }

//---------------------------------------------------
void ProjectDocChildFrame::OnAddCustomTestBundle(wxCommandEvent& event)
    {
    wxGetApp().GetMainFrameEx()->OnAddCustomTestBundle(event);
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnProjectSettings([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    wxASSERT(doc);
    if ((doc == nullptr) || !doc->IsSafeToUpdate())
        {
        return;
        }
    ToolsOptionsDlg optionsDlg(this, doc);
    if (optionsDlg.ShowModal() == wxID_OK)
        {
        doc->RefreshProject();
        auto* view = dynamic_cast<BaseProjectView*>(GetView());
        wxASSERT(view);
        if (view != nullptr)
            {
            view->UpdateRibbonState();
            }
        }
    }

//-------------------------------------------------------
void ProjectDocChildFrame::OnToolsOptions([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    const wxString previousReportTheme = wxGetApp().GetAppOptions()->GetReportTheme();
    ToolsOptionsDlg optionsDlg(this);
    if (optionsDlg.ShowModal() == wxID_OK)
        {
        wxGetApp().GetAppOptions()->SaveOptionsFile();
        MainFrame::RefreshOpenProjectsIfThemeChanged(previousReportTheme);
        }
    }
