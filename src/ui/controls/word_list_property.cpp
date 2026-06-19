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

#include "word_list_property.h"

wxIMPLEMENT_DYNAMIC_CLASS(WordListProperty, wxLongStringProperty)

    //------------------------------------------------------
    WordListProperty::WordListProperty(const wxString& label /*= wxPG_LABEL*/,
                                       const wxString& name /*= wxPG_LABEL*/,
                                       const wxString& value /*= wxString{}*/)
    : wxLongStringProperty(label, name, value)
    {
    }

//------------------------------------------------------
wxValidator* WordListProperty::DoGetValidator() const
    {
    return wxFileProperty::GetClassValidator();
    }

//------------------------------------------------------
bool WordListProperty::DoSetAttribute(const wxString& name, wxVariant& value)
    {
    if (name == wxPG_HELP_PATH)
        {
        m_helpPath = value.GetString();
        return true;
        }
    if (name == wxPG_TOPIC_PATH)
        {
        m_topicPath = value.GetString();
        return true;
        }
    if (name == wxPG_PHRASE_MODE)
        {
        m_phraseMode = value.GetBool();
        return true;
        }
    return wxLongStringProperty::DoSetAttribute(name, value);
    }

//------------------------------------------------------
bool WordListProperty::DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value)
    {
    wxASSERT_MSG(value.IsType(_DT("string")), "Function called for incompatible property");

    EditWordListDlg editDlg(pg->GetPanel(), wxID_ANY,
                            !m_dlgTitle.empty() ? m_dlgTitle : _(L"Edit Word List"));
    if (!m_helpPath.empty())
        {
        editDlg.SetHelpTopic(m_helpPath, m_topicPath);
        }
    editDlg.SetPhraseFileMode(m_phraseMode);
    editDlg.SetFilePath(value.GetString());
    if (editDlg.ShowModal() == wxID_OK)
        {
        value = editDlg.GetFilePath();
        return true;
        }
    return false;
    }
