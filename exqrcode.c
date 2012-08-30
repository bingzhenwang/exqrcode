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
  | Author: bingzhenwang@hotmail.com                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 321634 2012-01-01 13:15:04Z felipe $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_exqrcode.h"
#include <qrencode.h>



#define INCHES_PER_METER (100.0/2.54)
//max of qr text
#define TEXT_MAX 1024

static int casesensitive = 1;
static int eightbit = 0;
static int version = 0;
static int size = 3;
static int margin = -1;
static int dpi = 72;
static int structured = 0;
static int micro = 0;
static QRecLevel level = QR_ECLEVEL_L;
//static QRecLevel level = QR_ECLEVEL_M;

static QRencodeMode hint = QR_MODE_8;

enum imageType {
	PNG_TYPE,
	EPS_TYPE,
	ANSI_TYPE,
	ANSI256_TYPE,
	ASCII_TYPE,
	ASCIIi_TYPE
};
static enum imageType image_type = PNG_TYPE;
#define MAX_DATA_SIZE (7090 * 16) /* from the specification */


static FILE *openFile(const char *outfile)
{
	FILE *fp;

	if(outfile == NULL || (outfile[0] == '-' && outfile[1] == '\0')) {
		fp = stdout;
	} else {
		fp = fopen(outfile, "wb");
		if(fp == NULL) {
			fprintf(stderr, "Failed to create file: %s\n", outfile);
			perror(NULL);
			exit(EXIT_FAILURE);
		}
	}

	return fp;
}


static QRcode *encode(const unsigned char *intext, int length)
{
	QRcode *code;

	if(micro) {
		if(eightbit) {
			code = QRcode_encodeDataMQR(length, intext, version, level);
		} else {
			code = QRcode_encodeStringMQR((char *)intext, version, level, hint, casesensitive);
		}
	} else {
		if(eightbit) {
			code = QRcode_encodeData(length, intext, version, level);
		} else {
			code = QRcode_encodeString((char *)intext, version, level, hint, casesensitive);
		}
	}

	return code;
}


static int write_to_file(char *text, char *outfile) {
		
		if(margin < 0) {
			if(micro) {
				margin = 2;
			} else {
				margin = 4;
			}
		}
	
		//qrencode(name, name_len, outfile);
		QRcode *qrcode;
		int text_len = strlen(text);
		qrcode = encode(text, text_len);
		//writePNG(qrcode, outfile);
		//writePNG start
		static FILE *fp; // avoid clobbering by setjmp.
		png_structp png_ptr;
		png_infop info_ptr;
		unsigned char *row, *p, *q;
		int x, y, xx, yy, bit;
		int realwidth;
	
		realwidth = (qrcode->width + margin * 2) * size;
		row = (unsigned char *)malloc((realwidth + 7) / 8);
		if(row == NULL) {
			php_printf("Failed to allocate memory.\n");
			return 0;
		}
	
		if(outfile[0] == '-' && outfile[1] == '\0') {
			fp = stdout;
		} else {
			fp = fopen(outfile, "wb");
			if(fp == NULL) {
				php_printf( "Failed to create file: %s\n", outfile);
				
				return 0;
			}
		}
	
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if(png_ptr == NULL) {
			php_printf( "Failed to initialize PNG writer.\n");
			return 0;
		}
	
		info_ptr = png_create_info_struct(png_ptr);
		if(info_ptr == NULL) {
			php_printf(  "Failed to initialize PNG write.\n");
			return 0;
		}
	
		if(setjmp(png_jmpbuf(png_ptr))) {
			png_destroy_write_struct(&png_ptr, &info_ptr);
			php_printf("Failed to write PNG image.\n");
			return 0;
		}
	
		png_init_io(png_ptr, fp);
		png_set_IHDR(png_ptr, info_ptr,
				realwidth, realwidth,
				1,
				PNG_COLOR_TYPE_GRAY,
				PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_DEFAULT,
				PNG_FILTER_TYPE_DEFAULT);
		png_set_pHYs(png_ptr, info_ptr,
				dpi * INCHES_PER_METER,
				dpi * INCHES_PER_METER,
				PNG_RESOLUTION_METER);
		png_write_info(png_ptr, info_ptr);
	
		/* top margin */
		memset(row, 0xff, (realwidth + 7) / 8);
		for(y=0; y<margin * size; y++) {
			png_write_row(png_ptr, row);
		}
	
		/* data */
		
		p = qrcode->data;
		for(y=0; y<qrcode->width; y++) {
			bit = 7;
			memset(row, 0xff, (realwidth + 7) / 8);
			q = row;
			q += margin * size / 8;
			bit = 7 - (margin * size % 8);
			for(x=0; x<qrcode->width; x++) {
				for(xx=0; xx<size; xx++) {
					*q ^= (*p & 1) << bit;
					bit--;
					if(bit < 0) {
						q++;
						bit = 7;
					}
				}
				p++;
			}
			for(yy=0; yy<size; yy++) {
				png_write_row(png_ptr, row);
			}
		}
		/* bottom margin */
		memset(row, 0xff, (realwidth + 7) / 8);
		for(y=0; y<margin * size; y++) {
			png_write_row(png_ptr, row);
		}
	
		png_write_end(png_ptr, info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);
	
		fclose(fp);
		free(row);
		//writePNG end
		QRcode_free(qrcode);
		return 1;

}

/* If you declare any globals in php_exqrcode.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(exqrcode)
*/

/* True global resources - no need for thread safety here */
static int le_exqrcode;

/* {{{ exqrcode_functions[]
 *
 * Every user visible function must have an entry in exqrcode_functions[].
 */



const zend_function_entry exqrcode_functions[] = {
	//PHP_FE(confirm_exqrcode_compiled,	NULL)		/* For testing, remove later. */
	{NULL,NULL,NULL}
	//PHP_FE_END	/* Must be the last line in exqrcode_functions[] */
};
/* }}} */

PHP_METHOD(ExQrcode, __construct);

PHP_METHOD(ExQrcode, setDpi);
PHP_METHOD(ExQrcode, setLevel);
PHP_METHOD(ExQrcode, setSize);

PHP_METHOD(ExQrcode, encodeGeneral);
PHP_METHOD(ExQrcode, encodeUrl);
PHP_METHOD(ExQrcode, encodeVCard);

PHP_METHOD(ExQrcode, encodeMECard);



PHP_METHOD(ExQrcode, encodePhone);


PHP_METHOD(ExQrcode, encodeSMS);

PHP_METHOD(ExQrcode, encodeICAL);

PHP_METHOD(ExQrcode, writeToFile);


zend_class_entry *exqrcode_ce;
 
/* 类方法的参数 */
ZEND_BEGIN_ARG_INFO(setMargin_args, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()


static const zend_function_entry exqrcode_methods[] = {
    ZEND_ME(ExQrcode, __construct,
            NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)

	//PHP_ME(phppan, getName, 	 	 	NULL, ZEND_ACC_PUBLIC) 
	//PHP_ME(ExQrcode, setMargin, 	 	 	setMargin_args, ZEND_ACC_PUBLIC)
	ZEND_ME(ExQrcode, setDpi,
            NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ExQrcode, setLevel,
            NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ExQrcode, setSize,
            NULL, ZEND_ACC_PUBLIC)        
    ZEND_ME(ExQrcode, encodeGeneral,
            NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ExQrcode, encodeUrl,
            NULL, ZEND_ACC_PUBLIC)
    //ZEND_ME(ExQrcode, encodeVCard,
    //       NULL, ZEND_ACC_PUBLIC)
	ZEND_ME(ExQrcode, encodeMECard,
					NULL, ZEND_ACC_PUBLIC)
	ZEND_ME(ExQrcode, encodePhone,
            NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ExQrcode, encodeSMS,
            NULL, ZEND_ACC_PUBLIC)

	ZEND_ME(ExQrcode, writeToFile,
            NULL, ZEND_ACC_PUBLIC)        
    {NULL, NULL, NULL}
};

 PHP_METHOD(ExQrcode, __construct) {
	RETURN_TRUE;
}
 PHP_METHOD(ExQrcode, setDpi) {
	 	char * str = NULL;
		
			int str_len;
		
			if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s",&str,&str_len) == FAILURE)
		
				RETURN_FALSE;
			//set level
			unsigned short tm_dpi = (unsigned short) atoi(str);
			level = tm_dpi;
				 
			 RETURN_TRUE;

 	}

PHP_METHOD(ExQrcode, setLevel) {

   		char * str = NULL;
   
	   int str_len;
   
	   if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s",&str,&str_len) == FAILURE)
   
		   RETURN_FALSE;
	   //set level
	   unsigned short tm_level = (unsigned short) atoi(str);
 	    if(tm_level >3) {
		   level = 3;

	   	} else {
		level = tm_level;
	   		}
		RETURN_TRUE;
   }

PHP_METHOD(ExQrcode, setSize) {
			char * str = NULL;
	   
		   int str_len;
	   
		   if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s",&str,&str_len) == FAILURE)
	   
			   RETURN_FALSE ;
		   //set level
		   unsigned short tm_size = (unsigned short) atoi(str);
			size = tm_size;
			RETURN_TRUE;

   }


PHP_METHOD(ExQrcode, encodeGeneral){

		char *arg = NULL;
		int arg_len;
		zval *value, *self;
	 
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	 
		self = getThis();
	 
		MAKE_STD_ZVAL(value);
		ZVAL_STRING(value, arg, 1);
	 
		SEPARATE_ZVAL_TO_MAKE_IS_REF(&value);
		zend_update_property(Z_OBJCE_P(self), self, ZEND_STRL("_text"), value TSRMLS_CC);

		

 	}
PHP_METHOD(ExQrcode, encodeUrl){
			char *arg = NULL;
			int arg_len;
			zval *value, *self;
		 
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
		 
			self = getThis();
		 
			MAKE_STD_ZVAL(value);
			ZVAL_STRING(value, arg, 1);
		 
			SEPARATE_ZVAL_TO_MAKE_IS_REF(&value);
			zend_update_property(Z_OBJCE_P(self), self, ZEND_STRL("_text"), value TSRMLS_CC);

 	}
PHP_METHOD(ExQrcode, encodeVCard){

//BEGIN:VCARD\nN:晖;刘\nTEL:+86 186********\nEMAIL:***@gmail.com\nURL:www.xieyidian.com\nEND:VCARD

 	}
/**
* MECARD:N:xxx;TEL:c;EMAIL:c;ADR:c;URL:c
* array("N" => "name","TEL" => "1390125","EMAIL" => "email" 
* , "ADDR" => "address" ,"URL" => "http://www.helloworld.com")

**/
PHP_METHOD(ExQrcode, encodeMECard){
	zval *arr, **data;
	zval *value, *self;
	
	char tm_text[TEXT_MAX] = "MECARD:";
	int tm_text_len = strlen(tm_text);
	HashTable *arr_hash;
	HashPosition pointer;
	int array_count;
	char *key;
	int key_len;
	long index;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arr) == FAILURE) {
	RETURN_FALSE;
	}
	arr_hash = Z_ARRVAL_P(arr);
	array_count = zend_hash_num_elements(arr_hash);
	 
	 php_printf("The array passed contains %d elements\n", array_count);
	 for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer);
	 zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS;
	 zend_hash_move_forward_ex(arr_hash, &pointer)) {
	 	

	//get data Z_TYPE_PP(data) == IS_STRING
	if (zend_hash_get_current_key_ex(arr_hash, &key, &key_len, &index, 0, &pointer) == HASH_KEY_IS_STRING) {
	//PHPWRITE(Z_STRVAL_PP(data), Z_STRLEN_PP(data));
	//continue;
	//check length, strlen("EMAIL:;") = 7 
	if (Z_STRLEN_PP(data) + 7 + tm_text_len > TEXT_MAX) {
			break;
		}
		if (strcmp("N",key) == 0) {
				tm_text_len += sprintf(tm_text + tm_text_len,"N:%s;",Z_STRVAL_PP(data)); 
				//php_printf("lengthof name %d %d\n",strlen(Z_STRVAL_PP(data)),Z_STRLEN_PP(data));
				//break;
	 		}
 		else if (strcmp("TEL",key) == 0) {
 			tm_text_len += sprintf(tm_text + tm_text_len,"TEL:%s;",Z_STRVAL_PP(data)); 
 
 
 			}
 		else if (strcmp("EMAIL",key) == 0) {
 			tm_text_len += sprintf(tm_text + tm_text_len ,"EMAIL:%s;",Z_STRVAL_PP(data)); 
 
 
 			}
 		else if (strcmp("ADDR",key)  == 0) {
 			tm_text_len += sprintf(tm_text + tm_text_len,"ADDR:%s;",Z_STRVAL_PP(data)); 
 
 
 			}
 		else if (strcmp("URL",key) == 0) {
 			tm_text_len += sprintf(tm_text + tm_text_len ,"URL:%s;",Z_STRVAL_PP(data)); 
 			}
		else {
			;
			}

	} else {
	//php_printf("%ld", index);
	}
	//php_printf("%s", tm_text);

	 //RETURN_TRUE;
	 }
	//update _text
	//php_printf("%s", tm_text);
	self = getThis();
	MAKE_STD_ZVAL(value);
	ZVAL_STRING(value, tm_text, 1);
	SEPARATE_ZVAL_TO_MAKE_IS_REF(&value);
	zend_update_property(Z_OBJCE_P(self), self, ZEND_STRL("_text"), value TSRMLS_CC);

}

/**
* phone:xxx

**/
PHP_METHOD(ExQrcode, encodePhone){
	
	
	char *arg_s = NULL;
			int arg_s_len;
			char *arg_c = NULL;
			int arg_c_len;
			char tm_text[TEXT_MAX] = "phone:";
			int tm_text_len = strlen(tm_text);
			zval *value, *self;
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg_s, &arg_s_len) == FAILURE) {
							WRONG_PARAM_COUNT;
							RETURN_FALSE;
			}
			if ( arg_s_len +   tm_text_len > 50) {
				RETURN_FALSE;
			}
			//todo check phone
			
			tm_text_len += sprintf(tm_text + tm_text_len,"%s",arg_s); 
			
			
			self = getThis();
			MAKE_STD_ZVAL(value);
			ZVAL_STRING(value, tm_text, 1);
			SEPARATE_ZVAL_TO_MAKE_IS_REF(&value);
			zend_update_property(Z_OBJCE_P(self), self, ZEND_STRL("_text"), value TSRMLS_CC);

	

}

/**
** sms:s:c

**/
PHP_METHOD(ExQrcode, encodeSMS){
		char *arg_s = NULL;
		int arg_s_len;
		char *arg_c = NULL;
		int arg_c_len;
		char tm_text[TEXT_MAX] = "sms:";
		int tm_text_len = strlen(tm_text);
		zval *value, *self;
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &arg_s, &arg_s_len,&arg_c, &arg_c_len) == FAILURE) {
						WRONG_PARAM_COUNT;
						RETURN_FALSE;
		}
		if (arg_c_len +arg_s_len + 7 + tm_text_len > TEXT_MAX) {
			RETURN_FALSE;
		}
		
		tm_text_len += sprintf(tm_text + tm_text_len,"%s:%s",arg_s,arg_c); 
		
	 	
		self = getThis();
		MAKE_STD_ZVAL(value);
		ZVAL_STRING(value, tm_text, 1);
		SEPARATE_ZVAL_TO_MAKE_IS_REF(&value);
		zend_update_property(Z_OBJCE_P(self), self, ZEND_STRL("_text"), value TSRMLS_CC);

}


PHP_METHOD(ExQrcode, writeToFile){

			char *filename = NULL;
			int arg_len;
			 
		 
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &arg_len) == FAILURE) {
				WRONG_PARAM_COUNT;
			}

		zval *self, *text;
	 
		self = getThis();
	 
		text = zend_read_property(Z_OBJCE_P(self), self, ZEND_STRL("_text"), 0 TSRMLS_CC);
		//php_printf("text:%s",Z_STRVAL_P(text));
if (write_to_file(Z_STRVAL_P(text),filename)) {
	RETURN_TRUE;
} else {
	RETURN_FALSE;

}
	 	
		//RETURN_STRING(Z_STRVAL_P(name), 0);
		

 	}


/* }}} */


/* {{{ exqrcode_module_entry
 */
 

//ZEND_DECLARE_MODULE_GLOBALS(hello)

zend_module_entry exqrcode_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"exqrcode",
	exqrcode_functions,
	PHP_MINIT(exqrcode),
	PHP_MSHUTDOWN(exqrcode),
	PHP_RINIT(exqrcode),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(exqrcode),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(exqrcode),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */




#ifdef COMPILE_DL_EXQRCODE
ZEND_GET_MODULE(exqrcode)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("exqrcode.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_exqrcode_globals, exqrcode_globals)
    STD_PHP_INI_ENTRY("exqrcode.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_exqrcode_globals, exqrcode_globals)
PHP_INI_END()
*/
PHP_INI_BEGIN()
PHP_INI_ENTRY("exqrcode.greeting", "exqrcode", PHP_INI_ALL, NULL)
PHP_INI_END()


/* }}} */

/* {{{ php_exqrcode_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_exqrcode_init_globals(zend_exqrcode_globals *exqrcode_globals)
{
	exqrcode_globals->global_value = 0;
	exqrcode_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
 


PHP_MINIT_FUNCTION(exqrcode)
{
	zend_class_entry ExQrcode;
 
	INIT_CLASS_ENTRY(ExQrcode, "ExQrcode", exqrcode_methods);
	exqrcode_ce = zend_register_internal_class_ex(&ExQrcode, NULL, NULL TSRMLS_CC);
 
 
	/* 声明常量URL */
	
 	zend_declare_class_constant_stringl(exqrcode_ce, ZEND_STRL("QRLEVEL_L"), ZEND_STRL("0") TSRMLS_CC);
	zend_declare_class_constant_stringl(exqrcode_ce, ZEND_STRL("QRLEVEL_M"), ZEND_STRL("1") TSRMLS_CC);
	zend_declare_class_constant_stringl(exqrcode_ce, ZEND_STRL("QRLEVEL_Q"), ZEND_STRL("2") TSRMLS_CC);
	zend_declare_class_constant_stringl(exqrcode_ce, ZEND_STRL("QRLEVEL_H"), ZEND_STRL("3") TSRMLS_CC);

	/* 声明私有成员变量 _text  */
	zend_declare_property_null(exqrcode_ce, ZEND_STRL("_text"), ZEND_ACC_PRIVATE TSRMLS_CC);

	zend_declare_property_null(exqrcode_ce, ZEND_STRL("_dpi"), ZEND_ACC_PRIVATE TSRMLS_CC);

	zend_declare_property_null(exqrcode_ce, ZEND_STRL("_size"), ZEND_ACC_PRIVATE TSRMLS_CC);

	zend_declare_property_null(exqrcode_ce, ZEND_STRL("_level"), ZEND_ACC_PRIVATE TSRMLS_CC);

	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	

REGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(exqrcode)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
//UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(exqrcode)
{

	
return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(exqrcode)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(exqrcode)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "exqrcode support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
