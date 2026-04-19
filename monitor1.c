#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <string.h>

void garbageFunc(int n)
{
    printf("\nSignal Overriden");
}

int main()
{
    signal(SIGINT, garbageFunc);
    signal(SIGTSTP, garbageFunc);
    signal(SIGQUIT, garbageFunc);

    struct stat st;
    int * childAlive = (int *) mmap(NULL, 4 ,PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    int *childPID = (int *) mmap(NULL, 4 ,PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    char input[256];
    strcpy(input, "null");
    char cwd[256];
    int status;

    int parentPID = getpid();
    printf("\nParent PID: %d\n", parentPID);

    *childAlive = 1;
    if (fork() == 0)
    {
        signal(SIGINT, garbageFunc);
        signal(SIGTSTP, garbageFunc);
        signal(SIGQUIT, garbageFunc);

        DIR *d;
        struct dirent *dp;
        getcwd(cwd, sizeof(cwd));
        int changeDirStatus;

        *childPID = getpid();
        printf("Child PID: %d\n", *childPID);
        while (1)
        {
            printf("\033[01;34m");
            printf("\nstat monitor1 . %s", cwd);
            printf("\033[0m");
            printf("$");

            scanf("%s", input);
            printf("got scaned %s\n", input);

            if (strcmp(input, "q") ==  0)
                break;
            if (strcmp(input, "..") ==  0)
            {
                getcwd(cwd, sizeof(cwd));
                changeDirStatus = chdir("..");
                getcwd(cwd, sizeof(cwd)); 
                if (changeDirStatus == -1)
                    printf("Specified directory not found\n");
                else
                    printf("Directory updated: %s\n", cwd);
                //break;
                continue;
            }
            if (strcmp(input, "list") ==  0)
            {
                if ((d = opendir(cwd)) != NULL)
                {
                    printf("Current working directory: %s\n",cwd);
                    printf("Files:\n");
                    while ((dp = readdir(d)) != NULL)
                    {
                        printf("- %s\n", dp->d_name);
                    }
                    printf("\n");
                } 
                else
                {
                    printf("Unable to open directory");
                }
                closedir(d);
                //break;
                continue;
            }
            if (strncmp(input, "/", 1) == 0)
            {
                char cwd2[256];
                strcpy(cwd2, ".");
                strcat(cwd2, input);
                changeDirStatus = chdir(cwd2);
                getcwd(cwd, sizeof(cwd));
                if (changeDirStatus == -1)
                    printf("Specified directory not found\n");
                else
                    printf("Directory updated: %s\n", cwd);
                //break;
                continue;
            }
                stat(input, &st);
                printf("\nID of device containing file: %d", st.st_dev);
                printf("\nInode number: %llu", st.st_ino);
                printf("\nFile type and mode: %d", st.st_mode);
                printf("\nNumber of hard links: %d", st.st_nlink);
                printf("\nUser ID of owner: %d", st.st_uid);
                printf("\nGroup ID of owner: %d", st.st_gid);
                printf("\nDevice ID (if special file): %d", st.st_rdev);
                printf("\nTotal size, in bytes: %lld", st.st_size);
                printf("\nBlock size for filesystem I/O: %d", st.st_blksize);
                printf("\nNumber of 512B blocks allocated: %lld\n", st.st_blocks);
        }
        *childAlive = 0;
        printf("exiting child %d\n", *childAlive);
    }
    else 
    { 
        

        while (*childAlive == 1)
        {
            waitpid(*childPID, &status, WNOHANG);
            if (status != NULL)
            {
                *childAlive = 0;
                printf("\nwaitpid return %x\n", status);
            }
            else
            {
                sleep(10);
                printf("\nParent process\n");
            }
        }
        
        printf("terminating\n");
        kill(*childPID, SIGTERM);
        wait(0); 
    }
    printf("final exit %d\n", getpid());
    return 1;
}
