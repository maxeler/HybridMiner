/*********************************************************************
 * Maxeler Technologies : Hybrid Coin Miner                          *
 *                                                                   *
 * Version: 1.0                                                      *
 * Date:    13th March 2014                                          *
 *                                                                   *
 * CPU code source file                                              *
 *                                                                   *
 *********************************************************************/

/**
 * \file
* \brief Implementation of logging infrastructure.
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "logging.h"
//#include "common.h"

/*============================================================================*
 * Private defines
 *============================================================================*/
/* None */

/*============================================================================*
 * Private types
 *============================================================================*/
/* None */

/*============================================================================*
 * Public types
 *============================================================================*/
/* None */

/*============================================================================*
 * Private function declarations
 *============================================================================*/
/* None */

/*============================================================================*
 * Private data
 *============================================================================*/
static FILE *logfile = NULL;

/*============================================================================*
 * Public function definitions
 *============================================================================*/

void logging_log_msg(logging_target target, const char *type, const char *filename, const int linenum, const char *fmt, ...)
{
    // Needs to match the order of the log_target enum
    FILE *fids[] = { stdout, stderr, logfile };

    for (int i = 0; i < 3; i++)
    {
        FILE *const fid = fids[i];
        if ((target & (1 << i)) && (fid != NULL))
        {
        	time_t now;
        	time(&now);
        	struct tm* tm_info;
        	tm_info = localtime(&now);
        	char buffer[25];
        	char header[1000];
        	char msg[1000000];
        	strftime(buffer, 25, "%H:%M:%S", tm_info);

        	if(!strcmp("INFO", type)){
        		sprintf(header, "[%s] [%s]: ", type, buffer);
        	}
        	else{
        		sprintf(header, "[%s] [%s] [%s:%d]: ", type, buffer, filename, linenum);
        	}
            va_list args;
            va_start(args, fmt);
            vsprintf(msg, fmt, args);
            va_end(args);
            fprintf(fid, "%s%s\n", header, msg);
            if (fid == stdout) {
                fflush(fid);
            }
        }
    }
}

void logging_set_file(FILE *file) {
    logfile = file;
}


/*============================================================================*
 * Private function definitions
 *============================================================================*/
/* None */
