#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"

//
// GLOBALS
//

FSError fserror;

//
// private functions
//

static int seek_file(File file, SeekAnchor start, long offset){
  if (! file->fp || (start != BEGINNING_OF_FILE && 
	       start != CURRENT_POSITION && start != END_OF_FILE)){
    return 0;
  }
  else{
    if (! fseek(file->fp, offset, start == BEGINNING_OF_FILE ? SEEK_SET : 
		(start == END_OF_FILE ? SEEK_END : SEEK_CUR))){
      return 1;
    }
    else{
      return 0;
    }
  }
}

//
// public functions
//

// open or create a file with pathname 'name' and return a File
// handle.  The file is always opened with read/write access. If the
// open operation fails, the global 'fserror' is set to OPEN_FAILED,
// otherwise to NONE.
File open_file(char *name){
  File ptr = malloc(sizeof(FileInternal));

  fserror=NONE;
  // try to open existing file
  ptr->fp=fopen(name, "r+");
  if (! ptr->fp){
    // fail, fall back to creation
    ptr->fp=fopen(name, "w+");
    if (! ptr->fp){
      fserror=OPEN_FAILED;
      return NULL;
    }
  }
  return ptr;
}

// close a 'file'.  If the close operation fails, the global 'fserror'
// is set to CLOSE_FAILED, otherwise to NONE.
void close_file(File file){
  if (file->fp && ! fclose(file->fp)){
    fserror=NONE;
  }
  else{
    fserror=CLOSE_FAILED;
  }
  free(file);
}

// read at most 'num_bytes' bytes from 'file' into the buffer 'data',
// starting 'offset' bytes from the 'start' position.  The starting
// position is BEGINNING_OF_FILE, CURRENT_POSITION, or END_OF_FILE. If
// the read operation fails, the global 'fserror' is set to READ_FAILED,
// otherwise to NONE.
unsigned long read_file_from(File file, void *data, unsigned long num_bytes, 
			     SeekAnchor start, long offset){

  unsigned long bytes_read=0L;

  fserror=NONE;
  if (! file->fp || ! seek_file(file, start, offset)){
    fserror=READ_FAILED;
  }
  else {
    bytes_read=fread(data, 1, num_bytes, file->fp);
    if (ferror(file->fp)){
      fserror=READ_FAILED;
    }
  }
  return bytes_read;
}
  
// write 'num_bytes' to 'file' from the buffer 'data', starting
// 'offset' bytes from the 'start' position.  The starting position is
// BEGINNING_OF_FILE, CURRENT_POSITION, or END_OF_FILE.  If an attempt
// is made to modify a file such that "MZ" appears in the first two
// bytes of the file, the write operation fails and ILLEGAL_MZ is
// stored in the global 'fserror'.  If the write fails for any other
// reason, the global 'fserror' is set to WRITE_FAILED, otherwise to
// NONE.
unsigned long write_file_at(File file, void *data, unsigned long num_bytes, 
			     SeekAnchor start, long offset){
  unsigned long bytes_written=0L;
  
  long global_offset = 0L;
  
  // calculate global_offset (offset from the beginning of the file)
  if (start == BEGINNING_OF_FILE){
      global_offset = offset;
  }
  else if (start == CURRENT_POSITION){
      fseek(file->fp, 0L, SEEK_CUR);
      global_offset = ftell(file->fp) + offset;
  }
  else{
      fseek(file->fp, 0L, SEEK_END);
      global_offset = ftell(file->fp) + offset;
  }
  

  fserror=NONE;
  if (! file->fp || ! seek_file(file, start, offset)){
    fserror=WRITE_FAILED;
  }
  // writing starting at very beginning of the file
  else if (global_offset == 0L){
    // first 2 bytes of data contains "MZ"
    if (! strncmp(data, "MZ", 2)){
        fserror=ILLEGAL_MZ;
    }
    // writing "M" to second byte but "Z" already exist as first byte
    else if (file->mem[1] == 'Z' && *(char *)data == 'M'){
        fserror = ILLEGAL_MZ;
    }
    // proceed to write
    else{
        bytes_written=fwrite(data, 1, num_bytes, file->fp);
        if (bytes_written < num_bytes){
            fserror=WRITE_FAILED;
        }
        // if write is done to the beginning, set mem[] = to the first two bytes of the file
        read_file_from(file, file->mem, 2, BEGINNING_OF_FILE, 0L);
        fseek(file -> fp, 0L + num_bytes, SEEK_SET);
    }
  }
  // writing starting at the second byte
  else if (global_offset == 1L){
    // writing "Z" to second byte but "M" already exist as first byte
    if ( file->mem[0] == 'M' && ! strncmp(data,"Z",1)){
        fserror = ILLEGAL_MZ;
    }
    // proceed to write
    else{
        fseek(file->fp, 1L, SEEK_SET);
        bytes_written=fwrite(data, 1, num_bytes, file->fp);
        if (bytes_written < num_bytes){
            fserror=WRITE_FAILED;
        }
        // if write is done to the beginning, set mem[] = to the first two bytes of the file
        read_file_from(file, file->mem, 2, BEGINNING_OF_FILE, 0L);
        fseek(file -> fp, 1L + num_bytes, SEEK_SET);
    }
  }
  // not writing to the beginning, don't care
  else{
        bytes_written=fwrite(data, 1, num_bytes, file->fp);
        if (bytes_written < num_bytes){
            fserror=WRITE_FAILED;
        }
  }
  
  return bytes_written;
  
}



// print a string representation of the error indicated by the global
// 'fserror'.
void fs_print_error(void) {
  printf("FS ERROR: ");
  switch (fserror) {
  case NONE:
    puts("NONE");
    break;
  case OPEN_FAILED:
    puts("OPEN_FAILED");
    break;
  case CLOSE_FAILED:
    puts("CLOSE_FAILED");
    break;
  case READ_FAILED:
    puts("READ_FAILED");
    break;
  case WRITE_FAILED:
    puts("WRITE_FAILED");
    break;
  case ILLEGAL_MZ:
    puts("ILLEGAL_MZ: SHAME ON YOU!");
  break;
  default:
    puts("** UNKNOWN ERROR **");
  }
}
