/*
 * Doubly Linked List Implementation
 * Author: Petteri Reunamo
 * Creation Date: September 11, 2024
 * Version: 1.0
 *
 * This program implements a sorted doubly linked list with efficient insertion
 * using a binary search-like approach.
 */

#include <stdio.h>
#include <stdlib.h>

// Define the Node structure
typedef struct Node {
    int value;              // Value of the node
    struct Node* prev;      // Pointer to the previous node
    struct Node* next_node; // Pointer to the next node
} Node;

// Define the DoublyLinkedList structure
typedef struct DoublyLinkedList {
    Node* head; // Pointer to the head of the list
    Node* tail; // Pointer to the tail of the list
} DoublyLinkedList;

// Function to create a new node
Node* create_node(int value) {
    Node* new_node = (Node*)malloc(sizeof(Node)); // Allocate memory for the new node
    new_node->value = value;                      // Set the value of the new node
    new_node->prev = NULL;                        // Initialize the previous pointer to NULL
    new_node->next_node = NULL;                   // Initialize the next pointer to NULL
    return new_node;                              // Return the new node
}

// Function to find the middle node between start and end
Node* find_middle(Node* start, Node* end) {
    Node* slow = start; // Slow pointer
    Node* fast = start; // Fast pointer
    // Move fast pointer two steps and slow pointer one step until fast reaches the end
    while (fast != end && fast->next_node != end) {
        slow = slow->next_node;
        fast = fast->next_node->next_node;
    }
    return slow; // Return the middle node
}

// Function to insert a new node in the sorted list using binary search
void binary_search_insert(DoublyLinkedList* list, Node* new_node) {
    // If the list is empty, set the new node as the head and tail
    if (list->head == NULL) {
        list->head = list->tail = new_node;
        return;
    }

    Node* start = list->head; // Start pointer
    Node* end = NULL;         // End pointer

    // Perform binary search to find the correct position for the new node
    while (start != end) {
        Node* mid = find_middle(start, end); // Find the middle node
        if (mid->value < new_node->value) {
            start = mid->next_node; // Move start to the right half
        } else {
            end = mid; // Move end to the left half
        }
    }

    // Insert the new node in the correct position
    if (start == list->head && new_node->value < list->head->value) {
        new_node->next_node = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    } else if (start == NULL) {
        list->tail->next_node = new_node;
        new_node->prev = list->tail;
        list->tail = new_node;
    } else {
        Node* prev_node = start->prev;
        prev_node->next_node = new_node;
        new_node->prev = prev_node;
        new_node->next_node = start;
        start->prev = new_node;
    }
}

// Function to print the values in the list
void print_list(DoublyLinkedList* list) {
    Node* current = list->head; // Start from the head
    while (current != NULL) {
        printf("%d ", current->value); // Print the value of the current node
        current = current->next_node;  // Move to the next node
    }
    printf("\n");
}

int main() {
    DoublyLinkedList list = {NULL, NULL}; // Initialize an empty list

    // Insert nodes into the list
    binary_search_insert(&list, create_node(10));
    binary_search_insert(&list, create_node(5));
    binary_search_insert(&list, create_node(15));
    binary_search_insert(&list, create_node(7));

    // Print the list
    print_list(&list);

    return 0;
}

