#include "../src/utf8str.h"
#include "munit/munit.h"

#include <stdio.h>

MunitResult TestNew(const MunitParameter params[], void* data) {
	/* empty */
	Str* s = StrNew(0);
	munit_assert_not_null(s);
	munit_assert_size(s->length, ==, 0);
	munit_assert_size(s->size, ==, 1);
	munit_assert_size(s->cap, >=, 0);
	munit_assert_true(!strcmp("", s->arr));
	StrDel(s);

	/* initialized with a string */
	s = StrNew("µnit testing");
	munit_assert_not_null(s);
	munit_assert_size(s->length, ==, 12);
	munit_assert_size(s->size, ==, 14);
	munit_assert_size(s->cap, >=, 14);
	munit_assert_true(!strcmp("µnit testing", s->arr));
	StrDel(s);

	/* initialized with an empty string */
	s = StrNew("");
	munit_assert_not_null(s);
	munit_assert_size(s->length, ==, 0);
	munit_assert_size(s->size, ==, 1);
	munit_assert_size(s->cap, >=, 0);
	munit_assert_true(!strcmp("", s->arr));
	StrDel(s);

	return MUNIT_OK;
}

static MunitParameterEnum paramsNewSetCap[] = { { "maxCap", NULL } };

MunitResult TestNewSetCap(const MunitParameter params[], void* data) {
	/* cap cannot be set to 0 */
	Str* s = StrNewSetCap(0);
	munit_assert_null(s);

	/* randomly generated sizes */
	size_t maxCap;
	sscanf(munit_parameters_get(params, "maxCap"), "%zu", &maxCap);
	munit_assert_true(maxCap > 0);

	size_t order = 1;
	while (order <= maxCap / 2) {
		const size_t initCap = munit_rand_int_range(order, 2 * order);
		s = StrNewSetCap(initCap);
		munit_assert_not_null(s);
		StrDel(s);
		order *= 2;
	}

	return MUNIT_OK;
}

MunitResult TestIsNull(const MunitParameter params[], void* data) {
	Str* s;

	s = StrNew(0);
	munit_assert_true(StrIsNull(s));
	StrAddChars(s, "hello!");
	munit_assert_false(StrIsNull(s));
	StrDel(s);

	s = StrNew("εξαίρετος");
	munit_assert_false(StrIsNull(s));
	StrDel(s);

	s = StrNew("");
	munit_assert_true(StrIsNull(s));
	StrDel(s);

	return MUNIT_OK;
}

MunitResult TestLength(const MunitParameter params[], void* data) {
	Str* s;

	s = StrNew(0);
	munit_assert_size(StrLength(s), ==, 0);
	StrAddChars(s, "hello!");
	munit_assert_size(StrLength(s), ==, 6);
	StrDel(s);

	s = StrNew("εξαίρετος");
	munit_assert_size(StrLength(s), ==, 9);
	StrDel(s);

	s = StrNew("");
	munit_assert_size(StrLength(s), ==, 0);
	StrDel(s);

	return MUNIT_OK;
}

MunitResult TestAt(const MunitParameter params[], void* data) {
	Str* s;

	s = StrNew("εξαίρετος - Greek, \"excellent\"");
	munit_assert_uint(StrAt(s, 0), ==, L'ε');
	munit_assert_uint(StrAt(s, 8), ==, L'ς');
	munit_assert_uint(StrAt(s, 19), ==, L'"');
	munit_assert_uint(StrAt(s, 29), ==, L'"');
	munit_assert_uint(StrAt(s, 30), ==, 0);
	munit_assert_uint(StrAt(s, 500), ==, 0);
	StrAddChar(s, L'.');
	munit_assert_uint(StrAt(s, 29), ==, L'"');
	munit_assert_uint(StrAt(s, 30), ==, L'.');
	munit_assert_uint(StrAt(s, 500), ==, 0);
	StrDel(s);

	s = StrNew(0);
	munit_assert_uint(StrAt(s, 0), ==, 0);
	munit_assert_uint(StrAt(s, 500), ==, 0);
	StrDel(s);

	return MUNIT_OK;
}

MunitResult TestFirst(const MunitParameter params[], void* data) {
	Str* s;

	s = StrNew("A");
	munit_assert_uint(StrFirst(s), ==, L'A');
	StrDel(s);

	s = StrNew("€ euros");
	munit_assert_uint(StrFirst(s), ==, L'€');
	StrDel(s);

	s = StrNew("道沖， 而用之或不盈。");
	munit_assert_uint(StrFirst(s), ==, L'道');
	StrDel(s);

	s = StrNew("🙂🙂🙂");
	munit_assert_uint(StrFirst(s), ==, L'🙂');
	StrDel(s);

	s = StrNew(0);
	munit_assert_uint(StrFirst(s), ==, 0);
	StrDel(s);

	return MUNIT_OK;
}

MunitResult TestLast(const MunitParameter params[], void* data) {
	Str* s;

	s = StrNew("A");
	munit_assert_uint(StrLast(s), ==, L'A');
	StrDel(s);

	s = StrNew("€ euros");
	munit_assert_uint(StrLast(s), ==, L's');
	StrDel(s);

	s = StrNew("道沖， 而用之或不盈。");
	munit_assert_uint(StrLast(s), ==, L'。');
	StrDel(s);

	s = StrNew("🙂🙂🙂");
	munit_assert_uint(StrLast(s), ==, L'🙂');
	StrDel(s);

	s = StrNew(0);
	munit_assert_uint(StrLast(s), ==, 0);
	StrDel(s);

	return MUNIT_OK;
}

MunitResult TestCopy(const MunitParameter params[], void* data) {
	Str* s = StrNew("Ingen ko på isen!");
	munit_assert_not_null(s);
	munit_assert_size(s->length, ==, 17);
	munit_assert_size(s->size, ==, 19);
	munit_assert_size(s->cap, >=, 19);
	munit_assert_true(!strcmp("Ingen ko på isen!", s->arr));

	Str* t = StrCopy(s);
	munit_assert_size(t->length, ==, 17);
	munit_assert_size(t->size, ==, 19);
	munit_assert_size(t->cap, >=, 19);
	munit_assert_size(t->cap, ==, s->cap);
	munit_assert_true(!strcmp("Ingen ko på isen!", t->arr));

	StrDel(s);
	/* it is an actual copy - still exists after s is deleted */
	munit_assert_true(!strcmp("Ingen ko på isen!", t->arr));

	StrDel(t);

	return MUNIT_OK;
}

MunitResult TestSlice(const MunitParameter params[], void* data) {
	Str* s = StrNew("Parciais fy jac codi baw hud llawn dŵr ger tŷ Mabon.");
	Str* t;

	t = StrSlice(s, 0, 4);
	munit_assert_true(!strcmp("Parc", t->arr));
	StrDel(t);

	t = StrSlice(s, 0, 0);
	munit_assert_true(!strcmp("", t->arr));
	StrDel(t);

	t = StrSlice(s, 52, 52);
	munit_assert_true(!strcmp("", t->arr));
	StrDel(t);

	t = StrSlice(s, 33, 44);
	munit_assert_true(!strcmp("n dŵr ger t", t->arr));
	StrDel(t);

	t = StrSlice(s, 4, 0);
	munit_assert_true(!strcmp("", t->arr));
	StrDel(t);

	t = StrSlice(s, 55, 99);
	munit_assert_true(!strcmp("", t->arr));
	StrDel(t);

	t = StrSlice(s, 99, 55);
	munit_assert_true(!strcmp("", t->arr));
	StrDel(t);

	t = StrSlice(s, 0, 99);
	munit_assert_true(!strcmp(s->arr, t->arr));
	StrDel(t);

	t = StrSlice(s, 99, 0);
	munit_assert_true(!strcmp("", t->arr));
	StrDel(t);

	StrDel(s);

	return MUNIT_OK;
}

MunitResult TestTake(const MunitParameter params[], void* data) {
	Str* s = StrNew("🌶🐎😆😈😈😈");
	Str* t;

	t = StrTake(s, 1);
	munit_assert_true(!strcmp("🌶", t->arr));
	StrDel(t);

	t = StrTake(s, 4);
	munit_assert_true(!strcmp("🌶🐎😆😈", t->arr));
	StrDel(t);

	t = StrTake(s, 0);
	munit_assert_true(!strcmp("", t->arr));
	StrDel(t);

	t = StrTake(s, 52);
	munit_assert_true(!strcmp(s->arr, t->arr));
	StrDel(t);

	StrDel(s);

	s = StrNew("");

	t = StrTake(s, 1);
	munit_assert_true(!strcmp(s->arr, t->arr));
	StrDel(t);

	t = StrTake(s, 4);
	munit_assert_true(!strcmp(s->arr, t->arr));
	StrDel(t);

	t = StrTake(s, 0);
	munit_assert_true(!strcmp(s->arr, t->arr));
	StrDel(t);

	t = StrTake(s, 52);
	munit_assert_true(!strcmp(s->arr, t->arr));
	StrDel(t);

	StrDel(s);

	return MUNIT_OK;
}

MunitResult TestDrop(const MunitParameter params[], void* data) {
	Str* s = StrNew("🌶🐎😆😈😈😈");
	Str* t;

	t = StrTake(s, 1);
	munit_assert_true(!strcmp("🌶", t->arr));
	StrDel(t);

	t = StrTake(s, 4);
	munit_assert_true(!strcmp("🌶🐎😆😈", t->arr));
	StrDel(t);

	t = StrTake(s, 0);
	munit_assert_true(!strcmp("", t->arr));
	StrDel(t);

	t = StrTake(s, 52);
	munit_assert_true(!strcmp(s->arr, t->arr));
	StrDel(t);

	StrDel(s);

	s = StrNew("");

	t = StrTake(s, 1);
	munit_assert_true(!strcmp(s->arr, t->arr));
	StrDel(t);

	t = StrTake(s, 4);
	munit_assert_true(!strcmp(s->arr, t->arr));
	StrDel(t);

	t = StrTake(s, 0);
	munit_assert_true(!strcmp(s->arr, t->arr));
	StrDel(t);

	t = StrTake(s, 52);
	munit_assert_true(!strcmp(s->arr, t->arr));
	StrDel(t);

	return MUNIT_OK;
}

int IsAscii(unsigned int c) { return c < 0x80; }
int IsFourBytes(unsigned int c) { return c >= 0x110000; }
int IsNotFourBytes(unsigned int c) { return !IsFourBytes(c); }

MunitResult TestTakeWhile(const MunitParameter params[], void* data) {
	Str* s = StrNew("Hellô ẃöŗłd‼️");
	Str* t;
	
	t = StrTakeWhile(s, IsAscii);
	munit_assert_true(!strcmp("Hell", t->arr));
	StrDel(t);
	
	t = StrTakeWhile(s, IsFourBytes);
	munit_assert_true(!strcmp("", t->arr));
	StrDel(t);
	
	t = StrTakeWhile(s, IsNotFourBytes);
	munit_assert_true(!strcmp(s->arr, t->arr));
	StrDel(t);

	StrDel(s);

	return MUNIT_OK;
}

MunitResult TestAdd(const MunitParameter params[], void* data) {
	int ret;
	Str* s = StrNew(0);
	munit_assert_not_null(s);

	/* adding several characters one at a time */
	ret = StrAddChar(s, L's');
	munit_assert_int(ret, ==, 1);
	ret = StrAddChar(s, L'a');
	munit_assert_int(ret, ==, 1);
	ret = StrAddChar(s, L'l');
	munit_assert_int(ret, ==, 1);
	ret = StrAddChar(s, L'v');
	munit_assert_int(ret, ==, 1);
	ret = StrAddChar(s, L'ē');
	munit_assert_int(ret, ==, 1);
	munit_assert_size(s->length, ==, 5);
	munit_assert_size(s->size, ==, 7);
	munit_assert_size(s->cap, >=, 7);
	munit_assert_true(!strcmp("salvē", s->arr));

	/* adding zero character */
	ret = StrAddChar(s, 0);
	munit_assert_int(ret, ==, 0);
	munit_assert_size(s->length, ==, 5);
	munit_assert_size(s->size, ==, 7);
	munit_assert_size(s->cap, >=, 7);
	munit_assert_true(!strcmp("salvē", s->arr));

	/* adding an invalid character */
	ret = StrAddChar(s, -1);
	munit_assert_int(ret, ==, 0);
	munit_assert_size(s->length, ==, 5);
	munit_assert_size(s->size, ==, 7);
	munit_assert_size(s->cap, >=, 7);
	munit_assert_true(!strcmp("salvē", s->arr));

	/* adding a byte array */
	ret = StrAddChars(s, ", 世界");
	munit_assert_int(ret, ==, 1);
	munit_assert_size(s->length, ==, 9);
	munit_assert_size(s->size, ==, 15);
	munit_assert_size(s->cap, >=, 15);
	munit_assert_true(!strcmp("salvē, 世界", s->arr));

	/* adding a singleton byte array */
	ret = StrAddChars(s, "!");
	munit_assert_int(ret, ==, 1);
	munit_assert_size(s->length, ==, 10);
	munit_assert_size(s->size, ==, 16);
	munit_assert_size(s->cap, >=, 16);
	munit_assert_true(!strcmp("salvē, 世界!", s->arr));

	/* adding an empty byte array */
	ret = StrAddChars(s, "");
	munit_assert_int(ret, ==, 1);
	munit_assert_size(s->length, ==, 10);
	munit_assert_size(s->size, ==, 16);
	munit_assert_size(s->cap, >=, 16);
	munit_assert_true(!strcmp("salvē, 世界!", s->arr));

	StrDel(s);

	/* Testing resizing for individual char (single byte) */
	s = StrNewSetCap(4);
	ret = StrAddChars(s, "hè");
	munit_assert_int(ret, ==, 1);
	munit_assert_size(s->length, ==, 2);
	munit_assert_size(s->size, ==, 4);
	munit_assert_size(s->cap, ==, 4);
	munit_assert_true(!strcmp("hè", s->arr));

	ret = StrAddChar(s, L'h');
	munit_assert_int(ret, ==, 1);
	munit_assert_size(s->length, ==, 3);
	munit_assert_size(s->size, ==, 5);
	munit_assert_size(s->cap, ==, 8);
	munit_assert_true(!strcmp("hèh", s->arr));

	StrDel(s);

	/* Testing resizing for individual char (multi byte) */
	s = StrNewSetCap(4);
	ret = StrAddChars(s, "hè");
	munit_assert_int(ret, ==, 1);
	munit_assert_size(s->length, ==, 2);
	munit_assert_size(s->size, ==, 4);
	munit_assert_size(s->cap, ==, 4);
	munit_assert_true(!strcmp("hè", s->arr));

	ret = StrAddChar(s, L'è');
	munit_assert_int(ret, ==, 1);
	munit_assert_size(s->length, ==, 3);
	munit_assert_size(s->size, ==, 6);
	munit_assert_size(s->cap, ==, 8);
	munit_assert_true(!strcmp("hèè", s->arr));

	StrDel(s);

	/* Testing resizing for char array */
	s = StrNewSetCap(4);
	ret = StrAddChars(s, "CC♯DD♯EFF♯GG♯AA♯B");
	munit_assert_int(ret, ==, 1);
	munit_assert_size(s->length, ==, 17);
	munit_assert_size(s->size, ==, 28);
	munit_assert_size(s->cap, ==, 32);
	munit_assert_true(!strcmp("CC♯DD♯EFF♯GG♯AA♯B", s->arr));

	/* add Strs together */
	Str* t = StrNewSetCap(4);
	ret = StrAddChars(t, "CD♭DE♭EFG♭GA♭AB♭B ");
	munit_assert_int(ret, ==, 1);
	munit_assert_size(t->length, ==, 18);
	munit_assert_size(t->size, ==, 29);
	munit_assert_size(t->cap, ==, 32);
	munit_assert_true(!strcmp("CD♭DE♭EFG♭GA♭AB♭B ", t->arr));

	ret = StrAdd(t, s);
	munit_assert_int(ret, ==, 1);
	munit_assert_size(t->length, ==, 35);
	munit_assert_size(t->size, ==, 56);
	munit_assert_size(t->cap, ==, 64);
	munit_assert_true(!strcmp("CC♯DD♯EFF♯GG♯AA♯B", s->arr));
	munit_assert_true(!strcmp("CD♭DE♭EFG♭GA♭AB♭B CC♯DD♯EFF♯GG♯AA♯B", t->arr));

	StrDel(s);
	StrDel(t);

	return MUNIT_OK;
}

MunitTest tests[] = {
	{ "/StrNew", TestNew, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ "/StrNewSetCap", TestNewSetCap, NULL, NULL, MUNIT_TEST_OPTION_NONE,
		paramsNewSetCap },
	{ "/StrIsNull", TestIsNull, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ "/StrLength", TestLength, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ "/StrAt", TestAt, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ "/StrFirst", TestFirst, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ "/StrLast", TestFirst, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ "/StrCopy", TestCopy, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ "/StrSlice", TestSlice, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ "/StrTake", TestTake, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ "/StrDrop", TestDrop, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ "/StrTakeWhile", TestTakeWhile, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ "/StrAdd*", TestAdd, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite suite = 
	{ "/utf8str", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE };

int main(int argc, char* const argv[]) {
	return munit_suite_main(&suite, NULL, argc, argv);
}

