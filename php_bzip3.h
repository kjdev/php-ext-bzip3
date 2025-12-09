
#ifndef PHP_BZIP3_H
#define PHP_BZIP3_H

#define PHP_BZIP3_EXT_VERSION "0.1.2"
#define PHP_BZIP3_NS "Bzip3"

extern zend_module_entry bzip3_module_entry;
#define phpext_bzip3_ptr &bzip3_module_entry

#ifdef PHP_WIN32
#   define PHP_BZIP3_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#   define PHP_BZIP3_API __attribute__ ((visibility("default")))
#else
#   define PHP_BZIP3_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef ZTS
#define PHP_BZIP3_G(v) TSRMG(bzip3_globals_id, zend_bzip3_globals *, v)
#else
#define PHP_BZIP3_G(v) (bzip3_globals.v)
#endif

#endif /* PHP_BZIP3_H */
