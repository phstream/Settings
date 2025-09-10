/*****************************************************************************
 * \\  __   
 * \ \(o >  
 * \/ ) |   
 *  // /    
 *   || CROW - Communicatio Retis Omni Via
 * 
 * @file      error.h
 * @author    phstream
 * @copyright 2025, phstream
 * @license   MIT
 * @date      4 Jul 2025
 ****************************************************************************/
/** @defgroup  CROW CROW Build Helpers
 * @brief     Error codes and descriptions
 * @details   Local error code definitions and mapping to strings.
 *            Includes errno handling macros by adding an offset and making
 *            them negative. This allows returning both local and errno
 *            -(errno + ERRNO_OFFSET)
 * @pre
 * @bug       
 * @warning
 ****************************************************************************/
#ifndef ERROR_H_
#define ERROR_H_

#include <errno.h>

/*---- crow error definitions ----------------------------------------------*/
#define RET_OK       (0)
#define RET_ERR      (-1)
#define RET_NULL     (-2)
#define RET_VAL      (-3)
#define RET_EOF      (-4)
#define RET_BUF      (-5)
#define RET_FMT      (-6)
#define RET_EXIST    (-7)
#define RET_NO_EXIST (-8)
#define RET_SIZE     (-9)
#define RET_BIG      (-10)
#define RET_AGAIN    (-11) /* Mapping of EAGAIN */
#define RET_SMALL    (-12)

/*---- errno handling ------------------------------------------------------*/
#define ERRNO_OFFSET (1000)
#define RET_ERRNO (-(errno + ERRNO_OFFSET))
#define RET_ERRVAL(x) (-(x + ERRNO_OFFSET))

/**
 * @brief      Get a human-readable error string for a given error code
 * @param      err  The error code. Always negative for errors.
 * @return     Pointer to a static string briefly describing the error.
 * @details    This function maps error codes to human-readable strings.
 */
const char*
get_error_string(int err);

#endif /* ERROR_H_ */
