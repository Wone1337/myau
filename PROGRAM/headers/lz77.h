#ifndef LZ77_H
#define LZ77_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Структура заголовка LZ77 файла
typedef struct {
    uint32_t magic;           // Магическое число для проверки
    uint32_t original_size;   // Исходный размер
    uint32_t compressed_size; // Размер сжатых данных
    uint32_t checksum;        // Контрольная сумма
} LZ77_Header;

// Структура токена LZ77
typedef struct {
    uint16_t offset;
    uint8_t length;
    uint8_t next_byte;
} LZ77_Token;

// Константы
#define LZ77_MAGIC 0x4C5A3737  // "LZ77"
#define LZ77_WINDOW_SIZE 4096
#define LZ77_BUFFER_SIZE 18

// Основные функции сжатия/декомпрессии
int lz77_compress_file(const char *input_filename, const char *output_filename);
int lz77_decompress_file(const char *input_filename, const char *output_filename);

// Вспомогательные функции
uint32_t calculate_checksumm(const uint8_t *data, size_t size);
void lz77_find_match(const uint8_t *data, size_t pos, size_t data_len, 
                    int *best_offset, int *best_length);

#endif // LZ77_H
