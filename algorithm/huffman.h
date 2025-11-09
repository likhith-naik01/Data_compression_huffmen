#ifndef HUFFMAN_H
#define HUFFMAN_H

#define MAX_TREE_HT 256
#define MAX_CHARS 256

// Huffman tree node structure
typedef struct Node {
    char data;
    unsigned frequency;
    struct Node *left, *right;
} Node;

// Priority queue node structure
typedef struct QNode {
    Node *tree_node;
    struct QNode *next;
} QNode;

// Common functions for compression and decompression
Node* createNode(char data, unsigned frequency);
QNode* createQNode(Node *tree_node);
void enqueue(QNode **head, Node *tree_node);
Node* dequeue(QNode **head);
Node* buildHuffmanTree(char *data, unsigned *freq, int size);
void generateCodes(Node *root, int *arr, int top, char **codes);

// Compression specific functions
void writeCompressedFile(const char *input_file, const char *output_file);

// Decompression specific functions
void writeDecompressedFile(const char *input_file, const char *output_file);

#endif