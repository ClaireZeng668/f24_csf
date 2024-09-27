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

/* the following functions will be implemented in MS3 */
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

    movl $0, %r11d                        # %r11 = top row index
    movl %ebx, %r10d                      # %r10 = bottom row index (start from the last row)
    subl $1, %r10d                        # bottom = rows - 1

    movq IMAGE_DATA_OFFSET(%rsi), %r14     # r14 = address of output data
    movq IMAGE_DATA_OFFSET(%rdi), %r15     # r15 = address of input data

    # Vertical mirror logic
    jmp .Ltestcondr_v
    .Ltopr_v:
        movl $0, %r12d                    # %r12 = current column (start column for this row)

        jmp .Ltestcondc_v
        .Ltopc_v:
            # Calculate the pixel index for the current top row
            movl %r12d, %r13d             # index = current column
            imul %eax, %r13d              # index = column * cols
            addl %r11d, %r13d             # index += top row

            # Calculate the pixel index for the corresponding bottom row
            movl %r12d, %r8d              # mirrored column = current column
            imul %eax, %r8d               # mirrored index = column * cols
            addl %r10d, %r8d              # mirrored index += bottom row

            # Copy pixels from the bottom row to the top row
            movl (%r15,%r8,4), %r9d       # load pixel from bottom row
            movl %r9d, (%r14,%r13,4)      # store pixel in top row

            # Copy pixels from the top row to the bottom row
            movl (%r15,%r13,4), %r9d      # load pixel from top row
            movl %r9d, (%r14,%r8,4)       # store pixel in bottom row

            addl $1, %r12d                # Increment column
        .Ltestcondc_v:
            cmp %r12d, %eax               # Compare current column to cols
            jl .Ltopc_v                   # Loop if column < cols

        addl $1, %r11d                    # Increment top row
        subl $1, %r10d                    # Decrement bottom row
    .Ltestcondr_v:
        cmp %r11d, %r10d                  # Stop when top row >= bottom row
        jl .Ltopr_v                       # Loop if top row < bottom row

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    popq %rbp
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

 /* this function will be implemented in MS3 */
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

	//Load the width and height of the image
    movl IMAGE_WIDTH_OFFSET(%rdi), %r15d   //Load input_img->width into %eax
    movl IMAGE_HEIGHT_OFFSET(%rdi), %ebx  //Load input_img->height into %ebx

    # Total number of pixels
    imul %ebx, %r15d               //Multiply width * height to get pixel count stored in eax
	movl $0, %r12d //r12 is current index  

	movq IMAGE_DATA_OFFSET(%rsi), %r13 //r13 is output->data
	movq IMAGE_DATA_OFFSET(%rdi), %r14 //r14 is input->data
	jmp .Ltestcond
	.Ltop:
		//print *((unsigned *) ($r15 + ($r10 * 4)))
		//movl (%r14d,%r12d,4), %r15d //r15 is address of pixel to convert (input[current])
		movl (%r14d,%r12d,4), %edi //move address of pixel to first argument
		call to_grayscale //call grayscale on pixel, %eax now hold grayscaled pixel
		movl %eax, (%r13d,%r12d,4) //put grayscaled pixel into output[current] 
		addl $1, %r12d
	.Ltestcond:
		cmp %r12d, %r15d       			  # Check if we've processed all pixels
    	jg .Ltop

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

 /* this function will be implemented in MS3 */
	.globl imgproc_composite
imgproc_composite:
	/* TODO: implement (this stub implementation just returns 0) */
	xorl %eax, %eax
	ret

/*
vim:ft=gas:
*/
