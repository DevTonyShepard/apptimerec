#include "apptimerec.h"

int getProcIndex(struct apptimerec * list, char * procname,int n){
    for(int i = 0; i < n; i++){
        if(strcmp(list[i].name, procname) == 0)
            return i;
    }
    return NEGATIVE;
}

int exists(struct apptimerec * list, char * procname, int n){
    return getProcIndex(list, procname, n) >= 0;
}

int isNewSession(struct apptimerec * list, char * procname, long starttime, int n, int * index){
    *index = getProcIndex(list,procname,n);
    if(list[*index].starttime < starttime)
        return TRUE;
    return FALSE;
}
