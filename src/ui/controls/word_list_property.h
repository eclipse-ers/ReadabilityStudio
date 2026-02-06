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

#ifndef PROPGRID_WORDLIST_H
#define PROPGRID_WORDLIST_H

#include "../dialogs/edit_word_list_dlg.h"
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/property.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propgriddefs.h>
#include <wx/propgrid/props.h>
#include <wx/wx.h>

#define wxPG_HELP_PATH L"wxPG_HELP_PATH"
#define wxPG_TOPIC_PATH L"wxPG_TOPIC_PATH"
#define wxPG_PHRASE_MODE L"wxPG_PHRASE_MODE"

/// @brief Property grid feature for editing a word list.
class WordListProperty final : public wxLongStringProperty
    {
    wxDECLARE_DYNAMIC_CLASS(WordListProperty);

  public:
    /** @brief Constructor.
        @param label The label to display in the property grid.
        @param name The property name.
        @param value The file path to the word list.*/
    explicit WordListProperty(const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
                              const wxString& value = wxString{});
    /// @private
    [[nodiscard]]
    wxValidator* DoGetValidator() const final;
    /// @private
    bool DoSetAttribute(const wxString& name, wxVariant& value) final;
    /// @private
    bool DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value) final;

  private:
    wxString m_helpPath;
    wxString m_topicPath;
    bool m_phraseMode{ false };
    };

#endif // PROPGRID_WORDLIST_H
