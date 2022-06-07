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