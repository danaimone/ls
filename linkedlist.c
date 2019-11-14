//
// Created by aimoned on 11/11/19.
//

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
    if (head != NULL) {
        node *temp = head;
        while (temp != NULL) {
            printf("%s\t", temp->string);
            temp = temp->next;
        }
    } else {
        printf("printList error: function was called, but the given list did not have anything to print.");
    }
}

void freeList(node *head) {
    node *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

bool inList(node *head, char *data) {
    node *temp = head;
    while (temp != NULL) {
        if (strcoll(temp->string, data)) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

void push(node **headReference, char *data) {
    node *newNode = (node*) malloc(sizeof(node));
    newNode->string = data;
    newNode->next = (*headReference);
    (*headReference) = newNode;
}

node *unionLinkedList(node *head1, node *head2) {
    node *result = NULL;
    node *t1 = head1, *t2 = head2;

    while (t1 != NULL) {
        push(&result, t1->string);
        t1 = t1->next;
    }

    while (t2 != NULL) {
        if (!inList(result, t2->string)) {
            push(&result, t2->string);
        }
        t2 = t2->next;
    }

    return result;
}

node *sortedMerge(node *head1, node *head2) {
    node *result = NULL;

    if (head1 == NULL) {
        return (head2);
    } else if (head2 == NULL) {
        return (head1);
    }

    if (strcoll(head1->string, head2->string) <= 0) {
        result = head1;
        result->next = sortedMerge(head1->next, head2);
    } else {
        result = head2;
        result->next = sortedMerge(head1, head2->next);
    }
    return (result);
}
