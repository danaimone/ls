//
// Created by aimoned on 11/11/19.
//

#ifndef LS_LINKEDLIST_H
#define LS_LINKEDLIST_H

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

// TODO: comment function
void swapNodes(node *a, node *b);

// TODO: comment function
void printList(node *head);
#endif //LS_LINKEDLIST_H
