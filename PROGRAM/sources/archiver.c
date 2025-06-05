#include "../headers/archiver.h"

static uint32_t crc32_table[256];
static int crc32_initialized = 0;

//void print_hex_dump(const uint8_t* data, size_t size, const char* label) 
//{
//    printf("=== %s ===\n", label);
//    printf("Размер: %zu байт\n", size);
//    printf("Первые 64 байта (или весь файл, если меньше):\n");
//    
//    size_t bytes_to_show = (size < 64) ? size : 64;
//    for (size_t i = 0; i < bytes_to_show; i++) {
//        if (i % 16 == 0) printf("%04zx: ", i);
//        printf("%02x ", data[i]);
//        if ((i + 1) % 16 == 0) printf("\n");
//    }
//    if (bytes_to_show % 16 != 0) printf("\n");
//    
//    if (size > 64) {
//        printf("... (показаны первые 64 из %zu байт)\n", size);
//        printf("Последние 32 байта:\n");
//        size_t start = size - 32;
//        for (size_t i = start; i < size; i++) {
//            if ((i - start) % 16 == 0) printf("%04zx: ", i);
//            printf("%02x ", data[i]);
//            if ((i - start + 1) % 16 == 0) printf("\n");
//        }
//        if ((size - start) % 16 != 0) printf("\n");
//    }
//    
//    uint32_t crc = calculate_crc32(data, size);
//    printf("CRC32: 0x%08X\n", crc);
//    printf("================\n\n");
//}
//

//uint8_t* compress_data_debug(const uint8_t* data, size_t size, size_t* compressed_size) 
//{
//    printf("\n=== ДИАГНОСТИКА СЖАТИЯ ===\n");
//    
//    // Показываем исходные данные
//    print_hex_dump(data, size, "ИСХОДНЫЕ ДАННЫЕ");
//    
//    if (!ensure_temp_directory()) {
//        return NULL;
//    }
//
//    char temp_input[MAX_PATH_LENGTH];
//    char temp_mid[MAX_PATH_LENGTH];
//    char temp_output[MAX_PATH_LENGTH];
//    
//    snprintf(temp_input, sizeof(temp_input), "%stemp_compress_input_%d_%ld.tmp", 
//             DEF_PATH_OUTPUT_TMP, getpid(), time(NULL));
//    snprintf(temp_mid, sizeof(temp_mid), "%stemp_compress_mid_%d_%ld.tmp", 
//             DEF_PATH_OUTPUT_TMP, getpid(), time(NULL));
//    snprintf(temp_output, sizeof(temp_output), "%stemp_compress_output_%d_%ld.tmp", 
//             DEF_PATH_OUTPUT_TMP, getpid(), time(NULL));
//
//    
//    if (!write_memory_to_file(temp_input, data, size)) {
//        printf("Ошибка записи исходных данных\n");
//        return NULL;
//    }
//
//    
//    size_t verify_size;
//    uint8_t* verify_data = read_file_to_memory(temp_input, &verify_size);
//    if (verify_data) {
//        printf("Проверка записи исходных данных:\n");
//        printf("  Размер: %zu (ожидалось %zu)\n", verify_size, size);
//        uint32_t verify_crc = calculate_crc32(verify_data, verify_size);
//        uint32_t original_crc = calculate_crc32(data, size);
//        printf("  CRC32: 0x%08X (ожидалось 0x%08X) - %s\n", 
//               verify_crc, original_crc, 
//               (verify_crc == original_crc) ? "OK" : "ОШИБКА");
//        free(verify_data);
//    }
//
//    // Этап 1: LZ77 сжатие
//    printf("\n--- ЭТАП 1: LZ77 СЖАТИЕ ---\n");
//    if (lz77_compress_file(temp_input, temp_mid) != 0) {
//        printf("Ошибка на этапе LZ77 сжатия\n");
//        remove(temp_input);
//        return NULL;
//    }
//
//    // Проверяем результат LZ77
//    size_t lz77_size;
//    uint8_t* lz77_data = read_file_to_memory(temp_mid, &lz77_size);
//    if (lz77_data) {
//        print_hex_dump(lz77_data, lz77_size, "ПОСЛЕ LZ77");
//        free(lz77_data);
//    }
//
//    // Этап 2: Huffman сжатие
//    printf("--- ЭТАП 2: HUFFMAN СЖАТИЕ ---\n");
//    if (huffman_compress_file(temp_mid, temp_output) != 0) {
//        printf("Ошибка на этапе Huffman сжатия\n");
//        remove(temp_input);
//        remove(temp_mid);
//        return NULL;
//    }
//
//    // Читаем финальный результат
//    uint8_t* result = read_file_to_memory(temp_output, compressed_size);
//    if (result) {
//        print_hex_dump(result, *compressed_size, "ФИНАЛЬНЫЙ РЕЗУЛЬТАТ");
//    }
//
//    // Очистка временных файлов
//    remove(temp_input);
//    remove(temp_mid);
//    remove(temp_output);
//
//    printf("=== КОНЕЦ ДИАГНОСТИКИ СЖАТИЯ ===\n\n");
//    return result;
//}
//
//uint8_t* decompress_data_debug(const uint8_t* compressed_data, size_t compressed_size, size_t* original_size) 
//{
//    printf("\n=== ДИАГНОСТИКА ДЕКОМПРЕССИИ ===\n");
//    
//    // Показываем сжатые данные
//    print_hex_dump(compressed_data, compressed_size, "СЖАТЫЕ ДАННЫЕ");
//    
//    if (!ensure_temp_directory()) {
//        return NULL;
//    }
//
//    char temp_huffman[MAX_PATH_LENGTH];
//    char temp_lz77[MAX_PATH_LENGTH];
//    char temp_output[MAX_PATH_LENGTH];
//    
//    snprintf(temp_huffman, sizeof(temp_huffman), "%stemp_decompress_huffman_%d_%ld.tmp", 
//             DEF_PATH_OUTPUT_TMP, getpid(), time(NULL));
//    snprintf(temp_lz77, sizeof(temp_lz77), "%stemp_decompress_lz77_%d_%ld.tmp", 
//             DEF_PATH_OUTPUT_TMP, getpid(), time(NULL));
//    snprintf(temp_output, sizeof(temp_output), "%stemp_decompress_output_%d_%ld.tmp", 
//             DEF_PATH_OUTPUT_TMP, getpid(), time(NULL));
//

//    if (!write_memory_to_file(temp_huffman, compressed_data, compressed_size)) {
//        printf("Ошибка записи сжатых данных\n");
//        return NULL;
//    }
//printf("--- ЭТАП 1: HUFFMAN ДЕКОМПРЕССИЯ ---\n");
//if (huffman_decompress_file(temp_huffman, temp_lz77) != 0) {
//    printf("Ошибка Huffman декомпрессии\n");
//    remove(temp_huffman);
//    return NULL;
//}
//

//size_t huffman_result_size;
//uint8_t* huffman_result = read_file_to_memory(temp_lz77, &huffman_result_size);
//if (huffman_result) {
//    uint32_t huffman_crc = calculate_crc32(huffman_result, huffman_result_size);
//    printf("CRC32 после HUFFMAN: 0x%08X\n", huffman_crc);
//    print_hex_dump(huffman_result, huffman_result_size, "ПОСЛЕ HUFFMAN ДЕКОМПРЕССИИ");
//    free(huffman_result);
//}
//
//printf("--- ЭТАП 2: LZ77 ДЕКОМПРЕССИЯ ---\n");
//if (lz77_decompress_file(temp_lz77, temp_output) != 0) {
//    printf("Ошибка LZ77 декомпрессии\n");
//    remove(temp_huffman);
//    remove(temp_lz77);
//    return NULL;
//}
//
//uint8_t* result = read_file_to_memory(temp_output, original_size);
//if (result) {
//    uint32_t final_crc = calculate_crc32(result, *original_size);
//    printf("CRC32 после LZ77: 0x%08X\n", final_crc);
//    print_hex_dump(result, *original_size, "ФИНАЛЬНЫЙ РЕЗУЛЬТАТ ДЕКОМПРЕССИИ");
//}
//	remove(temp_huffman);
//    	remove(temp_lz77);
//    	remove(temp_output);
//
//    printf("=== КОНЕЦ ДИАГНОСТИКИ ДЕКОМПРЕССИИ ===\n\n");
//    return result;
//}
//
//int test_compression_pipeline(const char* test_file) {
//    printf("=== ТЕСТ PIPELINE СЖАТИЯ-ДЕКОМПРЕССИИ ===\n");
//    printf("Тестовый файл: %s\n", test_file);
//    
//    size_t original_size;
//    uint8_t* original_data = read_file_to_memory(test_file, &original_size);
//    if (!original_data) {
//        printf("Ошибка чтения тестового файла\n");
//        return 0;
//    }
//    
//    uint32_t original_crc = calculate_crc32(original_data, original_size);
//    printf("Исходный файл: %zu байт, CRC32=0x%08X\n", original_size, original_crc);
//    
//    size_t compressed_size;
//    uint8_t* compressed_data = compress_data_debug(original_data, original_size, &compressed_size);
//    if (!compressed_data) {
//        printf("Ошибка сжатия\n");
//        free(original_data);
//        return 0;
//    }
//    
//    printf("Сжатые данные: %zu байт\n", compressed_size);
//    
//    size_t decompressed_size;
//    uint8_t* decompressed_data = decompress_data_debug(compressed_data, compressed_size, &decompressed_size);
//    if (!decompressed_data) {
//        printf("Ошибка декомпрессии\n");
//        free(original_data);
//        free(compressed_data);
//        return 0;
//    }
//    
//    uint32_t decompressed_crc = calculate_crc32(decompressed_data, decompressed_size);
//    printf("Декомпрессированные данные: %zu байт, CRC32=0x%08X\n", decompressed_size, decompressed_crc);
//    
//    
//    printf("\n=== СРАВНЕНИЕ РЕЗУЛЬТАТОВ ===\n");
//    printf("Размер: %zu -> %zu (%s)\n", original_size, decompressed_size, 
//           (original_size == decompressed_size) ? "OK" : "ОШИБКА");
//    printf("CRC32: 0x%08X -> 0x%08X (%s)\n", original_crc, decompressed_crc,
//           (original_crc == decompressed_crc) ? "OK" : "ОШИБКА");
//    
//    if (original_size == decompressed_size && original_crc == decompressed_crc) {
//        printf("ТЕСТ ПРОЙДЕН УСПЕШНО!\n");
//        
//        
//        int bytes_differ = 0;
//        for (size_t i = 0; i < original_size; i++) {
//            if (original_data[i] != decompressed_data[i]) {
//                if (bytes_differ < 10) { // Показываем первые 10 различий
//                    printf("Различие в позиции %zu: 0x%02X != 0x%02X\n", 
//                           i, original_data[i], decompressed_data[i]);
//                }
//                bytes_differ++;
//            }
//        }
//        if (bytes_differ > 0) {
//            printf("Обнаружено различий в байтах: %d\n", bytes_differ);
//        }
//    } else {
//        printf("ТЕСТ НЕ ПРОЙДЕН!\n");
//    }
//    
//    free(original_data);
//    free(compressed_data);
//    free(decompressed_data);
//    
//    return (original_size == decompressed_size && original_crc == decompressed_crc) ? 1 : 0;
//}




void init_crc32(void) {
    if (crc32_initialized) return;
    for (int i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 8; j > 0; j--) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
        crc32_table[i] = crc;
    }
    crc32_initialized = 1;
}

uint32_t calculate_crc32(const uint8_t* data, size_t size) {
    init_crc32();
    uint32_t crc = 0xFFFFFFFF;
    
    
    const uint8_t* end = data + size;
    while (data + 8 <= end) {
        crc = crc32_table[(crc ^ data[0]) & 0xFF] ^ (crc >> 8);
        crc = crc32_table[(crc ^ data[1]) & 0xFF] ^ (crc >> 8);
        crc = crc32_table[(crc ^ data[2]) & 0xFF] ^ (crc >> 8);
        crc = crc32_table[(crc ^ data[3]) & 0xFF] ^ (crc >> 8);
        crc = crc32_table[(crc ^ data[4]) & 0xFF] ^ (crc >> 8);
        crc = crc32_table[(crc ^ data[5]) & 0xFF] ^ (crc >> 8);
        crc = crc32_table[(crc ^ data[6]) & 0xFF] ^ (crc >> 8);
        crc = crc32_table[(crc ^ data[7]) & 0xFF] ^ (crc >> 8);
        data += 8;
    }
    
   
    while (data < end) {
        crc = crc32_table[(crc ^ *data++) & 0xFF] ^ (crc >> 8);
    }
    
    return crc ^ 0xFFFFFFFF;
}

int ensure_temp_directory(void) {
    struct stat st = {0};
    if (stat(DEF_PATH_OUTPUT_TMP, &st) == -1) {
        if (mkdir(DEF_PATH_OUTPUT_TMP, 0755) != 0) {
            printf("Ошибка: не удается создать временную папку %s\n", DEF_PATH_OUTPUT_TMP);
            return 0;
        }
    }
    return 1;
}


uint8_t* read_file_to_memory(const char* filename, size_t* size) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Ошибка: не удается открыть файл %s (%s)\n", filename, strerror(errno));
        return NULL;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        printf("Ошибка: не удается получить размер файла %s\n", filename);
        close(fd);
        return NULL;
    }

    *size = st.st_size;
    
    if (*size == 0) {
        printf("Предупреждение: файл %s пустой\n", filename);
        close(fd);
        *size = 0;
        return malloc(1);
    }

    uint8_t* data;
    
   
    if (*size > 100 * 1024 * 1024) {
        data = mmap(NULL, *size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (data == MAP_FAILED) {
            printf("Ошибка: не удается создать memory mapping для файла %s\n", filename);
            close(fd);
            return NULL;
        }
        
        
	uint8_t* result = malloc(*size);
        if (!result) {
            munmap(data, *size);
            close(fd);
            return NULL;
        }
        
        memcpy(result, data, *size);
        munmap(data, *size);
        close(fd);
        return result;
    } else {
        
	    data = malloc(*size);
        if (!data) {
            printf("Ошибка: не удается выделить память для файла %s (размер: %zu)\n", filename, *size);
            close(fd);
            return NULL;
        }

        ssize_t total_read = 0;
        size_t chunk_size = 1024 * 1024; 
        
        while (total_read < *size) {
            size_t to_read = (*size - total_read < chunk_size) ? *size - total_read : chunk_size;
            ssize_t read_count = read(fd, data + total_read, to_read);
            
            if (read_count <= 0) {
                if (read_count == 0) break; 
                printf("Ошибка чтения файла %s\n", filename);
                free(data);
                close(fd);
                return NULL;
            }
            total_read += read_count;
        }

        close(fd);
        
        if (total_read != *size) {
            printf("Предупреждение: прочитано %zd из %zu байт файла %s\n", 
                   total_read, *size, filename);
            *size = total_read;
	    data = realloc(data, *size);
        }

        return data;
    }
}


int write_memory_to_file(const char* filename, const uint8_t* data, size_t size) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        printf("Ошибка: не удается создать файл %s (%s)\n", filename, strerror(errno));
        return 0;
    }

    if (size == 0) {
        close(fd);
        return 1;
    }

    ssize_t total_written = 0;
    size_t chunk_size = 1024 * 1024; 
    
    while (total_written < size) {
        size_t to_write = (size - total_written < chunk_size) ? size - total_written : chunk_size;
        ssize_t written = write(fd, data + total_written, to_write);
        
        if (written <= 0) {
            printf("Ошибка записи в файл %s\n", filename);
            close(fd);
            return 0;
        }
        total_written += written;
    }

    close(fd);

    if (total_written != size) {
        printf("Ошибка: записано только %zd из %zu байт в файл %s\n",
               total_written, size, filename);
        return 0;
    }

    return 1;
}

void create_directory_recursive(const char* path) {
    char* path_copy = strdup(path);
    char* dir_end = strrchr(path_copy, '/');
    
    if (dir_end) {
        *dir_end = '\0';
        
        char* p = path_copy + 1;
        while (*p) {
            if (*p == '/') {
                *p = '\0';
                mkdir(path_copy, 0755);
                *p = '/';
            }
            p++;
        }
        mkdir(path_copy, 0755);
    }

    free(path_copy);
}

char* get_full_path(const char* filename) {
    char* full_path = malloc(MAX_PATH_LENGTH);
    if (!full_path) return NULL;
    
    if (filename[0] == '/') {
        strncpy(full_path, filename, MAX_PATH_LENGTH - 1);
    } else {
        if (getcwd(full_path, MAX_PATH_LENGTH) == NULL) {
            free(full_path);
            return NULL;
        }
        strncat(full_path, "/", MAX_PATH_LENGTH - strlen(full_path) - 1);
        strncat(full_path, filename, MAX_PATH_LENGTH - strlen(full_path) - 1);
    }
    
    full_path[MAX_PATH_LENGTH - 1] = '\0';
    return full_path;
}

char* get_filename_from_path(const char* path) {
    char* path_copy = strdup(path);
    char* filename = basename(path_copy);
    char* result = strdup(filename);
    free(path_copy);
    return result;
}

uint8_t* run_compression_pipeline(const uint8_t* data, size_t size, size_t* compressed_size,int (*first_stage)(const char*, const char*),int (*second_stage)(const char*, const char*)) {
    
    if (!ensure_temp_directory()) {
        return NULL;
    }

    char temp_input[MAX_PATH_LENGTH];
    char temp_mid[MAX_PATH_LENGTH];
    char temp_output[MAX_PATH_LENGTH];
    
    snprintf(temp_input, sizeof(temp_input), "%stemp_compress_input_%d_%ld.tmp", 
             DEF_PATH_OUTPUT_TMP, getpid(), time(NULL));
    snprintf(temp_mid, sizeof(temp_mid), "%stemp_compress_mid_%d_%ld.tmp", 
             DEF_PATH_OUTPUT_TMP, getpid(), time(NULL));
    snprintf(temp_output, sizeof(temp_output), "%stemp_compress_output_%d_%ld.tmp", 
             DEF_PATH_OUTPUT_TMP, getpid(), time(NULL));

      if (!write_memory_to_file(temp_input, data, size)) {
        return NULL;
    }

    if (first_stage(temp_input, temp_mid) != 0) {
        printf("Ошибка на этапе LZ77 сжатия\n");
        remove(temp_input);
        return NULL;
    }


    struct stat st;
    if (stat(temp_mid, &st) != 0) {
        printf("Ошибка: промежуточный файл не создан\n");
        remove(temp_input);
        return NULL;
    }

    if (second_stage(temp_mid, temp_output) != 0) {
        printf("Ошибка на этапе Huffman сжатия\n");
        remove(temp_input);
        remove(temp_mid);
        return NULL;
    }
    uint8_t* result = read_file_to_memory(temp_output, compressed_size);

    remove(temp_input);
    remove(temp_mid);
    remove(temp_output);

    return result;
}

uint8_t* compress_data(const uint8_t* data, size_t size, size_t* compressed_size) {
    return run_compression_pipeline(data, size, compressed_size, lz77_compress_file, huffman_compress_file);
}

uint8_t* decompress_data(const uint8_t* compressed_data, size_t compressed_size, size_t* original_size) {
    if (!ensure_temp_directory()) {
        return NULL;
    }

    char temp_huffman[MAX_PATH_LENGTH];
    char temp_lz77[MAX_PATH_LENGTH];
    char temp_output[MAX_PATH_LENGTH];
    
    snprintf(temp_huffman, sizeof(temp_huffman), "%stemp_decompress_huffman_%d_%ld.tmp", 
             DEF_PATH_OUTPUT_TMP, getpid(), time(NULL));
    snprintf(temp_lz77, sizeof(temp_lz77), "%stemp_decompress_lz77_%d_%ld.tmp", 
             DEF_PATH_OUTPUT_TMP, getpid(), time(NULL));
    snprintf(temp_output, sizeof(temp_output), "%stemp_decompress_output_%d_%ld.tmp", 
             DEF_PATH_OUTPUT_TMP, getpid(), time(NULL));

    if (!write_memory_to_file(temp_huffman, compressed_data, compressed_size)) {
        return NULL;
    }


    if ( huffman_decompress_file(temp_huffman, temp_lz77)!= 0) {
        printf("Ошибка Huffman декомпрессии\n");
        remove(temp_huffman);
        return NULL;
    }


    if (lz77_decompress_file(temp_lz77, temp_output) != 0) {
        printf("Ошибка LZ77 декомпрессии\n");
        remove(temp_huffman);
        remove(temp_lz77);
        return NULL;
    }

    uint8_t* result = read_file_to_memory(temp_output, original_size);

    remove(temp_huffman);
    remove(temp_lz77);
    remove(temp_output);

    return result;
}

void cleanup_and_exit(char** full_paths, FileEntryFixed* files, int file_count, FILE* archive_fp) {
    if (full_paths) {
        for (int i = 0; i < file_count; i++) {
            if (full_paths[i]) {
                free(full_paths[i]);
            }
        }
        free(full_paths);
    }
    
    if (files) {
        free(files);
    }
    
    if (archive_fp) {
        fclose(archive_fp);
    }
}

uint32_t calculate_checksum(const uint8_t* data, size_t size) {
    return calculate_crc32(data, size);
}


int extract_archive(const char* archive_name, const char* output_dir) {
    printf("Извлечение архива %s в папку %s...\n", archive_name, output_dir);

    FILE* archive_fp = fopen(archive_name, "rb");
    if (!archive_fp) {
        printf("Ошибка: не удается открыть архив %s (%s)\n", archive_name, strerror(errno));
        return 0;
    }

    setvbuf(archive_fp, NULL, _IOFBF, 1024 * 1024);

    ArchiveHeader header;
    if (fread(&header, sizeof(ArchiveHeader), 1, archive_fp) != 1 ||
        memcmp(header.signature, ARCHIVE_SIGNATURE, 8) != 0) {
        printf("Ошибка: неверная подпись архива\n");
        fclose(archive_fp);
        return 0;
    }

    if (header.file_count == 0) {
        printf("Архив пустой\n");
        fclose(archive_fp);
        return 1;
    }

    FileEntry* files = malloc(header.file_count * sizeof(FileEntry));
    if (!files) {
        printf("Ошибка: не удается выделить память для таблицы файлов\n");
        fclose(archive_fp);
        return 0;
    }

    for (uint32_t i = 0; i < header.file_count; i++) {
        if (fread(&files[i].name_length, sizeof(uint16_t), 1, archive_fp) != 1) {
            printf("Ошибка чтения длины имени файла %u\n", i);
            goto cleanup;
        }
        
        files[i].filename = malloc(files[i].name_length + 1);
        if (!files[i].filename) {
            printf("Ошибка выделения памяти для имени файла %u\n", i);
            goto cleanup;
        }
        
        if (fread(files[i].filename, 1, files[i].name_length, archive_fp) != files[i].name_length) {
            printf("Ошибка чтения имени файла %u\n", i);
            free(files[i].filename);
            goto cleanup;
        }
        files[i].filename[files[i].name_length] = '\0';
        
        if (fread(&files[i].original_size, sizeof(uint64_t), 1, archive_fp) != 1 ||
            fread(&files[i].compressed_size, sizeof(uint64_t), 1, archive_fp) != 1 ||
            fread(&files[i].data_offset, sizeof(uint64_t), 1, archive_fp) != 1 ||
            fread(&files[i].crc32, sizeof(uint32_t), 1, archive_fp) != 1) {
            printf("Ошибка чтения метаданных файла %u\n", i);
            free(files[i].filename);
            goto cleanup;
        }
        
        printf("Файл %d: %s, исходный=%lu, сжатый=%lu, CRC32=0x%08X\n", 
               i, files[i].filename, files[i].original_size, 
               files[i].compressed_size, files[i].crc32);
    }

    mkdir(output_dir, 0755);

    for (uint32_t i = 0; i < header.file_count; i++) {
        printf("Извлечение файла %s (%lu байт)... ", files[i].filename, files[i].original_size);
        fflush(stdout);

        if (fseek(archive_fp, files[i].data_offset, SEEK_SET) != 0) {
            printf("Ошибка позиционирования в архиве\n");
            continue;
        }

        uint8_t* compressed_data = malloc(files[i].compressed_size);
        if (!compressed_data) {
            printf("Ошибка выделения памяти для сжатых данных\n");
            continue;
        }

        if (fread(compressed_data, 1, files[i].compressed_size, archive_fp) != files[i].compressed_size) {
            printf("Ошибка чтения сжатых данных\n");
            free(compressed_data);
            continue;
        }

        printf("\n  -> Декомпрессия %lu байт... ", files[i].compressed_size);
        fflush(stdout);

        size_t decompressed_size;
        uint8_t* decompressed_data = decompress_data(compressed_data, files[i].compressed_size, &decompressed_size);
        free(compressed_data);

        if (!decompressed_data) {
            printf("Ошибка декомпрессии\n");
            continue;
        }

        printf("получено %zu байт\n", decompressed_size);

        if (decompressed_size != files[i].original_size) {
            printf("  -> Ошибка: неверный размер после декомпрессии (%zu != %lu)\n", 
                   decompressed_size, files[i].original_size);
            free(decompressed_data);
            continue;
        }

        printf("  -> Проверка CRC32... ");
        uint32_t calculated_crc = calculate_crc32(decompressed_data, decompressed_size);
        printf("ожидается 0x%08X, получено 0x%08X... ", files[i].crc32, calculated_crc);
        
        if (calculated_crc != files[i].crc32) {
            printf("ОШИБКА\n");
            printf("     Контрольные суммы не совпадают!\n");
            
            printf("     Первые 32 байта данных: ");
            for (int j = 0; j < 32 && j < decompressed_size; j++) {
                printf("%02X ", decompressed_data[j]);
            }
            printf("\n");
            
            free(decompressed_data);
            continue;
        }
        printf("OK\n");

        char full_path[MAX_PATH_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", output_dir, files[i].filename);
        create_directory_recursive(full_path);

        printf("  -> Запись в файл %s... ", full_path);
        if (write_memory_to_file(full_path, decompressed_data, decompressed_size)) {
            printf("OK\n");
        } else {
            printf("Ошибка записи\n");
        }

        free(decompressed_data);
    }

cleanup:
    for (uint32_t i = 0; i < header.file_count; i++) {
        if (files[i].filename) free(files[i].filename);
    }
    free(files);
    fclose(archive_fp);

    printf("Архив успешно извлечен!\n");
    return 1;
}

int list_archive(const char* archive_name) {
    FILE* archive_fp = fopen(archive_name, "rb");
    if (!archive_fp) {
        printf("Ошибка: не удается открыть архив %s (%s)\n", archive_name, strerror(errno));
        return 0;
    }

    ArchiveHeader header;
    if (fread(&header, sizeof(ArchiveHeader), 1, archive_fp) != 1 ||
        memcmp(header.signature, ARCHIVE_SIGNATURE, 8) != 0) {
        printf("Ошибка: неверная подпись архива\n");
        fclose(archive_fp);
        return 0;
    }

    printf("Содержимое архива %s:\n", archive_name);
    printf("%-40s %12s %12s %8s %10s\n", "Имя файла", "Исходный", "Сжатый", "Степень", "CRC32");
    printf("--------------------------------------------------------------------------------\n");

    uint64_t total_original = 0, total_compressed = 0;
    for (uint32_t i = 0; i < header.file_count; i++) {
        uint16_t name_length;
        if (fread(&name_length, sizeof(uint16_t), 1, archive_fp) != 1) {
            printf("Ошибка чтения длины имени файла %u\n", i);
            break;
        }

        char* filename = malloc(name_length + 1);
        if (!filename || fread(filename, 1, name_length, archive_fp) != name_length) {
            printf("Ошибка чтения имени файла %u\n", i);
            if (filename) free(filename);
            break;
        }
        filename[name_length] = '\0';

        uint64_t original_size, compressed_size, data_offset;
        uint32_t crc32;
        if (fread(&original_size, sizeof(uint64_t), 1, archive_fp) != 1 ||
            fread(&compressed_size, sizeof(uint64_t), 1, archive_fp) != 1 ||
            fread(&data_offset, sizeof(uint64_t), 1, archive_fp) != 1 ||
            fread(&crc32, sizeof(uint32_t), 1, archive_fp) != 1) {
            printf("Ошибка чтения метаданных файла %u\n", i);
            free(filename);
            break;
        }

        float ratio = original_size > 0 ? (float)compressed_size / original_size * 100 : 0;
        printf("%-40s %12lu %12lu %7.1f%% %08X\n", filename, original_size, compressed_size, ratio, crc32);

        total_original += original_size;
        total_compressed += compressed_size;
        free(filename);
    }

    float total_ratio = total_original > 0 ? (float)total_compressed / total_original * 100 : 0;
    printf("--------------------------------------------------------------------------------\n");
    printf("%-40s %12lu %12lu %7.1f%%\n", "ИТОГО:", total_original, total_compressed, total_ratio);
    fclose(archive_fp);
    return 1;
}

int create_archive(const char* archive_name, char** filenames, int file_count) {
    printf("Создание архива %s с %d файлами...\n", archive_name, file_count);
		
//if (!test_compression_pipeline("/home/Bobik/Изображения/wallpaper/jinx-arcane-netflix-series-lol-hd-wallpaper-uhdpaper.com-268@0@e.jpg")) {
//    printf("Pipeline сжатия работает некорректно!\n");
//    return 0;
//}

    FILE* archive_fp = fopen(archive_name, "wb");
    if (!archive_fp) {
        printf("Ошибка: не удается создать архив %s (%s)\n", archive_name, strerror(errno));
        return 0;
    }

    setvbuf(archive_fp, NULL, _IOFBF, 1024 * 1024);

    ArchiveHeader header;
    memcpy(header.signature, ARCHIVE_SIGNATURE, 8);
    header.file_count = file_count;
    header.table_size = 0;

    FileEntryFixed* files = calloc(file_count, sizeof(FileEntryFixed));
    if (!files) {
        printf("Ошибка: не удается выделить память для таблицы файлов\n");
        fclose(archive_fp);
        return 0;
    }

    char** full_paths = calloc(file_count, sizeof(char*));
    if (!full_paths) {
        printf("Ошибка: не удается выделить память для путей файлов\n");
        free(files);
        fclose(archive_fp);
        return 0;
    }

    uint32_t table_size = 0;
    for (int i = 0; i < file_count; i++) {
        full_paths[i] = get_full_path(filenames[i]);
        if (!full_paths[i]) {
            printf("Ошибка: не удается получить полный путь для %s\n", filenames[i]);
            cleanup_and_exit(full_paths, files, file_count, archive_fp);
            return 0;
        }

        struct stat st;
        if (stat(full_paths[i], &st) != 0) {
            printf("Ошибка: не удается получить информацию о файле %s (%s)\n", 
                   full_paths[i], strerror(errno));
            cleanup_and_exit(full_paths, files, file_count, archive_fp);
            return 0;
        }
        
        files[i].original_size = st.st_size;

        char* base_name = get_filename_from_path(filenames[i]);
        strncpy(files[i].filename, base_name, MAX_FILENAME_LENGTH - 1);
        files[i].filename[MAX_FILENAME_LENGTH - 1] = '\0';
        free(base_name);

        files[i].compressed_size = 0;
        files[i].file_offset = 0;
        files[i].checksum = 0;

        table_size += sizeof(uint16_t) + strlen(files[i].filename) + 
                     sizeof(uint64_t) * 3 + sizeof(uint32_t);
    }

    header.table_size = table_size;

    if (fwrite(&header, sizeof(ArchiveHeader), 1, archive_fp) != 1) {
        printf("Ошибка записи заголовка архива\n");
        cleanup_and_exit(full_paths, files, file_count, archive_fp);
        return 0;
    }

    size_t table_start = ftell(archive_fp);
    fseek(archive_fp, table_size, SEEK_CUR);

    for (int i = 0; i < file_count; i++) {
        printf("Обработка файла %d/%d: %s (%lu байт)\n", 
               i + 1, file_count, files[i].filename, files[i].original_size);
        fflush(stdout);
        
        files[i].file_offset = ftell(archive_fp);

        size_t file_size;
        uint8_t* file_data = read_file_to_memory(full_paths[i], &file_size);
        if (!file_data) {
            printf("Ошибка чтения файла %s\n", full_paths[i]);
            cleanup_and_exit(full_paths, files, file_count, archive_fp);
            return 0;
        }

         files[i].checksum = calculate_crc32(file_data, file_size);
        printf("  -> CRC32 исходного файла: 0x%08X\n", files[i].checksum);

        size_t compressed_size;
        uint8_t* compressed_data = compress_data(file_data, file_size, &compressed_size);
        
        free(file_data);

        if (!compressed_data) {
            printf("Ошибка сжатия файла %s\n", full_paths[i]);
            cleanup_and_exit(full_paths, files, file_count, archive_fp);
            return 0;
        }

        files[i].compressed_size = compressed_size;

        if (fwrite(compressed_data, 1, compressed_size, archive_fp) != compressed_size) {
            printf("Ошибка записи сжатых данных файла %s\n", full_paths[i]);
            free(compressed_data);
            cleanup_and_exit(full_paths, files, file_count, archive_fp);
            return 0;
        }

        free(compressed_data);
        
        float ratio = file_size > 0 ? (float)compressed_size * 100 / file_size : 0;
        printf("  -> Сжато: %lu байт (%.1f%%)\n", compressed_size, ratio);
    }
    if (fseek(archive_fp, table_start, SEEK_SET) != 0) {
        printf("Ошибка позиционирования для записи таблицы файлов\n");
        cleanup_and_exit(full_paths, files, file_count, archive_fp);
        return 0;
    }

    for (int i = 0; i < file_count; i++) {
        uint16_t name_length = strlen(files[i].filename);
        
        if (fwrite(&name_length, sizeof(uint16_t), 1, archive_fp) != 1 ||
            fwrite(files[i].filename, 1, name_length, archive_fp) != name_length ||
            fwrite(&files[i].original_size, sizeof(uint64_t), 1, archive_fp) != 1 ||
            fwrite(&files[i].compressed_size, sizeof(uint64_t), 1, archive_fp) != 1 ||
            fwrite(&files[i].file_offset, sizeof(uint64_t), 1, archive_fp) != 1 ||
            fwrite(&files[i].checksum, sizeof(uint32_t), 1, archive_fp) != 1) {
            printf("Ошибка записи метаданных файла %s\n", files[i].filename);
            cleanup_and_exit(full_paths, files, file_count, archive_fp);
            return 0;
        }
    }

    cleanup_and_exit(full_paths, files, file_count, archive_fp);

    printf("Архив %s успешно создан!\n", archive_name);
    
    uint64_t total_original = 0, total_compressed = 0;
    for (int i = 0; i < file_count; i++) {
        total_original += files[i].original_size;
        total_compressed += files[i].compressed_size;
    }
    
    float total_ratio = total_original > 0 ? (float)total_compressed * 100 / total_original : 0;
    printf("Статистика сжатия:\n");
    printf("  Исходный размер: %lu байт\n", total_original);
    printf("  Сжатый размер: %lu байт\n", total_compressed);
    printf("  Степень сжатия: %.1f%%\n", total_ratio);
    
    if (total_compressed < total_original) {
        printf("  Экономия: %lu байт (%.1f%%)\n", 
               total_original - total_compressed, 
               100.0 - total_ratio);
    } else {
        printf("  Увеличение: %lu байт (+%.1f%%)\n", 
               total_compressed - total_original, 
               total_ratio - 100.0);
    }

    return 1;
}

int test_archive(const char* archive_name) {
    printf("Проверка целостности архива %s...\n", archive_name);

    FILE* archive_fp = fopen(archive_name, "rb");
    if (!archive_fp) {
        printf("Ошибка: не удается открыть архив %s (%s)\n", archive_name, strerror(errno));
        return 0;
    }

    ArchiveHeader header;
    if (fread(&header, sizeof(ArchiveHeader), 1, archive_fp) != 1 ||
        memcmp(header.signature, ARCHIVE_SIGNATURE, 8) != 0) {
        printf("Ошибка: неверная подпись архива\n");
        fclose(archive_fp);
        return 0;
    }

    printf("Найдено файлов в архиве: %u\n", header.file_count);

    if (header.file_count == 0) {
        printf("Архив пустой\n");
        fclose(archive_fp);
        return 1;
    }

    FileEntry* files = malloc(header.file_count * sizeof(FileEntry));
    if (!files) {
        printf("Ошибка: не удается выделить память для таблицы файлов\n");
        fclose(archive_fp);
        return 0;
    }

    for (uint32_t i = 0; i < header.file_count; i++) {
        if (fread(&files[i].name_length, sizeof(uint16_t), 1, archive_fp) != 1) {
            printf("Ошибка чтения длины имени файла %u\n", i);
            goto cleanup_test;
        }
        
        files[i].filename = malloc(files[i].name_length + 1);
        if (!files[i].filename) {
            printf("Ошибка выделения памяти для имени файла %u\n", i);
            goto cleanup_test;
        }
        
        if (fread(files[i].filename, 1, files[i].name_length, archive_fp) != files[i].name_length) {
            printf("Ошибка чтения имени файла %u\n", i);
            free(files[i].filename);
            goto cleanup_test;
        }
        files[i].filename[files[i].name_length] = '\0';
        
        if (fread(&files[i].original_size, sizeof(uint64_t), 1, archive_fp) != 1 ||
            fread(&files[i].compressed_size, sizeof(uint64_t), 1, archive_fp) != 1 ||
            fread(&files[i].data_offset, sizeof(uint64_t), 1, archive_fp) != 1 ||
            fread(&files[i].crc32, sizeof(uint32_t), 1, archive_fp) != 1) {
            printf("Ошибка чтения метаданных файла %u\n", i);
            free(files[i].filename);
            goto cleanup_test;
        }
    }
    int errors = 0;
    for (uint32_t i = 0; i < header.file_count; i++) {
        printf("Проверка файла %s... ", files[i].filename);
        fflush(stdout);

        if (fseek(archive_fp, files[i].data_offset, SEEK_SET) != 0) {
            printf("Ошибка позиционирования\n");
            errors++;
            continue;
        }

        uint8_t* compressed_data = malloc(files[i].compressed_size);
        if (!compressed_data) {
            printf("Ошибка выделения памяти\n");
            errors++;
            continue;
        }

        if (fread(compressed_data, 1, files[i].compressed_size, archive_fp) != files[i].compressed_size) {
            printf("Ошибка чтения данных\n");
            free(compressed_data);
            errors++;
            continue;
        }

        size_t decompressed_size;
        uint8_t* decompressed_data = decompress_data(compressed_data, files[i].compressed_size, &decompressed_size);
        free(compressed_data);

        if (!decompressed_data) {
            printf("Ошибка декомпрессии\n");
            errors++;
            continue;
        }

        if (decompressed_size != files[i].original_size) {
            printf("Ошибка размера (%zu != %lu)\n", decompressed_size, files[i].original_size);
            free(decompressed_data);
            errors++;
            continue;
        }

        uint32_t calculated_crc = calculate_crc32(decompressed_data, decompressed_size);
        if (calculated_crc != files[i].crc32) {
            printf("Ошибка контрольной суммы (0x%08X != 0x%08X)\n", calculated_crc, files[i].crc32);
            free(decompressed_data);
            errors++;
            continue;
        }

        free(decompressed_data);
        printf("OK\n");
    }

cleanup_test:
    for (uint32_t i = 0; i < header.file_count; i++) {
        if (files[i].filename) free(files[i].filename);
    }
    free(files);
    fclose(archive_fp);

    if (errors == 0) {
        printf("Архив прошел проверку целостности!\n");
        return 1;
    } else {
        printf("Обнаружено ошибок: %d\n", errors);
        return 0;
    }
}

void print_file_info(const char* filename) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        printf("Ошибка получения информации о файле %s: %s\n", filename, strerror(errno));
        return;
    }

    printf("Информация о файле: %s\n", filename);
    printf("  Размер: %lu байт\n", st.st_size);
    printf("  Права доступа: %o\n", st.st_mode & 0777);
    
    char* time_str = ctime(&st.st_mtime);
    if (time_str) {
        time_str[strlen(time_str) - 1] = '\0'; // Убираем \n
        printf("  Дата изменения: %s\n", time_str);
    }

    if (S_ISREG(st.st_mode)) {
        printf("  Тип: обычный файл\n");
    } else if (S_ISDIR(st.st_mode)) {
        printf("  Тип: директория\n");
    } else {
        printf("  Тип: специальный файл\n");
    }
}

char* format_size(uint64_t size) {
    static char buffer[32];
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double size_d = (double)size;
    
    while (size_d >= 1024.0 && unit < 4) {
        size_d /= 1024.0;
        unit++;
    }
    
    if (unit == 0) {
        snprintf(buffer, sizeof(buffer), "%lu %s", size, units[unit]);
    } else {
        snprintf(buffer, sizeof(buffer), "%.1f %s", size_d, units[unit]);
    }
    
    return buffer;
}

void print_progress(int current, int total, const char* operation) {
    int bar_width = 50;
    float progress = (float)current / total;
    int pos = bar_width * progress;
    
    printf("\r%s [", operation);
    for (int i = 0; i < bar_width; i++) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %d/%d (%.1f%%)", current, total, progress * 100);
    fflush(stdout);
    
    if (current == total) {
        printf("\n");
    }
}
