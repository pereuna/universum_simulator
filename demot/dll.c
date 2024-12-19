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
#include <limits.h>

// Define the CollisionPair structure
typedef struct CollisionPair {
    int value;              // Value of the pair
    struct CollisionPair* prev;      // Pointer to the previous pair
    struct CollisionPair* next_pair; // Pointer to the next pair
} CollisionPair;

// Define the DoublyLinkedList structure
typedef struct DoublyLinkedList {
    CollisionPair* head; // Pointer to the head of the list
    CollisionPair* tail; // Pointer to the tail of the list
} DoublyLinkedList;

// Function to create a new pair
CollisionPair* create_pair(int value) {
    CollisionPair* new_pair = (CollisionPair*)malloc(sizeof(CollisionPair)); // Allocate memory for the new pair
    new_pair->value = value;                      // Set the value of the new pair
    new_pair->prev = NULL;                        // Initialize the previous pointer to NULL
    new_pair->next_pair = NULL;                   // Initialize the next pointer to NULL
    return new_pair;                              // Return the new pair
}

// Function to find the middle pair between start and end
CollisionPair* find_middle(CollisionPair* start, CollisionPair* end) {
    CollisionPair* slow = start; // Slow pointer
    CollisionPair* fast = start; // Fast pointer
    // Move fast pointer two steps and slow pointer one step until fast reaches the end
    while (fast != end && fast->next_pair != end) {
        slow = slow->next_pair;
        fast = fast->next_pair->next_pair;
    }
    return slow; // Return the middle pair
}

// Function to insert a new pair in the sorted list using binary search
void binary_search_insert(DoublyLinkedList* list, CollisionPair* new_pair) {
    // If the list is empty, set the new pair as the head and tail
    if (list->head == NULL) {
        list->head = list->tail = new_pair;
        return;
    }

    CollisionPair* start = list->head; // Start pointer
    CollisionPair* end = NULL;         // End pointer

    // Perform binary search to find the correct position for the new pair
    while (start != end) {
        CollisionPair* mid = find_middle(start, end); // Find the middle pair
        if (mid->value < new_pair->value) {
            start = mid->next_pair; // Move start to the right half
        } else {
            end = mid; // Move end to the left half
        }
    }

    // Insert the new pair in the correct position
    if (start == list->head && new_pair->value < list->head->value) {
        new_pair->next_pair = list->head;
        list->head->prev = new_pair;
        list->head = new_pair;
    } else if (start == NULL) {
        list->tail->next_pair = new_pair;
        new_pair->prev = list->tail;
        list->tail = new_pair;
    } else {
        CollisionPair* prev_pair = start->prev;
        prev_pair->next_pair = new_pair;
        new_pair->prev = prev_pair;
        new_pair->next_pair = start;
        start->prev = new_pair;
    }
}

// Function to print the values in the list
void print_list(DoublyLinkedList* list) {
    CollisionPair* current = list->head; // Start from the head
    while (current != NULL) {
        printf("%d ", current->value); // Print the value of the current pair
        current = current->next_pair;  // Move to the next pair
    }
    printf("\n");
}

void free_list(DoublyLinkedList* list) {
    CollisionPair* current = list->head;
    CollisionPair* next;

    while (current != NULL) {
        next = current->next_pair;
        free(current);
        current = next;
    }

    list->head = NULL;
    list->tail = NULL;
}

int main() {
    DoublyLinkedList list = {NULL, NULL}; // Initialize an empty list

    // Insert pairs into the list
    binary_search_insert(&list, create_pair(10));
    binary_search_insert(&list, create_pair(5));
    binary_search_insert(&list, create_pair(15));
    binary_search_insert(&list, create_pair(7));
    binary_search_insert(&list, create_pair(8));
    binary_search_insert(&list, create_pair(10));
    binary_search_insert(&list, create_pair(-7));
    binary_search_insert(&list, create_pair(0));
    binary_search_insert(&list, create_pair(INT_MAX));

    // Print the list
    print_list(&list);

    // Free the list
    free_list(&list);

    return 0;
}

