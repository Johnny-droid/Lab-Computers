#include "leaderboard.h"


void(readSaveFile)(struct leaderboard * LB){
    memset(LB->names, 0, sizeof LB->names);
    FILE *file = fopen("/home/lcom/labs/proj/src/savefile.txt", "r");
    if(!file){
        for(int i = 0; i<10; i++){
            LB->scores[i] = 0;
        }
        return;
    }
    
    char line[48];
    int i = 9;
    while(fgets(line, 48, file) && i>=0){
        size_t j = 0;
        char temp_str[10];
        memset(temp_str, 0, sizeof temp_str);
        while(line[j]!=' '){
            temp_str[j] = line[j];
            j++;
        }
        
        j++;
        int sc = atoi(temp_str);
        LB->scores[i] = sc;
        memset(temp_str, 0, sizeof temp_str);
        size_t k = 0;
        while(line[j]!='.'){
            temp_str[k] = line[j];
            j++; k++;
        }
        strcpy(LB->names[i], temp_str);
        i--;
    }
    fclose(file);
}


void(writeSaveFile)(struct leaderboard *LB){
    FILE *file = fopen("/home/lcom/labs/proj/src/savefile.txt", "w+");
    
    for(int i = 9; i<=0; i--)
        fprintf(file, "%d %s.\n", LB->scores[i], LB->names[i]);
    
    fclose(file);
}

int(compareScore)(int score, struct leaderboard LB){
    return score>LB.scores[0];
}

char*(getLeaderBoard)(struct leaderboard* LB, char* str){
    memset(str, 0, 256);
    int i = 9;
    char sc[4];
    size_t c;
    int n;
    strcat(str, "LEADERBOARD\n\n\n");
    while(i>=0){
        memset(sc, 0, sizeof sc);
        sprintf(sc, "%d", LB->scores[i]);
        n = LB->scores[i];
        c = 0 + (LB->scores[i]==0);
        while (n != 0){
            n/=10;
            c++;
        }
        for(size_t j = 4; j > c; j--)
            strcat(str, "0");
        strcat(str, sc);
        strcat(str,"-");
        strcat(str, LB->names[i]);
        strcat(str,"\n");
        i--;
    }
    return str;
}

void(addScore)(int score, char* name, struct leaderboard* LB){
    size_t i = 1;
    while(LB->scores[i]<score && i<10){
        memset(LB->names[i-1], 0, sizeof LB->names[i-1]);
        LB->scores[i-1]= LB->scores[i];
        strcpy(LB->names[i-1], LB->names[i]);
        i++;
    }
    i--;
    memset(LB->names[i], 0, sizeof LB->names[i]);
    LB->scores[i] = score;
    strcpy(LB->names[i], name);
}
