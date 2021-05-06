/**
 * AppTimeRec
 * @copyright Copyright (c) 2021 DevTonyShepard
 * @author Antonio "Shepard"
 * GNU General Public License v3.0
 * @brief Daemon written in C to track process time day by day
 */
#ifndef APPTIMEREC_H_
#define APPTIMEREC_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <asm-generic/param.h>
#include <pwd.h>
#include <ctype.h>
#include "utils.h"

#define TRUE 1
#define FALSE 0
#define NEGATIVE -1
#define SSS S_IRWXU | S_IRWXG | S_IRWXO

struct apptimerec{
    // int pid;
    char name[512];
    int hours;
    int mins;
    int sec;
    long starttime;
};

/**
 * obtain the process index in the list
 * @param list the list of processes
 * @param procname the process to get the index
 * @param n number of processes in the list
 * @return the index if exists otherwise NEGATIVE;
 */
int getProcIndex(struct apptimerec * list, char * procname,int n);

/**
 * checks if process exist in the list
 * @param list the list of processes
 * @param procname the process to check
 * @param n how many process in the list
 * @return the index of the process in the list if exists, otherwise NEGATIVE
 */
int exists(struct apptimerec * list, char * procname, int n);
/**
 * checks if "procname" closed and reopened
 * @param list the process list
 * @param procname process name executable to check
 * @param starttime when last started
 * @param n how long is list
 * @param index outputs the process index in the list, can be NULL
 * @return true or false
 */
int isNewSession(struct apptimerec * list, char * procname, long starttime, int n, int * index);

#endif
