#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tctest.h"
#include "imgproc.h"

// An expected color identified by a (non-zero) character code.
// Used in the "Picture" data type.
typedef struct {
  char c;
  uint32_t color;
} ExpectedColor;

// Type representing a "picture" of an expected image.
// Useful for creating a very simple Image to be accessed
// by test functions.
typedef struct {
  ExpectedColor colors[20];
  int width, height;
  const char *data;
} Picture;

// Some "basic" colors to use in test Pictures
#define TEST_COLORS \
    { \
      { ' ', 0x000000FF }, \
      { 'r', 0xFF0000FF }, \
      { 'g', 0x00FF00FF }, \
      { 'b', 0x0000FFFF }, \
      { 'c', 0x00FFFFFF }, \
      { 'm', 0xFF00FFFF }, \
    }

// Expected "basic" colors after grayscale transformation
#define TEST_COLORS_GRAYSCALE \
    { \
      { ' ', 0x000000FF }, \
      { 'r', 0x4E4E4EFF }, \
      { 'g', 0x7F7F7FFF }, \
      { 'b', 0x303030FF }, \
      { 'c', 0xB0B0B0FF }, \
      { 'm', 0x7F7F7FFF }, \
    }

// Colors for test overlay image (for testing the composite
// transformation). Has some fully-opaque colors,
// some partially-transparent colors, and a complete
// transparent color.
#define OVERLAY_COLORS \
  { \
    { 'r', 0xFF0000FF }, \
    { 'R', 0xFF000080 }, \
    { 'g', 0x00FF00FF }, \
    { 'G', 0x00FF0080 }, \
    { 'b', 0x0000FFFF }, \
    { 'B', 0x0000FF80 }, \
    { ' ', 0x00000000 }, \
  }

// Data type for the test fixture object.
// This contains data (including Image objects) that
// can be accessed by test functions. This is useful
// because multiple test functions can access the same
// data (so you don't need to create/initialize that
// data multiple times in different test functions.)
typedef struct {
  // smiley-face picture
  Picture smiley_pic;

  // original smiley-face Image object
  struct Image *smiley;

  // empty Image object to use for output of
  // transformation on smiley-face image
  struct Image *smiley_out;

  // Picture for overlay image (for basic imgproc_composite test)
  Picture overlay_pic;

  // overlay image object
  struct Image *overlay;
} TestObjs;

// Functions to create and clean up a test fixture object
TestObjs *setup( void );
void cleanup( TestObjs *objs );

// Helper functions used by the test code
struct Image *picture_to_img( const Picture *pic );
uint32_t lookup_color(char c, const ExpectedColor *colors);
bool images_equal( struct Image *a, struct Image *b );
void destroy_img( struct Image *img );

// Test functions
void test_mirror_h_basic( TestObjs *objs );
void test_mirror_v_basic( TestObjs *objs );
void test_tile_basic( TestObjs *objs );
void test_grayscale_basic( TestObjs *objs );
void test_composite_basic( TestObjs *objs );
// void test_all_tiles_nonempty( TestObjs *objs );
// void test_determine_tile_w( TestObjs *objs );
// void test_determine_tile_x_offset( TestObjs *objs );
// void test_determine_tile_h( TestObjs *objs );
// void test_determine_tile_y_offset( TestObjs *objs );
// void test_get_r( TestObjs *objs );
// void test_get_g( TestObjs *objs );
// void test_get_b( TestObjs *objs );
// void test_get_a( TestObjs *objs );
// void test_make_pixel( TestObjs *objs );
// void test_to_grayscale( TestObjs *objs );
// void test_blend_components( TestObjs *objs );
// void test_blend_colors( TestObjs *objs );
// TODO: add prototypes for additional test functions

int main( int argc, char **argv ) {
  // allow the specific test to execute to be specified as the
  // first command line argument
  if ( argc > 1 )
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  // Run tests.
  // Make sure you add additional TEST() macro invocations
  // for any additional test functions you add.
  TEST( test_mirror_h_basic );
  TEST( test_mirror_v_basic );
  TEST( test_tile_basic );
  TEST( test_grayscale_basic );
  TEST( test_composite_basic );
  // TEST( test_all_tiles_nonempty );
  // TEST( test_determine_tile_w );
  // TEST( test_determine_tile_x_offset );
  // TEST( test_determine_tile_h );
  // TEST( test_determine_tile_y_offset );
  // TEST( test_get_r );
  // TEST( test_get_g );
  // TEST( test_get_b );
  // TEST( test_get_a );
  // TEST( test_make_pixel );
  // TEST( test_to_grayscale );
  // TEST( test_blend_components );
  // TEST( test_blend_colors );
  TEST_FINI();
}

////////////////////////////////////////////////////////////////////////
// Test fixture setup/cleanup functions
////////////////////////////////////////////////////////////////////////

TestObjs *setup( void ) {
  TestObjs *objs = (TestObjs *) malloc( sizeof(TestObjs) );

  Picture smiley_pic = {
    TEST_COLORS,
    16, // width
    10, // height
    "    mrrrggbc    "
    "   c        b   "
    "  r   r  b   c  "
    " b            b "
    " b            r "
    " g   b    c   r "
    "  c   ggrb   b  "
    "   m        c   "
    "    gggrrbmc    "
    "                "
  };
  objs->smiley_pic = smiley_pic;
  objs->smiley = picture_to_img( &smiley_pic );

  objs->smiley_out = (struct Image *) malloc( sizeof( struct Image ) );
  img_init( objs->smiley_out, objs->smiley->width, objs->smiley->height );

  Picture overlay_pic = {
    OVERLAY_COLORS,
    16, 10,
   "                "
   "                "
   "                "
   "                "
   "                "
   "  rRgGbB        "
   "                "
   "                "
   "                "
   "                "
  };
  objs->overlay_pic = overlay_pic;
  objs->overlay = picture_to_img( &overlay_pic );

  return objs;
}

void cleanup( TestObjs *objs ) {
  destroy_img( objs->smiley );
  destroy_img( objs->smiley_out );
  destroy_img( objs->overlay );

  free( objs );
}

////////////////////////////////////////////////////////////////////////
// Test code helper functions
////////////////////////////////////////////////////////////////////////

struct Image *picture_to_img( const Picture *pic ) {
  struct Image *img;

  img = (struct Image *) malloc( sizeof(struct Image) );
  img_init( img, pic->width, pic->height );

  for ( int i = 0; i < pic->height; ++i ) {
    for ( int j = 0; j < pic->width; ++j ) {
      int index = i * img->width + j;
      uint32_t color = lookup_color( pic->data[index], pic->colors );
      img->data[index] = color;
    }
  }

  return img;
}

uint32_t lookup_color(char c, const ExpectedColor *colors) {
  for (int i = 0; ; i++) {
    assert(colors[i].c != 0);
    if (colors[i].c == c) {
      return colors[i].color;
    }
  }
}

// Returns true IFF both Image objects are identical
bool images_equal( struct Image *a, struct Image *b ) {
  if ( a->width != b->width || a->height != b->height )
    return false;

  int num_pixels = a->width * a->height;
  for ( int i = 0; i < num_pixels; ++i ) {
    if ( a->data[i] != b->data[i] )
      return false;
  }

  return true;
}

void destroy_img( struct Image *img ) {
  if ( img != NULL )
    img_cleanup( img );
  free( img );
}

////////////////////////////////////////////////////////////////////////
// Test functions
////////////////////////////////////////////////////////////////////////

void test_mirror_h_basic( TestObjs *objs ) {
  Picture smiley_mirror_h_pic = {
    TEST_COLORS,
    16, 10,
    "    cbggrrrm    "
    "   b        c   "
    "  c   b  r   r  "
    " b            b "
    " r            b "
    " r   c    b   g "
    "  b   brgg   c  "
    "   c        m   "
    "    cmbrrggg    "
    "                "
  };
  struct Image *smiley_mirror_h_expected = picture_to_img( &smiley_mirror_h_pic );

  imgproc_mirror_h( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( smiley_mirror_h_expected, objs->smiley_out ) );

  destroy_img( smiley_mirror_h_expected );
}

void test_mirror_v_basic( TestObjs *objs ) {
  Picture smiley_mirror_v_pic = {
    TEST_COLORS,
    16, 10,
    "                "
    "    gggrrbmc    "
    "   m        c   "
    "  c   ggrb   b  "
    " g   b    c   r "
    " b            r "
    " b            b "
    "  r   r  b   c  "
    "   c        b   "
    "    mrrrggbc    "
  };
  struct Image *smiley_mirror_v_expected = picture_to_img( &smiley_mirror_v_pic );

  imgproc_mirror_v( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( smiley_mirror_v_expected, objs->smiley_out ) );

  destroy_img( smiley_mirror_v_expected );
}

void test_tile_basic( TestObjs *objs ) {
  Picture smiley_tile_3_pic = {
    TEST_COLORS,
    16, 10,
    "  rg    rg   rg "
    "                "
    "  gb    gb   gb "
    "                "
    "  rg    rg   rg "
    "                "
    "  gb    gb   gb "
    "  rg    rg   rg "
    "                "
    "  gb    gb   gb "
  };
  struct Image *smiley_tile_3_expected = picture_to_img( &smiley_tile_3_pic );

  int success = imgproc_tile( objs->smiley, 3, objs->smiley_out );
  ASSERT( success );
  ASSERT( images_equal( smiley_tile_3_expected, objs->smiley_out ) );

  destroy_img( smiley_tile_3_expected );
}

void test_grayscale_basic( TestObjs *objs ) {
  Picture smiley_grayscale_pic = {
    TEST_COLORS_GRAYSCALE,
    16, // width
    10, // height
    "    mrrrggbc    "
    "   c        b   "
    "  r   r  b   c  "
    " b            b "
    " b            r "
    " g   b    c   r "
    "  c   ggrb   b  "
    "   m        c   "
    "    gggrrbmc    "
    "                "
  };

  struct Image *smiley_grayscale_expected = picture_to_img( &smiley_grayscale_pic );

  imgproc_grayscale( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( smiley_grayscale_expected, objs->smiley_out ) );

  destroy_img( smiley_grayscale_expected );
}

void test_composite_basic( TestObjs *objs ) {
  imgproc_composite( objs->smiley, objs->overlay, objs->smiley_out );

  // for all of the fully-transparent pixels in the overlay image,
  // the result image should have a pixel identical to the corresponding
  // pixel in the base image
  for ( int i = 0; i < 160; ++i ) {
    if ( objs->overlay->data[i] == 0x00000000 )
      ASSERT( objs->smiley->data[i] == objs->smiley_out->data[i] );
  }

  // check the computed colors for the partially transparent or
  // fully opaque colors in the overlay image
  ASSERT( 0xFF0000FF == objs->smiley_out->data[82] );
  ASSERT( 0x800000FF == objs->smiley_out->data[83] );
  ASSERT( 0x00FF00FF == objs->smiley_out->data[84] );
  ASSERT( 0x00807FFF == objs->smiley_out->data[85] );
  ASSERT( 0x0000FFFF == objs->smiley_out->data[86] );
  ASSERT( 0x000080FF == objs->smiley_out->data[87] );
}

// Test for all_tiles_nonempty function
void test_all_tiles_nonempty( TestObjs *objs ) {
    ASSERT(all_tiles_nonempty(objs->smiley->width, objs->smiley->height, 1) == 1);
    ASSERT(all_tiles_nonempty(objs->smiley->width, objs->smiley->height, 10) == 1);
    ASSERT(all_tiles_nonempty(objs->smiley->width, objs->smiley->height, 16) == 0);

        // n divides image dimensions perfectly
    ASSERT(all_tiles_nonempty(16, 10, 1) == 1);
    ASSERT(all_tiles_nonempty(16, 10, 2) == 1);
    ASSERT(all_tiles_nonempty(16, 10, 5) == 1);
    
    // n larger than dimensions (some tiles will be empty)
    ASSERT(all_tiles_nonempty(16, 10, 16) == 0);
    ASSERT(all_tiles_nonempty(16, 10, 20) == 0);

    // Edge case: n = 0 (invalid input)
    ASSERT(all_tiles_nonempty(16, 10, 0) == 0);

    // n equal to dimensions
    ASSERT(all_tiles_nonempty(16, 10, 10) == 1);
}

// Test for determine_tile_w function
void test_determine_tile_w( TestObjs *objs ) {
    ASSERT(determine_tile_w(objs->smiley->width, 1) == 16);
    ASSERT(determine_tile_w(objs->smiley->width, 2) == 8);
    ASSERT(determine_tile_w(objs->smiley->width, 3) == 5);
    ASSERT(determine_tile_w(objs->smiley->width, 14) == 1);
    ASSERT(determine_tile_w(objs->smiley->width, 16) == 1);

    // Cases where width divides evenly by n
    ASSERT(determine_tile_w(16, 1) == 16);
    ASSERT(determine_tile_w(16, 2) == 8);
    ASSERT(determine_tile_w(16, 4) == 4);
    
    // Cases where width doesn't divide evenly by n
    ASSERT(determine_tile_w(16, 3) == 5);  // 16 / 3 = 5 remainder 1
    ASSERT(determine_tile_w(16, 5) == 3);  // 16 / 5 = 3 remainder 1

    // Edge case: n = 1 (entire width in one tile)
    ASSERT(determine_tile_w(16, 1) == 16);
}

// Test for determine_tile_x_offset function
void test_determine_tile_x_offset( TestObjs *objs ) {
    ASSERT(determine_tile_x_offset(objs->smiley->width, 3, 0) == 1);
    ASSERT(determine_tile_x_offset(objs->smiley->width, 3, 1) == 0);
    ASSERT(determine_tile_x_offset(objs->smiley->width, 7, 0) == 1);
    ASSERT(determine_tile_x_offset(objs->smiley->width, 7, 6) == 0);
    ASSERT(determine_tile_x_offset(objs->smiley->width, 16, 0) == 0);

    // Evenly divisible
    ASSERT(determine_tile_x_offset(16, 4, 0) == 0);  
    ASSERT(determine_tile_x_offset(16, 4, 1) == 4);  
    
    // Non-evenly divisible
    ASSERT(determine_tile_x_offset(16, 3, 0) == 0); 
    ASSERT(determine_tile_x_offset(16, 3, 2) == 10); 
    
    // Edge cases: last tile
    ASSERT(determine_tile_x_offset(16, 4, 3) == 12); 
}

// Test for determine_tile_h function
void test_determine_tile_h( TestObjs *objs ) {
    ASSERT(determine_tile_h(objs->smiley->height, 1) == 10);
    ASSERT(determine_tile_h(objs->smiley->height, 2) == 5);
    ASSERT(determine_tile_h(objs->smiley->height, 3) == 3);
    ASSERT(determine_tile_h(objs->smiley->height, 9) == 1);

    // Cases where height divides evenly by n
    ASSERT(determine_tile_h(10, 1) == 10);
    ASSERT(determine_tile_h(10, 2) == 5);
    ASSERT(determine_tile_h(10, 5) == 2);

    // Cases where height doesn't divide evenly by n
    ASSERT(determine_tile_h(10, 3) == 4);  // 10 / 3 = 3 remainder 1
    ASSERT(determine_tile_h(10, 4) == 3);  // 10 / 4 = 2 remainder 2
    
    // Edge case: n = 1 (entire height in one tile)
    ASSERT(determine_tile_h(10, 1) == 10);
}

// Test for determine_tile_y_offset function
void test_determine_tile_y_offset( TestObjs *objs ) {
    ASSERT(determine_tile_y_offset(objs->smiley->height, 3, 0) == 1);
    ASSERT(determine_tile_y_offset(objs->smiley->height, 6, 1) == 1);
    ASSERT(determine_tile_y_offset(objs->smiley->height, 9, 1) == 0);

    // Evenly divisible
    ASSERT(determine_tile_y_offset(10, 5, 0) == 0);  
    ASSERT(determine_tile_y_offset(10, 5, 1) == 2);  
    
    // Non-evenly divisible
    ASSERT(determine_tile_y_offset(10, 3, 0) == 0);  
    ASSERT(determine_tile_y_offset(10, 3, 2) == 6);  
    
    // Edge cases: last tile
    ASSERT(determine_tile_y_offset(10, 5, 4) == 8);  
}

// Test for RGBA extraction functions
void test_get_r( TestObjs *objs ) {
    ASSERT(get_r(0x00000000U) == 0x00000000U);
    ASSERT(get_r(0xFF000000U) == 0x000000FFU);
    ASSERT(get_r(0xFFFFFFFFU) == 0x000000FFU);
}

void test_get_g( TestObjs *objs ) {
    ASSERT(get_g(0x00FF0000U) == 0x000000FFU);
    ASSERT(get_g(0xFFFFFFFFU) == 0x000000FFU);
}

void test_get_b( TestObjs *objs ) {
    ASSERT(get_b(0x0000FF00U) == 0x000000FFU);
    ASSERT(get_b(0xFFFFFFFFU) == 0x000000FFU);
}

void test_get_a( TestObjs *objs ) {
    ASSERT(get_a(0x000000FFU) == 0x000000FFU);
}

// Additional test for grayscale function
void test_grayscale( TestObjs *objs ) {
    // Use known color pixel and its corresponding grayscale version
    ASSERT(to_grayscale(0x00FF0000U) == make_pixel(54, 54, 54, 255)); // Red to grayscale

    // Green to grayscale
    ASSERT(to_grayscale(0x0000FF00U) == make_pixel(150, 150, 150, 255));
    
    // Blue to grayscale
    ASSERT(to_grayscale(0x000000FFU) == make_pixel(28, 28, 28, 255));

    // White to grayscale
    ASSERT(to_grayscale(0xFFFFFFFFU) == make_pixel(255, 255, 255, 255));

    // Black should remain black in grayscale
    ASSERT(to_grayscale(0x00000000U) == make_pixel(0, 0, 0, 0));    
}

// Additional test for blend function
void test_blend_components(TestObjs *objs) {
    // Case 1: Full opacity foreground
    uint32_t fg1 = 0xFF0000FF;
    uint32_t bg1 = 0x00FF00FF; 
    uint32_t alpha1 = 255;
    uint32_t result1 = blend_components(fg1, bg1, alpha1);
    assert(result1 == 0xFF0000FF); // Should return red

    // Case 2: Full transparency foreground
    uint32_t fg2 = 0xFF0000FF;
    uint32_t bg2 = 0x00FF00FF; 
    uint32_t alpha2 = 0; 
    uint32_t result2 = blend_components(fg2, bg2, alpha2);
    assert(result2 == 0x00FF00FF); // Should return green

    // Case 3: 50% alpha
    uint32_t fg3 = 0xFFFF00FF; 
    uint32_t bg3 = 0x0000FFFF; 
    uint32_t alpha3 = 128;
    uint32_t result3 = blend_components(fg3, bg3, alpha3);
    assert(result3 == 0x7F7F80FF); // Expected blended color

    // Case 4: Edge case with zero values
    uint32_t fg4 = 0x00000000; 
    uint32_t bg4 = 0xFFFFFFFF; 
    uint32_t alpha4 = 0; 
    uint32_t result4 = blend_components(fg4, bg4, alpha4);
    assert(result4 == 0xFFFFFFFF); // Should return white

    // Case 5: Edge case with full alpha on a black background
    uint32_t fg5 = 0xFF00FFFF; 
    uint32_t bg5 = 0x00000000; 
    uint32_t alpha5 = 255; 
    uint32_t result5 = blend_components(fg5, bg5, alpha5);
    assert(result5 == 0xFF00FFFF); // Should return magenta
}

// Additional test for blend_colors function
void test_blend_colors(TestObjs *objs) {
    // Case 1: Full opacity foreground
    uint32_t fg1 = 0xFF0000FF; 
    uint32_t bg1 = 0x00FF00FF; 
    uint32_t result1 = blend_colors(fg1, bg1);
    assert(result1 == 0xFF0000FF); // Should return red

    // Case 2: Full transparency foreground
    uint32_t fg2 = 0xFF0000FF; 
    uint32_t bg2 = 0x00FF00FF; 
    uint32_t result2 = blend_colors(fg2, bg2);
    assert(result2 == 0x00FF00FF); // Should return green

    // Case 3: 50% alpha blending
    uint32_t fg3 = 0xFFFF00FF; 
    uint32_t bg3 = 0x0000FFFF; 
    uint32_t result3 = blend_colors(fg3, bg3);
    assert(result3 == 0x7F7F80FF); 

    // Case 4: Blending fully transparent foreground
    uint32_t fg4 = 0x00000000; 
    uint32_t bg4 = 0xFFFFFFFF; 
    uint32_t result4 = blend_colors(fg4, bg4);
    assert(result4 == 0xFFFFFFFF); // whte

    // Case 5: Edge case with high alpha and dark background
    uint32_t fg5 = 0x00FFFFFF; 
    uint32_t bg5 = 0x00000000; 
    uint32_t result5 = blend_colors(fg5, bg5);
    assert(result5 == 0x00FFFFFF); // cyan

    // Case 6: Edge case with low alpha
    uint32_t fg6 = 0xFFFF00FF; 
    uint32_t bg6 = 0x0000FFFF;
    uint32_t result6 = blend_colors(fg6, bg6);
    assert(result6 == 0x7F7F80FF); 
}