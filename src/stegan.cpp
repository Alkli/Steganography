#include "stegan.h"

#pragma warning(disable : 4996)

const int FILE_BUFFER_SIZE = 4096;
const int SIZE_NAME_FILE = 256;

void writing(char name_file_message[SIZE_NAME_FILE], char name_file_case[SIZE_NAME_FILE], char name_file_result[SIZE_NAME_FILE])
{
   FILE *file_message, *file_case, *file_result;

   check_length_name_file (name_file_message);
   exist_file_check(name_file_message);

   check_length_name_file (name_file_case);
   exist_file_check(name_file_case);

   check_length_name_file (name_file_result);

   file_message = fopen (name_file_message, "r+b");
   file_case = fopen(name_file_case, "r+b");
   file_result = fopen(name_file_result, "w+b");

   if ((file_message == NULL) ||
      (file_case    == NULL) ||
      (file_result  == NULL))
   {
      std::cerr << "Error opening of file. " << std::endl;
      exit(1);
   }

   check_size_of_files_writing(file_case, file_message, name_file_message);

   fseek(file_message, 0, 0);

   copy_headers(file_case, file_result);
   write_int(strlen(name_file_message), file_case, file_result);
   write_string(name_file_message, file_case, file_result);
   write_int(filelength (fileno(file_message)), file_case, file_result);
   write_data(file_message, file_case, file_result, filelength (fileno(file_message))); 
   write_tail(file_case, file_result); 

   fclose (file_message);
   fclose (file_case);
   fclose (file_result);
}

void write_int(int size, FILE *file_case, FILE *file_result)
{
   for (int i=0; i<8*sizeof(int); i++)
   {
      writing_bit (size, i, file_case, file_result);
   }
}

void write_string(char *str, FILE *file_case, FILE *file_result)
{
   int size = strlen (str);

   for (int i=0; i<size; i++)
   {		
      byte c_file_data = str[i]; 
      for (int j=0; j<8; j++)
      {
         writing_bit (c_file_data, j, file_case, file_result);
      }		
   }	   
}

void write_data(FILE *file_message, FILE *file_case, FILE *file_result, int fsize)
{
   for (int i=0; i<fsize; i++)
   {		
      byte c_file_data = fgetc(file_message); 
      for (int j=0; j<8; j++)
      {
         writing_bit (c_file_data, j, file_case, file_result);
      }		
   }
}

void write_tail(FILE *file_message, FILE *file_result)
{
   char buffer[FILE_BUFFER_SIZE];
   size_t size_buffer;
   while ( !feof(file_message) ) 
   {
      size_buffer = fread (buffer, 1, FILE_BUFFER_SIZE, file_message); 
      fwrite (buffer, 1, size_buffer, file_result); 
   }   
}

template<typename T>
inline void writing_bit (T data, int i, FILE *file_case, FILE *file_result)
{
   byte temp_bit = data & (1<<i) ? 1 : 0;
   byte c_file_case = fgetc(file_case); 
   c_file_case = (c_file_case/2)*2 + temp_bit; 
   fputc (c_file_case, file_result);
}




void reading (char name_file_case[SIZE_NAME_FILE])
{
   FILE *file_message, *file_case;

   exist_file_check(name_file_case);

   file_case= fopen (name_file_case, "r");
   if (file_case    == NULL) 
   {
      std::cerr << "Error opening of file. " << std::endl;
      exit(1);
   }

   read_title(file_case); 
   int size = read_int(file_case); 
   if (size > (SIZE_NAME_FILE))
   {
      std::cerr << "Error reading. File was corrupted. " << std::endl;
      exit(1);
   }

   std::string name_file_message = read_name(size, file_case);
   file_message = fopen (name_file_message.c_str(), "w+b");
   if (file_message == NULL) 
   {
      std::cerr << "Error opening of file. " << std::endl;
      exit(1);
   }         

   size = read_int(file_case);
   check_size_of_files_reading(file_case, size);

   read_data(size, file_case, file_message);

   fclose (file_message);
   fclose (file_case);
}

void read_title(FILE *file_case)
{
   char c_file_case;
   int size_title = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);  
   for (int i=0; i<size_title; i++)
   {
      c_file_case = fgetc(file_case);
   }
}

int read_int(FILE *file_case)
{
   int size = 0;
   byte c_file_case;
   bool temp_bit;
   for (int i=0; i < sizeof(int)*8; i++)
   {
      c_file_case = fgetc(file_case);
      temp_bit = (c_file_case&1);
      size = size + (temp_bit << i);
   }
   return size;
}

std::string read_name(int size, FILE *file_case)
{
   byte c_file_data; 
   std::string name_file;
   for (int i=0; i<size; i++)
   {
      c_file_data = read_byte(file_case);
      name_file += c_file_data;
   }
   return name_file;
}

void read_data(int size, FILE *file_case, FILE *file_message)
{
   byte c_file_message; 
   for (int i=0; i<size; i++)
   {
      c_file_message = read_byte(file_case);
      fputc(c_file_message, file_message);
   }
}

inline byte read_byte (FILE *file_case)
{  
   byte c_file_case;
   byte c_file_message = 0; 
   bool temp_bit;

   for (int j=0; j<8; j++)
   {
      c_file_case = fgetc(file_case);
      temp_bit = (c_file_case&1);
      c_file_message = c_file_message + (temp_bit << j);
   }
   return c_file_message;
}

void help()
{
   std::cout << "This program hides contents of any file in 24bit-.bmp. Existing file with data and .bmp-image are required for writing. Maximum size file with data is expressed by the formula: " << std::endl;
   std::cout << "file_size_data = (file_size_case-118- strlen(name_file_data)*8)/8;" << std::endl;
   std::cout << "where file_size_data - maximum size file with data, file_size_case- size of .bmp, strlen(name_file_data) - lenght of name file with data" << std::endl;

   std::cout << std::endl;
   std::cout << "The program uses the following notation: " << std::endl;
   std::cout << "for writing: " << std::endl;
   std::cout << "name_file_message - name file with data" << std::endl;
   std::cout << "name_file_case - name of .bmp-file without data" << std::endl;
   std::cout << "name_file_result - name of result file" << std::endl;
   std::cout << "command for writing: steganography.exe writing name_file_message name_file_case name_file_result" << std::endl;

   std::cout << std::endl;
   std::cout << "for reading: " << std::endl;
   std::cout << "data name_file - name of .bmp-file contained hidden data" << std::endl;
   std::cout << "command for readign: steganography.exe readign name_file" << std::endl;

   std::cout << "command for help: steganography.exe help" << std::endl;

   std::cout << std::endl << "Press any key to output in menu.";

   getch();
}

void exist_file_check(char* name_file)
{
   if (access(name_file, 0)) 
   {
      std::cerr << "File " << name_file << " does not exist" << std::endl;
      exit(1);
   }
} 

void check_size_of_files_writing(FILE *file_case, FILE *file_message, char *name_file_message)
{
   int size_case = filelength (fileno(file_case));
   int size_message = filelength (fileno(file_message));
   int size_data_for_writing =   sizeof(BITMAPFILEHEADER) + 
      sizeof(BITMAPINFOHEADER) + 
      sizeof(int) + 
      sizeof(int) + 
      strlen (name_file_message)*8 + 
      size_message*8;

   if ((size_case-size_data_for_writing)<0) 
   {
      std::cerr << "File containing message is too large for this .bmp" << std::endl;
      exit(1);
   }   
}

void copy_headers(FILE *file_case, FILE *file_result)
{
   BITMAPFILEHEADER bfh_l; 
   BITMAPINFOHEADER bih_l; 

   fread (&bfh_l, sizeof(bfh_l), 1, file_case);       
   fwrite(&bfh_l, sizeof(bfh_l), 1, file_result);   
   fread (&bih_l, sizeof(bih_l), 1, file_case);       
   fwrite(&bih_l, sizeof(bih_l), 1, file_result);  
}

void check_size_of_files_reading(FILE *file_case, int size)
{  
   int size_case = filelength (fileno(file_case));

   if ( (unsigned int)size > ((size_case) - 
      (sizeof(BITMAPFILEHEADER) +      
      sizeof(BITMAPINFOHEADER) + 
      sizeof(int) + 
      sizeof(int) ) ) )
   {
      std::cerr << "Error reading. File was corrupted. " << std::endl;
      exit(0);
   }
};

void check_length_name_file(char *name_file)
{
   int length = strlen(name_file);
   if (length > 256)
   {
      std::cerr << "Error: name of file " << name_file << "is too large." << std::endl;
      exit(1);
   }
};

int main(int argc, char* argv[])
{
   if (argc<2) 
   {  
      std::cerr << "Not enough arguments." << std::endl;
      return 0;
   }
   if (strcmp(argv[1], "writing") == 0)
   {
      writing(argv[2], argv[3], argv[4]);
   }
   if (strcmp(argv[1], "reading") == 0)
   {
      reading(argv[2]);
   }
   if (strcmp(argv[1], "help") == 0)
   {
      help();
   }
}

