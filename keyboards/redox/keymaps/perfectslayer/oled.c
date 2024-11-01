// This file is used to define the OLED screen behavior
#ifdef OLED_ENABLE

#include QMK_KEYBOARD_H
#include "oled.h"
#include "keymap.h"

#define ANIM_DELAY 200
#define COLUMN_COUNT 128
#define ROW_COUNT 4

bool isRendered = false;
uint32_t anim_timer = 0;
uint8_t anim_frame = 0;

// int8_t anim_offset[] = {1, 2, 3, 4, 5, 6, 7, 8, 0, 0};
// int8_t anim_offset[] = {-1, -2, -3, -4, -5, -6, -7, -8, 0, 0};
// int8_t anim_offset[] = {2, 3, 1, -3, 2, 0, 0, 0, 0, 0};

int8_t anim_offset_scale[] = { 20, 75, 75, 85, 80, 75, 30, 30, 30, 10};
int8_t anim_offset_max[COLUMN_COUNT] = {0};

// char upperRowBuffer[128];
// char lowerRowBuffer[128];


// 'PERFECT' logo, 128x32px
// Generated using https://javl.github.io/image2cpp/ with the following settings:
// - Canvas size: 128x32
// - Background color: Black
// - Rotate image: 0 degrees
// - Code dutput format: Plain bytes
// - Draw mode: Vertical - 1 bit per pixel
const char PROGMEM perfect_logo[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xf0, 0xfc, 0xfc, 0x7c, 0x0c, 0x0c,
    0x0c, 0x0c, 0x0c, 0x0c, 0xcc, 0xfc, 0xfc, 0xfc, 0x1c, 0x80, 0xe0, 0xfc, 0xfc, 0xfc, 0x0c, 0x0c,
    0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x04, 0x80, 0xf0, 0xfc, 0xfc, 0x7c, 0x8c, 0x8c,
    0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0xec, 0xfc, 0xfc, 0x7c, 0x1c, 0x80, 0xf0, 0xfc, 0xfc, 0xfc, 0x8c,
    0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0x0c, 0x0c, 0x04, 0xe0, 0xfc, 0xfc, 0xfc, 0x1c, 0x0c,
    0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x04, 0x04, 0xe0, 0xfc, 0xfc, 0xfc, 0x1c, 0x0c,
    0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x04, 0x04, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0xcc,
    0xfc, 0xfc, 0xfc, 0x7c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x7c, 0x7f, 0x7f, 0x1f, 0x03, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x60, 0x7c, 0x7f, 0x7f, 0x7f, 0x63, 0x61, 0x61, 0x61,
    0x61, 0x61, 0x61, 0x61, 0x61, 0x01, 0x01, 0x60, 0x7c, 0x7f, 0x7f, 0x1f, 0x03, 0x00, 0x00, 0x01,
    0x07, 0x1f, 0x7f, 0x7f, 0x7d, 0x71, 0x41, 0x01, 0x60, 0x7c, 0x7f, 0x7f, 0x1f, 0x03, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x40, 0x78, 0x7f, 0x7f, 0x7f, 0x67, 0x61, 0x61, 0x61,
    0x61, 0x61, 0x61, 0x61, 0x61, 0x41, 0x01, 0x40, 0x78, 0x7f, 0x7f, 0x7f, 0x67, 0x60, 0x60, 0x60,
    0x60, 0x60, 0x60, 0x60, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x7e, 0x7f,
    0x7f, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'SLAYER' logo, 128x32px
// Generated using https://javl.github.io/image2cpp/ with the following settings:
// - Canvas size: 128x32
// - Background color: Black
// - Rotate image: 180 degrees
// - Code dutput format: Plain bytes
// - Draw mode: Vertical - 1 bit per pixel
const char PROGMEM slayer_logo[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x80, 0x86, 0x8e, 0xbe, 0xfe, 0xfe, 0xf8, 0xe0, 0x80, 0x00, 0x00, 0xe0, 0xfc, 0xfe, 0xfe,
    0x1e, 0x02, 0x80, 0x82, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0xe6, 0xfe, 0xfe, 0xfe,
    0x1e, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xf0, 0xfe, 0xfe, 0x3e, 0x0e, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0xf8, 0xfe, 0xfe, 0xfe, 0x9e, 0x82, 0x80, 0x80, 0x80, 0xc0, 0xf0,
    0xfe, 0xfe, 0xfe, 0x1e, 0x02, 0x00, 0x04, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xe6, 0xfe, 0xfe,
    0xfe, 0x1e, 0x82, 0xf0, 0xf8, 0xfc, 0xfc, 0x9e, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
    0x86, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38,
    0x3f, 0x3f, 0x3f, 0x33, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x38, 0x3f, 0x3f, 0x3f, 0x07, 0x20,
    0x20, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 0x3f, 0x3f, 0x3f, 0x07, 0x00,
    0x20, 0x30, 0x38, 0x3c, 0x1e, 0x0e, 0x07, 0x07, 0x03, 0x0f, 0x3f, 0x3f, 0x3e, 0x38, 0x20, 0x00,
    0x00, 0x00, 0x30, 0x3e, 0x3f, 0x3f, 0x3f, 0x37, 0x31, 0x31, 0x31, 0x31, 0x31, 0x3d, 0x3f, 0x3f,
    0x3f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x3f, 0x3f, 0x3f, 0x07,
    0x00, 0x20, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 0x3f, 0x3f, 0x1f, 0x0f,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static void render_perfect_logo(void) {
    oled_write_raw_P(perfect_logo, sizeof(perfect_logo));
}

static void render_slayer_logo(void) {
    oled_write_raw_P(slayer_logo, sizeof(slayer_logo));
}

void shif_logo(void) {
   // If it's the first frame, generate the max offset for each column
   if (anim_frame == 0) {
      uint8_t min_length = 12; // Minimum length of the offset
      uint8_t max_length = COLUMN_COUNT / 4; // Maximum length of the offset
      uint8_t max_offset = ROW_COUNT * 8 / 4; // Maximum offset in pixels
      uint8_t col = 0;
      while (col < COLUMN_COUNT) {
         uint8_t length = rand() % (max_length - min_length + 1) + min_length; // Get random offset length between min and max length
         int8_t offset = rand() % (2 * max_offset + 1) - max_offset; // Get random offset between -max_offset and max_offset
        //  printf("length: %d | offset: %d\n", length, offset);
         for (uint8_t i = 0; i < length && col < COLUMN_COUNT; i++) {
            anim_offset_max[col] = offset;
            col++;
         }
      }
    //   print("--- end of init\n");
    //   for (uint8_t i = 0; i < COLUMN_COUNT; i++) {
    //      printf("anim_offset_max[%d]: %d\n", i, anim_offset_max[i]);
    //   }
   }


   char buff_char;
   char complement_char;
   // Get animation ofset
   // int8_t offset = anim_offset[anim_frame];

   for (uint8_t col = 0; col < COLUMN_COUNT; col++) {
      int8_t offset = anim_offset_scale[anim_frame] * anim_offset_max[col] / 100;
      // printf("col: %d | scale: %d | offset_max: %d | offset: %d\n", col, anim_offset_scale[anim_frame], anim_offset_max[col], offset);
      for (uint8_t row = 0; row < ROW_COUNT; row++) {
         if (offset > 0) {
            complement_char = row == (ROW_COUNT - 1) ? 0 : slayer_logo[(row + 1) * COLUMN_COUNT + col] ;
            buff_char = slayer_logo[row * COLUMN_COUNT + col] >> offset | complement_char  << (8 - offset);
         } else {
            complement_char = row == 0 ? 0 : slayer_logo[(row - 1) * COLUMN_COUNT + col] ;
            buff_char = slayer_logo[row * COLUMN_COUNT + col] << -offset | complement_char >> (8 + offset);
         }
         oled_write_raw_byte(buff_char, row * COLUMN_COUNT + col);
      }
   }


   anim_frame++;
   if (anim_frame == 10) {
      anim_frame = 0;
      isRendered = false;
   }
}

void animate_logo(void) {
   // print("animate_logo\n");
   if (!isRendered) {
      render_perfect_logo();
      isRendered = true;
      return;
   }
   // fade_display();
   if (timer_elapsed(anim_timer) > ANIM_DELAY) {
      anim_timer = timer_read();
      // print("before shif_logo\n");
      shif_logo();
      printf("after shif_logo %d\n", anim_frame);
   }
}

void oled_show_status(void) {
    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer: "), false);

    switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("Default\n"), false);
            break;
        case _SYMB:
            oled_write_P(PSTR("Symbols\n"), false);
            break;
        case _ADJUST:
            oled_write_P(PSTR("ADJ\n"), false);
            break;
        default:
            // Or use the write_ln shortcut over adding '\n' to the end of your string
            oled_write_ln_P(PSTR("Undefined"), false);
    }

    // Host Keyboard LED Status
    led_t led_state = host_keyboard_led_state();
    oled_write_P(led_state.num_lock ? PSTR("NUM ") : PSTR("    "), false);
    oled_write_P(led_state.caps_lock ? PSTR("CAP ") : PSTR("    "), false);
    oled_write_P(led_state.scroll_lock ? PSTR("SCR ") : PSTR("    "), false);
}

void oled_master_render(void) {
    // oled_show_status();
    render_slayer_logo();
}

void oled_slave_render(void) {
    // animate_logo();
    render_perfect_logo();
}

#endif
