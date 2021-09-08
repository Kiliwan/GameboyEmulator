#include "sidlib.h"
#include "gameboy.h"

#include <stdint.h>

#include <sys/time.h>

// Key press bits
#define MY_KEY_UP_BIT    0x01
#define MY_KEY_DOWN_BIT  0x02
#define MY_KEY_RIGHT_BIT 0x04
#define MY_KEY_LEFT_BIT  0x08
#define MY_KEY_A_BIT     0x10
#define MY_KEY_B_BIT 	 0x20
#define MY_KEY_SELECT_BIT 	 0x40
#define MY_KEY_START_BIT 	 0x80

// Global variables
gameboy_t gb;

struct timeval start;
struct timeval paused;


/**
 * @brief scale factor used for the screen of the gameboy
 */
#define GB_SCREEN_SCALE_FACTOR 3

/**
 * @brief framerate used for the gameboy
 */
#define GB_FRAMERATE 40

/*
 * for gettimeofday, NULL timezone as specified in man gettimeofday
 */

// ======================================================================
uint64_t get_time_in_GB_cyles_since(struct timeval* from)
{
    struct timeval temp = *from;
    gettimeofday(from,NULL);
    if(!timercmp(from,&temp,>)) {
        return 0;
    }
    struct timeval delta;
    timersub(from,&temp,&delta);
    return delta.tv_sec * GB_CYCLES_PER_S +  (delta.tv_usec * GB_CYCLES_PER_S) / 1000000;
}

// ======================================================================
static void set_grey(guchar* pixels, int row, int col, int width, guchar grey)
{
    const size_t i = (size_t) (3 * (row * width + col)); // 3 = RGB
    pixels[i+2] = pixels[i+1] = pixels[i] = grey;
}

// ======================================================================
static void generate_image(guchar* pixels, int height, int width)
{
    gameboy_run_until(&gb,get_time_in_GB_cyles_since(&start));

    for(int i=0; i<height; ++i) {
        for(int j=0; j<width; ++j) {
            uint8_t pixel_gameboy = 0;
            image_get_pixel(&pixel_gameboy,&(gb.screen.display),j/GB_SCREEN_SCALE_FACTOR,i/GB_SCREEN_SCALE_FACTOR);
            set_grey(pixels,i,j,width,(255 - 85 * pixel_gameboy));
        }
    }
}

// ======================================================================
#define do_key(X) \
    do { \
        if (! (psd->key_status & MY_KEY_ ## X ##_BIT)) { \
            psd->key_status |= MY_KEY_ ## X ##_BIT; \
            puts(#X " key pressed"); \
        } \
    } while(0)

static gboolean keypress_handler(guint keyval, gpointer data)
{
    simple_image_displayer_t* const psd = data;
    if (psd == NULL) return FALSE;

    switch(keyval) {
    case GDK_KEY_Up:
        return joypad_key_pressed(&(gb.pad),UP_KEY);

    case GDK_KEY_Down:
        return joypad_key_pressed(&(gb.pad),DOWN_KEY);

    case GDK_KEY_Right:
        return joypad_key_pressed(&(gb.pad),RIGHT_KEY);

    case GDK_KEY_Left:
        return joypad_key_pressed(&(gb.pad),LEFT_KEY);

    case 'A':
    case 'a':
        return joypad_key_pressed(&(gb.pad),A_KEY);

    case 'B':
    case 'b':
        return joypad_key_pressed(&(gb.pad),B_KEY);

    case GDK_KEY_Page_Up:
        return joypad_key_pressed(&(gb.pad),SELECT_KEY);

    case GDK_KEY_Page_Down:
        return joypad_key_pressed(&(gb.pad),START_KEY);

    case GDK_KEY_space: // pause management
        if(psd->timeout_id>0) {
            gettimeofday(&paused,NULL);
        } else {
            struct timeval temp;
            gettimeofday(&temp,NULL);
            timersub(&temp,&paused,&paused);
            timeradd(&start,&paused,&start);
            timerclear(&paused);
        }
        return ds_simple_key_handler(keyval, data);
    }

    return ds_simple_key_handler(keyval, data);
}
#undef do_key

// ======================================================================
#define do_key(X) \
    do { \
        if (psd->key_status & MY_KEY_ ## X ##_BIT) { \
          psd->key_status &= (unsigned char) ~MY_KEY_ ## X ##_BIT; \
            puts(#X " key released"); \
        } \
    } while(0)

static gboolean keyrelease_handler(guint keyval, gpointer data)
{
    simple_image_displayer_t* const psd = data;
    if (psd == NULL) return FALSE;

    switch(keyval) {
    case GDK_KEY_Up:
        return joypad_key_released(&(gb.pad),UP_KEY);

    case GDK_KEY_Down:
        return joypad_key_released(&(gb.pad),DOWN_KEY);

    case GDK_KEY_Right:
        return joypad_key_released(&(gb.pad),RIGHT_KEY);

    case GDK_KEY_Left:
        return joypad_key_released(&(gb.pad),LEFT_KEY);

    case 'A':
    case 'a':
        return joypad_key_released(&(gb.pad),A_KEY);

    case 'B':
    case 'b':
        return joypad_key_released(&(gb.pad),B_KEY);

    case GDK_KEY_Page_Up:
        return joypad_key_released(&(gb.pad),SELECT_KEY);

    case GDK_KEY_Page_Down:
        return joypad_key_released(&(gb.pad),START_KEY);
    }

    return FALSE;
}
#undef do_key

// ======================================================================
int main(int argc, char *argv[])
{
    gettimeofday(&start,NULL);
    timerclear(&paused);

    const char* const filename = argv[1];
    gameboy_create(&gb,filename);
    sd_launch(&argc, &argv,
              sd_init("Gameboy", LCD_WIDTH*GB_SCREEN_SCALE_FACTOR, LCD_HEIGHT*GB_SCREEN_SCALE_FACTOR, GB_FRAMERATE,
                      generate_image, keypress_handler, keyrelease_handler));
    gameboy_free(&gb);
    return 0;
}
