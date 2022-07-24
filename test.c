#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *trim_line(char *line){
    //beginning
    int start = 0;
    int end = strlen(line)-1;

    while(line[start]==' ' && start <= end){
        start++;
    }
    if(start==strlen(line))
        return strdup("");
    
    //end
    while(line[end]==' '){
        end--;
    }
    end++;
    char *newl = (char*)malloc(sizeof(char)* (end-start+1));
    for(int i=0;i<=(end-start);i++){
        newl[i]=newl[start+i];
    }
    newl[end]='\0';
    return strndup(&line[start],end-start);
}

int main ()
{
  char* temp= "  d     d ";
  printf(".%s.\n",trim_line(temp));
}
