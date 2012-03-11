/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Joshua Thijssen <jthijssen@noxlogic.nl>                      |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "php.h"
#include "php_main.h"
#include "php_globals.h"
#include "zend_objects.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include "php_asn1.h"


static zend_function_entry asn1_funcs[] = {
	/* End of functions */
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(asn1)
{
	zend_class_entry ce;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(asn1)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "asn.1 support", "enabled");
	php_info_print_table_row(2, "version", PHP_ASN1_VERSION);
	php_info_print_table_end();
}
/* }}} */

zend_module_entry asn1_module_entry = {
	STANDARD_MODULE_HEADER,
	"asn1",
	asn1_funcs,
	PHP_MINIT(asn1),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(ans1),
	PHP_ASN1_VERSION,
	STANDARD_MODULE_PROPERTIES
};

//#ifdef COMPILE_DL_ASN1
ZEND_GET_MODULE(asn1)
//#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */

