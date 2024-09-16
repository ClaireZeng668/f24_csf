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
  int32_t rows = input_img->height;
  int32_t cols = input_img->width;
  uint32_t *orig = input_img->data;

  for (int32_t row = 0; row < rows; row++) {
    int pos = row*cols;
    int end = pos + cols - 1;
    for (int32_t cur = 0; cur <= cols/2; cur++) {
      uint32_t front = orig[pos + cur];
      orig[pos + cur] = orig[end];
      orig[end] = front;
      end--;
    }
  }

  output_img->data = orig;
}

// Mirror input image vertically.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_mirror_v( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
  int32_t rows = input_img->height;
  int32_t cols = input_img->width;
  uint32_t *orig = input_img->data;

  for (int32_t col = 0; col < cols; col++) {
  }
}

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
  // TODO: implement
  return 0;
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
  // TODO: implement
  return 0;
}

uint32_t get_r( uint32_t pixel ) {
  uint32_t add = (1 << 8) - 1;
  return pixel & add;
}
uint32_t get_g( uint32_t pixel ) {
  uint32_t add = (1 << 8) - 1;
  uint32_t altered = pixel >> 8;
  return altered & add;
}
uint32_t get_b( uint32_t pixel ) {
  uint32_t add = (1 << 8) - 1;
  uint32_t altered = pixel >> 16;
  return altered & add;
}
uint32_t get_a( uint32_t pixel ) {
  uint32_t add = (1 << 8) - 1;
  uint32_t altered = pixel >> 24;
  return altered & add;
}
uint32_t make_pixel( uint32_t r, uint32_t g, uint32_t b, uint32_t a ) {
  uint32_t pixel;
  pixel = (a << 24) & (b << 16) & (g << 8) & r;
  return pixel;
}
uint32_t to_grayscale( uint32_t pixel ) {
  uint32_t grey;
  uint32_t red = get_r(pixel);
  uint32_t green = get_g(pixel);
  uint32_t blue = get_b(pixel);
  grey = ((79*red)+(128*green)+(49*blue))/256;
  return grey;
}
uint32_t blend_components( uint32_t fg, uint32_t bg, uint32_t alpha ) {
  uint32_t result = ((alpha*fg)+(255-alpha)*bg)/255;
  return result;
}
uint32_t blend_colors( uint32_t fg, uint32_t bg ) {
  uint32_t red_blend = blend_components(get_r(fg), get_r(bg), get_a(fg));
  uint32_t green_blend = blend_components(get_g(fg), get_g(bg), get_a(fg));
  uint32_t blue_blend = blend_components(get_b(fg), get_b(bg), get_a(fg));
  uint32_t result = make_pixel(red_blend, green_blend, blue_blend, 255);
  return result;
}