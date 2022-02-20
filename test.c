#include <stdio.h>
#include "fileio.h"


int main(int argc, char *argv[]) {
  File f1, f2;
  f1=open_file("one.dat");
  fs_print_error();
  write_file_at(f1, "the quick brown fox", 19, BEGINNING_OF_FILE, 0L);
  fs_print_error();
  write_file_at(f1, "M", 1, BEGINNING_OF_FILE, 0L);
  fs_print_error();
  write_file_at(f1, "MZ", 2, BEGINNING_OF_FILE, 0L);
  fs_print_error();
  write_file_at(f1, "Z", 1, BEGINNING_OF_FILE, 1L);
  fs_print_error();  
  f2=open_file("two.dat");
  write_file_at(f2, "AB", 2, BEGINNING_OF_FILE, 0L);
  fs_print_error();
  write_file_at(f1, "M", 1, BEGINNING_OF_FILE, 0L);
  fs_print_error();
  
  printf("%c\t",f1->mem[0]);      // print 1st byte of one.dat
  printf("%c\n",f1->mem[1]);      // print 2nd byte of one.dat
  printf("%c\t",f2->mem[0]);      // print 1st byte of two.dat
  printf("%c\n",f2->mem[1]);      // print 2nd byte of two.dat
  // prints nothing if mem[x] is empty

  close_file(f1); //close file for memory
  close_file(f2); //close file for memory
}