# GEMINI.md

This file provides guidance to AI systems when working with code in this repository.

## Gemini Rules

- Never perform any git operations
- Never run builds or test runners - the user will do that
- Only make edits to local files, and only after the user has approved each edit one-by-one

## Project Overview

*Eclipse Readability Studio* is a C++20 cross-platform desktop application for analyzing text readability.
It uses *wxWidgets* 3.3.1+ for the GUI framework and *CMake* for building.

### Build Targets
- `readstudio` - Main application
- `manuals` - User documentation (requires R and Quarto)
- `doxygen-docs` - API documentation

## Creating Tests

Tests use the Catch2 (v3) framework and are located in @tests (core tests) and @tests/gui-tests (GUI tests).
Test-driven development is practiced; failing tests should be created before implementing new (non-GUI) code.

## Code Style

The project uses clang-format (v20) and clang-tidy. Key style rules:

- **No C++ `<regex>` library:** Avoid using the C++ `<regex>` library in this project and instead use the hand-rolled parsing implementations in `src/indexing/`. This folder contains various text processing utilities including:
  - Character handling (`character_traits.h`, `characters.h`, `diacritics.cpp`)
  - Tokenization (`tokenize.h`)
  - Syllabification (`syllable.cpp`, `german_syllabize.h`, `russian_syllabize.h`, `spanish_syllabize.h`)
  - Word/phrase analysis (`word.h`, `phrase.h`, `word_functional.cpp`)
  - Various linguistic helpers (abbreviations, contractions, conjunctions, pronouns, etc.)
- Use `std::wstring_view`, `std::prev`, `std::next`, `std::advance` instead of pointer arithmetic
- Make variables and lambdas `const` if possible

- **Indentation:** 4 spaces, no tabs
- **Line length:** 100 characters max
- **Brace style:** Whitesmiths
- **Naming conventions:**
  - Class members: `m_` prefix with camelBack (e.g., `m_wordCount`)
  - Local variables: camelBack
  - Macros/constants: UPPER_CASE
- **Pointers/references:** Left-aligned (`int* ptr`, not `int *ptr`)
- **Comments:** First word lowercase, unless the comment contains multiple sentences
- **Line endings:** LF (Unix-style)

Run formatting check:
```bash
clang-format --dry-run -Werror src/**/*.cpp src/**/*.h
```

## Architecture

### Directory Structure
```
src/
├── app/              # Application entry point and main frame
├── readability/      # Readability test algorithms (core domain logic)
├── indexing/         # Text parsing and linguistic analysis
├── projects/         # Document management (Standard/Batch projects)
├── graphs/           # Readability visualization (Fry, Raygor, etc.) not in Wisteria-Dataviz
├── ui/               # Dialogs and custom controls
├── lua-scripting/    # Lua automation bindings
└── Wisteria-Dataviz/ # Data visualization and controls framework (submodule)
```

### Key Architectural Patterns

**Document-View Pattern:** Uses wxWidgets document-view architecture
- `StandardProjectDoc`/`StandardProjectView` - Single document analysis
- `BatchProjectDoc`/`BatchProjectView` - Batch processing
- `BaseProject` - Abstract base class for project analysis

**Analysis Pipeline:**
1. Text → Indexing (`src/indexing/`) - word/sentence/paragraph parsing
2. Analysis (`src/readability/`) - readability tests run via `BaseProject`
3. Results → Visualization (`src/graphs/`) - graphs and reports

**Readability Tests:** Derive from template class `readability_test_base<>` with language-specific implementations in `english_readability.h`, `spanish_readability.h`, `german_readability.h`.

### Entry Points
- `src/app/readability_app.cpp` - `ReadabilityApp::OnInit()` initializes the application
- `src/app/readability_app.h` - Main application class extending `Wisteria::UI::BaseApp`

### Submodules
- `Wisteria-Dataviz` - Data visualization and controls framework framework
- `OleanderStemmingLibrary` - Word stemming
- `Catch2` - Testing framework
- `lua` - Scripting engine
- `tinyxml2`, `tinyexpr-plusplus` - XML/expression parsing

## Word List Maintenance

After editing word lists in `resources/words/`:
1. Build the program
2. Open Readability Studio
3. Run `resources/finalize-word-lists.lua` in the Lua editor
4. Rebuild the program to repackage

## CI/CD

GitHub Actions workflows enforce:
- clang-format formatting (v20)
- clang-tidy static analysis
- cppcheck analysis
- Unit tests on Linux, Windows, and macOS
- Spell checking (typos)
