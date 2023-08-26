/* Copyright (C) 2023 nikolashn */
/* See LICENSE for copyright/license information */

#include "utf8str.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INIT_CAP 256

/* Creates and initializes a new Str.
 * Returns 0 if unable to create, otherwise a pointer to the Str. */
Str* StrNew() { return StrNewSetCap(INIT_CAP); }

/* Creates and initializes a new Str, with initial capacity set by initCap.
 * Returns 0 if unable to create, otherwise a pointer to the Str */
Str* StrNewSetCap(size_t initCap) {
	Str* s = malloc(sizeof(*s));
	if (!s) return 0;
	s->length = 0;
	s->size = 0;
	s->cap = initCap;
	s->arr = calloc(initCap, 1);
	if (!s->arr) return 0;
	return s;
}

/* Free the memory underlying s and then s itself. */
void StrDel(Str* s) {
	free(s->arr);
	free(s);
}

/* Returns 1 if s contains no characters, otherwise returns 0. */
int StrIsNull(const Str* s) { return s->length == 0; }

/* Returns the amount of UTF-8 characters in s. */
size_t StrLength(const Str* s) { return s->length; }

/* Attempt to create a copy of s and return it. 
 * Returns 0 if unsuccessful, otherwise a pointer to the copy of s. */
Str* StrCopy(const Str* s) {
	Str* s1 = StrNewSetCap(s->cap);
	if (!s1) return 0;
	memcpy(s1->arr, s->arr, s->size);
	s1->length = s->length;
	s1->size = s->size;
	return s1;
}

/*
unsigned int StrAt(const Str* s, size_t index);
unsigned int StrFirst(const Str* s);
unsigned int StrLast(const Str* s);
Str* StrSlice(const Str* s, size_t first, size_t last);

void StrAppendChar(Str* s, unsigned int c);
void StrAppendChars(Str* s, const char* cs);
void StrAppend(Str* s, const Str* s1);
*/

