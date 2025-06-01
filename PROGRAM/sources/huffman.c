#include "../headers/huffman.h"

void build_canonical_codes(uint8_t *code_lengths, char **codes) {
    // Сортируем символы по длинам кодов, затем по значению
    uint16_t symbols[256];
    int symbol_count = 0;
    
    for (int i = 0; i < 256; i++) {
        if (code_lengths[i] > 0) {
            symbols[symbol_count++] = i;
        }
    }
    
    // Сортировка по длине, затем по символу
    for (int i = 0; i < symbol_count - 1; i++) {
        for (int j = i + 1; j < symbol_count; j++) {
            uint16_t a = symbols[i], b = symbols[j];
            if (code_lengths[a] > code_lengths[b] || 
                (code_lengths[a] == code_lengths[b] && a > b)) {
                symbols[i] = b;
                symbols[j] = a;
            }
        }
    }
    
    // Генерируем канонические коды
    int code = 0;
    for (int i = 0; i < symbol_count; i++) {
        uint16_t symbol = symbols[i];
        int length = code_lengths[symbol];
        
        codes[symbol] = malloc(length + 1);
        for (int j = 0; j < length; j++) {
            codes[symbol][j] = ((code >> (length - 1 - j)) & 1) ? '1' : '0';
        }
        codes[symbol][length] = '\0';
        
        code++;
        if (i + 1 < symbol_count && code_lengths[symbols[i + 1]] > length) {
            code <<= (code_lengths[symbols[i + 1]] - length);
        }
    }
}

// Подсчет частот
void count_frequencies(const uint8_t *data, size_t size, uint64_t *frequencies) {
    for (int i = 0; i < 256; i++) {
        frequencies[i] = 0;
    }
    for (size_t i = 0; i < size; i++) {
        frequencies[data[i]]++;
    }
}

// Построение дерева и получение длин кодов (упрощенная версия)
void calculate_code_lengths(uint64_t *frequencies, uint8_t *code_lengths) {
    // Инициализация
    for (int i = 0; i < 256; i++) {
        code_lengths[i] = 0;
    }
    
    // Простой алгоритм: длина обратно пропорциональна частоте
    uint64_t max_freq = 0;
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > max_freq) {
            max_freq = frequencies[i];
        }
    }
    
    if (max_freq == 0) return;
    
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            // Простая формула для длины кода
            int length = 1;
            uint64_t freq_ratio = max_freq / frequencies[i];
            while (freq_ratio > 1 && length < 15) {
                length++;
                freq_ratio >>= 1;
            }
            code_lengths[i] = length;
        }
    }
}

// САМОДОСТАТОЧНОЕ сжатие Huffman
int huffman_compress_file(const char *input_filename, const char *output_filename) {
    FILE *input = fopen(input_filename, "rb");
    if (!input) return -1;
    
    // Читаем файл
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
    
    // Подсчитываем частоты и строим коды
    uint64_t frequencies[256];
    count_frequencies(data, file_size, frequencies);
    
    Huffman_Header header;
    header.magic = HUFFMAN_MAGIC;
    header.original_size = file_size;
    calculate_code_lengths(frequencies, header.code_lengths);
    
    char *codes[256] = {NULL};
    build_canonical_codes(header.code_lengths, codes);
    
    // Открываем выходной файл
    FILE *output = fopen(output_filename, "wb");
    if (!output) {
        free(data);
        return -1;
    }
    
    // Записываем заголовок
    fwrite(&header, sizeof(Huffman_Header), 1, output);
    long compressed_start = ftell(output);
    
    // Кодируем данные
    uint8_t bit_buffer = 0;
    int bit_count = 0;
    
    for (size_t i = 0; i < file_size; i++) {
        char *code = codes[data[i]];
        if (code) {
            for (int j = 0; code[j] != '\0'; j++) {
                if (code[j] == '1') {
                    bit_buffer |= (1 << (7 - bit_count));
                }
                bit_count++;
                
                if (bit_count == 8) {
                    fwrite(&bit_buffer, 1, 1, output);
                    bit_buffer = 0;
                    bit_count = 0;
                }
            }
        }
    }
    
    // Записываем последний байт
    if (bit_count > 0) {
        fwrite(&bit_buffer, 1, 1, output);
    }
    
    // Обновляем размер
    long compressed_end = ftell(output);
    header.compressed_size = compressed_end - compressed_start;
    
    fseek(output, 0, SEEK_SET);
    fwrite(&header, sizeof(Huffman_Header), 1, output);
    
    fclose(output);
    free(data);
    
    // Освобождение кодов
    for (int i = 0; i < 256; i++) {
        if (codes[i]) free(codes[i]);
    }
    
    printf("Huffman: %zu -> %u байт (%.1f%%)\n", 
           file_size, header.compressed_size,
           (float)header.compressed_size * 100 / file_size);
    
    return 0;
}

// САМОДОСТАТОЧНОЕ декодирование Huffman
int huffman_decompress_file(const char *input_filename, const char *output_filename) {
    FILE *input = fopen(input_filename, "rb");
    if (!input) return -1;
    
    // Читаем заголовок
    Huffman_Header header;
    if (fread(&header, sizeof(Huffman_Header), 1, input) != 1) {
        fclose(input);
        return -1;
    }
    
    if (header.magic != HUFFMAN_MAGIC) {
        printf("Ошибка: неверный формат Huffman файла\n");
        fclose(input);
        return -1;
    }
    
    // Строим коды из длин
    char *codes[256] = {NULL};
    build_canonical_codes(header.code_lengths, codes);
    
    // Создаем обратную таблицу (код -> символ)
    // Упрощенный декодер - читаем бит за битом
    uint8_t *result = malloc(header.original_size);
    size_t decoded = 0;
    
    char current_code[16] = {0};
    int code_pos = 0;
    
    uint8_t byte;
    while (fread(&byte, 1, 1, input) == 1 && decoded < header.original_size) {
        for (int bit = 7; bit >= 0 && decoded < header.original_size; bit--) {
            current_code[code_pos++] = ((byte >> bit) & 1) ? '1' : '0';
            current_code[code_pos] = '\0';
            
            // Ищем совпадение
            for (int i = 0; i < 256; i++) {
                if (codes[i] && strcmp(codes[i], current_code) == 0) {
                    result[decoded++] = i;
                    code_pos = 0;
                    current_code[0] = '\0';
                    break;
                }
            }
            
            if (code_pos >= 15) { // Защита от переполнения
                code_pos = 0;
                current_code[0] = '\0';
            }
        }
    }
    
    // Записываем результат
    FILE *output = fopen(output_filename, "wb");
    if (output) {
        fwrite(result, 1, decoded, output);
        fclose(output);
    }
    
    // Освобождение памяти
    for (int i = 0; i < 256; i++) {
        if (codes[i]) free(codes[i]);
    }
    free(result);
    fclose(input);
    
    printf("Huffman: декомпрессия завершена (%zu байт)\n", decoded);
    return 0;
}


