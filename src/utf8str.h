/* Copyright (C) 2023 nikolashn */
/* See LICENSE for copyright/license information */

#ifndef __NIKOLASHN_UTF8STR_H
#define __NIKOLASHN_UTF8STR_H

#include <stddef.h>

/* Mutable UTF-8 strings in C. */

struct Str {
	char* arr; /* Underlying array of bytes. Maintains a final zero. */
	size_t length; /* Amount of UTF-8 characters (not including zero) */
	size_t size; /* Amount of bytes of data in container (including zero) */
	size_t cap; /* Capacity of container in bytes (including zero) */
};

typedef struct Str Str;

/* Creation and deletion */
Str* StrNew(const char* cs);
Str* StrNewSetCap(size_t initCap);
void StrDel(Str* s);

/* Non-mutating functions */
int StrIsNull(const Str* s);
size_t StrLength(const Str* s);
unsigned int StrAt(const Str* s, size_t index);
unsigned int StrFirst(const Str* s);
unsigned int StrLast(const Str* s);
Str* StrCopy(const Str* s);
Str* StrSlice(const Str* s, size_t first, size_t last);
Str* StrTake(const Str* s, size_t n);

/* Mutating functions */
int StrAddChar(Str* s, unsigned int c);
int StrAddChars(Str* s, const char* cs);
int StrAdd(Str* s, const Str* t);

/* TODO: intersperse, take, drop, takeWhile, dropWhile, contains, filter,
 * elemIndex, delAt, replace, break, toUpper, toLower, map, reverse, cmp,
 * startsWith, endsWith */

#endif
