
 #ifndef H_STEGAN
 #define H_STEGAN

void writing();
void write_int(int size, FILE *file_case, FILE *file_result); 
void write_string(char *str, FILE *file_case, FILE *file_result);
void write_data(FILE *file_message, FILE *file_case, FILE *file_result, int fsize); 
void write_tail(FILE *file_message, FILE *file_result); 

template<typename T>
inline void writing_bit (T data, int i, FILE *file_case, FILE *file_result);

void reading();
void read_title(FILE *file_result); 
int read_int(FILE *file_result);
std::string read_name(int size, FILE *file_result); 
void read_data(int size, FILE *file_result, FILE *file_message);
inline byte read_byte (FILE *file_case);

void help();
void exist_file_check(char* name_file);
void check_size_of_files_writing(FILE *file_case, FILE *file_message, char *name_file_message);
void copy_headers(FILE *file_case, FILE *file_result);
void check_size_of_files_reading(FILE *file_case, int size);
void check_length_name_file(char *name_file);

 #endif