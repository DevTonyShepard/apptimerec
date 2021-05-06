#ifndef UTILS_H_
#define UTILS_H_
#include "apptimerec.h"

/**
 *
 * @param x
 * @param s
 * @param DO_OR_DIE
 * @param isdaemon
 */
void CTRLSYS(int x,char * s,int DO_OR_DIE,int isdaemon);
/**
 *
 * @param str
 * @param c
 */
void remove_all_chars(char* str, char c);
/**
 *
 * @param str
 * @return
 */
int strlng(char * str);
/**
 *
 * @param str
 * @param target
 * @param inst
 * @return
 */
int chpos(char * str, char target,int inst);
/**
 * splits a string into substrings based on the delimiter
 * @param str the whole string
 * @param dlm delimier
 * @param n_sstr_o how many substing if initually null, it will not r
 * @return the splitted sting
 */
char ** strspl(char * str,char dlm,int * n_sstr_o);
/**
 * checks if a string is a number
 * @param string
 * @return true or false
 */
int isNumber(char * string);
/**
 * deamonize a process. 
 * @param cmd the process executable name (pass argv[0])
 */
void daemonize(const char *cmd);
/**
 *
 * @param list
 * @param string
 * @param n
 * @return
 */
int belongsToList(char ** list, char * string, int n);

#endif
