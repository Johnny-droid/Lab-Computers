#include "leaderboard.h"

void(readSaveFile)(){
    memset(scores, 0, sizeof scores);
    memset(dates, 0, sizeof dates);
    memset(names, 0, sizeof names);
    FILE *file = fopen("savefile.txt", "r");
    char line[48];
    int i = 0;
    while(fgets(line, sizeof line, file) && i<10){
        size_t j = 0;
        char str[10];
        while(line[j]!=' ')
            str[j] = line[j++];
        
        j++;
        int sc = stoi(str);
        scores[i] = sc;
        memset(str, 0, sizeof str);
        while(line[j]!=' ')
            str[j] = line[j++];
        
        strcpy(names[i], str);
        memset(str, 0, sizeof str);
        while(line[j]!='.')
            str[j] = line[j++];
        strcpy(dates[i], str);
        i++;
    }
    fclose(file);
}


void(writeSaveFile)(){
    FILE *file = fopen("savefile.txt", "w");
    for(int i = 0; i<10; i++){
        char sc[4];
        itoa(scores[i],sc,10);
        fwrite(sc,1,sizeof sc,file);
        fwrite(" ",1,1,file);
        fwrite(names[i],1,sizeof names[i],file);
        fwrite(" ",1,1,file);
        fwrite(dates[i],1,sizeof dates[i],file);
        fwrite(".",1,1,file);
    }
    fclose(file);
}

int(compareScore)(int score){
    return score>scores[9];
}

char*(getLeaderBoard)(){
    char str[256];
    memset(str, 0, sizeof str);
    int i = 0;
    char sc[4];
    while(i<10 && names[i]!='\0'){
        memset(sc, 0, sizeof sc);
        itoa(scores[i],sc,10);
        for(int j = 4; j > sizeof(sc)/sizeof(sc[0]); j--)
            strcat(str, "0");
        strcat(str, sc);
        strcat(str,"-");
        strcat(str, names[i]);
        strcat(str,"-");
        strcat(str, dates[i++]);
        strcat(str,"\n");
    }
    return str;
}

void(addScore)(int score, char name[10], char date[10]){
    int i = 1;
    while(scores[i]<score){
        memset(scores[i-1], 0, sizeof scores[i-1]);
        memset(names[i-1], 0, sizeof names[i-1]);
        memset(dates[i-1], 0, sizeof dates[i-1]);
        scores[i-1]= scores[i];
        strcpy(names[i-1], names[i]);
        strcpy(dates[i-1], dates[i++]);
    }
    scores[--i] = score;
    strcpy(names[i], name);
    strcpy(dates[i], date);
}