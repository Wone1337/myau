#include "../headers/archiver.h"

// Таблица CRC32
static uint32_t crc32_table[256];
static int crc32_initialized = 0;

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
    for (size_t i = 0; i < size; i++) {
        crc = crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

// Чтение файла в память
uint8_t* read_file_to_memory(const char* filename, size_t* size) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        printf("Ошибка: не удается открыть файл %s\n", filename);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint8_t* data = malloc(*size);
    if (!data) {
        printf("Ошибка: не удается выделить память для файла %s\n", filename);
        fclose(fp);
        return NULL;
    }

    if (fread(data, 1, *size, fp) != *size) {
        printf("Ошибка: не удается прочитать файл %s\n", filename);
        free(data);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    return data;
}

// Запись данных из памяти в файл
int write_memory_to_file(const char* filename, const uint8_t* data, size_t size) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        printf("Ошибка: не удается создать файл %s\n", filename);
        return 0;
    }

    size_t written = fwrite(data, 1, size, fp);
    fclose(fp);

    if (written != size) {
        printf("Ошибка: записано только %zu из %zu байт в файл %s\n",
               written, size, filename);
        return 0;
    }

    return 1;
}

// Создание директорий рекурсивно
void create_directory_recursive(const char* path) {
    char* path_copy = strdup(path);
    char* p = path_copy;

    while (*p) {
        if (*p == '/' && p != path_copy) {
            *p = '\0';
            mkdir(path_copy, 0755);
            *p = '/';
        }
        p++;
    }

    free(path_copy);
}

uint8_t* run_compression_pipeline(const uint8_t* data,size_t size,size_t* compressed_size,int (*first_stage)(const char*, const char*),int (*second_stage)(const char*, const char*)) {
    // Создаём временные файлы
    FILE* input_mem = tmpfile();
    if (!input_mem) {
        printf("Ошибка: не удалось создать временный файл\n");
        return NULL;
    }

    fwrite(data, 1, size, input_mem);
    rewind(input_mem);

    char temp_input[] = DEF_PATH_OUTPUT_TMP "temp_compress_input.tmp";
    char temp_mid[] = DEF_PATH_OUTPUT_TMP "temp_compress_mid.tmp";
    char temp_output[] = DEF_PATH_OUTPUT_TMP "temp_compress_output.tmp";

    // Запись временного входного файла
    FILE* fp_input = fopen(temp_input, "wb+");
    if (!fp_input) {
        fclose(input_mem);
        return NULL;
    }

    char buffer[4096];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), input_mem)) > 0) {
        fwrite(buffer, 1, bytes_read, fp_input);
    }
    fclose(fp_input);
    fclose(input_mem);

    // Этап 1: первое сжатие
    if (first_stage(temp_input, temp_mid) != 0) {
        remove(temp_input);
        return NULL;
    }

    // Этап 2: второе сжатие
    if (second_stage(temp_mid, temp_output) != 0) {
        remove(temp_input);
        remove(temp_mid);
        return NULL;
    }

    // Читаем результат
    FILE* fp_result = fopen(temp_output, "rb");
    if (!fp_result) {
        remove(temp_input);
        remove(temp_mid);
        remove(temp_output);
        return NULL;
    }

    fseek(fp_result, 0, SEEK_END);
    *compressed_size = ftell(fp_result);
    rewind(fp_result);

    uint8_t* result = malloc(*compressed_size);
    if (result && fread(result, 1, *compressed_size, fp_result) == *compressed_size) {
        // OK
    } else {
        free(result);
        result = NULL;
    }

    fclose(fp_result);
    remove(temp_input);
    remove(temp_mid);
    remove(temp_output);

    return result;
}

// Оптимизированная функция сжатия: LZ77 -> Huffman

uint8_t* compress_data(const uint8_t* data, size_t size, size_t* compressed_size) {
    return run_compression_pipeline(data, size, compressed_size, lz77_compress_file, huffman_compress_file);
}

uint8_t* decompress_data(const uint8_t* compressed_data, size_t compressed_size, size_t* original_size) {
    // Создаем временные файлы
    char temp_huffman[] = DEF_PATH_OUTPUT_TMP "temp_decompress_huffman.tmp";
    char temp_lz77[] = DEF_PATH_OUTPUT_TMP "temp_decompress_lz77.tmp";
    char temp_output[] = DEF_PATH_OUTPUT_TMP "temp_decompress_output.tmp";

    // Записываем сжатые данные
    FILE* fp = fopen(temp_huffman, "wb+");
    if (!fp) return NULL;
    fwrite(compressed_data, 1, compressed_size, fp);
    fclose(fp);

    // Этап 1: Huffman декомпрессия
    if (huffman_decompress_file(temp_huffman, temp_lz77) != 0) {
        remove(temp_huffman);
        return NULL;
    }

    // Этап 2: LZ77 декомпрессия
    if (lz77_decompress_file(temp_lz77, temp_output) != 0) {
        remove(temp_huffman);
        remove(temp_lz77);
        return NULL;
    }

    // Читаем результат
    FILE* fp_result = fopen(temp_output, "rb");
    if (!fp_result) {
        remove(temp_huffman);
        remove(temp_lz77);
        remove(temp_output);
        return NULL;
    }

    fseek(fp_result, 0, SEEK_END);
    *original_size = ftell(fp_result);
    rewind(fp_result);

    uint8_t* result = malloc(*original_size);
    if (result && fread(result, 1, *original_size, fp_result) == *original_size) {
        // OK
    } else {
        free(result);
        result = NULL;
    }

    fclose(fp_result);
    remove(temp_huffman);
    remove(temp_lz77);
    remove(temp_output);

    return result;
}

// Создание архива
int create_archive(const char* archive_name, char** filenames, int file_count) {
    printf("Создание архива %s с %d файлами...\n", archive_name, file_count);

    FILE* archive_fp = fopen(archive_name, "wb");
    if (!archive_fp) {
        printf("Ошибка: не удается создать архив %s\n", archive_name);
        return 0;
    }

    ArchiveHeader header;
    memcpy(header.signature, ARCHIVE_SIGNATURE, 8);
    header.file_count = file_count;
    header.table_size = 0;

    FileEntry* files = malloc(file_count * sizeof(FileEntry));
    if (!files) {
        printf("Ошибка: не удается выделить память для таблицы файлов\n");
        fclose(archive_fp);
        return 0;
    }

    char** full_paths = malloc(file_count * sizeof(char*));
    if (!full_paths) {
        printf("Ошибка: не удается выделить память для путей файлов\n");
        free(files);
        fclose(archive_fp);
        return 0;
    }

    uint32_t table_size = 0;
    for (int i = 0; i < file_count; i++) {
        full_paths[i] = strdup(filenames[i]);
        if (!full_paths[i]) {
            printf("Ошибка: не удается скопировать путь файла %s\n", filenames[i]);
            for (int j = 0; j <= i; j++) free(full_paths[j]);
            free(full_paths);
            free(files);
            fclose(archive_fp);
            return 0;
        }

        const char* file_base_name = strrchr(filenames[i], '/');
        file_base_name = file_base_name ? file_base_name + 1 : filenames[i];

        files[i].name_length = strlen(file_base_name);
        files[i].filename = malloc(files[i].name_length + 1);
        if (!files[i].filename) {
            printf("Ошибка: не удается выделить память для имени файла\n");
            for (int j = 0; j <= i; j++) {
                free(full_paths[j]);
                free(files[j].filename);
            }
            free(full_paths);
            free(files);
            fclose(archive_fp);
            return 0;
        }

        strcpy(files[i].filename, file_base_name);

        size_t file_size;
        uint8_t* file_data = read_file_to_memory(full_paths[i], &file_size);
        if (!file_data) {
            for (int j = 0; j <= i; j++) {
                free(full_paths[j]);
                free(files[j].filename);
            }
            free(full_paths);
            free(files);
            fclose(archive_fp);
            return 0;
        }

        files[i].original_size = file_size;
        files[i].crc32 = calculate_crc32(file_data, file_size);
        files[i].compressed_size = 0;
        files[i].data_offset = 0;
        free(file_data);

        table_size += 2 + files[i].name_length + 8 + 8 + 8 + 4;
    }

    header.table_size = table_size;
    fwrite(&header, sizeof(ArchiveHeader), 1, archive_fp);

    long table_pos = ftell(archive_fp);
    for (int i = 0; i < file_count; i++) {
        fwrite(&files[i].name_length, sizeof(uint16_t), 1, archive_fp);
        fwrite(files[i].filename, 1, files[i].name_length, archive_fp);
        fwrite(&files[i].original_size, sizeof(uint64_t), 1, archive_fp);
        fwrite(&files[i].compressed_size, sizeof(uint64_t), 1, archive_fp);
        fwrite(&files[i].data_offset, sizeof(uint64_t), 1, archive_fp);
        fwrite(&files[i].crc32, sizeof(uint32_t), 1, archive_fp);
    }

    for (int i = 0; i < file_count; i++) {
        printf("Сжатие файла %s... ", files[i].filename);
        size_t file_size;
        uint8_t* file_data = read_file_to_memory(full_paths[i], &file_size);
        if (!file_data) continue;

        files[i].data_offset = ftell(archive_fp);
        size_t compressed_size;
        uint8_t* compressed_data = compress_data(file_data, file_size, &compressed_size);
        free(file_data);

        if (!compressed_data) {
            printf("Ошибка сжатия\n");
            continue;
        }

        fwrite(compressed_data, 1, compressed_size, archive_fp);
        files[i].compressed_size = compressed_size;
        free(compressed_data);
        float ratio = (float)compressed_size / files[i].original_size * 100;
        printf("OK (%.1f%%)\n", ratio);
    }

    fseek(archive_fp, table_pos, SEEK_SET);
    for (int i = 0; i < file_count; i++) {
        fwrite(&files[i].name_length, sizeof(uint16_t), 1, archive_fp);
        fwrite(files[i].filename, 1, files[i].name_length, archive_fp);
        fwrite(&files[i].original_size, sizeof(uint64_t), 1, archive_fp);
        fwrite(&files[i].compressed_size, sizeof(uint64_t), 1, archive_fp);
        fwrite(&files[i].data_offset, sizeof(uint64_t), 1, archive_fp);
        fwrite(&files[i].crc32, sizeof(uint32_t), 1, archive_fp);
    }

    fseek(archive_fp, 0, SEEK_END);
    fwrite(ARCHIVE_END_SIGNATURE, 8, 1, archive_fp);

    for (int i = 0; i < file_count; i++) {
        free(files[i].filename);
        free(full_paths[i]);
    }
    free(files);
    free(full_paths);
    fclose(archive_fp);

    printf("Архив %s успешно создан!\n", archive_name);
    return 1;
}

// Извлечение архива
int extract_archive(const char* archive_name, const char* output_dir) {
    printf("Извлечение архива %s в папку %s...\n", archive_name, output_dir);

    FILE* archive_fp = fopen(archive_name, "rb");
    if (!archive_fp) {
        printf("Ошибка: не удается открыть архив %s\n", archive_name);
        return 0;
    }

    ArchiveHeader header;
    if (fread(&header, sizeof(ArchiveHeader), 1, archive_fp) != 1 ||
        memcmp(header.signature, ARCHIVE_SIGNATURE, 8) != 0) {
        printf("Ошибка: неверная подпись архива\n");
        fclose(archive_fp);
        return 0;
    }

    FileEntry* files = malloc(header.file_count * sizeof(FileEntry));
    if (!files) {
        printf("Ошибка: не удается выделить память для таблицы файлов\n");
        fclose(archive_fp);
        return 0;
    }

    for (uint32_t i = 0; i < header.file_count; i++) {
        fread(&files[i].name_length, sizeof(uint16_t), 1, archive_fp);
        files[i].filename = malloc(files[i].name_length + 1);
        fread(files[i].filename, 1, files[i].name_length, archive_fp);
        files[i].filename[files[i].name_length] = '\0';
        fread(&files[i].original_size, sizeof(uint64_t), 1, archive_fp);
        fread(&files[i].compressed_size, sizeof(uint64_t), 1, archive_fp);
        fread(&files[i].data_offset, sizeof(uint64_t), 1, archive_fp);
        fread(&files[i].crc32, sizeof(uint32_t), 1, archive_fp);
    }

    mkdir(output_dir, 0755);

    for (uint32_t i = 0; i < header.file_count; i++) {
        printf("Извлечение файла %s... ", files[i].filename);

        fseek(archive_fp, files[i].data_offset, SEEK_SET);
        uint8_t* compressed_data = malloc(files[i].compressed_size);
        fread(compressed_data, 1, files[i].compressed_size, archive_fp);

        size_t decompressed_size;
        uint8_t* decompressed_data = decompress_data(compressed_data, files[i].compressed_size, &decompressed_size);
        free(compressed_data);

        if (!decompressed_data) {
            printf("Ошибка декомпрессии\n");
            continue;
        }

        if (decompressed_size != files[i].original_size) {
            printf("Ошибка: неверный размер после декомпрессии\n");
            free(decompressed_data);
            continue;
        }

        uint32_t calculated_crc = calculate_crc32(decompressed_data, decompressed_size);
        if (calculated_crc != files[i].crc32) {
            printf("Ошибка: неверная контрольная сумма\n");
            free(decompressed_data);
            continue;
        }

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s%s", output_dir, files[i].filename);
        create_directory_recursive(full_path);

        if (write_memory_to_file(full_path, decompressed_data, decompressed_size)) {
            printf("OK\n");
        } else {
            printf("Ошибка записи\n");
        }

        free(decompressed_data);
    }

    for (uint32_t i = 0; i < header.file_count; i++) {
        free(files[i].filename);
    }
    free(files);
    fclose(archive_fp);

    printf("Архив успешно извлечен!\n");
    return 1;
}

// Просмотр содержимого архива
int list_archive(const char* archive_name) {
    FILE* archive_fp = fopen(archive_name, "rb");
    if (!archive_fp) {
        printf("Ошибка: не удается открыть архив %s\n", archive_name);
        return 0;
    }

    ArchiveHeader header;
    fread(&header, sizeof(ArchiveHeader), 1, archive_fp);
    if (memcmp(header.signature, ARCHIVE_SIGNATURE, 8) != 0) {
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
        fread(&name_length, sizeof(uint16_t), 1, archive_fp);
        char* filename = malloc(name_length + 1);
        fread(filename, 1, name_length, archive_fp);
        filename[name_length] = '\0';

        uint64_t original_size, compressed_size, data_offset;
        uint32_t crc32;
        fread(&original_size, sizeof(uint64_t), 1, archive_fp);
        fread(&compressed_size, sizeof(uint64_t), 1, archive_fp);
        fread(&data_offset, sizeof(uint64_t), 1, archive_fp);
        fread(&crc32, sizeof(uint32_t), 1, archive_fp);

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

