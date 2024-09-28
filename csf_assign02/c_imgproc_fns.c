
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


void imgproc_mirror_h( struct Image *input_img, struct Image *output_img ) {
  //check if the input and output images are valid and not null
  if (!input_img || !output_img || !input_img->data || !output_img->data) return;
  //check if the input and output images have the same dimensions
  if (input_img->width != output_img->width || input_img->height != output_img->height) return;

  int32_t rows = input_img->height;
  int32_t cols = input_img->width;

  //iterate through each row
  for (int32_t row = 0; row < rows; row++) {
    int end = (row * cols) + cols - 1;  //index of the last pixel in the row
    //iterate through each pixel in the row and copy the pixel to the output image
    for (int32_t current = 0; current < cols; current++) {
      output_img->data[(row*cols) + current] = input_img->data[end];
      end--;
    }
  }
}

void imgproc_mirror_v( struct Image *input_img, struct Image *output_img ) {
  //check if the input and output images are valid and not null
  if (!input_img || !output_img || !input_img->data || !output_img->data) return;
  //check if the input and output images have the same dimensions
  if (input_img->width != output_img->width || input_img->height != output_img->height) return;

  uint32_t rows = input_img->height;
  uint32_t cols = input_img->width;

  //iterate through each row
  for (uint32_t row = 0; row < rows; row++) {
      uint32_t opposite_row = rows - row - 1;  //index of the opposite row
      //iterate through each pixel in the row and copy the pixel to the output image
      for (uint32_t col = 0; col < cols; col++) {
          output_img->data[(row * cols) + col] = input_img->data[(opposite_row * cols) + col];
      }
  }
}


int imgproc_tile( struct Image *input_img, int n, struct Image *output_img ) {
  int32_t rows = input_img->height;
  int32_t cols = input_img->width;

  //if n < 1 or n is too large for the image, return failure
  //validate that all tiles are nonempty
  if (n < 1 || !all_tiles_nonempty(cols, rows, n)) {
    return 0;
  }

  //loop over each tile row and column
  for (int tile_row = 0; tile_row < n; tile_row++) {
    for (int tile_col = 0; tile_col < n; tile_col++) {
      //copy the sampled tile from the input image to the output
      copy_tile(output_img, input_img, tile_row, tile_col, n);
    }
  }

  return 1;
}


void imgproc_grayscale( struct Image *input_img, struct Image *output_img ) {
  int32_t count = (input_img->height * input_img->width);  //total number of pixels in the image
  //iterate through each pixel in the image and convert it to grayscale
  for (int32_t current = 0; current < count; current++) {
    output_img->data[current] = to_grayscale(input_img->data[current]);
  }
}


int imgproc_composite( struct Image *base_img, struct Image *overlay_img, struct Image *output_img ) {
  if (base_img->height != overlay_img->height || base_img->width != overlay_img->width) {
    return 0;
  }

  int32_t count = base_img->height * base_img->width;  //total number of pixels in the image
  //iterate through each pixel in the image and blend the overlay image with the base image
  for (int32_t i = 0; i < count; i++) {
    uint32_t fg_pixel = overlay_img->data[i];  //foreground pixel
    uint32_t bg_pixel = base_img->data[i];  //background pixel
    output_img->data[i] = blend_colors(fg_pixel, bg_pixel);  //blend the two pixels, store in output image
  }

  return 1;
}

//helper functions for tilting

//ensure that all tiles are nonempty (nonzero width and height)
int all_tiles_nonempty( int width, int height, int n ) {
  return (width >= n && height >= n && width > 0 && height > 0);
}

//determine the width of tiles
int determine_tile_w( int width, int n) {
  int tile_width = width / n;
  return tile_width;
}

//determine the x offset of tiles
int determine_tile_x_offset( int width, int n, int tile_col ) {
  int number_offset = width % n;
  return (number_offset != 0 && tile_col < number_offset);
}

//determine the height of tiles
int determine_tile_h( int height, int n ) {
  int tile_height = height / n;
  return tile_height;
}

//determine the y offset of tiles
int determine_tile_y_offset( int height, int n, int tile_row ) {
  int number_offset = height % n;
  return (tile_row < number_offset);
}

//copy a tile from the input image to the output image
void copy_tile( struct Image *out_img, struct Image *img, int tile_row, int tile_col, int n ) {
  int row_len = determine_tile_w(img->width, n) + determine_tile_x_offset(img->width, n, tile_col);
  int col_len = determine_tile_h(img->height, n)  + determine_tile_y_offset(img->height, n, tile_row);

  int orig_start_index = 0;
  int out_start_index = calculate_starting_index(img, tile_row, tile_col, n);

  //copy data from input tile to output tile
  int orig_pos = orig_start_index;
  int out_pos = out_start_index;
  for (int row = 0; row < row_len; row++) {
    for (int col = 0; col < col_len; col++) {
      out_img->data[out_pos] = img->data[orig_pos];  //copy pixel data
      out_pos+=img->width;
      orig_pos+=(n*img->width);
    }
    out_pos = out_start_index+row+1;
    orig_pos = orig_start_index+((row+1)*n);
  }
}

//get the output image index based on tile row, tile col, and tiling factor n
int calculate_starting_index (struct Image *img, int tile_row, int tile_col, int n ) {
  int total_width_offset = img->width % n;  //offset for the width of the image if it is not divisible by n
  int total_height_offset = img->height % n;  //offset for the height of the image if it is not divisible by n
  int out_start_index = 0;
  //calculate start index for output tile
  for (int i = 0; i < tile_col; i++) {
    if (total_width_offset > 0) {
      out_start_index = out_start_index + determine_tile_w(img->width, n) + 1;
      total_width_offset--;
    } else {
      out_start_index = out_start_index + determine_tile_w(img->width, n);
    }
  }

  //calculate start index for row
  for (int i = 0; i < tile_row; i++) {
    if (total_height_offset > 0) {
      out_start_index = out_start_index + (img->width * (determine_tile_h(img->height, n)+1));
      total_height_offset--;
    } else {
      out_start_index = out_start_index + (img->width * determine_tile_h(img->height, n));
    }
  }
  return out_start_index;
}

//functions to extract RBGA values from 32-bit pixel
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

//combining RGBA values into a single 32-bit pixel, using bitwise OR
uint32_t make_pixel( uint32_t r, uint32_t g, uint32_t b, uint32_t a ) {
    return (r << 24) | (g << 16) | (b << 8) | a;
}

//convert a pixel to grayscale
uint32_t to_grayscale(uint32_t pixel) {
  uint32_t red = get_r(pixel);
  uint32_t green = get_g(pixel);
  uint32_t blue = get_b(pixel);
  uint32_t gray = (79 * red + 128 * green + 49 * blue) / 256;
  return make_pixel(gray, gray, gray, get_a(pixel));
}

//blend two colors components together based on alpha value
uint32_t blend_components( uint32_t fg, uint32_t bg, uint32_t alpha ) {
  uint32_t result = ((alpha*fg)+(255-alpha)*bg)/255;
  return result;
}

//blend two colors together based on foreground pixel's alpha value
uint32_t blend_colors(uint32_t fg, uint32_t bg) {
  uint32_t red_blend = blend_components(get_r(fg), get_r(bg), get_a(fg));
  uint32_t green_blend = blend_components(get_g(fg), get_g(bg), get_a(fg));
  uint32_t blue_blend = blend_components(get_b(fg), get_b(bg), get_a(fg));
  return make_pixel(red_blend, green_blend, blue_blend, 255);
}
/*
imgproc_mirror_h:
//break 282
	pushq %rbp
	movq %rsp, %rbp
	pushq %rbx
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	# Load the width and height of the image
	movl IMAGE_WIDTH_OFFSET(%rdi), %eax   # Load input_img->width into %eax = cols
    movl IMAGE_HEIGHT_OFFSET(%rdi), %ebx  # Load input_img->height into %ebx = rows

	movl $0, %r10d //%r10 is end variable
	movl $0, %r11d //%r11 is row for loop
	movl $0, %r12d //%r12 is current for loop
	movl $0, %r13d //%r13 is index for output image
	
	movq IMAGE_DATA_OFFSET(%rsi), %r14 //r14 is address of output data
	movq IMAGE_DATA_OFFSET(%rdi), %r15 //r15 is address of input data

	//for loop
	jmp .Ltestcondr_h
	.Ltopr_h:
		#Calculate end - index of last pixel of row
		movl $0, %r10d //%r10 is end variable
		movl %r11d, %r10d //end = row
		imul %eax, %r10d //end = end * cols
		addl %eax, %r10d //end = end + cols
		subl $1, %r10d //end = end - 1
		movl $0, %r12d //%r12 is current for loop

		jmp .Ltestcondc_h
		.Ltopc_h:
			movl $0, %r13d //%r13 is index for output image
			movl %r11d, %r13d //index = row
			imul %eax, %r13d //index = row*cols
			addl %r12d, %r13d //index = index + current

			//gdb: print *((unsigned *) ($r15 + ($r10 * 4)))
			//gdb print $r8d
			//print/x $r8d

			movl (%r15,%r10,4), %r8d
			movl %r8d, (%r14,%r13,4)
			
			subl $1, %r10d //subtract 1 from end
			addl $1, %r12d //add 1 to current
		.Ltestcondc_h:
			cmp %r12d, %eax
			jg .Ltopc_h

		addl $1, %r11d //add 1 to row
	.Ltestcondr_h:
		cmp %r11d, %ebx
		jg .Ltopr_h

	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %rbx
	popq %rbp
	ret

*/