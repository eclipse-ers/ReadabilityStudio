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

#include "edit_word_list_dlg.h"
#include "../../Wisteria-Dataviz/src/utfcpp/source/utf8.h"
#include "../../Wisteria-Dataviz/src/util/textstream.h"
#include "../../indexing/phrase.h"
#include <wx/file.h>
#include <wx/tokenzr.h>

//---------------------------------------------
EditWordListDlg::EditWordListDlg(
                                wxWindow* parent, wxWindowID id /*= wxID_ANY*/,
                                            const wxString& caption /*= _(L"Edit List")*/,
                                            const wxPoint& pos /*= wxDefaultPosition*/,
                                            const wxSize& size /*= wxSize(1000, 1000)*/,
                                            long style /*= wxDEFAULT_DIALOG_STYLE |
                                                           wxCLIP_CHILDREN | wxRESIZE_BORDER*/)
    {
    Create(parent, id, caption, pos, size, style);

    Bind(wxEVT_BUTTON, &EditWordListDlg::OnOK, this, wxID_OK);
    Bind(wxEVT_BUTTON, &EditWordListDlg::OnClose, this, wxID_CLOSE);
    Bind(wxEVT_BUTTON, &EditWordListDlg::OnClose, this, wxID_CANCEL);
    Bind(wxEVT_BUTTON, &EditWordListDlg::OnAddItem, this, EditWordListDlg::ID_ADD_ITEM);
    Bind(wxEVT_BUTTON, &EditWordListDlg::OnEditItem, this, EditWordListDlg::ID_EDIT_ITEM);
    Bind(wxEVT_BUTTON, &EditWordListDlg::OnDeleteItem, this, EditWordListDlg::ID_DELETE_ITEM);
    Bind(wxEVT_BUTTON, &EditWordListDlg::OnBrowseForFileClick, this,
         EditWordListDlg::ID_BROWSE_FOR_FILE);
    Bind(wxEVT_TEXT, &EditWordListDlg::OnFilePathChanged, this,
         EditWordListDlg::ID_FILE_PATH_FIELD);
    }

//---------------------------------------------
bool EditWordListDlg::Save(const wxString& filePath)
    {
    // if empty then get the user to save the word list
    if (filePath.empty())
        {
        TransferDataFromWindow();
        wxFileName fn(m_wordListFilePath);
        wxFileDialog dialog(this, _(L"Save Word List"), fn.GetPath(), fn.GetFullName(),
                            _(L"Text files (*.txt)|*.txt"), wxFD_SAVE | wxFD_PREVIEW);
        if (dialog.ShowModal() != wxID_OK)
            {
            return false;
            }

        m_wordListFilePath = dialog.GetPath();
        TransferDataToWindow();
        }

    wxString outputStr;

    if (m_wordData->GetColumnCount() == 1)
        {
        std::vector<OutputStringType> outputWords;
        outputWords.resize(m_wordData->GetItemCount());
        for (size_t i = 0; i < m_wordData->GetItemCount(); ++i)
            {
            outputWords[i] = m_wordData->GetItemText(i, 0);
            }
        // sort and remove duplicates
        std::ranges::sort(outputWords);
        std::vector<OutputStringType>::iterator endOfUniquePos =
            std::unique(outputWords.begin(), outputWords.end());
        if (endOfUniquePos != outputWords.end())
            {
            outputWords.erase(endOfUniquePos, outputWords.end());
            }
        // save the new list back to the original file
        outputStr.reserve(outputWords.size() * 5);
        for (size_t i = 0; i < outputWords.size(); ++i)
            {
            outputStr.append(outputWords[i].c_str()).append(L"\r\n");
            }
        }
    else
        {
        m_wordsList->FormatToText(outputStr,
                                  Wisteria::UI::ListCtrlEx::ExportRowSelection::ExportAll, 0, -1, 0,
                                  -1, false);
        grammar::phrase_collection phrases;
        phrases.load_phrases(outputStr, true, false);
        phrases.remove_duplicates();

        outputStr = phrases.to_string();
        }

    wxFileName(filePath).SetPermissions(wxS_DEFAULT);
    wxFile outputFile(filePath, wxFile::write);
    if (!outputFile.IsOpened())
        {
        wxMessageBox(
            wxString::Format(
                _(L"Unable to save \"%s\".\n"
                  "Verify that you have write access to this file or that it is not in use."),
                filePath),
            _(L"Error"), wxOK | wxICON_ERROR);
        return false;
        }
    return outputFile.Write(outputStr, wxConvUTF8);
    }

//---------------------------------------------
void EditWordListDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    wxBusyCursor wait;
    TransferDataFromWindow();

    if (m_wordsList->HasItemBeenEditedByUser() && !Save(m_wordListFilePath))
        {
        return;
        }

    if (IsModal())
        {
        EndModal(wxID_OK);
        }
    else
        {
        Show(false);
        }
    }

//---------------------------------------------
void EditWordListDlg::OnClose([[maybe_unused]] wxCommandEvent& event)
    {
    wxBusyCursor wait;
    TransferDataFromWindow();

    if (m_wordsList->HasItemBeenEditedByUser() &&
        wxMessageBox(_(L"The list have been edited. Do you wish to save your changes?"),
                     _(L"Save Changes"), wxYES_NO | wxICON_QUESTION) == wxYES)
        {
        Save(m_wordListFilePath);
        }

    if (IsModal())
        {
        EndModal(wxID_CLOSE);
        }
    else
        {
        Show(false);
        }
    }

//------------------------------------------------------
bool EditWordListDlg::Create(wxWindow* parent, wxWindowID id, const wxString& caption,
                             const wxPoint& pos, const wxSize& size, long style)
    {
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);
    DialogWithHelp::Create(parent, id, caption, pos, size, style);

    CreateControls();
    Centre();
    return true;
    }

//------------------------------------------------------
void EditWordListDlg::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBox* wordFilePathBox = new wxStaticBox(this, wxID_ANY, _(L"File containing list:"));
    wxStaticBoxSizer* wordFilePathSizer = new wxStaticBoxSizer(wordFilePathBox, wxHORIZONTAL);
    mainSizer->Add(wordFilePathSizer, wxSizerFlags{}.Expand().Border(wxTOP | wxLEFT | wxRIGHT));

    m_wordListFilePathCtrl = new wxTextCtrl(
        wordFilePathSizer->GetStaticBox(), ID_FILE_PATH_FIELD, wxString{}, wxDefaultPosition,
        wxDefaultSize, wxBORDER_THEME, wxTextValidator(wxFILTER_NONE, &m_wordListFilePath));
    m_wordListFilePathCtrl->AutoCompleteFileNames();
    wordFilePathSizer->Add(m_wordListFilePathCtrl,
                           wxSizerFlags{ 1 }.Expand().Border(wxLEFT | wxTOP | wxBOTTOM));

    wordFilePathSizer->Add(
        new wxBitmapButton(
            wordFilePathSizer->GetStaticBox(), ID_BROWSE_FOR_FILE,
            wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_BUTTON, FromDIP(wxSize{ 16, 16 }))),
        wxSizerFlags{}.CenterVertical().Border(wxRIGHT | wxTOP | wxBOTTOM));

    wxBoxSizer* wordListSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(wordListSizer, wxSizerFlags{ 1 }.Expand().Border());
    wxBoxSizer* toolbarSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBitmapButton* listButton = new wxBitmapButton(
        this, ID_ADD_ITEM,
        wxArtProvider::GetBitmap(L"ID_ADD", wxART_BUTTON, FromDIP(wxSize{ 16, 16 })));
    listButton->SetToolTip(_(L"Add a new item"));
    toolbarSizer->Add(listButton);
    listButton = new wxBitmapButton(
        this, ID_EDIT_ITEM,
        wxArtProvider::GetBitmap(L"ID_EDIT", wxART_BUTTON, FromDIP(wxSize{ 16, 16 })));
    listButton->SetToolTip(_(L"Edit selected item"));
    toolbarSizer->Add(listButton);
    listButton = new wxBitmapButton(
        this, ID_DELETE_ITEM,
        wxArtProvider::GetBitmap(wxART_DELETE, wxART_BUTTON, FromDIP(wxSize{ 16, 16 })));
    listButton->SetToolTip(_(L"Remove selected items"));
    toolbarSizer->Add(listButton);
    wordListSizer->Add(toolbarSizer, 0, wxALIGN_RIGHT);

    // the word list
    m_wordsList = new Wisteria::UI::ListCtrlEx(
        this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize{ 400, 300 }),
        wxLC_VIRTUAL | wxLC_EDIT_LABELS | wxLC_REPORT | wxLC_ALIGN_LEFT | wxBORDER_THEME);
    m_wordsList->EnableGridLines();
    m_wordsList->InsertColumn(0, wxString{});
    m_wordsList->SetColumnEditable(0);
    m_wordsList->EnableItemAdd();
    m_wordsList->EnableLabelEditing();
    m_wordsList->EnableItemDeletion();
    m_wordsList->SetVirtualDataProvider(m_wordData);
    m_wordsList->SetVirtualDataSize(1, 1);
    m_wordsList->Refresh();
    wordListSizer->Add(m_wordsList, wxSizerFlags{ 1 }.Expand());

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxHELP),
                   wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainSizer);
    }

//---------------------------------------------
void EditWordListDlg::OnFilePathChanged(wxCommandEvent& event)
    {
    TransferDataFromWindow();

    if (wxFile::Exists(m_wordListFilePath) &&
        CompareFilePaths(m_previousListFilePath, m_wordListFilePath) != 0)
        {
        // if switching the file and the user had edited this list...
        if (wxFile::Exists(m_previousListFilePath) && m_wordsList->HasItemBeenEditedByUser())
            {
            // ...then ask if they should save any changes...
            if (wxMessageBox(_(L"The list have been edited. Do you wish to save your changes?"),
                             _(L"Save Changes"), wxYES_NO | wxICON_QUESTION) == wxYES)
                {
                Save(m_previousListFilePath);
                }
            }

        wxString buffer;
        if (!Wisteria::TextStream::ReadFile(m_wordListFilePath, buffer))
            {
            wxMessageBox(_(L"Error loading word list file."), _(L"Error"),
                         wxOK | wxICON_EXCLAMATION);
            return;
            }

        wxBusyCursor wait;

        wxWindowUpdateLocker noUpdates(m_wordsList);
        if (m_phraseFileMode)
            {
            lily_of_the_valley::standard_delimited_character_column tabbedColumn(
                lily_of_the_valley::text_column_delimited_character_parser{ L'\t' }, 5);
            lily_of_the_valley::text_row<Wisteria::UI::ListCtrlExDataProvider::ListCellString> row(
                std::nullopt);
            row.treat_consecutive_delimiters_as_one(false);
            row.add_column(tabbedColumn);

            lily_of_the_valley::text_matrix<Wisteria::UI::ListCtrlExDataProvider::ListCellString>
                importer(&m_wordData->GetMatrix());
            importer.add_row_definition(row);

            // see how many lines (which will be individual phrases) are in the file
            lily_of_the_valley::text_preview preview;
            size_t rowCount = preview(buffer, L'\t', true, true);
            size_t maxColumnCount{ 1 };
            wxStringTokenizer tokenizer;
            for (auto rowPos = preview.get_line_info().cbegin();
                 rowPos != preview.get_line_info().cend(); ++rowPos)
                {
                tokenizer.SetString(wxString(rowPos->first, (rowPos->second - rowPos->first)),
                                    L'\t', wxTOKEN_RET_EMPTY_ALL);
                maxColumnCount =
                    std::min<size_t>(std::max(tokenizer.CountTokens(), maxColumnCount), 5);
                }
            m_wordData->SetSize(rowCount, 5);
            // now read it
            rowCount = importer.read(buffer, rowCount, 5, true);

            m_wordsList->DeleteAllColumns();
            m_wordsList->InsertColumn(0, _(L"Word/Phrase"));
            if (maxColumnCount > 1)
                {
                m_wordsList->InsertColumn(1, _(L"Suggestion"));
                }
            if (maxColumnCount > 2)
                {
                m_wordsList->InsertColumn(2, _(L"Type"));
                }
            if (maxColumnCount > 3)
                {
                // TRANSLATORS: An exception for a word before a phrase that negates
                // a grammar check.
                m_wordsList->InsertColumn(3, _(L"Proceeding Exception"));
                }
            if (maxColumnCount > 4)
                {
                // TRANSLATORS: An exception for a word after a phrase that negates
                // a grammar check.
                m_wordsList->InsertColumn(4, _(L"Trailing Exception"));
                }
            m_wordsList->SetVirtualDataSize(rowCount, 5);
            m_wordsList->SetColumnEditable(0);
            m_wordsList->SetColumnEditable(1);
            m_wordsList->SetColumnEditable(2);
            m_wordsList->SetColumnEditable(3);
            m_wordsList->SetColumnEditable(4);
            m_wordsList->DistributeColumns();
            }
        else
            {
            m_wordData->DeleteAllItems();

            string_util::string_tokenize<wxString> tkzr(buffer, L" \t\n\r;,", true);
            const auto tokenCount = tkzr.count_tokens(buffer);
            m_wordData->SetSize(tokenCount);

            size_t currentRow{ 0 };
            while (tkzr.has_more_tokens())
                {
                if (m_wordData->GetItemCount() < currentRow + 1)
                    {
                    m_wordData->SetSize(currentRow + 1);
                    }
                m_wordData->SetItemText(currentRow++, 0, tkzr.get_next_token());
                }

            m_wordData->SetSize(currentRow, 1);
            m_wordsList->DeleteAllColumns();
            m_wordsList->InsertColumn(0, _(L"Word"), wxLIST_FORMAT_LEFT,
                                      m_wordsList->GetClientSize().GetWidth());
            m_wordsList->SetVirtualDataSize(currentRow, 1);
            }

        m_wordsList->Refresh();
        m_wordsList->SetItemBeenEditedByUser(false);
        m_previousListFilePath = m_wordListFilePath;
        }
    event.Skip(true);
    }

//---------------------------------------------
void EditWordListDlg::OnAddItem([[maybe_unused]] wxCommandEvent& event)
    {
    m_wordsList->EditItem(m_wordsList->AddRow(), 0);
    }

//---------------------------------------------
void EditWordListDlg::OnEditItem([[maybe_unused]] wxCommandEvent& event)
    {
    m_wordsList->EditItem(m_wordsList->GetFocusedItem(), 0);
    }

//---------------------------------------------
void EditWordListDlg::OnDeleteItem([[maybe_unused]] wxCommandEvent& event)
    {
    m_wordsList->DeleteSelectedItems();
    }

//---------------------------------------------
void EditWordListDlg::OnBrowseForFileClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    wxFileDialog dialog(this, _(L"Select Word List"),
                        m_wordListFilePath.length() ? wxString{} : m_defaultDir, m_wordListFilePath,
                        _(L"Text files (*.txt)|*.txt"), wxFD_OPEN | wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    m_wordListFilePath = dialog.GetPath();
    TransferDataToWindow();
    }
