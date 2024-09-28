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
void test_all_tiles_nonempty( TestObjs *objs );
void test_determine_tile_w( TestObjs *objs );
void test_determine_tile_x_offset( TestObjs *objs );
void test_determine_tile_h( TestObjs *objs );
void test_determine_tile_y_offset( TestObjs *objs );
void test_get_r( TestObjs *objs );
void test_get_g( TestObjs *objs );
void test_get_b( TestObjs *objs );
void test_get_a( TestObjs *objs );
void test_make_pixel( TestObjs *objs );
void test_to_grayscale( TestObjs *objs );
void test_blend_components( TestObjs *objs );
void test_blend_colors( TestObjs *objs );
void test_calculate_starting_index ( TestObjs *objs );
void test_copy_tile_1 ( TestObjs *objs);
void test_copy_tile_2 ( TestObjs *objs);
void test_copy_tile_3 ( TestObjs *objs);
void test_get_r_M2();
void test_get_g_M2();
void test_get_b_M2();
void test_get_a_M2();
void test_make_pixel_M2();
void test_to_grayscale_M2();


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
  TEST( test_all_tiles_nonempty );
  TEST( test_determine_tile_w );
  TEST( test_determine_tile_x_offset );
  TEST( test_determine_tile_h );
  TEST( test_determine_tile_y_offset );
  TEST( test_get_r );
  TEST( test_get_g );
  TEST( test_get_b );
  TEST( test_get_a );
  TEST( test_make_pixel );
  TEST( test_to_grayscale );
  TEST( test_blend_components );
  TEST( test_blend_colors );
  TEST( test_calculate_starting_index );
  TEST( test_copy_tile_1 );
  TEST( test_copy_tile_2 );
  TEST( test_copy_tile_3 );
  TEST(test_get_r_M2);
  TEST(test_get_g_M2);
  TEST(test_get_b_M2);
  TEST(test_get_a_M2);
  TEST(test_make_pixel_M2);
  TEST(test_to_grayscale_M2);
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




// M2 TESTS

void test_get_r_M2() {
  // Standard case: Extract red from a known pixel value
  uint32_t pixel = 0x11223344;  
  uint32_t expected_red = 0x11;
  assert(get_r(pixel) == expected_red);

  // Edge case: All color channels maxed out
  pixel = 0xFFFFFFFF;  // Red = 0xFF
  expected_red = 0xFF;
  assert(get_r(pixel) == expected_red);

  // Edge case: All channels zeroed out
  pixel = 0x00000000;  // Red = 0x00
  expected_red = 0x00;
  assert(get_r(pixel) == expected_red);
}

void test_get_g_M2() {
  uint32_t pixel = 0x11223344;  // Green = 0x22
  uint32_t expected_green = 0x22;
  assert(get_g(pixel) == expected_green);

  // Edge case: All color channels maxed out
  pixel = 0xFFFFFFFF;  // Green = 0xFF
  expected_green = 0xFF;
  assert(get_g(pixel) == expected_green);

  // Edge case: All channels zeroed out
  pixel = 0x00000000;  // Green = 0x00
  expected_green = 0x00;
  assert(get_g(pixel) == expected_green);
}

void test_get_b_M2() {
  uint32_t pixel = 0x11223344;  // Blue = 0x33
  uint32_t expected_blue = 0x33;
  assert(get_b(pixel) == expected_blue);

  // Edge case: All color channels maxed out
  pixel = 0xFFFFFFFF;  // Blue = 0xFF
  expected_blue = 0xFF;
  assert(get_b(pixel) == expected_blue);

  // Edge case: All channels zeroed out
  pixel = 0x00000000;  // Blue = 0x00
  expected_blue = 0x00;
  assert(get_b(pixel) == expected_blue);
}

void test_get_a_M2() {
  uint32_t pixel = 0x11223344;  // Alpha = 0x44
  uint32_t expected_alpha = 0x44;
  assert(get_a(pixel) == expected_alpha);

  // Edge case: All color channels maxed out
  pixel = 0xFFFFFFFF;  // Alpha = 0xFF
  expected_alpha = 0xFF;
  assert(get_a(pixel) == expected_alpha);

  // Edge case: All channels zeroed out
  pixel = 0x00000000;  // Alpha = 0x00
  expected_alpha = 0x00;
  assert(get_a(pixel) == expected_alpha);
}

void test_make_pixel_M2() {
  uint32_t r = 0x11, g = 0x22, b = 0x33, a = 0x44;
  uint32_t expected_pixel = 0x11223344;
  assert(make_pixel(r, g, b, a) == expected_pixel);

  // Edge case: All channels zeroed out
  r = g = b = a = 0x00;
  expected_pixel = 0x00000000;
  assert(make_pixel(r, g, b, a) == expected_pixel);

  // Edge case: All channels maxed out
  r = g = b = a = 0xFF;
  expected_pixel = 0xFFFFFFFF;
  assert(make_pixel(r, g, b, a) == expected_pixel);

  // Edge case: Half maxed values
  r = 0x7F, g = 0x7F, b = 0x7F, a = 0x7F;
  expected_pixel = 0x7F7F7F7F;
  assert(make_pixel(r, g, b, a) == expected_pixel);
}

void test_to_grayscale_M2() {
  // Example pixel: R = 79, G = 128, B = 49 -> Grayscale approx 0x7F7F7F7F
  uint32_t pixel = 0x4F803100;  // R=79, G=128, B=49, A=0x00
  uint32_t expected_grayscale = 0x7F7F7F00;
  assert(to_grayscale(pixel) == expected_grayscale);

  // Edge case: Fully transparent pixel
  pixel = 0x00000000;  // All color channels = 0, A = 0
  expected_grayscale = 0x00000000;
  assert(to_grayscale(pixel) == expected_grayscale);

  // Edge case: Opaque white pixel (R=G=B=255)
  pixel = 0xFFFFFFFF;  // R=G=B=255, A=255
  expected_grayscale = 0xFFFFFFFF;
  assert(to_grayscale(pixel) == expected_grayscale);

  // Edge case: Black pixel (R=G=B=0, A=255)
  pixel = 0x000000FF;
  expected_grayscale = 0x000000FF;
  assert(to_grayscale(pixel) == expected_grayscale);

  // Edge case: Middle gray pixel (R=G=B=127)
  pixel = 0x7F7F7FFF;
  expected_grayscale = 0x7F7F7FFF;
  assert(to_grayscale(pixel) == expected_grayscale);
}



// M1 TESTS
void test_all_tiles_nonempty( TestObjs *objs ) {
  ASSERT(all_tiles_nonempty(objs->smiley->width, objs->smiley->height, 1) == 1);
  ASSERT(all_tiles_nonempty(objs->smiley->width, objs->smiley->height, 10) == 1);
  ASSERT(all_tiles_nonempty(objs->smiley->width, objs->smiley->height, 16) == 0);
  ASSERT(all_tiles_nonempty(objs->smiley->width, objs->smiley->height, 17) == 0);

  Picture empty = {
    TEST_COLORS,
    0, // width
    0, // height
    ""
  };
  struct Image *empty_img = picture_to_img( &empty );

  ASSERT(all_tiles_nonempty(empty_img->width, empty_img->height, 1) == 0);

  Picture empty_one_row = {
    TEST_COLORS,
    0, // width
    2, // height
    ""
  };
  struct Image *empty_one_row_img = picture_to_img( &empty_one_row );
  ASSERT(all_tiles_nonempty(empty_one_row_img->width, empty_one_row_img->height, 1) == 0);

  Picture empty_one_col = {
    TEST_COLORS,
    0, // width
    2, // height
    ""
  };
  struct Image *empty_one_col_img = picture_to_img( &empty_one_col );
  ASSERT(all_tiles_nonempty(empty_one_col_img->width, empty_one_col_img->height, 1) == 0);
}

void test_determine_tile_w( TestObjs *objs ) {
  ASSERT(determine_tile_w(objs->smiley->width, 1) == 16);
  ASSERT(determine_tile_w(objs->smiley->width, 2) == 8);
  ASSERT(determine_tile_w(objs->smiley->width, 3) == 5);
  ASSERT(determine_tile_w(objs->smiley->width, 14) == 1);
  ASSERT(determine_tile_w(objs->smiley->width, 15) == 1);
  ASSERT(determine_tile_w(objs->smiley->width, 16) == 1);
}

void test_determine_tile_x_offset( TestObjs *objs ) {
  ASSERT(determine_tile_x_offset(objs->smiley->width, 3,0) == 1);
  ASSERT(determine_tile_x_offset(objs->smiley->width, 3,1) == 0);
  ASSERT(determine_tile_x_offset(objs->smiley->width, 3,2) == 0);

  ASSERT(determine_tile_x_offset(objs->smiley->width, 7,0) == 1);
  ASSERT(determine_tile_x_offset(objs->smiley->width, 7,1) == 1);
  ASSERT(determine_tile_x_offset(objs->smiley->width, 7,6) == 0);
  ASSERT(determine_tile_x_offset(objs->smiley->width, 7,7) == 0);

  ASSERT(determine_tile_x_offset(objs->smiley->width, 13,0) == 1);
  ASSERT(determine_tile_x_offset(objs->smiley->width, 13,1) == 1);
  ASSERT(determine_tile_x_offset(objs->smiley->width, 13,2) == 1);
  ASSERT(determine_tile_x_offset(objs->smiley->width, 13,11) == 0);
  ASSERT(determine_tile_x_offset(objs->smiley->width, 13,12) == 0);

  ASSERT(determine_tile_x_offset(objs->smiley->width, 16,0) == 0);
  ASSERT(determine_tile_x_offset(objs->smiley->width, 16,1) == 0);
  ASSERT(determine_tile_x_offset(objs->smiley->width, 16,14) == 0);
  ASSERT(determine_tile_x_offset(objs->smiley->width, 16,15) == 0);
}

void test_determine_tile_h( TestObjs *objs ) {
  ASSERT(determine_tile_h(objs->smiley->height, 1) == 10);
  ASSERT(determine_tile_h(objs->smiley->height, 2) == 5);
  ASSERT(determine_tile_h(objs->smiley->height, 3) == 3);
  ASSERT(determine_tile_h(objs->smiley->height, 8) == 1);
  ASSERT(determine_tile_h(objs->smiley->height, 9) == 1);
  ASSERT(determine_tile_h(objs->smiley->height, 10) == 1);
}

void test_determine_tile_y_offset( TestObjs *objs ) {
   ASSERT(determine_tile_y_offset(objs->smiley->height, 3,0) == 1);
  ASSERT(determine_tile_y_offset(objs->smiley->height, 3,1) == 0);
  ASSERT(determine_tile_y_offset(objs->smiley->height, 3,2) == 0);

  ASSERT(determine_tile_y_offset(objs->smiley->height, 6,0) == 1);
  ASSERT(determine_tile_y_offset(objs->smiley->height, 6,1) == 1);
  ASSERT(determine_tile_y_offset(objs->smiley->height, 6,4) == 0);
  ASSERT(determine_tile_y_offset(objs->smiley->height, 6,5) == 0);

  ASSERT(determine_tile_y_offset(objs->smiley->height, 9,0) == 1);
  ASSERT(determine_tile_y_offset(objs->smiley->height, 9,1) == 0);
  ASSERT(determine_tile_y_offset(objs->smiley->height, 9,8) == 0);
  ASSERT(determine_tile_y_offset(objs->smiley->height, 9,8) == 0);

  ASSERT(determine_tile_y_offset(objs->smiley->height, 10,0) == 0);
  ASSERT(determine_tile_y_offset(objs->smiley->height, 10,1) == 0);
  ASSERT(determine_tile_y_offset(objs->smiley->height, 10,9) == 0);
  ASSERT(determine_tile_y_offset(objs->smiley->height, 10,10) == 0);
}

void test_get_r( TestObjs *objs ) {
  ASSERT(get_r(0x00000000U) == 0x00000000U);
  ASSERT(get_r(0xFF000000U) == 0x000000FFU);
  ASSERT(get_r(0x01000000U) == 0x00000001U);
  ASSERT(get_r(0x11000000U) == 0x00000011U);
  ASSERT(get_r(0xFFFFFFFFU) == 0x000000FFU);
}
void test_get_g( TestObjs *objs ) {
  ASSERT(get_g(0x00000000U) == 0x00000000U);
  ASSERT(get_g(0x00FF0000U) == 0x000000FFU);
  ASSERT(get_g(0x00110000U) == 0x00000011U);
  ASSERT(get_g(0x00010000U) == 0x00000001U);
  ASSERT(get_g(0xFFFFFFFFU) == 0x000000FFU);
}
void test_get_b( TestObjs *objs ) {
  ASSERT(get_b(0x00000000U) == 0x00000000U);
  ASSERT(get_b(0x0000FF00U) == 0x000000FFU);
  ASSERT(get_b(0x00001100U) == 0x00000011U);
  ASSERT(get_b(0x00000100U) == 0x00000001U);
  ASSERT(get_b(0xFFFFFFFFU) == 0x000000FFU);
}
void test_get_a( TestObjs *objs ) {
  ASSERT(get_a(0x00000000U) == 0x00000000U);
  ASSERT(get_a(0x000000FFU) == 0x000000FFU);
  ASSERT(get_a(0x00000011U) == 0x00000011U);
  ASSERT(get_a(0x00000001U) == 0x00000001U);
  ASSERT(get_a(0xFFFFFFFFU) == 0x000000FFU);
}
void test_make_pixel( TestObjs *objs ) {
  ASSERT(make_pixel(0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U) == 0x00000000U);
  ASSERT(make_pixel(0x00000001U, 0x00000000U, 0x00000000U, 0x00000000U) == 0x01000000U);
  ASSERT(make_pixel(0x00000000U, 0x00000001U, 0x00000000U, 0x00000000U) == 0x00010000U);
  ASSERT(make_pixel(0x00000000U, 0x00000000U, 0x00000001U, 0x00000000U) == 0x00000100U);
  ASSERT(make_pixel(0x00000000U, 0x00000000U, 0x00000000U, 0x00000001U) == 0x00000001U);

  ASSERT(make_pixel(0x000000FFU, 0x000000FFU, 0x000000FFU, 0x000000FFU) == 0xFFFFFFFFU);
}

void test_to_grayscale( TestObjs *objs ) {
  ASSERT(to_grayscale(0x00000000U) == 0x00000000U);
  ASSERT(to_grayscale(0x000000FFU) == 0x000000FFU);
  ASSERT(to_grayscale(0xFFFFFFFFU) == 0xFFFFFFFFU);
  ASSERT(to_grayscale(0xFFFFFF00U) == 0xFFFFFF00U);

  ASSERT(to_grayscale(0x01000000U) == 0x00000000U);
  ASSERT(to_grayscale(0xFF000000U) == 0x4E4E4E00U);

  ASSERT(to_grayscale(0x00010000U) == 0x00000000U);
  ASSERT(to_grayscale(0x00FF0000U) == 0x7F7F7F00U);

  ASSERT(to_grayscale(0x00000100U) == 0x00000000U);
  ASSERT(to_grayscale(0x0000FF00U) == 0x30303000U);

  ASSERT(to_grayscale(0x0F0F0F0FU) == 0x0F0F0F0FU);
}

void test_blend_components ( TestObjs *objs ) {
  ASSERT(blend_components(0x00000000U, 0x00000000U, 0x00000000U) == 0x00000000U);
  ASSERT(blend_components(0x000000FFU, 0x00000000U, 0x00000001U) == 0x00000001U);
  ASSERT(blend_components(0x000000FFU, 0x00000000U, 0x000000FFU) == 0x000000FFU);
  ASSERT(blend_components(0x00000000U, 0x000000FFU, 0x000000FFU) == 0x00000000U);
  ASSERT(blend_components(0x00000000U, 0x000000FFU, 0x00000000U) == 0x000000FFU);
  ASSERT(blend_components(0x000000FFU, 0x00000001U, 0x00000080U) == 0x00000080U);
  ASSERT(blend_components(0x00000001U, 0x00000000U, 0x00000000U) == 0x00000000U);
}

void test_blend_colors ( TestObjs *objs ) {
  ASSERT(blend_colors(0x00000000U, 0x00000000U) == 0x000000FFU);
  ASSERT(blend_colors(0xFFFFFF00U, 0x00000000U) == 0x000000FFU);
  ASSERT(blend_colors(0xFFFFFF00U, 0x01010101U) == 0x010101FFU);
  ASSERT(blend_colors(0xFFFFFF00U, 0x01010101U) == 0x010101FFU);
  ASSERT(blend_colors(0x01010101U, 0x01010101U) == 0x010101FFU);
  ASSERT(blend_colors(0x00000000U, 0xFFFFFFFFU) == 0xFFFFFFFFU);
  ASSERT(blend_colors(0x00000000U, 0xFF00FFFFU) == 0xFF00FFFFU);
}

void test_calculate_starting_index ( TestObjs *objs ) {
  ASSERT(calculate_starting_index(objs->smiley, 0, 0, 2) == 0);
  ASSERT(calculate_starting_index(objs->smiley, 1, 1, 2) == 88);

  ASSERT(calculate_starting_index(objs->smiley, 0, 0, 3) == 0);
  ASSERT(calculate_starting_index(objs->smiley, 2, 2, 3) == 123);
  ASSERT(calculate_starting_index(objs->smiley, 1, 1, 3) == 70);
  
  ASSERT(calculate_starting_index(objs->smiley, 0, 0, 6) == 0);
  ASSERT(calculate_starting_index(objs->smiley, 5, 5, 6) == 158);
  ASSERT(calculate_starting_index(objs->smiley, 3, 2, 6) == 102);

  ASSERT(calculate_starting_index(objs->smiley, 0, 0, 9) == 0);
  ASSERT(calculate_starting_index(objs->smiley, 8, 8, 9) == 159);
  ASSERT(calculate_starting_index(objs->smiley, 5, 6, 9) == 108);
}

void test_copy_tile_1 ( TestObjs *objs) {
  for (int tile_row = 0; tile_row < 2; tile_row++) {
    for (int tile_col = 0; tile_col < 2; tile_col++) {
      //copy the sampled tile from the input image to the output
      copy_tile(objs->smiley_out, objs->smiley, tile_row, tile_col, 2);
    }
  }
  int pos;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      pos = (i*16) + j;
      if (pos == 2) {
        ASSERT(objs->smiley_out->data[pos] = 0xFF0000FFU);
      } else if (pos == 3 || pos == 34) {
        ASSERT(objs->smiley_out->data[pos] = 0x00FF00FFU);
      }  else if (pos == 35) {
        ASSERT(objs->smiley_out->data[pos] = 0x0000FFFFU);
      } else {
        ASSERT(objs->smiley_out->data[pos] = 0x000000FFU);
      }
    }
  }
}


void test_copy_tile_2 ( TestObjs *objs) {
  copy_tile(objs->smiley_out, objs->smiley, 0, 0, 9);
  ASSERT(objs->smiley_out->data[0] = 0x000000FFU);
  ASSERT(objs->smiley_out->data[2] = 0x00FF00FFU);
  ASSERT(objs->smiley_out->data[16] = 0x000000FFU);
  ASSERT(objs->smiley_out->data[17] = 0x000000FFU);

  copy_tile(objs->smiley_out, objs->smiley, 2, 5, 9);
  ASSERT(objs->smiley_out->data[58] = 0x000000FFU);
  ASSERT(objs->smiley_out->data[59] = 0x00FF00FFU);

  copy_tile(objs->smiley_out, objs->smiley, 6, 1, 9);
  ASSERT(objs->smiley_out->data[114] = 0x000000FFU);
  ASSERT(objs->smiley_out->data[115] = 0x00FF00FFU);

  ASSERT(objs->smiley_out->data[159] = 0x000000FFU);
}

void test_copy_tile_3 ( TestObjs *objs) {
  copy_tile(objs->smiley_out, objs->smiley, 0, 0, 2);
  ASSERT(objs->smiley_out->data[0] = 0x000000FFU);
  ASSERT(objs->smiley_out->data[2] = 0xFF00FFFFU);
  ASSERT(objs->smiley_out->data[3] = 0xFF0000FFU);
  ASSERT(objs->smiley_out->data[4] = 0x00FF00FFU);
  ASSERT(objs->smiley_out->data[5] = 0x0000FFFFU);

  ASSERT(objs->smiley_out->data[66] = 0x00FF00FFU);
  ASSERT(objs->smiley_out->data[67] = 0x00FF00FFU);
  ASSERT(objs->smiley_out->data[68] = 0xFF0000FFU);
  ASSERT(objs->smiley_out->data[69] = 0xFF00FFFFU);


  copy_tile(objs->smiley_out, objs->smiley, 1, 1, 2);
  ASSERT(objs->smiley_out->data[88] = 0x000000FFU);
  ASSERT(objs->smiley_out->data[90] = 0xFF00FFFFU);
  ASSERT(objs->smiley_out->data[91] = 0xFF0000FFU);
  ASSERT(objs->smiley_out->data[92] = 0x00FF00FFU);
  ASSERT(objs->smiley_out->data[93] = 0x0000FFFFU);

  ASSERT(objs->smiley_out->data[154] = 0x00FF00FFU);
  ASSERT(objs->smiley_out->data[155] = 0x00FF00FFU);
  ASSERT(objs->smiley_out->data[156] = 0xFF0000FFU);
  ASSERT(objs->smiley_out->data[157] = 0xFF00FFFFU);
  
}