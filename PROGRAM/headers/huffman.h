#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#if !defined( HUFFMAN )
#define HUFFMAN

typedef struct {
    uint32_t magic;           // Магическое число
    uint32_t original_size;   // Исходный размер
    uint32_t tree_size;       // Размер данных дерева
    uint32_t compressed_size; // Размер сжатых данных
    uint8_t code_lengths[256]; // Длины кодов для каждого байта
} Huffman_Header;

#define HUFFMAN_MAGIC 0x48554646  // "HUFF"

void build_canonical_codes(uint8_t *code_lengths, char **codes); 
// Подсчет частот
void count_frequencies(const uint8_t *data, size_t size, uint64_t *frequencies); 
// Построение дерева и получение длин кодов (упрощенная версия)
void calculate_code_lengths(uint64_t *frequencies, uint8_t *code_lengths);
// САМОДОСТАТОЧНОЕ сжатие Huffman
int huffman_compress_file(const char *input_filename, const char *output_filename);
// САМОДОСТАТОЧНОЕ декодирование Huffman
int huffman_decompress_file(const char *input_filename, const char *output_filename); 

#endif
