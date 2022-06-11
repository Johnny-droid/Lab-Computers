//
// Created by Acer on 07/06/2022.
//

#ifndef LC_LEADERBOARD_H
#define LC_LEADERBOARD_H

#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../game.h"

struct leaderboard
{
    char names[10][7];

    uint8_t scores[10];
};


void(readSaveFile)(struct leaderboard* LB);

void(writeSaveFile)(struct leaderboard* LB);

int(compareScore)(int score, struct leaderboard LB);

void(addScore)(int score, char* name, /*char date[10],*/ struct leaderboard* LB);

char * (getLeaderBoard)(struct leaderboard* LB, char* str);

#endif //LC_LEADERBOARD_H
