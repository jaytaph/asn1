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
        ASN1_TYPE definitions;
        ASN1_TYPE structure;

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

        if (zend_parse_parameters_none() == FAILURE) {
#if ZEND_MODULE_API_NO > 20060613
                zend_restore_error_handling(&error_handling TSRMLS_CC);
#else
                php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
#endif
		return;
	}

	intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        intern->definitions = ASN1_TYPE_EMPTY;
        intern->structure = ASN1_TYPE_EMPTY;

#if ZEND_MODULE_API_NO > 20060613
	zend_restore_error_handling(&error_handling TSRMLS_CC);
#else
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
#endif
}
/* }}} */

/* {{{ proto static ASN1::get_error_string()
 * Returns textual representation for error */
PHP_METHOD(ASN1, get_error_string) {
        long error_code;
        const char *error_str;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &error_code) == FAILURE) {
		RETURN_FALSE
	}

        error_str = asn1_strerror(error_code);

        if (error_str) {
                RETURN_STRING(error_str, 1)
        }

        RETURN_NULL();
}
/* }}} */

/* {{{ proto static ASN1::get_version()
 * Returns current library version */
PHP_METHOD(ASN1, get_version) {

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        RETURN_STRING(asn1_check_version(NULL), 1)
}
/* }}} */

/* {{{ proto static ASN1::check_version($required_version)
 * Check if the required version i */
PHP_METHOD(ASN1, check_version) {
	char *version = NULL;
	int version_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &version, &version_len) == FAILURE) {
		RETURN_FALSE
	}

        if (! asn1_check_version(version)) {
                RETURN_FALSE
        }

        RETURN_TRUE
}
/* }}} */


/* {{{ proto ASN1::asn1_parser2tree
 *  Return status code */
PHP_METHOD(ASN1, parser2tree) {
	php_asn1_obj *intern;
        char *filename = NULL;
	int filename_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
                RETURN_FALSE
        }

	intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);

        int result = asn1_parser2tree(filename, &intern->definitions, NULL);
        RETURN_LONG(result)
}
/* }}} */


PHP_METHOD(ASN1, dump_structure) {
        php_asn1_obj *intern;
        char *filename = NULL;
        int filename_len = 0;
        char *tag = NULL;
        int tag_len = 0;
        int what = ASN1_PRINT_ALL;
        int structure = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ll", &filename, &filename_len, &tag, &tag_len, &what, &structure) == FAILURE) {
                RETURN_FALSE
        }

        if (what <= 0 || what >= ASN1_PRINT_ALL) {
                what = ASN1_PRINT_ALL;
        }

        FILE *f = fopen(filename, "w");
        if (!f) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        asn1_print_structure(f, structure ? intern->definitions : intern->structure, tag, what);
        fclose(f);

        RETURN_TRUE
}

PHP_METHOD(ASN1, create_element) {
        php_asn1_obj *intern;
        char *name = NULL;
        int name_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        //int result = asn1_create_element (intern->definitions, name, &intern->structure);

        int result = 0;
        RETURN_LONG(result)
}


PHP_METHOD(ASN1, delete_element) {
        php_asn1_obj *intern;
        char *name = NULL;
        int name_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int result = asn1_delete_element (intern->structure, name);

        RETURN_LONG(result)
}

PHP_METHOD(ASN1, count_elements) {
        php_asn1_obj *intern;
        int count = 0;

        if (zend_parse_parameters_none() == FAILURE) {
          RETURN_FALSE
        }

        printf("count");

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int result = asn1_number_of_elements(intern->structure, "", &count);

        printf("C'%d': %d", count, result);

        if (result != ASN1_SUCCESS) {
                RETURN_FALSE
        }

        RETURN_LONG(count)
}

PHP_METHOD(ASN1, write_element) {
        php_asn1_obj *intern;
        char *name = NULL;
        int name_len = 0;
        char *value = NULL;
        int value_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &value, &value_len) == FAILURE) {
                RETURN_FALSE
        }

        printf("writing: %s with %s\n", name, value);

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int result = asn1_write_value (intern->structure, name, value, value_len);

        RETURN_LONG(result)
}

PHP_METHOD(ASN1, find_from_oid) {
        php_asn1_obj *intern;
        char *oid = NULL;
        int oid_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &oid, &oid_len) == FAILURE) {
                RETURN_FALSE
        }

        printf("find_from_oid: %s \n", oid);

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        const char *result = asn1_find_structure_from_oid(intern->structure, oid);

        if (! result) {
                RETURN_NULL()
        }

        RETURN_STRING(result, 1)
}

PHP_METHOD(ASN1, read_tag) {
        php_asn1_obj *intern;
        char *tag = NULL;
        int tag_len = 0;
        int value, class = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tag, &tag_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int result = asn1_read_tag(intern->structure, tag, &value, &class);
        if (result != ASN1_SUCCESS) {
                RETURN_FALSE
        }

        object_init(return_value);
        zend_update_property_long(NULL, return_value, "value", strlen("value"), value TSRMLS_CC);
        zend_update_property_long(NULL, return_value, "class", strlen("class"), class TSRMLS_CC);
}

PHP_METHOD(ASN1, read_value) {
        php_asn1_obj *intern;
        char *tag = NULL;
        int tag_len = 0;
        char *value = NULL;
        int value_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tag, &tag_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int result = asn1_read_value(intern->structure, tag, &value, &value_len);
        if (result != ASN1_SUCCESS) {
                RETURN_FALSE
        }

        RETURN_STRINGL(value, value_len, 1);
}


/* {{{ */
static void asn1_object_free(void *object TSRMLS_DC) {
	php_asn1_obj *intern = (php_asn1_obj *)object;

        // Free any internal data
        asn1_delete_structure(&intern->definitions);
        asn1_delete_structure(&intern->structure);

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
	//php_asn1_data_clone(&old_obj->definitions, &new_obj->definitions);

	return retval;
}
/* }}} */



static zend_function_entry asn1_funcs[] = {
  	PHP_ME(ASN1, __construct,      NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(ASN1, check_version,    NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(ASN1, get_version,      NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(ASN1, get_error_string, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(ASN1, parser2tree,      NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, dump_structure,   NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, create_element,   NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, write_element,    NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, delete_element,   NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, count_elements,   NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, find_from_oid,    NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, read_tag,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, read_value,       NULL, ZEND_ACC_PUBLIC)

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
         * ASN1 constants (returned from read_tag)
         */
        REGISTER_LONG_CONSTANT("CLASS_UNIVERSAL",        ASN1_CLASS_UNIVERSAL,        CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("CLASS_APPLICATION",      ASN1_CLASS_APPLICATION,      CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("CLASS_CONTEXT_SPECIFIC", ASN1_CLASS_CONTEXT_SPECIFIC, CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("CLASS_PRIVATE",          ASN1_CLASS_PRIVATE,          CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("CLASS_STRUCTURED",       ASN1_CLASS_STRUCTURED,       CONST_CS | CONST_PERSISTENT);

        REGISTER_LONG_CONSTANT("TAG_BOOLEAN",          ASN1_TAG_BOOLEAN,         CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("TAG_INTEGER",          ASN1_TAG_INTEGER,         CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("TAG_SEQUENCE",         ASN1_TAG_SEQUENCE,        CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("TAG_SET",   	       ASN1_TAG_SET,             CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("TAG_OCTET_STRING",     ASN1_TAG_OCTET_STRING,    CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("TAG_BIT_STRING",       ASN1_TAG_BIT_STRING,      CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("TAG_UTCTime",          ASN1_TAG_UTCTime,         CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("TAG_GENERALIZEDTime",  ASN1_TAG_GENERALIZEDTime, CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("TAG_OBJECT_ID",        ASN1_TAG_OBJECT_ID,       CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("TAG_ENUMERATED",       ASN1_TAG_ENUMERATED,      CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("TAG_NULL",   	       ASN1_TAG_NULL,            CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("TAG_GENERALSTRING",    ASN1_TAG_GENERALSTRING,   CONST_CS | CONST_PERSISTENT);

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

