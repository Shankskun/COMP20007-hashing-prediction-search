# Hashing Dictionary Prediction Search  
Word prediction project written in C

---

### How it works
1. It reads all words from a given file `jabberwocky.txt` and compares with words from other files (ie `words-250K.txt` or `words-1M.txt`).
1. Uses hashing to form a dictionary for fast look up when comparing
1. When it finds an identical word, it will display it; when it doesn't, the misspelled word will be predicted using *levenshtein distance algorithm*.

### How to run
1. go into the right directory with the `makefile` (cd assignment\ 2)
1. run `makefile` (make)
1. task 1 and 2 is relatively useless tbh, so just run task 3 and 4

---

#### Task 3 (Spell check)
1. after `make` has ran
1. `./a2 check words-250K.txt jabberwocky.txt`to start
1. it will print out all words in "jabberwocky" (file2) that are present in the dictionary "words-250k"(file1)
1. words that are not present, will display a "?" on the side of it

#### Task 4 (Spell correction)
1. after `make` has ran
1. `./a2 spell words-250K.txt jabberwocky.txt`to start
1. it will print out all words in "jabberwocky" (file2) that are present in the dictionary "words-250k"(file1)
1. words that are not present, will perform *levenshtein distance algorithm* to find the closest word to it
1. once found it will print it out; but if its pure gibberish it will still show a "?" besides it


*note - words-250K.txt, words-1M.txt and jabberwocky.txt are in a different directory `data & docs folder`, be sure to put it in the same folder before running it!
