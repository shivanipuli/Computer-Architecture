#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

/* 

	MEMORY GUIDE:
		totCount = however many commands there are 
		curr = where you are in the individual command
		count = number of entities in the single command
		redirection = whether or not redirection is applied on the single entity command	

*/


int contains(char* str, char c, int len){
        int ret = 0;
        for (int i=0; i<len;i++){
                if (str[i] == c){
                        ret++;
                }
        }
        return ret;
}


int redirMode(char* str){
    int len = strlen(str);
    int ret = 1;
    int count = contains (str,'>', strlen(str));

    if (count != 1){
        return 0;
    }

    for (int i=0;i<len;i++){
        if (str[i]=='>'){
            if (i<len-1){
                if (str[i+1] == '+'){
                    ret = 2;
                }
            }
        }
    }
    return ret;
}

int validRedirect(char* str){
        int ret = 1;
        char* copy = strdup(str);
        if (!redirMode(copy)){
            return 0;
        }
        //Gets the command
        char* temp = strtok(copy, ">");

        /*char* clone = strdup(temp);
        char* tok = strtok(clone, " \f\r\v\t\n");
        if (!tok){
                return 0;
        }*/
        //regular redirection
        if (redirMode(str) == 1){
            temp = strtok(NULL, " \f\r\v\t\n");

            if(!temp){
                return 0;
            }
        }
        //advanced
        else{
            ret = 2;
            temp = strtok(NULL, "+ \f\r\v\t\n");
            if(!temp){
                return 0;
            }
        }

        temp = strtok(NULL, " \f\r\v\t\n");

        if(temp){
                return 0;
        }

        return ret;
}

void myPrint(char *msg)
{
        if (msg){
            write(STDOUT_FILENO, msg, strlen(msg));
        }
}

void errorMsg(){

        char error_message[30] = "An error has occurred\n";
        write(STDOUT_FILENO, error_message, strlen(error_message));

}


int main(int argc, char *argv[])
{
        if (argc > 2){
                errorMsg();
                exit(0);
        }

        char cmd_buff[514];
        char *pinput;
        char* checkSpace ;

        int saved;
        int fileNum = -1;

        int count = 0;
        int curr=0;
        int totCount = 0;
        int redirect = 0;
        int madeNew = 0;

        char* bigBuffer;

        char* commandList[100];
        char * ovrCommandList[50];

        FILE* fp = NULL;

         if (argc == 2){
                fp = fopen(argv[1], "r");

                if(!fp){
                        errorMsg();
                        exit(0);
                }
        }

        while (1) {

                if (argc == 1 && !redirect){
                        myPrint("myshell> ");
                }

                /*SO you Gotta Loop THrough it*/
                //IF there is a file (ex: ./myshell filename)

                if (!fp){
                        fp = stdin;
                }
                int lick = 0;
                while (fgets(cmd_buff, 514, fp)){
                        lick++;
                        pinput = strdup(cmd_buff);
                        char* copy = strdup(pinput);
                        checkSpace = strtok(copy," \f\r\v\t\n");

                        char* cop2 = strdup(cmd_buff);
                        char* cop = strtok(cop2, "\n");

                        if (checkSpace){
                                myPrint(cop);
                        }
                        else{
                                break;
                        }
                        if (strlen(cop) < 513){
                                break;
                        }
                        checkSpace = strdup("a");
                }
                if (!lick){
                        exit(0);
                }
                if (checkSpace || lick>1){
                        myPrint("\n");
                }
                if (lick>1){
                        errorMsg();
                        continue;
                }

                //getting the user input and putting it into array
                //Tokenizes by ;
                char * semiToken = strtok(pinput, ";");
                ovrCommandList[totCount] = semiToken;
                totCount++;

                while (semiToken){
                        semiToken = strtok(NULL, ";");
                        if (semiToken){
                                ovrCommandList[totCount] = semiToken;
                                totCount++;
                        }
                }
	        //execute every command individually ; ;
                for (int i = 0; i<totCount; i++){

	                //this is a clone to use just in case
                        char* copy = strdup(ovrCommandList[i]);


                        char* redi = strtok(strdup(copy), ">");
                        char* red;
                        char* fileName;
                        int redirect = validRedirect(copy);

                        if (contains(ovrCommandList[i],'>',strlen(ovrCommandList[i]))){
                                if (!redirect){
                                        errorMsg();
                                        break;
                                }
                                //basic
                                if (redirect == 1){
                                        red = strtok(copy, ">");
                                        red = strtok(NULL, " \f\r\v\t\n");
                                        fileName = strdup(red);
                                }

                                //advanced
                                else if (redirect == 2){
                                        red = strtok(copy, ">");
                                        red = strtok(NULL, " +\f\r\v\t\n");
                                        fileName = strdup(red);
                                }

                                if (redirect == 2){
                                        fileNum = open(fileName, O_RDONLY);
                                //makes new file if it can't find one
                                        if (fileNum < 0){
                                                fileNum = creat(fileName, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                                                madeNew = 1;
                                        }
                                }
                                else{
                                        fileNum = open(fileName, O_WRONLY);
                                }
                                saved = dup(STDOUT_FILENO);

                                //if the file exists
                                if (fileNum >= 0){
                                        //if advanced, read the whole thing into a big buffer
                                        if (redirect == 2){
                                                if (!madeNew){
							bigBuffer = malloc(100000);
                                                        close(fileNum);
							char boof [514];
                                                        FILE* file = fopen(fileName, "r");
                                                        if(!file){
                                                                errorMsg();
                                                                continue;
                                                        }
                                                        while(fgets(boof,514,file)!= NULL){
                                                                strcat(bigBuffer, boof);
                                                        }
                                                        fclose(file);
							fileNum = open(fileName, O_WRONLY);
						}
                                        }
                                        else{
                                                close(fileNum);
                                                errorMsg();
                                                continue;
                                        }
                                }

                                if (redirect == 1){
                                        fileNum = creat(fileName, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                                }
                                if (fileNum < 0 ){
                                        errorMsg();
                                        continue;
                                }
                                //redirects the stream here
                                dup2(fileNum, 1);
                                close(fileNum);
                        }
                        //further breaks the command down into another aray
                        char* token = strtok(redi," >\f\r\v\t\n");
                        //protects against empty inputs
                        if (!token){
                                if (!totCount){
                                        break;
                                }
                                else{
                                        continue;
                                }
                        }

                        commandList[count] = token;
                        count++;
                        while (token){
                                token = strtok(NULL, " >\f\r\v\t\n");
                                if (token){
                                        commandList[count] = token;
                                        count++;
                                }
                        }
                        while (curr<count){
                        //exits the program
                                if (strcmp("exit", commandList[0]) == 0){
                                        if (redirect || count > 1){
                                                dup2(saved, 1);
                                                close(saved);
                                                errorMsg();
                                                break;
                                        }
                                                exit(0);
                                        continue;
                                }
                        //pwd
                                else if (strcmp("pwd", commandList[0]) == 0){
                                        if (redirect || count>1){
                                                dup2(saved, 1);
                                                close(saved);
                                                errorMsg();
                                                break;
                                        }
                                                char buff[100];
                                                getcwd(buff, 100);
                                                myPrint(buff);
                                                myPrint("\n");
                                                curr++;
                                }
                        //cd
                                else if (strcmp("cd", commandList[0])==0){
                                        if (redirect || count > 2){
                                                dup2(saved, 1);
                                                close(saved);
                                                errorMsg();
                                                break;
                                        }

                                        if (count > 1){

                                                DIR* test = opendir(commandList[1]);

                                                if (test){
                                                        chdir(commandList[1]);
                                                        closedir(test);
                                                        curr +=2;
                                                }
                                                else{
                                                        errorMsg();
                                                        break;
                                                }
                                        }
                                        else{
                                                chdir(getenv("HOME"));
                                                curr++;
                                        }
                                }
                //Executes a non-built in command
                                else{
                                        char* argv[count+1];
                                        for (int i=0; i< count;i++){
                                                argv[i] = commandList[i];
                                        }
                                        argv[count] = NULL;
                                        //child process
                                        if (fork()== 0){

                                                if (execvp(argv[0], argv)<0) {
                                                        errorMsg();
                                                }
                                                exit(0);
                                        }
                                        else {
                                                wait(NULL);
                                        }
                                        break;
                                        curr++;
                                }

                        }

                        // Write the whole file back
                        if (redirect == 2){
                                if (!madeNew){
                                        myPrint(bigBuffer);
                                }
                        }

                        dup2(saved, 1);
                        close(saved);
                        curr = 0;
                        count = 0;
                        madeNew = 0;
                }
                        madeNew = 0;
                        totCount = 0;
                        redirect = 0;
        }
}
