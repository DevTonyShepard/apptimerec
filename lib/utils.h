#ifndef UTILS_H_
#define UTILS_H_
#include "apptimerec.h"

void CTRLSYS(int x,char * s,int DO_OR_DIE,int isdaemon);
void remove_all_chars(char* str, char c);
int strlng(char * str);
int chpos(char * str, char target,int inst);
char ** strspl(char * str,char dlm,int * n_sstr_o);
int isNumber(char * string);
void daemonize(const char *cmd);
int belongsToList(char ** list, char * string, int n);

#endif
