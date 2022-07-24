#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

void myPrint(char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}

int error(){
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
    return 0;
}
char *trim_line(char *line){
    //beginning
    int start = 0;
    int end = strlen(line)-1;

    while(isspace(line[start]) && start <= end){
        start++;
    }
    if(start==strlen(line))
        return strdup("");
    
    //end
    while(isspace(line[end])){
        end--;
    }
    end++;
    return strndup(&line[start],end-start);
}

char** parse(char *msg, char *delim){
    char **l = (char**)malloc(sizeof(char*)*strlen(msg));
    char *lines= strtok(msg,delim);
    int ind=0;
        while(lines!=NULL){
            char *temp = trim_line(strdup(lines));
            if(strcmp(temp,"")!=0){
                l[ind]=temp;
                ind++;
            }
            lines=strtok(NULL,delim);
        }
    return l;
}

char* pwd(){
    char* cwd = (char*)malloc(sizeof(char)*1024);
    getcwd(cwd,1024);
    myPrint(cwd);
    myPrint("\n");
    return cwd;
}

int cd(char *arg){
    if(!arg){
        chdir(getenv("HOME")); 
        return 2;
    }
    return chdir(arg)+1;
}

int exec(char **args,int last_ind){
    int status;
    pid_t p;

    if((p=fork())<0){
        //char error_message[30] = "An error has occurred\n";
        //write(STDOUT_FILENO, error_message, strlen(error_message));
        return 0;
    }

    if(p==0){//child
        //int err = dup(2);
        
        if(execvp(args[0],args)==-1){
            //char error_message[30] = "An error has occurred\n";
            //write(STDOUT_FILENO, error_message, strlen(error_message));
            return 0;
        }
        else
            return 1;
    }    
    else{ // parent
        waitpid(p, &status, 0);
        return WEXITSTATUS(status);
    }
}

int too_long(char *msg){
    for(int i=0;i<514;i++)
        if(msg[i]=='\0')
            return 0;
    return 1;
}

int built_ins(char **cmds, int last_ind){
    if(!cmds[0]){
        return 1;//1=complete
    }
    //Built in commands
    if(strcmp(cmds[0],"exit")==0){
        if(last_ind>0){
            return 0;//0=error
        }
        return 3;//exit(0);
    }
    else if(strcmp(cmds[0],"pwd")==0){
        if(last_ind>0){
            return 0;
        }
        pwd();
        return 1;
    }
    else if(strcmp(cmds[0],"cd")==0){
        if(last_ind>1){
            return 0;
        }
        if(last_ind==0){//if(no args) for cd
            cd(NULL);
            return 1;
        }
        return cd(cmds[1]); //if 1 arg for cd
    }
    return -1;
}


int main(int argc, char *argv[]) 
{
    char cmd_buff[514];
    char *pinput;
    FILE *file;
    
    if(argc==2){  // batch mode
        file = fopen(argv[1], "r");
        if(!file)
            exit(0);
    }
    else if(argc>2){ //error
        char error_message[30] = "An error has occurred\n";
        write(STDOUT_FILENO, error_message, strlen(error_message));
        exit(0);
    }
    else{ //interactive mode
        file = stdin;
    }

    while (1) {

        if(file==stdin)
            myPrint("myshell> ");
        pinput = fgets(cmd_buff, 514, file);
        if (!pinput) {
            exit(0);
        }
        char* temp = trim_line(cmd_buff);
        if(strcmp(temp,"")==0)
            continue;

        if(file!=stdin){
            myPrint(cmd_buff);
        }
        
        if(too_long(cmd_buff)){
            while(pinput){
                pinput = fgets(cmd_buff, 514, file);
                myPrint(pinput); 
            }
            error();
            continue;
        }

        char **lines=parse(strdup(cmd_buff),"\n;");
        int line_ind=0;
        /*while(lines[line_ind]!=NULL)
            line_ind++;

        for(int i=0; i< line_ind;i++){
            char *line = strdup(lines[line_ind]);
            if(strcmp(line,"")==0)
                continue;
                //goto start;
            if(line[strlen(line)-1]=='>'){
                error();
                continue;//goto start;
            }
            //check redirect
            char **redir =parse(line,">");
            if(redir[1]==NULL){//no redir file
                char** args = parse(redir[0], " \f\r\v\t");
                int last_ind=0;
                while(args[last_ind]!=NULL)
                    last_ind++;
                last_ind--;

                int succ = built_ins(args,last_ind);
                if(succ==3){
                    exit(0);
                }
                else if(succ==0){
                    error();
                    //goto start;
                }
                else{// if(succ==-1){
                    succ = exec(args,last_ind);
                    if(succ==0)
                        error();
                    //goto start;
                }
            }
            else if(redir[2]==NULL){//redir
                //char** args = parse(redir[0], " \f\r\v\t");
                char** files = parse(redir[1], " \f\r\v\t");
                if(strcmp(files[0],"+")==0){//advanced redirect
                    if(files[1]==NULL){
                        error();
                        //goto start;
                    }
                    else if(files[2]!=NULL){//too many args
                        error();
                        //goto start;
                    }
                    else{//perform advanced redirect on args

                    }
                }
                else{
                    if(files[0]==NULL || files[1]!=NULL){//too many args
                        error();
                        //goto start;
                    }
                    else{//redirect
                        //do_command(args,NULL);
                    }
                }
            }
            else{//too many redirs
                error();
                //goto start;
            }
        }*/

            char *tline = strdup(lines[line_ind]);
            char *line = trim_line(tline);
            if(strcmp(line,"")==0){
                continue;
            }
            if(line[strlen(line)-1]=='>'){
                char error_message[30] = "An error has occurred\n";
                write(STDOUT_FILENO, error_message, strlen(error_message));
                continue;
            }
            char **redir =parse(line,">");
            char *cmd = strdup(redir[0]);
            char **args=parse(cmd," \f\r\v\t");

            int last_ind=0;
            while(args[last_ind]!=NULL){
                last_ind++;
            }
                last_ind--;

            char *outfile = NULL;
            if(redir[1]!=NULL){
                if(strcmp(args[0],"pwd")==0 || strcmp(args[0],"cd")==0 || strcmp(args[0],"exit")==0 ){
                    char error_message[30] = "An error has occurred\n";
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    continue;
                }
                if(redir[2]!=NULL){//too many args
                    char error_message[30] = "An error has occurred\n";
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    continue;
                }
                char *cmd2 = strdup(redir[1]);
                char **arg2=parse(cmd2," \f\r\v\t");
                int lind=0;
                while(arg2[last_ind]!=NULL){
                    lind++;
                }
                lind--;
                if(lind>0){
                    char error_message[30] = "An error has occurred\n";
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    continue;
                }
            else//lind==0
                outfile=strdup(arg2[0]);
            }
                
                int succ = built_ins(args,last_ind);
                if(succ==3){
                    exit(0);
                }
                else if(succ==0){
                    char error_message[30] = "An error has occurred\n";
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    continue;
                }
                else if(succ==-1){
                    if(outfile==NULL){
                        succ = exec(args,last_ind);
                        printf("%d",succ);
                        if(succ==0){
                            char error_message[30] = "An error has occurred\n";
                            write(STDOUT_FILENO, error_message, strlen(error_message));
                            continue;
                        }
                    }
                    else{
                        outfile = fopen(file,"w");
                        if(outfile==NULL){
                            char error_message[30] = "An error has occurred\n";
                            write(STDOUT_FILENO, error_message, strlen(error_message));
                            return(0);
                        }
                    }
                    succ = exec(args,last_ind);
                    if(succ==0){
                        continue;
                    }
                }
            
            }
    }



