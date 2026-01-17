#############################################################################
# Name:        BuildG11NDashboard.txt
# Purpose:     Build script for Readability Studio
# Author:      Blake Madden
# Created:     2025-10-26
# Copyright:   (c) 2025 Blake Madden
# License:     Eclipse Public License 2.0
#############################################################################

# Globalization dashboard generator (l10n + i18n)
set(MD "")
include("${CMAKE_CURRENT_LIST_DIR}/DashboardCommon.cmake")
include("${CMAKE_BINARY_DIR}/translation_stats.cmake")

# ----- Inputs -----
if(NOT DEFINED ROOT)
  set(ROOT "${CMAKE_SOURCE_DIR}")
endif()

if(NOT DEFINED STATUS_DIR)
  set(STATUS_DIR "${ROOT}/project-management/g11n/status")
endif()

if(NOT DEFINED STATUS_DIR_I18N)
  set(STATUS_DIR_I18N "${ROOT}/project-management/g11n/status/i18n")
endif()

if(NOT DEFINED OUT_MD)
  set(OUT_MD "${ROOT}/project-management/g11n/DASHBOARD.md")
endif()

if(NOT DEFINED PROJECT_NAME)
  set(PROJECT_NAME "${CMAKE_PROJECT_NAME}")
endif()

# Normalize paths (Windows safe)
string(REPLACE "\\" "/" STATUS_DIR "${STATUS_DIR}")
string(REPLACE "\\" "/" STATUS_DIR_I18N "${STATUS_DIR_I18N}")
string(REPLACE "\\" "/" OUT_MD "${OUT_MD}")

# L10n files (per-language)
set(UI_JSON       "${STATUS_DIR}/ui.json")
set(DOCS_JSON     "${STATUS_DIR}/docs.json")
set(WIN_JSON      "${STATUS_DIR}/windows-installer.json")
set(ART_JSON      "${STATUS_DIR}/art.json")
set(LICENSE_JSON  "${STATUS_DIR}/license.json")

# I18n files (single number + comment)
set(I18N_UI_JSON      "${STATUS_DIR_I18N}/ui.json")
set(I18N_DOCS_JSON    "${STATUS_DIR_I18N}/docs.json")
set(I18N_WIN_JSON     "${STATUS_DIR_I18N}/windows-installer.json")
set(I18N_ART_JSON     "${STATUS_DIR_I18N}/art.json")
set(I18N_LICENSE_JSON "${STATUS_DIR_I18N}/license.json")

# ----- Load L10n JSONs -----
set(SEEN_LANGS "")
_read_json("${UI_JSON}"      UI)
_read_json("${DOCS_JSON}"    DOCS)
_read_json("${WIN_JSON}"     WIN)
_read_json("${ART_JSON}"     ART)
_read_json("${LICENSE_JSON}" LICENSE)

list(REMOVE_DUPLICATES SEEN_LANGS)
set(LANGS ${SEEN_LANGS})
list(SORT LANGS)
list(JOIN LANGS "," _langs_joined)
message(STATUS "[G11N] LANGS detected: ${_langs_joined}")

string(REPLACE "," ";" _wlist "${WEIGHTS}")
foreach(tok IN LISTS _wlist)
  string(REPLACE "=" ";" kv "${tok}")
  list(LENGTH kv _kvlen)
  if(_kvlen EQUAL 2)
    list(GET kv 0 k)
    list(GET kv 1 v)
    string(TOLOWER "${k}" k)
    if(k STREQUAL "ui")
      set(W_UI "${v}")
    elseif(k STREQUAL "docs")
      set(W_DOCS "${v}")
    elseif(k STREQUAL "installer")
      set(W_INST "${v}")
    elseif(k STREQUAL "art")
      set(W_ART "${v}")
    elseif(k STREQUAL "license")
      set(W_LICENSE "${v}")
    endif()
  endif()
endforeach()

math(EXPR W_SUM "${W_UI}+${W_DOCS}+${W_INST}+${W_ART}+${W_LICENSE}")
if(W_SUM EQUAL 0)
  message(FATAL_ERROR "All weights are zero; nothing to average.")
endif()

# ----- Compute L10n per-language, cache values -----
set(OVERALL_NUM 0)
set(OVERALL_DEN 0)

set(SUM_UI 0)
set(SUM_DOCS 0)
set(SUM_INST 0)
set(SUM_ART 0)
set(SUM_LICENSE 0)

foreach(L IN LISTS LANGS)
  _val_or(UI_${L} 0)
  set(p_ui "${RET}")

  _val_or(DOCS_${L} 0)
  set(p_docs "${RET}")

  _val_or(WIN_${L} 0)
  set(p_win "${RET}")
  set(p_inst "${p_win}")  # installer = Windows for now

  _val_or(ART_${L} 0)
  set(p_art "${RET}")

  _val_or(LICENSE_${L} 0)
  set(p_license "${RET}")

  math(EXPR num "${p_ui}*${W_UI} + ${p_docs}*${W_DOCS} + ${p_inst}*${W_INST} + ${p_art}*${W_ART} + ${p_license}*${W_LICENSE}")
  math(EXPR p_all "${num} / ${W_SUM}")

  _bar10(bar "${p_all}")
  _color_for(color "${p_all}")

  math(EXPR SUM_UI      "${SUM_UI}+${p_ui}")
  math(EXPR SUM_DOCS    "${SUM_DOCS}+${p_docs}")
  math(EXPR SUM_INST    "${SUM_INST}+${p_inst}")
  math(EXPR SUM_ART     "${SUM_ART}+${p_art}")
  math(EXPR SUM_LICENSE "${SUM_LICENSE}+${p_license}")

  set(UI_PCT_${L}      "${p_ui}")
  set(DOCS_PCT_${L}    "${p_docs}")
  set(INST_PCT_${L}    "${p_inst}")
  set(LICENSE_PCT_${L} "${p_license}")
  set(ART_PCT_${L}     "${p_art}")
  set(PALL_${L} "${p_all}")
  set(BAR_${L}  "${bar}")

  math(EXPR OVERALL_NUM "${OVERALL_NUM}+${p_all}")
  math(EXPR OVERALL_DEN "${OVERALL_DEN}+1")
endforeach()

# L10n overall (across languages)
if(OVERALL_DEN EQUAL 0)
  set(OVERALL 0)
else()
  math(EXPR OVERALL "${OVERALL_NUM}/${OVERALL_DEN}")
endif()
_bar10(OVERALL_BAR "${OVERALL}")
_color_for(OVERALL_COLOR "${OVERALL}")

if(OVERALL_DEN EQUAL 0)
  set(AVG_UI 0)
  set(AVG_DOCS 0)
  set(AVG_INST 0)
  set(AVG_ART 0)
  set(AVG_LICENSE 0)
else()
  math(EXPR AVG_UI      "${SUM_UI}/${OVERALL_DEN}")
  math(EXPR AVG_DOCS    "${SUM_DOCS}/${OVERALL_DEN}")
  math(EXPR AVG_INST    "${SUM_INST}/${OVERALL_DEN}")
  math(EXPR AVG_ART     "${SUM_ART}/${OVERALL_DEN}")
  math(EXPR AVG_LICENSE "${SUM_LICENSE}/${OVERALL_DEN}")
endif()

# ----- Read I18n JSONs -----
_read_i18n("${I18N_UI_JSON}"      I18N_UI)
_read_i18n("${I18N_DOCS_JSON}"    I18N_DOCS)
_read_i18n("${I18N_WIN_JSON}"     I18N_WIN)
_read_i18n("${I18N_ART_JSON}"     I18N_ART)
_read_i18n("${I18N_LICENSE_JSON}" I18N_LICENSE)

# ----- Markdown emit -----
string(TIMESTAMP TODAY_YYYY_MM_DD "%Y-%m-%d")

# Header
set(MD "# 🌐 Globalization Dashboard\n\n> **Project:** ${PROJECT_NAME} · **Last updated:** ${TODAY_YYYY_MM_DD}\n\n")

# Legend
string(APPEND MD
  "## Legend\n\n"
  "- 🟢 **Complete** (≥ 95%)\n"
  "- 🟡 **In Progress** (10–94%)\n"
  "- 🔴 **Not Started** (< 10%)\n"
  "- ⏳ **Needs Review** (pending proofread/QA)\n\n"
  "`██████████` = 100%\n\n`█████░░░░░` ≈ 50%\n\n"
  "---\n\n")

# --- Localization band ---
string(APPEND MD "## 🌍 Localization\n\n_Shipping the product in multiple languages. This section tracks per-language translation status for each area._\n\n")

# Roll-up Status (All Components)
string(APPEND MD "### 📈 Roll-up Status (All Components)\n\n> Overall = weighted average of **UI (${W_UI})**, **Docs (${W_DOCS})**, **Installer (${W_INST})**, **Art (${W_ART})**, **License (${W_LICENSE})**.\n\n")
string(APPEND MD "| Language | Progress | Status |\n|---|:--:|:--:|\n")
foreach(L IN LISTS LANGS)
  set(_pall_var "PALL_${L}")
  set(_bar_var  "BAR_${L}")
  set(P_ALL "${${_pall_var}}")
  set(BAR   "${${_bar_var}}")
  _emoji_for(EMJ "${P_ALL}")
  _resolve_lang_name(_LNAME "${L}")
  string(APPEND MD "| ${_LNAME} (`${L}`) | `${BAR}` **${P_ALL}%** | ${EMJ} |\n")
endforeach()
string(APPEND MD "\n---\n\n")

# ===== L10N COMPONENTS =====

# UI
string(APPEND MD "### 1) 🧩 Application UI\n\n")
string(APPEND MD "**Policy**: All visible strings must be externalized into gettext \`.po\` files under \`locale/\`. Add translator comments when context isn’t obvious.\n\n")
string(APPEND MD "**Scope**: Menus, dialogs, errors, tooltips, status messages.  \n**Source**: \`src/\`  \n**Locale dir**: \`locale/<lang>/LC_MESSAGES/\`  \n**Recommended tool**: **POEdit** — Update from POT, translate, compile to \`.mo\`.\n\n")
string(APPEND MD "> Use *POEdit*’s *Catalog → Update from POT/Source code* to merge new strings; keep placeholders intact and run QA before commit.\n\n")
string(APPEND MD "**Status**\n\n| Language | Progress | Status |\n|:--:|:--:|:--:|\n")
foreach(L IN LISTS LANGS)
  set(_v "${L}_PERCENT_TRANSLATED")
  if(DEFINED ${_v})
    set(_pct "${${_v}}")
  # Already in English completely
  elseif(${L} STREQUAL "en")
    set(_pct 100)
  else()
    set(_pct 0)
  endif()

  _bar10(_bar "${_pct}")
  _emoji_for(_emj "${_pct}")
  _resolve_lang_name(_LNAME "${L}")

  string(APPEND MD "| ${_LNAME} | `${_bar}` **${_pct}%** | ${_emj} |\n")
endforeach()
# optional l10n comment
if(NOT "${UI_COMMENT}" STREQUAL "")
  string(APPEND MD "\n${UI_COMMENT}\n")
endif()

# Installer (Windows)
string(APPEND MD "### 2) 📦 Installer\n\n")
string(APPEND MD "**Policy**: All installer message catalogs must be Unicode-safe and localized by platform.\n\n")
string(APPEND MD "#### 2.1 Windows Installer\n\n")
string(APPEND MD "**Strings**: \`installers/windows/messages/*.isl\`  \n**Encoding**: UTF-8 (Unicode Inno Setup).  \n**Notes**: Preserve placeholders like \`{app}\`, \`{cf}\`, \`%1\`. Add comments above each key to clarify intent.\n\n")
string(APPEND MD "**Status**\n\n| Language | Progress | Status |\n|:--:|:--:|:--:|\n")
foreach(L IN LISTS LANGS)
  set(_v "INST_PCT_${L}")
  set(_pct "${${_v}}")
  if(_pct STREQUAL "")
    set(_pct 0)
  endif()

  _bar10(_bar "${_pct}")
  _emoji_for(_emj "${_pct}")
  _resolve_lang_name(_LNAME "${L}")

  string(APPEND MD "| ${_LNAME} | `${_bar}` **${_pct}%** | ${_emj} |\n")
endforeach()
if(NOT "${WIN_COMMENT}" STREQUAL "")
  string(APPEND MD "\n${WIN_COMMENT}\n")
endif()

# Docs
string(APPEND MD "### 3) 📚 Documentation (User Manuals)\n\n")
string(APPEND MD "**Policy**: Documentation follows the same translation review cycle as UI. Source text is English Quarto/Markdown; translations tracked via \`.po\` in \`locale/docs/\`.\n\n")
string(APPEND MD "**Docs root**: \`docs/\`  \n**Doc locale**: \`locale/docs/\`  \n**File types**: \`.qmd\` / \`.md\` → \`.po\` → \`.mo\`\n\n")
string(APPEND MD "**build-localized-docs.R**\nUpdates \`.po\` files and generates the localized help.\n\n")
string(APPEND MD "**Status**\n\n| Language | Progress | Status |\n|:--:|:--:|:--:|\n")
foreach(L IN LISTS LANGS)
  set(_v "DOCS_PCT_${L}")
  set(_pct "${${_v}}")
  if(_pct STREQUAL "")
    set(_pct 0)
  endif()

  _bar10(_bar "${_pct}")
  _emoji_for(_emj "${_pct}")
  _resolve_lang_name(_LNAME "${L}")

  string(APPEND MD "| ${_LNAME} | `${_bar}` **${_pct}%** | ${_emj} |\n")
endforeach()
if(NOT "${DOCS_COMMENT}" STREQUAL "")
  string(APPEND MD "\n${DOCS_COMMENT}\n")
endif()

# Art
string(APPEND MD "### 4) 🎨 Art & Screenshots\n\n")
string(APPEND MD "**Policy**: Core art assets remain language-neutral. Textual variants are generated at runtime from scriptable sources.\n\n")
string(APPEND MD "**Assets**: \`resources/images/\`  \n**Help screenshots**: generated by \`docs/generate-screenshots.lua\` per language.\n\n")
string(APPEND MD "**Status**\n\n| Language | Progress | Status |\n|:--:|:--:|:--:|\n")
foreach(L IN LISTS LANGS)
  set(_v "ART_PCT_${L}")
  set(_pct "${${_v}}")
  if(_pct STREQUAL "")
    set(_pct 0)
  endif()

  _bar10(_bar "${_pct}")
  _emoji_for(_emj "${_pct}")
  _resolve_lang_name(_LNAME "${L}")

  string(APPEND MD "| ${_LNAME} | `${_bar}` **${_pct}%** | ${_emj} |\n")
endforeach()
if(NOT "${ART_COMMENT}" STREQUAL "")
  string(APPEND MD "\n${ART_COMMENT}\n")
endif()
string(APPEND MD "\n---\n\n")

# Licenses
string(APPEND MD "### 5) 📜 Licenses\n\n")
string(APPEND MD "**Policy**: Use **only** the Eclipse Foundation license text and the **approved EF-provided translations**. Do not ship custom or modified licenses. Keep copies in UTF-8 as distributed by EF.\n\n")
string(APPEND MD "**Status**\n\n| Lang | Progress | Status |\n|:--:|:--:|:--:|\n")
foreach(L IN LISTS LANGS)
  set(_v "LICENSE_PCT_${L}")
  set(_pct "${${_v}}")
  if(_pct STREQUAL "")
    set(_pct 0)
  endif()

  _bar10(_bar "${_pct}")
  _emoji_for(_emj "${_pct}")
  _resolve_lang_name(_LNAME "${L}")

  string(APPEND MD "| ${_LNAME} | `${_bar}` **${_pct}%** | ${_emj} |\n")
endforeach()
if(NOT "${LICENSE_COMMENT}" STREQUAL "")
  string(APPEND MD "\n${LICENSE_COMMENT}\n")
endif()
string(APPEND MD "\n---\n\n")

# --- Internationalization band ---
string(APPEND MD "## 💻 Internationalization\n\n_Making the product world-ready (Unicode, plural rules, RTL, locale-sensitive formats, input methods). Each area shows a single readiness score plus a short note._\n\n")

# i18n summary table
string(APPEND MD "| Area | Progress | Status |\n|---|:--:|:--:|\n")

# UI
set(_v_ui "${I18N_UI_VALUE}")
if(_v_ui STREQUAL "")
  set(_v_ui 0)
endif()
_bar10(_bar_ui "${_v_ui}")
_emoji_for(_emj_ui "${_v_ui}")
string(APPEND MD "| 🧩 UI | `${_bar_ui}` **${_v_ui}%** | ${_emj_ui} |\n")

# Installer
set(_v_inst "${I18N_WIN_VALUE}")
if(_v_inst STREQUAL "")
  set(_v_inst 0)
endif()
_bar10(_bar_inst "${_v_inst}")
_emoji_for(_emj_inst "${_v_inst}")
string(APPEND MD "| 📦 Installer | `${_bar_inst}` **${_v_inst}%** | ${_emj_inst} |\n")

# Docs
set(_v_docs "${I18N_DOCS_VALUE}")
if(_v_docs STREQUAL "")
  set(_v_docs 0)
endif()
_bar10(_bar_docs "${_v_docs}")
_emoji_for(_emj_docs "${_v_docs}")
string(APPEND MD "| 📚 Docs | `${_bar_docs}` **${_v_docs}%** | ${_emj_docs} |\n")

# Art
set(_v_art "${I18N_ART_VALUE}")
if(_v_art STREQUAL "")
  set(_v_art 0)
endif()
_bar10(_bar_art "${_v_art}")
_emoji_for(_emj_art "${_v_art}")
string(APPEND MD "| 🎨 Art | `${_bar_art}` **${_v_art}%** | ${_emj_art} |\n")

# License
set(_v_lic "${I18N_LICENSE_VALUE}")
if(_v_lic STREQUAL "")
  set(_v_lic 0)
endif()
_bar10(_bar_lic "${_v_lic}")
_emoji_for(_emj_lic "${_v_lic}")
string(APPEND MD "| 📜 License | `${_bar_lic}` **${_v_lic}%** | ${_emj_lic} |\n\n")

# UI
_emit_section(I18N_UI_TITLE I18N_UI_MUST I18N_UI_RECOMMENDED I18N_UI_COMMENT)

# Docs
_emit_section(I18N_DOCS_TITLE I18N_DOCS_MUST I18N_DOCS_RECOMMENDED I18N_DOCS_COMMENT)

# Windows Installer
_emit_section(I18N_WIN_TITLE I18N_WIN_MUST I18N_WIN_RECOMMENDED I18N_WIN_COMMENT)

# License
_emit_section(I18N_LICENSE_TITLE I18N_LICENSE_MUST I18N_LICENSE_RECOMMENDED I18N_LICENSE_COMMENT)

# Art
_emit_section(I18N_ART_TITLE I18N_ART_MUST I18N_ART_RECOMMENDED I18N_ART_COMMENT)

# Write file
get_filename_component(_md_dir "${OUT_MD}" DIRECTORY)
file(MAKE_DIRECTORY "${_md_dir}")
file(WRITE "${OUT_MD}" "${MD}")

message(STATUS "Globalization dashboard written to ${OUT_MD}")
