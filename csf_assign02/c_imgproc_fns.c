/*
 * C implementations of image processing functions
 * CSF Assignment 2
 * Claire Zeng
 * Prasi Thapa
 * czeng8@jhu.edu
 * pthapa6@jhu.edu
 */
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "imgproc.h"

// TODO: define your helper functions here

// Mirror input image horizontally.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_mirror_h( struct Image *input_img, struct Image *output_img ) {
  if (input_img == NULL || output_img == NULL) return;
  if (input_img->width != output_img->width || input_img->height != output_img->height) return;

  int32_t rows = input_img->height;
  int32_t cols = input_img->width;

  for (int32_t row = 0; row < rows; row++) {
    int end = (row*cols) + cols - 1;
    for (int32_t current = 0; current < cols; current++) {
      output_img->data[(row*cols) + current] = input_img->data[end];
      end--;
    }
  }
}

// Mirror input image vertically.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_mirror_v( struct Image *input_img, struct Image *output_img ) {
  if (input_img == NULL || output_img == NULL) return;
  if (input_img->width != output_img->width || input_img->height != output_img->height) return;

  // TODO: implement
  int32_t rows = input_img->height;
  int32_t cols = input_img->width;

  for (int32_t row = 0; row < rows; row++) {
      for (int32_t col = 0; col < cols; col++) {
          output_img->data[(row * cols) + col] = input_img->data[((rows - row - 1) * cols) + col];
      }
  }

}
/* 0  1  2  3  4
   5  6  7  8  9
  10 11 12 13 14 6*5
  15 16 17 18 19
  20 21 22 23 24
  25 26 27 28 29

  0  1  2  3  4  5
  6  7  8  9  10 11 4*6
  12 13 14 15 16 17
  18 19 20 21 22 23
*/

// Transform image by generating a grid of n x n smaller tiles created by
// sampling every n'th pixel from the original image.
//
// Parameters:
//   input_img  - pointer to original struct Image
//   n          - tiling factor (how many rows and columns of tiles to generate)
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
// Returns:
//   1 if successful, or 0 if either
//     - n is less than 1, or
//     - the output can't be generated because at least one tile would
//       be empty (i.e., have 0 width or height)
int imgproc_tile( struct Image *input_img, int n, struct Image *output_img ) {
  int32_t rows = input_img->height;
  int32_t cols = input_img->width;

  // If n < 1 or n is too large for the image, return failure
  if (n < 1 || !all_tiles_nonempty(cols, rows, n)) {
    return 0;
  }

  //int tile_width, tile_height, x_offset, y_offset;

  // Loop over each tile row and column
  for (int tile_row = 0; tile_row < n; tile_row++) {
    for (int tile_col = 0; tile_col < n; tile_col++) {
      // Determine tile dimensions and offsets
      // tile_width = determine_tile_w(cols, n, tile_col);
      // tile_height = determine_tile_h(rows, n, tile_row);
      // x_offset = determine_tile_x_offset(cols, n, tile_col);
      // y_offset = determine_tile_y_offset(rows, n, tile_row);

      // Copy the sampled tile from the input image to the output
      copy_tile(output_img, input_img, tile_row, tile_col, n);
    }
  }

  return 1;
}

// Convert input pixels to grayscale.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_grayscale( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
  int32_t count = (input_img->height * input_img->width);
  for (int32_t current = 0; current < count; current++) {
    output_img->data[current] = to_grayscale(input_img->data[current]);
  }
}

// Overlay a foreground image on a background image, using each foreground
// pixel's alpha value to determine its degree of opacity in order to blend
// it with the corresponding background pixel.
//
// Parameters:
//   base_img - pointer to base (background) image
//   overlay_img - pointer to overlaid (foreground) image
//   output_img - pointer to output Image
//
// Returns:
//   1 if successful, or 0 if the transformation fails because the base
//   and overlay image do not have the same dimensions
int imgproc_composite( struct Image *base_img, struct Image *overlay_img, struct Image *output_img ) {
  if (base_img->height != overlay_img->height || base_img->width != overlay_img->width) {
    return 0;
  }

  int32_t count = base_img->height * base_img->width;
  for (int32_t i = 0; i < count; i++) {
    uint32_t fg_pixel = overlay_img->data[i];
    uint32_t bg_pixel = base_img->data[i];
    output_img->data[i] = blend_colors(fg_pixel, bg_pixel);
  }

  return 1;
}

int all_tiles_nonempty( int width, int height, int n ) {
  return (width >= n && height >= n && width > 0 && height > 0);
}

int determine_tile_w( int width, int n) {
  int tile_width = width / n;
  return tile_width;
}

int determine_tile_x_offset( int width, int n, int tile_col ) {
  int number_offset = width % n;
  return (number_offset != 0 && tile_col < number_offset);
}

int determine_tile_h( int height, int n ) {
  int tile_height = height / n;
  return tile_height;
}

int determine_tile_y_offset( int height, int n, int tile_row ) {
  int number_offset = height % n;
  return (tile_row < number_offset);
}

void copy_tile( struct Image *out_img, struct Image *img, int tile_row, int tile_col, int n ) {
  // int tile_width = determine_tile_w(img->width, n, tile_col);
  // int tile_height = determine_tile_h(img->height, n, tile_row);
  // int row_offset = tile_row * tile_height;
  // int col_offset = tile_col * tile_width;

  // // Copy pixels from the input image to the tile in the output image
  // for (int row = 0; row < tile_height; row++) {
  //   for (int col = 0; col < tile_width; col++) {
  //     // Ensure we're not reading outside the bounds of the input image
  //     int sampled_row = row_offset + row * n;
  //     int sampled_col = col_offset + col * n;
  //     if (sampled_row < img->height && sampled_col < img->width) {
  //       out_img->data[(tile_row * tile_height + row) * out_img->width + (tile_col * tile_width + col)] =
  //         img->data[sampled_row * img->width + sampled_col];
  //     }
  //   }
  //}
  int total_width_offset = img->width % n;
  int total_height_offset = img->height % n;
  int base_width = determine_tile_w(img->width, n);
  int base_height = determine_tile_h(img->height, n);

  int row_len = base_width + determine_tile_x_offset(img->width, n, tile_col);
  int col_len = base_height  + determine_tile_y_offset(img->height, n, tile_row);


  int orig_start_index = (tile_row)*(img->width) + (tile_col);
  int out_start_index = 0;

  for (int i = 0; i < tile_col; i++) {
    if (total_width_offset > 0) {
      out_start_index = out_start_index + base_width + 1;
      total_width_offset--;
    } else {
      out_start_index = out_start_index + base_width;
    }
  }

  for (int i = 0; i < tile_row; i++) {
    if (total_height_offset > 0) {
      out_start_index = out_start_index + (img->width * (base_height+1));
      total_height_offset--;
    } else {
      out_start_index = out_start_index + (img->width * base_height);
    }
  }

  int orig_pos = orig_start_index;
  int out_pos = out_start_index;
  for (int row = 0; row < row_len; row++) {
    for (int col = 0; col < col_len; col++) {
      //printf("copying output: %d, input: %d\n", out_pos, orig_pos);
      out_img->data[out_pos] = img->data[orig_pos];
      out_pos+=img->width;
      orig_pos+=(n*img->width);
    }
    out_pos = out_start_index+row+1;
    orig_pos = orig_start_index+((row+1)*n);
  }
  //printf("tile done\n");
}


uint32_t get_r( uint32_t pixel ) {
  uint32_t add = 0x000000FFU;
  uint32_t altered = pixel >> 24;
  return altered & add;
}
uint32_t get_g( uint32_t pixel ) {
  uint32_t add = 0x000000FFU;
  uint32_t altered = pixel >> 16;
  return altered & add;
}
uint32_t get_b( uint32_t pixel ) {
  uint32_t add = 0x000000FFU;
  uint32_t altered = pixel >> 8;
  return altered & add;
}
uint32_t get_a( uint32_t pixel ) {
  uint32_t add = 0x000000FFU;
  return pixel & add;
}



//changed this to using bitwise OR
uint32_t make_pixel( uint32_t r, uint32_t g, uint32_t b, uint32_t a ) {
    return (r << 24) | (g << 16) | (b << 8) | a;
}

uint32_t to_grayscale(uint32_t pixel) {
  uint32_t red = get_r(pixel);
  uint32_t green = get_g(pixel);
  uint32_t blue = get_b(pixel);
  uint32_t gray = (79 * red + 128 * green + 49 * blue) / 256;
  return make_pixel(gray, gray, gray, get_a(pixel));
}


uint32_t blend_components( uint32_t fg, uint32_t bg, uint32_t alpha ) {
  uint32_t result = ((alpha*fg)+(255-alpha)*bg)/255;
  return result;
}

uint32_t blend_colors(uint32_t fg, uint32_t bg) {
  uint32_t red_blend = blend_components(get_r(fg), get_r(bg), get_a(fg));
  uint32_t green_blend = blend_components(get_g(fg), get_g(bg), get_a(fg));
  uint32_t blue_blend = blend_components(get_b(fg), get_b(bg), get_a(fg));
  return make_pixel(red_blend, green_blend, blue_blend, 255);
}