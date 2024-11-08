/*
  * parsort.c
  *
  * Parallel sorting using fork/join parallelism.
  *
  * Authors: Prasi Thapa, Claire Zeng\
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>

// Function for comparing two elements, used in partitioning
int compare( const void *left, const void *right );

// Function for swapping two elements in an array
void swap( int64_t *arr, unsigned long i, unsigned long j );

// Partition function to arrange elements based on a pivot
unsigned long partition( int64_t *arr, unsigned long start, unsigned long end );

// Recursive quicksort function with parallel execution using fork
int quicksort( int64_t *arr, unsigned long start, unsigned long end, unsigned long par_threshold );


int main( int argc, char **argv ) {
  // check if the number of arguments is correct
  unsigned long par_threshold;
  if ( argc != 3 || sscanf( argv[2], "%lu", &par_threshold ) != 1 ) {
    fprintf( stderr, "Usage: parsort <file> <par threshold>\n" );
    exit( 1 );
  }

  // Open the file for input
  int fd;
  const char *filename = argv[1];
  // open the named file
  fd = open(filename, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Couldn't open %s\n", argv[1]);
    return 1;
  }

  unsigned long num_elements; //file_size not used?
  // determine the file size and number of elements
  struct stat statbuf;
  int rc = fstat( fd, &statbuf );
  if ( rc != 0 ) {
    fprintf(stderr, "Error: fstat failed\n");
    return 1;
  }
  int file_size_in_bytes = statbuf.st_size;  // indicates the number of bytes in the file
  num_elements = file_size_in_bytes/sizeof(int64_t);

  


  // mmap the file data
  int64_t *arr;
  arr = mmap( NULL, file_size_in_bytes, PROT_READ | PROT_WRITE,
    MAP_SHARED, fd, 0 );
  close( fd ); // file can be closed after mmap
  if ( arr == MAP_FAILED ) {
    fprintf( stderr, "Error: mmap failed\n" );
    return 1;
  }

  // Sort the data!
  int success;
  success = quicksort( arr, 0, num_elements, par_threshold );
  if ( !success ) {
    fprintf( stderr, "Error: sorting failed\n" );
    return 1;
  }

  // Unmap the file data
  munmap( arr, file_size_in_bytes );

  return 0;
}

/*
* Function to compare two elements.
* Can be used as a comaprater for a call to qsort.
* Parameters:
*   left - pointer to left element
*   right - pointer to right element
* Return:
*   negative if *left < *right,
*   positive if *left > *right,
*   0 if *left == *right
*/
int compare( const void *left, const void *right ) {
  int64_t left_elt = *(const int64_t *)left, right_elt = *(const int64_t *)right;
  if ( left_elt < right_elt )
    return -1;
  else if ( left_elt > right_elt )
    return 1;
  else
    return 0;
}

/*
* Function to swap two elements in an array.
* Parameters:
*   arr - pointer to the first element of the array
*   i - index of the first element to swap
*   j - index of the second element to swap
*/
void swap( int64_t *arr, unsigned long i, unsigned long j ) {
  int64_t tmp = arr[i];
  arr[i] = arr[j];
  arr[j] = tmp;
}

/*
* Function to partition a region of an array.
* Parameters:
*   arr - pointer to the first element of the array
*   start - inclusive lower bound index
*   end - exclusive upper bound index
* Return:
*   index of the pivot element, which is globally in the correct place;
*   all elements to the left of the pivot will have values less than
*   the pivot element, and all elements to the right of the pivot will
*   have values greater than or equal to the pivot
*/
unsigned long partition( int64_t *arr, unsigned long start, unsigned long end ) {
  assert( end > start );

  // choose the middle element as the pivot
  unsigned long len = end - start;
  assert( len >= 2 );
  unsigned long pivot_index = start + (len / 2);
  int64_t pivot_val = arr[pivot_index];

  // stash the pivot at the end of the sequence
  swap( arr, pivot_index, end - 1 );

  // partition all of the elements based on how they compare
  // to the pivot element: elements less than the pivot element
  // should be in the left partition, elements greater than or
  // equal to the pivot should go in the right partition
  unsigned long left_index = start,
                right_index = start + ( len - 2 );

  while ( left_index <= right_index ) {
    // extend the left partition?
    if ( arr[left_index] < pivot_val ) {
      ++left_index;
      continue;
    }

    // extend the right partition?
    if ( arr[right_index] >= pivot_val ) {
      --right_index;
      continue;
    }

    // left_index refers to an element that should be in the right
    // partition, and right_index refers to an element that should
    // be in the left partition, so swap them
    swap( arr, left_index, right_index );
  }

  // at this point, left_index points to the first element
  // in the right partition, so place the pivot element there
  // and return the left index, since that's where the pivot
  // element is now
  swap( arr, left_index, end - 1 );
  return left_index;
}

/*
* Function to recursively sort a region of an array.
* Note that the only reason that sorting should fail is 
* if a child process can't be created or if there is any
* other system call failure.
*
* Parameters:
*   arr - pointer to the first element of the array
*   start - inclusive lower bound index
*   end - exclusive upper bound index
*   par_threshold - if the number of elements in the region is less
*                   than or equal to this threshold, sort sequentially,
*                   otherwise sort in parallel using child processes
* Return:
*   1 if the sort was successful, 0 otherwise
*/
int quicksort( int64_t *arr, unsigned long start, unsigned long end, unsigned long par_threshold ) {
  assert( end >= start );
  unsigned long len = end - start;

  // Base case: if there are fewer than 2 elements to sort,
  // do nothing
  if ( len < 2 ) {
    return 1;
  }
  // Base case: if number of elements is less than or equal to
  // the threshold, sort sequentially using qsort
  if ( len <= par_threshold ) {
    qsort( arr + start, len, sizeof(int64_t), compare );
    return 1;
  }

  // Partition
  unsigned long mid = partition( arr, start, end );

  // Recursively sort the left and right partitions
  int left_success, right_success;
  // TODO: modify this code so that the recursive calls execute in child

  pid_t child_pidl = fork();
  if ( child_pidl == 0 ) {
    // executing in the child
    left_success = quicksort( arr, start, mid, par_threshold );
    if (left_success) {
      exit( 0 );
    }
    else
      exit( 1 );
  } else if ( child_pidl < 0 ) {
    // fork failed
    fprintf( stderr, "Error: fork failed\n" );
    return 0;
  }

  pid_t child_pidr = fork();
  if ( child_pidr == 0 ) {
    // executing in the child
    right_success = quicksort( arr, mid + 1, end, par_threshold );
    if (right_success) {
      exit( 0 );
    }
    else
      exit( 1 );
  } else if ( child_pidr < 0 ) {
    // fork failed
    fprintf( stderr, "Error: fork failed\n" );
    return 0;
  }

  int rcl, wstatusl;
  rcl = waitpid( child_pidl, &wstatusl,0 );
  if ( rcl < 0 ) {
    // waitpid failed
    fprintf( stderr, "Error: waitpid failed\n" );
    return 1;
  } else {
    // check status of child
    if ( !WIFEXITED( wstatusl ) ) {
      // child did not exit normally (e.g., it was terminated by a signal)
      fprintf( stderr, "Error: child exit failed\n" );
      return 0;
    } else if ( WEXITSTATUS( wstatusl ) != 0 ) {
      // child exited with a non-zero exit code
      fprintf( stderr, "Error: non zero child exit code\n" );
      return 0;
    }
  }

int rcr, wstatusr;
  rcr = waitpid( child_pidr, &wstatusr,0 );
  if ( rcr < 0 ) {
    // waitpid failed
    fprintf( stderr, "Error: waitpid failed\n" );
    return 1;
  } else {
    // check status of child
    if ( !WIFEXITED( wstatusr ) ) {
      // child did not exit normally (e.g., it was terminated by a signal)
      fprintf( stderr, "Error: child exit failed\n" );
      return 0;
    } else if ( WEXITSTATUS( wstatusr ) != 0 ) {
      // child exited with a non-zero exit code
      fprintf( stderr, "Error: non zero child exit code\n" );
      return 0;
    }
  }

  return 1;
}
