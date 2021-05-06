/**
 * AppTimeRec
 * @copyright Copyright (c) 2021 DevTonyShepard
 * @author Antonio "Shepard"
 * GNU General Public License v3.0
 * @brief Daemon written in C to track process time day by day
 */
#include "lib/apptimerec.h"
#include "lib/utils.h"

int main(int argc, char *argv[]) {

    int c;
    char ** listaddproc;
    int howmanyproc;
    unsigned int uid;
    int sample = 1;
    int fd, dfd,fdtimes,fdboot;;
    char filenamePath[1000];
    char buf[1024];
    //get time of day
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    //manage options
    while ((c = getopt (argc, argv, "pa:u:s:")) != -1)
        switch (c)
        {
            case 'p':
                sprintf(filenamePath,"/etc/apptimerec/times_%02d_%02d_%02d",local->tm_mday,local->tm_mon+1,local->tm_year+1900);
                CTRLSYS(fdtimes = open(filenamePath,O_RDONLY),"error opening today file",TRUE,TRUE);
                while ((read(fdtimes, buf, 1024)) > 0)
                    printf("%s",buf);
                return 0;
            case 'a':
                CTRLSYS(creat("/etc/apptimerec/trackproc",SSS),"failed to create trackprog file",TRUE,TRUE);
                CTRLSYS((fd = open("/etc/apptimerec/trackproc",O_WRONLY)),"failed to open trackprog file",TRUE,TRUE);
                listaddproc = strspl(optarg,  ' ' , &howmanyproc);
                for(int i = 0; i < howmanyproc;i++) {
                    CTRLSYS(write(fd, listaddproc[i], strlng(listaddproc[i])),"failed to write to trackprog file",TRUE,TRUE);
                    CTRLSYS(write(fd," ",1),"failed to write to trackprog file",TRUE,TRUE);
                }
                break;
            case 'u':
                CTRLSYS(creat("/etc/apptimerec/users",SSS),"failed to create users file",TRUE,TRUE);
                CTRLSYS((fd = open("/etc/apptimerec/users",O_WRONLY)),"failed to open users file",TRUE,TRUE);
                CTRLSYS(write(fd,optarg, strlng(optarg)),"failed to write to users file",TRUE,TRUE);
               // uid = getpwnam(optarg)->pw_uid;
                break;
            case 's':
                CTRLSYS(creat("/etc/apptimerec/sample",SSS),"failed to create sample rate file",TRUE,TRUE);
                CTRLSYS((fd = open("/etc/apptimerec/sample",O_WRONLY)),"failed to open sample rate file" ,TRUE,TRUE);
                CTRLSYS(write(fd,optarg, strlng(optarg)),"failed to write to sample rate file",TRUE,TRUE);
                sample = atoi(optarg);
                break;
            case '?':
                if (optopt == 'a')
                   fprintf (stderr, "Option -%d requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%d'.\n", optopt);
                else
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
                return 1;
            default:
                abort ();
        }
    DIR * datadir;
    struct dirent *dataDirEntry;
    struct apptimerec record[1000];
    unsigned long uptime;
    static time_t boottime;
    char *p;
    FILE *fp;
    int nread;
    char trackprog[2048];
    char users[2048];
    char samples[64];
    users[0] = '\0';
    trackprog[0] = '\0';
    //start daemon
    daemonize(argv[0]);

    //check for first time use
    if((datadir = opendir("/etc/apptimerec")) == NULL){
        printf("error opening conf folder, creating one in /etc/apptimerec\n");
        CTRLSYS(mkdir("/etc/apptimerec",0777),"error creating daemon folder",TRUE,TRUE);
    }

    //start today file
    sprintf(filenamePath,"/etc/apptimerec/times_%02d_%02d_%02d",local->tm_mday,local->tm_mon+1,local->tm_year+1900);
    CTRLSYS(creat(filenamePath,SSS),"failed to create today file, make sure to run with sudo",TRUE,TRUE);
    CTRLSYS(fdtimes = open(filenamePath,O_WRONLY),"failed to open today file,  make sure to run with sudo",TRUE,TRUE);

    //get configuration
    //get processes to track
    CTRLSYS((fd = open("/etc/apptimerec/trackproc",O_RDONLY)),"failed to open trackproc while getting the configuration",TRUE,TRUE);
    while ((nread = read(fd, buf, 2048)) > 0) {
        strcpy(trackprog, buf);
    }
    trackprog[strlng(trackprog)] = '\0';
    listaddproc = strspl(trackprog,  ' ' , &howmanyproc);
    //get users
    buf[0] = '\0';
    CTRLSYS((fd = open("/etc/apptimerec/users",O_RDONLY)),"failed to open users while getting the configuration",TRUE,TRUE);
    while ((nread = read(fd, users, 2048)) > 0) {
        uid = getpwnam(users)->pw_uid;
    }
    //get sample time
    CTRLSYS((fd = open("/etc/apptimerec/sample",O_RDONLY)),"failed to open trackproc while getting the configuration",TRUE,TRUE);
    while ((nread = read(fd, samples, 2048)) > 0) {
        sample = atoi(samples);
    }
    DIR * dir;
    struct dirent *direntry;
    int nproc = 0;

    //main update loop
    while(TRUE) {
        fp = fopen(filenamePath,"w");
        fclose(fp);
        CTRLSYS(fdtimes = open(filenamePath,O_WRONLY),"error opening today file",TRUE,TRUE);
        int nproctmp=0;
        int index;
        struct apptimerec tmp[1000];
        if((dir = opendir("/proc")) == NULL) {
            syslog(LOG_ERR, "Apptimerec: unable to open /proc");
            return -1;
        }
        fd = dirfd(dir);
        while ((direntry = readdir(dir)) != NULL) {
            if ((fdboot = open("/proc/uptime", 0)) != -1) {
                if (read(fdboot, buf, sizeof(buf)) > 0)
                    uptime = strtoul(buf, &p, 10);
                close(fdboot);
            }
            if (isNumber(direntry->d_name)) {
                fchdir(fd);
                DIR *procdir;
                int procfd, statfd, loginuidfd;
                struct dirent *procdirentry;
                char statfilePath[1000];
                char processname[1000];
                char statline[1000];
                char loginuidfilePath[1000];
                char uidread[100];
                char **statlineSplit;
                long starttime;
                ssize_t n;
                int pid;
                int sec, min, hours;
                long totalsec;
                int procuid;

                sprintf(statfilePath, "/proc/%s/stat", direntry->d_name);
                sprintf(loginuidfilePath, "/proc/%s/loginuid", direntry->d_name);
                CTRLSYS((statfd = open(statfilePath, O_RDONLY)), "error opening a stat file", FALSE, TRUE);
                CTRLSYS((int) (n = read(statfd, statline, 1000)), "error while getting process name", FALSE, TRUE);
                CTRLSYS((loginuidfd = open(loginuidfilePath, O_RDONLY)), "error opening a loginuid file", FALSE, TRUE);
                statline[n] = '\0';
                statlineSplit = strspl(statline, ' ', NULL);
                CTRLSYS((int) (n = read(loginuidfd, uidread, 100)), "error while getting process name", FALSE, TRUE);
                uidread[n] = '\0';
                procuid = atoi(uidread);
                if(statlineSplit !=  NULL)
                    strcpy(processname, statlineSplit[1]);
                else {
                    syslog(LOG_ERR,"error reading stat");
                    exit(-1);
                }
                remove_all_chars(processname, '(');
                remove_all_chars(processname, ')');
                if ((strcmp(processname, "") != 0) && (procuid == uid) &&
                    (belongsToList(listaddproc, processname, howmanyproc))) {

                    starttime = atoi(statlineSplit[21]);
                    totalsec = (uptime - (starttime / HZ));
                    hours = (int) totalsec / 3600;
                    min = ((int) totalsec - hours * 3600) / 60;
                    sec = (int) totalsec - hours * 3600 - min * 60;
                    if (!exists(tmp, processname, nproctmp)) {
                        //add process to list
                        strcpy(tmp[nproctmp].name, processname);
                        tmp[nproctmp].hours = hours;
                        tmp[nproctmp].mins = min;
                        tmp[nproctmp].sec = sec;
                        tmp[nproctmp].starttime = starttime;
                        nproctmp++;
                    } else if (!isNewSession(tmp, processname, starttime, nproctmp, &index)) {
                        //get older process with same cmd
                        tmp[index].sec = sec;
                        tmp[index].mins = min;
                        tmp[index].hours = hours;
                        tmp[index].starttime = starttime;
                    } else {
                        //just update
                        tmp[index].hours = hours;
                        tmp[index].mins = min;
                        tmp[index].sec = sec;

                    }
                }
            }
        }
        for(int i = 0; i < nproctmp;i++) {
            if (!exists(record, tmp[i].name, nproc)) {
                //add to list
                strcpy(record[nproc].name, tmp[i].name);
                record[nproc].hours = tmp[i].hours;
                record[nproc].mins = tmp[i].mins;
                record[nproc].sec = tmp[i].sec;
                record[nproc].starttime = tmp[i].starttime;
                nproc++;
            } else if (isNewSession(record, tmp[i].name, tmp[i].starttime, nproc, &index)) {
                //check if closed and reopened
                record[index].sec += tmp[i].sec;
                if(record[index].sec >= 60) {
                    record[index].sec -= 60;
                    record[index].mins++;
                    if(record[index].mins >= 60){
                        record[index].mins -= 60;
                        record[index].hours++;
                    }
                }
                record[index].mins += tmp[i].mins;
                if(record[index].mins >= 60){
                    record[index].mins -= 60;
                    record[index].hours++;
                }
                record[index].hours += tmp[i].hours;
                record[index].starttime = tmp[i].starttime;
            } else {
                //still running, update with sample rate
                    record[index].sec += sample;
                    if(record[index].sec >= 60) {
                        record[index].sec -= 60;
                        record[index].mins++;
                        if(record[index].mins >= 60){
                            record[index].mins -= 60;
                            record[index].hours++;
                        }
                    }

            }
        }
        char proc[1000];
        for(int i = 0; i < nproc;i++){
            sprintf(proc,"%s \t\t\t %d:%d:%d \t\t\t %ld\n", record[i].name,record[i].hours, record[i].mins, record[i].sec,record[i].starttime);
            CTRLSYS((int)(write(fdtimes,proc, strlng(proc))),"error writing on times file",TRUE,TRUE);
        }
        nproctmp = 0;
        sleep(sample);
     }
}
