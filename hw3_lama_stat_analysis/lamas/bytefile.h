#ifndef BYTEFILE_H
#define BYTEFILE_H

#include "util.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>

/* The unpacked representation of bytecode file */
struct bytefile {
  const char* string_ptr;         /* A pointer to the beginning of the string table */
  const int*  public_ptr;         /* A pointer to the beginning of publics table    */
  const unsigned char* code_ptr;           /* A pointer to the bytecode itself               */
  size_t      code_size;          /* The size (in bytes) of the bytecode section    */
  /* ^^^ Custom fields ^^^ */
  size_t stringtab_size;          /* The size (in bytes) of the string table        */
  size_t global_area_size;        /* The size (in words) of global area             */
  size_t public_symbols_number;   /* The number of public symbols                   */
  char   buffer[0];
  /* ^^^ Bytefile structure ^^^ */

  inline bool check_ip(ip_t ip, int sz) {
    return code_ptr <= ip && ip + sz <= code_ptr + code_size;
  }

  inline void assert_ip(ip_t ip, int sz) {
    if (unlikely(!check_ip(ip, sz))) {
      std::cerr << "Instruction pointer outer of code bounds.\n" \
                   "IP: " << (void*) ip << "\nCode start: " << (void*) code_ptr << "\n" << \
                   "Size: " << sz << "\nCode end: " << (void*) (code_ptr + code_size) << "\n";
      exit(1);
    }
  }

  inline const char* get_string(size_t pos) const {
    if (pos < stringtab_size) {
      return &string_ptr[pos];
    } else {
        std::cerr << "String offset out of bounds.\nRequested string offset: " << pos << \
                     "\nStringtab size: " << stringtab_size << '\n';
        exit(1);
    }
  }

  inline std::vector<ip_t> get_public_ptrs() const {
    std::vector<ip_t> res(public_symbols_number);
    for (size_t i = 0; i < public_symbols_number; ++i) {
      res[i] = (ip_t) (public_ptr[2 * i] + code_ptr);
    }
    return res;
  }


};

/* Reads a binary bytecode file by name and unpacks it */
static inline bytefile* read_file(const char* fname) {
  FILE* f = fopen(fname, "rb");
  long size;
  bytefile* file;

  if (f == 0) {
    std::cerr << strerror(errno) << '\n';
    exit(1);
  }

  if (fseek (f, 0, SEEK_END) == -1) {
    std::cerr << strerror(errno) << '\n';
    exit(1);
  }

  file = (bytefile*) malloc(sizeof(int*) * 3 + sizeof(size_t) + (size = ftell(f)));

  if (file == 0) {
    std::cerr << "Unable to allocate memory.\n";
    exit(1);
  }

  rewind(f);

  if (size != fread(&file->stringtab_size, 1, size, f)) {
    std::cerr << strerror(errno) << '\n';
    exit(1);
  }

  fclose(f);

  size_t stringtab_offset = file->public_symbols_number * 2 * sizeof(int);
  if (stringtab_offset < size) {
    file->string_ptr = file->buffer + file->public_symbols_number * 2 * sizeof(int);
  } else {
    std::cerr << "String table can't be found in file.\n" << \
                 "Recognized string offset: " << stringtab_offset << "\n"\
                 "File size: " << size;
    exit(1);
  }
  file->public_ptr = (int*) file->buffer;
  size_t code_offset = stringtab_offset + file->stringtab_size;
  if (code_offset < size) {
    file->code_ptr = (ip_t) file->buffer + code_offset;
    file->code_size = size - 3 * sizeof(size_t) - code_offset - file->global_area_size * sizeof(size_t) + 3;
  } else {
    std::cerr << "Code section can't be found in file.\n" << \
                 "Recognized code offset: " << code_offset << "\n"\
                 "File size: " << size;
    exit(1);
  }

  return file;
}

#endif
