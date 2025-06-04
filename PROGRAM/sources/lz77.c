#include "../headers/lz77.h"

// Оптимизированная контрольная сумма
uint32_t calculate_checksumm(const uint8_t *data, size_t size) {
    uint32_t checksum = 0;
    const uint8_t *end = data + size;
    
    // Обрабатываем по 8 байт за раз для лучшей производительности
    while (data + 8 <= end) {
        checksum = checksum * 31 + data[0];
        checksum = checksum * 31 + data[1];
        checksum = checksum * 31 + data[2];
        checksum = checksum * 31 + data[3];
        checksum = checksum * 31 + data[4];
        checksum = checksum * 31 + data[5];
        checksum = checksum * 31 + data[6];
        checksum = checksum * 31 + data[7];
        data += 8;
    }
    
    // Обрабатываем оставшиеся байты
    while (data < end) {
        checksum = checksum * 31 + *data++;
    }
    
    return checksum;
}

// Оптимизированный поиск совпадений с хешированием
void lz77_find_match(const uint8_t *data, size_t pos, size_t data_len, 
                    int *best_offset, int *best_length) {
    *best_offset = 0;
    *best_length = 0;
    
    size_t search_start = (pos >= LZ77_WINDOW_SIZE) ? pos - LZ77_WINDOW_SIZE : 0;
    size_t lookahead_size = (data_len - pos > LZ77_BUFFER_SIZE) ? LZ77_BUFFER_SIZE : data_len - pos;
    
    if (lookahead_size < 3) return; // Минимальная длина для эффективного сжатия
    
    // Простое хеширование для быстрого поиска кандидатов
    uint32_t target_hash = 0;
    if (lookahead_size >= 3) {
        target_hash = (data[pos] << 16) | (data[pos + 1] << 8) | data[pos + 2];
    }
    
    // Ограничиваем количество проверок для ускорения
    int max_checks = 256;
    int checks = 0;
    
    for (size_t i = search_start; i < pos && checks < max_checks; i++) {
        checks++;
        
        // Быстрая проверка первых 3 байт
        if (i + 2 < pos) {
            uint32_t candidate_hash = (data[i] << 16) | (data[i + 1] << 8) | data[i + 2];
            if (candidate_hash != target_hash) continue;
        }
        
        int match_length = 0;
        size_t max_match = (pos - i < lookahead_size) ? pos - i : lookahead_size;
        
        // Оптимизированное сравнение по 4 байта
        while (match_length + 4 <= max_match && 
               i + match_length + 4 <= pos &&
               *((uint32_t*)(data + i + match_length)) == *((uint32_t*)(data + pos + match_length))) {
            match_length += 4;
        }
        
        // Дополняем побайтно
        while (match_length < max_match && 
               i + match_length < pos &&
               data[i + match_length] == data[pos + match_length]) {
            match_length++;
        }
        
        // Обработка циклических совпадений (упрощенная)
        if (match_length > 0 && match_length < lookahead_size) {
            int pattern_len = pos - i;
            while (match_length < lookahead_size &&
                   data[i + (match_length % pattern_len)] == data[pos + match_length]) {
                match_length++;
            }
        }
        
        if (match_length > *best_length && match_length >= 3) {
            *best_length = match_length;
            *best_offset = pos - i;
            
            // Ранний выход при хорошем совпадении
            if (match_length >= LZ77_BUFFER_SIZE / 2) break;
        }
    }
}

int lz77_compress_file(const char *input_filename, const char *output_filename) {
    FILE *input = fopen(input_filename, "rb");
    if (!input) {
        printf("Ошибка: не удается открыть файл %s\n", input_filename);
        return -1;
    }
    
    fseek(input, 0, SEEK_END);
    size_t file_size = ftell(input);
    fseek(input, 0, SEEK_SET);
    
    uint8_t *data = malloc(file_size);
    if (!data || fread(data, 1, file_size, input) != file_size) {
        free(data);
        fclose(input);
        return -1;
    }
    fclose(input);
    
    FILE *output = fopen(output_filename, "wb");
    if (!output) {
        free(data);
        return -1;
    }
    
    LZ77_Header header;
    header.magic = LZ77_MAGIC;
    header.original_size = file_size;
    header.checksum = calculate_checksumm(data, file_size);
    
    fwrite(&header, sizeof(LZ77_Header), 1, output);
    long compressed_start = ftell(output);
    
    // Буферизация записи для ускорения
    LZ77_Token *token_buffer = malloc(sizeof(LZ77_Token) * 8192);
    int buffer_pos = 0;
    
    size_t pos = 0;
    while (pos < file_size) {
        int offset, length;
        lz77_find_match(data, pos, file_size, &offset, &length);
        
        LZ77_Token token;
        token.offset = (uint16_t)offset;
        token.length = (uint8_t)length;
        token.next_byte = (pos + length < file_size) ? data[pos + length] : 0;
        
        token_buffer[buffer_pos++] = token;
        
        // Сбрасываем буфер при заполнении
        if (buffer_pos >= 8192) {
            fwrite(token_buffer, sizeof(LZ77_Token), buffer_pos, output);
            buffer_pos = 0;
        }
        
        pos += length + 1;
    }
    
    // Записываем оставшиеся токены
    if (buffer_pos > 0) {
        fwrite(token_buffer, sizeof(LZ77_Token), buffer_pos, output);
    }
    
    long compressed_end = ftell(output);
    header.compressed_size = compressed_end - compressed_start;
    
    fseek(output, 0, SEEK_SET);
    fwrite(&header, sizeof(LZ77_Header), 1, output);
    
    fclose(output);
    free(data);
    free(token_buffer);
    
    printf("LZ77: %zu -> %u байт (%.1f%%)\n", 
           file_size, header.compressed_size, 
           (float)header.compressed_size * 100 / file_size);
    
    return 0;
}

int lz77_decompress_file(const char *input_filename, const char *output_filename) {
    FILE *input = fopen(input_filename, "rb");
    if (!input) {
        printf("Ошибка: не удается открыть файл %s\n", input_filename);
        return -1;
    }
    
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
    
    uint8_t *result = malloc(header.original_size);
    if (!result) {
        fclose(input);
        return -1;
    }
    
    size_t result_pos = 0;
    
    // Буферизованное чтение токенов
    LZ77_Token token_buffer[4096];
    int tokens_read;
    int token_pos = 0;
    
    while ((tokens_read = fread(token_buffer, sizeof(LZ77_Token), 4096, input)) > 0) {
        for (int i = 0; i < tokens_read && result_pos < header.original_size; i++) {
            LZ77_Token *token = &token_buffer[i];
            
            // Оптимизированное копирование по ссылке
            if (token->length > 0 && token->offset > 0 && result_pos >= token->offset) {
                size_t copy_pos = result_pos - token->offset;
                
                // Копируем блоками для лучшей производительности
                for (int j = 0; j < token->length && result_pos < header.original_size; j++) {
                    result[result_pos] = result[copy_pos + (j % token->offset)];
                    result_pos++;
                }
            }
            
            // Добавляем новый байт
            if (result_pos < header.original_size) {
                result[result_pos] = token->next_byte;
                result_pos++;
            }
        }
    }
    
    // Проверяем контрольную сумму
    uint32_t checksum = calculate_checksumm(result, header.original_size);
    if (checksum != header.checksum) {
        printf("Предупреждение: контрольная сумма не совпадает\n");
    }
    
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
