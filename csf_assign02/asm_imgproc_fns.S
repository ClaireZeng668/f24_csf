/*
 * x86-64 assembly language implementations of functions
 * CSF Assignment 2 M3
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
 *   6 registers pushed, but no procedure call in this function.
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
 *   6 registers pushed, but no procedure call in this function.
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


/*
 * Transform image by tiling it multiple times.
 * This transformation is applied when tiling factor n is greater than 1.
 * The image is repeated both horizontally and vertically to fill the output image.
 *
 * Parameters:
 *   %rdi - pointer to the original struct Image (input image)
 *   %rsi - tiling factor (n)
 *   %rdx - pointer to output struct Image (output image)
 *
 * Register use:
 *   %rdi  - pointer to input Image
 *   %rsi  - tiling factor (n)
 *   %rdx  - pointer to output Image
 *   %rbx  - pointer to input image data
 *   %r12  - tiling factor (n)
 *   %r13  - pointer to output image data
 *   %r10d - width of the input image
 *   %r11d - height of the input image
 *   %r14d - current tile row index
 *   %r15d - current tile column index
 *   %r8d  - tile copy counter (used in subroutine)
 *   %rax  - return value (success/failure)
 *
 * Memory use:
 *   Stack is aligned by pushing an even number of registers (6 in this case) and subtracting 8.
 *
 */
	.globl imgproc_tile
imgproc_tile:
	pushq %rbp
    movq %rsp, %rbp
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
	subq $8, %rsp

	// save parameters
    movq %rdi, %rbx      // %rbx = pointer to input image
    movq %rsi, %r12      // %r12 = tiling factor (n)
    movq %rdx, %r13      // %r13 = pointer to output image

    // load width and height of the input image
    movl IMAGE_WIDTH_OFFSET(%rdi), %r10d   // %r10 = image width (cols)
    movl IMAGE_HEIGHT_OFFSET(%rdi), %r11d  // %r11 = image height (rows)

    movq $0, %rax         // Initialize return value (success/failure)

    // if tiling factor is less than 1, exit the function
    cmpl $1, %esi
    jl .Ltile_done

	// check if all valid
	movl %r10d, %edi
	movl %r11d, %esi
	movl %r12d, %edx
	call all_tiles_nonempty  // check if all tiles are nonempty
	cmpl $1, %eax
	jne .Ltile_done  // if invalid, exit function

    movl $0, %r14d   // Initialize tile row counter
    movl $0, %r15d   // Initialize tile column counter

	// loop through each tile row
	jmp .Ltile_condr
	.Ltile_topr:
		movl $0, %r15d   // reset tile column counter for each row
		// loop through each tile column
		jmp .Ltile_condc
		.Ltile_topc:
            movq %r13, %rdi   // %rdi = pointer to output image
            movq %rbx, %rsi   // %rsi = pointer to input image
            movl %r14d, %edx  // %edx = current tile row index
            movl %r15d, %ecx  // %ecx = current tile column index
            movl %r12d, %r8d  // %r8d = tiling factor
            call copy_tile    // Copy the tile to the output image
			incl %r15d  	  // increment tile column counter
		.Ltile_condc:
            cmpl %r12d, %r15d  // compare column counter to tiling factor
            jl .Ltile_topc     // loop if column counter < n

        incl %r14d             // increment tile row counter
	.Ltile_condr:
        cmpl %r12d, %r14d      // compare row counter to tiling factor
        jl .Ltile_topr         // loop if row counter < n

    // end of tiling process
    movq $1, %rax  // set return value to success (1)

	.Ltile_done:
		addq $8, %rsp
		popq %r15
		popq %r14
		popq %r13
		popq %r12
		popq %rbx
		popq %rbp
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
 *   Stack is aligned by pushing an even number of registers (6 in this case) and subtracting 8.
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
	subq $8, %rsp

    // Load image dimensions (width = cols, height = rows)
    movl IMAGE_WIDTH_OFFSET(%rdi), %r15d   // r15d = width
    movl IMAGE_HEIGHT_OFFSET(%rdi), %ebx    // ebx = height

    imull %ebx, %r15d                       // r15d = width * height (total pixels)
    movl $0, %r12d                         // r12d = current index = 0

    movq IMAGE_DATA_OFFSET(%rsi), %r13      // r13 = output data pointer
    movq IMAGE_DATA_OFFSET(%rdi), %r14      // r14 = input data pointer

    jmp .Ltestcond

.Ltop:
    // Load pixel from input image: pixel = *(input_data + index)
    movl (%r14, %r12, 4), %edi             // Load 32-bit pixel into %edi
    call to_grayscale                      // Grayscale pixel in %eax
    movl %eax, (%r13, %r12, 4)             // Store 32-bit grayscale pixel
    // Increment current index
    addl $1, %r12d

.Ltestcond:
    // Compare current index with total pixels
    cmpl %r15d, %r12d                      // Compare index < total pixels
    jl .Ltop                 // If index < total pixels, continue loop

    // Restore registers and return
	addq $8, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    popq %rbp
    ret


/*
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
 *
 * Register use:
 *   %rdi  - pointer to base image
 *   %rsi  - pointer to overlay image
 *   %rdx  - pointer to output image
 *   %r8d  - base image width
 *   %r9d  - base image height
 *   %r10d - overlay image width
 *   %r11d - overlay image height
 *   %rax  - return value (success/failure)
 *   %rbx  - base image data pointer
 *   %r12  - overlay image data pointer
 *   %r13  - output image data pointer
 *   %r14d - total number of pixels to compare
 *   %r15d - loop counter
 *
 * Memory use:
 *   Stack is aligned by pushing an even number of registers (6 in this case) and subtracting 8.
 */
	.globl imgproc_composite
imgproc_composite:
	pushq %rbp
    movq %rsp, %rbp
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
	subq $8, %rsp

	movl IMAGE_WIDTH_OFFSET(%rdi), %r8d       // base img width
    movl IMAGE_HEIGHT_OFFSET(%rdi), %r9d      // base img height
	movl IMAGE_WIDTH_OFFSET(%rsi), %r10d      // overlay img width
    movl IMAGE_HEIGHT_OFFSET(%rsi), %r11d     // overlay img height
	movq $0, %rax

	// check same dimensions
	cmpl %r8d, %r10d
	jne .Lcomp_done
	cmpl %r9d, %r11d
	jne .Lcomp_done

	// dimensions equal
	movq $1, %rax
	imull %r9d, %r8d
	movl %r8d, %ebx                         // total num pixels to compare with loop counter
	movq $0, %r12                           // loop counter
	movq IMAGE_DATA_OFFSET(%rdi), %r13      // r13 = base data pointer
    movq IMAGE_DATA_OFFSET(%rsi), %r14      // r14 = overlay data pointer
	movq IMAGE_DATA_OFFSET(%rdx), %r15		// r15 = output data pointer

	jmp .Lcomp_cond

	.Lcomp_top:
		movl (%r14, %r12, 4), %edi           // Load overlay pixel into %edi
		movl (%r13, %r12, 4), %esi           // Load base pixel into %esi
		call blend_colors
		movl %eax, (%r15, %r12, 4)
		incl %r12d

	.Lcomp_cond:
		cmpl %ebx, %r12d        // Compare index < total pixels
		jl .Lcomp_top           // If index < total pixels, continue loop

	.Lcomp_done:
		addq $8, %rsp
		popq %r15
		popq %r14
		popq %r13
		popq %r12
		popq %rbx
		popq %rbp
		ret


//helper functions for tiling, no function calls within

.globl all_tiles_nonempty
all_tiles_nonempty:
	movq $1, %rax
	cmpq %rdi, %rdx //compare n and width
	jg .Lempty      //jump if rdx(n) > rdi(width)
	cmpq %rsi, %rdx 
	jg .Lempty      //jump if rdx(n) > rsi(height)
	cmpq $0, %rdi
	jle .Lempty     //jump if rdi(width) <= 0
	cmpq $0, %rsi
	jle .Lempty     //jump if rsi(height) <= 0
	jmp .Ldone_empty
	.Lempty:
		movq $0, %rax
	.Ldone_empty:
		ret

.globl determine_tile_w
determine_tile_w:
	movq $0, %rdx 		//set rdx to 0 for 64 bit division
	movq %rdi, %rax 	//put dividend in rax (width)
	movq %rsi, %r10	
	divq %r10 			//quotient in rax
	ret

.globl determine_tile_x_offset
determine_tile_x_offset:
	movq %rdx, %r10 	//save tile_col in r10
	movq $0, %rdx 		//set rdx to 0 for 64 bit division
	movq %rdi, %rax 	//put dividend in rax (width)
	movq %rsi, %r11
	divq %r11 			//put quotient in rax, remainder in rdx (we want the remainder)
	movq $1, %rax
	cmpq %rdx, %r10
	jge .Lno_xoffset 	//jump if tile col >= number of offset
	jmp .Ldone_xoffset
	.Lno_xoffset:
		movq $0, %rax 	//offset is zero
	.Ldone_xoffset:
		ret

.globl determine_tile_h
determine_tile_h:
	movq $0, %rdx 		//set rdx to 0 for 64 bit division
	movq %rdi, %rax 	//put dividend in rax (height)
	movq %rsi, %r10
	divq %r10 			//put quotient in rax
	ret

.globl determine_tile_y_offset
determine_tile_y_offset:
	movq %rdx, %r10 	//save tile_row in r10
	movq $0, %rdx 		//set rdx to 0 for 64 bit division
	movq %rdi, %rax 	//put dividend in rax (height)
	movq %rsi, %r11
	divq %r11 			//put quotient in rax, remainder in rdx (we want the remainder)
	movq $1, %rax
	cmpq %rdx, %r10
	jge .Lno_yoffset 	//jump if tile row >= number of offset
	jmp .Ldone_yoffset
	.Lno_yoffset:
		movq $0, %rax 	//offset is zero
	.Ldone_yoffset:
		ret


/*
 * Copy a tile from the input image to the output image.
 * This transformation places a portion of the input image into its corresponding
 * location in the output image based on the tiling factor (n), row, and column indices.
 * 
 * Parameters:
 *   %rdi - pointer to the output image
 *   %rsi - pointer to the input image
 *   %rdx - tile row index (to determine position in the output image)
 *   %rcx - tile column index (to determine position in the input image)
 *   %r8d - tiling factor (n)
 *
 * Register use:
 *   %rbx  - pointer to output image data
 *   %r12  - pointer to input image
 *   %r13  - tile row index (used to calculate output start index)
 *   %r14  - tile column index (used to calculate input start index)
 *   %r15  - tiling factor (n)
 *   %r10d - output image position index
 *   %r11d - input image position index
 *   %r8d  - row counter (for row iterations)
 *   %r9d  - column counter (for column iterations)
 *   %rax  - pointer to output image data
 *   %rdi  - pointer to input image data
 *
 * Memory use:
 *   Local stack variables are used to store values (tile width, height, etc.).
 *   Stack is aligned by pushing an even number of registers (6 in this case) and subtracting an odd multiple of 8.
 */
.globl copy_tile
copy_tile:
	pushq %rbp
    movq %rsp, %rbp
	subq $24, %rsp
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

	// save parameters
	movq %rdi, %rbx    // %rbx = pointer to output image
	movq %rsi, %r12    // %r12 = pointer to input image
	movq %rdx, %r13    // %r13 = tile row index
	movq %rcx, %r14    // %r14 = tile column index
	movq %r8, %r15	   //n

	// calculate tile width and offset
	movl IMAGE_WIDTH_OFFSET(%r12), %edi   // img width
	movl %r15d, %esi
	call determine_tile_w
	movl %eax, -8(%rbp)     // tile width

	movl IMAGE_WIDTH_OFFSET(%r12), %edi   // img width
	movl %r15d, %esi 
	movl %r14d, %edx
	call determine_tile_x_offset
	movl %eax, -16(%rbp)   // tile width offset
	
	// add tile width offset to calculate row length
	movl -16(%rbp), %r10d
	addl %r10d, -8(%rbp)  

    movl IMAGE_HEIGHT_OFFSET(%r12), %edi   // img height
	movl %r15d, %esi
	call determine_tile_h
	movl %eax, -16(%rbp)  // tile height

	// calculate tile height and offset
	movl IMAGE_HEIGHT_OFFSET(%r12), %edi   //img height
	movl %r15d, %esi
	movl %r13d, %edx
	call determine_tile_y_offset
	movl %eax, -24(%rbp) // tile height offset

	// add tile height offset to calculate column length
	movl -24(%rbp), %r10d
	addl %r10d, -16(%rbp)   

	// call calculate_starting_index to determine position in output image
	movq %r12, %rdi
	movq %r13, %rsi
	movq %r14, %rdx
	movq %r15, %rcx
	call calculate_starting_index
	movl %eax, %r13d   // output start index
	
	movq $0, %r14   // original start index

	// access image data
	movq IMAGE_DATA_OFFSET(%rbx), %rax
    movq IMAGE_DATA_OFFSET(%r12), %rdi

	movl %r13d, %r10d  // output position
	movl %r14d, %r11d  // original position
	movq $0, %r8       // row counter

	// copy tile from input to output image
	jmp .Lcopy_test_condr
    .Lcopyc_top:
		movl $0, %r9d      // reset column counter
        jmp .Lcopy_test_condc
        .Lcopyr_top:      
			// copy pixel from input image to output image
			movl (%rdi, %r11, 4), %esi
			movl %esi, (%rax, %r10, 4) 

			addl IMAGE_WIDTH_OFFSET(%r12), %r10d  // add output position

			movl IMAGE_WIDTH_OFFSET(%r12), %edx
			imull %r15d, %edx
			addl %edx, %r11d  // add original position

            addl $1, %r9d     // increment column counter
        .Lcopy_test_condc:
            cmpl -16(%rbp), %r9d
            jl .Lcopyr_top    // jump if column counter < col len

		// Reset for next row
		movl %r8d, %edx       // row
		incl %edx 		      // row + 1
		addl %r13d, %edx      // row + 1 + out_start_index
		movl %edx, %r10d      // out_pos = out_start_index + row + 1

		// Reset original position
		movl %r8d, %edx       // row
		incl %edx             // row + 1
		imull %r15d, %edx     // (row + 1) * n
		addl %r14d, %edx      // row + 1 + orig_start_index
		movl %edx, %r11d
        addl $1, %r8d         // increment row counter
    .Lcopy_test_condr:
		cmpl -8(%rbp), %r8d
		jl .Lcopyc_top        // jump if row counter < row len

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
	addq $24, %rsp
    popq %rbp	
	ret


/*
 * Calculate the starting index for a tile in the output image.
 * This determines the pixel position in the output image where the top-left
 * corner of the current tile should be placed.
 * 
 * Parameters:
 *   %rdi - pointer to the input image
 *   %rsi - tile row index
 *   %rdx - tile column index
 *   %rcx - tiling factor (n)
 * 
 * Register use:
 *   %rbx  - width of the input image
 *   %r12d - height of the input image
 *   %r13  - tile row index
 *   %r14  - tile column index
 *   %r15  - tiling factor (n)
 *   %r8d  - width of a single tile
 *   %r9d  - accumulated width offset
 *   %r10d - accumulated height offset
 *   %r11  - loop counter (used for iterating over tile columns and rows)
 *   %r15d - starting index in the output image
 *
 * Memory use:
 *   Local stack variables are used to store temporary values (tile width, height, etc.).
 *   Stack is aligned by pushing an even number of registers (6 in this case) and subtracting 8.
 * 
 */
.globl calculate_starting_index
calculate_starting_index:
	pushq %rbp
    movq %rsp, %rbp
	subq $8, %rsp
    pushq %rbx
    pushq %r12 
    pushq %r13
    pushq %r14
    pushq %r15

	// save parameters
	movl IMAGE_WIDTH_OFFSET(%rdi), %ebx    // %ebx = width of the input image
    movl IMAGE_HEIGHT_OFFSET(%rdi), %r12d  // %r12d = height of the input image
	movq %rsi, %r13   // %r13 = tile row index
	movq %rdx, %r14   // %r14 = tile column index
	movq %rcx, %r15   // %r15 = tiling factor (n)

	movq %r12, %rdi
	movq %r15, %rsi
	call determine_tile_h   // %rax = height
	movl %eax, -8(%rbp)    

	movq %rbx, %rdi
	movq %r15, %rsi
	call determine_tile_w  // %rax = width
	movl %eax, %r8d  

	movq $0, %rdx 		// set rdx to 0 for 64 bit division
	movq %rbx, %rax 	// put dividend (img.width) in rax
	divq %r15 			// put quotient in rax, remainder in rdx
	movq %rdx, %r9 		// r9 holds total_width_offset

	movq $0, %rdx 		// set rdx to 0 for 64 bit division
	movq %r12, %rax 	// put dividend (img.height) in rax
	divq %r15 			// put quotient in rax, remainder in rdx
	movq %rdx, %r10 	// r10 holds total_height_offset

	movl -8(%rbp), %r12d    // move height into r12d

	movq $0, %r11   // loop counter
	movq $0, %r15   // starting index in the output image
	.Ltopc_calc:
		cmpq %r14, %r11
		jge .Lcalc_reset    // jump to reset loop counter if counter >= tile col

		cmpq $0, %r9
		jle .Lccalc_else	// jump if total_width_offset <= 0
		addl %r8d, %r15d
		incl %r15d
		decl %r9d			// decrement total_width_offset
		incl %r11d			// increment counter
		jmp .Ltopc_calc
		.Lccalc_else:
			addl %r8d, %r15d
			incl %r11d			// increment counter
			jmp .Ltopc_calc

	.Lcalc_reset:
		movq $0, %r11       // reset loop counter
	.Ltopr_calc:
		cmpq %r13, %r11
		jge .Lcalc_done     // jump to end if counter >= tile row

		cmpq $0, %r10
		jle .Lrcalc_else	// jump if total_height_offset <= 0
		movl %r12d, %edi
		incl %edi			// increment height
		imull %ebx, %edi 	// img.width * height
		addl %edi, %r15d	// out_start_index = out_start_index + edi
		decl %r10d			// decrement total_height_offset
		incl %r11d 			// increment counter
		jmp .Ltopr_calc
		.Lrcalc_else:
			movl %r12d, %edi
			imull %ebx, %edi 	// img.width * height
			addl %edi, %r15d	// out_start_index = out_start_index + edi
			incl %r11d 		    // increment counter
			jmp .Ltopr_calc

	.Lcalc_done:
		movl %r15d, %eax
		popq %r15
		popq %r14
		popq %r13
		popq %r12
		popq %rbx
		addq $8, %rsp
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
 *   Stack is aligned by pushing three registers.
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
 * Function to blend color components.
 * Parameters:
 *   %rdi: pointer to foreground color component
 *   %rsi: pointer to background color component
 *
 * Register use:
 *   %edi - holds intermediate calculations
 *   %r10d - holds intermediate calculations
 *   %esi - holds intermediate calculations
 *   %eax - holds calculated pixel value
 *
 * Memory use:
 *   No stack usage; function does not alter stack state.
 */
.globl blend_components
blend_components:
	movq $0, %rax
	imull %edx, %edi	//edi holds alpha*foreground
	movl $255, %r10d
	subl %edx, %r10d	//r10d holds 255-alpha
	imull %r10d, %esi	//esi hold (255-alpha)*background
	addl %esi, %edi		//edi holds number to be divided by 255
	movq $0, %rdx 		//set rdx to 0 for 64 bit division
	movl %edi, %eax 	//put dividend in eax
	movl $255, %r10d
	divl %r10d 			//divide eax by 255, put quotient in rax
	ret

/*
 * Function to blend foreground and background pixel.
 * Parameters:
 *   %rdi: pointer to foreground pixel
 *   %rsi: pointer to background pixel
 *
 * Register use:
 *   %ebx - holds the foregound pixel, foreground blue component (intermediate), blended blue component
 *   %r12d - holds the background pixel
 *   %r15d  - foregound alpha component
 *   %r13d  - foregound red component (intermediate), blended red component
 *   %r14d  - foregound green component (intermediate), blended green component
 *   %eax  - blended pixel
 *   %edi, %esi, %edx, %ecx - various intermediate values to pass into blend_components
 *
 * Memory use:
 *   Stack is aligned by pushing an even number of registers (6 in this case) and subtracting an odd multiple of 8.
 */
.globl blend_colors
blend_colors:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	pushq %rbx 
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	movl %edi, %ebx //foreground pixel
	movl %esi, %r12d //background pixel

	call get_a //rax holds fg alpha
	movl %eax, %r15d //r15 holds fg alpha

	movl %ebx, %edi
	call get_r
	movl %eax, %r13d //r13d holds fg red comp
	movl %r12d, %edi //first agrument is bg pixel
	call get_r
	movl %eax, %esi //esi holds bg red comp
	movl %r13d, %edi //edi holds fg red comp
	movl %r15d, %edx //edx holds fg alpha
	call blend_components
	movl %eax, %r13d //r13 holds red blended component

	movl %ebx, %edi
	call get_g
	movl %eax, %r14d //r14d holds fg green comp
	movl %r12d, %edi //first agrument is bg pixel
	call get_g
	movl %eax, %esi //esi holds bg green comp
	movl %r14d, %edi //edi holds fg green comp
	movl %r15d, %edx //edx holds fg alpha
	call blend_components
	movl %eax, %r14d //r14 holds green blended component

	movl %ebx, %edi
	call get_b
	movl %eax, %ebx //ebx holds fg blue comp
	movl %r12d, %edi //first agrument is bg pixel
	call get_b
	movl %eax, %esi //esi holds bg blue comp
	movl %ebx, %edi //edi holds fg blue comp
	movl %r15d, %edx //edx holds fg alpha
	call blend_components
	movl %eax, %r15d //r15 holds blue blended component

	movl %r13d, %edi //edi holds red blended
	movl %r14d, %esi //esi holds green blended
	movl %r15d, %edx //edx holds blue blended
	movl $255, %ecx //ecx holds alpha = 255
	call make_pixel //rax holds pixel made from r15,r14,r13, and 255

	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %rbx
	addq $8, %rsp
	popq %rbp
	ret
