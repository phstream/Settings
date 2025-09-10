/*****************************************************************************
 * @file      ini.c
 * @author    Peter Hillerström <prohstream@gmail.com>
 * @copyright 2025, Peter Hillerström
 * @license   MIT
 * @date      7 jun 2025
 ****************************************************************************/
#include "ini.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#define INI_VERSION_MAJOR (01)
#define INI_VERSION_MINOR (00)
#define INI_VERSION_BUILD (0000)

#define MAX_LINE_LENGTH (256)
#define INI_TMP_NAME_LEN (256)

#define TOLOWER(x) ((x) >= 'A' && (x) <= 'Z' ? (x) + ('a' - 'A') : (x))
#define ISSPACE(x) ((x) == ' ' || (x) == '\t')
#define ISCOMMENT(x) ((x) == ';' || (x) == '#')

#define RET_OK       (0)
#define RET_ERR      (-1)
#define RET_NULL     (-2)
#define RET_VAL      (-3)
#define RET_EOF      (-4)
#define RET_BUF      (-5)
#define RET_FMT      (-6)
#define RET_TMPEXIST (-7)
#define ERRNO_OFFSET (1000)
#define RET_ERRNO (-(errno + ERRNO_OFFSET))
#define RET_ERRVAL(x) (-(x + ERRNO_OFFSET))

LIB_EXPORT int
ini_version() 
{
    return INI_VERSION_MAJOR * 1000000
         + INI_VERSION_MINOR * 10000
         + INI_VERSION_BUILD;
}

LIB_EXPORT const char*
ini_error_string(int err) 
{
    if (err >= 0) return "No Error"; /* Success or valid data */
    if (err <= -ERRNO_OFFSET) return strerror(-err - ERRNO_OFFSET); /* errno error */
    switch (err) { /* Local defined errors */
        case RET_ERR:      return "An Error Occurred";
        case RET_NULL:     return "NULL Pointer Error";
        case RET_VAL:      return "Bad Value";
        case RET_EOF:      return "End of File";
        case RET_BUF:      return "Buffer Full";
        case RET_FMT:      return "Format Error";
        case RET_TMPEXIST: return "Temp File Exist";
        default:       return "Unknown Error";}
}

static int
ini_write_header(FILE* file)
{
    /* Write header */
    if (fprintf(file, "# _ _|  \\  |_ _| INI-File Parser Version %d.%d.%d\n", INI_VERSION_MAJOR, INI_VERSION_MINOR, INI_VERSION_BUILD) < 0) return RET_ERRNO; 
    if (fprintf(file, "#   |    \\ |  |  Author: Peter Hillerström 2025, License: MIT\n") < 0) return RET_ERRNO; 
    if (fprintf(file, "#   |  |\\  |  |  This is an auto generated configuration ini-file.\n") < 0) return RET_ERRNO; 
    if (fprintf(file, "# ___|_| \\_|___| Remove or change this comment block as you wish.\n") < 0) return RET_ERRNO; 
    if (fprintf(file, "# Use a text editor to change values. Comments start with ';' or '#'.\n") < 0) return RET_ERRNO; 
    if (fprintf(file, "# Inline comments after values are allowed.\n") < 0) return RET_ERRNO; 
    if (fprintf(file, "# Values after '=' are treated as strings and trimmed from whitespace.\n") < 0) return RET_ERRNO; 
    if (fprintf(file, "# For example: key = \"A value\" is the same as key = A value\n") < 0) return RET_ERRNO; 
    if (fprintf(file, "# Inside quotes (\") you may use escape sequences: \\\\ \\\" \\n \\r \\t.\n") < 0) return RET_ERRNO; 
    if (fprintf(file, "# Section and key names are case-insensitive. Arrays are not supported.\n") < 0) return RET_ERRNO; 
    if (fprintf(file, "# https://en.wikipedia.org/wiki/INI_file\n") < 0) return RET_ERRNO; 
    if (fprintf(file, "\n") < 0) return RET_ERRNO; 
    return RET_OK;
}

static int
ini_temp_file(char* p_filename,
              size_t filename_size)
{
    if (!p_filename) return RET_NULL; // Null pointer

#if defined(_WIN32) || defined(_WIN64)
    /* Get Temp Path */
    char p_dir[MAX_PATH];
    if (!GetTempPathA(sizeof(p_dir), p_dir)) return RET_ERR;

    /* Get PID */
    unsigned int pid = (unsigned int)GetCurrentProcessId();
#else  
    /* Get Temp Path */
    const char* p_dir = "/tmp/";

    /* Get PID */
    unsigned int pid = (unsigned int)getpid();
#endif

    /* Generate a random number */
    srand((unsigned)time(NULL) ^ pid);
    unsigned int rnd_val = (unsigned int)rand();

    /* Generate path + filename */
    int result = snprintf(p_filename, filename_size, "%sini-%u.%08X.tmp", p_dir, pid, rnd_val);
    if (result < 0) return RET_FMT;
    if ((size_t)result >= filename_size) return RET_BUF;

    /* Check if file exist (Not good) */
#if defined(_WIN32) || defined(_WIN64)
    if (_access(p_filename, 0) == 0) return RET_TMPEXIST;
#else  
    if (access(p_filename, F_OK) == 0) return RET_TMPEXIST;
#endif
    return 0;
}   

static int 
ini_replace_file(const char *temp_name,
                 const char *filename) 
{
#if defined(_WIN32) || defined(_WIN64)
    if (!MoveFileExA(temp_name, filename, MOVEFILE_REPLACE_EXISTING))
        return RET_ERRNO;
#else
    if (rename(temp_name, filename) != 0)
        return RET_ERRNO;
#endif
    return RET_OK;
}

static int
ini_writeln(FILE* file, 
            const char* p_buf)
{
    if (!file || !p_buf) {
        return RET_NULL; /* Invalid parameters */
    }

    int len = fprintf(file, "%s\n", p_buf);
    if (len < 0) return RET_ERRNO; /* Write error */

    return len; 
}

static int
ini_writef(FILE* file,
          const char *fmt,
          ...) 
{
    /* Print buffer */
    char buffer[MAX_LINE_LENGTH];
    
    /* Print the formatted string */
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buffer, MAX_LINE_LENGTH - 1, fmt, args);
    int errval = errno;
    va_end(args);

    if (len < 0) return RET_ERRVAL(errval); /* Write error */
    
    /* Null terminate for secure string */
    buffer[MAX_LINE_LENGTH - 1] = '\0';

    return ini_writeln(file, buffer);
}

static int
ini_readln(FILE* file,
           char* p_buf,
           size_t buf_len)
{
    if (!file || !p_buf || buf_len == 0) {
        return RET_NULL; /* Invalid parameters */
    }

    /* Read Line */
    if (fgets(p_buf, buf_len, file) == NULL) {
        int errval = errno;
        if (feof(file)) return RET_EOF;
        return RET_ERRVAL(errval);
    }

    /* Remove trailing newlines character if present */
    size_t len = strcspn(p_buf, "\r\n");
    p_buf[len] = '\0';

    return (int) len; // Return the length of the line read
}


static int
ini_parse_value(const char* p_src,
                char* p_dest,
                size_t dest_len)
{
    if (!p_src || !p_dest || dest_len == 0) return 0;

    size_t i_src = 0, i_dest = 0;

    // Skip leading whitespace
    while (ISSPACE(p_src[i_src])) ++i_src;

    /* Start parsing value string */
    if (p_src[i_src] == '"') { // Quoted string
        int escape_flag = 0;
        
        /* Scan value */
        for (i_src += 1; p_src[i_src] /* EoS */ && i_dest < dest_len - 1; ++i_src) 
        {
            /* Check for escape sequencies */
            if (escape_flag) { // If we are in escape sequence 
                escape_flag = 0; // Reset escape flag
                switch (p_src[i_src]) {
                    case 'n': p_dest[i_dest++] = '\n'; break;
                    case 't': p_dest[i_dest++] = '\t'; break;
                    case 'r': p_dest[i_dest++] = '\r'; break;
                    case '\\': p_dest[i_dest++] = '\\'; break;
                    case '"': p_dest[i_dest++] = '"'; break;
                    // Add more escape sequences as needed
                    default: 
                        p_dest[i_dest++] = '\\'; /* Unknown escape seq add the '\' to output */ 
                        if (i_dest < dest_len) p_dest[i_dest++] = p_src[i_src]; 
                        break;
                }
            }
            else {
                /* Check for escape sequencies */
                if (p_src[i_src] == '\\') {escape_flag = 1; continue; } // Set escape flag
                
                /* Check for end quote */
                else if (p_src[i_src] == '"') break;
                
                /* Copy string value to destination */
                else p_dest[i_dest++] = p_src[i_src];
            }
        }
    } else { /* Unquoted string */
        /* White space handling */
        int whitespace_flag = 0;
        size_t i_start = i_src; /* Just for initialization */
        
        /* Scan and copy value until comment or EOL */
        for (; p_src[i_src] && !ISCOMMENT(p_src[i_src]) && i_dest < dest_len - 1; ++i_src) {
            if (ISSPACE(p_src[i_src])) {
                if (!whitespace_flag) { 
                    i_start = i_src; // Remember the start of the whitespace
                    whitespace_flag = 1; // Set whitespace flag
                }
            }
            else { /* No white space */
                if (whitespace_flag) { 
                    /* Copy whitespaces */
                    while (i_start < i_src && i_dest < dest_len - 1) p_dest[i_dest++] = p_src[i_start++]; 
                    whitespace_flag = 0;
                }
                /* No whitespace, copy value */
                p_dest[i_dest++] = p_src[i_src]; 
            }
        }
        /* Terminate output result */
        p_dest[i_dest] = '\0';
    }

    return (int)i_dest;
}

static int
ini_write_value(FILE* file,
                const char* p_key,
                const char* p_value,
                const char* p_comment)
{
    if (p_comment) {
        int result = ini_writef(file, "\n# %s", p_comment);
        if (result < 0) return result;
    }
    return ini_writef(file, "%s = %s", p_key, p_value);
}

static int
ini_scan_for_section(FILE* in_file,
                     char* p_buf,
                     const char* p_section)
{ 
    if (!in_file) return RET_VAL; /* No input file */

    /* Read first line */
    int len = ini_readln(in_file, p_buf, MAX_LINE_LENGTH);
    if (len < 0) return len;

    /* Scan & parse line */
    int i_buf = 0;
    int i_section = 0;

    /* Skip leading whitespace */
    while (len - i_buf > 0 && ISSPACE(p_buf[i_buf])) ++i_buf; 
    if (len - i_buf == 0) return 0;

    /* Check for section */
    if (p_buf[i_buf++] != '[') return 0; /* Not a section header */

    /* Scan for section name */
    for (; i_buf < len; ++i_buf, ++i_section) {
            
        if (p_section[i_section] == '\0') { /* End of Section name */
            if (p_buf[i_buf] == ']') return i_buf+1; /* Return the index after the ']' character */
            else return 0; /* Return not found */
        }
        else if (TOLOWER(p_buf[i_buf]) != TOLOWER(p_section[i_section])) return 0; /* Return not found */
    }

    return 0; /* Section not found */
}

static int
ini_scan_for_key(FILE* in_file,
                 char* p_buf,
                 const char* p_key)
{
    if (!in_file) return RET_VAL; /* No input file */

    /* Read first line */
    int len = ini_readln(in_file, p_buf, MAX_LINE_LENGTH);
    if (len < 0) return len;

    /* Scan & parse line */
    int i_buf = 0;
    int i_key = 0;

    /* Skip leading whitespace */
    while (len - i_buf > 0 && ISSPACE(p_buf[i_buf])) ++i_buf; 
    if (len - i_buf == 0) return 0;
    
    /* Search key name */
    for (; i_buf < len; ++i_buf, ++i_key) {
        if (p_key[i_key] == '\0') break; // We found the key
        if (TOLOWER(p_buf[i_buf]) != TOLOWER(p_key[i_key])) return 0; /* Key not found */
    }
    /* Search '=' or ':' */
    for (; i_buf < len; ++i_buf) {
        if (p_buf[i_buf] == '=' || p_buf[i_buf] == ':') return i_buf+1; /* Return the index after the '=' character */
        if (!ISSPACE(p_buf[i_buf])) return 0; /* Bad syntax? Key not found */
    }
    return 0; /* Key not found */
}

LIB_EXPORT int
ini_get_section(const char* filename,
                const char* p_section,
                char* p_key[], size_t size_keys,
                char* p_value[], size_t size_value,
                char* buffer[], size_t size_buffer)
{
    int key_idx = 0;
    int result;

    /* Pointer check */
    if (!filename || !p_section || !p_key || !p_value || !buffer ) return RET_NULL;
    p_value[0] = '\0'; /* Empty string for not found */

    /* Open file for reading */
    FILE* file = fopen(filename, "r");
    if (!file) return RET_ERRNO; /* File not found or could not be opened */

    /* Read and scan for Section */
    do { result = ini_scan_for_section(file, buffer, p_section); } while (result == 0);
    if (result < 0) { fclose(file); return result; }

        /* Read first line */
    int len = ini_readln(in_file, p_buf, MAX_LINE_LENGTH);
    if (len < 0) return len;

}
LIB_EXPORT int
ini_read_key(const char* filename,
             const char* p_section,
             const char* p_key,
             char* p_value,
             size_t value_size) 
{
    char buffer[MAX_LINE_LENGTH];
    int result = 0;
    
    /* Input parameter check */
    if (!filename || !p_section || !p_key || !p_value || value_size == 0) return -1; /* Invalid parameters */
    p_value[0] = '\0'; /* Empty string for not found */

    /* Open file for reading */
    FILE* file = fopen(filename, "r");
    if (!file) return RET_ERRNO; /* File not found or could not be opened */

    /* Read and scan for Section */
    do { result = ini_scan_for_section(file, buffer, p_section); } while (result == 0);
    if (result < 0) { fclose(file); return result; }
    
    /* Read and Scan for Key */
    do { result = ini_scan_for_key(file, buffer, p_key); } while (result == 0);
    if (result < 0) { fclose(file); return result; }
    fclose(file);

    /* Advance past whitespace to value start */
    size_t i = result;
    while (ISSPACE(buffer[i])) { ++i; } 
                
    /* Parse rest as value and return length of value */
    return ini_parse_value(buffer + i, p_value, value_size);
}

LIB_EXPORT int
ini_write_key(const char* filename, 
              const char* p_section, 
              const char* p_key, 
              const char* p_value, 
              const char* p_comment)
{
    char buffer[MAX_LINE_LENGTH], temp_name[INI_TMP_NAME_LEN];
    int result = 0, temp_created = 0;
    FILE* file_out = NULL;
    FILE* file_in = NULL;

    /* Check input pointers */
    if (!filename || !p_section || !p_key || !p_value) return RET_NULL;

    /* Generate Temporary Filename */
    result = ini_temp_file(temp_name, sizeof(temp_name));
    if (result < 0) return result;

    /* Open temporary file for writing */
    file_out = fopen(temp_name, "w");
    if (!file_out) return RET_ERRNO;
    temp_created = 1;
    
    /* Open actual ini file for reading */
    file_in = fopen(filename, "r");
    if (!file_in) { /* INI-file read error */
        if (errno != ENOENT) { result = RET_ERRNO; goto cleanup; }
        
        /* Create new file with section/key */
        if ((result = ini_write_header(file_out)) < 0) goto cleanup;
        if ((result = ini_writef(file_out, "[%s]", p_section)) < 0) goto cleanup;
        if ((result = ini_write_value(file_out, p_key, p_value, p_comment)) < 0) goto cleanup;
    } 
    
    else { /* INI file found, scan and update */
 
        /* Read and scan for Section */
        while ((result = ini_scan_for_section(file_in, buffer, p_section)) == 0)
        {
            /* Copy line to output */
            if ((result = ini_writeln(file_out, buffer)) < 0) goto cleanup;
        }

        /* Check scan section result */
        if (result == RET_EOF) { /* No section found */
            fclose(file_in); file_in = NULL;
            
            /* Create new section/key */
            result = ini_writef(file_out, "\n[%s]", p_section);
            if (result >= 0) result = ini_write_value(file_out, p_key, p_value, p_comment);
            if (result < 0) goto cleanup; 
        }

        else if (result < 0) goto cleanup; /* Scan section error */

        else { /* Section found */
            /* Get the section line copied too */
            if ((result = ini_writeln(file_out, buffer)) < 0) goto cleanup;

            /* Read and Scan for Key */
            while ((result = ini_scan_for_key(file_in, buffer, p_key)) == 0) { // TODO Doesn't stop for [section]
                /* Copy line to output */
                if ((result = ini_writeln(file_out, buffer)) < 0) goto cleanup;  
            }

            /* Check scan results */
            if (result == RET_EOF) { /* No key found */
                fclose(file_in); file_in = NULL;
                
                /* Create new key */
                result = ini_write_value(file_out, p_key, p_value, p_comment);
                if (result < 0) goto cleanup;
            }

            else if (result < 0) goto cleanup;  /* Scan key error */ 

            else { /* Key found */
                if ((result = ini_write_value(file_out, p_key, p_value, NULL)) < 0) goto cleanup;
                
                /* Copy rest of file */
                while ((result = ini_readln(file_in, buffer, MAX_LINE_LENGTH)) >= 0) {
                    /* Copy line to output */
                    result = ini_writeln(file_out, buffer); 
                    if (result < 0) goto cleanup;
                }
                /* Check copy results */
                if (result != RET_EOF) goto cleanup;
            }        
        }
    }
    if (file_in) fclose(file_in);
    if (file_out) fclose(file_out);

    return ini_replace_file(temp_name, filename);

cleanup:
    if (file_in) fclose(file_in);
    if (file_out) fclose(file_out);
    if (temp_created) remove(temp_name);
    return result;
}

