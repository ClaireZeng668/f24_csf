// C main function for image processing program

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "imgproc.h"

void usage( const char *progname ) {
  fprintf( stderr, "Error: invalid command-line arguments\n" );
  fprintf( stderr, "Usage: %s <transform> <input img> <output img> [args...]\n", progname );
  exit( 1 );
}

// Make a new empty image the same dimensions as given image
struct Image *create_output_img( struct Image *input_img ) {
  struct Image *out_img;

  // Allocate Image object
  out_img = (struct Image *) malloc( sizeof( struct Image ) );
  if ( out_img == NULL )
    return NULL;

  // Set data to NULL for now
  out_img->data = NULL;

  // Attempt to initialize the Image object by calling img_init
  if ( img_init( out_img, input_img->width, input_img->height ) != IMG_SUCCESS ) {
    free( out_img );
    return NULL;
  }

  // Success!
  return out_img;
}

// Free memory allocated to given Image object
void cleanup_image( struct Image *img ) {
  if ( img != NULL ) {
    img_cleanup( img );
    free( img );
  }
}

int main( int argc, char **argv ) {
  if ( argc < 4 )
    usage( argv[0] );

  const char *transformation = argv[1];
  const char *input_filename = argv[2];
  const char *output_filename = argv[3];

  // Allocate and read the input image
  struct Image *input_img = (struct Image *) malloc( sizeof( struct Image ) );
  if ( input_img == NULL ) {
    fprintf( stderr, "Error: couldn't allocate input image\n" );
    exit( 1 );
  }
  if ( img_read( input_filename, input_img ) != IMG_SUCCESS ) {
    fprintf( stderr, "Error: couldn't read input image\n" );
    free( input_img );
    return 1;
  }

  // Create output Image object
  struct Image *output_img = create_output_img( input_img );
  if ( output_img == NULL ) {
    fprintf( stderr, "Error: couldn't create output image object\n" );
    cleanup_image( input_img );
    return 1;
  }

  // Set to true if an error occurs
  bool error_occurred = false;

  // Execute the appropriate transformation
  if ( strcmp( transformation, "mirror_h" ) == 0 ) {
    imgproc_mirror_h( input_img, output_img );
  } else if ( strcmp( transformation, "mirror_v" ) == 0 ) {
    imgproc_mirror_v( input_img, output_img );
  } else if ( strcmp( transformation, "tile" ) == 0 ) {
    if ( argc != 5 ) {
      fprintf( stderr, "Error: tile transformation needs tiling factor argument\n" );
      error_occurred = true;
    } else {
      int n;
      if ( sscanf( argv[4], "%d", &n ) != 1 ) {
        fprintf( stderr, "Error: could not parse tiling factor\n" );
        error_occurred = true;
      } else {
        int success = imgproc_tile( input_img, n, output_img );
        if ( !success ) {
          fprintf( stderr, "Error: tile transformation failed\n" );
          error_occurred = true;
        }
      }
    }
  } else if ( strcmp( transformation, "grayscale" ) == 0 ) {
    imgproc_grayscale( input_img, output_img );
    if ( output_img == NULL ) {
      fprintf( stderr, "Error: grayscale transformation failed\n" );
      error_occurred = true;
    }
  } else if ( strcmp( transformation, "composite" ) == 0 ) {
    if ( argc != 5 ) {
      fprintf( stderr, "Error: composite transformation needs overlay image argument\n" );
      error_occurred = true;
    } else {
      struct Image *overlay_img = (struct Image *) malloc( sizeof(struct Image) );
      if ( overlay_img == NULL ) {
        fprintf( stderr, "Error: failed to allocate Image object\n" );
        error_occurred = true;
      } else {
        // Set overlay_img->data to NULL so that cleanup_image
        // will work correctly even if overlay image can't be read
        overlay_img->data = NULL;

        if ( img_read( argv[4], overlay_img ) != IMG_SUCCESS ) {
          fprintf( stderr, "Error: could not read overlay image\n" );
          error_occurred = true;
        } else {
          int success = imgproc_composite( input_img, overlay_img, output_img );
          if ( !success ) {
            fprintf( stderr, "Error: composite transformation failed\n" );
            error_occurred = true;
          }
        }
      }
      // ensure memory of overlay image is cleaned up
      cleanup_image( overlay_img );
    }
  } else {
    fprintf( stderr, "Error: unknown transformation '%s'\n", transformation );
    error_occurred = true;
  }

  if ( !error_occurred ) {
    // Write output image
    if ( img_write( output_filename, output_img ) != IMG_SUCCESS ) {
      fprintf( stderr, "Error: couldn't write output image\n" );
      error_occurred = true;
    }
  }

  cleanup_image( input_img );
  cleanup_image( output_img );

  return error_occurred ? 1 : 0;
}
