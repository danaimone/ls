//
// Created by aimoned on 11/11/19.
//

#ifndef LS_LINKEDLIST_H
#define LS_LINKEDLIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

typedef struct Node {
    struct Node *next;
    char *string;
} node;

/*
 * Function: createNode
 * --------------------
 * creates a new node with a null next pointer
 *
 * string: string to be stored in node
 *
 * returns: the newly created node
 */
node *createNode(char *string);

/*
 * Function: deleteNode
 * --------------------
 * given the head, deletes a node that matches the given string through pointer changes
 *
 * node **head: the pointer to the head of the node to be deleted
 * string: string to be stored in node
 */
void deleteNode(node **head, char *string);

/*
 * Function: appendNode
 * --------------------
 * appends a node to a given linked list with string information
 *
 * node **head: a pointer to a pointer of the linked list
 * fileName: string to be stored in node
 */
void appendNode(node **head, char *fileName);

/*
 * Function: copyLinkedList
 * --------------------
 * copies a LinkedList to a newly created LinkedList into a new section of memory so no pointers are affected
 *
 * node *head: head of the linked list to be copied.
 *
 * returns: the copied linked list
 */
node *copyLinkedList(node *head);

/*
 * Function: bubbleSort
 * --------------------
 * bubble sorts a given linkedList
 *
 * node *head: head of the linked list to be sorted
 */
void bubbleSort(node *head);

/*
 * Function: swapNodes
 * --------------------
 * swaps two nodes pointers
 *
 * node *a: node to be switched
 * node *b: node to be switched
 */
void swapNodes(node *a, node *b);

/*
 * Function: printList
 * --------------------
 * while a linkedList still has next, print the data stored in node
 *
 * node *head: linkedList to be iterated through and printed
 */
void printList(node *head);

/*
 * Function: freeList
 * --------------------
 * frees a linkedList from memory
 *
 * node *head: linked list to be freed
 */
void freeList(node *head);

/*
 * Function: inList
 * --------------------
 * checks if given data is in list
 *
 * node *head: linked list to be checked
 * char *data: string to be checked for
 */
bool inList(node *head, char *data);

/*
 * Function: push
 * --------------------
 * insert a node at the beginning of the given linkedList
 *
 * node *head: pointer to the entire linked list
 * char *data: new data to be inserted
 */
void push(node **headReference, char *data);

/*
 * Function: unionLinkedList
 * --------------------
 * unions two linked lists together
 *
 * node *head1: linked list to be unioned
 * node *head2: linked list to be unioned
 */
node *unionLinkedList(node *head1, node *head2);

/*
 * Function: sortedMerge
 * --------------------
 * sorts and merged two linkedLists
 *
 * node *head1: linked list to be sorted and merged with other linkedList
 * node *head2: linked list to be sorted and merged with other linkedList
 */
node *sortedMerge(node *head1, node *head2);
#endif //LS_LINKEDLIST_H
