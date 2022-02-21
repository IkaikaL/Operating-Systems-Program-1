#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"

//
// GLOBALS
//
typedef struct _FileInternal{ //given in class
    FILE* fp; //pointer to file
    char mem[2]; //stores first two bytes to easily compare them
}FileInternal;

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
unsigned long read_file_from(File file, void *data, unsigned long num_bytes, SeekAnchor start, long offset){

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
unsigned long write_file_at(File file, void *data, unsigned long num_bytes, SeekAnchor start, long offset){

  unsigned long bytes_written = 0L; //track how many letters have been written
  
  long global_offset = 0L; //track the overall position

  if (start == BEGINNING_OF_FILE){
      global_offset = offset; //begins at beginning so offset is the only thing that matters
  }
  else if (start == CURRENT_POSITION){
      fseek(file->fp, 0L, SEEK_CUR); //use fseek to find the current position for modification
      global_offset = ftell(file->fp) + offset; //sets global_offset to current position + offset
  }
  else{
      fseek(file->fp, 0L, SEEK_END); //use fseek to find the end of the data on the text file
      global_offset = ftell(file->fp) + offset; //sets global_offset to end of file + offset
  }

  fserror=NONE;
  if (! file->fp || ! seek_file(file, start, offset)){ //if given position is not in file or file doesn't exist return error
    fserror = WRITE_FAILED;
  }
  else if (! strncmp(data, "MZ", 2)){ //if MZ is already in the beginning of the file return error
    fserror = ILLEGAL_MZ;
  }

  else if (global_offset == 0L){ //if editing first byte of file
    if (file->mem[1] == 'Z' && ! strncmp(data,"M",0)){ //if Z is already in position 1 make sure that M cannot be put into 0
        fserror = ILLEGAL_MZ;
    }
    else{
        bytes_written = fwrite(data, 1, num_bytes, file->fp); //update bytes_written as characters are written
        if (bytes_written < num_bytes){ //not necessary?
            fserror = WRITE_FAILED;
        }
        file -> mem[0] = ((char*)data)[0]; //set mem[0] equal to first new first byte
        fseek(file -> fp, 0L + num_bytes, SEEK_SET); //reset position of pointer to second byte of file
    }
  }
  else if (global_offset == 1L){ //if editing second byte of file
    if ( file->mem[0] == 'M' && ! strncmp(data,"Z",1)){ //if M is already in position 0 make sure that Z cannot be put into 1
        fserror = ILLEGAL_MZ;
    }
    else{
        fseek(file->fp, 1L, SEEK_SET); //go to second byte of file
        bytes_written = fwrite(data, 1, num_bytes, file->fp); //add characters added to bytes_written
        if (bytes_written < num_bytes){ //not necessary?
            fserror = WRITE_FAILED;
        }
        file -> mem[1] = ((char*)data)[1]; //set mem[1] equal to second byte
        fseek(file -> fp, 1L + num_bytes, SEEK_SET); //reset position of pointer to third byte of file
    }
  }
  else{
        bytes_written = fwrite(data, 1, num_bytes, file->fp); //passed all tests and can be added to file
        if (bytes_written < num_bytes){ //not necessary?
            fserror = WRITE_FAILED;
        }
  }
  
  return bytes_written;
  
}


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
