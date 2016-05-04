#include <stdarg.h>

/* is c the start of a utf8 sequence? */
#define IS_UTF(c) (((c)&0xC0)!=0x80)

/* convert UTF-8 data to wide characters */
int utf8_to_ints(int32_t *dest, int sz, const char *src, int srcsz);

/* the opposite conversion */
int utf8_from_ints(char *dest, int sz, const int32_t *src, int srcsz);

/* single character to UTF-8 */
int utf8_from_int(char *dest, int32_t ch);

/* character number to byte offset */
int utf8_offset(const char *str, int charnum);

/* byte offset to character number */
int utf8_character_number(const char *s, int offset);

/* return next character, updating an index variable */
int32_t utf8_next_character(const char *s, int *i);

/* move to next character */
void utf8_increment(const char *s, int *i);

/* move to previous character */
void utf8_decrement(const char *s, int *i);

/* returns length of next utf-8 sequence */
int utf8_seqlen(const char *s);

/* assuming src points to the character after a backslash, read an
   escape sequence, storing the result in dest and returning the number of
   input characters processed */
int utf8_read_escape_sequence(int32_t *dest, const char *src);

/* given a wide character, convert it to an ASCII escape sequence stored in
   buf, where buf is "sz" bytes. returns the number of characters output. */
int utf8_escape_int(char *buf, int sz, int32_t ch);

/* convert a string "src" containing escape sequences to UTF-8 */
int utf8_unescape(char *buf, int sz, const char *src);

/* convert UTF-8 "src" to ASCII with escape sequences.
   if escape_quotes is nonzero, quote characters will be preceded by
   backslashes as well. */
int utf8_escape(char *buf, int sz, const char *src, int escape_quotes);

/* utility predicates used by the above */
int octal_digit(char c);
int hex_digit(char c);

/* return a pointer to the first occurrence of ch in s, or NULL if not
   found. character index of found character returned in *charn. */
const char *utf8_strchr(const char *s, int32_t ch, int *charn);

/* count the number of characters in a UTF-8 string */
int utf8_strlen(const char *s);

int utf8_is_locale_utf8(char *locale);
