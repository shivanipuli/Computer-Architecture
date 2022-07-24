#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <math.h>


struct block {
    int valid;
    int tag;
    int lru;
};

typedef struct block block;


int load(block **sets,int E, int tag, int set, int offset,int time){
    int e=0;
    do{
        block *bl = sets[E*set+e];
        if(bl->valid==0){
                bl->tag = tag;
                bl->valid = 1;
                bl->lru=time;
                return 0;//0 = miss
            }
        else if(bl->tag == tag){
                bl->lru=time;
                return 1; // 1 = hit
            }
        e++;
    }while(e<E);
    //miss eviction
    //find lru_index
    int ind=E*set;
    for(int x = ind+1;x<E*(set+1);x++){
            if(sets[ind]->lru > sets[x]->lru)
                ind=x;
    }

    block *bl = sets[ind];
    bl->lru=time;
    bl->tag=tag;
    bl->valid=1;
    return 2;// 2 = miss eviction
}



int main(int argc, char **argv)
{	
    int hitCount=0,missCount=0,evictionCount=0;
    int time=0;
    int s,S,E,b,B;
    int v=0,h=0;
    char *traces;
    int input;
    FILE *f; 

    //Reading arg variables
    while((input = getopt(argc, argv, "hvs:E:b:t:")) != -1){
		switch(input){
			case 's':
				s=atoi(optarg);
                S = pow(2,s);
				break;
			case 'E':
				E = atoi(optarg);
				break;
			case 'b':
				b=atoi(optarg);
                B = pow(2,b);
				break;
			case 't':
				traces = optarg;
				break;
            case 'v':
                v = 1;
				break;
            case 'h':
                h = 1;
				break;
            default:
                break;
		}
    }

    if(!S || !B || !E || !traces){
        fprintf(stderr,"Invalid Input \n");
        exit(1);
    }
    f = fopen(traces,"r");
    
    //Creating caches
    block **sets = (block**)malloc(sizeof(block*)*E*S);
    int i=0;
    for(int s=0;s<S;s++){
        for(int e=0;e<E;e++){
            sets[i] = (block*)malloc(sizeof(block));
            sets[i]->lru=0;
            sets[i]->tag=-1;
            sets[i]->valid = 0;
            i++;
        }
    }
    //Read File
    char buffer[3];
    while(fscanf(f,"%2s",buffer)!=EOF){
        if(buffer[0]!='I'){
            int size;
            unsigned long address;
            fscanf(f," %lx,%u\n",&address,&size);
            printf("%c %lx,%u ",buffer[0],address,size);
            unsigned long tag = address;
            int offset = tag & (B-1);
            tag = tag >> b;
            int set = tag & (S-1);
            tag = tag >> s; 
            //printf("tag: %lu set: %d offset: %d \n",tag,set,offset);
            int res;
            switch(buffer[0]){
                case 'L': 
                case 'S':
                    time+=1;
                    res = load(sets,E,tag,set,offset,time);
                    switch(res){
                        case 0:
                            printf("miss\n");
                            missCount++;
                            break;
                        case 1:
                            printf("hit\n");
                            hitCount++;
                            break;
                        case 2:
                            printf("miss eviction\n");
                            missCount++;
                            evictionCount++;
                            break;
                    }
                    break;
                case 'M':
                    time+=1;
                    res = load(sets,E,tag,set,offset,time);
                    switch(res){
                        case 0:
                            printf("miss hit\n");
                            missCount++;
                            break;
                        case 1:
                            printf("hit hit\n");
                            hitCount++;
                            break;
                        case 2:
                            printf("miss eviction hit\n");
                            missCount++;
                            evictionCount++;
                            break;
                    }
                    time+=1;
                    load(sets,E,tag,set,offset,time);
                    hitCount++;
                    break;
            }
        }
    }
    printSummary(hitCount,missCount,evictionCount);

    //Free Malloc
    fclose(f);
    for(i=0;i<S*E;i++)
        free(sets[i]);
    free(sets);
    return 0;
}