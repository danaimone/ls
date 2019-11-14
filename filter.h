//
// Created by Dan Aimone on 11/12/19.
//

#ifndef LS_FILTER_H
#define LS_FILTER_H

#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

char* fileType(struct stat fstat);
bool prefix(const char *pre, const char *str);
/*
 * Filters out unnecessary files (such as . and ..) as well as files prefixed with ., since
 * this is not presented normally in ls.
 */
int filter(char *entry);
bool isFlag(const char *str);
#endif //LS_FILTER_H
