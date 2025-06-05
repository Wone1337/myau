#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>


typedef struct {
    uint32_t magic;
    uint64_t original_size;
    uint64_t compressed_size;
    uint8_t code_lengths[256];
} Huffman_Header;


typedef struct {
    uint16_t symbol;
    uint8_t length;
} DecodeEntry;


#define HUFFMAN_MAGIC 0x454E4441
#define MAX_FAST_BITS 8


int huffman_compress_file(const char *input_filename, const char *output_filename);
int huffman_decompress_file(const char *input_filename, const char *output_filename);
void count_frequencies(const uint8_t *data, size_t size, uint64_t *frequencies);
void calculate_code_lengths(uint64_t *frequencies, uint8_t *code_lengths);
void build_canonical_codes(uint8_t *code_lengths, char **codes);
void build_decode_table(uint8_t *code_lengths);

#endif 
