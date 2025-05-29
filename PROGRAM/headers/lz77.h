#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#if !defined(LZ77)
#define LZ77

#define LZ77_WINDOW_SIZE 4096
#define LZ77_BUFFER_SIZE 18
#define CHUNK_SIZE 8192

typedef struct {
    uint16_t offset;
    uint8_t length;
    uint8_t next_byte;
} LZ77_Token;

typedef struct {
    LZ77_Token *tokens;
    size_t count;
    size_t capacity;
} LZ77_Result;

void lz77_find_match_binary(const uint8_t *data, size_t pos, size_t data_len,int *best_offset, int *best_length);// Поиск совпадения для бинарных данных
int lz77_encode_file(const char *input_filename, const char *output_filename); // Кодирование файла LZ77
int lz77_decode_file(const char *input_filename, const char *output_filename); // Декодирование файла LZ77


#endif
