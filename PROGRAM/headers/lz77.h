#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#if !defined(LZ77)
#define LZ77


typedef struct {
    uint32_t magic;           // Магическое число для проверки
    uint32_t original_size;   // Исходный размер
    uint32_t compressed_size; // Размер сжатых данных
    uint32_t checksum;        // Контрольная сумма
} LZ77_Header;

#define LZ77_MAGIC 0x4C5A3737  // "LZ77"
#define LZ77_WINDOW_SIZE 4096
#define LZ77_BUFFER_SIZE 18

typedef struct {
    uint16_t offset;
    uint8_t length;
    uint8_t next_byte;
} LZ77_Token;

// Простая контрольная сумма
uint32_t calculate_checksum(const uint8_t *data, size_t size);
// Поиск совпадения
void lz77_find_match(const uint8_t *data, size_t pos, size_t data_len, int *best_offset, int *best_length); // САМОДОСТАТОЧНОЕ кодирование LZ77
int lz77_compress_file(const char *input_filename, const char *output_filename); 

// САМОДОСТАТОЧНОЕ декодирование LZ77
int lz77_decompress_file(const char *input_filename, const char *output_filename);

#endif
