/*
 * x86-64 assembly language implementations of functions
 */

	.section .text

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/*
 * TODO: define your helper functions here.
 * Don't forget to use the .globl directive to make
 * them visible to the test code.
 */

.globl all_tiles_nonempty
all_tiles_nonempty:
	ret
.globl determine_tile_w
determine_tile_w:
	ret
.globl determine_tile_h
determine_tile_h:
	ret
.globl determine_tile_x_offset
determine_tile_x_offset:
	ret
.globl determine_tile_y_offset
determine_tile_y_offset:
	ret
.globl copy_tile
copy_tile:
	ret
.globl calculate_starting_index
calculate_starting_index:
	ret
.globl get_r
get_r:
	ret
.globl get_g
get_g:
	ret
.globl get_b
get_b:
	ret
.globl get_a
get_a:
	ret
.globl make_pixel
make_pixel:
	ret
.globl to_grayscale
to_grayscale:
	ret
.globl blend_components
blend_components:
	ret
.globl blend_colors
blend_colors:
	ret

/*
 * Implementations of API functions
 */

/*
 * void imgproc_mirror_h( struct Image *input_img, struct Image *output_img );
 *
 * Transform image by mirroring its pixels horizontally.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 */
	.globl imgproc_mirror_h
imgproc_mirror_h:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	pushq %rbx //%rbx is cols

	movl 4(%rdi), %ebp
	movl (%rdi), %ebx

	movl $0, %r10d //%r10 is end variable
	movl $0, %r11d //%r11 is row for loop
	movl $0, %r13d //%r13 is index for output image

	//for loop
	.Ltopr:
		movl %r11d, %r10d //end = row
		imull %ebx, %r10d //end = end * cols
		addl %ebx, %r10d //end = end + cols
		subl $1, %r10d //end = end - 1
		movl $0, %r12d //%r12 is current for loop
		jmp .Ltestcondc
		.Ltopc:
			movl %r11d, %r13d //index = row
			imull %ebx, %r13d //index = row*cols
			addl %r12d, %r13d //index = index + current
			
			movl IMAGE_DATA_OFFSET(%rdi), %r15d //r15 is address of input data
			movl IMAGE_DATA_OFFSET(%rsi), %r14d //r14 is address of output data
			
			movl (%r15d,%r10d,4), %r15d //r15 holds address of input to copy
			movl %r15d, (%r14d,%r13d,4) //r14 holds address of output pixel to change
			
			subl $1, %r10d //subtract 1 from end
			addl $1, %r12d //add 1 to current
		.Ltestcondc:
			cmp %r12d, %ebx
			jge .Ltopc
		addl $1, %r11d //add 1 to row
	.Ltestcondr:
		cmp %r11d, %ebp
		jge .Ltopr

	popq %rbx
	addq $8, %rsp
	popq %rbp
	ret

/*
 * void imgproc_mirror_v( struct Image *input_img, struct Image *output_img );
 *
 * Transform image by mirroring its pixels vertically.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 */
	.globl imgproc_mirror_v
imgproc_mirror_v:
	/* TODO: implement */
	ret

/*
 * int imgproc_tile( struct Image *input_img, int n, struct Image *output_img );
 *
 * Transform image by generating a grid of n x n smaller tiles created by
 * sampling every n'th pixel from the original image.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %esi - tiling factor (how many rows and columns of tiles to generate)
 *   %rdx - pointer to the output Image (in which the transformed
 *          pixels should be stored)
 *
 * Returns (in %eax):
 *   1 if successful, or 0 if either
 *     - n is less than 1, or
 *     - the output can't be generated because at least one tile would
 *       be empty (i.e., have 0 width or height)
 */
	.globl imgproc_tile
imgproc_tile:
	/* TODO: implement (this stub implementation just returns 0) */
	xorl %eax, %eax
	ret

/*
 * void imgproc_grayscale( struct Image *input_img, struct Image *output_img );
 *
 * Transform image by converting each pixel to grayscale.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 */
	.globl imgproc_grayscale
imgproc_grayscale:
	/* TODO: implement */
	ret

/*
 * int imgproc_composite( struct Image *base_img, struct Image *overlay_img );
 *
 * Overlay a foreground image on a background image, using each foreground
 * pixel's alpha value to determine its degree of opacity in order to blend
 * it with the corresponding background pixel.
 *
 * Parameters:
 *   %rdi - pointer to base (background) image
 *   %rsi - pointer to overlaid (foreground) image
 *   %rdx - pointer to output Image
 *
 * Returns:
 *   1 if successful, or 0 if the transformation fails because the base
 *   and overlay image do not have the same dimensions
 */
	.globl imgproc_composite
imgproc_composite:
	/* TODO: implement (this stub implementation just returns 0) */
	xorl %eax, %eax
	ret

/*
vim:ft=gas:
*/
