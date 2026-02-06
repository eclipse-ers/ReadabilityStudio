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

#include "word_functional.h"

// documents
std::set<traits::case_insensitive_wstring_ex> is_file_extension::m_file_extensions = { // NOLINT
    L"xml", L"html", L"htm", L"xhtml", L"rtf", L"doc", L"docx", L"dot", L"docm", L"txt", L"ppt",
    L"pptx", L"pdf", L"ps", L"odt", L"ott", L"odp", L"otp", L"pptm", L"md", L"xaml",
    // Visual Studio files
    L"sln", L"csproj", L"json", L"pbxproj", L"apk", L"tlb", L"ocx", L"pdb", L"tlh", L"hlp", L"msi",
    L"rc", L"vcxproj", L"resx", L"appx", L"vcproj",
    // macOS
    L"dmg", L"proj", L"xbuild", L"xmlns",
    // Database
    L"mdb", L"db",
    // Markdown files
    L"md", L"Rmd", L"qmd", L"yml",
    // help files
    L"hhc", L"hhk", L"hhp",
    // spreadsheets
    L"xls", L"xlsx", L"ods", L"csv",
    // image formats
    L"gif", L"jpg", L"jpeg", L"jpe", L"bmp", L"tiff", L"tif", L"png", L"tga", L"svg", L"xcf",
    L"ico", L"psd", L"hdr", L"pcx",
    // webpages
    L"asp", L"aspx", L"cfm", L"cfml", L"php", L"php3", L"php4", L"sgml", L"wmf", L"js",
    // style sheets
    L"css",
    // movies
    L"mov", L"qt", L"rv", L"rm", L"wmv", L"mpg", L"mpeg", L"mpe", L"avi",
    // music
    L"mp3", L"wav", L"wma", L"midi", L"ra", L"ram",
    // programs
    L"exe", L"swf", L"vbs",
    // source files
    L"cpp", L"h", L"c", L"idl", L"cs", L"hpp", L"po",
    // compressed files
    L"gzip", L"bz2"
};
