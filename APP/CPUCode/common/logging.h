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
 * \brief Interface to logging infrastructure.
 */

#ifndef LOGGING_H_
#define LOGGING_H_

/*============================================================================*
 * Includes
 *============================================================================*/
#include <stdio.h>
#include <stdlib.h>

/*============================================================================*
 * Public defines
 *============================================================================*/
#define LOG_PLACE LOG_FILE

#define LOG_BASE_(p,t,...)  { logging_log_msg(p, t, __FILE__, __LINE__, __VA_ARGS__); }
#define LOG_FATAL(...)      { LOG_BASE_(LOG_PLACE | LOG_STDERR, "FATAL",    __VA_ARGS__); exit(1); }
#define LOG_ERROR(...)      { LOG_BASE_(LOG_PLACE | LOG_STDERR, "ERROR",    __VA_ARGS__); }
#define LOG_WARNING(...)    { LOG_BASE_(LOG_PLACE | LOG_STDOUT, "WARNING",  __VA_ARGS__); }
#define LOG_INFO(...)       { LOG_BASE_(LOG_PLACE,              "INFO",     __VA_ARGS__); }
#ifdef LOG_DEBUG_ENABLED
#define LOG_DEBUG(...)      { LOG_BASE_(LOG_PLACE,              "DEBUG",    __VA_ARGS__); }
#else
#define LOG_DEBUG(...)
#endif

#define LOG_VAR_(t,r,f,...) LOG_##t("%30s = " f, #r, __VA_ARGS__)
#define LOG_FLOAT(t,r)      LOG_VAR_(t, r, "%8.6f", (float)(r))
#define LOG_INT(t,r)        LOG_VAR_(t, r, "%4ld",  (long)(r))
#define LOG_PTR(t,r)        LOG_VAR_(t, r, "%p",    (r))
#define LOG_BOOL(t,r)       LOG_VAR_(t, r, "%s",    (r) ? "true" : "false")
#define LOG_DEBUG_ENTER()   LOG_DEBUG("------ %s ------", __func__)

/*============================================================================*
 * Public types
 *============================================================================*/
typedef enum logging_target
{
    LOG_NONE   = 0,
    LOG_STDOUT = 1,
    LOG_STDERR = 2,
    LOG_FILE   = 4
} logging_target;

/*============================================================================*
 * Public function declarations
 *============================================================================*/
void logging_log_msg(logging_target target, const char *type, const char *filename, const int linenum, const char *fmt, ...);
void logging_set_file(FILE *file);

/*============================================================================*
 * Trailing boilerplate
 *============================================================================*/

#endif /* include guard */
