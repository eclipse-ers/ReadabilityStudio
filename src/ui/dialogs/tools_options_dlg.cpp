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

#include "tools_options_dlg.h"
#include "../../Wisteria-Dataviz/src/base/colorbrewer.h"
#include "../../Wisteria-Dataviz/src/graphs/danielsonbryan2plot.h"
#include "../../Wisteria-Dataviz/src/graphs/lixgauge.h"
#include "../../Wisteria-Dataviz/src/graphs/lixgaugegerman.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/warningmessagesdlg.h"
#include "../../app/readability_app.h"
#include "../../app/readability_app_options.h"
#include "../../graphs/schwartzgraph.h"
#include "../../projects/base_project_view.h"
#include "../../projects/batch_project_doc.h"
#include "../../projects/standard_project_doc.h"
#include "../controls/word_list_property.h"
#include "doc_group_select_dlg.h"
#include "edit_word_list_dlg.h"
#include <wx/bannerwindow.h>
#include <wx/colordlg.h>
#include <wx/dir.h>
#include <wx/valgen.h>
#include <wx/wx.h>

wxDECLARE_APP(ReadabilityApp);

wxIMPLEMENT_CLASS(ToolsOptionsDlg, wxDialog)

    namespace
    {
    // "oceanic.css" -> "Oceanic"
    wxString ThemeFileNameToLabel(const wxString& fileName)
        {
        wxString label{ wxFileName(fileName).GetName() };
        label.Replace(L"-", L" ");
        if (!label.empty())
            {
            label[0] = wxToupper(label[0]);
            }
        return label;
        }

    // "Oceanic" -> "oceanic.css"
    wxString ThemeLabelToFileName(const wxString& label)
        {
        wxString fileName{ label.Lower() };
        fileName.Replace(L" ", L"-");
        fileName += L".css";
        return fileName;
        }
    } // namespace

//-------------------------------------------------------------
void ToolsOptionsDlg::OnExcludeNumeralsCheck(wxCommandEvent& event)
    {
    if (m_syllableLabel != nullptr)
        {
        m_syllableLabel->Enable(!event.IsChecked());
        }
    if (m_syllableCombo != nullptr)
        {
        m_syllableCombo->Enable(!event.IsChecked());
        }
    if (m_readTestsSyllableLabel != nullptr)
        {
        m_readTestsSyllableLabel->Enable(!event.IsChecked());
        }
    if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetFleschNumeralSyllabicationLabel()))
        {
        m_readabilityTestsPropertyGrid->EnableProperty(GetFleschNumeralSyllabicationLabel(),
                                                       !event.IsChecked());
        }
    if (IsPropertyAvailable(m_readabilityTestsPropertyGrid,
                            GetFleschKincaidNumeralSyllabicationLabel()))
        {
        m_readabilityTestsPropertyGrid->EnableProperty(GetFleschKincaidNumeralSyllabicationLabel(),
                                                       !event.IsChecked());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnExclusionBlockTagChange([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    m_exclusionBlockTags.get_value().clear();
    if (m_exclusionBlockTagsOption == 1)
        {
        m_exclusionBlockTags.get_value().emplace_back(L'^', L'^');
        }
    else if (m_exclusionBlockTagsOption == 2)
        {
        m_exclusionBlockTags.get_value().emplace_back(L'<', L'>');
        }
    else if (m_exclusionBlockTagsOption == 3)
        {
        m_exclusionBlockTags.get_value().emplace_back(L'[', L']');
        }
    else if (m_exclusionBlockTagsOption == 4)
        {
        m_exclusionBlockTags.get_value().emplace_back(L'{', L'}');
        }
    else if (m_exclusionBlockTagsOption == 5)
        {
        m_exclusionBlockTags.get_value().emplace_back(L'(', L')');
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnNumberSyllabizeChange([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (m_readTestsSyllableLabel != nullptr)
        {
        m_readTestsSyllableLabel->SetLabel(
            (m_syllabicationMethod == static_cast<int>(NumeralSyllabize::WholeWordIsOneSyllable)) ?
                _(L"* Numeral syllabication system default: numerals are one syllable.") :
                _(L"* Numeral syllabication system default: sound out each digit."));
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDeleteFileClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_fileList != nullptr)
        {
        m_fileList->DeleteSelectedItems();
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnAddFileClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_fileList != nullptr)
        {
        m_fileList->EditItem(m_fileList->AddRow(), 0);
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnAddFilesClick([[maybe_unused]] wxCommandEvent& event)
    {
    wxFileDialog dialog(this, _(L"Add Documents to Project"), wxEmptyString, wxEmptyString,
                        ReadabilityAppOptions::GetDocumentFilter(),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW | wxFD_MULTIPLE);

    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    wxArrayString files;
    dialog.GetPaths(files);

    if (files.GetCount() == 0)
        {
        return;
        }

    // see what sort of labeling should be used
    DocGroupSelectDlg selectLabelTypeDlg(this);
    if (selectLabelTypeDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    const size_t currentFileCount = m_fileData->GetItemCount();
    m_fileData->SetSize(currentFileCount + files.GetCount(), 2);
    for (size_t i = 0; i < files.GetCount(); ++i)
        {
        m_fileData->SetItemText(
            currentFileCount + i, 0, files.Item(i),
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        if (selectLabelTypeDlg.GetSelection() == 1)
            {
            m_fileData->SetItemText(
                currentFileCount + i, 1, selectLabelTypeDlg.GetGroupingLabel(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        }
    m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
    m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
    m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
    m_fileList->SetItemBeenEditedByUser(true);
    if (m_fileList->GetItemCount() > 0)
        {
        m_fileList->EnsureVisible(m_fileList->GetItemCount() - 1);
        }
    if (m_docStorageRadioBox != nullptr)
        {
        m_docStorageRadioBox->Enable(false);
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnIncompleteSentencesChange([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (m_syllableLabel != nullptr)
        {
        m_syllableLabel->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
            !m_excludeNumerals);
        }
    if (m_syllableCombo != nullptr)
        {
        m_syllableCombo->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
            !m_excludeNumerals);
        }
    if (m_readTestsSyllableLabel != nullptr)
        {
        m_readTestsSyllableLabel->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
            !m_excludeNumerals);
        }
    if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetFleschNumeralSyllabicationLabel()))
        {
        m_readabilityTestsPropertyGrid->EnableProperty(
            GetFleschNumeralSyllabicationLabel(),
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
                !m_excludeNumerals);
        }
    if (IsPropertyAvailable(m_readabilityTestsPropertyGrid,
                            GetFleschKincaidNumeralSyllabicationLabel()))
        {
        m_readabilityTestsPropertyGrid->EnableProperty(
            GetFleschKincaidNumeralSyllabicationLabel(),
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
                !m_excludeNumerals);
        }
    if (m_ignoreCopyrightsCheckBox != nullptr)
        {
        m_ignoreCopyrightsCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        }
    if (m_ignoreCitationsCheckBox != nullptr)
        {
        m_ignoreCitationsCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        }
    if (m_ignoreFileAddressesCheckBox != nullptr)
        {
        m_ignoreFileAddressesCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        }
    if (m_ignoreNumeralsCheckBox != nullptr)
        {
        m_ignoreNumeralsCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        }
    if (m_ignoreProperNounsCheckBox != nullptr)
        {
        m_ignoreProperNounsCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        }
    if (m_includeExcludedPhraseFirstOccurrenceCheckBox != nullptr)
        {
        m_includeExcludedPhraseFirstOccurrenceCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        }
    if (m_excludedPhrasesPathFilePathEdit != nullptr)
        {
        m_excludedPhrasesPathFilePathEdit->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        }
    if (m_excludedPhrasesEditBrowseButton != nullptr)
        {
        m_excludedPhrasesEditBrowseButton->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        }
    if (m_exclusionBlockTagsCombo != nullptr)
        {
        m_exclusionBlockTagsCombo->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        }
    if (m_exclusionBlockTagsLabel != nullptr)
        {
        m_exclusionBlockTagsLabel->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        }
    if (m_aggressiveExclusionCheckBox != nullptr)
        {
        m_aggressiveExclusionCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        }
    if (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis))
        {
        if (m_textExclusionLabel != nullptr)
            {
            m_textExclusionLabel->SetLabel(
                _(L"** Text exclusion system default: exclude all incomplete sentences."));
            }
        }
    else if (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings))
        {
        if (m_textExclusionLabel != nullptr)
            {
            m_textExclusionLabel->SetLabel(_(L"** Text exclusion system default: exclude all "
                                             "incomplete sentences, except headings."));
            }
        }
    else if (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences))
        {
        if (m_textExclusionLabel != nullptr)
            {
            m_textExclusionLabel->SetLabel(
                _(L"** Text exclusion system default: do not exclude any text."));
            }
        }
    TransferDataToWindow();
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnParagraphParseChange([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (m_ignoreBlankLinesCheckBox != nullptr)
        {
        m_ignoreBlankLinesCheckBox->Enable(
            static_cast<ParagraphParse>(m_paragraphParsingMethod.get_value()) ==
            ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs);
        }
    if (m_ignoreIndentingCheckBox != nullptr)
        {
        m_ignoreIndentingCheckBox->Enable(
            static_cast<ParagraphParse>(m_paragraphParsingMethod.get_value()) ==
            ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs);
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnWarningMessagesButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    Wisteria::UI::WarningMessagesDlg dlg(this);
    dlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"online/program-options.html");
    dlg.ShowModal();
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnExcludedPhrasesFileEditButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    EditWordListDlg editDlg(this, wxID_ANY, _(L"Edit Words/Phrases To Exclude"));
    editDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                         L"online/program-options.html");
    editDlg.SetPhraseFileMode(true);
    editDlg.SetFilePath(m_excludedPhrasesPath.get_value());
    if (editDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    m_excludedPhrasesPath = editDlg.GetFilePath();
    m_excludedPhrasesEdited = true;
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDocumentStorageRadioButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (m_filePathEdit != nullptr)
        {
        m_filePathEdit->Enable(m_documentStorageMethod ==
                               static_cast<int>(TextStorage::NoEmbedText));
        }
    if (m_fileBrowseButton != nullptr)
        {
        m_fileBrowseButton->Enable(m_documentStorageMethod ==
                                   static_cast<int>(TextStorage::NoEmbedText));
        }
    if (m_fileList != nullptr && m_addFileButton != nullptr && m_deleteFileButton != nullptr &&
        m_addFilesButton != nullptr)
        {
        m_fileList->Enable(m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText));
        m_addFileButton->Enable(m_documentStorageMethod ==
                                static_cast<int>(TextStorage::NoEmbedText));
        m_deleteFileButton->Enable(m_documentStorageMethod ==
                                   static_cast<int>(TextStorage::NoEmbedText));
        m_addFilesButton->Enable(m_documentStorageMethod ==
                                 static_cast<int>(TextStorage::NoEmbedText));
        }
    if (m_realTimeUpdateCheckBox != nullptr)
        {
        m_realTimeUpdateCheckBox->Enable(m_documentStorageMethod ==
                                         static_cast<int>(TextStorage::NoEmbedText));
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnAdditionalDocumentFileBrowseButtonClick(
    [[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    const wxFileName fn(m_appendedDocumentFilePath.get_value());
    wxFileDialog dialog(this, _(L"Select Document to Append"), fn.GetPath(), fn.GetFullName(),
                        ReadabilityAppOptions::GetDocumentFilter(),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    m_appendedDocumentFilePath = dialog.GetPath();
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnFileBrowseButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    const wxFileName fn(m_filePath.get_value());
    wxFileDialog dialog(this, _(L"Select Document to Analyze"), fn.GetPath(), fn.GetFullName(),
                        ReadabilityAppOptions::GetDocumentFilter(),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    m_filePath = dialog.GetPath();
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnFontSelect(wxCommandEvent& event)
    {
    wxFontData data;
    switch (event.GetId())
        {
    case ID_FONT_BUTTON:
        data.SetInitialFont(m_font.get_value());
        data.SetColour(m_fontColor.get_value());
        break;
    case ID_X_AXIS_FONT_BUTTON:
        data.SetInitialFont(m_xAxisFont.get_value());
        data.SetColour(m_xAxisFontColor.get_value());
        break;
    case ID_Y_AXIS_FONT_BUTTON:
        data.SetInitialFont(m_yAxisFont.get_value());
        data.SetColour(m_yAxisFontColor.get_value());
        break;
    case ID_GRAPH_TOP_TITLE_FONT_BUTTON:
        data.SetInitialFont(m_topTitleFont.get_value());
        data.SetColour(m_topTitleFontColor.get_value());
        break;
    case ID_GRAPH_BOTTOM_TITLE_FONT_BUTTON:
        data.SetInitialFont(m_bottomTitleFont.get_value());
        data.SetColour(m_bottomTitleFontColor.get_value());
        break;
    case ID_GRAPH_LEFT_TITLE_FONT_BUTTON:
        data.SetInitialFont(m_leftTitleFont.get_value());
        data.SetColour(m_leftTitleFontColor.get_value());
        break;
    case ID_GRAPH_RIGHT_TITLE_FONT_BUTTON:
        data.SetInitialFont(m_rightTitleFont.get_value());
        data.SetColour(m_rightTitleFontColor.get_value());
        break;
    default:
        // no-op
        break;
        };

    wxFontDialog dialog(this, data);
    if (dialog.ShowModal() == wxID_OK)
        {
        switch (event.GetId())
            {
        case ID_FONT_BUTTON:
            m_font = dialog.GetFontData().GetChosenFont();
            m_fontColor = dialog.GetFontData().GetColour();
            break;
        case ID_X_AXIS_FONT_BUTTON:
            m_xAxisFont = dialog.GetFontData().GetChosenFont();
            m_xAxisFontColor = dialog.GetFontData().GetColour();
            break;
        case ID_Y_AXIS_FONT_BUTTON:
            m_yAxisFont = dialog.GetFontData().GetChosenFont();
            m_yAxisFontColor = dialog.GetFontData().GetColour();
            break;
        case ID_GRAPH_TOP_TITLE_FONT_BUTTON:
            m_topTitleFont = dialog.GetFontData().GetChosenFont();
            m_topTitleFontColor = dialog.GetFontData().GetColour();
            break;
        case ID_GRAPH_BOTTOM_TITLE_FONT_BUTTON:
            m_bottomTitleFont = dialog.GetFontData().GetChosenFont();
            m_bottomTitleFontColor = dialog.GetFontData().GetColour();
            break;
        case ID_GRAPH_LEFT_TITLE_FONT_BUTTON:
            m_leftTitleFont = dialog.GetFontData().GetChosenFont();
            m_leftTitleFontColor = dialog.GetFontData().GetColour();
            break;
        case ID_GRAPH_RIGHT_TITLE_FONT_BUTTON:
            m_rightTitleFont = dialog.GetFontData().GetChosenFont();
            m_rightTitleFontColor = dialog.GetFontData().GetColour();
            break;
        default:
            // no-op
            break;
            };
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_highlightColorButton == nullptr)
        {
        return;
        }
    wxColourData data;
    wxGetApp().GetAppOptions()->CopyCustomColorsToColorData(data);
    data.SetChooseFull(true);
    data.SetColour(m_highlightedColor.get_value());

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_highlightedColor = dialog.GetColourData().GetColour();
        m_highlightColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_highlightedColor.get_value()));
        wxGetApp().GetAppOptions()->CopyColorDataToCustomColors(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnExcludedHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_excludedHighlightColorButton == nullptr)
        {
        return;
        }
    wxColourData data;
    wxGetApp().GetAppOptions()->CopyCustomColorsToColorData(data);
    data.SetChooseFull(true);
    data.SetColour(m_excludedTextHighlightColor.get_value());

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_excludedTextHighlightColor = dialog.GetColourData().GetColour();
        m_excludedHighlightColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_excludedTextHighlightColor.get_value()));
        wxGetApp().GetAppOptions()->CopyColorDataToCustomColors(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDupWordHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_duplicateWordHighlightColorButton == nullptr)
        {
        return;
        }
    wxColourData data;
    wxGetApp().GetAppOptions()->CopyCustomColorsToColorData(data);
    data.SetChooseFull(true);
    data.SetColour(m_duplicateWordHighlightColor.get_value());

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_duplicateWordHighlightColor = dialog.GetColourData().GetColour();
        m_duplicateWordHighlightColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_duplicateWordHighlightColor.get_value()));
        wxGetApp().GetAppOptions()->CopyColorDataToCustomColors(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnWordyPhraseHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_wordyPhraseHighlightColorButton == nullptr)
        {
        return;
        }
    wxColourData data;
    wxGetApp().GetAppOptions()->CopyCustomColorsToColorData(data);
    data.SetChooseFull(true);
    data.SetColour(m_wordyPhraseHighlightColor.get_value());

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_wordyPhraseHighlightColor = dialog.GetColourData().GetColour();
        m_wordyPhraseHighlightColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_wordyPhraseHighlightColor.get_value()));
        wxGetApp().GetAppOptions()->CopyColorDataToCustomColors(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDolchConjunctionsHighlightColorSelect(
    [[maybe_unused]] wxCommandEvent& event)
    {
    if (m_DolchConjunctionsColorButton == nullptr)
        {
        return;
        }
    wxColourData data;
    wxGetApp().GetAppOptions()->CopyCustomColorsToColorData(data);
    data.SetChooseFull(true);
    data.SetColour(m_dolchConjunctionsColor.get_value());

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_dolchConjunctionsColor = dialog.GetColourData().GetColour();
        m_DolchConjunctionsColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_dolchConjunctionsColor.get_value()));
        wxGetApp().GetAppOptions()->CopyColorDataToCustomColors(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDolchPrepositionsHighlightColorSelect(
    [[maybe_unused]] wxCommandEvent& event)
    {
    if (m_DolchPrepositionsColorButton == nullptr)
        {
        return;
        }
    wxColourData data;
    wxGetApp().GetAppOptions()->CopyCustomColorsToColorData(data);
    data.SetChooseFull(true);
    data.SetColour(m_dolchPrepositionsColor.get_value());

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_dolchPrepositionsColor = dialog.GetColourData().GetColour();
        m_DolchPrepositionsColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_dolchPrepositionsColor.get_value()));
        wxGetApp().GetAppOptions()->CopyColorDataToCustomColors(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDolchPronounsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_DolchPronounsColorButton == nullptr)
        {
        return;
        }
    wxColourData data;
    wxGetApp().GetAppOptions()->CopyCustomColorsToColorData(data);
    data.SetChooseFull(true);
    data.SetColour(m_dolchPronounsColor.get_value());

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_dolchPronounsColor = dialog.GetColourData().GetColour();
        m_DolchPronounsColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_dolchPronounsColor.get_value()));
        wxGetApp().GetAppOptions()->CopyColorDataToCustomColors(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDolchAdverbsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_DolchAdverbsColorButton == nullptr)
        {
        return;
        }
    wxColourData data;
    wxGetApp().GetAppOptions()->CopyCustomColorsToColorData(data);
    data.SetChooseFull(true);
    data.SetColour(m_dolchAdverbsColor.get_value());

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_dolchAdverbsColor = dialog.GetColourData().GetColour();
        m_DolchAdverbsColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_dolchAdverbsColor.get_value()));
        wxGetApp().GetAppOptions()->CopyColorDataToCustomColors(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDolchHighlightColorSelect(wxCommandEvent& event)
    {
    if (event.GetId() == ID_DOLCH_ADJECTIVES_COLOR_BUTTON)
        {
        wxColourData data;
        wxGetApp().GetAppOptions()->CopyCustomColorsToColorData(data);
        data.SetChooseFull(true);
        data.SetColour(m_dolchAdjectivesColor.get_value());

        wxColourDialog dialog(this, &data);
        if (dialog.ShowModal() == wxID_OK)
            {
            m_dolchAdjectivesColor = dialog.GetColourData().GetColour();
            m_DolchAdjectivesColorButton->SetBitmapLabel(
                ResourceManager::CreateColorIcon(m_dolchAdjectivesColor.get_value()));
            wxGetApp().GetAppOptions()->CopyColorDataToCustomColors(dialog.GetColourData());
            }
        }
    else if (event.GetId() == ID_DOLCH_VERBS_COLOR_BUTTON)
        {
        wxColourData data;
        wxGetApp().GetAppOptions()->CopyCustomColorsToColorData(data);
        data.SetChooseFull(true);
        data.SetColour(m_dolchVerbsColor.get_value());

        wxColourDialog dialog(this, &data);
        if (dialog.ShowModal() == wxID_OK)
            {
            m_dolchVerbsColor = dialog.GetColourData().GetColour();
            m_DolchVerbsColorButton->SetBitmapLabel(
                ResourceManager::CreateColorIcon(m_dolchVerbsColor.get_value()));
            wxGetApp().GetAppOptions()->CopyColorDataToCustomColors(dialog.GetColourData());
            }
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDolchNounHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_DolchNounsColorButton == nullptr)
        {
        return;
        }
    wxColourData data;
    wxGetApp().GetAppOptions()->CopyCustomColorsToColorData(data);
    data.SetChooseFull(true);
    data.SetColour(m_dolchNounsColor.get_value());

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_dolchNounsColor = dialog.GetColourData().GetColour();
        m_DolchNounsColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_dolchNounsColor.get_value()));
        wxGetApp().GetAppOptions()->CopyColorDataToCustomColors(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
ToolsOptionsDlg::ToolsOptionsDlg(wxWindow* parent, BaseProjectDoc* project /*= nullptr*/,
                                 const ToolSections sectionsToInclude /*= AllSections*/)
    : m_readabilityProjectDoc(project), m_userAgent(wxGetApp().GetAppOptions()->GetUserAgent()),
      m_disablePeerVerify(wxGetApp().GetAppOptions()->IsPeerVerifyDisabled()),
      m_useJsCookies(wxGetApp().GetAppOptions()->IsUsingJavaScriptCookies()),
      m_persistJsCookies(wxGetApp().GetAppOptions()->IsPersistingJavaScriptCookies()),
      m_uiLanguage(static_cast<int>(wxGetApp().GetAppOptions()->GetUiLanguage())),
      // report options
      m_reportTheme(ThemeFileNameToLabel(wxGetApp().GetAppOptions()->GetReportTheme())),
      m_disableGpuAcceleration(wxGetApp().GetAppOptions()->IsGpuAccelerationDisabled()),
      // log options
      m_logVerbose(wxGetApp().GetLogFile() != nullptr ? wxLog::GetVerbose() : false),
      m_logAppendDailyLog(wxGetApp().GetAppOptions()->IsAppendingDailyLog()),
      m_showLogTab(wxGetApp().GetAppOptions()->IsShowingLogTab()),
      m_logAutoRefresh(wxGetApp().GetAppOptions()->IsLogAutoRefresh()),
      // scripting options
      m_showDeveloperTab(wxGetApp().GetAppOptions()->IsShowingDeveloperTab()),
      m_luaUnsafeMode(wxGetApp().GetAppOptions()->IsLuaUnsafeModeEnabled()),
      // project settings
      m_projectLanguage(static_cast<int>((project != nullptr) ?
                                             project->GetProjectLanguage() :
                                             wxGetApp().GetAppOptions()->GetProjectLanguage())),
      m_reviewer((project != nullptr) ? project->GetReviewer() :
                                        wxGetApp().GetAppOptions()->GetReviewer()),
      m_status((project != nullptr) ? project->GetStatus() : wxString{}),
      m_description(((project != nullptr) && !project->GetSourceFilesInfo().empty()) ?
                        project->GetSourceFilesInfo().at(0).second :
                        wxString()),
      m_appendedDocumentFilePath((project != nullptr) ?
                                     project->GetAppendedDocumentFilePath() :
                                     wxGetApp().GetAppOptions()->GetAppendedDocumentFilePath()),
      m_realTimeUpdate((project != nullptr) ? project->IsRealTimeUpdating() :
                                              wxGetApp().GetAppOptions()->IsRealTimeUpdating()),
      // general program options needs to show everything
      m_sectionsBeingShown((project != nullptr) ? sectionsToInclude : AllSections),
      // document storage/linking information
      m_documentStorageMethod(
          (project != nullptr) ?
              static_cast<int>(project->GetDocumentStorageMethod()) :
              static_cast<int>(wxGetApp().GetAppOptions()->GetDocumentStorageMethod())),
      m_filePath((project != nullptr) ? project->GetOriginalDocumentFilePath() : wxString{}),
      // text view highlighting
      m_textHighlightMethod(
          (project != nullptr) ?
              static_cast<int>(project->GetTextHighlightMethod()) :
              static_cast<int>(wxGetApp().GetAppOptions()->GetTextHighlightMethod())),
      m_highlightedColor((project != nullptr) ?
                             project->GetTextHighlightColor() :
                             wxGetApp().GetAppOptions()->GetTextHighlightColor()),
      m_excludedTextHighlightColor((project != nullptr) ?
                                       project->GetExcludedTextHighlightColor() :
                                       wxGetApp().GetAppOptions()->GetExcludedTextHighlightColor()),
      m_duplicateWordHighlightColor(
          (project != nullptr) ? project->GetDuplicateWordHighlightColor() :
                                 wxGetApp().GetAppOptions()->GetDuplicateWordHighlightColor()),
      m_wordyPhraseHighlightColor((project != nullptr) ?
                                      project->GetWordyPhraseHighlightColor() :
                                      wxGetApp().GetAppOptions()->GetWordyPhraseHighlightColor()),
      m_font((project != nullptr) ? project->GetTextViewFont() :
                                    wxGetApp().GetAppOptions()->GetTextViewFont()),
      m_fontColor((project != nullptr) ? project->GetTextFontColor() :
                                         wxGetApp().GetAppOptions()->GetTextFontColor()),
      // dolch
      m_dolchConjunctionsColor((project != nullptr) ?
                                   project->GetDolchConjunctionsColor() :
                                   wxGetApp().GetAppOptions()->GetDolchConjunctionsColor()),
      m_dolchPrepositionsColor((project != nullptr) ?
                                   project->GetDolchPrepositionsColor() :
                                   wxGetApp().GetAppOptions()->GetDolchPrepositionsColor()),
      m_dolchPronounsColor((project != nullptr) ?
                               project->GetDolchPronounsColor() :
                               wxGetApp().GetAppOptions()->GetDolchPronounsColor()),
      m_dolchAdverbsColor((project != nullptr) ?
                              project->GetDolchAdverbsColor() :
                              wxGetApp().GetAppOptions()->GetDolchAdverbsColor()),
      m_dolchAdjectivesColor((project != nullptr) ?
                                 project->GetDolchAdjectivesColor() :
                                 wxGetApp().GetAppOptions()->GetDolchAdjectivesColor()),
      m_dolchVerbsColor((project != nullptr) ? project->GetDolchVerbsColor() :
                                               wxGetApp().GetAppOptions()->GetDolchVerbsColor()),
      m_dolchNounsColor((project != nullptr) ? project->GetDolchNounColor() :
                                               wxGetApp().GetAppOptions()->GetDolchNounsColor()),
      m_highlightDolchConjunctions(
          (project != nullptr) ? project->IsHighlightingDolchConjunctions() :
                                 wxGetApp().GetAppOptions()->IsHighlightingDolchConjunctions()),
      m_highlightDolchPrepositions(
          (project != nullptr) ? project->IsHighlightingDolchPrepositions() :
                                 wxGetApp().GetAppOptions()->IsHighlightingDolchPrepositions()),
      m_highlightDolchPronouns((project != nullptr) ?
                                   project->IsHighlightingDolchPronouns() :
                                   wxGetApp().GetAppOptions()->IsHighlightingDolchPronouns()),
      m_highlightDolchAdverbs((project != nullptr) ?
                                  project->IsHighlightingDolchAdverbs() :
                                  wxGetApp().GetAppOptions()->IsHighlightingDolchAdverbs()),
      m_highlightDolchAdjectives((project != nullptr) ?
                                     project->IsHighlightingDolchAdjectives() :
                                     wxGetApp().GetAppOptions()->IsHighlightingDolchAdjectives()),
      m_highlightDolchVerbs((project != nullptr) ?
                                project->IsHighlightingDolchVerbs() :
                                wxGetApp().GetAppOptions()->IsHighlightingDolchVerbs()),
      m_highlightDolchNouns((project != nullptr) ?
                                project->IsHighlightingDolchNouns() :
                                wxGetApp().GetAppOptions()->IsHighlightingDolchNouns()),
      // long sentence method
      m_longSentencesNumberOfWords(
          (project != nullptr) ?
              (project->GetLongSentenceMethod() == LongSentence::LongerThanSpecifiedLength) :
              (wxGetApp().GetAppOptions()->GetLongSentenceMethod() ==
               LongSentence::LongerThanSpecifiedLength)),
      m_sentenceLength((project != nullptr) ?
                           project->GetDifficultSentenceLength() :
                           wxGetApp().GetAppOptions()->GetDifficultSentenceLength()),
      m_longSentencesOutliers(
          (project != nullptr) ?
              (project->GetLongSentenceMethod() == LongSentence::OutlierLength) :
              (wxGetApp().GetAppOptions()->GetLongSentenceMethod() == LongSentence::OutlierLength)),
      // batch project options
      m_minDocWordCountForBatch((project != nullptr) ?
                                    project->GetMinDocWordCountForBatch() :
                                    wxGetApp().GetAppOptions()->GetMinDocWordCountForBatch()),
      m_filePathTruncationMode(
          (project != nullptr) ?
              static_cast<int>(project->GetFilePathTruncationMode()) :
              static_cast<int>(wxGetApp().GetAppOptions()->GetFilePathTruncationMode())),
      // number syllabizing
      m_syllabicationMethod(
          (project != nullptr) ?
              static_cast<int>(project->GetNumeralSyllabicationMethod()) :
              static_cast<int>(wxGetApp().GetAppOptions()->GetNumeralSyllabicationMethod())),
      // paragraph parsing
      m_paragraphParsingMethod(
          (project != nullptr) ?
              static_cast<int>(project->GetParagraphsParsingMethod()) :
              static_cast<int>(wxGetApp().GetAppOptions()->GetParagraphsParsingMethod())),
      m_ignoreBlankLinesForParagraphsParser(
          (project != nullptr) ?
              project->IsIgnoringBlankLinesForParagraphsParser() :
              wxGetApp().GetAppOptions()->IsIgnoringBlankLinesForParagraphsParser()),
      m_ignoreIndentingForParagraphsParser(
          (project != nullptr) ?
              project->IsIgnoringIndentingForParagraphsParser() :
              wxGetApp().GetAppOptions()->IsIgnoringIndentingForParagraphsParser()),
      m_sentenceStartMustBeUppercased(
          (project != nullptr) ? project->GetSentenceStartMustBeUppercased() :
                                 wxGetApp().GetAppOptions()->GetSentenceStartMustBeUppercased()),
      m_aggressiveExclusion((project != nullptr) ?
                                project->IsExcludingAggressively() :
                                wxGetApp().GetAppOptions()->IsExcludingAggressively()),
      m_excludeTrailingCopyrightNoticeParagraphs(
          (project != nullptr) ?
              project->IsExcludingTrailingCopyrightNoticeParagraphs() :
              wxGetApp().GetAppOptions()->IsExcludingTrailingCopyrightNoticeParagraphs()),
      m_excludeTrailingCitations((project != nullptr) ?
                                     project->IsExcludingTrailingCitations() :
                                     wxGetApp().GetAppOptions()->IsExcludingTrailingCitations()),
      m_excludeFileAddresses((project != nullptr) ?
                                 project->IsExcludingFileAddresses() :
                                 wxGetApp().GetAppOptions()->IsExcludingFileAddresses()),
      m_excludeNumerals((project != nullptr) ? project->IsExcludingNumerals() :
                                               wxGetApp().GetAppOptions()->IsExcludingNumerals()),
      m_excludeProperNouns((project != nullptr) ?
                               project->IsExcludingProperNouns() :
                               wxGetApp().GetAppOptions()->IsExcludingProperNouns()),
      m_excludedPhrasesPath((project != nullptr) ?
                                project->GetExcludedPhrasesPath() :
                                wxGetApp().GetAppOptions()->GetExcludedPhrasesPath()),
      m_includeExcludedPhraseFirstOccurrence(
          (project != nullptr) ?
              project->IsIncludingExcludedPhraseFirstOccurrence() :
              wxGetApp().GetAppOptions()->IsIncludingExcludedPhraseFirstOccurrence()),
      m_exclusionBlockTags((project != nullptr) ?
                               project->GetExclusionBlockTags() :
                               wxGetApp().GetAppOptions()->GetExclusionBlockTags()),
      // header/list analysis
      m_includeIncompleteSentencesIfLongerThan(
          (project != nullptr) ?
              project->GetIncludeIncompleteSentencesIfLongerThanValue() :
              wxGetApp().GetAppOptions()->GetIncludeIncompleteSentencesIfLongerThanValue()),
      m_textExclusionMethod(
          (project != nullptr) ?
              static_cast<int>(project->GetInvalidSentenceMethod()) :
              static_cast<int>(wxGetApp().GetAppOptions()->GetInvalidSentenceMethod())),
      // title/font options
      m_xAxisFontColor((project != nullptr) ? project->GetXAxisFontColor() :
                                              wxGetApp().GetAppOptions()->GetXAxisFontColor()),
      m_xAxisFont((project != nullptr) ? project->GetXAxisFont() :
                                         wxGetApp().GetAppOptions()->GetXAxisFont()),
      m_yAxisFontColor((project != nullptr) ? project->GetYAxisFontColor() :
                                              wxGetApp().GetAppOptions()->GetYAxisFontColor()),
      m_yAxisFont((project != nullptr) ? project->GetYAxisFont() :
                                         wxGetApp().GetAppOptions()->GetYAxisFont()),
      m_topTitleFontColor((project != nullptr) ?
                              project->GetGraphTopTitleFontColor() :
                              wxGetApp().GetAppOptions()->GetGraphTopTitleFontColor()),
      m_topTitleFont((project != nullptr) ? project->GetGraphTopTitleFont() :
                                            wxGetApp().GetAppOptions()->GetGraphTopTitleFont()),
      m_bottomTitleFontColor((project != nullptr) ?
                                 project->GetGraphBottomTitleFontColor() :
                                 wxGetApp().GetAppOptions()->GetGraphBottomTitleFontColor()),
      m_bottomTitleFont((project != nullptr) ?
                            project->GetGraphBottomTitleFont() :
                            wxGetApp().GetAppOptions()->GetGraphBottomTitleFont()),
      m_leftTitleFontColor((project != nullptr) ?
                               project->GetGraphLeftTitleFontColor() :
                               wxGetApp().GetAppOptions()->GetGraphLeftTitleFontColor()),
      m_leftTitleFont((project != nullptr) ? project->GetGraphLeftTitleFont() :
                                             wxGetApp().GetAppOptions()->GetGraphLeftTitleFont()),
      m_rightTitleFontColor((project != nullptr) ?
                                project->GetGraphRightTitleFontColor() :
                                wxGetApp().GetAppOptions()->GetGraphRightTitleFontColor()),
      m_rightTitleFont((project != nullptr) ? project->GetGraphRightTitleFont() :
                                              wxGetApp().GetAppOptions()->GetGraphRightTitleFont())
    {
    wxString displayableProjectName =
        (m_readabilityProjectDoc != nullptr) ? m_readabilityProjectDoc->GetTitle() : wxString{};
    if (displayableProjectName.length() >= 50)
        {
        displayableProjectName.Truncate(49).Append(static_cast<wchar_t>(8230));
        }
    wxString title;
    if (IsGeneralSettings())
        {
        title = _(L"Options");
        }
    else
        {
        title = wxString::Format(_(L"\"%s\" Properties"), displayableProjectName);
        }
    Create(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
           wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    Bind(
        wxEVT_CHECKBOX,
        [this]([[maybe_unused]]
               wxCommandEvent& event)
        {
            TransferDataFromWindow();
            m_persistCookiesCheck->Enable(m_useJsCookies.get_value());
        },
        ID_JS_COOKIES_CHECKBOX);

    // Changing the list of files for a batch will need to disable the
    // linking & embedded options until this dialog is closed and
    // those new documents can be reloaded.
    Bind(
        wxEVT_LISTCTRLEX_EDITED,
        [this]([[maybe_unused]]
               wxCommandEvent& event)
        {
            if (m_docStorageRadioBox != nullptr && m_fileList != nullptr &&
                m_fileList->HasItemBeenEditedByUser())
                {
                m_docStorageRadioBox->Enable(false);
                }
        },
        ID_FILE_LIST);
    // Same for standard project with a single file path. Changing the file path will need to
    // load the new file before the user can try to embed it. They will need to accept the
    // changes in this dialog and then re-open it to embed the new document.
    Bind(
        wxEVT_TEXT,
        [this]([[maybe_unused]]
               wxCommandEvent& event)
        {
            TransferDataFromWindow();
            if (m_filePath.has_changed() && m_docStorageRadioBox != nullptr)
                {
                m_docStorageRadioBox->Enable(false);
                }
        },
        ID_DOCUMENT_PATH_FIELD);

    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnHighlightColorSelect, this, ID_HIGHLIGHT_COLOR_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnExcludedHighlightColorSelect, this,
         ID_EXCLUDED_HIGHLIGHT_COLOR_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnDupWordHighlightColorSelect, this,
         ID_DUP_WORD_COLOR_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnWordyPhraseHighlightColorSelect, this,
         ID_WORDY_PHRASE_COLOR_BUTTON);

    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnDolchNounHighlightColorSelect, this,
         ID_DOLCH_NOUN_COLOR_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnDolchHighlightColorSelect, this,
         ID_DOLCH_VERBS_COLOR_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnDolchHighlightColorSelect, this,
         ID_DOLCH_ADJECTIVES_COLOR_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnDolchAdverbsHighlightColorSelect, this,
         ID_DOLCH_ADVERBS_COLOR_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnDolchPronounsHighlightColorSelect, this,
         ID_DOLCH_PRONOUNS_COLOR_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnDolchPrepositionsHighlightColorSelect, this,
         ID_DOLCH_PREPOSITIONS_COLOR_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnDolchConjunctionsHighlightColorSelect, this,
         ID_DOLCH_CONJUNCTIONS_COLOR_BUTTON);

    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnDeleteFileClick, this, ID_DELETE_FILE_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnAddFileClick, this, ID_ADD_FILE_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnAddFilesClick, this, ID_ADD_FILES_BUTTON);

    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnExportSettings, this, ID_EXPORT_SETTINGS_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnImportSettings, this, ID_LOAD_SETTINGS_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnResetSettings, this, ID_RESET_SETTINGS_BUTTON);

    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnWarningMessagesButtonClick, this,
         ID_WARNING_MESSAGES_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnExcludedPhrasesFileEditButtonClick, this,
         ID_EXCLUDED_PHRASES_FILE_EDIT_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnAdditionalDocumentFileBrowseButtonClick, this,
         ID_ADDITIONAL_FILE_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnFileBrowseButtonClick, this, ID_FILE_BROWSE_BUTTON);

    Bind(wxEVT_CHECKBOX, &ToolsOptionsDlg::OnExcludeNumeralsCheck, this,
         ID_EXCLUDE_NUMERALS_CHECKBOX);
    Bind(wxEVT_CHOICE, &ToolsOptionsDlg::OnIncompleteSentencesChange, this, ID_TEXT_EXCLUDE_METHOD);
    Bind(wxEVT_RADIOBOX, &ToolsOptionsDlg::OnDocumentStorageRadioButtonClick, this,
         ID_DOCUMENT_STORAGE_RADIO_BOX);

    Bind(wxEVT_CHOICE, &ToolsOptionsDlg::OnParagraphParseChange, this, ID_PARAGRAPH_PARSE);
    Bind(wxEVT_CHOICE, &ToolsOptionsDlg::OnNumberSyllabizeChange, this, ID_NUMBER_SYLLABIZE_METHOD);
    Bind(wxEVT_CHOICE, &ToolsOptionsDlg::OnExclusionBlockTagChange, this,
         ID_EXCLUSION_TAG_BLOCK_SELECTION);

    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnOK, this, wxID_OK);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnHelp, this, wxID_HELP);
    Bind(wxEVT_HELP, &ToolsOptionsDlg::OnContextHelp, this);

    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnFontSelect, this, ID_FONT_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnFontSelect, this, ID_X_AXIS_FONT_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnFontSelect, this, ID_Y_AXIS_FONT_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnFontSelect, this, ID_GRAPH_TOP_TITLE_FONT_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnFontSelect, this, ID_GRAPH_BOTTOM_TITLE_FONT_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnFontSelect, this, ID_GRAPH_LEFT_TITLE_FONT_BUTTON);
    Bind(wxEVT_BUTTON, &ToolsOptionsDlg::OnFontSelect, this, ID_GRAPH_RIGHT_TITLE_FONT_BUTTON);
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::Create(wxWindow* parent, wxWindowID id, const wxString& caption,
                             const wxPoint& pos, const wxSize& size, long style)
    {
    SetExtraStyle(GetExtraStyle() | wxWS_EX_VALIDATE_RECURSIVELY);
    wxDialog::Create(parent, id, caption, pos, size, style);

    CreateControls();

    // DDX variables bound to controls
    TransferDataToWindow();

    // select the appropriate options if this is a project properties dialog
    if (GetSectionsBeingShown() == GraphsSection)
        {
        SelectPage(ToolsOptionsDlg::GRAPH_GENERAL_PAGE);
        }
    else if (GetSectionsBeingShown() == TextSection)
        {
        SelectPage(ToolsOptionsDlg::DOCUMENT_DISPLAY_GENERAL_PAGE);
        }
    else if (GetSectionsBeingShown() == ProjectSection)
        {
        SelectPage(ToolsOptionsDlg::PROJECT_SETTINGS_PAGE);
        }
    else if (GetSectionsBeingShown() == ScoresSection)
        {
        SelectPage(ToolsOptionsDlg::SCORES_TEST_OPTIONS_PAGE);
        }
    else if (GetSectionsBeingShown() == DocumentIndexing)
        {
        SelectPage(ToolsOptionsDlg::ANALYSIS_INDEXING_PAGE);
        }
    else if (GetSectionsBeingShown() == Grammar)
        {
        SelectPage(ToolsOptionsDlg::GRAMMAR_PAGE);
        }
    else if (GetSectionsBeingShown() == Statistics)
        {
        SelectPage(ToolsOptionsDlg::ANALYSIS_STATISTICS_PAGE);
        }
    else if (IsStandardProjectSettings())
        {
        const auto* view =
            dynamic_cast<const ProjectView*>(m_readabilityProjectDoc->GetFirstView());
        const auto selectedID = view->GetSideBar()->GetSelectedFolderId();
        if (view->GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
            {
            const auto graph = dynamic_cast<const Wisteria::Canvas*>(view->GetActiveProjectWindow())
                                   ->GetFixedObject(0, 0);
            if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::FleschChart)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::LixGauge)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::LixGaugeGerman)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::CrawfordGraph)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::DanielsonBryan2Plot)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::FraseGraph)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::FryGraph)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::RaygorGraph)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::SchwartzGraph)))
                {
                SelectPage(GRAPH_READABILITY_GRAPHS_PAGE);
                }
            else
                {
                SelectPage(GRAPH_GENERAL_PAGE);
                }
            }
        else if (!selectedID.has_value())
            {
            SelectPage(SCORES_DISPLAY_PAGE);
            }
        else if (selectedID == BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID)
            {
            SelectPage(SCORES_DISPLAY_PAGE);
            }
        else if (selectedID == BaseProjectView::SIDEBAR_STATS_SUMMARY_SECTION_ID)
            {
            SelectPage(ANALYSIS_INDEXING_PAGE);
            }
        else if (selectedID == BaseProjectView::SIDEBAR_GRAMMAR_SECTION_ID)
            {
            SelectPage(GRAMMAR_PAGE);
            }
        else if (selectedID == BaseProjectView::SIDEBAR_DOLCH_SECTION_ID)
            {
            SelectPage(DOCUMENT_DISPLAY_DOLCH_PAGE);
            }
        else if (selectedID == BaseProjectView::SIDEBAR_WORDS_BREAKDOWN_SECTION_ID)
            {
            SelectPage(WORDS_BREAKDOWN_PAGE);
            }
        else if (selectedID == BaseProjectView::SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID)
            {
            SelectPage(SENTENCES_BREAKDOWN_PAGE);
            }
        else
            {
            SelectPage(ANALYSIS_INDEXING_PAGE);
            }
        }
    else if (IsBatchProjectSettings())
        {
        const auto* view =
            dynamic_cast<const BatchProjectView*>(m_readabilityProjectDoc->GetFirstView());
        const auto selectedID = view->GetSideBar()->GetSelectedFolderId();
        if (view->GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
            {
            const auto graph = dynamic_cast<const Wisteria::Canvas*>(view->GetActiveProjectWindow())
                                   ->GetFixedObject(0, 0);
            if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::FleschChart)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::LixGauge)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::LixGaugeGerman)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::CrawfordGraph)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::DanielsonBryan2Plot)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::FraseGraph)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::FryGraph)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::RaygorGraph)) ||
                graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::SchwartzGraph)))
                {
                SelectPage(GRAPH_READABILITY_GRAPHS_PAGE);
                }
            else
                {
                SelectPage(GRAPH_GENERAL_PAGE);
                }
            }
        else if (!selectedID.has_value())
            {
            SelectPage(SCORES_DISPLAY_PAGE);
            }
        else if (selectedID == BatchProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID)
            {
            if (m_projectLanguage == static_cast<int>(readability::test_language::english_test))
                {
                SelectPage(SCORES_TEST_OPTIONS_PAGE);
                }
            else
                {
                SelectPage(SCORES_DISPLAY_PAGE);
                }
            }
        else if (selectedID == BatchProjectView::SIDEBAR_HISTOGRAMS_SECTION_ID)
            {
            SelectPage(GRAPH_HISTOGRAM_PAGE);
            }
        else if (selectedID == BatchProjectView::SIDEBAR_BOXPLOTS_SECTION_ID)
            {
            SelectPage(GRAPH_BOX_PLOT_PAGE);
            }
        else if (selectedID == BatchProjectView::SIDEBAR_GRAMMAR_SECTION_ID)
            {
            SelectPage(GRAMMAR_PAGE);
            }
        else
            {
            SelectPage(ANALYSIS_INDEXING_PAGE);
            }
        }

    Center();

    return true;
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveDocumentOptionsChanged() const
    {
    return m_realTimeUpdate.has_changed() || m_longSentencesNumberOfWords.has_changed() ||
           m_longSentencesOutliers.has_changed() || m_sentenceLength.has_changed() ||
           m_minDocWordCountForBatch.has_changed() || m_syllabicationMethod.has_changed() ||
           m_paragraphParsingMethod.has_changed() ||
           m_ignoreBlankLinesForParagraphsParser.has_changed() ||
           m_ignoreIndentingForParagraphsParser.has_changed() ||
           m_sentenceStartMustBeUppercased.has_changed() || m_aggressiveExclusion.has_changed() ||
           m_excludeTrailingCopyrightNoticeParagraphs.has_changed() ||
           m_excludeTrailingCitations.has_changed() || m_excludeFileAddresses.has_changed() ||
           m_excludeNumerals.has_changed() || m_excludeProperNouns.has_changed() ||
           m_excludedPhrasesPath.has_changed() ||
           // simple boolean flag set if user had edited this list from this dialog
           m_excludedPhrasesEdited || m_includeExcludedPhraseFirstOccurrence.has_changed() ||
           m_exclusionBlockTags.has_changed() || m_textExclusionMethod.has_changed() ||
           m_includeIncompleteSentencesIfLongerThan.has_changed() ||
           m_appendedDocumentFilePath.has_changed() ||
           // test-specific options
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetDCTextExclusionLabel()) &&
            m_readabilityTestsPropertyGrid->IsPropertyModified(GetDCTextExclusionLabel())) ||
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetProperNounsLabel()) &&
            m_readabilityTestsPropertyGrid->IsPropertyModified(GetProperNounsLabel())) ||
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetIncludeStockerLabel()) &&
            m_readabilityTestsPropertyGrid->IsPropertyModified(GetIncludeStockerLabel())) ||
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetHJCTextExclusionLabel()) &&
            m_readabilityTestsPropertyGrid->IsPropertyModified(GetHJCTextExclusionLabel())) ||
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid,
                                GetCountIndependentClausesLabel()) &&
            m_readabilityTestsPropertyGrid->IsPropertyModified(
                GetCountIndependentClausesLabel())) ||
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid,
                                GetFleschNumeralSyllabicationLabel()) &&
            m_readabilityTestsPropertyGrid->IsPropertyModified(
                GetFleschNumeralSyllabicationLabel())) ||
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid,
                                GetFleschKincaidNumeralSyllabicationLabel()) &&
            m_readabilityTestsPropertyGrid->IsPropertyModified(
                GetFleschKincaidNumeralSyllabicationLabel())) ||
           m_projectLanguage.has_changed() || m_documentStorageMethod.has_changed() ||
           m_filePath.has_changed() ||
           // grammar
           (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreProperNounsLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(GetIgnoreProperNounsLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreUppercasedWordsLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(GetIgnoreUppercasedWordsLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreNumeralsLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(GetIgnoreNumeralsLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreFileAddressesLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(GetIgnoreFileAddressesLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreProgrammerCodeLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(GetIgnoreProgrammerCodeLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetAllowColloquialismsLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(GetAllowColloquialismsLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreSocialMediaLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(GetIgnoreSocialMediaLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetGrammarHighlightedReportLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(GetGrammarHighlightedReportLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetMisspellingsLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(BaseProjectView::GetMisspellingsLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetRepeatedWordsLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(BaseProjectView::GetRepeatedWordsLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetArticleMismatchesLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(
                BaseProjectView::GetArticleMismatchesLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetPhrasingErrorsTabLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(
                BaseProjectView::GetPhrasingErrorsTabLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetRedundantPhrasesTabLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(
                BaseProjectView::GetRedundantPhrasesTabLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetOverusedWordsBySentenceLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(
                BaseProjectView::GetOverusedWordsBySentenceLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetWordyPhrasesTabLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(
                BaseProjectView::GetWordyPhrasesTabLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetClichesTabLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(BaseProjectView::GetClichesTabLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetPassiveLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(BaseProjectView::GetPassiveLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(
                BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetSentenceStartingWithLowercaseTabLabel()) &&
            m_grammarPropertyGrid->IsPropertyModified(
                BaseProjectView::GetSentenceStartingWithLowercaseTabLabel())) ||
           ((m_fileList != nullptr) && m_fileList->HasItemBeenEditedByUser());
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveStatisticsOptionsChanged() const
    {
    return (IsPropertyAvailable(m_statisticsPropertyGrid, GetParagraphsLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetParagraphsLabel())) ||
           (IsPropertyAvailable(m_statisticsPropertyGrid, GetSentencesLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetSentencesLabel())) ||
           (IsPropertyAvailable(m_statisticsPropertyGrid, GetWordsLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetWordsLabel())) ||
           (IsPropertyAvailable(m_statisticsPropertyGrid, GetExtendedWordsLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetExtendedWordsLabel())) ||
           (IsPropertyAvailable(m_statisticsPropertyGrid, GetGrammarLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetGrammarLabel())) ||
           (IsPropertyAvailable(m_statisticsPropertyGrid, GetNotesLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetNotesLabel())) ||
           (IsPropertyAvailable(m_statisticsPropertyGrid, GetExtendedInformationLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetExtendedInformationLabel())) ||
           (IsPropertyAvailable(m_statisticsPropertyGrid, GetCoverageLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetCoverageLabel())) ||
           (IsPropertyAvailable(m_statisticsPropertyGrid, GetDolchWordsLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetDolchWordsLabel())) ||
           (IsPropertyAvailable(m_statisticsPropertyGrid, GetDolchExplanationLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetDolchExplanationLabel())) ||
           (IsPropertyAvailable(m_statisticsPropertyGrid,
                                BaseProjectView::GetFormattedReportLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(
                BaseProjectView::GetFormattedReportLabel())) ||
           (IsPropertyAvailable(m_statisticsPropertyGrid,
                                BaseProjectView::GetTabularReportLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(BaseProjectView::GetTabularReportLabel()));
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveGraphOptionsChanged() const
    {
    return (IsPropertyAvailable(m_generalGraphPropertyGrid, GetDisplayDropShadowsLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetDisplayDropShadowsLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetShowcaseKeyItemsLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetShowcaseKeyItemsLabel())) ||
           // box plot options
           (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetOpacityLabel()) &&
            m_boxPlotsPropertyGrid->IsPropertyModified(GetOpacityLabel())) ||
           (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetLabelsOnBoxesLabel()) &&
            m_boxPlotsPropertyGrid->IsPropertyModified(GetLabelsOnBoxesLabel())) ||
           (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetShowAllDataPointsLabel()) &&
            m_boxPlotsPropertyGrid->IsPropertyModified(GetShowAllDataPointsLabel())) ||
           (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetConnectBoxesLabel()) &&
            m_boxPlotsPropertyGrid->IsPropertyModified(GetConnectBoxesLabel())) ||
           (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetColorLabel()) &&
            m_boxPlotsPropertyGrid->IsPropertyModified(GetColorLabel())) ||
           (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetEffectLabel()) &&
            m_boxPlotsPropertyGrid->IsPropertyModified(GetEffectLabel())) ||
           // general
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetGraphColorSchemeLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetGraphColorSchemeLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetLogoImageLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetLogoImageLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetBackgroundColorFadeLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetBackgroundColorFadeLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetStippleImageLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetStippleImageLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetStippleShapeLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetStippleShapeLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetStippleShapeColorLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetStippleShapeColorLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetCommonImageLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetCommonImageLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetBackgroundColorLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetBackgroundColorLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageOpacityLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetImageOpacityLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetColorLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetColorLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetColorOpacityLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetColorOpacityLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetImageLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageEffectLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetImageEffectLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageFitLabel()) &&
            m_generalGraphPropertyGrid->IsPropertyModified(GetImageFitLabel())) ||
           // barchart options
           (IsPropertyAvailable(m_barChartPropertyGrid, GetOpacityLabel()) &&
            m_barChartPropertyGrid->IsPropertyModified(GetOpacityLabel())) ||
           (IsPropertyAvailable(m_barChartPropertyGrid, GetColorLabel()) &&
            m_barChartPropertyGrid->IsPropertyModified(GetColorLabel())) ||
           (IsPropertyAvailable(m_barChartPropertyGrid, GeOrientationLabel()) &&
            m_barChartPropertyGrid->IsPropertyModified(GeOrientationLabel())) ||
           (IsPropertyAvailable(m_barChartPropertyGrid, GetEffectLabel()) &&
            m_barChartPropertyGrid->IsPropertyModified(GetEffectLabel())) ||
           (IsPropertyAvailable(m_barChartPropertyGrid, GetLabelsOnBarsLabel()) &&
            m_barChartPropertyGrid->IsPropertyModified(GetLabelsOnBarsLabel())) ||
           // histogram options
           (IsPropertyAvailable(m_histogramPropertyGrid, GetBinSortingLabel()) &&
            m_histogramPropertyGrid->IsPropertyModified(GetBinSortingLabel())) ||
           (IsPropertyAvailable(m_histogramPropertyGrid, GetGradeLevelRoundingLabel()) &&
            m_histogramPropertyGrid->IsPropertyModified(GetGradeLevelRoundingLabel())) ||
           (IsPropertyAvailable(m_histogramPropertyGrid, GetIntervalDisplayLabel()) &&
            m_histogramPropertyGrid->IsPropertyModified(GetIntervalDisplayLabel())) ||
           (IsPropertyAvailable(m_histogramPropertyGrid, GetBinLabelsLabel()) &&
            m_histogramPropertyGrid->IsPropertyModified(GetBinLabelsLabel())) ||
           (IsPropertyAvailable(m_histogramPropertyGrid, GetColorLabel()) &&
            m_histogramPropertyGrid->IsPropertyModified(GetColorLabel())) ||
           (IsPropertyAvailable(m_histogramPropertyGrid, GetOpacityLabel()) &&
            m_histogramPropertyGrid->IsPropertyModified(GetOpacityLabel())) ||
           (IsPropertyAvailable(m_histogramPropertyGrid, GetEffectLabel()) &&
            m_histogramPropertyGrid->IsPropertyModified(GetEffectLabel())) ||
           // title/font options
           m_xAxisFontColor.has_changed() || m_xAxisFont.has_changed() ||
           m_yAxisFontColor.has_changed() || m_yAxisFont.has_changed() ||
           m_topTitleFontColor.has_changed() || m_topTitleFont.has_changed() ||
           m_bottomTitleFontColor.has_changed() || m_bottomTitleFont.has_changed() ||
           m_leftTitleFontColor.has_changed() || m_leftTitleFont.has_changed() ||
           m_rightTitleFontColor.has_changed() || m_rightTitleFont.has_changed() ||
           (IsPropertyAvailable(m_readabilityGraphPropertyGrid, GetInvalidRegionsColorLabel()) &&
            m_readabilityGraphPropertyGrid->IsPropertyModified(GetInvalidRegionsColorLabel())) ||
           (IsPropertyAvailable(m_readabilityGraphPropertyGrid, GetRaygorStyleLabel()) &&
            m_readabilityGraphPropertyGrid->IsPropertyModified(GetRaygorStyleLabel())) ||
           (IsPropertyAvailable(m_readabilityGraphPropertyGrid,
                                GetUseEnglishLabelsForGermanLixLabel()) &&
            m_readabilityGraphPropertyGrid->IsPropertyModified(
                GetUseEnglishLabelsForGermanLixLabel())) ||
           (IsPropertyAvailable(m_readabilityGraphPropertyGrid,
                                GetFleschChartConnectPointsLabel()) &&
            m_readabilityGraphPropertyGrid->IsPropertyModified(
                GetFleschChartConnectPointsLabel())) ||
           (IsPropertyAvailable(m_readabilityGraphPropertyGrid,
                                GetFleschSyllableRulerDocGroupsLabel()) &&
            m_readabilityGraphPropertyGrid->IsPropertyModified(
                GetFleschSyllableRulerDocGroupsLabel()));
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveTextViewOptionsChanged() const noexcept
    {
    return m_textHighlightMethod.has_changed() || m_highlightedColor.has_changed() ||
           m_excludedTextHighlightColor.has_changed() ||
           m_duplicateWordHighlightColor.has_changed() ||
           m_wordyPhraseHighlightColor.has_changed() || m_font.has_changed() ||
           m_fontColor.has_changed() || m_dolchConjunctionsColor.has_changed() ||
           m_dolchPrepositionsColor.has_changed() || m_dolchPronounsColor.has_changed() ||
           m_dolchAdverbsColor.has_changed() || m_dolchAdjectivesColor.has_changed() ||
           m_dolchVerbsColor.has_changed() || m_dolchNounsColor.has_changed() ||
           m_highlightDolchConjunctions.has_changed() ||
           m_highlightDolchPrepositions.has_changed() || m_highlightDolchPronouns.has_changed() ||
           m_highlightDolchAdverbs.has_changed() || m_highlightDolchAdjectives.has_changed() ||
           m_highlightDolchVerbs.has_changed() || m_highlightDolchNouns.has_changed();
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveTestDisplayOptionsChanged() const
    {
    return (IsPropertyAvailable(m_gradeLevelPropertyGrid, GetCalculationLabel()) &&
            m_gradeLevelPropertyGrid->IsPropertyModified(GetCalculationLabel())) ||
           (IsPropertyAvailable(m_gradeLevelPropertyGrid, GetGradeScaleLabel()) &&
            m_gradeLevelPropertyGrid->IsPropertyModified(GetGradeScaleLabel())) ||
           (IsPropertyAvailable(m_gradeLevelPropertyGrid, GetGradesLongFormatLabel()) &&
            m_gradeLevelPropertyGrid->IsPropertyModified(GetGradesLongFormatLabel())) ||
           (IsPropertyAvailable(m_gradeLevelPropertyGrid, GetIncludeScoreSummaryLabel()) &&
            m_gradeLevelPropertyGrid->IsPropertyModified(GetIncludeScoreSummaryLabel()));
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveWordsBreakdownOptionsChanged() const
    {
    return (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetWordCountsLabel()) &&
            m_wordsBreakdownPropertyGrid->IsPropertyModified(
                BaseProjectView::GetWordCountsLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetSyllableCountsLabel()) &&
            m_wordsBreakdownPropertyGrid->IsPropertyModified(
                BaseProjectView::GetSyllableCountsLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetThreeSyllableWordsLabel()) &&
            m_wordsBreakdownPropertyGrid->IsPropertyModified(
                BaseProjectView::GetThreeSyllableWordsLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetSixCharWordsLabel()) &&
            m_wordsBreakdownPropertyGrid->IsPropertyModified(
                BaseProjectView::GetSixCharWordsLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetWordCloudLabel()) &&
            m_wordsBreakdownPropertyGrid->IsPropertyModified(
                BaseProjectView::GetWordCloudLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetDaleChallLabel()) &&
            m_wordsBreakdownPropertyGrid->IsPropertyModified(
                BaseProjectView::GetDaleChallLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetSpacheLabel()) &&
            m_wordsBreakdownPropertyGrid->IsPropertyModified(BaseProjectView::GetSpacheLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetHarrisJacobsonLabel()) &&
            m_wordsBreakdownPropertyGrid->IsPropertyModified(
                BaseProjectView::GetHarrisJacobsonLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, GetCustomTestsLabel()) &&
            m_wordsBreakdownPropertyGrid->IsPropertyModified(GetCustomTestsLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetAllWordsLabel()) &&
            m_wordsBreakdownPropertyGrid->IsPropertyModified(
                BaseProjectView::GetAllWordsLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetKeyWordsLabel()) &&
            m_wordsBreakdownPropertyGrid->IsPropertyModified(BaseProjectView::GetKeyWordsLabel()));
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveSentencesBreakdownOptionsChanged() const
    {
    return (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
                                BaseProjectView::GetLongSentencesLabel()) &&
            m_sentencesBreakdownPropertyGrid->IsPropertyModified(
                BaseProjectView::GetLongSentencesLabel())) ||
           (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
                                BaseProjectView::GetSentenceLengthBoxPlotLabel()) &&
            m_sentencesBreakdownPropertyGrid->IsPropertyModified(
                BaseProjectView::GetSentenceLengthBoxPlotLabel())) ||
           (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
                                BaseProjectView::GetSentenceLengthHistogramLabel()) &&
            m_sentencesBreakdownPropertyGrid->IsPropertyModified(
                BaseProjectView::GetSentenceLengthHistogramLabel())) ||
           (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
                                BaseProjectView::GetSentenceLengthHeatmapLabel()) &&
            m_sentencesBreakdownPropertyGrid->IsPropertyModified(
                BaseProjectView::GetSentenceLengthHeatmapLabel()));
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveOptionsChanged() const
    {
    return // text formatting options
        HaveTextViewOptionsChanged() || HaveTestDisplayOptionsChanged() ||
        HaveWordsBreakdownOptionsChanged() || HaveSentencesBreakdownOptionsChanged() ||
        // file list display
        m_filePathTruncationMode.has_changed() || m_reviewer.has_changed() ||
        m_status.has_changed() || m_description.has_changed() || HaveDocumentOptionsChanged() ||
        HaveStatisticsOptionsChanged() || HaveGraphOptionsChanged();
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::ValidateOptions()
    {
    Validate();
    TransferDataFromWindow();
    // trim any spaces off of the file paths
    m_filePath = ExpandPath(m_filePath.get_value().Trim(true).Trim(false));
    m_excludedPhrasesPath = ExpandPath(m_excludedPhrasesPath.get_value().Trim(true).Trim(false));
    m_appendedDocumentFilePath =
        ExpandPath(m_appendedDocumentFilePath.get_value().Trim(true).Trim(false));
    TransferDataToWindow();
    if (IsBatchProjectSettings() && (m_fileList != nullptr) && m_fileList->GetItemCount() == 0)
        {
        wxMessageBox(_(L"Project must contain at least one document."), wxGetApp().GetAppName(),
                     wxOK | wxICON_EXCLAMATION);
        SelectPage(PROJECT_SETTINGS_PAGE);
        return false;
        }
    // if linking to external file then make sure it is valid and set flag to reload it
    if (m_filePath.has_changed() ||
        (m_documentStorageMethod.has_changed() &&
         m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText) &&
         IsStandardProjectSettings()))
        {
        const FilePathResolver resolvePath(m_filePath.get_value(), true);
        if (resolvePath.IsInvalidFile() ||
            (resolvePath.IsLocalOrNetworkFile() && !wxFile::Exists(m_filePath.get_value())))
            {
            wxMessageBox(wxString::Format(_(L"\"%s\": file not found."), m_filePath.get_value()),
                         wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            SelectPage(PROJECT_SETTINGS_PAGE);
            return false;
            }
        }
    if (m_appendedDocumentFilePath.has_changed() && !m_appendedDocumentFilePath.get_value().empty())
        {
        if (!wxFile::Exists(m_appendedDocumentFilePath.get_value()))
            {
            wxMessageBox(wxString::Format(_(L"\"%s\": file not found."),
                                          m_appendedDocumentFilePath.get_value()),
                         wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            SelectPage(PROJECT_SETTINGS_PAGE);
            return false;
            }
        }
    if (((IsPropertyAvailable(m_generalGraphPropertyGrid, GetStippleImageLabel()) &&
          !m_generalGraphPropertyGrid->GetPropertyValueAsString(GetStippleImageLabel()).empty()) ||
         (IsPropertyAvailable(m_barChartPropertyGrid, GetEffectLabel()) &&
          static_cast<Wisteria::BoxEffect>(m_barChartPropertyGrid->GetPropertyValueAsInt(
              GetEffectLabel())) == Wisteria::BoxEffect::StippleImage) ||
         (IsPropertyAvailable(m_histogramPropertyGrid, GetEffectLabel()) &&
          static_cast<Wisteria::BoxEffect>(m_histogramPropertyGrid->GetPropertyValueAsInt(
              GetEffectLabel())) == Wisteria::BoxEffect::StippleImage) ||
         (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetEffectLabel()) &&
          static_cast<Wisteria::BoxEffect>(m_boxPlotsPropertyGrid->GetPropertyValueAsInt(
              GetEffectLabel())) == Wisteria::BoxEffect::StippleImage)) &&
        !wxFile::Exists(
            m_generalGraphPropertyGrid->GetPropertyValueAsString(GetStippleImageLabel())))
        {
        wxMessageBox(wxString::Format(_(L"\"%s\": stipple image file not found."),
                                      m_generalGraphPropertyGrid->GetPropertyValueAsString(
                                          GetStippleImageLabel())),
                     wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
        wxFileDialog fd(this, _(L"Select Stipple Image"), wxString{}, wxString{},
                        Wisteria::GraphItems::Image::GetImageFileFilter(),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
        if (fd.ShowModal() != wxID_OK)
            {
            return false;
            }
        m_generalGraphPropertyGrid->ChangePropertyValue(GetStippleImageLabel(), fd.GetPath());
        }
    if (((IsPropertyAvailable(m_generalGraphPropertyGrid, GetCommonImageLabel()) &&
          !m_generalGraphPropertyGrid->GetPropertyValueAsString(GetCommonImageLabel()).empty()) ||
         (IsPropertyAvailable(m_barChartPropertyGrid, GetEffectLabel()) &&
          static_cast<Wisteria::BoxEffect>(m_barChartPropertyGrid->GetPropertyValueAsInt(
              GetEffectLabel())) == Wisteria::BoxEffect::CommonImage) ||
         (IsPropertyAvailable(m_histogramPropertyGrid, GetEffectLabel()) &&
          static_cast<Wisteria::BoxEffect>(m_histogramPropertyGrid->GetPropertyValueAsInt(
              GetEffectLabel())) == Wisteria::BoxEffect::CommonImage) ||
         (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetEffectLabel()) &&
          static_cast<Wisteria::BoxEffect>(m_boxPlotsPropertyGrid->GetPropertyValueAsInt(
              GetEffectLabel())) == Wisteria::BoxEffect::CommonImage)) &&
        !wxFile::Exists(
            m_generalGraphPropertyGrid->GetPropertyValueAsString(GetCommonImageLabel())))
        {
        wxMessageBox(wxString::Format(_(L"\"%s\": common image file not found."),
                                      m_generalGraphPropertyGrid->GetPropertyValueAsString(
                                          GetCommonImageLabel())),
                     wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
        wxFileDialog fd(this, _(L"Select Common Image"), wxString{}, wxString{},
                        Wisteria::GraphItems::Image::GetImageFileFilter(),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
        if (fd.ShowModal() != wxID_OK)
            {
            return false;
            }
        m_generalGraphPropertyGrid->ChangePropertyValue(GetCommonImageLabel(), fd.GetPath());
        }
    if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageLabel()) &&
        !m_generalGraphPropertyGrid->GetPropertyValueAsString(GetImageLabel()).empty() &&
        !wxFile::Exists(m_generalGraphPropertyGrid->GetPropertyValueAsString(GetImageLabel())))
        {
        wxMessageBox(
            wxString::Format(_(L"\"%s\": graph background image file not found."),
                             m_generalGraphPropertyGrid->GetPropertyValueAsString(GetImageLabel())),
            wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
        wxFileDialog fd(this, _(L"Select Background Image"), wxString{}, wxString{},
                        Wisteria::GraphItems::Image::GetImageFileFilter(),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
        if (fd.ShowModal() != wxID_OK)
            {
            return false;
            }
        m_generalGraphPropertyGrid->ChangePropertyValue(GetImageLabel(), fd.GetPath());
        }
    if ((IsPropertyAvailable(m_generalGraphPropertyGrid, GetLogoImageLabel()) &&
         !m_generalGraphPropertyGrid->GetPropertyValueAsString(GetLogoImageLabel()).empty()) &&
        !wxFile::Exists(m_generalGraphPropertyGrid->GetPropertyValueAsString(GetLogoImageLabel())))
        {
        wxMessageBox(wxString::Format(
                         _(L"\"%s\": graph logo image file not found."),
                         m_generalGraphPropertyGrid->GetPropertyValueAsString(GetLogoImageLabel())),
                     wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
        wxFileDialog fd(this, _(L"Select Logo Image"), wxString{}, wxString{},
                        Wisteria::GraphItems::Image::GetImageFileFilter(),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
        if (fd.ShowModal() != wxID_OK)
            {
            return false;
            }
        m_generalGraphPropertyGrid->ChangePropertyValue(GetLogoImageLabel(), fd.GetPath());
        }
    if (!IsStandardProjectSettings() &&
        IsPropertyAvailable(m_histogramPropertyGrid, GetBinSortingLabel()) &&
        IsPropertyAvailable(m_histogramPropertyGrid, GetIntervalDisplayLabel()) &&
        m_histogramPropertyGrid->GetPropertyValueAsInt(GetBinSortingLabel()) == 0 &&
        m_histogramPropertyGrid->GetPropertyValueAsInt(GetIntervalDisplayLabel()) != 1)
        {
        auto warningIter =
            WarningManager::GetWarning(_DT(L"histogram-unique-values-midpoints-required"));
        if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
            {
            wxMessageBox(warningIter->GetMessage(), wxGetApp().GetAppName(),
                         warningIter->GetFlags(), this);
            }
        m_histogramPropertyGrid->ChangePropertyValue(GetIntervalDisplayLabel(), 1);
        }
    if (!m_excludedPhrasesPath.get_value().empty() &&
        !wxFile::Exists(m_excludedPhrasesPath.get_value()))
        {
        wxMessageBox(wxString::Format(_(L"\"%s\": excluded phrase file not found."),
                                      m_excludedPhrasesPath.get_value()),
                     wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
        SelectPage(ANALYSIS_INDEXING_PAGE);
        return false;
        }
    return true;
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::SaveOptions()
    {
    if (GetSectionsBeingShown() == GraphsSection)
        {
        SaveProjectGraphOptions();
        return;
        }
    if (GetSectionsBeingShown() == TextSection)
        {
        SaveTextWindowOptions();
        return;
        }
    if (GetSectionsBeingShown() == Statistics)
        {
        SaveStatisticsOptions();
        return;
        }
    if (m_uiLanguage.has_changed())
        {
        wxGetApp().GetAppOptions()->SetUiLanguage(
            static_cast<UiLanguage>(m_uiLanguage.get_value()));
        }
    if (m_disableGpuAcceleration.has_changed())
        {
        wxGetApp().GetAppOptions()->DisableGpuAcceleration(m_disableGpuAcceleration.get_value());
        }
    if (m_reportTheme.has_changed())
        {
        wxGetApp().GetAppOptions()->SetReportTheme(ThemeLabelToFileName(m_reportTheme.get_value()));
        }
    if (m_logVerbose.has_changed() && wxGetApp().GetLogFile() != nullptr)
        {
        wxLog::SetVerbose(m_logVerbose.get_value());
        }
    if (m_logAppendDailyLog.has_changed())
        {
        wxGetApp().GetAppOptions()->AppendDailyLog(m_logAppendDailyLog.get_value());
        }
    if (m_showDeveloperTab.has_changed())
        {
        wxGetApp().GetAppOptions()->ShowDeveloperTab(m_showDeveloperTab.get_value());
        auto* mainFrame = wxGetApp().GetMainFrameEx();
        if (mainFrame != nullptr && mainFrame->GetRibbon() != nullptr &&
            mainFrame->GetDeveloperRibbonPage() != nullptr)
            {
            if (!m_showDeveloperTab.get_value() && mainFrame->IsDeveloperTabActive())
                {
                mainFrame->GetRibbon()->SetActivePage(mainFrame->GetHomeRibbonPage());
                }
            mainFrame->GetRibbon()->ShowPage(
                mainFrame->GetRibbon()->GetPageNumber(mainFrame->GetDeveloperRibbonPage()),
                m_showDeveloperTab.get_value());
            mainFrame->GetRibbon()->Realize();
            }
        // enable/disable the Lua Script button on every open project's Tools ribbon
        const auto& docs = wxGetApp().GetDocManager()->GetDocuments();
        for (size_t i = 0; i < docs.GetCount(); ++i)
            {
            const auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
            if (doc != nullptr)
                {
                auto* view = dynamic_cast<BaseProjectView*>(doc->GetFirstView());
                if (view != nullptr && view->GetRibbon() != nullptr)
                    {
                    auto* bar = dynamic_cast<wxRibbonButtonBar*>(
                        view->GetRibbon()->FindWindow(BaseProjectView::RIBBON_TOOLS_BUTTON_BAR_ID));
                    if (bar != nullptr)
                        {
                        bar->EnableButton(XRCID("ID_SCRIPT_WINDOW"),
                                          m_showDeveloperTab.get_value());
                        bar->Realize();
                        }
                    }
                }
            }
        }
    if (m_showLogTab.has_changed())
        {
        wxGetApp().GetAppOptions()->ShowLogTab(m_showLogTab.get_value());
        if (wxGetApp().GetMainFrameEx() != nullptr &&
            wxGetApp().GetMainFrameEx()->GetRibbon() != nullptr &&
            wxGetApp().GetMainFrameEx()->GetLogRibbonPage() != nullptr)
            {
            if (!m_showLogTab.get_value() && wxGetApp().GetMainFrameEx()->IsLogTabActive())
                {
                wxGetApp().GetMainFrameEx()->GetRibbon()->SetActivePage(
                    wxGetApp().GetMainFrameEx()->GetHomeRibbonPage());
                }
            wxGetApp().GetMainFrameEx()->GetRibbon()->ShowPage(
                wxGetApp().GetMainFrameEx()->GetRibbon()->GetPageNumber(
                    wxGetApp().GetMainFrameEx()->GetLogRibbonPage()),
                m_showLogTab.get_value());
            wxGetApp().GetMainFrameEx()->GetRibbon()->Realize();
            }
        }
    if (m_logAutoRefresh.has_changed())
        {
        wxGetApp().GetAppOptions()->SetLogAutoRefresh(m_logAutoRefresh.get_value());
        if (wxGetApp().GetMainFrameEx() != nullptr)
            {
            wxGetApp().GetMainFrameEx()->SetLogAutoRefresh(m_logAutoRefresh.get_value());
            }
        }
    if (m_luaUnsafeMode.has_changed())
        {
        wxGetApp().GetAppOptions()->EnableLuaUnsafeMode(m_luaUnsafeMode.get_value());
        }
    if (m_userAgent.has_changed())
        {
        wxGetApp().GetAppOptions()->SetUserAgent(m_userAgent.get_value());
        wxGetApp().GetWebHarvester().SetUserAgent(m_userAgent.get_value());
        }
    if (m_disablePeerVerify.has_changed())
        {
        wxGetApp().GetAppOptions()->DisablePeerVerify(m_disablePeerVerify.get_value());
        wxGetApp().GetWebHarvester().DisablePeerVerify(m_disablePeerVerify.get_value());
        }
    if (m_useJsCookies.has_changed())
        {
        wxGetApp().GetAppOptions()->UseJavaScriptCookies(m_useJsCookies.get_value());
        wxGetApp().GetWebHarvester().UseJavaScriptCookies(m_useJsCookies.get_value());
        }
    if (m_persistJsCookies.has_changed())
        {
        wxGetApp().GetAppOptions()->PersistJavaScriptCookies(m_persistJsCookies.get_value());
        wxGetApp().GetWebHarvester().PersistJavaScriptCookies(m_persistJsCookies.get_value());
        }

    if ((m_readabilityProjectDoc != nullptr) && HaveOptionsChanged())
        {
        SaveProjectOptions();
        }
    else if ((m_readabilityProjectDoc == nullptr) && HaveOptionsChanged())
        {
        SaveGlobalOptions();
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::SaveGlobalOptions()
    {
    if ((m_readabilityProjectDoc == nullptr) && HaveOptionsChanged())
        {
        wxGetApp().GetAppOptions()->SetTextHighlightMethod(
            static_cast<TextHighlight>(m_textHighlightMethod.get_value()));
        wxGetApp().GetAppOptions()->SetTextHighlightColor(m_highlightedColor.get_value());
        wxGetApp().GetAppOptions()->SetExcludedTextHighlightColor(
            m_excludedTextHighlightColor.get_value());
        wxGetApp().GetAppOptions()->SetDuplicateWordHighlightColor(
            m_duplicateWordHighlightColor.get_value());
        wxGetApp().GetAppOptions()->SetWordyPhraseHighlightColor(
            m_wordyPhraseHighlightColor.get_value());
        wxGetApp().GetAppOptions()->SetTextViewFont(m_font.get_value());
        wxGetApp().GetAppOptions()->SetTextFontColor(m_fontColor.get_value());
        wxGetApp().GetAppOptions()->SetDolchConjunctionsColor(m_dolchConjunctionsColor.get_value());
        wxGetApp().GetAppOptions()->SetDolchPrepositionsColor(m_dolchPrepositionsColor.get_value());
        wxGetApp().GetAppOptions()->SetDolchPronounsColor(m_dolchPronounsColor.get_value());
        wxGetApp().GetAppOptions()->SetDolchAdverbsColor(m_dolchAdverbsColor.get_value());
        wxGetApp().GetAppOptions()->SetDolchAdjectivesColor(m_dolchAdjectivesColor.get_value());
        wxGetApp().GetAppOptions()->SetDolchVerbsColor(m_dolchVerbsColor.get_value());
        wxGetApp().GetAppOptions()->SetDolchNounsColor(m_dolchNounsColor.get_value());
        wxGetApp().GetAppOptions()->HighlightDolchConjunctions(
            m_highlightDolchConjunctions.get_value());
        wxGetApp().GetAppOptions()->HighlightDolchPrepositions(
            m_highlightDolchPrepositions.get_value());
        wxGetApp().GetAppOptions()->HighlightDolchPronouns(m_highlightDolchPronouns.get_value());
        wxGetApp().GetAppOptions()->HighlightDolchAdverbs(m_highlightDolchAdverbs.get_value());
        wxGetApp().GetAppOptions()->HighlightDolchAdjectives(
            m_highlightDolchAdjectives.get_value());
        wxGetApp().GetAppOptions()->HighlightDolchVerbs(m_highlightDolchVerbs.get_value());
        wxGetApp().GetAppOptions()->HighlightDolchNouns(m_highlightDolchNouns.get_value());
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid, GetCalculationLabel()))
            {
            wxGetApp().GetAppOptions()->GetReadabilityMessageCatalog().SetReadingAgeDisplay(
                static_cast<ReadabilityMessages::ReadingAgeDisplay>(
                    m_gradeLevelPropertyGrid->GetPropertyValueAsInt(GetCalculationLabel())));
            }
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid, GetGradeScaleLabel()))
            {
            wxGetApp().GetAppOptions()->GetReadabilityMessageCatalog().SetGradeScale(
                static_cast<readability::grade_scale>(
                    m_gradeLevelPropertyGrid->GetPropertyValueAsInt(GetGradeScaleLabel())));
            }
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid, GetGradesLongFormatLabel()))
            {
            wxGetApp().GetAppOptions()->GetReadabilityMessageCatalog().SetLongGradeScaleFormat(
                m_gradeLevelPropertyGrid->GetPropertyValueAsBool(GetGradesLongFormatLabel()));
            }
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid, GetIncludeScoreSummaryLabel()))
            {
            wxGetApp().GetAppOptions()->IncludeScoreSummaryReport(
                m_gradeLevelPropertyGrid->GetPropertyValueAsBool(GetIncludeScoreSummaryLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetParagraphsLabel()))
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableParagraph(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetParagraphsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetSentencesLabel()))
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableSentences(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetSentencesLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetWordsLabel()))
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableWords(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetWordsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetExtendedWordsLabel()))
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableExtendedWords(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetExtendedWordsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetGrammarLabel()))
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableGrammar(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetGrammarLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetNotesLabel()))
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableNotes(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetNotesLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetExtendedInformationLabel()))
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableExtendedInformation(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetExtendedInformationLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetCoverageLabel()))
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableDolchCoverage(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetCoverageLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetDolchWordsLabel()))
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableDolchWords(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetDolchWordsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetDolchExplanationLabel()))
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableDolchExplanation(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetDolchExplanationLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,
                                BaseProjectView::GetFormattedReportLabel()))
            {
            wxGetApp().GetAppOptions()->GetStatisticsInfo().EnableReport(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetFormattedReportLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, BaseProjectView::GetTabularReportLabel()))
            {
            wxGetApp().GetAppOptions()->GetStatisticsInfo().EnableTable(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetTabularReportLabel()));
            }
        wxGetApp().GetAppOptions()->SetLongSentenceMethod(
            m_longSentencesNumberOfWords ? LongSentence::LongerThanSpecifiedLength :
                                           LongSentence::OutlierLength);
        wxGetApp().GetAppOptions()->SetDifficultSentenceLength(m_sentenceLength.get_value());
        wxGetApp().GetAppOptions()->SetMinDocWordCountForBatch(
            m_minDocWordCountForBatch.get_value());
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreProperNounsLabel()))
            {
            wxGetApp().GetAppOptions()->SpellCheckIgnoreProperNouns(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreProperNounsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreUppercasedWordsLabel()))
            {
            wxGetApp().GetAppOptions()->SpellCheckIgnoreUppercased(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreUppercasedWordsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreNumeralsLabel()))
            {
            wxGetApp().GetAppOptions()->SpellCheckIgnoreNumerals(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreNumeralsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreFileAddressesLabel()))
            {
            wxGetApp().GetAppOptions()->SpellCheckIgnoreFileAddresses(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreFileAddressesLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreProgrammerCodeLabel()))
            {
            wxGetApp().GetAppOptions()->SpellCheckIgnoreProgrammerCode(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreProgrammerCodeLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetAllowColloquialismsLabel()))
            {
            wxGetApp().GetAppOptions()->SpellCheckAllowColloquialisms(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetAllowColloquialismsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreSocialMediaLabel()))
            {
            wxGetApp().GetAppOptions()->SpellCheckIgnoreSocialMediaTags(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreSocialMediaLabel()));
            }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
                                BaseProjectView::GetLongSentencesLabel()))
            {
            wxGetApp().GetAppOptions()->GetSentencesBreakdownInfo().EnableLongSentences(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetLongSentencesLabel()));
            }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
                                BaseProjectView::GetSentenceLengthBoxPlotLabel()))
            {
            wxGetApp().GetAppOptions()->GetSentencesBreakdownInfo().EnableLengthsBoxPlot(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceLengthBoxPlotLabel()));
            }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
                                BaseProjectView::GetSentenceLengthHeatmapLabel()))
            {
            wxGetApp().GetAppOptions()->GetSentencesBreakdownInfo().EnableLengthsHeatmap(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceLengthHeatmapLabel()));
            }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
                                BaseProjectView::GetSentenceLengthHistogramLabel()))
            {
            wxGetApp().GetAppOptions()->GetSentencesBreakdownInfo().EnableLengthsHistogram(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceLengthHistogramLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetWordCountsLabel()))
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableWordBarchart(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetWordCountsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetSyllableCountsLabel()))
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableSyllableHistogram(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSyllableCountsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetThreeSyllableWordsLabel()))
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().Enable3PlusSyllables(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetThreeSyllableWordsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetSixCharWordsLabel()))
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().Enable6PlusCharacter(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSixCharWordsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetWordCloudLabel()))
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableWordCloud(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetWordCloudLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetDaleChallLabel()))
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableDCUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetDaleChallLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetSpacheLabel()))
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableSpacheUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSpacheLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetHarrisJacobsonLabel()))
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableHarrisJacobsonUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetHarrisJacobsonLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, GetCustomTestsLabel()))
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableCustomTestsUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(GetCustomTestsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetAllWordsLabel()))
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableAllWords(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetAllWordsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetKeyWordsLabel()))
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableKeyWords(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetKeyWordsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetGrammarHighlightedReportLabel()))
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableHighlightedReport(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetGrammarHighlightedReportLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetMisspellingsLabel()))
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableMisspellings(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetMisspellingsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetRepeatedWordsLabel()))
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableRepeatedWords(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetRepeatedWordsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetArticleMismatchesLabel()))
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableArticleMismatches(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetArticleMismatchesLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetPhrasingErrorsTabLabel()))
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableWordingErrors(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetPhrasingErrorsTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetRedundantPhrasesTabLabel()))
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableRedundantPhrases(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetRedundantPhrasesTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetOverusedWordsBySentenceLabel()))
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableOverUsedWordsBySentence(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetOverusedWordsBySentenceLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetWordyPhrasesTabLabel()))
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableWordyPhrases(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetWordyPhrasesTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetClichesTabLabel()))
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableCliches(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetClichesTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetPassiveLabel()))
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnablePassiveVoice(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetPassiveLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel()))
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableConjunctionStartingSentences(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetSentenceStartingWithLowercaseTabLabel()))
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableLowercaseSentences(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceStartingWithLowercaseTabLabel()));
            }
        wxGetApp().GetAppOptions()->SetFilePathTruncationMode(
            static_cast<Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode>(
                m_filePathTruncationMode.get_value()));
        wxGetApp().GetAppOptions()->SetNumeralSyllabicationMethod(
            static_cast<NumeralSyllabize>(m_syllabicationMethod.get_value()));
        wxGetApp().GetAppOptions()->SetParagraphsParsingMethod(
            static_cast<ParagraphParse>(m_paragraphParsingMethod.get_value()));
        wxGetApp().GetAppOptions()->IgnoreBlankLinesForParagraphsParser(
            m_ignoreBlankLinesForParagraphsParser.get_value());
        wxGetApp().GetAppOptions()->IgnoreIndentingForParagraphsParser(
            m_ignoreIndentingForParagraphsParser.get_value());
        wxGetApp().GetAppOptions()->SetSentenceStartMustBeUppercased(
            m_sentenceStartMustBeUppercased.get_value());
        wxGetApp().GetAppOptions()->AggressiveExclusion(m_aggressiveExclusion.get_value());
        wxGetApp().GetAppOptions()->ExcludeTrailingCopyrightNoticeParagraphs(
            m_excludeTrailingCopyrightNoticeParagraphs.get_value());
        wxGetApp().GetAppOptions()->ExcludeTrailingCitations(
            m_excludeTrailingCitations.get_value());
        wxGetApp().GetAppOptions()->ExcludeFileAddresses(m_excludeFileAddresses.get_value());
        wxGetApp().GetAppOptions()->ExcludeNumerals(m_excludeNumerals.get_value());
        wxGetApp().GetAppOptions()->ExcludeProperNouns(m_excludeProperNouns.get_value());
        wxGetApp().GetAppOptions()->IncludeExcludedPhraseFirstOccurrence(
            m_includeExcludedPhraseFirstOccurrence.get_value());
        wxGetApp().GetAppOptions()->SetExcludedPhrasesPath(m_excludedPhrasesPath.get_value());
        wxGetApp().GetAppOptions()->SetExclusionBlockTags(m_exclusionBlockTags.get_value());
        wxGetApp().GetAppOptions()->SetIncludeIncompleteSentencesIfLongerThanValue(
            m_includeIncompleteSentencesIfLongerThan.get_value());
        wxGetApp().GetAppOptions()->SetInvalidSentenceMethod(static_cast<InvalidSentence>(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ?
                static_cast<int>(InvalidSentence::ExcludeFromAnalysis) :
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)) ?
                static_cast<int>(InvalidSentence::ExcludeExceptForHeadings) :
                static_cast<int>(InvalidSentence::IncludeAsFullSentences)));

        wxGetApp().GetAppOptions()->SetProjectLanguage(
            static_cast<readability::test_language>(m_projectLanguage.get_value()));
        wxGetApp().GetAppOptions()->SetReviewer(m_reviewer.get_value());
        wxGetApp().GetAppOptions()->UseRealTimeUpdate(m_realTimeUpdate.get_value());
        wxGetApp().GetAppOptions()->SetAppendedDocumentFilePath(
            m_appendedDocumentFilePath.get_value());
        wxGetApp().GetAppOptions()->SetDocumentStorageMethod(
            static_cast<TextStorage>(m_documentStorageMethod.get_value()));

        // test-specific options
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetDCTextExclusionLabel()))
            {
            wxGetApp().GetAppOptions()->SetDaleChallTextExclusionMode(
                static_cast<SpecializedTestTextExclusion>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(
                        GetDCTextExclusionLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetProperNounsLabel()))
            {
            wxGetApp().GetAppOptions()->SetDaleChallProperNounCountingMethod(
                static_cast<readability::proper_noun_counting_method>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(GetProperNounsLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetIncludeStockerLabel()))
            {
            wxGetApp().GetAppOptions()->IncludeStockerCatholicSupplement(
                m_readabilityTestsPropertyGrid->GetPropertyValueAsBool(GetIncludeStockerLabel()));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetHJCTextExclusionLabel()))
            {
            wxGetApp().GetAppOptions()->SetHarrisJacobsonTextExclusionMode(
                static_cast<SpecializedTestTextExclusion>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(
                        GetHJCTextExclusionLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetCountIndependentClausesLabel()))
            {
            wxGetApp().GetAppOptions()->FogUseSentenceUnits(
                m_readabilityTestsPropertyGrid->GetPropertyValueAsBool(
                    GetCountIndependentClausesLabel()));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid,
                                GetFleschNumeralSyllabicationLabel()))
            {
            wxGetApp().GetAppOptions()->SetFleschNumeralSyllabizeMethod(
                static_cast<FleschNumeralSyllabize>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(
                        GetFleschNumeralSyllabicationLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid,
                                GetFleschKincaidNumeralSyllabicationLabel()))
            {
            wxGetApp().GetAppOptions()->SetFleschKincaidNumeralSyllabizeMethod(
                static_cast<FleschKincaidNumeralSyllabize>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(
                        GetFleschKincaidNumeralSyllabicationLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetGraphColorSchemeLabel()))
            {
            const auto foundColorScheme = wxGetApp().GetGraphColorSchemeMap().find(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetGraphColorSchemeLabel()));
            if (foundColorScheme != wxGetApp().GetGraphColorSchemeMap().cend())
                {
                wxGetApp().GetAppOptions()->SetGraphColorScheme(foundColorScheme->second);
                }
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetBackgroundColorLabel()))
            {
            wxGetApp().GetAppOptions()->SetBackGroundColor(
                wxAny(
                    m_generalGraphPropertyGrid->GetProperty(GetBackgroundColorLabel())->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageLabel()))
            {
            wxGetApp().GetAppOptions()->SetPlotBackGroundImagePath(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetImageLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageEffectLabel()))
            {
            wxGetApp().GetAppOptions()->SetPlotBackGroundImageEffect(
                static_cast<Wisteria::ImageEffect>(
                    m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetImageEffectLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageFitLabel()))
            {
            wxGetApp().GetAppOptions()->SetPlotBackGroundImageFit(static_cast<Wisteria::ImageFit>(
                m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetImageFitLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageOpacityLabel()))
            {
            wxGetApp().GetAppOptions()->SetPlotBackGroundImageOpacity(static_cast<uint8_t>(
                m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetImageOpacityLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetLogoImageLabel()))
            {
            wxGetApp().GetAppOptions()->SetWatermarkLogo(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetLogoImageLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetBackgroundColorFadeLabel()))
            {
            wxGetApp().GetAppOptions()->SetGraphBackGroundLinearGradient(
                m_generalGraphPropertyGrid->GetPropertyValueAsBool(GetBackgroundColorFadeLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetColorLabel()))
            {
            wxGetApp().GetAppOptions()->SetPlotBackGroundColor(
                wxAny(m_generalGraphPropertyGrid->GetProperty(GetColorLabel())->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetColorOpacityLabel()))
            {
            wxGetApp().GetAppOptions()->SetPlotBackGroundColorOpacity(static_cast<uint8_t>(
                m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetColorOpacityLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetStippleImageLabel()))
            {
            wxGetApp().GetAppOptions()->SetStippleImagePath(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetStippleImageLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetStippleShapeLabel()))
            {
            const auto foundShape = wxGetApp().GetShapeMap().find(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetStippleShapeLabel()));
            if (foundShape != wxGetApp().GetShapeMap().cend())
                {
                wxGetApp().GetAppOptions()->SetStippleShape(foundShape->second);
                }
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetStippleShapeColorLabel()))
            {
            wxGetApp().GetAppOptions()->SetStippleShapeColor(
                wxAny(m_generalGraphPropertyGrid->GetProperty(GetStippleShapeColorLabel())
                          ->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetCommonImageLabel()))
            {
            wxGetApp().GetAppOptions()->SetGraphCommonImagePath(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetCommonImageLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetDisplayDropShadowsLabel()))
            {
            wxGetApp().GetAppOptions()->DisplayDropShadows(
                m_generalGraphPropertyGrid->GetPropertyValueAsBool(GetDisplayDropShadowsLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetShowcaseKeyItemsLabel()))
            {
            wxGetApp().GetAppOptions()->ShowcaseKeyItems(
                m_generalGraphPropertyGrid->GetPropertyValueAsBool(GetShowcaseKeyItemsLabel()));
            }

        if (IsPropertyAvailable(m_barChartPropertyGrid, GetColorLabel()))
            {
            wxGetApp().GetAppOptions()->SetBarChartBarColor(
                wxAny(m_barChartPropertyGrid->GetProperty(GetColorLabel())->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid, GetOpacityLabel()))
            {
            wxGetApp().GetAppOptions()->SetGraphBarOpacity(static_cast<uint8_t>(
                m_barChartPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid, GeOrientationLabel()))
            {
            wxGetApp().GetAppOptions()->SetBarChartOrientation(static_cast<Wisteria::Orientation>(
                m_barChartPropertyGrid->GetPropertyValueAsInt(GeOrientationLabel())));
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid, GetEffectLabel()))
            {
            wxGetApp().GetAppOptions()->SetGraphBarEffect(static_cast<Wisteria::BoxEffect>(
                m_barChartPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())));
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid, GetLabelsOnBarsLabel()))
            {
            wxGetApp().GetAppOptions()->DisplayBarChartLabels(
                m_barChartPropertyGrid->GetPropertyValueAsBool(GetLabelsOnBarsLabel()));
            }

        if (IsPropertyAvailable(m_histogramPropertyGrid, GetBinSortingLabel()))
            {
            wxGetApp().GetAppOptions()->SetHistogramBinningMethod(
                static_cast<Wisteria::Graphs::Histogram::BinningMethod>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetBinSortingLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid, GetGradeLevelRoundingLabel()))
            {
            wxGetApp().GetAppOptions()->SetHistogramRoundingMethod(
                static_cast<Wisteria::RoundingMethod>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetGradeLevelRoundingLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid, GetBinLabelsLabel()))
            {
            wxGetApp().GetAppOptions()->SetHistogramBinLabelDisplay(
                static_cast<Wisteria::BinLabelDisplay>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetBinLabelsLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid, GetIntervalDisplayLabel()))
            {
            wxGetApp().GetAppOptions()->SetHistogramIntervalDisplay(
                static_cast<Wisteria::Graphs::Histogram::IntervalDisplay>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetIntervalDisplayLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid, GetColorLabel()))
            {
            wxGetApp().GetAppOptions()->SetHistogramBarColor(
                wxAny(m_histogramPropertyGrid->GetProperty(GetColorLabel())->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid, GetOpacityLabel()))
            {
            wxGetApp().GetAppOptions()->SetHistogramBarOpacity(static_cast<uint8_t>(
                m_histogramPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid, GetEffectLabel()))
            {
            wxGetApp().GetAppOptions()->SetHistogramBarEffect(static_cast<Wisteria::BoxEffect>(
                m_histogramPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())));
            }

        if (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetColorLabel()))
            {
            wxGetApp().GetAppOptions()->SetGraphBoxColor(
                wxAny(m_boxPlotsPropertyGrid->GetProperty(GetColorLabel())->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetOpacityLabel()))
            {
            wxGetApp().GetAppOptions()->SetGraphBoxOpacity(static_cast<uint8_t>(
                m_boxPlotsPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetEffectLabel()))
            {
            wxGetApp().GetAppOptions()->SetGraphBoxEffect(static_cast<Wisteria::BoxEffect>(
                m_boxPlotsPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetLabelsOnBoxesLabel()))
            {
            wxGetApp().GetAppOptions()->DisplayBoxPlotLabels(
                m_boxPlotsPropertyGrid->GetPropertyValueAsBool(GetLabelsOnBoxesLabel()));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetConnectBoxesLabel()))
            {
            wxGetApp().GetAppOptions()->ConnectBoxPlotMiddlePoints(
                m_boxPlotsPropertyGrid->GetPropertyValueAsBool(GetConnectBoxesLabel()));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetShowAllDataPointsLabel()))
            {
            wxGetApp().GetAppOptions()->ShowAllBoxPlotPoints(
                m_boxPlotsPropertyGrid->GetPropertyValueAsBool(GetShowAllDataPointsLabel()));
            }

        wxGetApp().GetAppOptions()->SetXAxisFont(m_xAxisFont.get_value());
        wxGetApp().GetAppOptions()->SetXAxisFontColor(m_xAxisFontColor.get_value());
        wxGetApp().GetAppOptions()->SetYAxisFont(m_yAxisFont.get_value());
        wxGetApp().GetAppOptions()->SetYAxisFontColor(m_yAxisFontColor.get_value());
        wxGetApp().GetAppOptions()->SetGraphTopTitleFont(m_topTitleFont.get_value());
        wxGetApp().GetAppOptions()->SetGraphTopTitleFontColor(m_topTitleFontColor.get_value());
        wxGetApp().GetAppOptions()->SetGraphBottomTitleFont(m_bottomTitleFont.get_value());
        wxGetApp().GetAppOptions()->SetGraphBottomTitleFontColor(
            m_bottomTitleFontColor.get_value());
        wxGetApp().GetAppOptions()->SetGraphLeftTitleFont(m_leftTitleFont.get_value());
        wxGetApp().GetAppOptions()->SetGraphLeftTitleFontColor(m_leftTitleFontColor.get_value());
        wxGetApp().GetAppOptions()->SetGraphRightTitleFont(m_rightTitleFont.get_value());
        wxGetApp().GetAppOptions()->SetGraphRightTitleFontColor(m_rightTitleFontColor.get_value());
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid, GetInvalidRegionsColorLabel()))
            {
            wxGetApp().GetAppOptions()->SetInvalidAreaColor(
                wxAny(m_readabilityGraphPropertyGrid->GetProperty(GetInvalidRegionsColorLabel())
                          ->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid, GetRaygorStyleLabel()))
            {
            wxGetApp().GetAppOptions()->SetRaygorStyle(static_cast<Wisteria::Graphs::RaygorStyle>(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsInt(GetRaygorStyleLabel())));
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid,
                                GetUseEnglishLabelsForGermanLixLabel()))
            {
            wxGetApp().GetAppOptions()->UseEnglishLabelsForGermanLix(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsBool(
                    GetUseEnglishLabelsForGermanLixLabel()));
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid, GetFleschChartConnectPointsLabel()))
            {
            wxGetApp().GetAppOptions()->ConnectFleschPoints(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsBool(
                    GetFleschChartConnectPointsLabel()));
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid,
                                GetFleschSyllableRulerDocGroupsLabel()))
            {
            wxGetApp().GetAppOptions()->IncludeFleschRulerDocGroups(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsBool(
                    GetFleschSyllableRulerDocGroupsLabel()));
            }
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::SaveProjectOptions()
    {
    if ((m_readabilityProjectDoc != nullptr) && HaveOptionsChanged())
        {
        // change the origin of the document if switching to external document
        if (m_documentStorageMethod == static_cast<int>(TextStorage::LoadFromExternalDocument) &&
            m_readabilityProjectDoc->GetTextSource() == TextSource::EnteredText)
            {
            wxLogMessage(L"'%s': manually entered text converted to linked document in project.",
                         m_readabilityProjectDoc->GetTitle());
            m_readabilityProjectDoc->SetTextSource(TextSource::FromFile);
            }

        if (IsBatchProjectSettings() && HaveDocumentOptionsChanged())
            {
            // batch projects may need to do a full re-indexing, so just set this flag as a shortcut
            m_readabilityProjectDoc->RefreshRequired(ProjectRefresh::FullReindexing);
            // also update the file paths if they were changed and we aren't embedding the documents
            if ((m_fileList != nullptr) && m_fileList->HasItemBeenEditedByUser() &&
                m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText))
                {
                m_readabilityProjectDoc->GetSourceFilesInfo().resize(m_fileList->GetItemCount());
                for (long i = 0; i < m_fileList->GetItemCount(); ++i)
                    {
                    m_readabilityProjectDoc->GetSourceFilesInfo()[i] = comparable_first_pair(
                        m_fileList->GetItemTextEx(i, 0), m_fileList->GetItemTextEx(i, 1));
                    }
                }
            }
        else if (IsStandardProjectSettings() && HaveDocumentOptionsChanged())
            {
            m_readabilityProjectDoc->RefreshRequired(ProjectRefresh::FullReindexing);
            }

        if (IsPropertyAvailable(m_gradeLevelPropertyGrid, GetCalculationLabel()))
            {
            m_readabilityProjectDoc->GetReadabilityMessageCatalog().SetReadingAgeDisplay(
                static_cast<ReadabilityMessages::ReadingAgeDisplay>(
                    m_gradeLevelPropertyGrid->GetPropertyValueAsInt(GetCalculationLabel())));
            }
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid, GetGradeScaleLabel()))
            {
            m_readabilityProjectDoc->GetReadabilityMessageCatalog().SetGradeScale(
                static_cast<readability::grade_scale>(
                    m_gradeLevelPropertyGrid->GetPropertyValueAsInt(GetGradeScaleLabel())));
            }
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid, GetGradesLongFormatLabel()))
            {
            m_readabilityProjectDoc->GetReadabilityMessageCatalog().SetLongGradeScaleFormat(
                m_gradeLevelPropertyGrid->GetPropertyValueAsBool(GetGradesLongFormatLabel()));
            }
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid, GetIncludeScoreSummaryLabel()))
            {
            m_readabilityProjectDoc->IncludeScoreSummaryReport(
                m_gradeLevelPropertyGrid->GetPropertyValueAsBool(GetIncludeScoreSummaryLabel()));
            }
        m_readabilityProjectDoc->SetLongSentenceMethod(m_longSentencesNumberOfWords ?
                                                           LongSentence::LongerThanSpecifiedLength :
                                                           LongSentence::OutlierLength);
        m_readabilityProjectDoc->SetDifficultSentenceLength(m_sentenceLength.get_value());
        m_readabilityProjectDoc->SetMinDocWordCountForBatch(m_minDocWordCountForBatch.get_value());
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
                                BaseProjectView::GetLongSentencesLabel()))
            {
            m_readabilityProjectDoc->GetSentencesBreakdownInfo().EnableLongSentences(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetLongSentencesLabel()));
            }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
                                BaseProjectView::GetSentenceLengthBoxPlotLabel()))
            {
            m_readabilityProjectDoc->GetSentencesBreakdownInfo().EnableLengthsBoxPlot(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceLengthBoxPlotLabel()));
            }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
                                BaseProjectView::GetSentenceLengthHeatmapLabel()))
            {
            m_readabilityProjectDoc->GetSentencesBreakdownInfo().EnableLengthsHeatmap(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceLengthHeatmapLabel()));
            }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
                                BaseProjectView::GetSentenceLengthHistogramLabel()))
            {
            m_readabilityProjectDoc->GetSentencesBreakdownInfo().EnableLengthsHistogram(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceLengthHistogramLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetWordCountsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableWordBarchart(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetWordCountsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetSyllableCountsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableSyllableHistogram(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSyllableCountsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetThreeSyllableWordsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().Enable3PlusSyllables(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetThreeSyllableWordsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetSixCharWordsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().Enable6PlusCharacter(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSixCharWordsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetWordCloudLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableWordCloud(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetWordCloudLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetDaleChallLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableDCUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetDaleChallLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetSpacheLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableSpacheUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSpacheLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
                                BaseProjectView::GetHarrisJacobsonLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableHarrisJacobsonUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetHarrisJacobsonLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, GetCustomTestsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableCustomTestsUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(GetCustomTestsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetAllWordsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableAllWords(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetAllWordsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetKeyWordsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableKeyWords(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetKeyWordsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreProperNounsLabel()))
            {
            m_readabilityProjectDoc->SpellCheckIgnoreProperNouns(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreProperNounsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreUppercasedWordsLabel()))
            {
            m_readabilityProjectDoc->SpellCheckIgnoreUppercased(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreUppercasedWordsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreNumeralsLabel()))
            {
            m_readabilityProjectDoc->SpellCheckIgnoreNumerals(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreNumeralsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreFileAddressesLabel()))
            {
            m_readabilityProjectDoc->SpellCheckIgnoreFileAddresses(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreFileAddressesLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreProgrammerCodeLabel()))
            {
            m_readabilityProjectDoc->SpellCheckIgnoreProgrammerCode(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreProgrammerCodeLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetAllowColloquialismsLabel()))
            {
            m_readabilityProjectDoc->SpellCheckAllowColloquialisms(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetAllowColloquialismsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreSocialMediaLabel()))
            {
            m_readabilityProjectDoc->SpellCheckIgnoreSocialMediaTags(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreSocialMediaLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, GetGrammarHighlightedReportLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableHighlightedReport(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetGrammarHighlightedReportLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetMisspellingsLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableMisspellings(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetMisspellingsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetRepeatedWordsLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableRepeatedWords(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetRepeatedWordsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetArticleMismatchesLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableArticleMismatches(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetArticleMismatchesLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetPhrasingErrorsTabLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableWordingErrors(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetPhrasingErrorsTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetRedundantPhrasesTabLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableRedundantPhrases(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetRedundantPhrasesTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetOverusedWordsBySentenceLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableOverUsedWordsBySentence(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetOverusedWordsBySentenceLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetWordyPhrasesTabLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableWordyPhrases(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetWordyPhrasesTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetClichesTabLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableCliches(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetClichesTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetPassiveLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnablePassiveVoice(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetPassiveLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableConjunctionStartingSentences(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,
                                BaseProjectView::GetSentenceStartingWithLowercaseTabLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableLowercaseSentences(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceStartingWithLowercaseTabLabel()));
            }
        m_readabilityProjectDoc->SetFilePathTruncationMode(
            static_cast<Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode>(
                m_filePathTruncationMode.get_value()));
        m_readabilityProjectDoc->SetNumeralSyllabicationMethod(
            static_cast<NumeralSyllabize>(m_syllabicationMethod.get_value()));
        m_readabilityProjectDoc->SetParagraphsParsingMethod(
            static_cast<ParagraphParse>(m_paragraphParsingMethod.get_value()));
        m_readabilityProjectDoc->IgnoreBlankLinesForParagraphsParser(
            m_ignoreBlankLinesForParagraphsParser.get_value());
        m_readabilityProjectDoc->IgnoreIndentingForParagraphsParser(
            m_ignoreIndentingForParagraphsParser.get_value());
        m_readabilityProjectDoc->SetSentenceStartMustBeUppercased(
            m_sentenceStartMustBeUppercased.get_value());
        m_readabilityProjectDoc->AggressiveExclusion(m_aggressiveExclusion.get_value());
        m_readabilityProjectDoc->ExcludeTrailingCopyrightNoticeParagraphs(
            m_excludeTrailingCopyrightNoticeParagraphs.get_value());
        m_readabilityProjectDoc->ExcludeTrailingCitations(m_excludeTrailingCitations.get_value());
        m_readabilityProjectDoc->ExcludeFileAddresses(m_excludeFileAddresses.get_value());
        m_readabilityProjectDoc->ExcludeNumerals(m_excludeNumerals.get_value());
        m_readabilityProjectDoc->ExcludeProperNouns(m_excludeProperNouns.get_value());
        m_readabilityProjectDoc->SetExcludedPhrasesPath(m_excludedPhrasesPath.get_value());
        m_readabilityProjectDoc->IncludeExcludedPhraseFirstOccurrence(
            m_includeExcludedPhraseFirstOccurrence.get_value());
        m_readabilityProjectDoc->SetExclusionBlockTags(m_exclusionBlockTags.get_value());
        m_readabilityProjectDoc->SetIncludeIncompleteSentencesIfLongerThanValue(
            m_includeIncompleteSentencesIfLongerThan.get_value());
        m_readabilityProjectDoc->SetInvalidSentenceMethod(static_cast<InvalidSentence>(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ?
                static_cast<int>(InvalidSentence::ExcludeFromAnalysis) :
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)) ?
                static_cast<int>(InvalidSentence::ExcludeExceptForHeadings) :
                static_cast<int>(InvalidSentence::IncludeAsFullSentences)));
        m_readabilityProjectDoc->SetProjectLanguage(
            static_cast<readability::test_language>(m_projectLanguage.get_value()));
        m_readabilityProjectDoc->SetReviewer(m_reviewer.get_value());
        m_readabilityProjectDoc->SetStatus(m_status.get_value());
        m_readabilityProjectDoc->UseRealTimeUpdate(m_realTimeUpdate.get_value());
        m_readabilityProjectDoc->SetAppendedDocumentFilePath(
            m_appendedDocumentFilePath.get_value());
        m_readabilityProjectDoc->SetDocumentStorageMethod(
            static_cast<TextStorage>(m_documentStorageMethod.get_value()));
        if (IsStandardProjectSettings())
            {
            m_readabilityProjectDoc->SetOriginalDocumentFilePath(m_filePath.get_value());
            m_readabilityProjectDoc->GetSourceFilesInfo().at(0).second = m_description.get_value();
            }

        // test-specific options
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetDCTextExclusionLabel()))
            {
            m_readabilityProjectDoc->SetDaleChallTextExclusionMode(
                static_cast<SpecializedTestTextExclusion>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(
                        GetDCTextExclusionLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetProperNounsLabel()))
            {
            m_readabilityProjectDoc->SetDaleChallProperNounCountingMethod(
                static_cast<readability::proper_noun_counting_method>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(GetProperNounsLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetIncludeStockerLabel()))
            {
            m_readabilityProjectDoc->IncludeStockerCatholicSupplement(
                m_readabilityTestsPropertyGrid->GetPropertyValueAsBool(GetIncludeStockerLabel()));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetHJCTextExclusionLabel()))
            {
            m_readabilityProjectDoc->SetHarrisJacobsonTextExclusionMode(
                static_cast<SpecializedTestTextExclusion>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(
                        GetHJCTextExclusionLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetCountIndependentClausesLabel()))
            {
            m_readabilityProjectDoc->FogUseSentenceUnits(
                m_readabilityTestsPropertyGrid->GetPropertyValueAsBool(
                    GetCountIndependentClausesLabel()));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid,
                                GetFleschNumeralSyllabicationLabel()))
            {
            m_readabilityProjectDoc->SetFleschNumeralSyllabizeMethod(
                static_cast<FleschNumeralSyllabize>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(
                        GetFleschNumeralSyllabicationLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid,
                                GetFleschKincaidNumeralSyllabicationLabel()))
            {
            m_readabilityProjectDoc->SetFleschKincaidNumeralSyllabizeMethod(
                static_cast<FleschKincaidNumeralSyllabize>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(
                        GetFleschKincaidNumeralSyllabicationLabel())));
            }

        SaveTextWindowOptions();
        SaveProjectGraphOptions();
        SaveStatisticsOptions();

        m_readabilityProjectDoc->RefreshRequired(ProjectRefresh::Minimal);
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::SaveStatisticsOptions()
    {
    if ((m_readabilityProjectDoc != nullptr) && HaveOptionsChanged())
        {
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetParagraphsLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableParagraph(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetParagraphsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetSentencesLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableSentences(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetSentencesLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetWordsLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableWords(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetWordsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetExtendedWordsLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableExtendedWords(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetExtendedWordsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetGrammarLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableGrammar(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetGrammarLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetNotesLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableNotes(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetNotesLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetExtendedInformationLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableExtendedInformation(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetExtendedInformationLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetCoverageLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableDolchCoverage(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetCoverageLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetDolchWordsLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableDolchWords(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetDolchWordsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, GetDolchExplanationLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableDolchExplanation(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetDolchExplanationLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,
                                BaseProjectView::GetFormattedReportLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsInfo().EnableReport(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetFormattedReportLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid, BaseProjectView::GetTabularReportLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsInfo().EnableTable(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetTabularReportLabel()));
            }

        m_readabilityProjectDoc->RefreshRequired(ProjectRefresh::Minimal);
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::SaveTextWindowOptions()
    {
    if ((m_readabilityProjectDoc != nullptr) && HaveOptionsChanged())
        {
        m_readabilityProjectDoc->SetTextHighlightMethod(
            static_cast<TextHighlight>(m_textHighlightMethod.get_value()));
        m_readabilityProjectDoc->SetTextHighlightColor(m_highlightedColor.get_value());
        m_readabilityProjectDoc->SetExcludedTextHighlightColor(
            m_excludedTextHighlightColor.get_value());
        m_readabilityProjectDoc->SetDuplicateWordHighlightColor(
            m_duplicateWordHighlightColor.get_value());
        m_readabilityProjectDoc->SetWordyPhraseHighlightColor(
            m_wordyPhraseHighlightColor.get_value());
        m_readabilityProjectDoc->SetTextViewFont(m_font.get_value());
        m_readabilityProjectDoc->SetTextFontColor(m_fontColor.get_value());
        m_readabilityProjectDoc->SetDolchConjunctionsColor(m_dolchConjunctionsColor.get_value());
        m_readabilityProjectDoc->SetDolchPrepositionsColor(m_dolchPrepositionsColor.get_value());
        m_readabilityProjectDoc->SetDolchPronounsColor(m_dolchPronounsColor.get_value());
        m_readabilityProjectDoc->SetDolchAdverbsColor(m_dolchAdverbsColor.get_value());
        m_readabilityProjectDoc->SetDolchAdjectivesColor(m_dolchAdjectivesColor.get_value());
        m_readabilityProjectDoc->SetDolchVerbsColor(m_dolchVerbsColor.get_value());
        m_readabilityProjectDoc->SetDolchNounColor(m_dolchNounsColor.get_value());
        m_readabilityProjectDoc->HighlightDolchConjunctions(
            m_highlightDolchConjunctions.get_value());
        m_readabilityProjectDoc->HighlightDolchPrepositions(
            m_highlightDolchPrepositions.get_value());
        m_readabilityProjectDoc->HighlightDolchPronouns(m_highlightDolchPronouns.get_value());
        m_readabilityProjectDoc->HighlightDolchAdverbs(m_highlightDolchAdverbs.get_value());
        m_readabilityProjectDoc->HighlightDolchAdjectives(m_highlightDolchAdjectives.get_value());
        m_readabilityProjectDoc->HighlightDolchVerbs(m_highlightDolchVerbs.get_value());
        m_readabilityProjectDoc->HighlightDolchNouns(m_highlightDolchNouns.get_value());

        m_readabilityProjectDoc->RefreshRequired(ProjectRefresh::TextSection);
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::SaveProjectGraphOptions()
    {
    if ((m_readabilityProjectDoc != nullptr) && HaveOptionsChanged())
        {
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetGraphColorSchemeLabel()))
            {
            const auto foundColorScheme = wxGetApp().GetGraphColorSchemeMap().find(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetGraphColorSchemeLabel()));
            if (foundColorScheme != wxGetApp().GetGraphColorSchemeMap().cend())
                {
                m_readabilityProjectDoc->SetGraphColorScheme(foundColorScheme->second);
                }
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetBackgroundColorLabel()))
            {
            m_readabilityProjectDoc->SetBackGroundColor(
                wxAny(
                    m_generalGraphPropertyGrid->GetProperty(GetBackgroundColorLabel())->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageLabel()))
            {
            m_readabilityProjectDoc->SetPlotBackGroundImagePath(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetImageLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageEffectLabel()))
            {
            m_readabilityProjectDoc->SetPlotBackGroundImageEffect(
                static_cast<Wisteria::ImageEffect>(
                    m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetImageEffectLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageFitLabel()))
            {
            m_readabilityProjectDoc->SetPlotBackGroundImageFit(static_cast<Wisteria::ImageFit>(
                m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetImageFitLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageOpacityLabel()))
            {
            m_readabilityProjectDoc->SetPlotBackGroundImageOpacity(static_cast<uint8_t>(
                m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetImageOpacityLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetLogoImageLabel()))
            {
            m_readabilityProjectDoc->SetWatermarkLogoPath(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetLogoImageLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetBackgroundColorFadeLabel()))
            {
            m_readabilityProjectDoc->SetGraphBackGroundLinearGradient(
                m_generalGraphPropertyGrid->GetPropertyValueAsBool(GetBackgroundColorFadeLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetColorLabel()))
            {
            m_readabilityProjectDoc->SetPlotBackGroundColor(
                wxAny(m_generalGraphPropertyGrid->GetProperty(GetColorLabel())->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetColorOpacityLabel()))
            {
            m_readabilityProjectDoc->SetPlotBackGroundColorOpacity(static_cast<uint8_t>(
                m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetColorOpacityLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetStippleImageLabel()))
            {
            m_readabilityProjectDoc->SetStippleImagePath(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetStippleImageLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetStippleShapeLabel()))
            {
            const auto foundShape = wxGetApp().GetShapeMap().find(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetStippleShapeLabel()));
            if (foundShape != wxGetApp().GetShapeMap().cend())
                {
                m_readabilityProjectDoc->SetStippleShape(foundShape->second);
                }
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetStippleShapeColorLabel()))
            {
            m_readabilityProjectDoc->SetStippleShapeColor(
                wxAny(m_generalGraphPropertyGrid->GetProperty(GetStippleShapeColorLabel())
                          ->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetCommonImageLabel()))
            {
            m_readabilityProjectDoc->SetGraphCommonImagePath(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetCommonImageLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetDisplayDropShadowsLabel()))
            {
            m_readabilityProjectDoc->DisplayDropShadows(
                m_generalGraphPropertyGrid->GetPropertyValueAsBool(GetDisplayDropShadowsLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetShowcaseKeyItemsLabel()))
            {
            m_readabilityProjectDoc->ShowcaseKeyItems(
                m_generalGraphPropertyGrid->GetPropertyValueAsBool(GetShowcaseKeyItemsLabel()));
            }

        if (IsPropertyAvailable(m_barChartPropertyGrid, GetColorLabel()))
            {
            m_readabilityProjectDoc->SetBarChartBarColor(
                wxAny(m_barChartPropertyGrid->GetProperty(GetColorLabel())->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid, GeOrientationLabel()))
            {
            m_readabilityProjectDoc->SetBarChartOrientation(static_cast<Wisteria::Orientation>(
                m_barChartPropertyGrid->GetPropertyValueAsInt(GeOrientationLabel())));
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid, GetOpacityLabel()))
            {
            m_readabilityProjectDoc->SetGraphBarOpacity(static_cast<uint8_t>(
                m_barChartPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid, GetEffectLabel()))
            {
            m_readabilityProjectDoc->SetGraphBarEffect(static_cast<Wisteria::BoxEffect>(
                m_barChartPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())));
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid, GetLabelsOnBarsLabel()))
            {
            m_readabilityProjectDoc->DisplayBarChartLabels(
                m_barChartPropertyGrid->GetPropertyValueAsBool(GetLabelsOnBarsLabel()));
            }

        if (IsPropertyAvailable(m_histogramPropertyGrid, GetBinSortingLabel()))
            {
            m_readabilityProjectDoc->SetHistogramBinningMethod(
                static_cast<Wisteria::Graphs::Histogram::BinningMethod>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetBinSortingLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid, GetGradeLevelRoundingLabel()))
            {
            m_readabilityProjectDoc->SetHistogramRoundingMethod(
                static_cast<Wisteria::RoundingMethod>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetGradeLevelRoundingLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid, GetIntervalDisplayLabel()))
            {
            m_readabilityProjectDoc->SetHistogramIntervalDisplay(
                static_cast<Wisteria::Graphs::Histogram::IntervalDisplay>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetIntervalDisplayLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid, GetBinLabelsLabel()))
            {
            m_readabilityProjectDoc->SetHistogramBinLabelDisplay(
                static_cast<Wisteria::BinLabelDisplay>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetBinLabelsLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid, GetColorLabel()))
            {
            m_readabilityProjectDoc->SetHistogramBarColor(
                wxAny(m_histogramPropertyGrid->GetProperty(GetColorLabel())->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid, GetOpacityLabel()))
            {
            m_readabilityProjectDoc->SetHistogramBarOpacity(static_cast<uint8_t>(
                m_histogramPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid, GetEffectLabel()))
            {
            m_readabilityProjectDoc->SetHistogramBarEffect(static_cast<Wisteria::BoxEffect>(
                m_histogramPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())));
            }

        if (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetColorLabel()))
            {
            m_readabilityProjectDoc->SetGraphBoxColor(
                wxAny(m_boxPlotsPropertyGrid->GetProperty(GetColorLabel())->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetOpacityLabel()))
            {
            m_readabilityProjectDoc->SetGraphBoxOpacity(static_cast<uint8_t>(
                m_boxPlotsPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetEffectLabel()))
            {
            m_readabilityProjectDoc->SetGraphBoxEffect(static_cast<Wisteria::BoxEffect>(
                m_boxPlotsPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetLabelsOnBoxesLabel()))
            {
            m_readabilityProjectDoc->DisplayBoxPlotLabels(
                m_boxPlotsPropertyGrid->GetPropertyValueAsBool(GetLabelsOnBoxesLabel()));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetConnectBoxesLabel()))
            {
            m_readabilityProjectDoc->ConnectBoxPlotMiddlePoints(
                m_boxPlotsPropertyGrid->GetPropertyValueAsBool(GetConnectBoxesLabel()));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid, GetShowAllDataPointsLabel()))
            {
            m_readabilityProjectDoc->ShowAllBoxPlotPoints(
                m_boxPlotsPropertyGrid->GetPropertyValueAsBool(GetShowAllDataPointsLabel()));
            }

        m_readabilityProjectDoc->SetXAxisFont(m_xAxisFont.get_value());
        m_readabilityProjectDoc->SetXAxisFontColor(m_xAxisFontColor.get_value());
        m_readabilityProjectDoc->SetYAxisFont(m_yAxisFont.get_value());
        m_readabilityProjectDoc->SetYAxisFontColor(m_yAxisFontColor.get_value());
        m_readabilityProjectDoc->SetGraphTopTitleFont(m_topTitleFont.get_value());
        m_readabilityProjectDoc->SetGraphTopTitleFontColor(m_topTitleFontColor.get_value());
        m_readabilityProjectDoc->SetGraphBottomTitleFont(m_bottomTitleFont.get_value());
        m_readabilityProjectDoc->SetGraphBottomTitleFontColor(m_bottomTitleFontColor.get_value());
        m_readabilityProjectDoc->SetGraphLeftTitleFont(m_leftTitleFont.get_value());
        m_readabilityProjectDoc->SetGraphLeftTitleFontColor(m_leftTitleFontColor.get_value());
        m_readabilityProjectDoc->SetGraphRightTitleFont(m_rightTitleFont.get_value());
        m_readabilityProjectDoc->SetGraphRightTitleFontColor(m_rightTitleFontColor.get_value());
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid, GetInvalidRegionsColorLabel()))
            {
            m_readabilityProjectDoc->SetInvalidAreaColor(
                wxAny(m_readabilityGraphPropertyGrid->GetProperty(GetInvalidRegionsColorLabel())
                          ->GetValue())
                    .As<wxColour>());
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid, GetRaygorStyleLabel()))
            {
            m_readabilityProjectDoc->SetRaygorStyle(static_cast<Wisteria::Graphs::RaygorStyle>(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsInt(GetRaygorStyleLabel())));
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid,
                                GetUseEnglishLabelsForGermanLixLabel()))
            {
            m_readabilityProjectDoc->UseEnglishLabelsForGermanLix(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsBool(
                    GetUseEnglishLabelsForGermanLixLabel()));
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid, GetFleschChartConnectPointsLabel()))
            {
            m_readabilityProjectDoc->ConnectFleschPoints(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsBool(
                    GetFleschChartConnectPointsLabel()));
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid,
                                GetFleschSyllableRulerDocGroupsLabel()))
            {
            m_readabilityProjectDoc->IncludeFleschRulerDocGroups(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsBool(
                    GetFleschSyllableRulerDocGroupsLabel()));
            }

        m_readabilityProjectDoc->RefreshRequired(ProjectRefresh::Minimal);
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    if (!ValidateOptions())
        {
        return;
        }

    SaveOptions();

    if (IsModal())
        {
        EndModal(wxID_OK);
        }
    else
        {
        Show(false);
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnExportSettings([[maybe_unused]] wxCommandEvent& event)
    {
    if (!ValidateOptions())
        {
        return;
        }

    if (HaveOptionsChanged())
        {
        if (wxMessageBox(_(L"Some of your settings have changed but have not been saved yet.\n"
                           "Do you wish to save these changes before exporting?"),
                         _(L"Loading Settings"),
                         wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION) == wxYES)
            {
            SaveOptions();
            }
        }
    wxFileDialog dialog(
        this, _(L"Export Settings File"), wxEmptyString, L"Settings.xml",
        // TRANSLATORS: %s is the application name.
        wxString::Format(_(L"%s Settings Files (*.xml)|*.xml"), wxGetApp().GetAppDisplayName()),
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    wxGetApp().GetAppOptions()->SaveOptionsFile(dialog.GetPath());
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnImportSettings([[maybe_unused]] wxCommandEvent& event)
    {
    wxFileDialog dialog(
        this, _(L"Import Settings File"), wxEmptyString, L"Settings.xml",
        wxString::Format(_(L"%s Settings Files (*.xml)|*.xml"), wxGetApp().GetAppDisplayName()),
        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    wxMessageBox(_(L"Settings from the selected file will now be applied."), _(L"Loading Settings"),
                 wxOK | wxICON_INFORMATION);
    const auto reviewer = wxGetApp().GetAppOptions()->GetReviewer();
    wxGetApp().GetAppOptions()->LoadOptionsFile(dialog.GetPath(), false, false);
    wxGetApp().GetAppOptions()->SetReviewer(reviewer);
    Close();
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnResetSettings([[maybe_unused]] wxCommandEvent& event)
    {
    if (wxMessageBox(_(L"Do you wish to reset all of your program and user settings?"),
                     _(L"Reset Settings"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION) == wxYES)
        {
        wxGetApp().GetAppOptions()->ResetSettings();
        Close();
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateDocumentIndexingSection()
    {
    const int optionIndentSize = wxSizerFlags::GetDefaultBorder() * 3;

    if ((GetSectionsBeingShown() & DocumentIndexing) != 0)
        {
        auto* analysisIndexingPage = new wxPanel(m_sideBar, ANALYSIS_INDEXING_PAGE,
                                                 wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        wxRadioButton* radioButton = nullptr;

        auto* panelSizer = new wxBoxSizer(wxVERTICAL);
        analysisIndexingPage->SetSizer(panelSizer);
        m_sideBar->AddPage(analysisIndexingPage, _(L"Document Indexing"), ANALYSIS_INDEXING_PAGE,
                           // if the only section being shown, then show this page
                           (GetSectionsBeingShown() == DocumentIndexing), 12);

        // long sentence section
        CreateLabelHeader(analysisIndexingPage, panelSizer,
                          _(L"Consider Sentences Overly Long If:"), true);

        auto* optionsIndentSizer = new wxBoxSizer(wxVERTICAL);
        panelSizer->Add(optionsIndentSizer,
                        wxSizerFlags{}.Expand().Border(wxLEFT | wxRIGHT, optionIndentSize));
        auto* longerThanWordSizer = new wxBoxSizer(wxHORIZONTAL);
        optionsIndentSizer->Add(longerThanWordSizer, wxSizerFlags{}.Border(wxBOTTOM));
        radioButton =
            new wxRadioButton(analysisIndexingPage, ID_SENTENCE_LONGER_THAN_BUTTON,
                              _(L"&Longer than"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP,
                              wxGenericValidator(&m_longSentencesNumberOfWords.get_value()));
        longerThanWordSizer->Add(radioButton, wxSizerFlags{}.CenterVertical());
        // the spin control for the number of words
        auto* wordsPerLongSentenceSpinCtrl =
            new wxSpinCtrl(analysisIndexingPage, wxID_ANY,
                           std::to_wstring(m_sentenceLength.get_value()), wxDefaultPosition,
                           wxDefaultSize, wxSP_ARROW_KEYS, 0, std::numeric_limits<int>::max(), 0);
        wordsPerLongSentenceSpinCtrl->SetValidator(
            wxGenericValidator(&m_sentenceLength.get_value()));
        longerThanWordSizer->Add(wordsPerLongSentenceSpinCtrl,
                                 wxSizerFlags{}.CenterVertical().Border());
        // "words" after it
        auto* wordsLabel = new wxStaticText(analysisIndexingPage, wxID_STATIC, _(L"words"),
                                            wxDefaultPosition, wxDefaultSize, 0);
        longerThanWordSizer->Add(wordsLabel,
                                 wxSizerFlags{}.CenterVertical().Border(wxLEFT | wxRIGHT));

        radioButton = new wxRadioButton(analysisIndexingPage, ID_SENTENCE_OUTLIER_LENGTH_BUTTON,
                                        _(L"Out&side sentence-length outlier range"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_longSentencesOutliers.get_value()));
        optionsIndentSizer->Add(radioButton, wxSizerFlags{}.Expand().Border(wxBOTTOM));
        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        // sentence/paragraph deduction
        CreateLabelHeader(analysisIndexingPage, panelSizer, _(L"Sentence/Paragraph Deduction:"),
                          true);
        optionsIndentSizer = new wxBoxSizer(wxVERTICAL);
        panelSizer->Add(optionsIndentSizer,
                        wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));

        auto* lineEndsSizer = new wxBoxSizer(wxHORIZONTAL);
        lineEndsSizer->Add(new wxStaticText(analysisIndexingPage, wxID_STATIC, _(L"Line ends:")),
                           wxSizerFlags{}.CenterVertical().Border(wxRIGHT));

        wxArrayString paragraphParseOptions;
        paragraphParseOptions.Add(_(L"only begin a new paragraph if following a valid sentence"));
        paragraphParseOptions.Add(_(L"always begin a new paragraph"));
        auto* paragraphParseCombo = new wxChoice(
            analysisIndexingPage, ID_PARAGRAPH_PARSE, wxDefaultPosition, wxDefaultSize,
            paragraphParseOptions, 0, wxGenericValidator(&m_paragraphParsingMethod.get_value()));
        lineEndsSizer->Add(paragraphParseCombo, wxSizerFlags{}.CenterVertical().Border(wxRIGHT));
        optionsIndentSizer->Add(lineEndsSizer, wxSizerFlags{}.Expand().Border(wxBOTTOM));

        m_ignoreBlankLinesCheckBox =
            new wxCheckBox(analysisIndexingPage, ID_IGNORE_BLANK_LINES_BUTTON,
                           _(L"Ignore &blank lines"), wxDefaultPosition, wxDefaultSize, 0,
                           wxGenericValidator(&m_ignoreBlankLinesForParagraphsParser.get_value()));
        m_ignoreBlankLinesCheckBox->Enable(
            static_cast<ParagraphParse>(m_paragraphParsingMethod.get_value()) ==
            ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs);
        optionsIndentSizer->Add(m_ignoreBlankLinesCheckBox,
                                wxSizerFlags{}.Expand().Border(wxBOTTOM));

        m_ignoreIndentingCheckBox =
            new wxCheckBox(analysisIndexingPage, ID_IGNORE_INDENTING_BUTTON,
                           _(L"&Ignore indenting"), wxDefaultPosition, wxDefaultSize, 0,
                           wxGenericValidator(&m_ignoreIndentingForParagraphsParser.get_value()));
        m_ignoreIndentingCheckBox->Enable(
            static_cast<ParagraphParse>(m_paragraphParsingMethod.get_value()) ==
            ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs);
        optionsIndentSizer->Add(m_ignoreIndentingCheckBox,
                                wxSizerFlags{}.Expand().Border(wxBOTTOM));

        auto* sentenceStartMustBeUppercasedCheckBox = new wxCheckBox(
            analysisIndexingPage, ID_SENTENCES_MUST_BE_CAP_BUTTON,
            _(L"Sentences must begin with capitalized words"), wxDefaultPosition, wxDefaultSize, 0,
            wxGenericValidator(&m_sentenceStartMustBeUppercased.get_value()));
        optionsIndentSizer->Add(sentenceStartMustBeUppercasedCheckBox,
                                wxSizerFlags{}.Expand().Border(wxBOTTOM));
        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        // incomplete sentence logic
        CreateLabelHeader(analysisIndexingPage, panelSizer, _(L"Text Exclusion:"), true);
        optionsIndentSizer = new wxBoxSizer(wxVERTICAL);
        panelSizer->Add(optionsIndentSizer,
                        wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));

        auto* exclusionSizer = new wxBoxSizer(wxHORIZONTAL);
        exclusionSizer->Add(new wxStaticText(analysisIndexingPage, wxID_STATIC, _(L"Exclusion:")),
                            wxSizerFlags{}.CenterVertical().Border(wxRIGHT));

        wxArrayString exclusionOptions;
        exclusionOptions.Add(_(L"Exclude all incomplete sentences"));
        exclusionOptions.Add(_(L"Do not exclude any text"));
        exclusionOptions.Add(_(L"Exclude all incomplete sentences, except headings"));
        auto* exclusionCombo = new wxChoice(analysisIndexingPage, ID_TEXT_EXCLUDE_METHOD,
                                            wxDefaultPosition, wxDefaultSize, exclusionOptions, 0,
                                            wxGenericValidator(&m_textExclusionMethod.get_value()));
        exclusionSizer->Add(exclusionCombo, wxSizerFlags{}.CenterVertical().Border(wxRIGHT));
        optionsIndentSizer->Add(exclusionSizer, wxSizerFlags{}.Expand().Border(wxBOTTOM));

        auto* includeIncompleteSentSizeSizer = new wxBoxSizer(wxHORIZONTAL);
        optionsIndentSizer->Add(includeIncompleteSentSizeSizer, wxSizerFlags{}.Border(wxBOTTOM));
        m_includeIncompleteSentSizeIncludeIncompleteLabel =
            new wxStaticText(analysisIndexingPage, ID_INCOMPLETE_SENTENCE_VALID_LABEL_START,
                             _(L"Include incomplete sentences containing more than"),
                             wxDefaultPosition, wxDefaultSize, 0);
        includeIncompleteSentSizeSizer->Add(m_includeIncompleteSentSizeIncludeIncompleteLabel,
                                            wxSizerFlags{}.CenterVertical());
        // the spin control for the number of words
        auto* includeIncompleteSentencesIfLongerThanSpinCtrl =
            new wxSpinCtrl(analysisIndexingPage, ID_INCOMPLETE_SENTENCE_VALID_VALUE_BOX,
                           std::to_wstring(m_includeIncompleteSentencesIfLongerThan.get_value()),
                           wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0,
                           std::numeric_limits<int>::max(), 0);
        includeIncompleteSentencesIfLongerThanSpinCtrl->SetValidator(
            wxGenericValidator(&m_includeIncompleteSentencesIfLongerThan.get_value()));
        includeIncompleteSentSizeSizer->Add(includeIncompleteSentencesIfLongerThanSpinCtrl,
                                            wxSizerFlags{}.Border(wxLEFT | wxRIGHT));
        // "words" after it
        m_includeIncompleteSentSizeWordsLabel =
            new wxStaticText(analysisIndexingPage, ID_INCOMPLETE_SENTENCE_VALID_LABEL_END,
                             _(L"words"), wxDefaultPosition, wxDefaultSize, 0);
        includeIncompleteSentSizeSizer->Add(m_includeIncompleteSentSizeWordsLabel,
                                            wxSizerFlags{}.CenterVertical());

        auto* ignoreOptionsGrid = new wxFlexGridSizer(
            2, wxSize(wxSizerFlags::GetDefaultBorder(), wxSizerFlags::GetDefaultBorder()));
        optionsIndentSizer->Add(ignoreOptionsGrid, wxSizerFlags{}.Expand().Border(wxBOTTOM));

        m_aggressiveExclusionCheckBox =
            new wxCheckBox(analysisIndexingPage, ID_AGGRESSIVE_LIST_DEDUCTION_CHECKBOX,
                           _(L"Aggressive exclusion"), wxDefaultPosition, wxDefaultSize, 0,
                           wxGenericValidator(&m_aggressiveExclusion.get_value()));
        m_aggressiveExclusionCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        ignoreOptionsGrid->Add(m_aggressiveExclusionCheckBox);

        m_ignoreFileAddressesCheckBox = new wxCheckBox(
            analysisIndexingPage, ID_EXCLUDE_FILE_ADDRESS_CHECKBOX,
            _(L"Also exclude Internet and file addresses"), wxDefaultPosition, wxDefaultSize, 0,
            wxGenericValidator(&m_excludeFileAddresses.get_value()));
        m_ignoreFileAddressesCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        ignoreOptionsGrid->Add(m_ignoreFileAddressesCheckBox);

        m_ignoreCopyrightsCheckBox = new wxCheckBox(
            analysisIndexingPage, ID_EXCLUDE_COPYRIGHT_CHECKBOX,
            _(L"Also exclude &copyright notices"), wxDefaultPosition, wxDefaultSize, 0,
            wxGenericValidator(&m_excludeTrailingCopyrightNoticeParagraphs.get_value()));
        m_ignoreCopyrightsCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        ignoreOptionsGrid->Add(m_ignoreCopyrightsCheckBox);

        m_ignoreNumeralsCheckBox =
            new wxCheckBox(analysisIndexingPage, ID_EXCLUDE_NUMERALS_CHECKBOX,
                           _(L"Also exclude numerals"), wxDefaultPosition, wxDefaultSize, 0,
                           wxGenericValidator(&m_excludeNumerals.get_value()));
        m_ignoreNumeralsCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        ignoreOptionsGrid->Add(m_ignoreNumeralsCheckBox);

        m_ignoreCitationsCheckBox =
            new wxCheckBox(analysisIndexingPage, ID_EXCLUDE_CITATIONS_CHECKBOX,
                           _(L"Also exclude trailing citations"), wxDefaultPosition, wxDefaultSize,
                           0, wxGenericValidator(&m_excludeTrailingCitations.get_value()));
        m_ignoreCitationsCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        ignoreOptionsGrid->Add(m_ignoreCitationsCheckBox);

        m_ignoreProperNounsCheckBox =
            new wxCheckBox(analysisIndexingPage, ID_EXCLUDE_PROPER_NOUNS_CHECKBOX,
                           _(L"Also exclude proper nouns"), wxDefaultPosition, wxDefaultSize, 0,
                           wxGenericValidator(&m_excludeProperNouns.get_value()));
        m_ignoreProperNounsCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        ignoreOptionsGrid->Add(m_ignoreProperNounsCheckBox);

        // excluded phrases
        auto* excludedPhrasesFileBrowseBoxSizer = new wxStaticBoxSizer(
            wxVERTICAL, analysisIndexingPage, _(L"Words && phrases to exclude:"));

        auto* excludePathSizer = new wxBoxSizer(wxHORIZONTAL);
        excludedPhrasesFileBrowseBoxSizer->Add(excludePathSizer, wxSizerFlags{ 1 }.Expand());

        m_excludedPhrasesPathFilePathEdit =
            new wxTextCtrl(excludedPhrasesFileBrowseBoxSizer->GetStaticBox(), wxID_ANY,
                           wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME,
                           wxGenericValidator(&m_excludedPhrasesPath.get_value()));
        m_excludedPhrasesPathFilePathEdit->AutoCompleteFileNames();
        excludePathSizer->Add(m_excludedPhrasesPathFilePathEdit, wxSizerFlags{ 1 }.Expand());

        m_excludedPhrasesEditBrowseButton = new wxBitmapButton(
            excludedPhrasesFileBrowseBoxSizer->GetStaticBox(), ID_EXCLUDED_PHRASES_FILE_EDIT_BUTTON,
            wxArtProvider::GetBitmapBundle(L"ID_EDIT", wxART_BUTTON));
        excludePathSizer->Add(m_excludedPhrasesEditBrowseButton);

        m_includeExcludedPhraseFirstOccurrenceCheckBox =
            new wxCheckBox(excludedPhrasesFileBrowseBoxSizer->GetStaticBox(),
                           ID_INCLUDE_FIRST_OCCURRENCE_EXCLUDE_CHECKBOX,
                           _(L"Include first occurrence"), wxDefaultPosition, wxDefaultSize, 0,
                           wxGenericValidator(&m_includeExcludedPhraseFirstOccurrence.get_value()));
        excludedPhrasesFileBrowseBoxSizer->Add(m_includeExcludedPhraseFirstOccurrenceCheckBox,
                                               wxSizerFlags{}.Expand().Border(wxTOP));

        m_excludedPhrasesPathFilePathEdit->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        m_excludedPhrasesEditBrowseButton->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        m_includeExcludedPhraseFirstOccurrenceCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));

        optionsIndentSizer->Add(excludedPhrasesFileBrowseBoxSizer,
                                wxSizerFlags{}.Expand().Border(wxBOTTOM | wxRIGHT));
        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        // tag block exclusion
        if (m_exclusionBlockTags.get_value().empty())
            {
            m_exclusionBlockTagsOption = 0;
            }
        else if (m_exclusionBlockTags.get_value().at(0) == std::make_pair(L'^', L'^'))
            {
            m_exclusionBlockTagsOption = 1;
            }
        else if (m_exclusionBlockTags.get_value().at(0) == std::make_pair(L'<', L'>'))
            {
            m_exclusionBlockTagsOption = 2;
            }
        else if (m_exclusionBlockTags.get_value().at(0) == std::make_pair(L'[', L']'))
            {
            m_exclusionBlockTagsOption = 3;
            }
        else if (m_exclusionBlockTags.get_value().at(0) == std::make_pair(L'{', L'}'))
            {
            m_exclusionBlockTagsOption = 4;
            }
        else if (m_exclusionBlockTags.get_value().at(0) == std::make_pair(L'(', L')'))
            {
            m_exclusionBlockTagsOption = 5;
            }
        else
            {
            m_exclusionBlockTagsOption = 0;
            }

        auto* exclusionBlockTagsSizer = new wxBoxSizer(wxHORIZONTAL);
        m_exclusionBlockTagsLabel = new wxStaticText(
            analysisIndexingPage, ID_EXCLUSION_TAG_BLOCK_LABEL, _(L"Exclude text between:"),
            wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
        m_exclusionBlockTagsLabel->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        exclusionBlockTagsSizer->Add(m_exclusionBlockTagsLabel,
                                     wxSizerFlags{}.Border(wxRIGHT).CenterVertical());
        wxArrayString exclusionBlockTagsOptions;
        exclusionBlockTagsOptions.Add(_(L"Not enabled"));
        exclusionBlockTagsOptions.Add(_(L"^ and ^"));
        exclusionBlockTagsOptions.Add(_(L"< and >"));
        exclusionBlockTagsOptions.Add(_(L"[ and ]"));
        exclusionBlockTagsOptions.Add(_(L"{ and }"));
        exclusionBlockTagsOptions.Add(_(L"( and )"));
        m_exclusionBlockTagsCombo =
            new wxChoice(analysisIndexingPage, ID_EXCLUSION_TAG_BLOCK_SELECTION, wxDefaultPosition,
                         wxDefaultSize, exclusionBlockTagsOptions, 0,
                         wxGenericValidator(&m_exclusionBlockTagsOption));
        m_exclusionBlockTagsCombo->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        exclusionBlockTagsSizer->Add(m_exclusionBlockTagsCombo);
        optionsIndentSizer->Add(exclusionBlockTagsSizer, wxSizerFlags{ 1 }.Expand().Border(wxTOP));

        // syllabication
        CreateLabelHeader(analysisIndexingPage, panelSizer, _(L"Numerals:"), true);

        auto* syllableSizer = new wxBoxSizer(wxHORIZONTAL);
        m_syllableLabel = new wxStaticText(analysisIndexingPage, wxID_STATIC, _(L"Syllabication:"));
        syllableSizer->Add(m_syllableLabel, wxSizerFlags{}.Border(wxRIGHT).CenterVertical());

        wxArrayString syllableOptions;
        syllableOptions.Add(_(L"Numerals are one syllable"));
        syllableOptions.Add(_(L"Sound out each digit"));
        m_syllableCombo = new wxChoice(analysisIndexingPage, ID_NUMBER_SYLLABIZE_METHOD,
                                       wxDefaultPosition, wxDefaultSize, syllableOptions, 0,
                                       wxGenericValidator(&m_syllabicationMethod.get_value()));
        m_syllableCombo->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
            !m_excludeNumerals);
        m_syllableLabel->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
            !m_excludeNumerals);
        syllableSizer->Add(m_syllableCombo, wxSizerFlags{}.Border(wxRIGHT).CenterVertical());
        panelSizer->Add(syllableSizer, wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateReadabilitySection()
    {
    if ((GetSectionsBeingShown() & ScoresSection) != 0)
        {
        auto* scoreTestOptionsPage = new wxPanel(m_sideBar, SCORES_TEST_OPTIONS_PAGE,
                                                 wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        m_sideBar->AddPage(scoreTestOptionsPage, GetReadabilityScoresLabel(),
                           SCORES_TEST_OPTIONS_PAGE,
                           // if the only section being shown, then show this page
                           (GetSectionsBeingShown() == ScoresSection), 1);

        // test options tab
        if (IsGeneralSettings() ||
            m_projectLanguage == static_cast<int>(readability::test_language::english_test))
            {
            auto* panelSizer = new wxBoxSizer(wxVERTICAL);
            scoreTestOptionsPage->SetSizer(panelSizer);
            m_sideBar->AddSubPage(scoreTestOptionsPage, GetTestOptionsLabel(),
                                  SCORES_TEST_OPTIONS_PAGE, false, 9);

            auto* pgMan = new wxPropertyGridManager(
                scoreTestOptionsPage, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);
            m_readabilityTestsPropertyGrid = pgMan->AddPage();
            // Fog
            m_readabilityTestsPropertyGrid->Append(new wxPropertyCategory(_DT(L"Gunning Fog")));
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                _DT(L"Gunning Fog"),
                // TRANSLATORS: "Gunning Fog" should not be translated; it is a test name.
                _(L"The options in this section customize how Gunning Fog "
                  "related tests are calculated."));

            m_readabilityTestsPropertyGrid->Append(
                new wxBoolProperty(GetCountIndependentClausesLabel(), wxPG_LABEL,
                                   ((m_readabilityProjectDoc != nullptr) ?
                                        m_readabilityProjectDoc->IsFogUsingSentenceUnits() :
                                        wxGetApp().GetAppOptions()->IsFogUsingSentenceUnits())));
            m_readabilityTestsPropertyGrid->SetPropertyAttribute(GetCountIndependentClausesLabel(),
                                                                 wxPG_BOOL_USE_CHECKBOX, true);
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                GetCountIndependentClausesLabel(),
                // TRANSLATORS: "Gunning Fog" should not be translated; it is a test name.
                _(L"Check this option to count independent clauses as separate "
                  "sentences when calculating a Gunning Fog score. Independent clauses "
                  "are detected by dashes, colons, and semicolons "
                  "within a sentence."));
            // Flesch
            m_readabilityTestsPropertyGrid->Append(new wxPropertyCategory(_DT(L"Flesch")));
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                _DT(L"Flesch"), _(L"The options in this section customize how Flesch "
                                  "related tests are calculated."));

            wxPGChoices fleschNumeralMethods;
            fleschNumeralMethods.Add(_(L"Count numerals as one syllable"));
            fleschNumeralMethods.Add(_(L"System default*"));
            m_readabilityTestsPropertyGrid->Append(new wxEnumProperty(
                GetNumeralSyllabicationLabel(), GetFleschNumeralSyllabicationLabel(),
                fleschNumeralMethods,
                ((m_readabilityProjectDoc != nullptr) ?
                     static_cast<int>(m_readabilityProjectDoc->GetFleschNumeralSyllabizeMethod()) :
                     static_cast<int>(
                         wxGetApp().GetAppOptions()->GetFleschNumeralSyllabizeMethod()))));
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                GetFleschNumeralSyllabicationLabel(),
                _(L"Controls how numerals are syllabized for Flesch related tests."));
            m_readabilityTestsPropertyGrid->EnableProperty(
                GetFleschNumeralSyllabicationLabel(),
                (m_textExclusionMethod ==
                 static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
                    !m_excludeNumerals);
            // Flesch-Kincaid
            m_readabilityTestsPropertyGrid->Append(new wxPropertyCategory(_DT(L"Flesch-Kincaid")));
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                _DT(L"Flesch-Kincaid"),
                _(L"The options in this section customize how Flesch-Kincaid is calculated."));

            wxPGChoices fleschKincaidNumeralMethods;
            fleschKincaidNumeralMethods.Add(_(L"Sound out each digit"));
            fleschKincaidNumeralMethods.Add(_(L"System default*"));
            m_readabilityTestsPropertyGrid->Append(new wxEnumProperty(
                GetNumeralSyllabicationLabel(), GetFleschKincaidNumeralSyllabicationLabel(),
                fleschKincaidNumeralMethods,
                ((m_readabilityProjectDoc != nullptr) ?
                     static_cast<int>(
                         m_readabilityProjectDoc->GetFleschKincaidNumeralSyllabizeMethod()) :
                     static_cast<int>(
                         wxGetApp().GetAppOptions()->GetFleschKincaidNumeralSyllabizeMethod()))));
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                GetFleschKincaidNumeralSyllabicationLabel(),
                _(L"Controls how numerals are syllabized for Flesch-Kincaid."));
            m_readabilityTestsPropertyGrid->EnableProperty(
                GetFleschKincaidNumeralSyllabicationLabel(),
                (m_textExclusionMethod ==
                 static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
                    !m_excludeNumerals);
            // DC options
            m_readabilityTestsPropertyGrid->Append(new wxPropertyCategory(_DT(L"Dale-Chall")));
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                _DT(L"Dale-Chall"), _(L"The options in this section customize how Dale-Chall "
                                      "related tests are calculated."));

            m_readabilityTestsPropertyGrid->Append(new wxBoolProperty(
                GetIncludeStockerLabel(), wxPG_LABEL,
                ((m_readabilityProjectDoc != nullptr) ?
                     m_readabilityProjectDoc->IsIncludingStockerCatholicSupplement() :
                     wxGetApp().GetAppOptions()->IsIncludingStockerCatholicSupplement())));
            m_readabilityTestsPropertyGrid->SetPropertyAttribute(GetIncludeStockerLabel(),
                                                                 wxPG_BOOL_USE_CHECKBOX, true);
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                GetIncludeStockerLabel(),
                _(L"Check this option to include Stocker's supplementary word "
                  "list for Catholic students."));

            wxPGChoices properNounMethods;
            properNounMethods.Add(_(L"Count as unfamiliar"));
            properNounMethods.Add(_(L"Count as familiar"));
            properNounMethods.Add(_(L"First occurrence of each is unfamiliar"));
            m_readabilityTestsPropertyGrid->Append(new wxEnumProperty(
                GetProperNounsLabel(), wxPG_LABEL, properNounMethods,
                ((m_readabilityProjectDoc != nullptr) ?
                     static_cast<int>(
                         m_readabilityProjectDoc->GetDaleChallProperNounCountingMethod()) :
                     static_cast<int>(
                         wxGetApp().GetAppOptions()->GetDaleChallProperNounCountingMethod()))));
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                GetProperNounsLabel(),
                _(L"Controls how proper nouns are treated by Dale-Chall related tests."));

            wxPGChoices textExclusionMethods;
            textExclusionMethods.Add(_(L"Exclude incomplete sentences, except headings"));
            textExclusionMethods.Add(_(L"System default**"));
            m_readabilityTestsPropertyGrid->Append(new wxEnumProperty(
                GetTextExclusionLabel(), GetDCTextExclusionLabel(), textExclusionMethods,
                ((m_readabilityProjectDoc != nullptr) ?
                     static_cast<int>(m_readabilityProjectDoc->GetDaleChallTextExclusionMode()) :
                     static_cast<int>(
                         wxGetApp().GetAppOptions()->GetDaleChallTextExclusionMode()))));
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                GetDCTextExclusionLabel(),
                _(L"Controls how text is excluded in Dale-Chall related tests."));
            // HJ options
            m_readabilityTestsPropertyGrid->Append(new wxPropertyCategory(_DT(L"Harris-Jacobson")));
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                _DT(L"Harris-Jacobson"),
                _(L"The options in this section customize how Harris-Jacobson is calculated."));

            m_readabilityTestsPropertyGrid->Append(new wxEnumProperty(
                GetTextExclusionLabel(), GetHJCTextExclusionLabel(), textExclusionMethods,
                ((m_readabilityProjectDoc != nullptr) ?
                     static_cast<int>(
                         m_readabilityProjectDoc->GetHarrisJacobsonTextExclusionMode()) :
                     static_cast<int>(
                         wxGetApp().GetAppOptions()->GetHarrisJacobsonTextExclusionMode()))));
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                GetHJCTextExclusionLabel(),
                _(L"Controls how text is excluded in Harris-Jacobson."));

            pgMan->SelectProperty(_DT(L"Gunning Fog"));
            panelSizer->Add(pgMan, wxSizerFlags{ 1 }.Expand());

            const int scaledNoteWidth = FromDIP(wxSize{ 400, 400 }).GetWidth();

            m_readTestsSyllableLabel = new wxStaticText(
                scoreTestOptionsPage, wxID_STATIC,
                (m_syllabicationMethod ==
                 static_cast<int>(NumeralSyllabize::WholeWordIsOneSyllable)) ?
                    _(L"* Numeral syllabication system default: numerals are one syllable.") :
                    _(L"* Numeral syllabication system default: sound out each digit."),
                wxDefaultPosition, wxDefaultSize, 0);
            m_readTestsSyllableLabel->Wrap(scaledNoteWidth);
            m_readTestsSyllableLabel->Enable(
                (m_textExclusionMethod ==
                 static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
                !m_excludeNumerals);
            panelSizer->Add(m_readTestsSyllableLabel, wxSizerFlags{}.Border(wxLEFT | wxRIGHT));

            m_textExclusionLabel = new wxStaticText(
                scoreTestOptionsPage, wxID_STATIC,
                (m_textExclusionMethod ==
                 static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ?
                    _(L"** Text exclusion system default: do not exclude any text.") :
                (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ?
                    _(L"** Text exclusion system default: exclude all incomplete sentences.") :
                    _(L"** Text exclusion system default: "
                      "exclude all incomplete sentences, except headings."),
                wxDefaultPosition, wxDefaultSize, 0);
            m_textExclusionLabel->Wrap(scaledNoteWidth);
            panelSizer->Add(m_textExclusionLabel, wxSizerFlags{}.Border(wxLEFT | wxRIGHT));
            }

            // grade level tab
            {
            auto* panel = new wxPanel(m_sideBar, SCORES_DISPLAY_PAGE, wxDefaultPosition,
                                      wxDefaultSize, wxTAB_TRAVERSAL);
            auto* panelSizer = new wxBoxSizer(wxVERTICAL);
            panel->SetSizer(panelSizer);
            m_sideBar->AddSubPage(panel, GetScoreDisplayLabel(), SCORES_DISPLAY_PAGE, false, 9);

            auto* pgMan = new wxPropertyGridManager(
                panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);
            m_gradeLevelPropertyGrid = pgMan->AddPage();

            // Results
            m_gradeLevelPropertyGrid->Append(new wxPropertyCategory(GetScoreResultsLabel()));
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetScoreResultsLabel(), _(L"The options in this section customize which results to "
                                          "display in the Scores section."));

            m_gradeLevelPropertyGrid->Append(new wxBoolProperty(
                GetIncludeScoreSummaryLabel(), wxPG_LABEL,
                ((m_readabilityProjectDoc != nullptr) ?
                     m_readabilityProjectDoc->IsIncludingScoreSummaryReport() :
                     wxGetApp().GetAppOptions()->IsIncludingScoreSummaryReport())));
            m_gradeLevelPropertyGrid->SetPropertyAttribute(GetIncludeScoreSummaryLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetIncludeScoreSummaryLabel(),
                _(L"Check this option to include a summary report of the "
                  "test scores in the results."));

            // grade display
            m_gradeLevelPropertyGrid->Append(new wxPropertyCategory(GetGradeLabel()));
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetGradeLabel(),
                _(L"The options in this section customize how the grade scores are displayed."));

            wxPGChoices gradeLevelOption;
            gradeLevelOption.Add(
                _(L"K-12+ (United States of America)"),
                static_cast<int>(readability::grade_scale::k12_plus_united_states));
            gradeLevelOption.Add(
                _(L"K-12+ (Newfoundland and Labrador)"),
                static_cast<int>(readability::grade_scale::k12_plus_newfoundland_and_labrador));
            gradeLevelOption.Add(
                _(L"K-12+ (British Columbia/Yukon)"),
                static_cast<int>(readability::grade_scale::k12_plus_british_columbia));
            gradeLevelOption.Add(_(L"K-12+ (New Brunswick)"),
                                 static_cast<int>(readability::grade_scale::k12_plus_newbrunswick));
            gradeLevelOption.Add(_(L"K-12+ (Nova Scotia)"),
                                 static_cast<int>(readability::grade_scale::k12_plus_nova_scotia));
            gradeLevelOption.Add(_(L"K-12+ (Ontario)"),
                                 static_cast<int>(readability::grade_scale::k12_plus_ontario));
            gradeLevelOption.Add(_(L"K-12+ (Saskatchewan)"),
                                 static_cast<int>(readability::grade_scale::k12_plus_saskatchewan));
            gradeLevelOption.Add(
                _(L"K-12+ (Prince Edward Island)"),
                static_cast<int>(readability::grade_scale::k12_plus_prince_edward_island));
            gradeLevelOption.Add(_(L"K-12+ (Manitoba)"),
                                 static_cast<int>(readability::grade_scale::k12_plus_manitoba));
            gradeLevelOption.Add(
                _(L"K-12+ (Northwest Territories)"),
                static_cast<int>(readability::grade_scale::k12_plus_northwest_territories));
            gradeLevelOption.Add(_(L"K-12+ (Alberta)"),
                                 static_cast<int>(readability::grade_scale::k12_plus_alberta));
            gradeLevelOption.Add(_(L"K-12+ (Nunavut)"),
                                 static_cast<int>(readability::grade_scale::k12_plus_nunavut));
            gradeLevelOption.Add(_DT(L"Quebec"),
                                 static_cast<int>(readability::grade_scale::quebec));
            gradeLevelOption.Add(
                _DT(L"Key stages (England & Wales)", DTExplanation::ProperNoun),
                static_cast<int>(readability::grade_scale::key_stages_england_wales));
            m_gradeLevelPropertyGrid->Append(new wxEnumProperty(
                GetGradeScaleLabel(), wxPG_LABEL, gradeLevelOption,
                ((m_readabilityProjectDoc != nullptr) ?
                     static_cast<int>(
                         m_readabilityProjectDoc->GetReadabilityMessageCatalog().GetGradeScale()) :
                     static_cast<int>(wxGetApp()
                                          .GetAppOptions()
                                          ->GetReadabilityMessageCatalog()
                                          .GetGradeScale()))));
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetGradeScaleLabel(),
                _(L"Select from this list the grade scale that you wish to use."));

            m_gradeLevelPropertyGrid->Append(
                new wxBoolProperty(GetGradesLongFormatLabel(), wxPG_LABEL,
                                   ((m_readabilityProjectDoc != nullptr) ?
                                        m_readabilityProjectDoc->GetReadabilityMessageCatalog()
                                            .IsUsingLongGradeScaleFormat() :
                                        wxGetApp()
                                            .GetAppOptions()
                                            ->GetReadabilityMessageCatalog()
                                            .IsUsingLongGradeScaleFormat())));
            m_gradeLevelPropertyGrid->SetPropertyAttribute(GetGradesLongFormatLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetGradesLongFormatLabel(),
                _(L"Check this to display readability scores in long format "
                  "(e.g., \"2nd grade\") within the score grid."));

            m_gradeLevelPropertyGrid->Append(new wxPropertyCategory(GetReadingAgeLabel()));
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetReadingAgeLabel(),
                _(L"The options in this section customize how reading ages are displayed."));
            // reading display
            wxPGChoices readingAgeDisplay;
            readingAgeDisplay.Add(_(L"School-year range"));
            readingAgeDisplay.Add(_(L"Round to semester"));
            m_gradeLevelPropertyGrid->Append(new wxEnumProperty(
                GetCalculationLabel(), wxPG_LABEL, readingAgeDisplay,
                ((m_readabilityProjectDoc != nullptr) ?
                     static_cast<int>(m_readabilityProjectDoc->GetReadabilityMessageCatalog()
                                          .GetReadingAgeDisplay()) :
                     static_cast<int>(wxGetApp()
                                          .GetAppOptions()
                                          ->GetReadabilityMessageCatalog()
                                          .GetReadingAgeDisplay()))));
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetCalculationLabel(),
                _(L"Selects how reading ages should be calculated from grade scores."));

            pgMan->SelectProperty(GetGradeLabel());

            panelSizer->Add(pgMan, wxSizerFlags{ 1 }.Expand());
            }
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateStatisticsSection()
    {
    if ((GetSectionsBeingShown() & Statistics) != 0)
        {
        auto* panel = new wxPanel(m_sideBar, ANALYSIS_STATISTICS_PAGE, wxDefaultPosition,
                                  wxDefaultSize, wxTAB_TRAVERSAL);
        auto* panelSizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(panelSizer);
        m_sideBar->AddPage(panel, BaseProjectView::GetSummaryStatisticsLabel(),
                           ANALYSIS_STATISTICS_PAGE,
                           // if the only section being shown, then show this page
                           (GetSectionsBeingShown() == Statistics), 2);

        auto* pgMan = new wxPropertyGridManager(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION |
                                                    wxPGMAN_DEFAULT_STYLE);
        m_statisticsPropertyGrid = pgMan->AddPage();
        // Results
        m_statisticsPropertyGrid->Append(new wxPropertyCategory(GetResultsLabel()));
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetResultsLabel(), _(L"The options in this section customize which results to include "
                                 "in the Summary Statistics."));
        m_statisticsPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetFormattedReportLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetStatisticsInfo().IsReportEnabled() :
                 wxGetApp().GetAppOptions()->GetStatisticsInfo().IsReportEnabled())));
        m_statisticsPropertyGrid->SetPropertyAttribute(BaseProjectView::GetFormattedReportLabel(),
                                                       wxPG_BOOL_USE_CHECKBOX, true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetFormattedReportLabel(),
            _(L"Check this to include the statistics report in the results."));
        m_statisticsPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetTabularReportLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetStatisticsInfo().IsTableEnabled() :
                 wxGetApp().GetAppOptions()->GetStatisticsInfo().IsTableEnabled())));
        m_statisticsPropertyGrid->SetPropertyAttribute(BaseProjectView::GetTabularReportLabel(),
                                                       wxPG_BOOL_USE_CHECKBOX, true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetTabularReportLabel(),
            _(L"Check this to include the statistics (tabular format) in the results."));
        // Report
        m_statisticsPropertyGrid->Append(new wxPropertyCategory(GetStatisticsReportLabel()));
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetStatisticsReportLabel(),
            _(L"The options in this section customize which statistics are "
              "included in the report and results."));
        m_statisticsPropertyGrid->Append(new wxBoolProperty(
            GetParagraphsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetStatisticsReportInfo().IsParagraphEnabled() :
                 wxGetApp().GetAppOptions()->GetStatisticsReportInfo().IsParagraphEnabled())));
        m_statisticsPropertyGrid->SetPropertyAttribute(GetParagraphsLabel(), wxPG_BOOL_USE_CHECKBOX,
                                                       true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetParagraphsLabel(),
            _(L"Check this to include the paragraph statistics in the report."));
        m_statisticsPropertyGrid->Append(new wxBoolProperty(
            GetSentencesLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetStatisticsReportInfo().IsSentencesEnabled() :
                 wxGetApp().GetAppOptions()->GetStatisticsReportInfo().IsSentencesEnabled())));
        m_statisticsPropertyGrid->SetPropertyAttribute(GetSentencesLabel(), wxPG_BOOL_USE_CHECKBOX,
                                                       true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetSentencesLabel(),
            _(L"Check this to include the sentence statistics in the report."));
        m_statisticsPropertyGrid->Append(new wxBoolProperty(
            GetWordsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetStatisticsReportInfo().IsWordsEnabled() :
                 wxGetApp().GetAppOptions()->GetStatisticsReportInfo().IsWordsEnabled())));
        m_statisticsPropertyGrid->SetPropertyAttribute(GetWordsLabel(), wxPG_BOOL_USE_CHECKBOX,
                                                       true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetWordsLabel(), _(L"Check this to include the word statistics in the report."));
        m_statisticsPropertyGrid->Append(new wxBoolProperty(
            GetExtendedWordsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetStatisticsReportInfo().IsExtendedWordsEnabled() :
                 wxGetApp().GetAppOptions()->GetStatisticsReportInfo().IsExtendedWordsEnabled())));
        m_statisticsPropertyGrid->SetPropertyAttribute(GetExtendedWordsLabel(),
                                                       wxPG_BOOL_USE_CHECKBOX, true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetExtendedWordsLabel(),
            _(L"Check this to include extended word statistics (e.g., numerals) in the report."));
        m_statisticsPropertyGrid->Append(new wxBoolProperty(
            GetGrammarLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetStatisticsReportInfo().IsGrammarEnabled() :
                 wxGetApp().GetAppOptions()->GetStatisticsReportInfo().IsGrammarEnabled())));
        m_statisticsPropertyGrid->SetPropertyAttribute(GetGrammarLabel(), wxPG_BOOL_USE_CHECKBOX,
                                                       true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetGrammarLabel(), _(L"Check this to include grammar statistics in the report."));
        m_statisticsPropertyGrid->Append(new wxBoolProperty(
            GetNotesLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetStatisticsReportInfo().IsNotesEnabled() :
                 wxGetApp().GetAppOptions()->GetStatisticsReportInfo().IsNotesEnabled())));
        m_statisticsPropertyGrid->SetPropertyAttribute(GetNotesLabel(), wxPG_BOOL_USE_CHECKBOX,
                                                       true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetNotesLabel(), _(L"Check this to include the notes in the report."));
        m_statisticsPropertyGrid->Append(new wxBoolProperty(
            GetExtendedInformationLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetStatisticsReportInfo().IsExtendedInformationEnabled() :
                 wxGetApp()
                     .GetAppOptions()
                     ->GetStatisticsReportInfo()
                     .IsExtendedInformationEnabled())));
        m_statisticsPropertyGrid->SetPropertyAttribute(GetExtendedInformationLabel(),
                                                       wxPG_BOOL_USE_CHECKBOX, true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetExtendedInformationLabel(),
            _(L"Check this to include more verbose statistics and notes in the results."));
        // Dolch report options
        if (m_projectLanguage == static_cast<int>(readability::test_language::english_test))
            {
            m_statisticsPropertyGrid->Append(
                new wxPropertyCategory(GetDolchStatisticsReportLabel()));
            m_statisticsPropertyGrid->SetPropertyHelpString(
                GetDolchStatisticsReportLabel(),
                _(L"The options in this section customize which statistics "
                  "are included in the Dolch report."));
            m_statisticsPropertyGrid->Append(new wxBoolProperty(
                GetCoverageLabel(), wxPG_LABEL,
                ((m_readabilityProjectDoc != nullptr) ?
                     m_readabilityProjectDoc->GetStatisticsReportInfo().IsDolchCoverageEnabled() :
                     wxGetApp()
                         .GetAppOptions()
                         ->GetStatisticsReportInfo()
                         .IsDolchCoverageEnabled())));
            m_statisticsPropertyGrid->SetPropertyAttribute(GetCoverageLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
            m_statisticsPropertyGrid->SetPropertyHelpString(
                GetCoverageLabel(),
                _(L"Check this to include the word coverage statistics in the report."));
            m_statisticsPropertyGrid->Append(new wxBoolProperty(
                GetWordsLabel(), GetDolchWordsLabel(),
                ((m_readabilityProjectDoc != nullptr) ?
                     m_readabilityProjectDoc->GetStatisticsReportInfo().IsDolchWordsEnabled() :
                     wxGetApp().GetAppOptions()->GetStatisticsReportInfo().IsDolchWordsEnabled())));
            m_statisticsPropertyGrid->SetPropertyAttribute(GetDolchWordsLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
            m_statisticsPropertyGrid->SetPropertyHelpString(
                GetDolchWordsLabel(),
                _(L"Check this to include the word statistics in the report."));
            m_statisticsPropertyGrid->Append(
                new wxBoolProperty(GetDolchExplanationLabel(), wxPG_LABEL,
                                   ((m_readabilityProjectDoc != nullptr) ?
                                        m_readabilityProjectDoc->GetStatisticsReportInfo()
                                            .IsDolchExplanationEnabled() :
                                        wxGetApp()
                                            .GetAppOptions()
                                            ->GetStatisticsReportInfo()
                                            .IsDolchExplanationEnabled())));
            m_statisticsPropertyGrid->SetPropertyAttribute(GetDolchExplanationLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
            m_statisticsPropertyGrid->SetPropertyHelpString(
                GetDolchExplanationLabel(),
                _(L"Check this to include the Dolch explanation in the report."));
            }

        pgMan->SelectProperty(GetStatisticsReportLabel());

        panelSizer->Add(pgMan, wxSizerFlags{ 1 }.Expand());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateWordsBreakdownSection()
    {
    if (((GetSectionsBeingShown() & WordsBreakdown) != 0) && !IsBatchProjectSettings())
        {
        auto* panel = new wxPanel(m_sideBar, WORDS_BREAKDOWN_PAGE, wxDefaultPosition, wxDefaultSize,
                                  wxTAB_TRAVERSAL);
        auto* panelSizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(panelSizer);
        m_sideBar->AddPage(panel, BaseProjectView::GetWordsBreakdownLabel(), WORDS_BREAKDOWN_PAGE,
                           // if the only section being shown, then show this page
                           (GetSectionsBeingShown() == WordsBreakdown), 13);

        auto* pgMan = new wxPropertyGridManager(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION |
                                                    wxPGMAN_DEFAULT_STYLE);
        m_wordsBreakdownPropertyGrid = pgMan->AddPage();

        // which features to include
        m_wordsBreakdownPropertyGrid->Append(new wxPropertyCategory(GetResultsLabel()));
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            GetResultsLabel(), _(L"The options on this page customize which results are "
                                 "included in the Words Breakdown section"));

        m_wordsBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetWordCountsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetWordsBreakdownInfo().IsWordBarchartEnabled() :
                 wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().IsWordBarchartEnabled())));
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(BaseProjectView::GetWordCountsLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetWordCountsLabel(),
            _(L"Check this to include the word barchart in the results."));

        m_wordsBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetSyllableCountsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetWordsBreakdownInfo().IsSyllableGraphsEnabled() :
                 wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().IsSyllableGraphsEnabled())));
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetSyllableCountsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSyllableCountsLabel(),
            _(L"Check this to include the syllable histogram and pie chart in the results."));

        m_wordsBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetThreeSyllableWordsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetWordsBreakdownInfo().Is3PlusSyllablesEnabled() :
                 wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().Is3PlusSyllablesEnabled())));
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetThreeSyllableWordsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetThreeSyllableWordsLabel(),
            _(L"Check this to include the 3+ syllable words list in the results."));

        m_wordsBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetSixCharWordsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetWordsBreakdownInfo().Is6PlusCharacterEnabled() :
                 wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().Is6PlusCharacterEnabled())));
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(BaseProjectView::GetSixCharWordsLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSixCharWordsLabel(),
            _(L"Check this to include the 6+ character words in the results."));

        m_wordsBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetWordCloudLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetWordsBreakdownInfo().IsWordCloudEnabled() :
                 wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().IsWordCloudEnabled())));
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(BaseProjectView::GetWordCloudLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetWordCloudLabel(),
            _(L"Check this to include the key word cloud in the results."));

        m_wordsBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetDaleChallLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetWordsBreakdownInfo().IsDCUnfamiliarEnabled() :
                 wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().IsDCUnfamiliarEnabled())));
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(BaseProjectView::GetDaleChallLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetDaleChallLabel(),
            _(L"Check this to include the Dale-Chall unfamiliar words list in the results. "
              "(If a Dale-Chall test is included in the project)."));

        m_wordsBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetSpacheLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetWordsBreakdownInfo().IsSpacheUnfamiliarEnabled() :
                 wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().IsSpacheUnfamiliarEnabled())));
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(BaseProjectView::GetSpacheLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSpacheLabel(),
            _(L"Check this to include the Spache unfamiliar words list in the results. "
              "(If Spache is included in the project)."));

        m_wordsBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetHarrisJacobsonLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ? m_readabilityProjectDoc->GetWordsBreakdownInfo()
                                                        .IsHarrisJacobsonUnfamiliarEnabled() :
                                                    wxGetApp()
                                                        .GetAppOptions()
                                                        ->GetWordsBreakdownInfo()
                                                        .IsHarrisJacobsonUnfamiliarEnabled())));
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetHarrisJacobsonLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetHarrisJacobsonLabel(),
            _(L"Check this to include the Harris-Jacobson unfamiliar words list in the results. "
              "(If Harris-Jacobson is included in the project)."));

        m_wordsBreakdownPropertyGrid->Append(new wxBoolProperty(
            GetCustomTestsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetWordsBreakdownInfo().IsCustomTestsUnfamiliarEnabled() :
                 wxGetApp()
                     .GetAppOptions()
                     ->GetWordsBreakdownInfo()
                     .IsCustomTestsUnfamiliarEnabled())));
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(GetCustomTestsLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            GetCustomTestsLabel(),
            _(L"Check this to include any custom tests' unfamiliar words lists in the results."));

        m_wordsBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetAllWordsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetWordsBreakdownInfo().IsAllWordsEnabled() :
                 wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().IsAllWordsEnabled())));
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(BaseProjectView::GetAllWordsLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetAllWordsLabel(),
            _(L"Check this to include a list of all words in the results."));

        m_wordsBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetKeyWordsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetWordsBreakdownInfo().IsKeyWordsEnabled() :
                 wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().IsKeyWordsEnabled())));
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(BaseProjectView::GetKeyWordsLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetKeyWordsLabel(),
            _(L"Check this to include a list of all words (in condensed form) in the results. "
              "Words with the same root (e.g., \"run\" and \"running\") will be "
              "combined into one row."));

        pgMan->SelectProperty(GetResultsLabel());
        pgMan->SetDescBoxHeight(FromDIP(wxSize{ 200, 200 }).GetHeight());

        panelSizer->Add(pgMan, wxSizerFlags{ 1 }.Expand());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateSentencesBreakdownSection()
    {
    if (((GetSectionsBeingShown() & SentencesBreakdown) != 0) && !IsBatchProjectSettings())
        {
        auto* panel = new wxPanel(m_sideBar, SENTENCES_BREAKDOWN_PAGE, wxDefaultPosition,
                                  wxDefaultSize, wxTAB_TRAVERSAL);
        auto* panelSizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(panelSizer);
        m_sideBar->AddPage(panel, BaseProjectView::GetSentencesBreakdownLabel(),
                           SENTENCES_BREAKDOWN_PAGE,
                           // if the only section being shown, then show this page
                           (GetSectionsBeingShown() == SentencesBreakdown), 14);

        auto* pgMan = new wxPropertyGridManager(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION |
                                                    wxPGMAN_DEFAULT_STYLE);
        m_sentencesBreakdownPropertyGrid = pgMan->AddPage();

        // which features to include
        m_sentencesBreakdownPropertyGrid->Append(new wxPropertyCategory(GetResultsLabel()));
        m_sentencesBreakdownPropertyGrid->SetPropertyHelpString(
            GetResultsLabel(), _(L"The options on this page customize which results are included "
                                 "in the Sentences Breakdown section"));

        m_sentencesBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetLongSentencesLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetSentencesBreakdownInfo().IsLongSentencesEnabled() :
                 wxGetApp()
                     .GetAppOptions()
                     ->GetSentencesBreakdownInfo()
                     .IsLongSentencesEnabled())));
        m_sentencesBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetLongSentencesLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_sentencesBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetLongSentencesLabel(),
            _(L"Check this to include the overly-long sentences list in the results."));

        m_sentencesBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetSentenceLengthBoxPlotLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetSentencesBreakdownInfo().IsLengthsBoxPlotEnabled() :
                 wxGetApp()
                     .GetAppOptions()
                     ->GetSentencesBreakdownInfo()
                     .IsLengthsBoxPlotEnabled())));
        m_sentencesBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetSentenceLengthBoxPlotLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_sentencesBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSentenceLengthBoxPlotLabel(),
            _(L"Check this to include the sentence-lengths box plot in the results."));

        m_sentencesBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetSentenceLengthHistogramLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetSentencesBreakdownInfo().IsLengthsHistogramEnabled() :
                 wxGetApp()
                     .GetAppOptions()
                     ->GetSentencesBreakdownInfo()
                     .IsLengthsHistogramEnabled())));
        m_sentencesBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetSentenceLengthHistogramLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_sentencesBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSentenceLengthHistogramLabel(),
            _(L"Check this to include the sentence-lengths histogram in the results."));

        m_sentencesBreakdownPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetSentenceLengthHeatmapLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetSentencesBreakdownInfo().IsLengthsHeatmapEnabled() :
                 wxGetApp()
                     .GetAppOptions()
                     ->GetSentencesBreakdownInfo()
                     .IsLengthsHeatmapEnabled())));
        m_sentencesBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetSentenceLengthHeatmapLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_sentencesBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSentenceLengthHeatmapLabel(),
            _(L"Check this to include the sentence-lengths heatmap in the results."));

        pgMan->SelectProperty(GetResultsLabel());
        pgMan->SetDescBoxHeight(FromDIP(wxSize{ 200, 200 }).GetHeight());

        panelSizer->Add(pgMan, wxSizerFlags{ 1 }.Expand());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateGrammarSection()
    {
    if ((GetSectionsBeingShown() & Grammar) != 0)
        {
        auto* panel =
            new wxPanel(m_sideBar, GRAMMAR_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        auto* panelSizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(panelSizer);
        m_sideBar->AddPage(panel, BaseProjectView::GetGrammarLabel(), GRAMMAR_PAGE,
                           // if the only section being shown, then show this page
                           (GetSectionsBeingShown() == Grammar), 4);

        auto* pgMan = new wxPropertyGridManager(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION |
                                                    wxPGMAN_DEFAULT_STYLE);
        m_grammarPropertyGrid = pgMan->AddPage();

        // which grammar features to include
        m_grammarPropertyGrid->Append(new wxPropertyCategory(GetResultsLabel()));
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetResultsLabel(), _(L"The options in this section customize which features are "
                                 "included in the grammar analysis."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            GetGrammarHighlightedReportLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetGrammarInfo().IsHighlightedReportEnabled() :
                 wxGetApp().GetAppOptions()->GetGrammarInfo().IsHighlightedReportEnabled())));
        m_grammarPropertyGrid->SetPropertyAttribute(GetGrammarHighlightedReportLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetGrammarHighlightedReportLabel(),
            _(L"Check this to include the highlighted grammar report in the results."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetPhrasingErrorsTabLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetGrammarInfo().IsWordingErrorsEnabled() :
                 wxGetApp().GetAppOptions()->GetGrammarInfo().IsWordingErrorsEnabled())));
        m_grammarPropertyGrid->SetPropertyAttribute(BaseProjectView::GetPhrasingErrorsTabLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetPhrasingErrorsTabLabel(),
            _(L"Check this to include wording errors and known misspellings in the results."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetMisspellingsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetGrammarInfo().IsMisspellingsEnabled() :
                 wxGetApp().GetAppOptions()->GetGrammarInfo().IsMisspellingsEnabled())));
        m_grammarPropertyGrid->SetPropertyAttribute(BaseProjectView::GetMisspellingsLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetMisspellingsLabel(),
            _(L"Check this to include possible misspellings in the results."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetRepeatedWordsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetGrammarInfo().IsRepeatedWordsEnabled() :
                 wxGetApp().GetAppOptions()->GetGrammarInfo().IsRepeatedWordsEnabled())));
        m_grammarPropertyGrid->SetPropertyAttribute(BaseProjectView::GetRepeatedWordsLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetRepeatedWordsLabel(),
            _(L"Check this to include repeated words in the results."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetArticleMismatchesLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetGrammarInfo().IsArticleMismatchesEnabled() :
                 wxGetApp().GetAppOptions()->GetGrammarInfo().IsArticleMismatchesEnabled())));
        m_grammarPropertyGrid->SetPropertyAttribute(BaseProjectView::GetArticleMismatchesLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetArticleMismatchesLabel(),
            _(L"Check this to include article mismatches in the results."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetRedundantPhrasesTabLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetGrammarInfo().IsRedundantPhrasesEnabled() :
                 wxGetApp().GetAppOptions()->GetGrammarInfo().IsRedundantPhrasesEnabled())));
        m_grammarPropertyGrid->SetPropertyAttribute(BaseProjectView::GetRedundantPhrasesTabLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetRedundantPhrasesTabLabel(),
            _(L"Check this to include redundant phrases in the results."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetOverusedWordsBySentenceLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetGrammarInfo().IsOverUsedWordsBySentenceEnabled() :
                 wxGetApp().GetAppOptions()->GetGrammarInfo().IsOverUsedWordsBySentenceEnabled())));
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetOverusedWordsBySentenceLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetOverusedWordsBySentenceLabel(),
            _(L"Check this to include overused words (by sentence) in the results."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetWordyPhrasesTabLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetGrammarInfo().IsWordyPhrasesEnabled() :
                 wxGetApp().GetAppOptions()->GetGrammarInfo().IsWordyPhrasesEnabled())));
        m_grammarPropertyGrid->SetPropertyAttribute(BaseProjectView::GetWordyPhrasesTabLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetWordyPhrasesTabLabel(),
            _(L"Check this to include wordy phrases in the results."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetClichesTabLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetGrammarInfo().IsClichesEnabled() :
                 wxGetApp().GetAppOptions()->GetGrammarInfo().IsClichesEnabled())));
        m_grammarPropertyGrid->SetPropertyAttribute(BaseProjectView::GetClichesTabLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetClichesTabLabel(),
            _(L"Check this to include clichés in the results."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetPassiveLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetGrammarInfo().IsPassiveVoiceEnabled() :
                 wxGetApp().GetAppOptions()->GetGrammarInfo().IsPassiveVoiceEnabled())));
        m_grammarPropertyGrid->SetPropertyAttribute(BaseProjectView::GetPassiveLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetPassiveLabel(),
            _(L"Check this to include passive voice in the results."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetGrammarInfo().IsConjunctionStartingSentencesEnabled() :
                 wxGetApp()
                     .GetAppOptions()
                     ->GetGrammarInfo()
                     .IsConjunctionStartingSentencesEnabled())));
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel(), wxPG_BOOL_USE_CHECKBOX,
            true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel(),
            _(L"Check this to include conjunction-starting sentences in the results."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            BaseProjectView::GetSentenceStartingWithLowercaseTabLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->GetGrammarInfo().IsLowercaseSentencesEnabled() :
                 wxGetApp().GetAppOptions()->GetGrammarInfo().IsLowercaseSentencesEnabled())));
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetSentenceStartingWithLowercaseTabLabel(), wxPG_BOOL_USE_CHECKBOX,
            true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSentenceStartingWithLowercaseTabLabel(),
            _(L"Check this to include lowercased sentences in the results."));

        // spell checker section
        m_grammarPropertyGrid->Append(new wxPropertyCategory(GetSpellCheckerLabel()));
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetSpellCheckerLabel(),
            _(L"The options in this section select what the spell checker should ignore."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            GetIgnoreProperNounsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->SpellCheckIsIgnoringProperNouns() :
                 wxGetApp().GetAppOptions()->SpellCheckIsIgnoringProperNouns())));
        m_grammarPropertyGrid->SetPropertyAttribute(GetIgnoreProperNounsLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetIgnoreProperNounsLabel(),
            _(L"Check this to not spell check proper nouns. This is useful for excluding names "
              "and places that the spell checker may not recognize."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(GetIgnoreUppercasedWordsLabel(), wxPG_LABEL,
                               ((m_readabilityProjectDoc != nullptr) ?
                                    m_readabilityProjectDoc->SpellCheckIsIgnoringUppercased() :
                                    wxGetApp().GetAppOptions()->SpellCheckIsIgnoringUppercased())));
        m_grammarPropertyGrid->SetPropertyAttribute(GetIgnoreUppercasedWordsLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetIgnoreUppercasedWordsLabel(), _(L"Check this to not spell check uppercased words "
                                               "(e.g., \"FORTRAN\" or \"S.C.U.B.A.\")."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(GetIgnoreNumeralsLabel(), wxPG_LABEL,
                               ((m_readabilityProjectDoc != nullptr) ?
                                    m_readabilityProjectDoc->SpellCheckIsIgnoringNumerals() :
                                    wxGetApp().GetAppOptions()->SpellCheckIsIgnoringNumerals())));
        m_grammarPropertyGrid->SetPropertyAttribute(GetIgnoreNumeralsLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetIgnoreNumeralsLabel(),
            _(L"Check this to not spell check numerals. "
              "Numerals are words that consist of 50% (or more) numbers."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            GetIgnoreFileAddressesLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->SpellCheckIsIgnoringFileAddresses() :
                 wxGetApp().GetAppOptions()->SpellCheckIsIgnoringFileAddresses())));
        m_grammarPropertyGrid->SetPropertyAttribute(GetIgnoreFileAddressesLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetIgnoreFileAddressesLabel(),
            _(L"Check this to not spell check file paths and website addresses."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            GetIgnoreProgrammerCodeLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->SpellCheckIsIgnoringProgrammerCode() :
                 wxGetApp().GetAppOptions()->SpellCheckIsIgnoringProgrammerCode())));
        m_grammarPropertyGrid->SetPropertyAttribute(GetIgnoreProgrammerCodeLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetIgnoreProgrammerCodeLabel(),
            _(L"Check this to not spell check words with mixed upper and lower case words. "
              "This is useful for excluding programming syntax which may appear "
              "in computer-related documents."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            GetAllowColloquialismsLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->SpellCheckIsAllowingColloquialisms() :
                 wxGetApp().GetAppOptions()->SpellCheckIsAllowingColloquialisms())));
        m_grammarPropertyGrid->SetPropertyAttribute(GetAllowColloquialismsLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetAllowColloquialismsLabel(), _(L"Check this to allow colloquialisms, such as \"ing\" "
                                             "at the end of a word being replaced with \"in'\"."));

        m_grammarPropertyGrid->Append(new wxBoolProperty(
            GetIgnoreSocialMediaLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 m_readabilityProjectDoc->SpellCheckIsIgnoringSocialMediaTags() :
                 wxGetApp().GetAppOptions()->SpellCheckIsIgnoringSocialMediaTags())));
        m_grammarPropertyGrid->SetPropertyAttribute(GetIgnoreSocialMediaLabel(),
                                                    wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetIgnoreSocialMediaLabel(),
            _(L"Check this to ignore hashtags, such as #ReadabilityFormulasRock."));

        pgMan->SelectProperty(GetSpellCheckerLabel());
        pgMan->SetDescBoxHeight(FromDIP(wxSize(200, 200)).GetHeight());

        panelSizer->Add(pgMan, wxSizerFlags{ 1 }.Expand());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateTextWindowSection()
    {
    const auto fontImage = wxArtProvider::GetBitmapBundle(L"ID_FONT", wxART_BUTTON);
    const int optionIndentSize = wxSizerFlags::GetDefaultBorder() * 3;

    if (((GetSectionsBeingShown() & TextSection) != 0) && !IsBatchProjectSettings())
        {
        auto* panel = new wxPanel(m_sideBar, DOCUMENT_DISPLAY_GENERAL_PAGE, wxDefaultPosition,
                                  wxDefaultSize, wxTAB_TRAVERSAL);

        m_sideBar->AddPage(panel, BaseProjectView::GetHighlightedReportsLabel(),
                           DOCUMENT_DISPLAY_GENERAL_PAGE,
                           // if the only section being shown, then show this page
                           (GetSectionsBeingShown() == TextSection), 0);

        auto* panelSizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(panelSizer);
        m_sideBar->AddSubPage(panel, GetGeneralLabel(), DOCUMENT_DISPLAY_GENERAL_PAGE, false, 9);

        CreateLabelHeader(panel, panelSizer, _(L"Formatting"), true);

        // sizer associated with static box holding controls
        auto* optionsSizer = new wxBoxSizer(wxVERTICAL);
        panelSizer->Add(optionsSizer, 0, wxLEFT, optionIndentSize);

        // font
        m_FontButton = new wxButton(panel, ID_FONT_BUTTON, _(L"Font"));
        m_FontButton->SetBitmap(fontImage);
        optionsSizer->Add(m_FontButton, 0, wxALIGN_TOP);

        optionsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        CreateLabelHeader(panel, panelSizer,
                          // TRANSLATORS: How text is highlighted
                          _(L"Highlighting"), true);

        auto* optionsIndentSizer = new wxBoxSizer(wxVERTICAL);
        panelSizer->Add(optionsIndentSizer, 0, wxLEFT, optionIndentSize);

            // highlight color for excluded text
            {
            m_excludedHighlightColorButton =
                new wxButton(panel, ID_EXCLUDED_HIGHLIGHT_COLOR_BUTTON, _(L"Excluded text color"));
            m_excludedHighlightColorButton->SetBitmap(
                ResourceManager::CreateColorIcon(m_excludedTextHighlightColor.get_value()));
            optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            optionsIndentSizer->Add(m_excludedHighlightColorButton, wxSizerFlags{}.Expand());
            }

            // highlight color
            {
            m_highlightColorButton = new wxButton(panel, ID_HIGHLIGHT_COLOR_BUTTON,
                                                  _(L"Difficult words and sentences color"));
            m_highlightColorButton->SetBitmap(
                ResourceManager::CreateColorIcon(m_highlightedColor.get_value()));
            optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            optionsIndentSizer->Add(m_highlightColorButton, wxSizerFlags{}.Expand());
            }

            // highlight color for grammar errors
            {
            m_duplicateWordHighlightColorButton =
                new wxButton(panel, ID_DUP_WORD_COLOR_BUTTON, _(L"Grammar errors color"));
            m_duplicateWordHighlightColorButton->SetBitmap(
                ResourceManager::CreateColorIcon(m_duplicateWordHighlightColor.get_value()));
            optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            optionsIndentSizer->Add(m_duplicateWordHighlightColorButton, wxSizerFlags{}.Expand());
            }

            // highlight color for wordy items
            {
            m_wordyPhraseHighlightColorButton =
                new wxButton(panel, ID_WORDY_PHRASE_COLOR_BUTTON, _(L"Wordy items color"));
            m_wordyPhraseHighlightColorButton->SetBitmap(
                ResourceManager::CreateColorIcon(m_wordyPhraseHighlightColor.get_value()));
            optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            optionsIndentSizer->Add(m_wordyPhraseHighlightColorButton, wxSizerFlags{}.Expand());
            }

        auto* highlightSizer = new wxBoxSizer(wxHORIZONTAL);
        highlightSizer->Add(
            new wxStaticText(panel, wxID_STATIC, _(L"Highlight text by changing its:")),
            wxSizerFlags{}.Border(wxRIGHT).CenterVertical());

        wxArrayString highlightOptions;
        highlightOptions.Add(_(L"Background color"));
        highlightOptions.Add(_(L"Font color"));
        auto* highlightCombo = new wxChoice(panel, ID_PARAGRAPH_PARSE, wxDefaultPosition,
                                            wxDefaultSize, highlightOptions, 0,
                                            wxGenericValidator(&m_textHighlightMethod.get_value()));
        highlightSizer->Add(highlightCombo, wxSizerFlags{}.Border(wxRIGHT).CenterVertical());
        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
        panelSizer->Add(highlightSizer, wxSizerFlags{}.Border(wxLEFT, optionIndentSize));

        // dolch tab
        if (IsGeneralSettings() ||
            m_projectLanguage == static_cast<int>(readability::test_language::english_test))
            {
            panel = new wxPanel(m_sideBar, DOCUMENT_DISPLAY_DOLCH_PAGE, wxDefaultPosition,
                                wxDefaultSize, wxTAB_TRAVERSAL);
            panelSizer = new wxBoxSizer(wxVERTICAL);
            panel->SetSizer(panelSizer);
            m_sideBar->AddSubPage(panel, GetDolchSightWordsLabel(), DOCUMENT_DISPLAY_DOLCH_PAGE,
                                  false, 9);

            CreateLabelHeader(panel, panelSizer, _(L"Highlighting"), true);
            panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            // sizer associated with static box holding controls
            optionsIndentSizer = new wxBoxSizer(wxVERTICAL);
            panelSizer->Add(optionsIndentSizer, 0, wxLEFT, optionIndentSize);

                // Conjunctions color
                {
                auto* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, wxSizerFlags{}.Expand());

                m_DolchConjunctionsColorButton = new wxBitmapButton(
                    panel, ID_DOLCH_CONJUNCTIONS_COLOR_BUTTON,
                    ResourceManager::CreateColorIcon(m_dolchConjunctionsColor.get_value()));
                // add them to the sizer
                rowSizer->Add(
                    new wxCheckBox(panel, wxID_ANY, _(L"Dolch conjunctions color:"),
                                   wxDefaultPosition, wxDefaultSize, 0,
                                   wxGenericValidator(&m_highlightDolchConjunctions.get_value())),
                    wxSizerFlags{ 1 }.CenterVertical());
                rowSizer->Add(m_DolchConjunctionsColorButton,
                              wxSizerFlags{}.Align(wxRight).Border(wxLEFT));
                }
                // Prepositions color
                {
                auto* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, wxSizerFlags{}.Expand());

                m_DolchPrepositionsColorButton = new wxBitmapButton(
                    panel, ID_DOLCH_PREPOSITIONS_COLOR_BUTTON,
                    ResourceManager::CreateColorIcon(m_dolchPrepositionsColor.get_value()));
                // add them to the sizer
                rowSizer->Add(
                    new wxCheckBox(panel, wxID_ANY, _(L"Dolch prepositions color:"),
                                   wxDefaultPosition, wxDefaultSize, 0,
                                   wxGenericValidator(&m_highlightDolchPrepositions.get_value())),
                    wxSizerFlags{ 1 }.CenterVertical());
                rowSizer->Add(m_DolchPrepositionsColorButton,
                              wxSizerFlags{}.Align(wxRight).Border(wxLEFT));
                }
                // pronouns
                {
                auto* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, wxSizerFlags{}.Expand());

                m_DolchPronounsColorButton = new wxBitmapButton(
                    panel, ID_DOLCH_PRONOUNS_COLOR_BUTTON,
                    ResourceManager::CreateColorIcon(m_dolchPronounsColor.get_value()));
                // add them to the sizer
                rowSizer->Add(
                    new wxCheckBox(panel, wxID_ANY, _(L"Dolch pronouns color:"), wxDefaultPosition,
                                   wxDefaultSize, 0,
                                   wxGenericValidator(&m_highlightDolchPronouns.get_value())),
                    wxSizerFlags{ 1 }.CenterVertical());
                rowSizer->Add(m_DolchPronounsColorButton,
                              wxSizerFlags{}.Align(wxRight).Border(wxLEFT));
                }
                // adverbs
                {
                auto* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, wxSizerFlags{}.Expand());

                m_DolchAdverbsColorButton = new wxBitmapButton(
                    panel, ID_DOLCH_ADVERBS_COLOR_BUTTON,
                    ResourceManager::CreateColorIcon(m_dolchAdverbsColor.get_value()));
                // add them to the sizer
                rowSizer->Add(
                    new wxCheckBox(panel, wxID_ANY, _(L"Dolch adverbs color:"), wxDefaultPosition,
                                   wxDefaultSize, 0,
                                   wxGenericValidator(&m_highlightDolchAdverbs.get_value())),
                    wxSizerFlags{ 1 }.CenterVertical());
                rowSizer->Add(m_DolchAdverbsColorButton,
                              wxSizerFlags{}.Align(wxRight).Border(wxLEFT));
                }
                // adjectives
                {
                auto* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, wxSizerFlags{}.Expand());

                m_DolchAdjectivesColorButton = new wxBitmapButton(
                    panel, ID_DOLCH_ADJECTIVES_COLOR_BUTTON,
                    ResourceManager::CreateColorIcon(m_dolchAdjectivesColor.get_value()));
                // add them to the sizer
                rowSizer->Add(
                    new wxCheckBox(panel, wxID_ANY, _(L"Dolch adjectives color:"),
                                   wxDefaultPosition, wxDefaultSize, 0,
                                   wxGenericValidator(&m_highlightDolchAdjectives.get_value())),
                    wxSizerFlags{ 1 }.CenterVertical());
                rowSizer->Add(m_DolchAdjectivesColorButton,
                              wxSizerFlags{}.Align(wxRight).Border(wxLEFT));
                }
                // verbs
                {
                auto* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, wxSizerFlags{}.Expand());

                m_DolchVerbsColorButton = new wxBitmapButton(
                    panel, ID_DOLCH_VERBS_COLOR_BUTTON,
                    ResourceManager::CreateColorIcon(m_dolchVerbsColor.get_value()));
                // add them to the sizer
                rowSizer->Add(
                    new wxCheckBox(panel, wxID_ANY, _(L"Dolch verbs color:"), wxDefaultPosition,
                                   wxDefaultSize, 0,
                                   wxGenericValidator(&m_highlightDolchVerbs.get_value())),
                    wxSizerFlags{ 1 }.CenterVertical());
                rowSizer->Add(m_DolchVerbsColorButton,
                              wxSizerFlags{}.Align(wxRight).Border(wxLEFT));
                }
                // noun color
                {
                auto* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, wxSizerFlags{}.Expand());

                m_DolchNounsColorButton = new wxBitmapButton(
                    panel, ID_DOLCH_NOUN_COLOR_BUTTON,
                    ResourceManager::CreateColorIcon(m_dolchNounsColor.get_value()));
                // add them to the sizer
                rowSizer->Add(
                    new wxCheckBox(panel, wxID_ANY, _(L"Dolch nouns color:"), wxDefaultPosition,
                                   wxDefaultSize, 0,
                                   wxGenericValidator(&m_highlightDolchNouns.get_value())),
                    wxSizerFlags{ 1 }.CenterVertical());
                rowSizer->Add(m_DolchNounsColorButton,
                              wxSizerFlags{}.Align(wxRight).Border(wxLEFT));
                }
            }
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateProjectSection()
    {
    const auto openImage = wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON);
    const int optionIndentSize = wxSizerFlags::GetDefaultBorder() * 3;

    if ((GetSectionsBeingShown() & ProjectSection) != 0)
        {
        auto* projectSettingsPage = new wxPanel(m_sideBar, PROJECT_SETTINGS_PAGE, wxDefaultPosition,
                                                wxDefaultSize, wxTAB_TRAVERSAL);
        auto* panelSizer = new wxBoxSizer(wxVERTICAL);
        m_sideBar->AddPage(projectSettingsPage, GetProjectSettingsLabel(), PROJECT_SETTINGS_PAGE,
                           // if the only section being shown, then show this page
                           (GetSectionsBeingShown() == ProjectSection), 11);

        // project properties
        CreateLabelHeader(projectSettingsPage, panelSizer, _(L"Project:"), true);

        auto* projectExtraInfoSizer = new wxFlexGridSizer(2, 5, 5);
        projectExtraInfoSizer->Add(
            new wxStaticText(projectSettingsPage, wxID_STATIC, _(L"Reviewer:")),
            wxSizerFlags{}.CenterVertical());
        auto* reviewerEdit = new wxTextCtrl(projectSettingsPage, wxID_ANY, wxEmptyString,
                                            wxDefaultPosition, wxDefaultSize, wxBORDER_THEME,
                                            wxGenericValidator(&m_reviewer.get_value()));
        projectExtraInfoSizer->Add(reviewerEdit, wxSizerFlags{ 1 }.Expand());

        if (!IsGeneralSettings())
            {
            projectExtraInfoSizer->Add(
                new wxStaticText(projectSettingsPage, wxID_STATIC, _(L"Status:")),
                wxSizerFlags{}.CenterVertical());
            auto* statusEdit = new wxTextCtrl(projectSettingsPage, wxID_ANY, wxEmptyString,
                                              wxDefaultPosition, wxDefaultSize, wxBORDER_THEME,
                                              wxGenericValidator(&m_status.get_value()));
            projectExtraInfoSizer->Add(statusEdit, wxSizerFlags{ 1 }.Expand());
            }

        panelSizer->Add(projectExtraInfoSizer,
                        wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));

        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        if (IsGeneralSettings() || IsBatchProjectSettings())
            {
            panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            auto* batchOptionsBox =
                new wxStaticBoxSizer(wxVERTICAL, projectSettingsPage, _(L"Batch options"));

            auto* minDocSizeBoxSizer = new wxBoxSizer(wxHORIZONTAL);
            batchOptionsBox->Add(minDocSizeBoxSizer, wxSizerFlags{}.Expand().Border(wxLEFT));

            auto* minDocSizeLabel = new wxStaticText(
                batchOptionsBox->GetStaticBox(), wxID_STATIC, _(L"Minimum document word count:"),
                wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
            minDocSizeBoxSizer->Add(minDocSizeLabel,
                                    wxSizerFlags{}.Border(wxRIGHT).CenterVertical());

            auto* minDocWordCountForBatchSpinCtrl = new wxSpinCtrl(
                batchOptionsBox->GetStaticBox(), wxID_ANY,
                std::to_wstring(m_minDocWordCountForBatch.get_value()), wxDefaultPosition,
                wxDefaultSize, wxSP_ARROW_KEYS, 1, std::numeric_limits<int>::max(), 0);
            minDocWordCountForBatchSpinCtrl->SetValidator(
                wxGenericValidator(&m_minDocWordCountForBatch.get_value()));
            minDocSizeBoxSizer->Add(minDocWordCountForBatchSpinCtrl,
                                    wxSizerFlags{}.CenterVertical());

            auto* fileTruncSizer = new wxBoxSizer(wxHORIZONTAL);
            fileTruncSizer->Add(new wxStaticText(batchOptionsBox->GetStaticBox(), wxID_STATIC,
                                                 _(L"File path display:")),
                                wxSizerFlags{}.Border(wxRIGHT).CenterVertical());

            wxArrayString truncModes;
            truncModes.Add(_(L"Partially truncate the file path"));
            truncModes.Add(_(L"Show only the file name"));
            truncModes.Add(_(L"Show the full file path"));
            auto* fileTruncCombo = new wxChoice(
                batchOptionsBox->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize,
                truncModes, 0, wxGenericValidator(&m_filePathTruncationMode.get_value()));
            fileTruncSizer->Add(fileTruncCombo, wxSizerFlags{}.Border(wxRIGHT).CenterVertical());
            batchOptionsBox->AddSpacer(wxSizerFlags::GetDefaultBorder());
            batchOptionsBox->Add(fileTruncSizer, wxSizerFlags{}.Expand().Border(wxLEFT));

            panelSizer->Add(batchOptionsBox, wxSizerFlags{}.Border(wxLEFT, optionIndentSize));
            }

        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        CreateLabelHeader(projectSettingsPage, panelSizer, _(L"Document:"), true);

            {
            wxArrayString languages;
            languages.Add(_(L"English"));
            languages.Add(_(L"Spanish"));
            languages.Add(_(L"German"));

            auto* langSizer = new wxBoxSizer(wxHORIZONTAL);
            auto* langLabel = new wxStaticText(projectSettingsPage, wxID_STATIC, _(L"Language:"),
                                               wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
            langSizer->Add(langLabel, wxSizerFlags{}.CenterVertical().Left());
            langSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            langSizer->Add(new wxChoice(projectSettingsPage, wxID_ANY, wxDefaultPosition,
                                        wxDefaultSize, languages, 0,
                                        wxGenericValidator(&m_projectLanguage.get_value())));
            panelSizer->Add(langSizer, wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));
            panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            }

            // Document origin settings
            {
            // document storage/linking section
            wxArrayString docLinking;
            docLinking.Add(_(L"&Embed the document's text into the project"));
            docLinking.Add(_(L"&Reload the document when opening project"));
            m_docStorageRadioBox = new wxRadioBox(
                projectSettingsPage, ID_DOCUMENT_STORAGE_RADIO_BOX, _(L"Linking and embedding"),
                wxDefaultPosition, wxDefaultSize, docLinking, 0, wxRA_SPECIFY_ROWS,
                wxGenericValidator(&m_documentStorageMethod.get_value()));
            panelSizer->Add(m_docStorageRadioBox, wxSizerFlags{}.Border(wxLEFT, optionIndentSize));

            // not relevant with batch projects
            if (IsGeneralSettings() || IsStandardProjectSettings())
                {
                panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

                m_realTimeUpdateCheckBox =
                    new wxCheckBox(projectSettingsPage, ID_REALTIME_UPDATE_BUTTON,
                                   _(L"Real-time update"), wxDefaultPosition, wxDefaultSize, 0,
                                   wxGenericValidator(&m_realTimeUpdate.get_value()));
                m_realTimeUpdateCheckBox->Enable(
                    static_cast<TextStorage>(m_documentStorageMethod.get_value()) ==
                    TextStorage::NoEmbedText);
                panelSizer->Add(m_realTimeUpdateCheckBox,
                                wxSizerFlags{}.Border(wxLEFT, optionIndentSize));
                }

            panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            }

        if (IsStandardProjectSettings())
            {
            auto* fileBrowseBoxSizer = new wxBoxSizer(wxHORIZONTAL);
            panelSizer->Add(fileBrowseBoxSizer,
                            wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));

            m_filePathEdit = new wxTextCtrl(
                projectSettingsPage, ID_DOCUMENT_PATH_FIELD, wxEmptyString, wxDefaultPosition,
                wxDefaultSize, wxBORDER_THEME, wxGenericValidator(&m_filePath.get_value()));
            m_filePathEdit->AutoCompleteFileNames();
            fileBrowseBoxSizer->Add(m_filePathEdit, wxSizerFlags{ 1 }.Expand());

            m_fileBrowseButton =
                new wxBitmapButton(projectSettingsPage, ID_FILE_BROWSE_BUTTON, openImage);
            fileBrowseBoxSizer->Add(m_fileBrowseButton, wxSizerFlags{}.CenterVertical());
            // only enable these if document linking is selected
            m_filePathEdit->Enable(m_documentStorageMethod ==
                                   static_cast<int>(TextStorage::NoEmbedText));
            m_fileBrowseButton->Enable(m_documentStorageMethod ==
                                       static_cast<int>(TextStorage::NoEmbedText));

            panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            auto* docDescriptionSizer = new wxBoxSizer(wxHORIZONTAL);
            panelSizer->Add(docDescriptionSizer,
                            wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));

            docDescriptionSizer->Add(
                new wxStaticText(projectSettingsPage, wxID_STATIC, _(L"Document description:")),
                wxSizerFlags{}.CenterVertical());
            auto* descriptionEdit =
                new wxTextCtrl(projectSettingsPage, ID_DOCUMENT_DESCRIPTION_FIELD, wxEmptyString,
                               wxDefaultPosition, wxDefaultSize, wxBORDER_THEME,
                               wxGenericValidator(&m_description.get_value()));
            docDescriptionSizer->Add(descriptionEdit, wxSizerFlags{ 1 }.Expand().Border(wxLEFT));
            }
        else if (IsBatchProjectSettings())
            {
            auto* filesSizer = new wxBoxSizer(wxVERTICAL);
            auto* filesButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
            // add files (uses file browser)
            m_addFilesButton =
                new wxBitmapButton(projectSettingsPage, ID_ADD_FILES_BUTTON,
                                   wxArtProvider::GetBitmapBundle(L"ID_DOCUMENTS", wxART_BUTTON));
            m_addFilesButton->SetToolTip(_(L"Browse for documents to add"));
            m_addFilesButton->Enable(m_documentStorageMethod ==
                                     static_cast<int>(TextStorage::NoEmbedText));
            filesButtonsSizer->Add(m_addFilesButton);

            // add row to file list
            m_addFileButton =
                new wxBitmapButton(projectSettingsPage, ID_ADD_FILE_BUTTON,
                                   wxArtProvider::GetBitmapBundle(L"ID_ADD", wxART_BUTTON));
            m_addFileButton->SetToolTip(_(L"Enter a document"));
            m_addFileButton->Enable(m_documentStorageMethod ==
                                    static_cast<int>(TextStorage::NoEmbedText));
            filesButtonsSizer->Add(m_addFileButton);

            m_deleteFileButton =
                new wxBitmapButton(projectSettingsPage, ID_DELETE_FILE_BUTTON,
                                   wxArtProvider::GetBitmapBundle(wxART_DELETE, wxART_BUTTON));
            m_deleteFileButton->SetToolTip(_(L"Remove selected document"));
            m_deleteFileButton->Enable(m_documentStorageMethod ==
                                       static_cast<int>(TextStorage::NoEmbedText));
            filesButtonsSizer->Add(m_deleteFileButton);
            filesSizer->Add(filesButtonsSizer, 0, wxALIGN_RIGHT);

            m_fileData->SetSize(m_readabilityProjectDoc->GetSourceFilesInfo().size(), 2);
            // fill the file info grid
            for (size_t i = 0; i < m_readabilityProjectDoc->GetSourceFilesInfo().size(); ++i)
                {
                m_fileData->SetItemText(
                    i, 0, m_readabilityProjectDoc->GetSourceFilesInfo()[i].first,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_fileData->SetItemText(
                    i, 1, m_readabilityProjectDoc->GetSourceFilesInfo()[i].second,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            m_fileList = new Wisteria::UI::ListCtrlEx(
                projectSettingsPage, ID_FILE_LIST, wxDefaultPosition, wxDefaultSize,
                wxLC_VIRTUAL | wxLC_EDIT_LABELS | wxLC_REPORT | wxLC_ALIGN_LEFT);
            m_fileList->EnableGridLines();
            m_fileList->EnableItemDeletion();
            m_fileList->InsertColumn(0, _(L"Files"));
            m_fileList->InsertColumn(1, _(L"Labels"));
            m_fileList->SetColumnEditable(0);
            m_fileList->SetColumnEditable(1);
            m_fileList->SetVirtualDataProvider(m_fileData);
            m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
            m_fileList->Enable(m_documentStorageMethod ==
                               static_cast<int>(TextStorage::NoEmbedText));
            filesSizer->Add(m_fileList,
                            wxSizerFlags{ 1 }.Expand().Border(wxLEFT, optionIndentSize));

            panelSizer->Add(filesSizer, wxSizerFlags{ 1 }.Expand());
            }

        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            // additional template file to append
            {
            CreateLabelHeader(
                projectSettingsPage, panelSizer,
                _(L"Append Additional Document (e.g., policies, license agreements, addendums):"),
                true);

            auto* fileBrowseBoxSizer = new wxBoxSizer(wxHORIZONTAL);
            panelSizer->Add(fileBrowseBoxSizer,
                            wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));

            auto* filePathEdit =
                new wxTextCtrl(projectSettingsPage, ID_ADDITIONAL_FILE_FIELD, wxEmptyString,
                               wxDefaultPosition, wxDefaultSize, wxBORDER_THEME,
                               wxGenericValidator(&m_appendedDocumentFilePath.get_value()));
            filePathEdit->AutoCompleteFileNames();
            fileBrowseBoxSizer->Add(filePathEdit, wxSizerFlags{ 1 }.Expand());

            fileBrowseBoxSizer->Add(new wxBitmapButton(projectSettingsPage,
                                                       ID_ADDITIONAL_FILE_BROWSE_BUTTON, openImage),
                                    wxSizerFlags{}.CenterVertical());
            }

        projectSettingsPage->SetSizer(panelSizer);
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateControls()
    {
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);
    if (GetSectionsBeingShown() == TextSection || GetSectionsBeingShown() == GraphsSection ||
        GetSectionsBeingShown() == Statistics)
        {
        mainSizer->SetMinSize(FromDIP(wxSize{ 500, 700 }));
        }
    else
        {
        mainSizer->SetMinSize(FromDIP(wxSize{ 650, 700 }));
        }

    const int optionIndentSize = wxSizerFlags::GetDefaultBorder() * 3;

    m_sideBar = new Wisteria::UI::SideBarBook(this, wxID_ANY);
    wxGetApp().ApplyThemeToSideBar(m_sideBar->GetSideBar());
    mainSizer->Add(m_sideBar, wxSizerFlags{ 1 }.Expand().Border());

    if (GetSectionsBeingShown() == AllSections)
        {
        if (IsGeneralSettings())
            {
            auto* generalSettingsPage =
                new wxPanel(m_sideBar, GENERAL_SETTINGS_PAGE, wxDefaultPosition, wxDefaultSize,
                            wxTAB_TRAVERSAL);
            auto* docPanelSizer = new wxBoxSizer(wxVERTICAL);
            generalSettingsPage->SetSizer(docPanelSizer);
            m_sideBar->AddPage(generalSettingsPage, GetGeneralSettingsLabel(),
                               GENERAL_SETTINGS_PAGE, true, 10);

            auto* optionsSizer = new wxBoxSizer(wxHORIZONTAL);
            docPanelSizer->Add(optionsSizer, wxSizerFlags{}.Expand());

            optionsSizer->Add(new wxStaticText(generalSettingsPage, wxID_STATIC,
                                               _(L"UI language (requires restart):")),
                              wxSizerFlags{}.Border(wxLEFT).CenterVertical());

            const wxArrayString choiceStrings = {
                _(L"System Default"), wxString{ _DT(L"English", DTExplanation::Constant) },
                wxString{ _DT(L"Español", DTExplanation::Constant,
                              L"This should be in the native language so that users can recognize "
                              "it when selecting which language to use.") }
            };
            auto* uiLangCombo =
                new wxComboBox(generalSettingsPage, wxID_ANY, wxString{}, wxDefaultPosition,
                               wxDefaultSize, choiceStrings, wxCB_DROPDOWN | wxCB_READONLY);
            uiLangCombo->SetValidator(wxGenericValidator(&m_uiLanguage.get_value()));
            optionsSizer->Add(uiLangCombo, wxSizerFlags{}.Expand().Border());

            CreateLabelHeader(generalSettingsPage, docPanelSizer, _(L"Settings:"), true);

            optionsSizer = new wxBoxSizer(wxVERTICAL);
            docPanelSizer->Add(optionsSizer,
                               wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));

            auto* loadSettingsButton =
                new wxButton(generalSettingsPage, ID_LOAD_SETTINGS_BUTTON, _(L"Import..."));
            optionsSizer->Add(loadSettingsButton, wxSizerFlags{}.Border(wxTOP | wxBOTTOM));

            auto* exportSettingsButton =
                new wxButton(generalSettingsPage, ID_EXPORT_SETTINGS_BUTTON, _(L"Export..."));
            optionsSizer->Add(exportSettingsButton, wxSizerFlags{}.Border(wxTOP | wxBOTTOM));

            auto* resetSettingsButton =
                new wxButton(generalSettingsPage, ID_RESET_SETTINGS_BUTTON, _(L"Reset"));
            optionsSizer->Add(resetSettingsButton, wxSizerFlags{}.Border(wxTOP | wxBOTTOM));

            CreateLabelHeader(generalSettingsPage, docPanelSizer, _(L"Internet:"), true);

            optionsSizer = new wxBoxSizer(wxVERTICAL);
            docPanelSizer->Add(optionsSizer,
                               wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));

            auto* userAgentSizer = new wxBoxSizer(wxHORIZONTAL);
            optionsSizer->Add(userAgentSizer, wxSizerFlags{}.Expand());

            userAgentSizer->Add(
                new wxStaticText(generalSettingsPage, wxID_STATIC, _(L"User agent:")),
                wxSizerFlags{}.CenterVertical());
            auto* userAgentEdit = new wxTextCtrl(generalSettingsPage, wxID_ANY, wxString{},
                                                 wxDefaultPosition, wxDefaultSize, wxBORDER_THEME,
                                                 wxGenericValidator(&m_userAgent.get_value()));
            userAgentSizer->Add(userAgentEdit, wxSizerFlags{ 1 }.Expand().Border(wxLEFT | wxRIGHT));

            optionsSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                             _(L"Disable SSL certificate verification"),
                                             wxDefaultPosition, wxDefaultSize, 0,
                                             wxGenericValidator(&m_disablePeerVerify.get_value())),
                              wxSizerFlags{}.Expand().Border(wxTOP));

            optionsSizer->Add(new wxCheckBox(generalSettingsPage, ID_JS_COOKIES_CHECKBOX,
                                             _(L"Use JavaScript cookies"), wxDefaultPosition,
                                             wxDefaultSize, 0,
                                             wxGenericValidator(&m_useJsCookies.get_value())),
                              wxSizerFlags{}.Expand().Border(wxTOP));

            m_persistCookiesCheck = new wxCheckBox(
                generalSettingsPage, wxID_ANY, _(L"Persist cookies during each harvest"),
                wxDefaultPosition, wxDefaultSize, 0,
                wxGenericValidator(&m_persistJsCookies.get_value()));
            optionsSizer->Add(m_persistCookiesCheck, wxSizerFlags{}.Expand().Border(wxTOP));
            m_persistCookiesCheck->Enable(m_useJsCookies.get_value());

            optionsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            CreateLabelHeader(generalSettingsPage, docPanelSizer, _(L"Warnings && Prompts:"), true);

            optionsSizer = new wxBoxSizer(wxVERTICAL);
            docPanelSizer->Add(optionsSizer,
                               wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));

            auto* warningsButton =
                new wxButton(generalSettingsPage, ID_WARNING_MESSAGES_BUTTON, _(L"Customize..."));
            optionsSizer->Add(warningsButton, wxSizerFlags{}.Border(wxTOP | wxBOTTOM));

            CreateLabelHeader(generalSettingsPage, docPanelSizer, _(L"Reports:"), true);

            optionsSizer = new wxBoxSizer(wxVERTICAL);
            docPanelSizer->Add(optionsSizer,
                               wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));

            optionsSizer->Add(
                new wxCheckBox(generalSettingsPage, wxID_ANY,
                               _(L"Disable GPU acceleration for reports (requires restart)"),
                               wxDefaultPosition, wxDefaultSize, 0,
                               wxGenericValidator(&m_disableGpuAcceleration.get_value())),
                wxSizerFlags{}.Expand().Border(wxTOP | wxBOTTOM));

            CreateLabelHeader(generalSettingsPage, docPanelSizer, _(L"Log:"), true);

            optionsSizer = new wxBoxSizer(wxVERTICAL);
            docPanelSizer->Add(optionsSizer,
                               wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));

            optionsSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY, _(L"Show Log tab"),
                                             wxDefaultPosition, wxDefaultSize, 0,
                                             wxGenericValidator(&m_showLogTab.get_value())),
                              wxSizerFlags{}.Expand().Border(wxTOP));

            optionsSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                             _(L"Auto refresh log report"), wxDefaultPosition,
                                             wxDefaultSize, 0,
                                             wxGenericValidator(&m_logAutoRefresh.get_value())),
                              wxSizerFlags{}.Expand().Border(wxTOP));

            optionsSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY, _(L"Verbose logging"),
                                             wxDefaultPosition, wxDefaultSize, 0,
                                             wxGenericValidator(&m_logVerbose.get_value())),
                              wxSizerFlags{}.Expand().Border(wxTOP));

            optionsSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY, _(L"Append daily log"),
                                             wxDefaultPosition, wxDefaultSize, 0,
                                             wxGenericValidator(&m_logAppendDailyLog.get_value())),
                              wxSizerFlags{}.Expand().Border(wxTOP | wxBOTTOM));

            CreateLabelHeader(generalSettingsPage, docPanelSizer,
                              // TRANSLATORS: Developer tools and automation scripting.
                              _(L"Developer:"), true);

            optionsSizer = new wxBoxSizer(wxVERTICAL);
            docPanelSizer->Add(optionsSizer,
                               wxSizerFlags{}.Expand().Border(wxLEFT, optionIndentSize));

            optionsSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                             _(L"Show Developer tab"), wxDefaultPosition,
                                             wxDefaultSize, 0,
                                             wxGenericValidator(&m_showDeveloperTab.get_value())),
                              wxSizerFlags{}.Expand().Border(wxTOP));

            optionsSizer->Add(
                new wxCheckBox(generalSettingsPage, wxID_ANY,
                               _(L"Allow Lua scripts to access system commands (requires restart)"),
                               wxDefaultPosition, wxDefaultSize, 0,
                               wxGenericValidator(&m_luaUnsafeMode.get_value())),
                wxSizerFlags{}.Expand().Border(wxTOP));

            CreateThemesSection();
            }
        }

    // Project settings
    CreateProjectSection();

    // Document parsing page
    CreateDocumentIndexingSection();

    // Readability settings
    CreateReadabilitySection();

    // Statistics page
    CreateStatisticsSection();

    // Words breakdown page (these options only apply to general options and standard projects)
    CreateWordsBreakdownSection();

    // Sentences breakdown page (these options only apply to general options and standard projects)
    CreateSentencesBreakdownSection();

    // Grammar page
    CreateGrammarSection();

    // Text window options page (these options only apply to general options and standard projects)
    CreateTextWindowSection();

    // Graphs options page
    CreateGraphSection();

    mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL | wxHELP), wxSizerFlags{}.Expand().Border());

    if (IsGeneralSettings())
        {
        auto* banner = new wxBannerWindow(this, wxTOP);
        banner->SetText(wxString::Format(_(L"%s Options"), wxGetApp().GetAppDisplayName()),
                        _(L"These options will only affect new projects.\n"
                          "To change an existing project, click \"Properties\" on the Home tab."));
        banner->SetGradient(banner->GetBackgroundColour(),
                            Wisteria::Colors::ColorContrast::ColorContrast::ShadeOrTint(
                                banner->GetBackgroundColour()));

        mainSizer->Insert(0, banner, wxSizerFlags{}.Expand());
        }
    else
        {
        wxString displayableProjectName =
            (m_readabilityProjectDoc != nullptr) ? m_readabilityProjectDoc->GetTitle() : wxString{};
        if (displayableProjectName.length() >= 50)
            {
            displayableProjectName.Truncate(49).Append(static_cast<wchar_t>(8230));
            }

        auto* banner = new wxBannerWindow(this, wxTOP);
        banner->SetText(
            _(L"Project Properties"),
            wxString::Format(_(L"These options only affect the current project (\"%s\").\n"
                               "To change options for future projects, click "
                               "\"Options\" on the \"Tools\" tab."),
                             displayableProjectName));
        banner->SetGradient(
            banner->GetBackgroundColour(),
            Wisteria::Colors::ColorContrast::ShadeOrTint(banner->GetBackgroundColour()));

        mainSizer->Insert(0, banner, wxSizerFlags{}.Expand());
        }

    SetSizerAndFit(mainSizer);

    // need to fit these columns after everything else was resized
    if (IsBatchProjectSettings() && (GetFileList() != nullptr))
        {
        GetFileList()->SetColumnWidth(0, GetFileList()->GetClientSize().GetWidth() * .75);
        GetFileList()->SetColumnWidth(1, GetFileList()->GetClientSize().GetWidth() * .25);
        }

    if (m_readabilityTestsPropertyGrid != nullptr)
        {
        m_readabilityTestsPropertyGrid->FitColumns();
        }
    if (m_gradeLevelPropertyGrid != nullptr)
        {
        m_gradeLevelPropertyGrid->FitColumns();
        }
    if (m_grammarPropertyGrid != nullptr)
        {
        m_grammarPropertyGrid->FitColumns();
        }
    if (m_wordsBreakdownPropertyGrid != nullptr)
        {
        m_wordsBreakdownPropertyGrid->FitColumns();
        }
    if (m_sentencesBreakdownPropertyGrid != nullptr)
        {
        m_sentencesBreakdownPropertyGrid->FitColumns();
        }
    if (m_statisticsPropertyGrid != nullptr)
        {
        m_statisticsPropertyGrid->FitColumns();
        }
    if (m_generalGraphPropertyGrid != nullptr)
        {
        m_generalGraphPropertyGrid->FitColumns();
        }
    if (m_readabilityGraphPropertyGrid != nullptr)
        {
        m_readabilityGraphPropertyGrid->FitColumns();
        }
    if (m_barChartPropertyGrid != nullptr)
        {
        m_barChartPropertyGrid->FitColumns();
        }
    if (m_histogramPropertyGrid != nullptr)
        {
        m_histogramPropertyGrid->FitColumns();
        }
    if (m_boxPlotsPropertyGrid != nullptr)
        {
        m_boxPlotsPropertyGrid->FitColumns();
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateThemesSection()
    {
    auto* themesPage =
        new wxPanel(m_sideBar, THEMES_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    auto* panelSizer = new wxBoxSizer(wxVERTICAL);
    themesPage->SetSizer(panelSizer);
    m_sideBar->AddPage(themesPage, GetThemesLabel(), THEMES_PAGE, false);

    auto* optionsSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(optionsSizer, wxSizerFlags{}.Expand().Border(wxLEFT | wxTOP,
                                                                 wxSizerFlags::GetDefaultBorder()));

    auto* themeSizer = new wxBoxSizer(wxHORIZONTAL);
    optionsSizer->Add(themeSizer, wxSizerFlags{}.Expand().Border(wxBOTTOM));

    themeSizer->Add(new wxStaticText(themesPage, wxID_STATIC, _(L"Theme:")),
                    wxSizerFlags{}.CenterVertical());

    wxArrayString themeFiles;
    // recurses into subfolders, so "export-themes" (CSSes for the "Export All"
    // single-file report, not a user-selectable in-app report theme) is filtered out below
    wxDir::GetAllFiles(wxGetApp().FindResourceDirectory(_DT(L"report-themes")), &themeFiles,
                       _DT(L"*.css"), wxDIR_FILES);
    wxArrayString themeChoices;
    for (const auto& themeFile : themeFiles)
        {
        const wxFileName themeFileName{ themeFile };
        if (themeFileName.GetName().CmpNoCase(_DT(L"default")) != 0 &&
            (themeFileName.GetDirs().empty() ||
             themeFileName.GetDirs().Last().CmpNoCase(_DT(L"export-themes")) != 0))
            {
            themeChoices.Add(ThemeFileNameToLabel(themeFile));
            }
        }
    themeChoices.Sort();

    auto* themeCombo = new wxComboBox(themesPage, wxID_ANY, wxString{}, wxDefaultPosition,
                                      wxDefaultSize, themeChoices, wxCB_DROPDOWN | wxCB_READONLY);
    themeCombo->SetValidator(wxGenericValidator(&m_reportTheme.get_value()));
    themeSizer->Add(themeCombo, wxSizerFlags{}.CenterVertical().Border(wxLEFT));
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateGraphTitlesSection()
    {
    auto* panel = new wxPanel(m_sideBar, GRAPH_TITLES_PAGE, wxDefaultPosition, wxDefaultSize,
                              wxTAB_TRAVERSAL);
    auto* panelSizer = new wxBoxSizer(wxVERTICAL);
    auto* optionsSizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(panelSizer);
    m_sideBar->AddSubPage(panel, GetTitlesLabel(), GRAPH_TITLES_PAGE, false, 9);

    CreateLabelHeader(panel, panelSizer, _(L"Fonts:"), true);
    m_graphTopTitleFontButton =
        new wxButton(panel, ID_GRAPH_TOP_TITLE_FONT_BUTTON, _(L"Top titles"));
    m_graphTopTitleFontButton->SetBitmap(
        wxGetApp().GetResourceManager().GetSVG(L"ribbon/top-titles.svg"));
    optionsSizer->Add(m_graphTopTitleFontButton, wxSizerFlags{}.Expand().DoubleBorder(wxLEFT));

    m_graphBottomTitleFontButton =
        new wxButton(panel, ID_GRAPH_BOTTOM_TITLE_FONT_BUTTON, _(L"Bottom titles"));
    m_graphBottomTitleFontButton->SetBitmap(
        wxGetApp().GetResourceManager().GetSVG(L"ribbon/bottom-titles.svg"));
    optionsSizer->Add(m_graphBottomTitleFontButton, wxSizerFlags{}.Expand().DoubleBorder(wxLEFT));

    m_graphLeftTitleFontButton =
        new wxButton(panel, ID_GRAPH_LEFT_TITLE_FONT_BUTTON, _(L"Left titles"));
    m_graphLeftTitleFontButton->SetBitmap(
        wxGetApp().GetResourceManager().GetSVG(L"ribbon/left-titles.svg"));
    optionsSizer->Add(m_graphLeftTitleFontButton, wxSizerFlags{}.Expand().DoubleBorder(wxLEFT));

    m_graphRightTitleFontButton =
        new wxButton(panel, ID_GRAPH_RIGHT_TITLE_FONT_BUTTON, _(L"Right titles"));
    m_graphRightTitleFontButton->SetBitmap(
        wxGetApp().GetResourceManager().GetSVG(L"ribbon/right-titles.svg"));
    optionsSizer->Add(m_graphRightTitleFontButton, wxSizerFlags{}.Expand().DoubleBorder(wxLEFT));

    panelSizer->Add(optionsSizer);
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateGraphAxesSection()
    {
    auto* panel =
        new wxPanel(m_sideBar, GRAPH_AXIS_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    auto* panelSizer = new wxBoxSizer(wxVERTICAL);
    auto* optionsSizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(panelSizer);
    m_sideBar->AddSubPage(panel, GetAxisSettingsLabel(), GRAPH_AXIS_PAGE, false, 9);

    CreateLabelHeader(panel, panelSizer, _(L"Fonts:"), true);
    m_xAxisFontButton = new wxButton(panel, ID_X_AXIS_FONT_BUTTON, _(L"x-axis"));
    m_xAxisFontButton->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/x-axis.svg"));
    optionsSizer->Add(m_xAxisFontButton, wxSizerFlags{}.Expand().DoubleBorder(wxLEFT));

    m_yAxisFontButton = new wxButton(panel, ID_Y_AXIS_FONT_BUTTON, _(L"y-axis"));
    m_yAxisFontButton->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/y-axis.svg"));
    optionsSizer->Add(m_yAxisFontButton, wxSizerFlags{}.Expand().DoubleBorder(wxLEFT));

    panelSizer->Add(optionsSizer);
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateGraphGeneralSection()
    {
    auto* graphGeneralPage = new wxPanel(m_sideBar, GRAPH_GENERAL_PAGE, wxDefaultPosition,
                                         wxDefaultSize, wxTAB_TRAVERSAL);
    m_sideBar->AddPage(graphGeneralPage, GetGraphsLabel(), GRAPH_GENERAL_PAGE,
                       // if the only section being shown, then show this page
                       (GetSectionsBeingShown() == GraphsSection), 7);

    auto* panelSizer = new wxBoxSizer(wxVERTICAL);
    graphGeneralPage->SetSizer(panelSizer);
    m_sideBar->AddSubPage(graphGeneralPage, GetGeneralLabel(), GRAPH_GENERAL_PAGE, false, 9);

    auto* pgMan = new wxPropertyGridManager(
        graphGeneralPage, ID_GRAPH_OPTIONS_PROPERTYGRID, wxDefaultPosition, wxDefaultSize,
        wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);
    m_generalGraphPropertyGrid = pgMan->AddPage();

    wxPGChoices graphColorSchemes;
    for (const auto& colorScheme : wxGetApp().GetGraphColorSchemeMap())
        {
        graphColorSchemes.Add(colorScheme.first);
        }
    // do a reverse lookup to map internal "coffeeshop" to UI's "Coffee Shop"
    wxString defaultColorSchemeValue{ _("Campfire") };
    const wxString systemColorSchemeValue =
        ((m_readabilityProjectDoc != nullptr) ? m_readabilityProjectDoc->GetGraphColorScheme() :
                                                wxGetApp().GetAppOptions()->GetGraphColorScheme());
    for (const auto& theme : wxGetApp().GetGraphColorSchemeMap())
        {
        if (systemColorSchemeValue.CmpNoCase(theme.second) == 0)
            {
            defaultColorSchemeValue = theme.first;
            break;
            }
        }
    auto* colorSchemeProp = m_generalGraphPropertyGrid->Append(
        new wxEnumProperty(GetGraphColorSchemeLabel(), wxPG_LABEL, graphColorSchemes));
    colorSchemeProp->SetValue(defaultColorSchemeValue);
    colorSchemeProp->SetHelpString(_(L"Select from here the color scheme to apply to pie charts, "
                                     "word clouds, and grouped histograms."));

    m_generalGraphPropertyGrid->Append(new wxPropertyCategory(GetGraphBackgroundLabel()));
    m_generalGraphPropertyGrid->SetPropertyHelpString(
        GetGraphBackgroundLabel(),
        _(L"The options in this section customize the backgrounds of the graphs."));
    // color
    auto* backgroundColorProp = m_generalGraphPropertyGrid->Append(new wxColourProperty(
        GetColorLabel(), GetBackgroundColorLabel(),
        ((m_readabilityProjectDoc != nullptr) ? m_readabilityProjectDoc->GetBackGroundColor() :
                                                wxGetApp().GetAppOptions()->GetBackGroundColor())));
    backgroundColorProp->SetHelpString(
        _(L"Selects the color for the graphs' background. "
          "Note that if you are displaying an image, then the image will be "
          "shown on top of this color."));
    // color fade
    auto* colorFadeProp = backgroundColorProp->AppendChild(
        new wxBoolProperty(GetApplyFadeLabel(), wxPG_LABEL,
                           ((m_readabilityProjectDoc != nullptr) ?
                                m_readabilityProjectDoc->GetGraphBackGroundLinearGradient() :
                                wxGetApp().GetAppOptions()->GetGraphBackGroundLinearGradient())));
    colorFadeProp->SetAttribute(wxPG_BOOL_USE_CHECKBOX, true);
    colorFadeProp->SetHelpString(
        _(L"Check this to apply a downward fade to the background color of your graphs. "
          "The background of your graphs will fade (top-to-bottom) starting with the "
          "color that you have selected into white."));

    // plot area
    m_generalGraphPropertyGrid->Append(new wxPropertyCategory(GetPlotAreaBackgroundLabel()));
    m_generalGraphPropertyGrid->SetPropertyHelpString(
        GetPlotAreaBackgroundLabel(), _(L"The options in this section customize the plot area "
                                        "backgrounds of the graphs."));

    // color
    auto* plotColor =
        new wxColourProperty(GetColorLabel(), wxPG_LABEL,
                             ((m_readabilityProjectDoc != nullptr) ?
                                  m_readabilityProjectDoc->GetPlotBackGroundColor() :
                                  wxGetApp().GetAppOptions()->GetPlotBackGroundColor()));
    plotColor->SetHelpString(_(L"Selects the color for the plot area background of the graphs."));
    // opacity
    auto* plotColorOpacity = plotColor->AppendChild(
        new wxIntProperty(GetOpacityLabel(), wxPG_LABEL,
                          ((m_readabilityProjectDoc != nullptr) ?
                               m_readabilityProjectDoc->GetPlotBackGroundColorOpacity() :
                               wxGetApp().GetAppOptions()->GetPlotBackGroundColorOpacity())));
    plotColorOpacity->SetEditor(wxPGEditor_SpinCtrl);
    plotColorOpacity->SetAttribute(wxPG_ATTR_MIN, wxALPHA_TRANSPARENT);
    plotColorOpacity->SetAttribute(wxPG_ATTR_MAX, wxALPHA_OPAQUE);
    plotColorOpacity->SetHelpString(_(L"Sets the transparency of the plot background color. "
                                      "A value of 255 will set the background to be fully opaque, "
                                      "whereas 0 will set the background to be transparent."));

    m_generalGraphPropertyGrid->Append(plotColor);

    // image
    auto* backgroundImage =
        new wxImageFileProperty(GetImageLabel(), wxPG_LABEL,
                                ((m_readabilityProjectDoc != nullptr) ?
                                     m_readabilityProjectDoc->GetPlotBackGroundImagePath() :
                                     wxGetApp().GetAppOptions()->GetPlotBackGroundImagePath()));
    backgroundImage->SetAttribute(wxPG_FILE_WILDCARD,
                                  Wisteria::GraphItems::Image::GetImageFileFilter());
    backgroundImage->SetAttribute(wxPG_DIALOG_TITLE, _(L"Select Plot Background Image"));
    backgroundImage->SetAttribute(wxPG_ATTR_HINT, _(L"Select an image"));
    backgroundImage->SetHelpString(_(L"Selects the image for the graphs' background."));

    // image effect
    wxPGChoices imgEffects;
    imgEffects.Add(_(L"No effect"));
    imgEffects.Add(_(L"Grayscale"));
    imgEffects.Add(_(L"Blur horizontally"));
    imgEffects.Add(_(L"Blur vertically"));
    imgEffects.Add(_(L"Sepia"));
    imgEffects.Add(_(L"Frosted glass"));
    imgEffects.Add(_(L"Oil painting"));
    auto* imgEffectProp = backgroundImage->AppendChild(new wxEnumProperty(
        GetEffectLabel(), wxPG_LABEL, imgEffects,
        ((m_readabilityProjectDoc != nullptr) ?
             static_cast<int>(m_readabilityProjectDoc->GetPlotBackGroundImageEffect()) :
             static_cast<int>(wxGetApp().GetAppOptions()->GetPlotBackGroundImageEffect()))));
    imgEffectProp->SetHelpString(_(L"Applies an effect to the plot background image."));

    // image opacity
    auto* imgOpacityProp = backgroundImage->AppendChild(
        new wxIntProperty(GetOpacityLabel(), wxPG_LABEL,
                          ((m_readabilityProjectDoc != nullptr) ?
                               m_readabilityProjectDoc->GetPlotBackGroundImageOpacity() :
                               wxGetApp().GetAppOptions()->GetPlotBackGroundImageOpacity())));
    imgOpacityProp->SetEditor(wxPGEditor_SpinCtrl);
    imgOpacityProp->SetAttribute(wxPG_ATTR_MIN, wxALPHA_TRANSPARENT);
    imgOpacityProp->SetAttribute(wxPG_ATTR_MAX, wxALPHA_OPAQUE);
    imgOpacityProp->SetHelpString(
        _(L"Sets the transparency of the plot background image. "
          "A value of 255 will set the image to be fully opaque, whereas 0 will set "
          "the image to be transparent."));

    // image fit
    wxPGChoices imgFits;
    // TRANSLATORS: "Crop" as in trimming an image. These are both verbs.
    imgFits.Add(_(L"Crop & center"));
    imgFits.Add(_(L"Shrink to fit"));
    auto* imgFitProp = backgroundImage->AppendChild(new wxEnumProperty(
        GetFitLabel(), wxPG_LABEL, imgFits,
        ((m_readabilityProjectDoc != nullptr) ?
             static_cast<int>(m_readabilityProjectDoc->GetPlotBackGroundImageFit()) :
             static_cast<int>(wxGetApp().GetAppOptions()->GetPlotBackGroundImageFit()))));
    imgFitProp->SetHelpString(_(L"How to fit the image within the plot background."));

    m_generalGraphPropertyGrid->Append(backgroundImage);
    // set the default folder to the global image folder if no image provided
    if (m_generalGraphPropertyGrid->GetPropertyValueAsString(GetImageLabel()).empty())
        {
        backgroundImage->SetAttribute(wxPG_FILE_INITIAL_PATH,
                                      wxGetApp().GetAppOptions()->GetImagePath());
        }

    m_generalGraphPropertyGrid->Append(new wxPropertyCategory(GetWatermarksLogosLabel()));
    m_generalGraphPropertyGrid->SetPropertyHelpString(
        GetWatermarksLogosLabel(), _(L"The options in this section customize the watermarks and "
                                     "logo images of the graphs."));

    // logo
    auto* graphLogo = new wxImageFileProperty(GetLogoImageLabel(), wxPG_LABEL,
                                              ((m_readabilityProjectDoc != nullptr) ?
                                                   m_readabilityProjectDoc->GetWatermarkLogoPath() :
                                                   wxGetApp().GetAppOptions()->GetWatermarkLogo()));
    graphLogo->SetAttribute(wxPG_FILE_WILDCARD, Wisteria::GraphItems::Image::GetImageFileFilter());
    graphLogo->SetAttribute(wxPG_DIALOG_TITLE, _(L"Select Logo Image"));
    graphLogo->SetAttribute(wxPG_ATTR_HINT, _(L"Select an image"));
    m_generalGraphPropertyGrid->Append(graphLogo);
    // set the default folder to the global image folder if no image provided
    if (m_generalGraphPropertyGrid->GetPropertyValueAsString(GetLogoImageLabel()).empty())
        {
        graphLogo->SetAttribute(wxPG_FILE_INITIAL_PATH, wxGetApp().GetAppOptions()->GetImagePath());
        }
    m_generalGraphPropertyGrid->SetPropertyHelpString(
        GetLogoImageLabel(),
        _(L"Enter the path to the image file used as your graphs' logo into this field. "
          "The logo will be displayed in the lower right-hand corner of each graph."));

    m_generalGraphPropertyGrid->Append(new wxPropertyCategory(GetEffectsLabel()));
    m_generalGraphPropertyGrid->SetPropertyHelpString(
        GetEffectsLabel(),
        _(L"The options in this section customize various visual effects of the graphs."));
    // stipple image
    auto* customBrushProp = new wxImageFileProperty(
        GetStippleImageLabel(), wxPG_LABEL,
        ((m_readabilityProjectDoc != nullptr) ? m_readabilityProjectDoc->GetStippleImagePath() :
                                                wxGetApp().GetAppOptions()->GetStippleImagePath()));
    customBrushProp->SetAttribute(wxPG_FILE_WILDCARD,
                                  Wisteria::GraphItems::Image::GetImageFileFilter());
    customBrushProp->SetAttribute(wxPG_DIALOG_TITLE, _(L"Select Stipple Image"));
    customBrushProp->SetAttribute(wxPG_ATTR_HINT, _(L"Select an image"));
    m_generalGraphPropertyGrid->Append(customBrushProp);
    m_generalGraphPropertyGrid->SetPropertyHelpString(
        GetStippleImageLabel(),
        _(L"Enter into this field the file path to the image used for a stipple brush. "
          "A stipple brush can be used to draw stacked images across bars and boxes"));
    // set the default folder to the global image folder if no image is provided
    if (m_generalGraphPropertyGrid->GetPropertyValueAsString(GetStippleImageLabel()).empty())
        {
        customBrushProp->SetAttribute(wxPG_FILE_INITIAL_PATH,
                                      wxGetApp().GetAppOptions()->GetImagePath());
        }

    wxPGChoices shapes;
    for (const auto& sh : wxGetApp().GetShapeMap())
        {
        shapes.Add(sh.first);
        }
    // do a reverse lookup to map internal "fall-leaf" to UI's "Fall leaf"
    wxString defaultValue{ _("Book") };
    const wxString systemValue =
        ((m_readabilityProjectDoc != nullptr) ? m_readabilityProjectDoc->GetStippleShape() :
                                                wxGetApp().GetAppOptions()->GetStippleShape());
    for (const auto& sh : wxGetApp().GetShapeMap())
        {
        if (systemValue.CmpNoCase(sh.second) == 0)
            {
            defaultValue = sh.first;
            break;
            }
        }
    auto* shapesProp = m_generalGraphPropertyGrid->Append(
        new wxEnumProperty(GetStippleShapeLabel(), wxPG_LABEL, shapes));
    shapesProp->SetValue(defaultValue);
    shapesProp->SetHelpString(
        _(L"Select from here which shape to use for a stipple brush. "
          "A stipple brush can be used to draw stacked shape across bars and boxes"));

    auto* shapeColorProp = shapesProp->AppendChild(
        new wxColourProperty(GetColorLabel(), wxPG_LABEL,
                             ((m_readabilityProjectDoc != nullptr) ?
                                  m_readabilityProjectDoc->GetStippleShapeColor() :
                                  wxGetApp().GetAppOptions()->GetStippleShapeColor())));
    shapeColorProp->SetHelpString(_(L"Selects the color used for certain stipple shapes."));

    // common image
    auto* commonImageProp =
        new wxImageFileProperty(GetCommonImageLabel(), wxPG_LABEL,
                                ((m_readabilityProjectDoc != nullptr) ?
                                     m_readabilityProjectDoc->GetGraphCommonImagePath() :
                                     wxGetApp().GetAppOptions()->GetGraphCommonImagePath()));
    commonImageProp->SetAttribute(wxPG_FILE_WILDCARD,
                                  Wisteria::GraphItems::Image::GetImageFileFilter());
    commonImageProp->SetAttribute(wxPG_DIALOG_TITLE, _(L"Select Common Image"));
    commonImageProp->SetAttribute(wxPG_ATTR_HINT, _(L"Select an image"));
    m_generalGraphPropertyGrid->Append(commonImageProp);
    m_generalGraphPropertyGrid->SetPropertyHelpString(
        GetCommonImageLabel(), _(L"Enter into this field the file path to the common image "
                                 "used used to draw across all bars/boxes."));
    // set the default folder to the global image folder if no image is provided
    if (m_generalGraphPropertyGrid->GetPropertyValueAsString(GetCommonImageLabel()).empty())
        {
        commonImageProp->SetAttribute(wxPG_FILE_INITIAL_PATH,
                                      wxGetApp().GetAppOptions()->GetImagePath());
        }

    // drop shadows
    m_generalGraphPropertyGrid->Append(
        new wxBoolProperty(GetDisplayDropShadowsLabel(), wxPG_LABEL,
                           ((m_readabilityProjectDoc != nullptr) ?
                                m_readabilityProjectDoc->IsDisplayingDropShadows() :
                                wxGetApp().GetAppOptions()->IsDisplayingDropShadows())));
    m_generalGraphPropertyGrid->SetPropertyAttribute(GetDisplayDropShadowsLabel(),
                                                     wxPG_BOOL_USE_CHECKBOX, true);
    m_generalGraphPropertyGrid->SetPropertyHelpString(
        GetDisplayDropShadowsLabel(),
        _(L"Check this to include shadows underneath items such as boxes, bars, "
          "and labels. Unchecking this option will give your graphs a more \"flat\" "
          "look."));

    auto* showcaseOption = m_generalGraphPropertyGrid->Append(
        new wxBoolProperty(GetShowcaseKeyItemsLabel(), wxPG_LABEL,
                           ((m_readabilityProjectDoc != nullptr) ?
                                m_readabilityProjectDoc->IsShowcasingKeyItems() :
                                wxGetApp().GetAppOptions()->IsShowcasingKeyItems())));
    showcaseOption->SetAttribute(wxPG_BOOL_USE_CHECKBOX, true);
    showcaseOption->SetHelpString(
        _(L"Check this to draw attention to the complex (i.e., 3+ syllable) word groups "
          "on syllable histograms and pie charts (standard projects)."));

    pgMan->SelectProperty(GetGraphBackgroundLabel());

    panelSizer->Add(pgMan, wxSizerFlags{ 1 }.Expand());
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateGraphReadabilitySection()
    {
    auto* panel = new wxPanel(m_sideBar, GRAPH_READABILITY_GRAPHS_PAGE, wxDefaultPosition,
                              wxDefaultSize, wxTAB_TRAVERSAL);
    auto* panelSizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(panelSizer);
    m_sideBar->AddSubPage(panel, GetReadabilityGraphLabel(), GRAPH_READABILITY_GRAPHS_PAGE, false,
                          9);

    // Fry-like graphs
    auto* pgMan =
        new wxPropertyGridManager(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                  wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);
    m_readabilityGraphPropertyGrid = pgMan->AddPage();
    m_readabilityGraphPropertyGrid->Append(new wxPropertyCategory(GetFryLikeLabel()));
    m_readabilityGraphPropertyGrid->SetPropertyHelpString(
        GetFryLikeLabel(),
        _(L"The options in this section customize the Fry, Gilliam-Peña-Mountain, "
          "Raygor, and Schwartz graphs."));
    m_readabilityGraphPropertyGrid->Append(
        new wxColourProperty(GetInvalidRegionsColorLabel(), wxPG_LABEL,
                             ((m_readabilityProjectDoc != nullptr) ?
                                  m_readabilityProjectDoc->GetInvalidAreaColor() :
                                  wxGetApp().GetAppOptions()->GetInvalidAreaColor())));
    m_readabilityGraphPropertyGrid->SetPropertyHelpString(
        GetInvalidRegionsColorLabel(),
        _(L"Selects the color for the invalid sentence/word regions."));

    wxPGChoices raygorStyles;
    raygorStyles.Add(_(L"Original"));
    raygorStyles.Add(_DT(L"Baldwin-Kaufman"));
    raygorStyles.Add(_(L"Modern"));
    m_readabilityGraphPropertyGrid->Append(
        new wxEnumProperty(GetRaygorStyleLabel(), wxPG_LABEL, raygorStyles,
                           ((m_readabilityProjectDoc != nullptr) ?
                                static_cast<int>(m_readabilityProjectDoc->GetRaygorStyle()) :
                                static_cast<int>(wxGetApp().GetAppOptions()->GetRaygorStyle()))));
    m_readabilityGraphPropertyGrid->SetPropertyHelpString(
        GetRaygorStyleLabel(), _(L"Selects the layout style of the Raygor graph."));
    // flesch
    m_readabilityGraphPropertyGrid->Append(new wxPropertyCategory(GetFleschChartLabel()));
    m_readabilityGraphPropertyGrid->SetPropertyHelpString(
        GetFleschChartLabel(), _(L"The options in this section customize the Flesch chart."));
    m_readabilityGraphPropertyGrid->Append(
        new wxBoolProperty(GetFleschChartConnectPointsLabel(), wxPG_LABEL,
                           ((m_readabilityProjectDoc != nullptr) ?
                                m_readabilityProjectDoc->IsConnectingFleschPoints() :
                                wxGetApp().GetAppOptions()->IsConnectingFleschPoints())));
    m_readabilityGraphPropertyGrid->SetPropertyAttribute(GetFleschChartConnectPointsLabel(),
                                                         wxPG_BOOL_USE_CHECKBOX, true);
    m_readabilityGraphPropertyGrid->SetPropertyHelpString(
        GetFleschChartConnectPointsLabel(),
        _(L"Check this to connect the factor and score points."));

    m_readabilityGraphPropertyGrid->Append(
        new wxBoolProperty(GetFleschSyllableRulerDocGroupsLabel(), wxPG_LABEL,
                           ((m_readabilityProjectDoc != nullptr) ?
                                m_readabilityProjectDoc->IsIncludingFleschRulerDocGroups() :
                                wxGetApp().GetAppOptions()->IsIncludingFleschRulerDocGroups())));
    m_readabilityGraphPropertyGrid->SetPropertyAttribute(GetFleschSyllableRulerDocGroupsLabel(),
                                                         wxPG_BOOL_USE_CHECKBOX, true);
    m_readabilityGraphPropertyGrid->SetPropertyHelpString(
        GetFleschSyllableRulerDocGroupsLabel(),
        _(L"Check this to display document names grouped along the syllable ruler."));

    // Lix gauge
    m_readabilityGraphPropertyGrid->Append(new wxPropertyCategory(GetLixGaugeLabel()));
    m_readabilityGraphPropertyGrid->SetPropertyHelpString(
        GetLixGaugeLabel(), _(L"The options in this section customize the Lix Gauge."));
    m_readabilityGraphPropertyGrid->Append(
        new wxBoolProperty(GetUseEnglishLabelsForGermanLixLabel(), wxPG_LABEL,
                           ((m_readabilityProjectDoc != nullptr) ?
                                m_readabilityProjectDoc->IsUsingEnglishLabelsForGermanLix() :
                                wxGetApp().GetAppOptions()->IsUsingEnglishLabelsForGermanLix())));
    m_readabilityGraphPropertyGrid->SetPropertyAttribute(GetUseEnglishLabelsForGermanLixLabel(),
                                                         wxPG_BOOL_USE_CHECKBOX, true);
    m_readabilityGraphPropertyGrid->SetPropertyHelpString(
        GetUseEnglishLabelsForGermanLixLabel(),
        _(L"Check this to display the English translated bracket labels "
          "on the German Lix gauge."));

    pgMan->SelectProperty(GetFryLikeLabel());

    panelSizer->Add(pgMan, wxSizerFlags{ 1 }.Expand());
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateGraphBarChartSection()
    {
    if (!IsBatchProjectSettings())
        {
        auto* panel = new wxPanel(m_sideBar, GRAPH_BAR_CHART_PAGE, wxDefaultPosition, wxDefaultSize,
                                  wxTAB_TRAVERSAL);
        auto* panelSizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(panelSizer);
        m_sideBar->AddSubPage(panel, GetBarChartLabel(), GRAPH_BAR_CHART_PAGE, false, 9);

        auto* pgMan = new wxPropertyGridManager(
            panel, ID_BARCHART_OPTIONS_PROPERTYGRID, wxDefaultPosition, wxDefaultSize,
            wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);
        m_barChartPropertyGrid = pgMan->AddPage();
        m_barChartPropertyGrid->Append(new wxPropertyCategory(GetBarAppearanceLabel()));
        m_barChartPropertyGrid->SetPropertyHelpString(
            GetBarAppearanceLabel(),
            _(L"The options in this section customize the display of the bars."));

        // color for bars
        m_barChartPropertyGrid->Append(
            new wxColourProperty(GetColorLabel(), wxPG_LABEL,
                                 ((m_readabilityProjectDoc != nullptr) ?
                                      m_readabilityProjectDoc->GetBarChartBarColor() :
                                      wxGetApp().GetAppOptions()->GetBarChartBarColor())));
        m_barChartPropertyGrid->SetPropertyHelpString(GetColorLabel(),
                                                      _(L"Selects the color used for the bars."));
        // effects
        wxPGChoices boxEffects;
        boxEffects.Add(_(L"Solid"),
                       wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-solid.svg"));
        boxEffects.Add(_(L"Glass effect"),
                       wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-glass.svg"));
        boxEffects.Add(_(L"Color fade, bottom to top"),
                       wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-bottom-to-top.svg"));
        boxEffects.Add(_(L"Color fade, top to bottom"),
                       wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
        boxEffects.Add(_(L"Stipple image"),
                       wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
        boxEffects.Add(_(L"Stipple shape"),
                       wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
        boxEffects.Add(_(L"Watercolor"),
                       wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
        boxEffects.Add(_(L"Thick watercolor"),
                       wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
        boxEffects.Add(_(L"Common image"),
                       wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
        m_barChartPropertyGrid->Append(new wxEnumProperty(
            GetEffectLabel(), wxPG_LABEL, boxEffects,
            ((m_readabilityProjectDoc != nullptr) ?
                 static_cast<int>(m_readabilityProjectDoc->GetGraphBarEffect()) :
                 static_cast<int>(wxGetApp().GetAppOptions()->GetGraphBarEffect()))));
        m_barChartPropertyGrid->SetPropertyHelpString(
            GetEffectLabel(), _(L"Selects which effect to apply to the bars."));
        // bar opacity
        m_barChartPropertyGrid->Append(new wxIntProperty(
            GetOpacityLabel(), wxPG_LABEL,
            ((m_readabilityProjectDoc != nullptr) ?
                 static_cast<int>(m_readabilityProjectDoc->GetGraphBarOpacity()) :
                 static_cast<int>(wxGetApp().GetAppOptions()->GetGraphBarOpacity()))));
        m_barChartPropertyGrid->SetPropertyEditor(GetOpacityLabel(), wxPGEditor_SpinCtrl);
        m_barChartPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MIN,
                                                     wxALPHA_TRANSPARENT);
        m_barChartPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MAX,
                                                     wxALPHA_OPAQUE);
        m_barChartPropertyGrid->SetPropertyHelpString(
            GetOpacityLabel(),
            _(L"Sets the transparency of the bars. A value of 255 will set the box "
              "to be fully opaque, whereas 0 will set the bars to be transparent."));
        // orientation
        wxPGChoices orientation;
        orientation.Add(_(L"Horizontal"),
                        wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-chart.svg"));
        orientation.Add(_(L"Vertical"),
                        wxGetApp().GetResourceManager().GetSVG(L"ribbon/histogram.svg"));
        m_barChartPropertyGrid->Append(new wxEnumProperty(
            GeOrientationLabel(), wxPG_LABEL, orientation,
            ((m_readabilityProjectDoc != nullptr) ?
                 static_cast<int>(m_readabilityProjectDoc->GetBarChartOrientation()) :
                 static_cast<int>(wxGetApp().GetAppOptions()->GetBarChartOrientation()))));
        m_barChartPropertyGrid->SetPropertyHelpString(
            GeOrientationLabel(),
            _(L"Selects whether to display the bars horizontally or vertically."));

        // labels
        m_barChartPropertyGrid->Append(
            new wxBoolProperty(GetLabelsOnBarsLabel(), wxPG_LABEL,
                               ((m_readabilityProjectDoc != nullptr) ?
                                    m_readabilityProjectDoc->IsDisplayingBarChartLabels() :
                                    wxGetApp().GetAppOptions()->IsDisplayingBarChartLabels())));
        m_barChartPropertyGrid->SetPropertyAttribute(GetLabelsOnBarsLabel(), wxPG_BOOL_USE_CHECKBOX,
                                                     true);
        m_barChartPropertyGrid->SetPropertyHelpString(
            GetLabelsOnBarsLabel(), _(L"Check this to display labels on the bars."));

        pgMan->SelectProperty(GetBarAppearanceLabel());

        panelSizer->Add(pgMan, wxSizerFlags{ 1 }.Expand());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateGraphHistogramSection()
    {
    auto* panel = new wxPanel(m_sideBar, GRAPH_HISTOGRAM_PAGE, wxDefaultPosition, wxDefaultSize,
                              wxTAB_TRAVERSAL);
    auto* panelSizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(panelSizer);
    m_sideBar->AddSubPage(panel, GetHistogramsLabel(), GRAPH_HISTOGRAM_PAGE, false, 9);

    auto* pgMan =
        new wxPropertyGridManager(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                  wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);
    m_histogramPropertyGrid = pgMan->AddPage();
    m_histogramPropertyGrid->Append(new wxPropertyCategory(GetBarAppearanceLabel()));
    m_histogramPropertyGrid->SetPropertyHelpString(
        GetBarAppearanceLabel(),
        _(L"The options in this section customize the display of the bars."));
    // color for bars
    m_histogramPropertyGrid->Append(
        new wxColourProperty(GetColorLabel(), wxPG_LABEL,
                             ((m_readabilityProjectDoc != nullptr) ?
                                  m_readabilityProjectDoc->GetHistogramBarColor() :
                                  wxGetApp().GetAppOptions()->GetHistogramBarColor())));
    m_histogramPropertyGrid->SetPropertyHelpString(GetColorLabel(),
                                                   _(L"Selects the color used for the bars."));
    // effects
    wxPGChoices boxEffects;
    boxEffects.Add(_(L"Solid"), wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-solid.svg"));
    boxEffects.Add(_(L"Glass effect"),
                   wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-glass.svg"));
    boxEffects.Add(_(L"Color fade, bottom to top"),
                   wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-bottom-to-top.svg"));
    boxEffects.Add(_(L"Color fade, top to bottom"),
                   wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
    boxEffects.Add(_(L"Stipple image"),
                   wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    boxEffects.Add(_(L"Stipple shape"),
                   wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    boxEffects.Add(_(L"Watercolor"), wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    boxEffects.Add(_(L"Thick watercolor"),
                   wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    boxEffects.Add(_(L"Common image"), wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_histogramPropertyGrid->Append(new wxEnumProperty(
        GetEffectLabel(), wxPG_LABEL, boxEffects,
        ((m_readabilityProjectDoc != nullptr) ?
             static_cast<int>(m_readabilityProjectDoc->GetHistogramBarEffect()) :
             static_cast<int>(wxGetApp().GetAppOptions()->GetHistogramBarEffect()))));
    m_histogramPropertyGrid->SetPropertyHelpString(
        GetEffectLabel(), _(L"Selects which effect to apply to the bars."));
    // bar opacity
    m_histogramPropertyGrid->Append(
        new wxIntProperty(GetOpacityLabel(), wxPG_LABEL,
                          ((m_readabilityProjectDoc != nullptr) ?
                               m_readabilityProjectDoc->GetHistogramBarOpacity() :
                               wxGetApp().GetAppOptions()->GetHistogramBarOpacity())));
    m_histogramPropertyGrid->SetPropertyEditor(GetOpacityLabel(), wxPGEditor_SpinCtrl);
    m_histogramPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MIN,
                                                  wxALPHA_TRANSPARENT);
    m_histogramPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MAX, wxALPHA_OPAQUE);
    m_histogramPropertyGrid->SetPropertyHelpString(
        GetOpacityLabel(),
        _(L"Sets the transparency of the bars. A value of 255 will set the box to "
          "be fully opaque, whereas 0 will set the bars to be transparent."));

    if (IsBatchProjectSettings() || IsGeneralSettings())
        {
        m_histogramPropertyGrid->Append(new wxPropertyCategory(GetBinningOptionsLabel()));
        m_histogramPropertyGrid->SetPropertyHelpString(
            GetBinningOptionsLabel(),
            _(L"The options in this section control how the data are categorized."));

        // sorting
        wxPGChoices sortChoices;
        wxArrayString categories;
        categories.Add(_(L"Create a bin for each unique value"));
        categories.Add(_(L"Sort by interval"));
        categories.Add(_(L"Sort by neat interval"));
        sortChoices.Add(categories);
        m_histogramPropertyGrid->Append(new wxEnumProperty(
            GetBinSortingLabel(), wxPG_LABEL, sortChoices,
            ((m_readabilityProjectDoc != nullptr) ?
                 static_cast<int>(m_readabilityProjectDoc->GetHistogramBinningMethod()) :
                 static_cast<int>(wxGetApp().GetAppOptions()->GetHistogramBinningMethod()))));
        m_histogramPropertyGrid->SetPropertyHelpString(
            GetBinSortingLabel(),
            _(L"Binning refers to how values (e.g., index and grade scores) are "
              "categorized into separate classes. Each class (or bin) is displayed "
              "on a histogram as a bar. The options listed here control how these "
              "bins are created and how your data are sorted into them."));

        // rounding
        wxPGChoices roundingChoices;
        roundingChoices.Add(_(L"Round"));
        roundingChoices.Add(_(L"Round down"));
        roundingChoices.Add(_(L"Round up"));
        roundingChoices.Add(_(L"Do not round"));
        m_histogramPropertyGrid->Append(new wxEnumProperty(
            GetGradeLevelRoundingLabel(), wxPG_LABEL, roundingChoices,
            ((m_readabilityProjectDoc != nullptr) ?
                 static_cast<int>(m_readabilityProjectDoc->GetHistogramRoundingMethod()) :
                 static_cast<int>(wxGetApp().GetAppOptions()->GetHistogramRoundingMethod()))));
        m_histogramPropertyGrid->SetPropertyHelpString(
            GetGradeLevelRoundingLabel(), _(L"This option controls how floating-point values are "
                                            "rounded when being sorted into bins."));
        }

    m_histogramPropertyGrid->Append(new wxPropertyCategory(GetBinDisplayLabel()));
    m_histogramPropertyGrid->SetPropertyHelpString(
        GetBinDisplayLabel(), _(L"The options in this section control how the bars are labeled."));

    if (IsBatchProjectSettings() || IsGeneralSettings())
        {
        wxPGChoices intervalTypes;
        intervalTypes.Add(_(L"Cutpoints"));
        intervalTypes.Add(_(L"Midpoints"));
        m_histogramPropertyGrid->Append(new wxEnumProperty(
            GetIntervalDisplayLabel(), wxPG_LABEL, intervalTypes,
            ((m_readabilityProjectDoc != nullptr) ?
                 static_cast<int>(m_readabilityProjectDoc->GetHistogramIntervalDisplay()) :
                 static_cast<int>(wxGetApp().GetAppOptions()->GetHistogramIntervalDisplay()))));
        m_histogramPropertyGrid->SetPropertyHelpString(
            GetIntervalDisplayLabel(),
            _(L"Specifies how to display the bars' values range on the axis."));
        }

    wxPGChoices catLabelTypes;
    catLabelTypes.Add(_(L"Counts"));
    catLabelTypes.Add(_(L"Percentages"));
    catLabelTypes.Add(_(L"Counts & percentages"));
    catLabelTypes.Add(_(L"No labels"));
    m_histogramPropertyGrid->Append(new wxEnumProperty(
        GetBinLabelsLabel(), wxPG_LABEL, catLabelTypes,
        ((m_readabilityProjectDoc != nullptr) ?
             static_cast<int>(m_readabilityProjectDoc->GetHistogramBinLabelDisplay()) :
             static_cast<int>(wxGetApp().GetAppOptions()->GetHistogramBinLabelDisplay()))));
    m_histogramPropertyGrid->SetPropertyHelpString(
        GetBinLabelsLabel(), _(L"Specifies what to display on the bars' labels."));

    pgMan->SelectProperty(GetBarAppearanceLabel());

    panelSizer->Add(pgMan, wxSizerFlags{ 1 }.Expand());
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateGraphBoxPlotSection()
    {
    auto* panel = new wxPanel(m_sideBar, GRAPH_BOX_PLOT_PAGE, wxDefaultPosition, wxDefaultSize,
                              wxTAB_TRAVERSAL);
    auto* panelSizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(panelSizer);
    m_sideBar->AddSubPage(panel, GetBoxPlotLabel(), GRAPH_BOX_PLOT_PAGE, false, 9);

    auto* pgMan =
        new wxPropertyGridManager(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                  wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);
    m_boxPlotsPropertyGrid = pgMan->AddPage();
    m_boxPlotsPropertyGrid->Append(new wxPropertyCategory(GetBoxAppearanceLabel()));
    m_boxPlotsPropertyGrid->SetPropertyHelpString(
        GetBoxAppearanceLabel(),
        _(L"The options in this section customize the display of the boxes."));
    // color for box
    m_boxPlotsPropertyGrid->Append(new wxColourProperty(
        GetColorLabel(), wxPG_LABEL,
        ((m_readabilityProjectDoc != nullptr) ? m_readabilityProjectDoc->GetGraphBoxColor() :
                                                wxGetApp().GetAppOptions()->GetGraphBoxColor())));
    m_boxPlotsPropertyGrid->SetPropertyHelpString(GetColorLabel(),
                                                  _(L"Selects the color used for the boxes."));
    // effects
    wxPGChoices boxEffects;
    boxEffects.Add(_(L"Solid"), wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-solid.svg"));
    boxEffects.Add(_(L"Glass effect"),
                   wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-glass.svg"));
    boxEffects.Add(_(L"Color fade, left to right"),
                   wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-left-to-right.svg"));
    boxEffects.Add(_(L"Color fade, right to left"),
                   wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-right-to-left.svg"));
    boxEffects.Add(_(L"Stipple image"),
                   wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    boxEffects.Add(_(L"Stipple shape"),
                   wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    boxEffects.Add(_(L"Watercolor"), wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    boxEffects.Add(_(L"Thick watercolor"),
                   wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    boxEffects.Add(_(L"Common image"), wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_boxPlotsPropertyGrid->Append(new wxEnumProperty(
        GetEffectLabel(), wxPG_LABEL, boxEffects,
        ((m_readabilityProjectDoc != nullptr) ?
             static_cast<int>(m_readabilityProjectDoc->GetGraphBoxEffect()) :
             static_cast<int>(wxGetApp().GetAppOptions()->GetGraphBoxEffect()))));
    m_boxPlotsPropertyGrid->SetPropertyHelpString(
        GetEffectLabel(), _(L"Selects which effect to apply to the boxes."));
    // box opacity
    m_boxPlotsPropertyGrid->Append(new wxIntProperty(
        GetOpacityLabel(), wxPG_LABEL,
        ((m_readabilityProjectDoc != nullptr) ? m_readabilityProjectDoc->GetGraphBoxOpacity() :
                                                wxGetApp().GetAppOptions()->GetGraphBoxOpacity())));
    m_boxPlotsPropertyGrid->SetPropertyEditor(GetOpacityLabel(), wxPGEditor_SpinCtrl);
    m_boxPlotsPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MIN,
                                                 wxALPHA_TRANSPARENT);
    m_boxPlotsPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MAX, wxALPHA_OPAQUE);
    m_boxPlotsPropertyGrid->SetPropertyHelpString(
        GetOpacityLabel(), _(L"Sets the transparency of the box. A value of 255 will set the box "
                             "to be fully opaque, whereas 0 will set the box to be transparent."));

    m_boxPlotsPropertyGrid->Append(new wxPropertyCategory(GetBoxOptionsLabel()));
    m_boxPlotsPropertyGrid->SetPropertyHelpString(
        GetBoxOptionsLabel(),
        _(L"The options in this section customize the display of the labels and data."));

    m_boxPlotsPropertyGrid->Append(
        new wxBoolProperty(GetLabelsOnBoxesLabel(), wxPG_LABEL,
                           ((m_readabilityProjectDoc != nullptr) ?
                                m_readabilityProjectDoc->IsDisplayingBoxPlotLabels() :
                                wxGetApp().GetAppOptions()->IsDisplayingBoxPlotLabels())));
    m_boxPlotsPropertyGrid->SetPropertyAttribute(GetLabelsOnBoxesLabel(), wxPG_BOOL_USE_CHECKBOX,
                                                 true);
    m_boxPlotsPropertyGrid->SetPropertyHelpString(
        GetLabelsOnBoxesLabel(), _(L"Check this to display labels on the middle points, "
                                   "upper/lower control limits, and whiskers."));

    m_boxPlotsPropertyGrid->Append(
        new wxBoolProperty(GetConnectBoxesLabel(), wxPG_LABEL,
                           ((m_readabilityProjectDoc != nullptr) ?
                                m_readabilityProjectDoc->IsConnectingBoxPlotMiddlePoints() :
                                wxGetApp().GetAppOptions()->IsConnectingBoxPlotMiddlePoints())));
    m_boxPlotsPropertyGrid->SetPropertyAttribute(GetConnectBoxesLabel(), wxPG_BOOL_USE_CHECKBOX,
                                                 true);
    m_boxPlotsPropertyGrid->SetPropertyHelpString(
        GetConnectBoxesLabel(),
        _(L"Check this to display a line connecting the middle points of each box. "
          "This only applies to plots with multiple boxes."));

    m_boxPlotsPropertyGrid->Append(
        new wxBoolProperty(GetShowAllDataPointsLabel(), wxPG_LABEL,
                           ((m_readabilityProjectDoc != nullptr) ?
                                m_readabilityProjectDoc->IsShowingAllBoxPlotPoints() :
                                wxGetApp().GetAppOptions()->IsShowingAllBoxPlotPoints())));
    m_boxPlotsPropertyGrid->SetPropertyAttribute(GetShowAllDataPointsLabel(),
                                                 wxPG_BOOL_USE_CHECKBOX, true);
    m_boxPlotsPropertyGrid->SetPropertyHelpString(
        GetShowAllDataPointsLabel(),
        _(L"Check this to display all data points on the box and whiskers. "
          "If this is unchecked, then only outliers will be shown."));

    pgMan->SelectProperty(GetBoxAppearanceLabel());

    panelSizer->Add(pgMan, wxSizerFlags{ 1 }.Expand());
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateGraphSection()
    {
    if ((GetSectionsBeingShown() & GraphsSection) != 0)
        {
        CreateGraphGeneralSection();
        CreateGraphAxesSection();
        CreateGraphTitlesSection();
        CreateGraphReadabilitySection();
        CreateGraphBarChartSection();
        CreateGraphHistogramSection();
        CreateGraphBoxPlotSection();
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnContextHelp([[maybe_unused]] wxHelpEvent& event)
    {
    wxCommandEvent cmd;
    OnHelp(cmd);
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnHelp([[maybe_unused]] wxCommandEvent& event)
    {
    wxLaunchDefaultBrowser(wxFileName::FileNameToURL(wxGetApp().GetMainFrame()->GetHelpDirectory() +
                                                     wxFileName::GetPathSeparator() +
                                                     L"online/program-options.html"));
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateLabelHeader(wxWindow* parent, wxSizer* parentSizer,
                                        const wxString& title,
                                        const bool addSidePadding /*= false*/)
    {
    wxASSERT_MSG(parent && parentSizer, L"Invalid parent or parent sizer!");
    if ((parent == nullptr) || (parentSizer == nullptr))
        {
        return;
        }
    // The title
    auto* titleSizer = new wxBoxSizer(wxHORIZONTAL);
    auto* titleText =
        new wxStaticText(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    titleSizer->Add(titleText);
    titleSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
    // The static line
    titleSizer->Add(
        new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),
        wxSizerFlags{ 1 }.CenterVertical());
    if (addSidePadding)
        {
        parentSizer->Add(titleSizer, wxSizerFlags{}.Expand().Border(wxLEFT | wxRIGHT));
        }
    else
        {
        parentSizer->Add(titleSizer, wxSizerFlags{}.Expand());
        }
    parentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::SelectPage(const int pageId)
    {
    for (size_t i = 0; i < m_sideBar->GetPageCount(); ++i)
        {
        const wxWindow* page = m_sideBar->GetPage(i);
        if ((page != nullptr) && page->GetId() == pageId)
            {
            m_sideBar->SetSelection(i);
            }
        }
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::IsGeneralSettings() const noexcept
    {
    return m_readabilityProjectDoc == nullptr;
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::IsStandardProjectSettings() const
    {
    return ((m_readabilityProjectDoc != nullptr) &&
            m_readabilityProjectDoc->IsKindOf(CLASSINFO(ProjectDoc)));
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::IsBatchProjectSettings() const
    {
    return ((m_readabilityProjectDoc != nullptr) &&
            m_readabilityProjectDoc->IsKindOf(CLASSINFO(BatchProjectDoc)));
    }

//-------------------------------------------------------------
wxString ToolsOptionsDlg::ExpandPath(wxString path)
    {
    if (path.starts_with(_DT(L"@[EXAMPLESDIR]")))
        {
        path.Replace(_DT(L"@[EXAMPLESDIR]"),
                     wxGetApp().FindResourceDirectory(_DT(L"examples", DTExplanation::FilePath)));
        }
    return path;
    }
