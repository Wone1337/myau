#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#if !defined( HUFFMAN )
#define HUFFMAN

#define LZ77_WINDOW_SIZE 4096
#define LZ77_BUFFER_SIZE 18
#define CHUNK_SIZE 8192

typedef struct HuffmanNode {
    uint8_t byte_value;
    uint64_t frequency;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

typedef struct {
    HuffmanNode **heap;
    int size;
    int capacity;
} MinHeap;

typedef struct {
    char *codes[256];
    uint8_t code_lengths[256];
    HuffmanNode *root;
    uint64_t frequencies[256];
} HuffmanTree;

// Создание узла
HuffmanNode* create_node_binary(uint8_t byte_value, uint64_t frequency);

// Функции для кучи (аналогично предыдущей версии, но с uint64_t для частот)
MinHeap* create_heap_binary(int capacity);
void heap_insert_binary(MinHeap *heap, HuffmanNode *node);
HuffmanNode* heap_extract_min_binary(MinHeap *heap);
// Подсчет частот байтов в файле
int count_file_frequencies(const char *filename, uint64_t *frequencies);
// Построение дерева Хаффмана для файла
HuffmanNode* build_huffman_tree_binary(uint64_t *frequencies);
// Генерация кодов
void generate_codes_binary(HuffmanNode *root, char *code, int depth, char **codes, uint8_t *lengths);

// Сохранение дерева в файл (упрощенная сериализация)
void save_tree_to_file(FILE *file, HuffmanNode *root); 
// Загрузка дерева из файла
HuffmanNode* load_tree_from_file(FILE *file); 
// Кодирование файла Huffman
int huffman_encode_file(const char *input_filename, const char *output_filename);
// Декодирование файла Huffman
int huffman_decode_file(const char *input_filename, const char *output_filename); 

#endif
