#include "../headers/huffman.h"

// Создание узла
HuffmanNode* create_node_binary(uint8_t byte_value, uint64_t frequency) {
    HuffmanNode *node = malloc(sizeof(HuffmanNode));
    node->byte_value = byte_value;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Функции для кучи (аналогично предыдущей версии, но с uint64_t для частот)
MinHeap* create_heap_binary(int capacity) {
    MinHeap *heap = malloc(sizeof(MinHeap));
    heap->heap = malloc(capacity * sizeof(HuffmanNode*));
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void heap_insert_binary(MinHeap *heap, HuffmanNode *node) {
    if (heap->size >= heap->capacity) return;
    
    int i = heap->size++;
    heap->heap[i] = node;
    
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap->heap[i]->frequency >= heap->heap[parent]->frequency) break;
        
        HuffmanNode *temp = heap->heap[i];
        heap->heap[i] = heap->heap[parent];
        heap->heap[parent] = temp;
        i = parent;
    }
}

HuffmanNode* heap_extract_min_binary(MinHeap *heap) {
    if (heap->size == 0) return NULL;
    
    HuffmanNode *min = heap->heap[0];
    heap->heap[0] = heap->heap[--heap->size];
    
    int i = 0;
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;
        
        if (left < heap->size && heap->heap[left]->frequency < heap->heap[smallest]->frequency)
            smallest = left;
        if (right < heap->size && heap->heap[right]->frequency < heap->heap[smallest]->frequency)
            smallest = right;
            
        if (smallest == i) break;
        
        HuffmanNode *temp = heap->heap[i];
        heap->heap[i] = heap->heap[smallest];
        heap->heap[smallest] = temp;
        i = smallest;
    }
    
    return min;
}

// Подсчет частот байтов в файле
int count_file_frequencies(const char *filename, uint64_t *frequencies) {
    FILE *file = fopen(filename, "rb");
    if (!file) return -1;
    
    for (int i = 0; i < 256; i++) {
        frequencies[i] = 0;
    }
    
    uint8_t buffer[CHUNK_SIZE];
    size_t bytes_read;
    
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            frequencies[buffer[i]]++;
        }
    }
    
    fclose(file);
    return 0;
}

// Построение дерева Хаффмана для файла
HuffmanNode* build_huffman_tree_binary(uint64_t *frequencies) {
    MinHeap *heap = create_heap_binary(256);
    
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            HuffmanNode *node = create_node_binary(i, frequencies[i]);
            heap_insert_binary(heap, node);
        }
    }
    
    // Особый случай: только один символ
    if (heap->size == 1) {
        HuffmanNode *single = heap_extract_min_binary(heap);
        HuffmanNode *root = create_node_binary(0, single->frequency);
        root->left = single;
        free(heap->heap);
        free(heap);
        return root;
    }
    
    while (heap->size > 1) {
        HuffmanNode *left = heap_extract_min_binary(heap);
        HuffmanNode *right = heap_extract_min_binary(heap);
        
        HuffmanNode *merged = create_node_binary(0, left->frequency + right->frequency);
        merged->left = left;
        merged->right = right;
        
        heap_insert_binary(heap, merged);
    }
    
    HuffmanNode *root = heap_extract_min_binary(heap);
    free(heap->heap);
    free(heap);
    
    return root;
}

// Генерация кодов
void generate_codes_binary(HuffmanNode *root, char *code, int depth, char **codes, uint8_t *lengths) {
    if (!root) return;
    
    if (!root->left && !root->right) {
        if (depth == 0) {
            // Особый случай: только один символ
            codes[root->byte_value] = malloc(2);
            strcpy(codes[root->byte_value], "0");
            lengths[root->byte_value] = 1;
        } else {
            codes[root->byte_value] = malloc(depth + 1);
            strcpy(codes[root->byte_value], code);
            lengths[root->byte_value] = depth;
        }
        return;
    }
    
    if (root->left) {
        code[depth] = '0';
        code[depth + 1] = '\0';
        generate_codes_binary(root->left, code, depth + 1, codes, lengths);
    }
    
    if (root->right) {
        code[depth] = '1';
        code[depth + 1] = '\0';
        generate_codes_binary(root->right, code, depth + 1, codes, lengths);
    }
}

// Сохранение дерева в файл (упрощенная сериализация)
void save_tree_to_file(FILE *file, HuffmanNode *root) {
    if (!root) {
        uint8_t marker = 0;
        fwrite(&marker, 1, 1, file);
        return;
    }
    
    if (!root->left && !root->right) {
        uint8_t marker = 1;
        fwrite(&marker, 1, 1, file);
        fwrite(&root->byte_value, 1, 1, file);
    } else {
        uint8_t marker = 2;
        fwrite(&marker, 1, 1, file);
        save_tree_to_file(file, root->left);
        save_tree_to_file(file, root->right);
    }
}

// Загрузка дерева из файла
HuffmanNode* load_tree_from_file(FILE *file) {
    uint8_t marker;
    if (fread(&marker, 1, 1, file) != 1) return NULL;
    
    if (marker == 0) {
        return NULL;
    } else if (marker == 1) {
        uint8_t byte_value;
        if (fread(&byte_value, 1, 1, file) != 1) return NULL;
        return create_node_binary(byte_value, 0);
    } else if (marker == 2) {
        HuffmanNode *node = create_node_binary(0, 0);
        node->left = load_tree_from_file(file);
        node->right = load_tree_from_file(file);
        return node;
    }
    
    return NULL;
}

// Кодирование файла Huffman
int huffman_encode_file(const char *input_filename, const char *output_filename) {
    // Подсчитываем частоты
    uint64_t frequencies[256];
    if (count_file_frequencies(input_filename, frequencies) < 0) {
        return -1;
    }
    
    // Строим дерево
    HuffmanNode *root = build_huffman_tree_binary(frequencies);
    if (!root) return -1;
    
    // Генерируем коды
    char *codes[256] = {NULL};
    uint8_t lengths[256] = {0};
    char code[256] = {0};
    generate_codes_binary(root, code, 0, codes, lengths);
    
    // Открываем файлы
    FILE *input = fopen(input_filename, "rb");
    FILE *output = fopen(output_filename, "wb");
    
    if (!input || !output) {
        if (input) fclose(input);
        if (output) fclose(output);
        return -1;
    }
    
    // Получаем размер файла
    fseek(input, 0, SEEK_END);
    size_t file_size = ftell(input);
    fseek(input, 0, SEEK_SET);
    
    // Записываем заголовок
    fwrite(&file_size, sizeof(size_t), 1, output);
    save_tree_to_file(output, root);
    
    // Кодируем данные
    uint8_t bit_buffer = 0;
    int bit_count = 0;
    
    uint8_t buffer[CHUNK_SIZE];
    size_t bytes_read;
    
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, input)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            char *char_code = codes[buffer[i]];
            if (char_code) {
                for (int j = 0; char_code[j] != '\0'; j++) {
                    if (char_code[j] == '1') {
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
    }
    
    // Записываем последний байт если нужно
    if (bit_count > 0) {
        fwrite(&bit_buffer, 1, 1, output);
    }
    
    // Записываем количество значащих бит в последнем байте
    uint8_t padding = bit_count;
    fwrite(&padding, 1, 1, output);
    
    printf("Huffman: Исходный размер: %zu байт\n", file_size);
    
    // Освобождение памяти
    for (int i = 0; i < 256; i++) {
        if (codes[i]) free(codes[i]);
    }
    
    fclose(input);
    fclose(output);
    return 0;
}

// Декодирование файла Huffman
int huffman_decode_file(const char *input_filename, const char *output_filename) {
    FILE *input = fopen(input_filename, "rb");
    FILE *output = fopen(output_filename, "wb");
    
    if (!input || !output) {
        if (input) fclose(input);
        if (output) fclose(output);
        return -1;
    }
    
    // Читаем заголовок
    size_t original_size;
    if (fread(&original_size, sizeof(size_t), 1, input) != 1) {
        fclose(input);
        fclose(output);
        return -1;
    }
    
    HuffmanNode *root = load_tree_from_file(input);
    if (!root) {
        fclose(input);
        fclose(output);
        return -1;
    }
    
    // Получаем позицию данных и размер
    long data_start = ftell(input);
    fseek(input, -1, SEEK_END);
    uint8_t padding;
    fread(&padding, 1, 1, input);
    long data_end = ftell(input) - 1;
    fseek(input, data_start, SEEK_SET);
    
    // Декодируем
    HuffmanNode *current = root;
    size_t decoded_count = 0;
    
    uint8_t byte;
    long current_pos = data_start;
    
    while (current_pos < data_end && decoded_count < original_size) {
        if (fread(&byte, 1, 1, input) != 1) break;
        current_pos++;
        
        int bits_to_process = (current_pos == data_end) ? padding : 8;
        if (bits_to_process == 0) bits_to_process = 8;
        
        for (int i = 0; i < bits_to_process && decoded_count < original_size; i++) {
            int bit = (byte >> (7 - i)) & 1;
            
            if (bit == 0) {
                current = current->left;
            } else {
                current = current->right;
            }
            
            if (!current->left && !current->right) {
                fwrite(&current->byte_value, 1, 1, output);
                decoded_count++;
                current = root;
            }
        }
    }
    
    fclose(input);
    fclose(output);
    return 0;
}
