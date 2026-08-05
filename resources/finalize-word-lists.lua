dofile(Application.GetLuaConstantsPath())

-- Sort the stop lists
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/stop-words/english.txt",
    Debug.GetScriptFolder().."words/stop-words/english.txt")
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/stop-words/spanish.txt",
    Debug.GetScriptFolder().."words/stop-words/spanish.txt")
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/stop-words/german.txt",
    Debug.GetScriptFolder().."words/stop-words/german.txt")

-- Sort the word lists
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/past-participles/exceptions.txt",
    Debug.GetScriptFolder().."words/past-participles/exceptions.txt")

Application.MergeWordLists(
    Debug.GetScriptFolder().."words/base-english-dictionary.txt",
    Debug.GetScriptFolder().."words/base-english-dictionary.txt")

Application.MergeWordLists(
    Debug.GetScriptFolder().."words/base-personal.txt",
    Debug.GetScriptFolder().."words/base-personal.txt")

Application.MergeWordLists(
    Debug.GetScriptFolder().."words/base-non-personal.txt",
    Debug.GetScriptFolder().."words/base-non-personal.txt")

Application.MergeWordLists(
    Debug.GetScriptFolder().."words/stop-words/proper-nouns-stoplist.txt",
    Debug.GetScriptFolder().."words/stop-words/proper-nouns-stoplist.txt")

Application.MergeWordLists(
    Debug.GetScriptFolder().."words/abbreviations/abbreviations.txt",
    Debug.GetScriptFolder().."words/abbreviations/abbreviations.txt")

Application.MergeWordLists(
    Debug.GetScriptFolder().."words/abbreviations/non-abbreviations.txt",
    Debug.GetScriptFolder().."words/abbreviations/non-abbreviations.txt")

Application.MergeWordLists(
    Debug.GetScriptFolder().."words/articles/a-exceptions.txt",
    Debug.GetScriptFolder().."words/articles/a-exceptions.txt")

Application.MergeWordLists(
    Debug.GetScriptFolder().."words/articles/an-exceptions.txt",
    Debug.GetScriptFolder().."words/articles/an-exceptions.txt")

Application.MergeWordLists(
    Debug.GetScriptFolder().."words/word-lists/new-dale-chall.txt",
    Debug.GetScriptFolder().."words/word-lists/new-dale-chall.txt")

Application.MergeWordLists(
    Debug.GetScriptFolder().."words/word-lists/harris-jacobson.txt",
    Debug.GetScriptFolder().."words/word-lists/harris-jacobson.txt")

Application.MergeWordLists(
    Debug.GetScriptFolder().."words/word-lists/revised-spache.txt",
    Debug.GetScriptFolder().."words/word-lists/revised-spache.txt")

Application.MergeWordLists(
    Debug.GetScriptFolder().."words/word-lists/stocker-catholic-supplement.txt",
    Debug.GetScriptFolder().."words/word-lists/stocker-catholic-supplement.txt")

-- Expand the proper nouns to possessive forms
-- (personal base is used by an upcoming test as well,
--  so it gets added to the resources as a new file also.)
Application.ExpandWordList(
    Debug.GetScriptFolder().."words/base-personal.txt",
    Debug.GetScriptFolder().."words/proper-nouns/personal.txt", "s", "'s")

Application.ExpandWordList(
    Debug.GetScriptFolder().."words/base-non-personal.txt",
    Debug.GetScriptFolder().."NonPersonalTEMP.txt", "s", "'s")

Application.MergeWordLists(
    Debug.GetScriptFolder().."words/proper-nouns/personal.txt",
    Debug.GetScriptFolder().."NonPersonalTEMP.txt",
    Debug.GetScriptFolder().."words/proper-nouns/all.txt")

os.remove(Debug.GetScriptFolder().."NonPersonalTEMP.txt")

-- Sort (and reformat) phrase lists
Application.MergePhraseLists(
    Debug.GetScriptFolder().."words/caption-labels/captions.txt",
    Debug.GetScriptFolder().."words/caption-labels/captions.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolder().."words/wordy-phrases/common-errors.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/common-errors.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolder().."words/wordy-phrases/base-english.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/base-english.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolder().."words/wordy-phrases/base-german.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/base-german.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolder().."words/wordy-phrases/base-spanish.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/base-spanish.txt")

Application.MergePhraseLists(
    Debug.GetScriptFolder().."words/wordy-phrases/common-errors.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/base-german.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/german.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolder().."words/wordy-phrases/common-errors.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/base-spanish.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/spanish.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolder().."words/wordy-phrases/common-errors.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/base-english.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/english.txt")

-- Create single word list from wordy phrase list
Application.PhraseListToWordList(
    Debug.GetScriptFolder().."words/wordy-phrases/english.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/single-word-replacements-english.txt")

Application.CrossReferencePhraseLists(
    Debug.GetScriptFolder().."words/wordy-phrases/single-word-replacements-english.txt",
    Debug.GetScriptFolder().."words/word-lists/new-dale-chall.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/dale-chall-replacements.txt")

Application.CrossReferencePhraseLists(
    Debug.GetScriptFolder().."words/wordy-phrases/single-word-replacements-english.txt",
    Debug.GetScriptFolder().."words/word-lists/harris-jacobson.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/harris-jacobson-replacements.txt")

Application.CrossReferencePhraseLists(
    Debug.GetScriptFolder().."words/wordy-phrases/single-word-replacements-english.txt",
    Debug.GetScriptFolder().."words/word-lists/revised-spache.txt",
    Debug.GetScriptFolder().."words/wordy-phrases/spache-replacements.txt")

-- Merge everything into the English dictionary
Application.MergePhraseLists(
    Debug.GetScriptFolder().."words/common-dictionary.txt",
    Debug.GetScriptFolder().."words/common-dictionary.txt")
Application.ExpandWordList(
    Debug.GetScriptFolder().."words/common-dictionary.txt",
    Debug.GetScriptFolder().."CommonDictionaryTEMP.txt", "'s")
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/base-english-dictionary.txt",
    Debug.GetScriptFolder().."words/abbreviations/abbreviations.txt",
    Debug.GetScriptFolder().."words/proper-nouns/all.txt",
    Debug.GetScriptFolder().."CommonDictionaryTEMP.txt",
    Debug.GetScriptFolder().."words/dictionaries/english.txt")

os.remove(Debug.GetScriptFolder().."CommonDictionaryTEMP.txt")

-- Programming dictionaries (this is separate and optionally included)
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/programming/assembly.txt",
    Debug.GetScriptFolder().."words/programming/assembly.txt")
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/programming/cpp.txt",
    Debug.GetScriptFolder().."words/programming/cpp.txt")
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/programming/csharp.txt",
    Debug.GetScriptFolder().."words/programming/csharp.txt")
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/programming/html.txt",
    Debug.GetScriptFolder().."words/programming/html.txt")
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/programming/java.txt",
    Debug.GetScriptFolder().."words/programming/java.txt")
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/programming/python.txt",
    Debug.GetScriptFolder().."words/programming/python.txt")
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/programming/visual-basic.txt",
    Debug.GetScriptFolder().."words/programming/visual-basic.txt")
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/programming/r.txt",
    Debug.GetScriptFolder().."words/programming/r.txt")
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/programming/sql.txt",
    Debug.GetScriptFolder().."words/programming/sql.txt")
Application.MergeWordLists(
    Debug.GetScriptFolder().."words/programming/assembly.txt",
    Debug.GetScriptFolder().."words/programming/cpp.txt",
    Debug.GetScriptFolder().."words/programming/csharp.txt",
    Debug.GetScriptFolder().."words/programming/html.txt",
    Debug.GetScriptFolder().."words/programming/java.txt",
    Debug.GetScriptFolder().."words/programming/python.txt",
    Debug.GetScriptFolder().."words/programming/visual-basic.txt",
    Debug.GetScriptFolder().."words/programming/sql.txt",
    Debug.GetScriptFolder().."words/programming/r.txt",
    Debug.GetScriptFolder().."words/programming/all-languages.txt")