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


static int seek_file(File file, SeekAnchor start, long offset) {
  if (! file->fp || (start != BEGINNING_OF_FILE && 
               start != CURRENT_POSITION && start != END_OF_FILE)) { //if file isnt valid or seek anchor isnt valid return 0
    return 0;
  }
  else {
    if (!fseek(file->fp, offset, start == BEGINNING_OF_FILE ? SEEK_SET : 
                (start == END_OF_FILE ? SEEK_END : SEEK_CUR))) { //
      return 1;
    }
    else {
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
File open_file(char *name) {
  File ptr = malloc(sizeof(FileInternal));


  fserror=NONE;
  // try to open existing file
  ptr->fp=fopen(name, "r+");
  if (! ptr->fp) {
    // fail, fall back to creation
    ptr->fp=fopen(name, "w+");
    if (! ptr->fp) {
      fserror=OPEN_FAILED;
      return NULL;
    }
  }
  return ptr;
}


// close a 'file'.  If the close operation fails, the global 'fserror'
// is set to CLOSE_FAILED, otherwise to NONE.
void close_file(File file) {
  if (file->fp && ! fclose(file->fp)) {
    fserror=NONE;
  }
  else {
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
                             SeekAnchor start, long offset) {


  unsigned long bytes_read=0L;


  fserror=NONE;
  if (! file->fp || ! seek_file(file, start, offset)) {
    fserror=READ_FAILED;
  }
  else {
    bytes_read=fread(data, 1, num_bytes, file->fp);
    if (ferror(file->fp)) {
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
                             SeekAnchor start, long offset) {
  unsigned long bytes_written=0L;




  fserror=NONE; //no error yet
  if (! file->fp || ! seek_file(file, start, offset)) //if file doesn't exist or 
  {
    fserror=WRITE_FAILED;
  }
  // writing to the beginning of the file
  else if (start == BEGINNING_OF_FILE)
  {
    // first 2 bytes of data contains "MZ"
    if (offset == 0L && strncmp(data, "MZ", 2) == 0) 
    {
        fserror=ILLEGAL_MZ;
    }
    // writing "Z" to second byte but "M" already exist as first byte
    else if (offset == 1L && file->mem[0] == 'M' && *(char *)data == 'Z')
    {
        fserror = ILLEGAL_MZ;
    }
    // writing "M" to first byte but "Z" already exist as first byte
    else if (offset == 0L && file->mem[1] == 'Z' && *(char *)data == 'M')
    {
        fserror = ILLEGAL_MZ;
    }
    else 
    {
        bytes_written=fwrite(data, 1, num_bytes, file->fp);
        if (bytes_written < num_bytes) 
        {
            fserror=WRITE_FAILED;
        }
        // if write is done to the beginning, set mem[] = to the first two bytes of the file
        read_file_from(file, file->mem, 2, BEGINNING_OF_FILE, 0L);
    }
    return bytes_written;
  }
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