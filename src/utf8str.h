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

struct StrIter {
	const Str* str;
	const char* cs;
};

typedef struct StrIter StrIter;

/* Creation and deletion */
Str* StrNew(const char* const cs);
Str* StrNewSetCap(const size_t initCap);
void StrDel(Str* s);

/* Non-mutating functions */
int StrIsNull(const Str* const s);
int StrEqual(const Str* const s, const Str* const t);
size_t StrLength(const Str* const s);
size_t StrFindChar(const Str* const s, const unsigned int c);
unsigned int StrAt(const Str* const s, const size_t index);
unsigned int StrFirst(const Str* const s);
unsigned int StrLast(const Str* const s);
Str* StrCopy(const Str* const s);
Str* StrSlice(const Str* const s, const size_t first, const size_t last);
Str* StrTake(const Str* const s, const size_t n);
Str* StrDrop(const Str* const s, const size_t n);
Str* StrTakeWhile(const Str* const s, int (*p)(unsigned int));
Str* StrDropWhile(const Str* const s, int (*p)(unsigned int));
Str* StrReverse(const Str* const s);

/* Mutating functions */
int StrAddChar(Str* const s, const unsigned int c);
int StrAddChars(Str* const s, const char* cs);
int StrAdd(Str* const s, const Str* const t);
int StrTrim(Str* const s, size_t n);

/* Iterator functions */
StrIter* StrIterNew(const Str* const s);
void StrIterDel(StrIter* it);
int StrIterHasNext(const StrIter* const it);
unsigned int StrIterNext(StrIter* const it);

#endif
