/*
 * x86-64 assembly language implementations of functions
 * CSF Assignment 2 M2
 * Prasi Thapa
 * pthapa6@jhu.edu
 * Claire Zeng
 * czeng8@jhu.edu
 */

	.section .text

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/*
 * Defining helper functions here.
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
 * Transform image by mirroring its pixels horizontally.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 *
 * Register use:
 *   %rdi  - pointer to input Image
 *   %rsi  - pointer to output Image
 *   %eax  - image width (cols)
 *   %ebx  - image height (rows)
 *   %r10d - reverse column index (end of row)
 *   %r11d - current row index
 *   %r12d - current column index
 *   %r13d - index in the output image
 *   %r14  - pointer to output image data
 *   %r15  - pointer to input image data
 *
 * Memory use:
 *   Stack is aligned by pushing an odd number of registers (6 in this case).
 *
 */
	.globl imgproc_mirror_h
imgproc_mirror_h:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rbx
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	// Load image dimensions (width = cols, height = rows)
	movl IMAGE_WIDTH_OFFSET(%rdi), %eax   //cols
    movl IMAGE_HEIGHT_OFFSET(%rdi), %ebx  //rows

	movl $0, %r10d   // %r10 = end (reverse index)
	movl $0, %r11d   // %r11 = current row, row for loop
	movl $0, %r12d   // %r12 = current col
	movl $0, %r13d   // %r13 = output index
	
	movq IMAGE_DATA_OFFSET(%rsi), %r14   // Output data pointer
	movq IMAGE_DATA_OFFSET(%rdi), %r15   // Input data pointer

	jmp .Ltestcondr_h
	.Ltopr_h:
		// Calculate the end index of the current row (last pixel in row)
		movl %r11d, %r10d     // end = row * cols + (cols - 1)
		imul %eax, %r10d      // row * cols
		addl %eax, %r10d      // + cols
		subl $1, %r10d        // - 1

		movl $0, %r12d        // Reset col index for this row

		jmp .Ltestcondc_h
		.Ltopc_h:
			movl %r11d, %r13d     // output index = row * cols + current col
			imul %eax, %r13d
			addl %r12d, %r13d

			movl (%r15, %r10, 4), %r8d  // Get pixel from input at reversed index
			movl %r8d, (%r14, %r13, 4)  // Write pixel to output at current index

			subl $1, %r10d        // Decrement reverse index
			addl $1, %r12d        // Increment current col
		.Ltestcondc_h:
			cmp %r12d, %eax
			jg .Ltopc_h

		addl $1, %r11d		  // Increment row index
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
 * Transform image by mirroring its pixels vertically.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 *
 * Register use:
 *   %rdi  - pointer to input Image
 *   %rsi  - pointer to output Image
 *   %eax  - image width (cols)
 *   %ebx  - image height (rows)
 *   %r10d - opposite row index (bottom-to-top mirror)
 *   %r11d - current row index
 *   %r12d - current column index
 *   %r13d - index in the output image
 *   %r14  - pointer to output image data
 *   %r15  - pointer to input image data
 *
 * Memory use:
 *   Stack is aligned by pushing an odd number of registers (6 in this case).
 *
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

    // Load the width and height of the image
    movl IMAGE_WIDTH_OFFSET(%rdi), %eax  
    movl IMAGE_HEIGHT_OFFSET(%rdi), %ebx  

    movl $0, %r10d   // %r10 = opposite row index
    movl $0, %r11d   // %r11 = current row
    movl $0, %r12d   // %r12 = current col
	movl $0, %r13d   // %r13 = output index

    movq IMAGE_DATA_OFFSET(%rsi), %r14   // Output data pointer
    movq IMAGE_DATA_OFFSET(%rdi), %r15   // Input data pointer

    // Vertical mirror logic
    jmp .Ltestcondr_v
    .Ltopr_v:
		// Calculate the opposite row
		movl $0, %r10d      // %r10 is opposite_row variable
		movl %ebx, %r10d    // opposite_row = rows - 1 - current_row
		subl %r11d, %r10d
		subl $1, %r10d

		movl $0, %r12d      // Reset col index

        jmp .Ltestcondc_v
        .Ltopc_v:      
			// Calculate the pixel index for the corresponding top row
			movl $0, %r13d                // %r13 is index for output image
			movl %r11d, %r13d             // index = row
            imul %eax, %r13d              // index = column * cols
            addl %r12d, %r13d             // index += col

            // Calculate the pixel index for the corresponding bottom row
			movl $0, %r8d                 // %r8 is index for input image
            movl %r10d, %r8d              // index = opposite_row
            imul %eax, %r8d               // mirrored index = column * cols
            addl %r12d, %r8d              // mirrored index += col

            // Copy pixels from the input image to the output image
            movl (%r15,%r8,4), %r9d       // load pixel from bottom row
            movl %r9d, (%r14,%r13,4)      // store pixel in top row

            addl $1, %r12d                // Increment col
        .Ltestcondc_v:
            cmp %r12d, %eax               // Compare col to cols
            jg .Ltopc_v                   // Loop if col < cols

        addl $1, %r11d                    // Increment row
    .Ltestcondr_v:
        cmp %r11d, %ebx                  // Stop when row >= rows
        jg .Ltopr_v                      // Loop if row < rows

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    popq %rbp
    ret


 /* this function will be implemented in MS3 */
	.globl imgproc_tile
imgproc_tile:
	/* TODO: implement (this stub implementation just returns 0) */
	xorl %eax, %eax
	ret


/*
 * Transform image by converting each pixel to grayscale.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 *
 * Register use:
 *   %rdi  - pointer to input Image
 *   %rsi  - pointer to output Image
 *   %r12d - current pixel index
 *   %r13  - pointer to output image data
 *   %r14  - pointer to input image data
 *   %r15d - total pixel count (width * height)
 *   %eax  - grayscale pixel value
 *   %ebx  - image height (rows)
 *
 * Memory use:
 *   Stack is aligned by pushing an odd number of registers (6 in this case).
 */
	.globl imgproc_grayscale
imgproc_grayscale:
	pushq %rbp
	movq %rsp, %rbp
	pushq %rbx
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	// Load image dimensions (width = cols, height = rows)
    movl IMAGE_WIDTH_OFFSET(%rdi), %r15d   
    movl IMAGE_HEIGHT_OFFSET(%rdi), %ebx 

    imul %ebx, %r15d   //Total number of pixels (width * height)            
	movl $0, %r12d     // r12 is current index  

	movq IMAGE_DATA_OFFSET(%rsi), %r13   // Output data pointer
	movq IMAGE_DATA_OFFSET(%rdi), %r14   // Input data pointer

	jmp .Ltestcond

	.Ltop:
		movl (%r14d,%r12d,4), %edi   // Move address of pixel to first argument
		call to_grayscale    // Call grayscale on pixel, %eax now holds grayscaled pixel
		movl %eax, (%r13d,%r12d,4)   // put grayscaled pixel into output[current] 
		addl $1, %r12d
	.Ltestcond:
		cmp %r12d, %r15d       		// Check if we've processed all pixels
    	jg .Ltop

	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %rbx
	popq %rbp
    ret


/*
 * Functions to extract the red, green, blue, and alpha components of a pixel.
 * Parameters:
 *   %rdi: pointer to pixel to extract color component
 *
 * Register use:
 *   %edi - pixel value (32-bit RGBA)
 *   %eax - color component (8-bit)
 *
 * Memory use:
 *   No stack usage; function does not alter stack state.
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
 * Function to create a pixel from red, green, blue, and alpha components.
 * Parameters:
 *   %rdi: red component
 *   %rsi: green component
 *   %rdx: blue component
 *   %rcx: alpha component
 *
 * Register use:
 *   %edi - red component (8 bits)
 *   %esi - green component (8 bits)
 *   %edx - blue component (8 bits)
 *   %ecx - alpha component (8 bits)
 *   %eax - final pixel value (32 bits)
 *
 * Memory use:
 *   No stack usage; function does not alter stack state.
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
 * Function to convert a pixel to grayscale.
 * Parameters:
 *   %rdi: pointer to pixel
 *
 * Register use:
 *   %rbx - holds the original pixel pointer
 *   %r12d - holds the grayscale value (intermediate)
 *   %eax - holds the extracted color component
 *   %edi - holds the pixel value
 *   %esi - holds the grayscaled pixel value
 *   %edx - holds the alpha value
 *   %ecx - holds the alpha value
 *
 * Memory use:
 *   Stack is aligned by pushing two registers (2 in this case).
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



 /* this function will be implemented in MS3 */
	.globl imgproc_composite
imgproc_composite:
	/* TODO: implement (this stub implementation just returns 0) */
	xorl %eax, %eax
	ret

/*
vim:ft=gas:
*/
