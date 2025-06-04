#include "../headers/huffman.h"

// Оптимизированная таблица декодирования
static DecodeEntry decode_table[65536]; // 2^16 для быстрого поиска

void build_decode_table(uint8_t *code_lengths) {
    // Инициализация таблицы
    for (int i = 0; i < 65536; i++) {
        decode_table[i].symbol = 0xFFFF;
        decode_table[i].length = 0;
    }
    
    // Строим канонические коды
    uint16_t symbols[256];
    int symbol_count = 0;
    
    for (int i = 0; i < 256; i++) {
        if (code_lengths[i] > 0) {
            symbols[symbol_count++] = i;
        }
    }
    
    // Сортировка
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
    
    // Заполняем таблицу декодирования
    int code = 0;
    for (int i = 0; i < symbol_count; i++) {
        uint16_t symbol = symbols[i];
        int length = code_lengths[symbol];
        
        if (length <= 16) { // Ограничиваем длину для таблицы
            // Заполняем все возможные префиксы
            int prefix_count = 1 << (16 - length);
            uint16_t base_code = code << (16 - length);
            
            for (int j = 0; j < prefix_count; j++) {
                decode_table[base_code + j].symbol = symbol;
                decode_table[base_code + j].length = length;
            }
        }
        
        code++;
        if (i + 1 < symbol_count && code_lengths[symbols[i + 1]] > length) {
            code <<= (code_lengths[symbols[i + 1]] - length);
        }
    }
}

void build_canonical_codes(uint8_t *code_lengths, char **codes) {
    uint16_t symbols[256];
    int symbol_count = 0;
    
    for (int i = 0; i < 256; i++) {
        if (code_lengths[i] > 0) {
            symbols[symbol_count++] = i;
        }
    }
    
    // Сортировка
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

void count_frequencies(const uint8_t *data, size_t size, uint64_t *frequencies) {
    // Обнуляем счетчики
    memset(frequencies, 0, 256 * sizeof(uint64_t));
    
    // Оптимизированный подсчет по блокам
    const uint8_t *end = data + size;
    while (data + 8 <= end) {
        frequencies[data[0]]++;
        frequencies[data[1]]++;
        frequencies[data[2]]++;
        frequencies[data[3]]++;
        frequencies[data[4]]++;
        frequencies[data[5]]++;
        frequencies[data[6]]++;
        frequencies[data[7]]++;
        data += 8;
    }
    
    // Оставшиеся байты
    while (data < end) {
        frequencies[*data++]++;
    }
}

void calculate_code_lengths(uint64_t *frequencies, uint8_t *code_lengths) {
    memset(code_lengths, 0, 256);
    
    uint64_t max_freq = 0;
    int unique_symbols = 0;
    
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            unique_symbols++;
            if (frequencies[i] > max_freq) {
                max_freq = frequencies[i];
            }
        }
    }
    
    if (max_freq == 0) return;
    
    // Улучшенный алгоритм расчета длин
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            if (unique_symbols == 1) {
                code_lengths[i] = 1; // Единственный символ
            } else {
                // Логарифмический расчет
                double prob = (double)frequencies[i] / max_freq;
                int length = 1;
                while (prob < 0.5 && length < 15) {
                    length++;
                    prob *= 2;
                }
                code_lengths[i] = length;
            }
        }
    }
}

int huffman_compress_file(const char *input_filename, const char *output_filename) {
    FILE *input = fopen(input_filename, "rb");
    if (!input) return -1;
    
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
    
    uint64_t frequencies[256];
    count_frequencies(data, file_size, frequencies);
    
    Huffman_Header header;
    header.magic = HUFFMAN_MAGIC;
    header.original_size = file_size;
    calculate_code_lengths(frequencies, header.code_lengths);
    
    char *codes[256] = {NULL};
    build_canonical_codes(header.code_lengths, codes);
    
    FILE *output = fopen(output_filename, "wb");
    if (!output) {
        free(data);
        return -1;
    }
    
    fwrite(&header, sizeof(Huffman_Header), 1, output);
    long compressed_start = ftell(output);
    
    // Буферизованное кодирование
    uint8_t *bit_buffer = malloc(65536);
    size_t buffer_pos = 0;
    uint8_t current_byte = 0;
    int bit_count = 0;
    
    for (size_t i = 0; i < file_size; i++) {
        char *code = codes[data[i]];
        if (code) {
            for (int j = 0; code[j] != '\0'; j++) {
                if (code[j] == '1') {
                    current_byte |= (1 << (7 - bit_count));
                }
                bit_count++;
                
                if (bit_count == 8) {
                    bit_buffer[buffer_pos++] = current_byte;
                    current_byte = 0;
                    bit_count = 0;
                    
                    // Сброс буфера при заполнении
                    if (buffer_pos >= 65536) {
                        fwrite(bit_buffer, 1, buffer_pos, output);
                        buffer_pos = 0;
                    }
                }
            }
        }
    }
    
    // Записываем последний байт
    if (bit_count > 0) {
        bit_buffer[buffer_pos++] = current_byte;
    }
    
    // Записываем оставшиеся данные
    if (buffer_pos > 0) {
        fwrite(bit_buffer, 1, buffer_pos, output);
    }
    
    long compressed_end = ftell(output);
    header.compressed_size = compressed_end - compressed_start;
    
    fseek(output, 0, SEEK_SET);
    fwrite(&header, sizeof(Huffman_Header), 1, output);
    
    fclose(output);
    free(data);
    free(bit_buffer);
    
    for (int i = 0; i < 256; i++) {
        if (codes[i]) free(codes[i]);
    }
    
    printf("Huffman: %zu -> %u байт (%.1f%%)\n", 
           file_size, header.compressed_size,
           (float)header.compressed_size * 100 / file_size);
    
    return 0;
}

int huffman_decompress_file(const char *input_filename, const char *output_filename) {
    FILE *input = fopen(input_filename, "rb");
    if (!input) return -1;

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

    // Строим таблицу декодирования
    build_decode_table(header.code_lengths);

    FILE *output = fopen(output_filename, "wb");
    if (!output) {
        fclose(input);
        return -1;
    }

    uint8_t current_byte = 0;
    int bit_position = 0;
    uint16_t bit_buffer = 0;
    int bits_in_buffer = 0;

    size_t decompressed_size = 0;

    while (fread(&current_byte, 1, 1, input) == 1) {
        for (bit_position = 7; bit_position >= 0; bit_position--) {
            bit_buffer = (bit_buffer << 1) | ((current_byte >> bit_position) & 1);
            bits_in_buffer++;

            if (bits_in_buffer >= 16) {
                DecodeEntry entry = decode_table[bit_buffer >> (bits_in_buffer - 16)];
                if (entry.length > 0) {
                    fputc(entry.symbol, output);
                    decompressed_size++;
                    bits_in_buffer -= entry.length;
                    bit_buffer &= (1 << bits_in_buffer) - 1; // Убираем использованные биты
                }
            }
        }
    }

    // Проверяем оставшиеся биты
    while (bits_in_buffer > 0) {
        DecodeEntry entry = decode_table[bit_buffer << (16 - bits_in_buffer)];
        if (entry.length > 0 && entry.length <= bits_in_buffer) {
            fputc(entry.symbol, output);
            decompressed_size++;
            bits_in_buffer -= entry.length;
            bit_buffer &= (1 << bits_in_buffer) - 1;
        } else {
            break; // Остались лишние биты
        }
    }

    fclose(input);
    fclose(output);

    printf("Huffman: декомпрессия завершена (%zu байт)\n", decompressed_size);
    return 0;
}
