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
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <sys/ioctl.h>
#include "linkedlist.h"
#include "filter.h"

bool printAllFlag = false;
bool printDirs = false;
bool printINodeFlag = false;
bool printLongFlag = false;

void printColumns(node *head, int maxFileLength);

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

void getArgFlags(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "adil:")) != -1) {
        switch (opt) {
            case 'a':
                printAllFlag = true;
                break;
            case 'd':
                printDirs = true;
                break;
            case 'i':
                printINodeFlag = true;
                break;
            case 'l':
                printLongFlag = true;
                break;
            case '?':
                printf("ls: invalid option -- %c\n", optopt);
            default:
                break;
        }
    }
}

void getCommandArgs(node *files, node *directories, int argc, char *argv[]) {
    struct stat sb;
    for (int i = 1; i < argc; i++) {
        if (!isFlag(argv[i])) {
            if (stat(argv[i], &sb) != 0) {
                printf("./ls: cannot access '%s': No such file or directory\n", argv[i]);
            } else {
                if (strcoll(fileType(sb), "d") == 0 ) {
                    appendNode(&directories, strdup(argv[i]));
                } else {
                    appendNode(&files, strdup(argv[i]));
                }
            }
        }
    }
}

int getMaxFileLength(node *head) {
    int length = NULL;
    int maxLength = NULL;
    while (head != NULL) {
        length = (int) strlen(head->string);
        if (length > maxLength) {
            maxLength = length;
        }
        head = head->next;
    }

    if (printINodeFlag) {
        maxLength += 12;
    }
    return maxLength + 1;
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void printLong(node *head) {
    struct stat sb;
    struct passwd *password;
    struct group *grp;
    char *name = "";
    char* group = "";
    char time[255];
    while (head != NULL) {
        if (stat(head->string, &sb) == -1){
            perror("stat in printLong");
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
        printf("%s %hu %s %s %5lld %s %s\n", permissionString, sb.st_nlink, name, group, fileSize, time, head->string);
        head = head->next;
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
    node *directory = NULL;
    if (!printDirs) {
        while (ent != NULL) {
            if (!filter(ent->d_name) || printAllFlag)
                appendNode(&directory, ent->d_name);
            ent = readdir(dir);
        }
    } else {
        appendNode(&directory, ".");
    }

    if (printLongFlag) {
        printLong(directory);
    } else {
        int maxLength = getMaxFileLength(directory);
        printColumns(directory, maxLength);
    }
}

char *buildFormatString(int maxFileLength) {
    char *format = malloc(sizeof(maxFileLength));
    sprintf(format, "%d", maxFileLength);
    char *printString = malloc(sizeof(10));
    strcpy(printString, "%-");
    strcat(printString, format);
    strcat(printString, "s ");
    return printString;
}

void printColumns(node *head, int maxFileLength) {
    struct winsize size;
    int ret = ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    if (ret != -1) {
        int maxColumns = size.ws_col / maxFileLength;
        int i = 1;
        while (head != NULL) {
            char *printString = buildFormatString(maxFileLength);
            printf(printString, head->string);
            if (maxColumns != 0 && i % maxColumns == 0) {
                printf("\n");
            }
            i++;
            head = head->next;
        }
    } else {
        printList(head);
    }
    printf("\n");
}

/*
 * listLinked
 * prints information on given arguments from argv
 * files and directories are linkedLists of arguments; files are handled first,
 * then directories
 *
 * Parameters:
 * node *files = the linked list of files given from argv
 * node *directories = the linked list of directories given from argv
 */
void listLinked(node *files, node *directories) {
    node *unionList = NULL;
    if (printDirs) {
        unionList = sortedMerge(directories, files);
        files = unionList;
    }

    int maxFileLength = getMaxFileLength(files);
    if (files != NULL) {
        if (!printLongFlag) {
            printColumns(files, maxFileLength);
            printf("\n");
        } else {
            printLong(files);
        }
        printf("\n");
    }


    while (directories != NULL && !printDirs) {
        node *fileList = NULL;
        DIR *dir = opendir(directories->string);
        if (dir == NULL) {
            perror("opendir");
        }
        char *currentDir = NULL;
        currentDir = getcwd(currentDir, PATH_MAX);
        chdir(directories->string);
        struct dirent *ent = readdir(dir);
        while (ent != NULL) {
            if (!filter(ent->d_name) || printAllFlag)
                appendNode(&fileList, ent->d_name);
            ent = readdir(dir);
        }
        printf("%s:\n", directories->string);
        bubbleSort(fileList);
        int maxDirectoryLength = getMaxFileLength(fileList);
        if (!printLongFlag) {
            printColumns(fileList, maxDirectoryLength);
        } else {
            printLong(fileList);
        }
        closedir(dir);
        chdir(currentDir);
        free(currentDir);
        directories = directories->next;
    }
}

int main(int argc, char *argv[]) {
    node *files = NULL;
    node *directories = NULL;
    getArgFlags(argc, argv);
    getCommandArgs(&files, &directories, argc, argv);
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
    freeList(files);
    freeList(directories);
    return 0;
}

