#include <stdio.h>
#include "fileio.h"

int main(int argc, char *argv[]){
  File f1, f2, f3;

  f1=open_file("one.dat");
  fs_print_error();
  
  write_file_at(f1, "MZ", 2, BEGINNING_OF_FILE, 0L);
  // should catch error here
  fs_print_error();
  
  write_file_at(f1, "Z", 1, BEGINNING_OF_FILE, 1L);
  fs_print_error();
  
  f2=open_file("two.dat");
  fs_print_error();
  
  write_file_at(f2, "AB", 2, BEGINNING_OF_FILE, 0L);
  fs_print_error();
  
  write_file_at(f1, "M", 1, BEGINNING_OF_FILE, 0L);
  // should catch error here
  fs_print_error();
  
  f3=open_file("three.dat");
  fs_print_error();
  
  write_file_at(f3, "HELLOTHERE", strlen("HELLOTHERE"), BEGINNING_OF_FILE, 0L);
  fs_print_error();
  
  write_file_at(f3, "M", 1, END_OF_FILE, -10L);
  fs_print_error();
  
  write_file_at(f3, "Z", 1, END_OF_FILE, -9L);
  // should catch error here
  fs_print_error();
  
  close_file(f1);
  //fs_print_error();
  close_file(f2);
  //fs_print_error();
  close_file(f3);
  //fs_print_error();
  
  system("rm one.dat two.dat three.dat");
  return 0;
}