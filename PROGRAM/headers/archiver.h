#ifndef ARCHIVER_H
#define ARCHIVER_H

// Стандартные библиотеки C
#include <stdio.h>          // FILE, fopen, fread, fwrite, printf, etc.
#include <stdlib.h>         // malloc, free, exit, etc.
#include <string.h>         // memcmp, strdup, strlen, strcpy, etc.
#include <stdint.h>         // uint8_t, uint32_t, uint64_t, etc.
#include <errno.h>          // errno, strerror
#include <time.h>           // time(), ctime()

// POSIX системные вызовы
#include <sys/stat.h>       // stat, mkdir, S_ISREG, S_ISDIR
#include <sys/types.h>      // size_t, ssize_t, mode_t
#include <unistd.h>         // read, write, close, getcwd, getpid
#include <fcntl.h>          // open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC

// Memory mapping
#include <sys/mman.h>       // mmap, munmap, MAP_FAILED, PROT_READ, MAP_PRIVATE

// Для работы с путями
#include <libgen.h>         // basename


// Подключаем алгоритмы сжатия
#ifndef LZ77_H
#define LZ77_H
int lz77_compress_file(const char* input_file, const char* output_file);
int lz77_decompress_file(const char* input_file, const char* output_file);
#endif

#ifndef HUFFMAN_H
#define HUFFMAN_H
int huffman_compress_file(const char* input_file, const char* output_file);
int huffman_decompress_file(const char* input_file, const char* output_file);
#endif

#define MAX_FILENAME_LENGTH 256
#define MAX_PATH_LENGTH 1024

// Структуры
typedef struct {
    char signature[8];
    uint32_t file_count;
    uint32_t table_size;
} ArchiveHeader;

typedef struct {
    uint16_t name_length;
    char* filename;
    uint64_t original_size;
    uint64_t compressed_size;
    uint64_t data_offset;
    uint32_t crc32;
} FileEntry;

typedef struct {
    char filename[MAX_FILENAME_LENGTH];
    uint64_t original_size;
    uint64_t compressed_size;
    uint64_t file_offset;
    uint32_t checksum;
} FileEntryFixed;

typedef struct {
    ArchiveHeader header;
    FileEntry* files;
    FILE* archive_fp;
} Archive;

// Константы
#define ARCHIVE_SIGNATURE "MYLZ77HF"
#define ARCHIVE_END_SIGNATURE "ENDARCH!"
#define DEF_PATH_OUTPUT_TMP "../PROGRAM/outputs/"

// CRC32 функции
void init_crc32(void);
uint32_t calculate_crc32(const uint8_t* data, size_t size);

// Основные функции архиватора
int create_archive(const char* archive_name, char** filenames, int file_count);
int extract_archive(const char* archive_name, const char* output_dir);
int list_archive(const char* archive_name);

// Файловые операции
uint8_t* read_file_to_memory(const char* filename, size_t* size);
int write_memory_to_file(const char* filename, const uint8_t* data, size_t size);
void create_directory_recursive(const char* path);

// Сжатие/распаковка
uint8_t* compress_data(const uint8_t* data, size_t size, size_t* compressed_size);
uint8_t* decompress_data(const uint8_t* compressed_data, size_t compressed_size, size_t* original_size);

// Pipeline сжатия
uint8_t* run_compression_pipeline(const uint8_t* data, size_t size, size_t* compressed_size,
                                 int (*first_stage)(const char*, const char*),
                                 int (*second_stage)(const char*, const char*));

// Вспомогательные функции
void cleanup_and_exit(char** full_paths, FileEntryFixed* files, int file_count, FILE* archive_fp);
uint32_t calculate_checksum(const uint8_t* data, size_t size);
char* get_full_path(const char* filename);
char* get_filename_from_path(const char* path);
int ensure_temp_directory(void);

#endif // ARCHIVER_H
