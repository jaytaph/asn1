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
#include "libtasn1.h"

typedef struct _php_asn1_obj {
	zend_object zo;

} php_asn1_obj;

/* The class entry pointers */
zend_class_entry *php_asn1_exception_class_entry;
zend_class_entry *php_asn1_sc_entry;

/* The object handlers */
static zend_object_handlers asn1_object_handlers;


/* {{{ proto ASN1::__construct()
 * Constructs a new ASN1 object. */
PHP_METHOD(ASN1, __construct) {
	php_asn1_obj *intern;
#if ZEND_MODULE_API_NO > 20060613
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, php_asn1_exception_class_entry, &error_handling TSRMLS_CC);
#else
	php_set_error_handling(EH_THROW, php_asn1_exception_class_entry TSRMLS_CC);
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
#if ZEND_MODULE_API_NO > 20060613
	zend_restore_error_handling(&error_handling TSRMLS_CC);
#else
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
#endif
		return;
	}

	intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);

#if ZEND_MODULE_API_NO > 20060613
	zend_restore_error_handling(&error_handling TSRMLS_CC);
#else
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
#endif
}
/* }}} */


/* {{{ proto ASN1::check_version($required_version)
 * Check if the required version i */
PHP_METHOD(ASN1, check_version) {
	char *version = NULL;
	int version_len = 0;
	php_asn1_obj *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &version, &version_len) == FAILURE) {
		RETURN_FALSE
	}

        if (! asn1_check_version(version)) {
                RETURN_FALSE
        }

        RETURN_TRUE
}
/* }}} */


/* {{{ */
static void asn1_object_free(void *object TSRMLS_DC) {
	php_asn1_obj *intern = (php_asn1_obj *)object;

        // @TODO: Free any internal data

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	efree(object);
}
/* }}} */

/* {{{ */
static zend_object_value asn1_object_new_ex(zend_class_entry *class_type, php_asn1_obj **ptr TSRMLS_DC) {
	php_asn1_obj *intern;
	zend_object_value retval;
	zval *tmp;

	intern = emalloc(sizeof(php_asn1_obj));
	memset(intern, 0, sizeof(php_asn1_obj));

	if (ptr) {
		*ptr = intern;
	}

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) asn1_object_free, NULL TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &asn1_object_handlers;
	return retval;
}
/* }}} */

/* {{{ */
static zend_object_value asn1_object_new(zend_class_entry *class_type TSRMLS_DC) {
	return asn1_object_new_ex(class_type, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ */
static zend_object_value asn1_object_clone(zval *this_ptr TSRMLS_DC) {
	php_asn1_obj *new_obj = NULL;
	php_asn1_obj *old_obj = (php_asn1_obj *)zend_object_store_get_object(this_ptr TSRMLS_CC);
	zend_object_value        retval = asn1_object_new_ex(old_obj->zo.ce, &new_obj TSRMLS_CC);

	zend_objects_clone_members(&new_obj->zo, retval, &old_obj->zo, Z_OBJ_HANDLE_P(this_ptr) TSRMLS_CC);

        // @TODO: Copy over data
/*
	php_asn1_data_init(&new_obj->image);
	php_asn1_data_clone(&old_obj->image, &new_obj->image);
*/

	return retval;
}
/* }}} */



static zend_function_entry asn1_funcs[] = {
  	PHP_ME(ASN1, __construct, NULL, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	PHP_ME(ASN1, check_version, NULL, ZEND_ACC_PUBLIC)

	/* End of functions */
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(asn1)
{
	zend_class_entry ce;

        /* Set up the object handlers */
	memcpy(&asn1_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	/*
	 * ASN1 class
	 */
	INIT_CLASS_ENTRY(ce, "ASN1", asn1_funcs);
	ce.create_object = asn1_object_new;
	asn1_object_handlers.clone_obj = asn1_object_clone;
	//asn1_object_handlers.read_property = asn1_object_read_property;
	php_asn1_sc_entry = zend_register_internal_class(&ce TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(asn1)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "asn.1 support", "enabled");
	php_info_print_table_row(2, "Version", PHP_ASN1_VERSION);
        php_info_print_table_row(2, "ASN1 Library Version", ASN1_VERSION);
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
	PHP_MINFO(asn1),
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

