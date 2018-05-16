#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int TCP_serv_socket(int, char*, int);
extern int UDP_serv_socket(int);
extern int read_store(char*, char*[]);
extern char servName;

pid_t pids[4];
char* fileNames[] = {"serverA.txt", "serverB.txt", "serverC.txt", "serverD.txt"};

int fork_childs_send(int n) {  // copy 

        int i, status;
        for(i = 0; i < n; i++) {
            if( (pids[i] = fork()) == 0 ) {
                    // printf("%dth child process created\n", i);
                    char* line;
                    int size = read_store(fileNames[i], &line);
                    // printf("line[%d] = %s\n", i, line);
                    TCP_serv_socket(25029, line, size);
                    free(line);
                    // printf("Server %c has sent the messages\n", fileNames[i][6]);
                    // printf("%dth child done with its work!\n", i);
                    exit(0);
            }
            else if (pids[i] > 0){
                // printf("Parent finish creating the %dth child\n", i);
                pids[i] = wait(&status);
                // printf("Child with PID %ld exited with status 0x%x.\n", (long)pids[i], status);
                continue;
            }
            printf("fork() failed!\n");
            return -1;
        }
        return 0;
}

int fork_childs_recv(int n) {

        int i, status;
        for(i = 0; i < n; i++) {
            if( (pids[i] = fork()) == 0 ) {
                    switch(fileNames[i][6]) {
                        case 'A': UDP_serv_socket(21029); break;
                        case 'B': UDP_serv_socket(22029); break;
                        case 'C': UDP_serv_socket(23029); break;
                        case 'D': UDP_serv_socket(24029); break;
                    default: 
                        perror("Not a valid server name!");
                        exit(-1);
                    }
                    exit(0);
            }
            else if (pids[i] > 0){
                // printf("Parent finish creating the %dth child\n", i);
                pids[i] = wait(&status);
                // printf("Child with PID %ld exited with status 0x%x.\n", (long)pids[i], status);
                continue;
            }
            printf("fork() failed!\n");
            return -1;
        }
        return 0;
}



int main()
{
    printf("--beginning of program\n");

    int n = 4;
    fork_childs_send(n);
    fork_childs_recv(n);

    printf("--end of program--\n");

    return 0;
}
