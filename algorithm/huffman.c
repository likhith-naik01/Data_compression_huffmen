#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

// Create a new Huffman tree node
Node* createNode(char data, unsigned frequency) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node) {
        node->data = data;
        node->frequency = frequency;
        node->left = node->right = NULL;
    }
    return node;
}

// Create a new queue node
QNode* createQNode(Node *tree_node) {
    QNode* node = (QNode*)malloc(sizeof(QNode));
    if (node) {
        node->tree_node = tree_node;
        node->next = NULL;
    }
    return node;
}

// Add a node to priority queue (sorted by frequency)
void enqueue(QNode **head, Node *tree_node) {
    QNode* new_node = createQNode(tree_node);
    if (!new_node) return;
    
    if (!*head || tree_node->frequency <= (*head)->tree_node->frequency) {
        new_node->next = *head;
        *head = new_node;
        return;
    }
    
    QNode *current = *head;
    while (current->next && current->next->tree_node->frequency < tree_node->frequency) {
        current = current->next;
    }
    
    new_node->next = current->next;
    current->next = new_node;
}

// Remove node from priority queue
Node* dequeue(QNode **head) {
    if (!*head) return NULL;
    
    QNode *temp = *head;
    Node *node = temp->tree_node;
    *head = temp->next;
    free(temp);
    return node;
}

// Build Huffman tree
Node* buildHuffmanTree(char *data, unsigned *freq, int size) {
    QNode *queue = NULL;
    
    // Create initial queue with all characters
    for (int i = 0; i < size; i++) {
        Node* node = createNode(data[i], freq[i]);
        if (!node) continue;
        enqueue(&queue, node);
    }
    
    // Build tree by combining nodes
    while (queue && queue->next) {
        Node *left = dequeue(&queue);
        Node *right = dequeue(&queue);
        
        Node *internal = createNode('\0', left->frequency + right->frequency);
        if (!internal) {
            free(left);
            free(right);
            continue;
        }
        
        internal->left = left;
        internal->right = right;
        enqueue(&queue, internal);
    }
    
    return dequeue(&queue);
}

// Generate Huffman codes recursively
void generateCodes(Node *root, int *arr, int top, char **codes) {
    if (!root) return;

    if (root->left) {
        arr[top] = 0;
        generateCodes(root->left, arr, top + 1, codes);
    }
    
    if (root->right) {
        arr[top] = 1;
        generateCodes(root->right, arr, top + 1, codes);
    }
    
    if (!root->left && !root->right) {
        codes[root->data] = (char*)malloc(top + 1);
        if (codes[root->data]) {
            for (int i = 0; i < top; i++) {
                codes[root->data][i] = arr[i] + '0';
            }
            codes[root->data][top] = '\0';
        }
    }
}

// Write compressed output
void writeCompressedFile(const char *input_file, const char *output_file) {
    printf("Starting compression...\n");
    printf("Input file: %s\n", input_file);
    printf("Output file: %s\n", output_file);
    
    // Count frequencies first
    unsigned freq[MAX_CHARS] = {0};
    char *codes[MAX_CHARS] = {NULL};
    FILE *f = fopen(input_file, "r");
    if (!f) {
        printf("Error opening input file: %s\n", input_file);
        perror("Reason");
        return;
    }

    int c;
    while ((c = fgetc(f)) != EOF) {
        freq[c]++;
    }
    fclose(f);

    // Build Huffman tree
    char data[MAX_CHARS];
    unsigned freqs[MAX_CHARS];
    int count = 0;

    for (int i = 0; i < MAX_CHARS; i++) {
        if (freq[i] > 0) {
            data[count] = i;
            freqs[count] = freq[i];
            count++;
        }
    }

    if (count == 0) {
        printf("Error: Empty input file\n");
        return;
    }

    printf("Building Huffman tree with %d unique characters\n", count);
    Node *root = buildHuffmanTree(data, freqs, count);
    if (!root) {
        printf("Error building Huffman tree\n");
        return;
    }
    printf("Huffman tree built successfully\n");

    // Generate Huffman codes
    int arr[MAX_TREE_HT];
    generateCodes(root, arr, 0, codes);

    // Write compressed data
    FILE *in = fopen(input_file, "r");
    FILE *out = fopen(output_file, "wb");
    
    if (!in || !out) {
        printf("Error opening files\n");
        if (in) fclose(in);
        if (out) fclose(out);
        return;
    }

    // Write frequency table
    fwrite(freq, sizeof(unsigned), MAX_CHARS, out);
    
    // Write bit count for padding info
    unsigned char bit_count = 0;
    fwrite(&bit_count, 1, 1, out);
    long bit_count_pos = ftell(out);
    
    unsigned char buf = 0;
    bit_count = 0;
    
    // Write compressed data
    while ((c = fgetc(in)) != EOF) {
        char *code = codes[c];
        if (!code) continue;
        
        for (int i = 0; code[i]; i++) {
            buf = (buf << 1) | (code[i] - '0');
            bit_count++;
            
            if (bit_count == 8) {
                fwrite(&buf, 1, 1, out);
                buf = 0;
                bit_count = 0;
            }
        }
    }
    
    // Write remaining bits
    if (bit_count > 0) {
        buf = buf << (8 - bit_count);
        fwrite(&buf, 1, 1, out);
    }
    
    // Update the final bit count
    fseek(out, bit_count_pos - 1, SEEK_SET);
    fwrite(&bit_count, 1, 1, out);
    
    // Cleanup
    fclose(in);
    fclose(out);
    for (int i = 0; i < MAX_CHARS; i++) {
        if (codes[i]) free(codes[i]);
    }
}

// Read and decompress file
void writeDecompressedFile(const char *input_file, const char *output_file) {
    FILE *in = fopen(input_file, "rb");
    FILE *out = fopen(output_file, "w");
    
    if (!in || !out) {
        printf("Error opening files\n");
        if (in) fclose(in);
        if (out) fclose(out);
        return;
    }
    
    // Read frequency table
    unsigned freq[MAX_CHARS] = {0};
    if (fread(freq, sizeof(unsigned), MAX_CHARS, in) != MAX_CHARS) {
        printf("Error reading frequency table\n");
        fclose(in);
        fclose(out);
        return;
    }
    
    // Read padding info
    unsigned char padding;
    if (fread(&padding, 1, 1, in) != 1) {
        printf("Error reading padding info\n");
        fclose(in);
        fclose(out);
        return;
    }
    
    // Rebuild Huffman tree
    char data[MAX_CHARS];
    unsigned freqs[MAX_CHARS];
    int count = 0;
    
    for (int i = 0; i < MAX_CHARS; i++) {
        if (freq[i] > 0) {
            data[count] = i;
            freqs[count] = freq[i];
            count++;
        }
    }
    
    if (count == 0) {
        printf("Error: No frequency data\n");
        fclose(in);
        fclose(out);
        return;
    }
    
    Node *root = buildHuffmanTree(data, freqs, count);
    if (!root) {
        printf("Error building Huffman tree\n");
        fclose(in);
        fclose(out);
        return;
    }
    
    // Get file size for proper padding handling
    long file_size;
    long current_pos = ftell(in);
    fseek(in, 0, SEEK_END);
    file_size = ftell(in);
    fseek(in, current_pos, SEEK_SET);
    
    // Read and decode compressed data
    Node *current = root;
    unsigned char buf;
    long bytes_read = 0;
    
    while (fread(&buf, 1, 1, in) == 1) {
        bytes_read++;
        int bits = (bytes_read == file_size - current_pos) ? padding : 8;
        
        for (int i = 7; i >= 8 - bits; i--) {
            int bit = (buf >> i) & 1;
            current = bit ? current->right : current->left;
            
            if (!current->left && !current->right) {
                fprintf(out, "%c", current->data);
                current = root;
            }
        }
    }
    
    // Cleanup
    fclose(in);
    fclose(out);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s [-c|-d] input_file output_file\n", argv[0]);
        return 1;
    }
    
    if (strcmp(argv[1], "-c") == 0) {
        writeCompressedFile(argv[2], argv[3]);
    } else if (strcmp(argv[1], "-d") == 0) {
        writeDecompressedFile(argv[2], argv[3]);
    } else {
        printf("Invalid option. Use -c for compression or -d for decompression.\n");
        return 1;
    }
    
    return 0;
}