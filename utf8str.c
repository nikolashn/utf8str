/* Copyright (C) 2023 nikolashn */
/* See LICENSE for copyright/license information */

#include "utf8str.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INIT_CAP 256

/* Get size of UTF-8 encoding of Unicode character c in bytes.
 * Returns 0 if c is not a valid Unicode character. */
size_t UTF8Size(const unsigned int c) {
	if (c < 0x80) return 1;
	else if (c < 0x800) return 2;
	else if (c < 0x10000) return 3;
	else if (c < 0x110000) return 4;
	return 0;
}

/* Get UTF8 character starting at pointer cs.
 * Returns code point if found, otherwise -1. */
unsigned int UTF8At(const char* const cs) {
	if (!cs) return -1;

	int c = -1;

	if ((*cs & 0x80) == 0) {
		c = *cs;
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

/* Repeatedly double the capacity of s until it is at least the target size.
 * If unable to do so, return 0. Otherwise return s. */
Str* StrResize(Str* s, const size_t size) {
	if (s->cap >= size) return s;
	while (s->cap < size) {
		if (s->cap > SIZE_MAX/2) return 0;
		s->cap *= 2;
	}
	s->arr = realloc(s->arr, s->cap);
	return s;
}

/* Creates and initializes a new Str with the byte array cs. cs can be set to 0
 * or NULL to initialize an empty string.
 * Returns 0 if unable to create, otherwise a pointer to the Str. */
Str* StrNew(const char* const cs) {
	Str* s = StrNewSetCap(INIT_CAP);
	if (s && cs) {
		const int ret = StrAddChars(s, cs);
		if (!ret) return 0;
	}
	return s;
}

/* Prærequisites: initCap > 0.
 * Creates and initializes a new Str, with initial capacity set by initCap.
 * Returns 0 if unable to create, otherwise a pointer to the Str */
Str* StrNewSetCap(const size_t initCap) {
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
void StrDel(Str* const s) {
	free(s->arr);
	free(s);
}

/* Returns 1 if s contains no characters, otherwise returns 0. */
int StrIsNull(const Str* const s) { return s->length == 0; }

/* Returns 1 if the strings s and t are equal, 0 otherwise. */
int StrEqual(const Str* const s, const Str* const t) {
	if (s->length != t->length || s->size != t->size)
		return 0;
	/* as every Unicode string has a unique representation in terms of an array of
	 * bytes, the following works */
	size_t i;
	for (i = 0; i < s->size; ++i) {
		if (s->arr[i] != t->arr[i]) return 0;
	}
	return 1;
}

/* Returns 1 if the string s has t as a præfix, 0 otherwise. */
int StrStartsWith(const Str* const s, const Str* const t) {
	if (t->length > s->length || t->size > s->size)
		return 0;

	StrIter* sit = StrIterNew(s);
	if (!sit) return 0;
	StrIter* tit = StrIterNew(t);
	if (!tit) return 0;

	while (StrIterHasNext(tit)) {
		const unsigned int c = StrIterNext(sit);
		const unsigned int d = StrIterNext(tit);
		if (c == -1 || d == -1 || c != d) {
			StrIterDel(tit); StrIterDel(sit); return 0;
		}
	}

	StrIterDel(tit);
	StrIterDel(sit);
	return 1;
}

/* Returns 1 if the string s has t as a postfix, 0 otherwise. */
int StrEndsWith(const Str* const s, const Str* const t) {
	if (t->length > s->length || t->size > s->size)
		return 0;

	StrIter* sit = StrIterNew(s);
	if (!sit) return 0;
	StrIter* tit = StrIterNew(t);
	if (!tit) return 0;

	while (StrIterHasPrev(tit)) {
		const unsigned int c = StrIterPrev(sit);
		const unsigned int d = StrIterPrev(tit);
		if (c == -1 || d == -1 || c != d) {
			StrIterDel(tit); StrIterDel(sit); return 0;
		}
	}

	StrIterDel(tit);
	StrIterDel(sit);
	return 1;
}

/* Returns the amount of UTF-8 characters in s. */
size_t StrLength(const Str* const s) { return s->length; }

/* Returns the first index of the character c if found, otherwise returns -1. */
size_t StrFindChar(const Str* const s, const unsigned int c) {
	StrIter* it = StrIterNew(s);
	if (!it) return 0;
	unsigned int d = 0;
	size_t index = -1;
	while (StrIterHasNext(it) && d != c) {
		d = StrIterNext(it);
		if (d == -1) { 
			StrIterDel(it);
			return -1;
		}
		++index;
	}
	StrIterDel(it);
	if (d == c) return index;
	return -1;
}

/* Prærequisites: index < StrLength(s).
 * Returns the code point of the index-th (starting from 0) Unicode character of
 * s if index is within range and valid UTF-8 character exists at that index,
 * otherwise returns 0. */
unsigned int StrAt(const Str* const s, const size_t index) {
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
unsigned int StrFirst(const Str* const s) {
	if (StrIsNull(s)) return 0;
	const unsigned int c = UTF8At(s->arr);
	if (c == -1) return 0;
	return c;
}

/* Prærequisites: s is not null.
 * Returns the code point of the last Unicode character of s, if it exists,
 * otherwise returns 0. */
unsigned int StrLast(const Str* const s) {
	if (StrIsNull(s)) return 0;
	const unsigned int c = UTF8Before(&(s->arr[s->size]));
	if (c == -1) return 0;
	return c;
}

/* Create a copy of s and return it. 
 * Returns 0 if unsuccessful, otherwise a pointer to the copy of s. */
Str* StrCopy(const Str* const s) {
	Str* t = StrNewSetCap(s->cap);
	if (!t) return 0;
	memcpy(t->arr, s->arr, s->size);
	t->length = s->length;
	t->size = s->size;
	return t;
}

/* Create a new Str containing a slice of s from indices first to last (not
 * inclusive), i.e.:
 *   if first <= last <= StrLength(s), the substring s[first..last);
 *   otherwise, if first > last or first > StrLength(s), the empty string.
 *   otherwise, the substring s[first..StrLength(s));
 * Returns 0 if unsuccessful, otherwise a pointer to the new Str containing the
 * slice. */
Str* StrSlice(const Str* const s, const size_t first, const size_t last) {
	Str* t = StrNew(0);
	if (!t) return 0;

	const char* cs = s->arr;
	size_t i;

	if (first <= last && last < s->length) {
		for (i = 0; i < last; ++i) {
			const unsigned int c = UTF8At(cs);
			if (c == -1) { StrDel(t); return 0; }
			cs += UTF8Size(c);
			if (i >= first) StrAddChar(t, c);
		}
	}
	else if (first <= last && first <= s->length) {
		for (i = 0; i < first; ++i) {
			const unsigned int c = UTF8At(cs);
			if (c == -1) { StrDel(t); return 0; }
			cs += UTF8Size(c);
		}

		size_t size = s->arr + s->size - cs;
		t = StrResize(t, size);
		if (!t) return 0;

		memcpy(t->arr, cs, size);
		t->length = s->length - first;
		t->size = size;
	}
	
	return t;
}

/* Create a new Str: if n < StrLength(s), the præfix of s of length n;
 * otherwise, a copy of s. 
 * Returns 0 if unsuccessful, otherwise a pointer to the new Str containing the
 * præfix. */
Str* StrTake(const Str* const s, const size_t n) {
	if (n < s->length) return StrSlice(s, 0, n);
	else return StrCopy(s);
}

/* Create a new Str: if n < StrLength(s), the postfix of s of length n;
 * otherwise, an empty string.
 * Returns 0 if unsuccessful, otherwise a pointer to the new Str containing the
 * postfix. */
Str* StrDrop(const Str* const s, const size_t n) {
	if (n < s->length) return StrSlice(s, n, s->length);
	else return StrNew(0);
}

/* Create a new Str that returns the longest præfix of s such that for each
 * Unicode character c in the præfix, p(c) != 0.
 * Returns 0 if unsuccessful, otherwise a pointer to the new Str containing the
 * præfix. */
Str* StrTakeWhile(const Str* const s, int (*p)(unsigned int)) {
	Str* t = StrNew(0);
	if (!t) return 0;
	const char* cs = s->arr;
	unsigned int c;
	while (cs < &(s->arr[s->size]) && (c = UTF8At(cs)) != -1 && p(c)) {
		cs += UTF8Size(c);
		StrAddChar(t, c);
	}
	if (c == -1) { StrDel(t); return 0; }
	return t;
}

/* Create a new Str that returns the longest præfix of s such that for each
 * Unicode character c in the præfix, p(c) != 0.
 * Returns 0 if unsuccessful, otherwise a pointer to the new Str containing the
 * præfix. */
Str* StrDropWhile(const Str* const s, int (*p)(unsigned int)) {
	Str* t = StrNew(0);
	if (!t) return 0;

	const char* cs = s->arr;
	unsigned int c;
	size_t length = s->length;
	while (cs < &(s->arr[s->size]) && (c = UTF8At(cs)) != -1 && p(c)) {
		cs += UTF8Size(c);
		--length;
	}
	if (cs >= &(s->arr[s->size])) return t;
	if (c == -1) { StrDel(t); return 0; }

	const size_t size = s->arr + s->size - cs;
	t = StrResize(t, size);
	if (!t) return 0;

	memcpy(t->arr, cs, size);
	t->length = length;
	t->size = size;

	return t;
}

/* Create a new Str which has the reverse of the characters of s.
 * Returns 0 if unsuccessful, otherwise a pointer to the new Str containing the
 * reverse. */
Str* StrReverse(const Str* const s) {
	Str* t = StrNew(0);
	if (!t) return 0;

	const char* cs = &(s->arr[s->size]);
	unsigned int c;
	while (cs > s->arr && (c = UTF8Before(cs)) != -1) {
		cs -= UTF8Size(c);
		StrAddChar(t, c);
	}
	if (c == -1) { StrDel(t); return 0; }
	return t;
}

/* Prærequisites: c is a valid Unicode code point && c > 0.
 * Adds Unicode character c to the end of Str s.
 * Returns 1 on success, otherwise 0. */
int StrAddChar(Str* const s, const unsigned int c) {
	const size_t charSize = UTF8Size(c);
	if (!c || !charSize) return 0;
	if (!StrResize(s, s->size + charSize))
		return 0;

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
int StrAddChars(Str* const s, const char* cs) {
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
int StrAdd(Str* const s, const Str* const t) {
	if (!StrResize(s, s->size - 1 + t->size))
		return 0;

	memcpy(&(s->arr[s->size-1]), t->arr, t->size);
	s->length += t->length;
	s->size += t->size - 1;
	
	return 1;
}

/* If n < StrLength(s), remove n characters from the end of s; otherwise, set s
 * to an empty string.
 * Returns 1 on success, otherwise 0. */
int StrTrim(Str* const s, size_t n) {
	if (n >= s->length) {
		s->length = 0;
		s->size = 1;
		s->arr[0] = 0;
		return 1;
	}

	const char* cs = &(s->arr[s->size]);
	while (n--) {
		size_t size = UTF8Size(UTF8Before(cs));
		cs -= size;
		if (!size) return 0;
		--s->length;
		s->size -= size;
	}
	s->arr[s->size - 1] = 0;
	return 1;
}

/* Create a new string iterator from a string s.
 * Returns a pointer to the string iterator if successful, otherwise 0. */
StrIter* StrIterNew(const Str* const s) {
	if (!s) return 0;
	StrIter* it = malloc(sizeof(*it));
	if (!it) return 0;
	it->str = s;
	it->cs = s->arr;
	return it;
}

/* Free a string iterator (doesn't free the underlying Str). */
void StrIterDel(StrIter* it) { free(it); }

/* Returns 1 if there are more characters left to be iterated over forwards in
 * the underlying string, otherwise 0. */
int StrIterHasNext(const StrIter* const it) {
	return it->cs < it->str->arr + it->str->size - 1;
}

/* If there are more characters to be iterated over forwards and the next
 * character is valid, returns the next character, then increments the iterator;
 * otherwise, if there are no more characters to be iterated over forwards,
 * returns 0; otherwise, returns -1. */
unsigned int StrIterNext(StrIter* const it) {
	if (!StrIterHasNext(it)) return 0;
	const unsigned int c = UTF8At(it->cs);
	if (c != -1) it->cs += UTF8Size(c);
	return c;
}

/* Returns 1 if there have are more characters left to be iterated over
 * backwards in the underlying string, otherwise 0. */
int StrIterHasPrev(const StrIter* const it) {
	return it->cs > it->str->arr;
}

/* If there are more characters to be iterated over backwards and the next
 * character is valid, returns the next character, then increments the iterator;
 * otherwise, if there are no more characters to be iterated over backwards,
 * returns 0; otherwise, returns -1. */
unsigned int StrIterPrev(StrIter* const it) {
	if (!StrIterHasPrev(it)) return 0;
	const unsigned int c = UTF8Before(it->cs);
	if (c != -1) it->cs -= UTF8Size(c);
	return c;
}

