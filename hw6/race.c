#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t forkret = 0;

void handler1(int sig){
    printf(" (%d) handler1\n",getpid());
    if(!forkret){
        printf(" (%d) seeya\n",getpid());
        exit(0);
    }
}

int main() {
    printf("(parent) hello --------------\n");
    signal(SIGUSR1, handler1);
    if ((forkret = fork()) == 0) {
        printf("(child) hello I’m %d\n", getpid());
        kill(getppid(), SIGUSR1);
        while (1) {};
    } else {
        printf("(parent) child is %d\n", forkret);
        kill(forkret, SIGUSR1);
        wait(NULL);
        printf("(parent) child done\n");
        }
        exit(0);
    }