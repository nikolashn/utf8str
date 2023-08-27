/* Copyright (C) 2023 nikolashn */
/* See LICENSE for copyright/license information */

#include "utf8str.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INIT_CAP 256

/* Get size of UTF-8 encoding of Unicode character c in bytes.
 * Returns 0 if c is not a valid Unicode character. */
size_t UTF8Size(unsigned int c) {
	if (c < 0x80) return 1;
	else if (c < 0x800) return 2;
	else if (c < 0x10000) return 3;
	else if (c < 0x110000) return 4;
	return 0;
}

/* Get UTF8 character starting at pointer cs.
 * Returns code point if found, otherwise -1. */
unsigned int UTF8At(const char* cs) {
	if (!cs) return -1;

	int c = -1;

	if ((*cs & 0x80) == 0) {
		c = *cs;
		++cs;
	}
	else if ((*cs & 0xe0) == 0xc0) {
		if (!cs[1]) c = -1;
		else c = ((cs[0] & 0x1f) << 6) | (cs[1] & 0x3f);
	}
	else if ((*cs & 0xf0) == 0xe0) {
		if (!cs[1] || !cs[2]) c = -1;
		else c = ((cs[0] & 0x0f) << 12) | ((cs[1] & 0x3f) << 6) | (cs[2] & 0x3f);
	}
	else if ((*cs & 0xf8) == 0xf0) {
		if (!cs[1] || !cs[2] || !cs[3]) c = -1;
		else {
			c = ((cs[0] & 0x07) << 18) | ((cs[1] & 0x3f) << 12) | 
				((cs[2] & 0x3f) << 6) | (cs[3] & 0x3f);
		}
	}

	return c;
}

/* Get UTF8 character before cs.
 * Returns code point if found, otherwise -1. */
unsigned int UTF8Before(const char* cs) {
	do { --cs; } while ((*cs & 0xc0) == 0x80);
	return UTF8At(cs);
}

/* Attempt to double the capacity of s.
 * If unable to do so, return 0. Otherwise return s. */
Str* StrResize(Str* s) {
	if (s->cap > SIZE_MAX/2)
		return 0;
	s->cap *= 2;
	s->arr = realloc(s->arr, s->cap);
	return s;
}

/* Creates and initializes a new Str with the byte array cs. cs can be set to 0
 * or NULL to initialize an empty string.
 * Returns 0 if unable to create, otherwise a pointer to the Str. */
Str* StrNew(const char* cs) {
	Str* s = StrNewSetCap(INIT_CAP);
	if (s && cs) {
		int ret = StrAddChars(s, cs);
		if (!ret) return 0;
	}
	return s;
}

/* Prærequisites: initCap > 0.
 * Creates and initializes a new Str, with initial capacity set by initCap.
 * Returns 0 if unable to create, otherwise a pointer to the Str */
Str* StrNewSetCap(size_t initCap) {
	if (!initCap) return 0;
	Str* s = malloc(sizeof(*s));
	if (!s) return 0;
	s->length = 0;
	s->size = 1;
	s->cap = initCap;
	s->arr = calloc(initCap, 1);
	if (!s->arr) return 0;
	s->arr[0] = 0;
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

/* Prærequisites: index < StrLength(s).
 * Returns the code point of the index-th (starting from 0) Unicode character of
 * s if index is within range and valid UTF-8 character exists at that index,
 * otherwise returns 0. */
unsigned int StrAt(const Str* s, size_t index) {
	if (index >= StrLength(s)) return 0;

	const char* cs = s->arr;
	unsigned int c;
	size_t i;
	for (i = 0; i <= index; ++i) {
		c = UTF8At(cs);
		if (c == -1) return 0;
		cs += UTF8Size(c);
	}
	
	return c;
}

/* Prærequisites: s is not null.
 * Returns the code point of the first Unicode character of s, if it exists,
 * otherwise returns 0. */
unsigned int StrFirst(const Str* s) {
	if (StrIsNull(s)) return 0;
	unsigned int c = UTF8At(s->arr);
	if (c == -1) return 0;
	return c;
}

/* Prærequisites: s is not null.
 * Returns the code point of the last Unicode character of s, if it exists,
 * otherwise returns 0. */
unsigned int StrLast(const Str* s) {
	if (StrIsNull(s)) return 0;
	unsigned int c = UTF8Before(&(s->arr[s->size]));
	if (c == -1) return 0;
	return c;
}

/* Attempt to create a copy of s and return it. 
 * Returns 0 if unsuccessful, otherwise a pointer to the copy of s. */
Str* StrCopy(const Str* s) {
	Str* t = StrNewSetCap(s->cap);
	if (!t) return 0;
	memcpy(t->arr, s->arr, s->size);
	t->length = s->length;
	t->size = s->size;
	return t;
}

/* Prærequisites: first <= last <= StrLength(s).
 * Attempt to create a new Str containing a slice of s from indices first to
 * last (not inclusive), i.e., the substring s[first..last).
 * Returns 0 if unsuccessful, otherwise a pointer to the new Str containing the
 * slice. */
Str* StrSlice(const Str* s, size_t first, size_t last) {
	if (first > last || last > StrLength(s)) return 0;
	Str* t = StrNewSetCap(INIT_CAP);
	if (!t) return 0;
	
	const char* cs = s->arr;
	unsigned int c;
	size_t i;
	for (i = 0; i < last; ++i) {
		c = UTF8At(cs);
		if (c == -1) return 0;
		cs += UTF8Size(c);
		if (i >= first) StrAddChar(t, c);
	}
	
	return t;
}

/* Prærequisites: c is a valid Unicode code point && c > 0.
 * Adds Unicode character c to the end of Str s.
 * Returns 1 on success, otherwise 0. */
int StrAddChar(Str* s, unsigned int c) {
	const size_t charSize = UTF8Size(c);
	if (!c || !charSize) return 0;

	while (s->cap < s->size + charSize) {
		Str* r = StrResize(s);
		if (!r) return 0;
	}

	char* const end = &(s->arr[s->size-1]);

	if (c < 0x80) {
		end[0] = (char)c;
	}
	else if (c < 0x800) {
		end[0] = (char)((c >> 6) | 0xc0);
		end[1] = (char)((c & 0x3f) | 0x80);
	}
	else if (c < 0x10000) {
		end[0] = (char)((c >> 12) | 0xe0);
		end[1] = (char)(((c >> 6) & 0x3f) | 0x80);
		end[2] = (char)((c & 0x3f) | 0x80);
	}
	else if (c < 0x110000) {
		end[0] = (char)((c >> 18) | 0xf0);
		end[1] = (char)(((c >> 12) & 0x3f) | 0x80);
		end[2] = (char)(((c >> 6) & 0x3f) | 0x80);
		end[3] = (char)((c & 0x3f) | 0x80);
	}
	else return 0;

	end[charSize] = 0;
	s->length += 1;
	s->size += charSize;
	return 1;
}

/* Prærequisites: cs is the start of a zero-terminated byte buffer containing
 * only valid UTF-8 encoded characters.
 * Adds UTF-8 encoded chars in cs to the end of s.
 * Returns 1 on success, otherwise 0. */
int StrAddChars(Str* s, const char* cs) {
	while (*cs) {
		const unsigned int c = UTF8At(cs);
		if (c == -1) return 0;
		StrAddChar(s, c);
		cs += UTF8Size(c);
	}
	return 1;
}

/* Adds the Str at t to the end of s.
 * Returns 1 on success, otherwise 0. */
int StrAdd(Str* s, const Str* t) {
	while (s->cap < s->size - 1 + t->size) {
		Str* r = StrResize(s);
		if (!r) return 0;
	}

	memcpy(&(s->arr[s->size-1]), t->arr, t->size);
	s->length += t->length;
	s->size += t->size - 1;
	
	return 1;
}

