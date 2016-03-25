/*
 * =====================================================================================
 *
 *       log_filename:  debug.h
 *
 *    Description:  debug utilities based on http://c.learncodethehardway.org/book/ex20.html
 *
 *        Version:  1.0
 *        Created:  01/19/2016 03:33:25 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */


#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

extern char* log_filename;

#define DEBUGGING_MODE 1
#define log_err(M, ...) if(DEBUGGING_MODE){ FILE* _log = fopen(log_filename, "a"); do{ fprintf(_log, "[ERR] %06d : %06d %s | %s | %-15s | " M "\n", getpgid(0), getpid(), __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__); } while(0); fclose(_log);}
#define log_inf(M, ...) if(DEBUGGING_MODE){ FILE* _log = fopen(log_filename, "a"); do{ fprintf(_log, "[INF] %06d : %06d %s | %s | %-15s | " M "\n", getpgid(0), getpid(), __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__); } while(0); fclose(_log);}
#define log_wrn(M, ...) if(DEBUGGING_MODE){ FILE* _log = fopen(log_filename, "a"); do{ fprintf(_log, "[WRN] %06d : %06d %s | %s | %-15s | " M "\n", getpgid(0), getpid(), __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__); } while(0); fclose(_log);}
#define log_dbg(M, ...) if(DEBUGGING_MODE){ FILE* _log = fopen(log_filename, "a"); do{ fprintf(_log, "[DBG] %06d : %06d %s | %s | %-15s | " M "\n", getpgid(0), getpid(), __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__); } while(0); fclose(_log);}
//#define log_wrn(M, ...) if(DEBUGGING_MODE){ int _log = open(log_filename, O_WRONLY | O_APPEND | O_CREAT); do { fprintf(stdout, "[WRN] %s | %s | %-15s | " M "\n", __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__);} while(0)}
//#define log_inf(M, ...) if(DEBUGGING_MODE){ int _log = open(log_filename, O_WRONLY | O_APPEND | O_CREAT); do { fprintf(stdout, "[INF] %s | %s | %-15s | " M "\n", __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__);} while(0)}
//#define log_dbg(M, ...) if(DEBUGGING_MODE){ int _log = open(log_filename, O_WRONLY | O_APPEND | O_CREAT); do { fprintf(stdout, "[DBG] %s | %s | %-15s | " M "\n", __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__);} while(0)}


#endif
