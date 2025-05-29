#include "../headers/lz77.h"
// Поиск совпадения для бинарных данных
void lz77_find_match_binary(const uint8_t *data, size_t pos, size_t data_len, int *best_offset, int *best_length)
	{
   		 *best_offset = 0;
   		 *best_length = 0;
   		 
   		 size_t search_start = (pos >= LZ77_WINDOW_SIZE) ? pos - LZ77_WINDOW_SIZE : 0;
   		 size_t lookahead_size = (data_len - pos > LZ77_BUFFER_SIZE) ? LZ77_BUFFER_SIZE : data_len - pos;
   		 
   		 for (size_t i = search_start; i < pos; i++) {
   		     int match_length = 0;
   		     
   		     while (match_length < lookahead_size &&  i + match_length < pos && data[i + match_length] == data[pos + match_length])
		     {
   		         match_length++;
   		     }
   		     
   		     // Обработка циклических совпадений
   		     if (match_length > 0) {
   		         int extended_length = match_length;
   		         int pattern_len = pos - i;
   		         
   		         while (extended_length < lookahead_size &&
   		                data[i + (extended_length % pattern_len)] == data[pos + extended_length]) {
   		             extended_length++;
   		         }
   		         
   		         if (extended_length > *best_length && extended_length >= 3) { // Минимальная длина 3
   		             *best_length = extended_length;
   		             *best_offset = pos - i;
   		         }
   		     }
   		 }
	}

// Кодирование файла LZ77
int lz77_encode_file(const char *input_filename, const char *output_filename)
	{
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
   		 
   		 // Читаем файл в память (для больших файлов нужна потоковая обработка)
   		 uint8_t *data = malloc(file_size);
   		 if (!data || fread(data, 1, file_size, input) != file_size) {
   		     free(data);
   		     fclose(input);
   		     fclose(output);
   		     return -1;
   		 }
   		 
   		 // Записываем размер исходного файла
   		 fwrite(&file_size, sizeof(size_t), 1, output);
   		 
   		 size_t pos = 0;
   		 uint32_t token_count = 0;
   		 
   		 while (pos < file_size) {
   		     int offset, length;
   		     lz77_find_match_binary(data, pos, file_size, &offset, &length);
   		     
   		     LZ77_Token token;
   		     token.offset = (uint16_t)offset;
   		     token.length = (uint8_t)length;
   		     token.next_byte = (pos + length < file_size) ? data[pos + length] : 0;
        
   		     fwrite(&token, sizeof(LZ77_Token), 1, output);
   		     token_count++;
   		     
   		     pos += length + 1;
   		 }
   		 
   		 printf("LZ77: Исходный размер: %zu байт, токенов: %u\n", file_size, token_count);
   		 
   		 free(data);

   		 fclose(input);

   		 fclose(output);

   		 return 0;
	}

// Декодирование файла LZ77
int lz77_decode_file(const char *input_filename, const char *output_filename)
	{
   		 FILE *input = fopen(input_filename, "rb");
   		 FILE *output = fopen(output_filename, "wb");
   		 
   		 if (!input || !output) {
   		     if (input) fclose(input);
   		     if (output) fclose(output);
   		     return -1;
   		 }
   		 
   		 // Читаем размер исходного файла
   		 size_t original_size;
   		 if (fread(&original_size, sizeof(size_t), 1, input) != 1) {
   		     fclose(input);
   		     fclose(output);
   		     return -1;
   		 }
   		 
   		 uint8_t *result = malloc(original_size);
   		 size_t result_pos = 0;
   		 
   		 LZ77_Token token;
   		 while (fread(&token, sizeof(LZ77_Token), 1, input) == 1 && result_pos < original_size) {
   		     // Копируем данные по смещению и длине
   		     for (int i = 0; i < token.length && result_pos < original_size; i++) {
   		         result[result_pos] = result[result_pos - token.offset];
   		         result_pos++;
   		     }
   		     
   		     // Добавляем следующий байт
   		     if (result_pos < original_size) {
   		         result[result_pos] = token.next_byte;
   		         result_pos++;
   		     }
   		 }
   		 
   		 fwrite(result, 1, result_pos, output);
   		 
   		 free(result);
   		 fclose(input);
   		 fclose(output);
   		 return 0;
	}
