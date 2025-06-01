#include "../headers/lz77.h"
// Поиск совпадения для бинарных данных

// Простая контрольная сумма
uint32_t calculate_checksum(const uint8_t *data, size_t size) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum = checksum * 31 + data[i];
    }
    return checksum;
}

// Поиск совпадения
void lz77_find_match(const uint8_t *data, size_t pos, size_t data_len, 
                    int *best_offset, int *best_length) {
    *best_offset = 0;
    *best_length = 0;
    
    size_t search_start = (pos >= LZ77_WINDOW_SIZE) ? pos - LZ77_WINDOW_SIZE : 0;
    size_t lookahead_size = (data_len - pos > LZ77_BUFFER_SIZE) ? LZ77_BUFFER_SIZE : data_len - pos;
    
    for (size_t i = search_start; i < pos; i++) {
        int match_length = 0;
        
        while (match_length < lookahead_size && 
               i + match_length < pos &&
               data[i + match_length] == data[pos + match_length]) {
            match_length++;
        }
        
        // Циклические совпадения
        if (match_length > 0) {
            int extended_length = match_length;
            int pattern_len = pos - i;
            
            while (extended_length < lookahead_size &&
                   data[i + (extended_length % pattern_len)] == data[pos + extended_length]) {
                extended_length++;
            }
            
            if (extended_length > *best_length && extended_length >= 3) {
                *best_length = extended_length;
                *best_offset = pos - i;
            }
        }
    }
}

// САМОДОСТАТОЧНОЕ кодирование LZ77
int lz77_compress_file(const char *input_filename, const char *output_filename) {
    FILE *input = fopen(input_filename, "rb");
    if (!input) {
        printf("Ошибка: не удается открыть файл %s\n", input_filename);
        return -1;
    }
    
    // Получаем размер файла
    fseek(input, 0, SEEK_END);
    size_t file_size = ftell(input);
    fseek(input, 0, SEEK_SET);
    
    // Читаем весь файл
    uint8_t *data = malloc(file_size);
    if (!data || fread(data, 1, file_size, input) != file_size) {
        free(data);
        fclose(input);
        return -1;
    }
    fclose(input);
    
    // Открываем выходной файл
    FILE *output = fopen(output_filename, "wb");
    if (!output) {
        free(data);
        return -1;
    }
    
    // Подготавливаем заголовок
    LZ77_Header header;
    header.magic = LZ77_MAGIC;
    header.original_size = file_size;
    header.checksum = calculate_checksum(data, file_size);
    
    // Резервируем место для заголовка
    fwrite(&header, sizeof(LZ77_Header), 1, output);
    long compressed_start = ftell(output);
    
    // Сжимаем данные
    size_t pos = 0;
    while (pos < file_size) {
        int offset, length;
        lz77_find_match(data, pos, file_size, &offset, &length);
        
        LZ77_Token token;
        token.offset = (uint16_t)offset;
        token.length = (uint8_t)length;
        token.next_byte = (pos + length < file_size) ? data[pos + length] : 0;
        
        fwrite(&token, sizeof(LZ77_Token), 1, output);
        pos += length + 1;
    }
    
    // Обновляем размер сжатых данных в заголовке
    long compressed_end = ftell(output);
    header.compressed_size = compressed_end - compressed_start;
    
    fseek(output, 0, SEEK_SET);
    fwrite(&header, sizeof(LZ77_Header), 1, output);
    
    fclose(output);
    free(data);
    
    printf("LZ77: %zu -> %u байт (%.1f%%)\n", 
           file_size, header.compressed_size, 
           (float)header.compressed_size * 100 / file_size);
    
    return 0;
}

// САМОДОСТАТОЧНОЕ декодирование LZ77
int lz77_decompress_file(const char *input_filename, const char *output_filename) {
    FILE *input = fopen(input_filename, "rb");
    if (!input) {
        printf("Ошибка: не удается открыть файл %s\n", input_filename);
        return -1;
    }
    
    // Читаем и проверяем заголовок
    LZ77_Header header;
    if (fread(&header, sizeof(LZ77_Header), 1, input) != 1) {
        fclose(input);
        return -1;
    }
    
    if (header.magic != LZ77_MAGIC) {
        printf("Ошибка: неверный формат файла (magic: 0x%X)\n", header.magic);
        fclose(input);
        return -1;
    }
    
    // Декодируем данные
    uint8_t *result = malloc(header.original_size);
    size_t result_pos = 0;
    
    LZ77_Token token;
    while (fread(&token, sizeof(LZ77_Token), 1, input) == 1 && result_pos < header.original_size) {
        // Копируем по ссылке
        for (int i = 0; i < token.length && result_pos < header.original_size; i++) {
            result[result_pos] = result[result_pos - token.offset];
            result_pos++;
        }
        
        // Добавляем новый байт
        if (result_pos < header.original_size) {
            result[result_pos] = token.next_byte;
            result_pos++;
        }
    }
    
    // Проверяем контрольную сумму
    uint32_t checksum = calculate_checksum(result, header.original_size);
    if (checksum != header.checksum) {
        printf("Предупреждение: контрольная сумма не совпадает\n");
    }
    
    // Записываем результат
    FILE *output = fopen(output_filename, "wb");
    if (output) {
        fwrite(result, 1, header.original_size, output);
        fclose(output);
    }
    
    free(result);
    fclose(input);
    
    printf("LZ77: декомпрессия завершена (%u байт)\n", header.original_size);
    return 0;
}
