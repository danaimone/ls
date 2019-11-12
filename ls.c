/* This program lists the files contained within the current working directory.
 *
 */

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include "linkedlist.h"

bool printAllFlag = false;
bool printLongFlag = false;
bool printINode = false;
bool printDirs = false;


char* fileType(struct stat fstat) {
    static const mode_t modeMask[] = {S_IFBLK, S_IFCHR, S_IFDIR, S_IFLNK, S_IFIFO, S_IFREG, S_IFSOCK};
    static char* modeChar[] = {"b", "c", "d", "l", "p", "f", "s", "?"};
    size_t modeSize = sizeof(modeMask) / sizeof(int);
    mode_t fileMode = fstat.st_mode;
    int i = 0;
    while (i < modeSize && modeMask[i] != (fileMode & S_IFMT)) {
        ++i;
    }
    return modeChar[i];
}

char* permissionStringBuilder(mode_t statBits, struct stat sb) {
    char *permissionString = malloc(sizeof(char[12]));
    char* fType = fileType(sb);
    strcpy(permissionString, fType);
    static const mode_t permissionMask[10] = {S_IRUSR, S_IWUSR, S_IXUSR,
                                              S_IRGRP, S_IWGRP, S_IXGRP,
                                              S_IROTH, S_IWOTH, S_IXOTH};
    static const char *permissionChar[4] = {'r', 'w', 'x'};
    int maskSize = 9;
    int i = 0;
    while (i < maskSize) {
        if ((permissionMask[i]&statBits)) {
            strcat(permissionString, &permissionChar[i%3]);
        } else {
            strcat(permissionString, "-");
        }
        ++i;
    }
    return permissionString;
}


int dirsize(struct dirent *dp, DIR *dir) {
    int size = 0;
    while (dp != NULL) {
        ++size;
        dp = readdir(dir);
    }
    return size;
}

bool prefix(const char *pre, const char *str) {
    return strncmp(pre, str, strlen(pre)) == 0 ? true : false;
}

/*
 * Filters out unnecessary files (such as . and ..) as well as files prefixed with ., since
 * this is not presented normally in ls.
 */
int filter(char *entry) {
    if ((strcoll(entry, ".") == 0) || (strcoll(entry, "..") == 0) || prefix(".", entry)) {
        return true;
    } else {
        return false;
    }
}

void printLong(struct dirent **dp, int size) {
    struct stat sb;
    struct passwd *password;
    struct group *grp;
    char *name = "";
    char* group = "";
    char time[255];
    int i = 0;
    while (i < size) {
        if (stat(dp[i]->d_name, &sb) == -1){
            perror("stat");
            exit(EXIT_FAILURE);
        }

        if ((password = getpwuid(sb.st_uid)) != NULL)
            name = password->pw_name;

        if((grp = getgrgid(sb.st_gid)) != NULL) {
            group = grp->gr_name;
        }
        char *permissionString = permissionStringBuilder(sb.st_mode, sb);
        strftime(time, 255, "%b %d %H:%M", localtime((&sb.st_mtime)));
        off_t fileSize = sb.st_size;
        printf("%s %ld %s %s %5ld %s %s\n", permissionString, sb.st_nlink, name, group, fileSize, time, dp[i]->d_name);
        ++i;
    }
}

void printDir(struct dirent **dp, int size) {
    int i = 0;
    while (i < size) {
        puts(dp[i]->d_name);
        ++i;
    }
}

/*
 * Given a DIR object, dir, list the files within that directory.
 */
void list(DIR *dir) {
    assert(dir != NULL);
    struct dirent *ent = readdir(dir);
    struct dirent **list;
    node *directory;
    int size = dirsize(ent, dir);
    list = malloc((size * sizeof(*list)));
    if (list == NULL) { // somehow we ran out of memory, should never happen
        closedir(dir);
        perror("malloc");
        exit(1);
    }
    rewinddir(dir); // directory reset, necessary as the DIR object has already been exhausted
    size = 0; // resetting size for iteration
    while ((ent = readdir(dir)) != NULL) {
        if (!filter(ent->d_name) || printAllFlag)
            list[size++] = ent;
    }
    if (printLongFlag) {
        printLong(list, size);
    } else {
        printDir(list, size);
    }
}

void getArgFlags(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "al")) != -1) {
        switch (opt) {
            case 'a':
                printAllFlag = true;
                break;
            case 'l':
                printLongFlag = true;
                break;
            case 'i':
                printINode = true;
            case 'd':
                printDirs = true;
        }
    }
}

bool isFlag(const char *str) {
    return prefix("-", str);
}

void getCommandArgs(node *files, node *directories, int argc, char *argv[]) {
    struct stat sb;
    for (int i = 1; i < argc; i++) {
        if (!isFlag(argv[i])) {
            if (stat(argv[i], &sb) != 0) {
                printf("./ls: cannot access '%s': No such file or directory\n", argv[i]);
            } else {
                if (strcoll(fileType(sb), "d") == 0) {
                    appendNode(&directories, strdup(argv[i]));
                } else {
                    appendNode(&files, strdup(argv[i]));
                }
            }
        }
    }
}

/*
 * listLinked
 * prints information on given arguments from argv
 * files and directories are linkedLists of arguments; files are handled first,
 * then directories
 *
 * Parameters:
 * node *files = the linked list of files given from argv
 * node *files = the linked list of directories given from argv
 */
void listLinked(node *files, node *directories) {
    if (files != NULL) {
        printList(files);
    }

    while (directories != NULL) {


    }
}

int main(int argc, char *argv[]) {
    node *files = NULL;
    node *directories = NULL;
    getCommandArgs(&files, &directories, argc, argv);
    getArgFlags(argc, argv);
    if (files == NULL && directories == NULL) {
        DIR *d = opendir(".");
        list(d);
        if (d == NULL) {
            perror("diropen");
            exit(1);
        }
        closedir(d);
    } else {
        listLinked(files, directories);
    }
    return 0;
}

/* TODO: process all argv that are lsable (skip flags) and loop through the directories
 * make a linked list of files and a linked list of directories
 */