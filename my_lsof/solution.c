#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include<unistd.h>
#include<dirent.h>
#include<getopt.h>
#include<fcntl.h>
#include<time.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<sys/errno.h>
#include<sys/time.h>

#include<pwd.h>

#define MAX_MEM_SIZE 256
#define MAX_PIDSTR_SIZE 16 

typedef struct {
    int pid; // (1)
    char* comm; // (2)
} pid_stat_t;

void pid_stat_read(int pid) {
    char pathname[MAX_MEM_SIZE];
    memset(pathname, '\0', MAX_MEM_SIZE);
    strcat(pathname, "/proc/");
    
    char pid_in_str[MAX_PIDSTR_SIZE];
    memset(pid_in_str, '\0', MAX_PIDSTR_SIZE);
    sprintf(pid_in_str, "%d", pid);
    strcat(pathname, pid_in_str);

    struct stat mystat;
    if(stat(pathname, &mystat) == -1) {
			perror("Stat error");
			exit(-1);
    }
    
    
    char stat[MAX_MEM_SIZE];
    strcat(stat, pathname);
    strcat(stat, "/stat");

    // Now we have stat = "/proc/[pid]/stat";
    // So we can read "stat" to print information;
    char strstat[MAX_MEM_SIZE];
    memset(strstat, '\0', MAX_MEM_SIZE);
    int fd = 0; 
    if((fd = open(stat, O_RDONLY)) == -1) {
        perror("Open error");
        exit(-1); 
    }
    if(read(fd, strstat, MAX_MEM_SIZE) == -1) {
        perror("Read error");
        exit(-1);
    }
    close(fd);

    // Now we will use sscanf to select information
    pid_stat_t pid_stat;
    char* left_pointer = strstat;
    char* right_pointer = strstat;
    struct passwd* user;
    char comm[MAX_MEM_SIZE];
    for(int i = 1; i < 52; ++i) {
        right_pointer = strstr(left_pointer, " ");
        
        if(right_pointer != NULL && i != 2) {
            *right_pointer = '\0';
        } else if(i == 2) {
            while(*(right_pointer + 2) != ' ') {
                *right_pointer = '_';
                right_pointer = strstr(right_pointer, " "); 
            }
        }
        switch(i) {
            case 1: // pid
                sscanf(left_pointer, "%d", &pid_stat.pid);
                break;
            case 2: // comm
                sscanf(left_pointer, "%s", comm);
                pid_stat.comm = comm;
                break;
        }
        left_pointer = right_pointer + 1;
        if(right_pointer == NULL) {
            break;
        }
    }
    user = getpwuid(mystat.st_uid);

    //Now we must read /proc/[pid]/fd to find fd
     
    char fd_info[MAX_MEM_SIZE];
    strcat(fd_info, pathname);
    //fd_info = "/proc/[pid]/fd/"
    strcat(fd_info, "/fd/");
    


}

int main() {
    DIR* dir;
    if((dir = opendir("/proc")) == NULL) {
		perror("Open dir error");
		exit(-1);
	}
    printf("%s %s %s %s %s %s %s %s %s\n",
           "COMMAND", "PID", "USER", //have
           "FD", "TYPE", "DEVICE", 
           "SIZE", "NODE", "NAME");
    struct dirent* dirbuf;

    for(;(dirbuf = readdir(dir)) != NULL;) {
       int pid = 0;
       sscanf(dirbuf -> d_name, "%d", &pid);
       if(pid == 0) {
           continue;
       }
       pid_stat_read(pid);
    }
    closedir(dir);
    return 0;
}
