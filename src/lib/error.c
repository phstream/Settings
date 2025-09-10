/*****************************************************************************
 * \\  __   
 * \ \(o >  
 * \/ ) |   
 *  // /    
 *   || CROW - Communicatio Retis Omni Via
 * 
 * @file      error.c
 * @author    phstream
 * @copyright 2025, phstream
 * @license   MIT
 * @date      4 Jul 2025
 ****************************************************************************/
#include "error.h"

const char*
get_error_string(int err)
{
    if (err >= 0) return "No Error"; /* Success or valid data */
    if (err <= -ERRNO_OFFSET) return strerror(-err - ERRNO_OFFSET); /* errno error */
    switch (err) { /* Local defined errors */
        case RET_ERR:      return "Error Occurred";
        case RET_NULL:     return "NULL Pointer Error";
        case RET_VAL:      return "Bad Value";
        case RET_EOF:      return "End of File";
        case RET_BUF:      return "Buffer Full";
        case RET_FMT:      return "Format Error";
        case RET_EXIST:    return "File Exist";
        case RET_NO_EXIST: return "File Not Exist";
        case RET_SIZE:     return "Bad Size";
        case RET_BIG:      return "Too Big";
        case RET_AGAIN:    return "Try Again";
        case RET_SMALL:    return "Too Small";
        default:       return "Undefined Error";}
}
