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

/*
 * Function: usage
 * --------------------
 * returns a program usage string
 *
 * returns: program usage string
 */
char *usage() {
    static char usage[] = "usage: ls [-aidl] [file ....]\n";
    return usage;

}

/*
 * Function: buildPermissionString
 * --------------------
 * builds a string for the permission bits from the statted bits and a given stat structure
 *
 *
 * mode_t statBits: the st.st_mode from a file that has been stat
 * struct stat sb: the stat structure of the file that has been stat
 */
char *buildPermissionString(mode_t statBits, struct stat sb) {
    char *permissionString = malloc(sizeof(char[12]));
    char *fType = fileType(sb);
    strcpy(permissionString, fType);
    static const mode_t permissionMask[10] = {S_IRUSR, S_IWUSR, S_IXUSR,
                                              S_IRGRP, S_IWGRP, S_IXGRP,
                                              S_IROTH, S_IWOTH, S_IXOTH};
    static const char *permissionChar[4] = {'r', 'w', 'x'};
    int maskSize = 9;
    int i = 0;
    while (i < maskSize) {
        if ((permissionMask[i] & statBits)) {
            strcat(permissionString, &permissionChar[i % 3]);
        } else {
            strcat(permissionString, "-");
        }
        ++i;
    }
    return permissionString;
}

/*
 * Function: getArgFlags
 * --------------------
 * sets global flags to be used in other functions
 *
 * int argc: the amount of args given
 *
 * returns: true if an invalid flag was given, false on success
 */
bool getArgFlags(int argc, char *argv[]) {
    int opt;
    bool err = false;
    while ((opt = getopt(argc, argv, "adil")) != -1) {
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
                err = true;
                break;
            default:
                printf(usage());
                break;
        }
    }
    return err;
}

/*
 * Function: getCommandArgs
 * --------------------
 * given empty linked lists of both file names and directory names, getCommandArgs appends any valid command
 * line argument to either file or directories based on whether it is a valid file or directory.
 *
 * Note: a check is done to make sure we are not including flags in either of our lists
 *
 * node *files: an empty linked list to be filled with valid file names provided by the command line arguments
 * node *directories: an empty linked list to be filled with valid directory names provided by the command line arguments
 * int argc: the amount of args given at program runtime
 * char* argv: the args given at runtime
 */
void getCommandArgs(node **files, node **directories, int argc, char *argv[]) {
    struct stat sb;
    for (int i = 1; i < argc; i++) {
        if (!isFlag(argv[i])) {
            if (stat(argv[i], &sb) != 0) {
                printf("./ls: cannot access '%s': No such file or directory\n", argv[i]);
            } else {
                if (strcoll(fileType(sb), "d") == 0) {
                    appendNode(directories, strdup(argv[i]));
                } else {
                    appendNode(files, strdup(argv[i]));
                }
            }
        }
    }
}

/*
 * Function: getMaxFileLength
 * --------------------
 * given a linked list full of files, calculates the longest file length within that linked list to be used for
 * formatting purposes.
 * Note: if INodeFlag is set, we account for the general size of an inode string and increase the maxFileLength by
 * the INode amount to be accounted for in formatting
 *
 * node *head: linked list of file names
 *
 * returns: an int of the max file length found
 */
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

/*
 * Function: printLong
 * --------------------
 * given a linked list, prints each filename in long format
 * Note: if inode is set, the inode will be printed before the rest of the long string information
 *
 * node *head: linked list of files to be printed
 */
void printLong(node *head) {
    struct stat sb;
    struct passwd *password;
    struct group *grp;
    char *name = "";
    char *group = "";
    char time[255];
    while (head != NULL) {
        if (stat(head->string, &sb) == -1) {
            perror("stat in printLong");
            exit(EXIT_FAILURE);
        }
        if ((password = getpwuid(sb.st_uid)) != NULL)
            name = password->pw_name;

        if ((grp = getgrgid(sb.st_gid)) != NULL) {
            group = grp->gr_name;
        }
        char *permissionString = buildPermissionString(sb.st_mode, sb);
        strftime(time, 255, "%b %d %H:%M", localtime((&sb.st_mtime)));
        off_t fileSize = sb.st_size;
        if (printINodeFlag) {
            printf("%llu ", sb.st_ino);
        }
        printf("%s %hu %s %s %5lld %s %s\n", permissionString, sb.st_nlink, name, group, fileSize, time, head->string);
        head = head->next;
    }
}

/*
 * Function: list
 * --------------------
 * given no command arguments in the program, list prints in columns the files in a directory
 * Note: if the -d flag is set, we do not recurse and instead just get information on the . fd
 *
 *
 * DIR *dir: the dir directory object to be recursed over (if -d is not set)
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

/*
 * Function: buildFormatString
 * --------------------
 * builds a formatting string to be used for properly formatting columns
 * if the INode flag is set, we need to account for the additional length of string added by the inode number
 *
 * int maxFileLength: the max file length of the files to be printed
 *
 * returns: a formatting string to be used in printf
 */
char *buildFormatString(int maxFileLength) {
    char *format = malloc(sizeof(maxFileLength));
    sprintf(format, "%d", maxFileLength);
    char *printString = malloc(sizeof(10));
    strcpy(printString, "%-");
    strcat(printString, format);
    strcat(printString, "s ");
    return printString;
}

/*
 * Function: printINode
 * --------------------
 * whenever the printINodeFlag is set, printINode prints the INode of a given file
 *
 * char *file: the file that will have its inode printed
 */
void *printINode(char *file) {
    struct stat sb;
    if (stat(file, &sb) != 0) {
        perror("stat");
    } else {
        printf("%llu ", sb.st_ino);
    }
    return NULL;
}

/*
 * Function: printColumns
 * --------------------
 * prints a linkedlist of files in column format
 * Note: if INodeFlag is set, the inode will be printed before the filename itself
 *
 * node *head: linked list to be freed
 */
void printColumns(node *head, int maxFileLength) {
    struct winsize size;
    int ret = ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    if (ret != -1) {
        int maxColumns = size.ws_col / maxFileLength;
        int i = 1;
        while (head != NULL) {
            char *printString = buildFormatString(maxFileLength);
            if (printINodeFlag) {
                printINode(head->string);
            }
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
 * Function: listLinked
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
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    node *files = NULL;
    node *directories = NULL;

    if (getArgFlags(argc, argv)) {
        fprintf(stderr, usage(), argv[0]);
        exit(EXIT_FAILURE);
    }

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

