#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <stdio.h>         
#include <stdlib.h>         
#include <string.h>        
#include <stdint.h>         
#include <errno.h>          
#include <time.h>          
#include <sys/stat.h>       
#include <sys/types.h>     
#include <unistd.h>         
#include <fcntl.h>         
#include <sys/mman.h>
#include <libgen.h>         

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


#define ARCHIVE_SIGNATURE "MYLZ77HF"
#define ARCHIVE_END_SIGNATURE "ENDARCH!"
#define DEF_PATH_OUTPUT_TMP "../PROGRAM/outputs/"

void init_crc32(void);
uint32_t calculate_crc32(const uint8_t* data, size_t size);
int create_archive(const char* archive_name, char** filenames, int file_count);
int extract_archive(const char* archive_name, const char* output_dir);
int list_archive(const char* archive_name);
uint8_t* read_file_to_memory(const char* filename, size_t* size);
int write_memory_to_file(const char* filename, const uint8_t* data, size_t size);
void create_directory_recursive(const char* path);
uint8_t* compress_data(const uint8_t* data, size_t size, size_t* compressed_size);
uint8_t* decompress_data(const uint8_t* compressed_data, size_t compressed_size, size_t* original_size);
uint8_t* run_compression_pipeline(const uint8_t* data, size_t size, size_t* compressed_size,int (*first_stage)(const char*, const char*),int (*second_stage)(const char*, const char*));
void cleanup_and_exit(char** full_paths, FileEntryFixed* files, int file_count, FILE* archive_fp);
uint32_t calculate_checksum(const uint8_t* data, size_t size);
char* get_full_path(const char* filename);
char* get_filename_from_path(const char* path);
int ensure_temp_directory(void);

#endif
