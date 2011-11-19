/*
 *
 * Copyright (C) 1997, 1998, 1999, 2000, 2001 Loic Dachary
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */
#ifndef _uri_h
#define _uri_h

#ifndef SWIG
#ifdef __cplusplus
extern "C" {
#endif
#endif /* SWIG */

/*
 * Possible values of the uri library mode
 */
/*
 * URI are transformed in cannonical form as soon as they are
 * parsed.
 */
#define URI_MODE_CANNONICAL		0x0001
/*
 * When not in cannonical mode, transform scheme in lowercase anyway
 */
#define URI_MODE_LOWER_SCHEME		0x0002
/*
 * When an error occur, print the error on stderr
 */
#define URI_MODE_ERROR_STDERR		0x0004
/*
 * When a field is set, memory for string is malloc'd
 * Implies an overhead but allow caller not to worry 
 * about space allocation.
 */
#define URI_MODE_FIELD_MALLOC		0x0008
/*
 * URL parsing as defined in draft-fielding-uri-syntax-04.txt must
 * be followed strictly. If not set prefer behaviour compatible with
 * navigator behaviour. For instance http:a.html relative URL with
 * http://www.senga.org/tmp/b.html will be transformed into the absolute
 * URL http://www.senga.org/tmp/a.html if strict is not set and to 
 * http:a.html if strict is set.
 */
#define URI_MODE_URI_STRICT		0x0010
/*
 * Apply strict policy for http:g style relative URLs.
 *
 * draft-fielding-uri-syntax-04.txt C.2
 *
 *   Some parsers allow the scheme name to be present in a relative URI
 *   if it is the same as the base URI scheme.  This is considered to be
 *   a loophole in prior specifications of partial URI [RFC1630]. Its
 *   use should be avoided.
 *
 *      http:g        =  http:g           ; for validating parsers
 *                    |  http://a/b/c/g   ; for backwards compatibility
 *
 */
#define URI_MODE_URI_STRICT_SCHEME	0x0020
/*
 * Use an alternate form for file name representation of an URI.
 * Instead of a readable path name that is as close as possible
 * to the original URL, use a splitted MD5 key.
 * The MD5 key is in ascii form (32 hexadecimal digits). The first four
 * pair of digit are used as directory names. The rest is used as a file
 * name.
 * Example:
 *
 * http://www.foo.com/
 * is transformed into the MD5 key
 * 33024cec6160eafbd2717e394b5bc201
 * and the corresponding FURI is
 * 33/02/4c/ec6160eafbd2717e394b5bc201
 */
#define URI_MODE_FURI_MD5		0x0040

/*
 * If uri_mode is not set the following parameters are in use
 * just as if uri_mode(URI_MODE_FLAG_DEFAULT)
 * was called.
 */
#define URI_MODE_FLAG_DEFAULT		(URI_MODE_CANNONICAL|URI_MODE_ERROR_STDERR|URI_MODE_FURI_MD5)
#define URI_MODE_FLAG_NONE		0

/*
 * The fixed size of a FURI generated using URI_MODE_FURI_MD5
 */
#define URI_FURI_MD5_SIZE	(32 + 3)

/*
 * Get library wide mode
 */
int uri_mode();
/*
 * Set library wide mode
 */
void uri_mode_set(int flag);
/*
 * True if flag is set in library wide mode
 */
int uri_modep(int flag);

/*
 * Possible values of the info field in the uri_t structure.
 */
/*
 * Set if URI is cannonical
 */
#define URI_INFO_CANNONICAL	0x00000001
/*
 * Set if the private uri field is filled.
 */
#define URI_INFO_URI		0x00000002
/*
 * Set if the private furi field is filled.
 */
#define URI_INFO_FURI		0x00000004
/*
 * Set if the URI is relative.
 */
#define URI_INFO_RELATIVE	0x00000008
/*
 * Set if the URI is relative and the path is relative.
 */
#define URI_INFO_RELATIVE_PATH	0x00000010
/*
 * Set if the URI is a null string.
 */
#define URI_INFO_EMPTY		0x00000020
/*
 * Set if the URI has been parsed successfully
 */
#define URI_INFO_PARSED		0x00000040
/*
 * Set if the URI is a robots.txt URI
 */
#define URI_INFO_ROBOTS		0x00000080
/*
 * Set if memory for field was malloc'd
 */
#define URI_INFO_M_SCHEME	0x00000100
#define URI_INFO_M_HOST		0x00000200
#define URI_INFO_M_PORT		0x00000400
#define URI_INFO_M_PATH		0x00000800
#define URI_INFO_M_PARAMS	0x00001000
#define URI_INFO_M_QUERY	0x00002000
#define URI_INFO_M_FRAG		0x00004000
#define URI_INFO_M_USER		0x00008000
#define URI_INFO_M_PASSWD	0x00010000
/*
 * Set if a field was modified using uri_<field>_set
 */
#define URI_INFO_FIELD_CHANGED	0x00020000
/*
 * Set if cannonical object is up to date
 */
#define URI_INFO_CANNONICAL_OK	0x00040000

/*
 * Convinience masks
 */
#define URI_INFO_PARSE_MASK	0x00000038
#define URI_INFO_CACHE_MASK	0x00040087
#define URI_INFO_M_MASK		0x0003ff00
#define URI_INFO_COPY_MASK	(URI_INFO_CACHE_MASK|URI_INFO_M_MASK)

/*
 * Actual definition in uri_schemes.h
 */
struct uri_scheme_desc;

/*
 * Splitted URI information.
 */
typedef struct uri {
#ifndef SWIG
  /* Public fields */
  int info;		/* Information */
  char* scheme;		/* URI scheme (http, ftp...) */
  char* host;		/* hostname part (www.foo.com) */
  char* port;		/* port part if any (www.foo.com:8080 => 8080) */
  char* path;		/* path portion without params and query */
  char* params;		/* params part (/foo;dir/bar => foo) */
  char* query;		/* query part (/foo?bar=val => bar=val) */
  char* frag;		/* frag part (/foo#part => part) */
  char* user;		/* user part (http://user:pass@www.foo.com => user) */
  char* passwd;		/* user part (http://user:pass@www.foo.com => pass) */

  /* Private fields */
  char* pool;
  int pool_size;
  char* furi;
  int furi_size;
  char* _uri;
  int _uri_size;
  char* robots;
  int robots_size;
  struct uri_scheme_desc* desc;
  struct uri* cannonical;
#endif /* SWIG */
} uri_t;

/*
 * Transform URI string in uri_t
 */
uri_t* uri_alloc_1();
uri_t* uri_alloc(char* uri, int uri_length);
int uri_realloc(uri_t* object, char* uri, int uri_length);
/*
 * Release object allocated by uri_alloc
 */
void uri_free(uri_t* object);

/*
 * Access structure fields
 */
int uri_info(uri_t* object);

char* uri_scheme(uri_t* object);
char* uri_host(uri_t* object);
char* uri_port(uri_t* object);
char* uri_path(uri_t* object);
char* uri_params(uri_t* object);
char* uri_query(uri_t* object);
char* uri_frag(uri_t* object);
char* uri_user(uri_t* object);
char* uri_passwd(uri_t* object);

char* uri_netloc(uri_t* object);
char* uri_auth(uri_t* object);
char* uri_auth_netloc(uri_t* object);
char* uri_all_path(uri_t* object);

/*
 * Set structure fields
 */
void uri_info_set(uri_t* object, int value);
void uri_scheme_set(uri_t* object, char* value);
void uri_host_set(uri_t* object, char* value);
void uri_port_set(uri_t* object, char* value);
void uri_path_set(uri_t* object, char* value);
void uri_params_set(uri_t* object, char* value);
void uri_query_set(uri_t* object, char* value);
void uri_frag_set(uri_t* object, char* value);
void uri_user_set(uri_t* object, char* value);
void uri_passwd_set(uri_t* object, char* value);

/*
 * Copy uri_t object
 */
void uri_copy(uri_t* to, uri_t* from);
/*
 * Clone uri_t object, result must be freed using uri_free
 */
uri_t* uri_clone(uri_t* from);
/*
 * Clear all fields, does not deallocate
 */
void uri_clear(uri_t* object);
/*
 * Show fields on stderr.
 */
void uri_dump(uri_t* object);

/*
 * Convert structure into string.
 */
/*
 * Set and get the root for FURI
 */
void uri_set_root(const char* root);
const char* uri_get_root();
/*
 * File equivalent of an URI
 */
char* uri_furi(uri_t* object);
/*
 * URI string
 */
char* uri_uri(uri_t* object);

/*
 * Escaping
 * These functions are provided for convinience but are not needed
 * in the normal process. Component escaping is done via uri_cannonicalize
 * that does it on a per component basis and also controls that 
 * characters used are allowed in each context (for instance , is not
 * allowed in scheme or netloc).
 */

/*
 * Possible values of the charset argument of uri_escape 
 */
#define URI_ESCAPE_RESERVED	";\\/?:@&=+#%"
#define URI_ESCAPE_PATH		";\\?:@&=+#%"
#define URI_ESCAPE_QUERY	";\\/?:@+#%"
extern char* uri_escape_unsafe;

/*
 * Returns a string with all chars contained in the reserved argument
 * in escape form. The result must not be freed.
 */
char* uri_escape(char* string, char* range);
/*
 * Returns a string with all escape sequences converted into chars.
 * The result must not be freed.
 */
char* uri_unescape(char* string);

/*
 * Convinience functions
 */
/*
 * Returns static structure instead of malloc'd structure.
 * Must *not* be freed.
 */
uri_t* uri_object(char* uri, int uri_length);

/*
 * Possible values of the flag argument of uri_furi_string.
 *
 * Prepend the content of the roo (uri_get_root) to the FURI.
 */
#define URI_FURI_REAL_PATH	1
/*
 * Do not prepend anything to the FURI.
 */
#define URI_FURI_NOP		0
/*
 * Equivalent to uri_furi(uri_object(uri, strlen(uri)))
 */
char* uri_furi_string(char* uri, int uri_length, int flag);

/*
 * Possible values of the flag argument of uri_cannonicalize_string.
 */
/*
 * Return a FURI
 */
#define URI_STRING_FURI_STYLE		0x01
/*
 * Build an FURI using MD5 key instead of readable representation
 */
#define URI_STRING_FURI_MD5		0x02
/*
 * Return an URI
 */
#define URI_STRING_URI_STYLE		0x04
/*
 * Return the robots.txt URI corresponding to this URI
 */
#define URI_STRING_ROBOTS_STYLE		0x08
/*
 * Omit the frag in the returned string.
 */
#define URI_STRING_URI_NOHASH_STYLE	0x10
/*
 * Cannonicalize the given uri and return it in the chosen form.
 */
char* uri_cannonicalize_string(char* uri, int uri_length, int flag);

/*
 * Build a string representation of object in stringp according
 * to flags. Possible values of flags is described in uri_cannonicalize_string
 * function.
 * Upon return the stringp pointer points to a static array of 
 * stringp_size bytes allocated with malloc. If stringp is not null
 * it must point to a buffer allocated with malloc and is reallocated
 * to fit the needs of the string conversion.
 */
void uri_string(uri_t* object, char** stringp, int* string_sizep, int flags);

/*
 * The last error message
 */
extern char* uri_errstr;

/*
 * Functions of uri_schemes.c that branch according to scheme.
 */

/*
 * Return values of uri_cannonicalize
 */
#define URI_NOT_CANNONICAL	-1
#define URI_CANNONICAL		0

/*
 * Implement cannonicalization of URI
 */
int uri_cannonicalize(uri_t* object);
#define uri_cannonicalp(o) ((o)->info & (URI_INFO_CANNONICAL|URI_INFO_CANNONICAL_OK))
/*
 * Return the object containing the cannonical form of object.
 */
uri_t* uri_cannonical(uri_t* object);

/*
 * Parse string pointed by pool and fill fields.
 */
int uri_parse(uri_t* object);

/*
 * Set object in scheme mode. Mostly usefull for relative URLs
 * that do not contain a scheme if you want the parsing mechanism
 * to behave in a given manner.
 */
int uri_scheme_switch(uri_t* object, char* scheme, int scheme_length);

/*
 * Returns 0 if object contains unparsable URL, returns != 0 if
 * object contains well formed URL. Must be called after a set
 * of field changes to reset flags and ensure that modified URL
 * is well formed.
 */
int uri_consistent(uri_t* object);

/*
 * Transform relative URI in absolute URI according to base.
 */
uri_t* uri_abs(uri_t* base, char* relative_string, int relative_length);
uri_t* uri_abs_1(uri_t* base, uri_t* relative);

/*
 * The following functions are not available for all scheme. Their
 * implementation may be found in the corresponding uri_scheme_???.c file.
 */

/*
 * HTTP specific
 */

/*
 * robots.txt URI corresponding to this URI.
 */
char* uri_robots(uri_t* object);

/*
 * Gopher specific
 */
#ifndef SWIG

#define gopher_type	user
#define gopher_selector	params
#define gopher_search	query
#define gopher_string	frag
#define gopher_tmp	robots
#define gopher_tmp_size	robots_size

#define URI_INFO_M_GOPHER_TYPE		URI_INFO_M_USER
#define URI_INFO_M_GOPHER_SELECTOR	URI_INFO_M_PARAMS
#define URI_INFO_M_GOPHER_SEARCH	URI_INFO_M_QUERY
#define URI_INFO_M_GOPHER_STRING	URI_INFO_M_FRAG

#endif /* SWIG */

char* uri_gopher_type(uri_t* object);
char* uri_gopher_selector(uri_t* object);
char* uri_gopher_search(uri_t* object);
char* uri_gopher_string(uri_t* object);

void uri_gopher_type_set(uri_t* object, char* value);
void uri_gopher_selector_set(uri_t* object, char* value);
void uri_gopher_search_set(uri_t* object, char* value);
void uri_gopher_string_set(uri_t* object, char* value);

/*
 * Nntp specific
 */
#ifndef SWIG

#define nntp_group	path
#define nntp_article	params

#endif /* SWIG */

char* uri_nntp_group(uri_t* object);
char* uri_nntp_article(uri_t* object);

void uri_nntp_group_set(uri_t* object, char* value);
void uri_nntp_article_set(uri_t* object, char* value);

/*
 * Wais specific
 */
#ifndef SWIG

#define wais_database	path
#define wais_wtype	params
#define wais_wpath	frag

#endif /* SWIG */

char* uri_wais_database(uri_t* object);
char* uri_wais_wtype(uri_t* object);
char* uri_wais_wpath(uri_t* object);

void uri_wais_database_set(uri_t* object, char* value);
void uri_wais_wtype_set(uri_t* object, char* value);
void uri_wais_wpath_set(uri_t* object, char* value);

#ifndef SWIG
#ifdef __cplusplus
}
#endif
#endif /* SWIG */

#ifdef ENABLE_NLS
/* This is an internationalized library ! */
#include <libintl.h>
#define _(String) dgettext("uri", String)
#else
#define _(String) (String)
#endif /* ENABLE_NLS */

#endif /* _uri_h */
