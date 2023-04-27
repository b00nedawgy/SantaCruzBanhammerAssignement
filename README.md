# Assignment 6: The Great Firewall of Santa Cruz: Bloom Filters, Linked Lists and Hash Tables
## Description:
### In this assignment, the objective is to make a “firewall” that understands ‘oldspeak’ and gives the ‘newspeak’ translation. As well as, give recommendations to the speaker. Like an “educational” language filter. By implementing a hashtable and a bloom filter, this can be done. If nothing inappropriate was said, the executable will print nothing.
## Directions:
### Makefile:
SYNOPSIS: compiles c files and removes associated .o files
### type 'make' and 'make clean' to build and remove associated objects and executables used to run this program
### ~~
### './banhammer':
SYNOPSIS: if inappropriate words were piped into the executable, generates a helpful message for the user to read and understand what they have done wrong.
### OPTIONS:
### -t size: specifies that the hash table will have 'size' entrries (the default will be 10000).
### -f size: specifies that the Bloom filter will have 'size' entries (the default will be 2^19).
### -m     : will enable the move-to-front rule. By default, the move-to-front rule is disabled.
### -s     : will enable the printing of statistics to stdout.
### -h     : prints out the program usage.
## Files:
### banhammer.c
### messages.h
### city.h
### city.c
### ht.h
### ht.c
### ll.h
### ll.c
### node.h
### node.c
### bf.h
### bf.c
### bv.h
### bv.c
### parser.h
### parser.c
### Makefile
### DESIGN.pdf
### WRITEUP.pdf

## scan-build: No bugs found.
