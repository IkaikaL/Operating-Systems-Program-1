#include <stdio.h>
#include "fileio.h"

int main(int argc, char *argv[]) {
  File f1, f2, f3;

  system("rm testfile1 testfile2 testfile3");

  f1=open_file("testfile1");
  fs_print_error();

  write_file_at(f1, "pleaseworkpleasework", strlen("pleaseworkpleasework"), BEGINNING_OF_FILE, 0L);
  fs_print_error();
  
  write_file_at(f1, "MZ", 2, BEGINNING_OF_FILE, 0L);
  // should catch error here
  fs_print_error();
  
  write_file_at(f1, "Z", 1, BEGINNING_OF_FILE, 1L);
  fs_print_error();
  
  f2=open_file("testfile2");
  fs_print_error();

  write_file_at(f2, "MZ", 2, CURRENT_POSITION, 0L);
  fs_print_error();

  write_file_at(f2, "testingtestingtesting", strlen("testingtestingtesting"), BEGINNING_OF_FILE, 0L);
  fs_print_error();
  
  write_file_at(f2, "AB", 2, BEGINNING_OF_FILE, 0L);
  fs_print_error();
  
  write_file_at(f1, "M", 1, BEGINNING_OF_FILE, 0L);
  // should catch error here
  fs_print_error();
  
  f3=open_file("testfile3");
  fs_print_error();
  
  write_file_at(f3, "thequickbrownfoxjumpedoverthelazydog", strlen("thequickbrownfoxjumpedoverthelazydog"), BEGINNING_OF_FILE, 0L);
  fs_print_error();
  
  write_file_at(f3, "MZ", 2, BEGINNING_OF_FILE, 0L);
  // should catch error here
  fs_print_error();
  
  write_file_at(f3, "M", 1, END_OF_FILE, -36L);
  fs_print_error();
  
  write_file_at(f3, "Z", 1, END_OF_FILE, -35L);
  // should catch error here
  fs_print_error();

  write_file_at(f3, "M", 1, END_OF_FILE, -10L);
  fs_print_error();

  write_file_at(f3, "Z", 1, END_OF_FILE, -9L);
  fs_print_error();
  
  close_file(f1); //should end in pzeaseworkpleasework
  close_file(f2); //should end in MhequickbrownfoxjumpedoverMZelazydog
  close_file(f3); //should end in ABstingtestingtesting
  
  return 0;
}