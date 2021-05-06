#include "utils.h"


void CTRLSYS(int x,char * s,int DO_OR_DIE,int isdaemon){
    if ((x) < 0) {
        if(isdaemon == TRUE)
            syslog(LOG_ERR, "%s\n",s);
        else
            perror(s);
        if(DO_OR_DIE) exit(errno);
    }
}

void remove_all_chars(char* str, char c) {
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

int strlng(char * str){
    char * ch;
    int out;
    for(ch = str, out=0;*ch != '\0';ch++,out++);
    return out;
}

int chpos(char * str, char target,int inst){
    char * ch;
    int out,founds=0;
    int strln = strlng(str);
    for(ch = str, out=0;founds < (inst+1) ;ch++,out++)
        if(*ch == target)
            founds++;
        else if(out >= (strln+1))
            return -1;
    return out;
}

char ** strspl(char * str,char dlm,int * n_sstr_o){
    char ** out;
    char * ch;
    int i,y,n_sstr,pos,prev_p;
    //int strln=strlng(str);

    for(i=0;chpos(str,dlm,i) > 0;i++);
    n_sstr = ++i;
    if(n_sstr_o != NULL) *n_sstr_o = n_sstr;
    int * ss_len = malloc((sizeof(int)*n_sstr));

    for(i=0;(pos=chpos(str,dlm,i))>0;i++)
        ss_len[i] = (pos-1)- ((prev_p = chpos(str,dlm,i-1)) < 0 ? 0 : prev_p);

    out = (char ** ) malloc(sizeof(char*) * n_sstr);


    for(ch = str,i = 0; i < n_sstr;i++){
        out[i] = (char *) malloc(sizeof(char)*ss_len[i]+1);
        for(y=0; *ch != dlm && *ch != '\0';ch++,y++)
            out[i][y] = *ch;
        out[i][y] = '\0';
        ch++;
    }
    return out;
}

int isNumber(char * string){
    for(int i = 0; i < strlng(string);i++)
        if(string[i] < '0' || string[i] > '9')
            return FALSE;
    return TRUE;
}


int belongsToList(char ** list, char * string, int n){
    for(int i = 0; i < n; i++)
        if(strcmp(list[i], string) == 0) {
            return TRUE;
        }
    return FALSE;

}

void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    /*
     * Clear file creation mask.
     */
    umask(0);

    /*
     * Get maximum number of file descriptors.
     */
    CTRLSYS(getrlimit(RLIMIT_NOFILE, &rl),"can't get the file limit",TRUE,0);

    /*
     * Become a session leader to lose controlling TTY.
     */
    CTRLSYS(pid = fork(),"can't fork",TRUE,0);

    if (pid != 0) /* parent */
        exit(0);
    setsid();

    /*
     * Ensure future opens won't allocate controlling TTYs.
     */
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    CTRLSYS(sigaction(SIGHUP, &sa, NULL),"can't ignore SIGHUP",TRUE,0);
    CTRLSYS(pid = fork(),"can't fork",TRUE,0);

    if (pid != 0) /* parent */
        exit(0);

    /*
     * Change the current working directory to the root so
     * we won't prevent file systems from being unmounted.
     */
    CTRLSYS(chdir("/"),"can't change directory to /",TRUE,0);

    /*
     * Close all open file descriptors.
     */
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);

    /*
     * Attach file descriptors 0, 1, and 2 to /dev/null.
     */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    /*
     * Initialize the log file.
     */
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
               fd0, fd1, fd2);
        exit(1);
    }
}
