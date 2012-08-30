/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 321634 2012-01-01 13:15:04Z felipe $ */

#ifndef PHP_EXQRCODE_H
#define PHP_EXQRCODE_H

extern zend_module_entry exqrcode_module_entry;
#define phpext_exqrcode_ptr &exqrcode_module_entry

#ifdef PHP_WIN32
#	define PHP_EXQRCODE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_EXQRCODE_API __attribute__ ((visibility("default")))
#else
#	define PHP_EXQRCODE_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif


ZEND_BEGIN_MODULE_GLOBALS(hello)
long counter;
ZEND_END_MODULE_GLOBALS(hello)
#ifdef ZTS
#define HELLO_G(v) TSRMG(hello_globals_id, zend_hello_globals *, v)
#else
#define HELLO_G(v) (hello_globals.v)
#endif


PHP_MINIT_FUNCTION(exqrcode);
PHP_MSHUTDOWN_FUNCTION(exqrcode);
PHP_RINIT_FUNCTION(exqrcode);
PHP_RSHUTDOWN_FUNCTION(exqrcode);
PHP_MINFO_FUNCTION(exqrcode);

PHP_FUNCTION(confirm_exqrcode_compiled);	/* For testing, remove later. */
PHP_FUNCTION(qr_encode);

PHP_FUNCTION(helloint);
PHP_FUNCTION(hello_world);
PHP_FUNCTION(hello_long);
PHP_FUNCTION(hello_greetme);
PHP_FUNCTION(hello_add);
PHP_FUNCTION(hello_array);
PHP_FUNCTION(hello_array_strings);
PHP_FUNCTION(hello_person_new);
PHP_FUNCTION(hello_person_pnew);
PHP_FUNCTION(hello_person_greet);
PHP_FUNCTION(hello_person_delete);

static void php_hello_person_persist_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void php_hello_person_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);





typedef struct _php_hello_person {
char *name;
int name_len;
long age;
} php_hello_person;
#define PHP_HELLO_PERSON_RES_NAME "Person Data"


/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(exqrcode)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(exqrcode)
*/

/* In every utility function you add that needs to use variables 
   in php_exqrcode_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as EXQRCODE_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define EXQRCODE_G(v) TSRMG(exqrcode_globals_id, zend_exqrcode_globals *, v)
#else
#define EXQRCODE_G(v) (exqrcode_globals.v)
#endif

#endif	/* PHP_EXQRCODE_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
