//
// Created by Dan Aimone on 11/12/19.
//

#ifndef LS_FILTER_H
#define LS_FILTER_H

#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/*
 * Function: fileType
 * --------------------
 * given a stat struct, returns the fileType character
 *
 * struct stat fstat: the stat struct of a file to be checked
 */
char* fileType(struct stat fstat);

/*
 * Function: prefix
 * --------------------
 * helper function to figure out whether a str begins with a certain character
 *
 * returns: true if str has prefix, false otherwise
 */
bool prefix(const char *pre, const char *str);

/*
 * Function: filter
 * --------------------
 * Filters out unnecessary files (such as . and ..) as well as files prefixed with ., since
 * this is not presented normally in ls
 * If the entry is a ., .., or a flag, this function returns true
 *
 * char *entry: string data to be compared to "." or ".."
 */

bool filter(char *entry);

/*
 * Function: isFlag
 * --------------------
 * checks whether a command line string has a - before it to denote a flag
 *
 * const char *str: command line arg to be checked against
 *
 * returns: true if str starts with a -, false otherwise
 */
bool isFlag(const char *str);
#endif //LS_FILTER_H
