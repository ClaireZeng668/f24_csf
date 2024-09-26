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
//break 282
	pushq %rbx
	movq %rsp, %rbx

	# Load the width and height of the image
	movl IMAGE_WIDTH_OFFSET(%rdi), %eax   # Load input_img->width into %eax = cols
    movl IMAGE_HEIGHT_OFFSET(%rdi), %ebx  # Load input_img->height into %ebx = rows

	movl $0, %r11d //%r11 is row for loop
	movl $0, %r12d //%r12 is current for loop
	movl $0, %r13d //%r13 is index for output image
	movq IMAGE_DATA_OFFSET(%rsi), %r14 //r14 is address of output data
	movq IMAGE_DATA_OFFSET(%rdi), %r15 //r15 is address of input data

	//for loop
	jmp .Ltestcondr
	.Ltopr:
		#Calculate end - index of last pixel of row
		movl $0, %r10d //%r10 is end variable
		movl %r11d, %r10d //end = row
		imull %eax, %r10d //end = end * cols
		addl %eax, %r10d //end = end + cols
		subl $1, %r10d //end = end - 1
		movl $0, %r12d //%r12 is current for loop

		jmp .Ltestcondc
		.Ltopc:
			movl $0, %r13d //%r13 is index for output image
			movl %r11d, %r13d //index = row
			imull %ebx, %r13d //index = row*cols
			addl %r12d, %r13d //index = index + current

			//gdb: print *((unsigned *) ($r15 + ($r10 * 4)))
			//gdb print $r8d
			//print/x $r8d

			movl (%r15,%r10,4), %r8d
			movl %r8d, (%r14,%r13,4)
			
			subl $1, %r10d //subtract 1 from end
			addl $1, %r12d //add 1 to current
		.Ltestcondc:
			cmp %r12d, %eax
			jg .Ltopc

		addl $1, %r11d //add 1 to row
	.Ltestcondr:
		cmp %r11d, %ebx
		jg .Ltopr

	popq %rbx
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
imgproc_mirror_v:/*
    # Function parameters
    # %rdi: pointer to input_img (struct Image)
    # %rsi: pointer to output_img (struct Image)

    # Load the width and height of the image
    movl IMAGE_WIDTH_OFFSET(%rdi), %eax   # Load input_img->width into %eax
    movl IMAGE_HEIGHT_OFFSET(%rdi), %ebx  # Load input_img->height into %ebx

    # Set up a loop for each column (width)
mirror_v_loop_cols:
    cmp %eax, 0                           # Check if width (cols) is 0
    jle done                              # Jump to done if no more columns

    # Calculate the vertical mirroring for the current column
    movl $0, %ecx                         # Row index (starting at top)
    subl $1, %ebx                         # height - 1, which gives us the last row

mirror_v_loop_rows:
    cmp %ecx, %ebx                        # Check if we've swapped all rows in the column
    jge next_col                          # Move to next column if all rows are done

    # Perform the swap by loading from input_img and storing to output_img
    mov (%rdi, %ecx, 4), %edx             # Load pixel from input_img at row %ecx
    mov (%rdi, %ebx, 4), %esi             # Load pixel from the mirrored side
    mov %edx, (%rsi, %ebx, 4)             # Store top pixel into output_img
    mov %esi, (%rsi, %ecx, 4)             # Store bottom pixel into output_img

    # Update loop indices
    inc %ecx                              # Move down the column
    dec %ebx                              # Move up the mirrored column
    jmp mirror_v_loop_rows                # Repeat for next row

next_col:
    # Move to the next column
    dec %eax                              # Decrement column index
    jmp mirror_v_loop_cols                # Loop for next column

done:*/
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
	//break 356
	pushq %rbp
	movq %rsp, %rbp
	pushq %rbx
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	subq $24, %rsp

	//Load the width and height of the image
    movl IMAGE_WIDTH_OFFSET(%rdi), %r10d   //Load input_img->width into %eax
    movl IMAGE_HEIGHT_OFFSET(%rdi), %ebx  //Load input_img->height into %ebx

    # Total number of pixels
    imul %ebx, %r10d               //Multiply width * height to get pixel count stored in eax
	movl $0, %r12d //r12 is current index  
	movq %rsi, %rbx //ebx holds output struct
	movq %rdi, %r15 //r15 holds input struct

	movl IMAGE_DATA_OFFSET(%rbx), %r13d //r13 is output->data
	movl IMAGE_DATA_OFFSET(%r15), %r14d //r14 is input->data
	pushq %r10

	jmp .Ltestcond
	.Ltop:
		//print *((unsigned *) ($r15 + ($r10 * 4)))
		//movl (%r14d,%r12d,4), %r15d //r15 is address of pixel to convert (input[current])
		movl (%r14d,%r12d,4), %edi //move address of pixel to first argument
		call to_grayscale //call grayscale on pixel, %eax now hold grayscaled pixel
		movl %eax, (%r13d,%r12d,4) //put grayscaled pixel into output[current] 
		addl $1, %r12d
	.Ltestcond:
		#popq %r10
		cmp %r12d, %r10d       			  # Check if we've processed all pixels
    	jg .Ltop

	addq $24, %rsp
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %rbx
	popq %rbp
    ret
/*
 * Parameters:
 *	%rdi: pointer to pixel to grayscale
 */
.globl get_r
get_r:
	shr $24, %edi
	and $0x000000FFU, %edi
	movl %edi, %eax
	ret
.globl get_g
get_g:
	shr $16, %edi
	and $0x000000FFU, %edi
	movl %edi, %eax
	ret
.globl get_b
get_b:
	shr $8, %edi
	and $0x000000FFU, %edi
	movl %edi, %eax
	ret
.globl get_a
get_a:
	and $0x000000FFU, %edi  
	movl %edi, %eax
	ret
/*
 * Parameters:
 *	%rdi: red component
 *	%rsi: green component
 *	%rdx: blue component
 *	%rcx: alpha component
 */
.globl make_pixel
make_pixel:
	shl $24, %edi
	shl $16, %esi
	shl $8, %edx
	movl $0, %eax
	or %edi, %eax
	or %esi, %eax
	or %edx, %eax
	or %ecx, %eax
	ret
/*
 * Parameters:
 *	%rdi: pointer to pixel
 */
.globl to_grayscale
to_grayscale:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rbx
	pushq %r12

	movl %edi, %ebx //ebx holds pixel parameter

	movl $0, %r12d
	call get_r //get r from pixel in edi, eax holds red component
	imul $79, %eax
	movl %eax, %r12d //r12 hold grayscale value

	movl %ebx, %edi
	call get_g //eax now holds green
	imul $128, %eax
	addl %eax, %r12d

	movl %ebx, %edi
	call get_b //eax now holds blue
	imul $49, %eax
	addl %eax, %r12d
	shr $8, %r12d //r12 holds grayscaled value

	movl %ebx, %edi
	call get_a //eax now holds alpha value
	movl %r12d, %edi
	movl %r12d, %esi
	movl %r12d, %edx
	movl %eax, %ecx
	call make_pixel //eax holds grayscaled pixel

	popq %r12
	popq %rbx
	popq %rbp
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
