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
    # Function parameters
    # %rdi: pointer to input_img (struct Image)
    # %rsi: pointer to output_img (struct Image)

    # Load the width and height of the image
    movl IMAGE_WIDTH_OFFSET(%rdi), %eax   # Load input_img->width into %eax
    movl IMAGE_HEIGHT_OFFSET(%rdi), %ebx  # Load input_img->height into %ebx

    # Set up a loop for each row (height)
mirror_h_loop_rows:
    cmp %ebx, 0                           # Check if height (rows) is 0
    jle done                              # Jump to done if no more rows

    # Calculate starting index for the row and loop through its pixels
    # We need to swap pixels across the row.
    movl $0, %ecx                         # Pixel index
    subl $1, %eax                         # width - 1, which gives us the last index of the row

mirror_h_loop_cols:
    # Compare if all pixels are mirrored in the row
    cmp %ecx, %eax
    jge next_row                          # Go to next row when done

    # Perform the swap by loading from input_img and storing to output_img
    mov (%rdi, %ecx, 4), %edx             # Load pixel from input_img at index %ecx
    mov (%rdi, %eax, 4), %esi             # Load pixel from the mirrored side
    mov %edx, (%rsi, %eax, 4)             # Store left pixel into output_img
    mov %esi, (%rsi, %ecx, 4)             # Store right pixel into output_img

    # Update loop indices
    inc %ecx                              # Increment left pixel index
    dec %eax                              # Decrement right pixel index
    jmp mirror_h_loop_cols                # Repeat for next pixel in the row

next_row:
    # Move to the next row
    dec %ebx                              # Decrement row index
    jmp mirror_h_loop_rows                # Loop for next row

done:
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

done:
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
    # Function parameters
    # %rdi: pointer to input_img (struct Image)
    # %rsi: pointer to output_img (struct Image)

    # Load the width and height of the image
    movl IMAGE_WIDTH_OFFSET(%rdi), %eax   # Load input_img->width into %eax
    movl IMAGE_HEIGHT_OFFSET(%rdi), %ebx  # Load input_img->height into %ebx

    # Total number of pixels
    imul %ebx, %eax                       # Multiply width * height to get pixel count

grayscale_loop:
    cmp %eax, 0                           # Check if we've processed all pixels
    jle done                              # If no more pixels, exit

    # Load pixel from input_img
    mov (%rdi, %eax, 4), %edx             # Load pixel into %edx

    # Extract the red, green, and blue components
    movzbl %dh, %ecx                      # Extract red
    movzbl %dl, %ebx                      # Extract green
    movzbl %dah, %esi                     # Extract blue

    # Apply grayscale conversion formula (0.3R + 0.59G + 0.11B)
    imul $77, %ecx                        # Multiply red by 0.3 * 256 (77)
    imul $150, %ebx                       # Multiply green by 0.59 * 256 (150)
    imul $29, %esi                        # Multiply blue by 0.11 * 256 (29)
    add %ecx, %ebx                        # Add red and green components
    add %esi, %ebx                        # Add blue component

    # Right shift to divide by 256 and get grayscale value
    shr $8, %ebx                          # Divide by 256

    # Pack grayscale value back into RGB (R=G=B)
    mov %ebx, %ecx
    mov %ebx, %esi

    # Store the result in output_img
    mov %ecx, (%rsi, %eax, 4)             # Store pixel

    # Move to the next pixel
    dec %eax                              # Decrement pixel count
    jmp grayscale_loop                    # Repeat for next pixel

done:
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
