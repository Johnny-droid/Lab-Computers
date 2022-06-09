#include "leaderboard.h"

void(readSaveFile)(struct leaderboard * LB){
    memset(LB->scores, 0, sizeof LB->scores);
    memset(LB->dates, 0, sizeof LB->dates);
    memset(LB->names, 0, sizeof LB->names);
    FILE *file = fopen("leaderboard/savefile.txt", "r");
    if(!file){
        //game_state = EXIT;
        return;
    }
    char line[48];
    int i = 0;
    while(fgets(line, 48, file) && i<10){
        size_t j = 0;
        char temp_str[10];
        while(line[j]!=' '){
            temp_str[j] = line[j];
            j++;
        }
        
        j++;
        int sc = atoi(temp_str);
        LB->scores[i] = sc;
        memset(temp_str, 0, sizeof temp_str);
        while(line[j]!=' '){
            temp_str[j] = line[j];
            j++;
        }
        j++;
        strcpy(LB->names[i], temp_str);
        memset(temp_str, 0, sizeof temp_str);
        while(line[j]!='.'){
            temp_str[j] = line[j];
            j++;
        }
        strcpy(LB->dates[i], temp_str);
        i++;
    }
    fclose(file);
}


void(writeSaveFile)(struct leaderboard *LB){
    FILE *file = fopen("leaderboard/savefile.txt", "w");
    if(!file)
        return;
    for(int i = 0; i<10; i++){
        char sc[4];
        sprintf(sc, "%d", LB->scores[i]);
        fwrite(sc,1,sizeof sc,file);
        fwrite(" ",1,1,file);
        fwrite(LB->names[i],1,sizeof LB->names[i],file);
        fwrite(" ",1,1,file);
        fwrite(LB->dates[i],1,sizeof LB->dates[i],file);
        fwrite(".",1,1,file);
    }
    fclose(file);
}

int(compareScore)(int score, struct leaderboard LB){
    return score>LB.scores[9];
}

char*(getLeaderBoard)(struct leaderboard* LB){
    memset(str, 0, sizeof str);
    int i = 0;
    char sc[4];
    size_t c;
    int n;
    while(i<10 && LB->names[i]!='\0'){
        memset(sc, 0, sizeof sc);
        sprintf(sc, "%d", LB->scores[i]);
        n = LB->scores[i];
        c = 0;
        while (n != 0){
            n/=10;
            c++;
        }
        for(size_t j = 3; j > c; j--)
            strcat(str, "0");
        strcat(str, sc);
        strcat(str,"-");
        strcat(str, LB->names[i]);
        for(size_t j = 6; j > (sizeof LB->names[i]/sizeof LB->names[i][0]); j--)
            strcat(str, " ");
        strcat(str,"-");
        strcat(str, LB->dates[i]);
        strcat(str,"\n");
        i++;
    }
    return str;
}

void(addScore)(int score, char name[10], char date[10], struct leaderboard* LB){
    size_t i = 1;
    while(LB->scores[i]<score && i<10){
        memset(LB->names[i-1], 0, sizeof LB->names[i-1]);
        memset(LB->dates[i-1], 0, sizeof LB->dates[i-1]);
        LB->scores[i-1]= LB->scores[i];
        strcpy(LB->names[i-1], LB->names[i]);
        strcpy(LB->dates[i-1], LB->dates[i]);
        i++;
    }
    i--;
    LB->scores[i] = score;
    strcpy(LB->names[i], name);
    strcpy(LB->dates[i], date);
}
