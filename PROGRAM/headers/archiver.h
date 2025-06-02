#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <dirent.h>
#include "../headers/lz77.h"
#include "../headers/huffman.h"


#ifndef ARCHIVER_H
#define ARCHIVER_H


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

#endif 

