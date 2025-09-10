/*****************************************************************************
 * \\  __   
 * \ \(o >  
 * \/ ) |   
 *  // /    
 *   || CROW - Communicatio Retis Omni Via
 *
 * @file      lib_defs.h
 * @author    phstream
 * @copyright 2025, phstream
 * @license   MIT
 * @date      4 Jul 2025
 ****************************************************************************/
/** @defgroup  CROW CROW Build Helpers
 * @brief     SO/DLL Library Build Definitions
 * @details   Define BUILD_LIB or USING_LIB before including 
 *            to control export visibility.
 *
 * @pre
 * @bug       
 * @warning
 ****************************************************************************/
#ifndef LIB_DEFINITIONS_H
#define LIB_DEFINITIONS_H

#ifdef LIB_EXPORT
  #error "LIB_EXPORT already defined — possible conflict or double include"
#endif

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

#endif /* LIB_DEFINITIONS_H */
