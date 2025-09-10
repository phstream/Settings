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
 * Define LIB_EXPORT for shared/static library builds.
 * Use -DBUILD_AS_LIB when building a shared library.
 */
#if defined(_WIN32) || defined(_WIN64)
  #if defined(BUILD_LIB)
    #define LIB_EXPORT __declspec(dllexport)
  #elif defined(USING_LIB)
    #define LIB_EXPORT __declspec(dllimport)
  #else
    #define LIB_EXPORT
  #endif
#elif defined(__GNUC__) && __GNUC__ >= 4
  #if defined(BUILD_LIB)
    #define LIB_EXPORT __attribute__((visibility("default")))
  #else
    #define LIB_EXPORT
  #endif
#else
  #define LIB_EXPORT
#endif

LIB_EXPORT int
ini_version();

LIB_EXPORT const char*
ini_error_string(int err);

LIB_EXPORT int
ini_read_key(const char* filename,
             const char* p_section,
             const char* p_key,
             char* p_value,
             size_t value_size);

LIB_EXPORT int
ini_write_key(const char* filename, 
              const char* p_section, 
              const char* p_key, 
              const char* p_value, 
              const char* p_comment);


#ifdef __cplusplus
}
#endif

#endif /* INI_H_ */