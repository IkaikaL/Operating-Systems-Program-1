#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// filesystem error code
typedef enum  {
  NONE, 
  OPEN_FAILED,
  CLOSE_FAILED,
  READ_FAILED,
  WRITE_FAILED,
  ILLEGAL_MZ
} FSError;


// file handle type
// typedef FILE* File;


// file handle type
typedef struct _FileInternal
{
    FILE* fp;
    char mem[2]; //allows modeling writes to detect MZ
}FileInternal;


// file handle type
typedef FileInternal* File;




// seek anchors
typedef enum {
  BEGINNING_OF_FILE,
  CURRENT_POSITION, 
  END_OF_FILE
} SeekAnchor;


// function prototypes for system calls


// open file with pathname 'name'. Files are always opened for
// read/write access.  Always sets 'fserror' global.
File open_file(char *name);


// close file with handle 'file'.  Always sets 'fserror' global.
void close_file(File file);


// read 'num_bytes' into 'data' from 'file' at 'offset' bytes from a
// particular 'start'-ing position.  Returns the number of bytes
// read. Always sets 'fserror' global.
unsigned long read_file_from(File file, void *data, unsigned long num_bytes,
                             SeekAnchor start, long offset);


// write 'num_bytes' from 'data' into 'file' at 'offset' bytes from a 
// particular 'start'-ing position. Returns the number of bytes
// written.  Always sets 'fserror' global.
unsigned long write_file_at(File file, void *data, unsigned long num_bytes, 
                            SeekAnchor start, long offset);


// describe current filesystem error code 
void fs_print_error(void);


// GLOBALS //


// filesystem error code set (set by each function)
extern FSError fserror;