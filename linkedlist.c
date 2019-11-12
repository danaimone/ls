//
// Created by aimoned on 11/11/19.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "linkedlist.h"

node *createNode(char *string) {
    node *temp = malloc(sizeof(struct Node));
    temp->string = string;
    temp->next = NULL;
    return temp;
}

void deleteNode(node **head, char *string) {
    node *temp = *head, *prev;
    if (temp != NULL && strcoll(temp->string, string) == 0) {
        *head = temp->next;
        free(temp);
        return;
    }

    while (temp != NULL && (strcoll(temp->string, string) != 0)) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) return;
    prev->next = temp->next;
    free(temp);
}

void appendNode(node **head, char *fileName) {
    assert(head != NULL);
    while (*head != NULL) {
        head = &((*head)->next);
    }
    *head = createNode(fileName);

}

node *copyLinkedList(node *head) {
    node *newList = NULL, **tail = &newList;
    for (; head; head = head->next) {
        *tail = malloc(sizeof **tail);
        (*tail)->string = head->string;
        (*tail)->next = NULL;
        tail = &(*tail)->next;
    }
    return newList;
}

void bubbleSort(node *head) {
    while (head != NULL) {
        node *temp = head->next;
        while (temp != NULL) {
            if (strcoll(head->string, temp->string) > 0) {
                swapNodes(temp, head);
            }
            temp = temp->next;
        }
        head = head->next;
    }
}

void swapNodes(node *a, node* b) {
    char* temp = a->string;
    a->string = b->string;
    b->string = temp;
}

void printList(node *head) {
    node *temp = head;
    while (temp != NULL) {
        printf("%s\t", temp->string);
        temp = temp -> next;
    }
}

