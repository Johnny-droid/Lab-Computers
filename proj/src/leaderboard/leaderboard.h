//
// Created by Acer on 07/06/2022.
//

#ifndef LC_LEADERBOARD_H
#define LC_LEADERBOARD_H

#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../game.h"
#include "ps2.h"

char names[10][6];

char dates[10][10];

uint8_t scores[10];

void(readSaveFile)();

void(writeSaveFile)();

int(compareScore)(int score);

void(addScore)(int score, char name[10], char date[10]);

char * (getLeaderBoard)();

#endif //LC_LEADERBOARD_H
