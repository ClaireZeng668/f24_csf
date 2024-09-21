/*
 * Header for image processing API functions (imgproc_mirror_h, etc.) as well as any helper functions they rely on.
 * CSF Assignment 2
 * Claire Zeng
 * Prasi Thapa
 * czeng8@jhu.edu
 * pthapa6@jhu.edu
 */


#ifndef IMGPROC_H
#define IMGPROC_H

#include "image.h" // for struct Image and related functions

// Mirror input image horizontally.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_mirror_h( struct Image *input_img, struct Image *output_img );

// Mirror input image vertically.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_mirror_v( struct Image *input_img, struct Image *output_img );

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
int imgproc_tile( struct Image *input_img, int n, struct Image *output_img );

// Convert input pixels to grayscale.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_grayscale( struct Image *input_img, struct Image *output_img );

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
int imgproc_composite( struct Image *base_img, struct Image *overlay_img, struct Image *output_img );

// TODO: add prototypes for your helper functions

/*
 * Check whether the given dimensions are compatible with the given tiling factor
 *
 * Parameters:
 *   width - int value corresponding to the image width
 *   height - int value corresponding to the image width
 *   n - int value corresponding to the tiling factor
 *
 * Returns:
 *   a 1 or 0 of whether the given dimensions and tiling factor are compatible (1 meaning they are compatible)
 */
int all_tiles_nonempty( int width, int height, int n );

/*
 * Determine the width of the small tiles
 *
 * Parameters:
 *   width - int value corresponding to the image width
 *   n - int value corresponding to the tiling factor
 *   tile_col - int value corresponding to the image width
 *
 * Returns:
 *   an integer tile width value
 */
int determine_tile_w( int width, int n);

/*
 * Determine the width offset of the small tiles
 *
 * Parameters:
 *   width - int value corresponding to the image width
 *   n - int value corresponding to the tiling factor
 *   tile_col - int value corresponding to the image width
 *
 * Returns:
 *   an integer of how many small tiles need a width offset
 */
int determine_tile_x_offset( int width, int n, int tile_col );

/*
 * Determine the height of the small tiles
 *
 * Parameters:
 *   height - int value corresponding to the image width
 *   n - int value corresponding to the tiling factor
 *   tile_row - int value corresponding to the image width
 *
 * Returns:
 *   an integer tile height value
 */
int determine_tile_h( int height, int n);

/*
 * Determine the height offset of the small tiles
 *
 * Parameters:
 *   height - int value corresponding to the image width
 *   n - int value corresponding to the tiling factor
 *   tile_row - int value corresponding to the image width
 *
 * Returns:
 *   an integer of how many small tiles need a height offset
 */
int determine_tile_y_offset( int height, int n, int tile_row );

/*
 * Populate the tile at the specifiec row and col from the image to the output image
 *
 * Parameters:
 *   out_imag - pointer to image of the tile destination
 *   img - pointer to image of the source
 *   tile_row - the row number of the result tile
 *   tile_col - the column number of the result tile
 *   n - the tiling factor
 */
void copy_tile( struct Image *out_img, struct Image *img, int tile_row, int tile_col, int n );

/*
 * Calculate the starting index to populate the output image based on the current 
 * tile row, column, and tiling factor
 *
 * Parameters:
 *   img - pointer to image of the source
 *   tile_row - the row number of the result tile
 *   tile_col - the column number of the result tile
 *   n - the tiling factor
* Returns:
 *   an integer index indictaing where to start filling the output image
 */
int calculate_starting_index (struct Image *img, int tile_row, int tile_col, int n ) ;

/*
 * Return a uint32_t value correpsonding to the 8 bit red/green/blue
 * component of a pixel represented by a uint32_t value
 *
 * Parameters:
 *   pixel - uint32_t value corresponding to a pixel
 *
 * Returns:
 *   a uint32_t value of red/green/blue extracted from pixel
 */
uint32_t get_r( uint32_t pixel );
uint32_t get_g( uint32_t pixel );
uint32_t get_b( uint32_t pixel );
uint32_t get_a( uint32_t pixel );

/*
 * Return a uint32_t value correpsonding to the 32 bit value
 * containing the red, green, blue, and alpha component of a pixel
 *
 * Parameters:
 *   r - uint32_t value corresponding to the red component
 *   g - uint32_t value corresponding to the green component
 *   b - uint32_t value corresponding to the blue component
 *   a - uint32_t value corresponding to the alpha component
 *
 * Returns:
 *   a uint32_t value representing the color of a pixel
 */
uint32_t make_pixel( uint32_t r, uint32_t g, uint32_t b, uint32_t a );

/*
 * Return a uint32_t value representing the greyscale transformation
 * of the given pixel
 *
 * Parameters:
 *   pixel - uint32_t value corresponding to a pixel
 *
 * Returns:
 *   a uint32_t value representing the greayscale transformation of the pixel
 */
uint32_t to_grayscale( uint32_t pixel );

/*
 * Return a uint32_t value representing the result of blending
 * the input pixel components.
 *
 * Parameters:
 *   fg - uint32_t value corresponding to the foreground pixel component
 *   bg - uint32_t value corresponding to the background pixel component
 *   alpha - uint32_t value corresponding to the alpha of the foreground
 *
 * Returns:
 *   a uint32_t value representing the blended pixel component result
 */
uint32_t blend_components( uint32_t fg, uint32_t bg, uint32_t alpha );

/*
 * Return a uint32_t value representing the result of blending
 * the input values.
 *
 * Parameters:
 *   fg - uint32_t value corresponding to the foreground pixel
 *   bg - uint32_t value corresponding to the background pixel
 *
 * Returns:
 *   a uint32_t value representing the blended pixel result
 */
uint32_t blend_colors( uint32_t fg, uint32_t bg );

#endif // IMGPROC_H
