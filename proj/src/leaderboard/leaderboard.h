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


/**
 * @brief Reads the leaderboard from the file.
 * If not found creates an empty leaderboard
 */
void(readSaveFile)(struct leaderboard* LB);


/**
 * @brief Writes to file the leaderboard
 */
void(writeSaveFile)(struct leaderboard* LB);


/**
 * @brief Comparison function for scores
 */
int(compareScore)(int score, struct leaderboard LB);

/**
 * @brief Adds score to leaderboard
 */
void(addScore)(int score, char* name, /*char date[10],*/ struct leaderboard* LB);

/**
 * @brief returns leaderboard
 */
char * (getLeaderBoard)(struct leaderboard* LB, char* str);

#endif //LC_LEADERBOARD_H
