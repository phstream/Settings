/*****************************************************************************
 * @file      ini.h
 * @author    Peter Hillerström <prohstream@gmail.com>
 * @copyright 2025, Peter Hillerström
 * @license   MIT
 * @date      7 jun 2025
 ****************************************************************************/
/**
 * @brief     INI-Reader/Writer
 * @details
 *
 * @pre       
 * @bug       
 * @warning
 * @ingroup   Settings
 ****************************************************************************/
#ifndef INI_H_
#define INI_H_

#ifdef __cplusplus    /* C++ compability */
#  include <cstdint>
#  include <cstddef>
extern "C" {
#else
#  include <stdint.h>
#  include <stddef.h>
#endif

/*
 * Define EXT_API for shared/static library builds.
 * Use -DBUILD_AS_LIB when building a shared library.
 */
#ifndef EXT_API
  #ifdef BUILD_LIB
    #if defined(_WIN32) || defined(_WIN64)
      #define EXT_API __declspec(dllexport)
    #elif defined(__GNUC__) && __GNUC__ >= 4
      #define EXT_API __attribute__((visibility("default")))
    #else
      #error "EXT_API: Unknown platform or compiler. Please define EXT_API manually."
    #endif
  #else
    #define EXT_API
  #endif
#endif

EXT_API int
ini_version();

EXT_API const char*
ini_error_string(int err);

EXT_API int
ini_read_key(const char* filename,
             const char* p_section,
             const char* p_key,
             char* p_value,
             size_t value_size);

EXT_API int
ini_write_key(const char* filename, 
              const char* p_section, 
              const char* p_key, 
              const char* p_value, 
              const char* p_comment);


#ifdef __cplusplus
}
#endif

#endif /* INI_H_ */