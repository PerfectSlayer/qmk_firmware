#include QMK_KEYBOARD_H
#include "keymap.h"
#ifdef OLED_ENABLE
#    include "metroid.c"
#endif


#define RAISE_L MO(_RAISE)
#define ADJUS_L MO(_ADJUST)

#define KC_LOSP LT(_LOWER, KC_SPC) // Turn on Lower when held, Space when tapped
#define KC_RSTB LT(_RAISE, KC_TAB) // Turn on Raise when held, Tab when tapped
#define KC_RAQU RALT_T(KC_QUOT) // Right Alt when held, M when tapped (Quote as M when remapped)

// Left Shift and left control need improvements (LSFT is doubled)
// On the right part, backspace and right GUI can be removed. What put instead?

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_ESC , KC_1   , KC_2   , KC_3   , KC_4   , KC_5   ,                   KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_EQL ,
        KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   ,                   KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_LBRC,
        KC_LCTL, KC_A   , KC_S   , KC_D   , KC_F   , KC_G   ,                   KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_RAQU,
        KC_LSFT, KC_LSFT, KC_X   , KC_C   , KC_V   , KC_B   , KC_GRV , KC_MINS, KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_RBRC,
                                   KC_LGUI, KC_LALT, KC_LOSP, KC_BSPC, KC_ENT , KC_RSTB, KC_BSPC, KC_RGUI
    ),
    [_LOWER] = LAYOUT(
        _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
        KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  , KC_F6  ,                   KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 , KC_F12 ,
        KC_GRV , KC_EXLM, KC_AT  , KC_HASH, KC_DLR , KC_PERC,                   KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_TILD,
        _______, _______, _______, _______, _______, _______, _______, _______, XXXXXXX, KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE,
                                   _______, _______, _______, _______, _______, ADJUS_L, _______, _______
    ),
    [_RAISE] = LAYOUT(
        _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
        KC_GRV , KC_1   , KC_2   , KC_3   , KC_4   , KC_5   ,                   KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , _______,
        _______, KC_Z   , XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, KC_LEFT, KC_DOWN, KC_UP  , KC_RGHT, XXXXXXX,
        _______, _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, _______, KC_PLUS, KC_MINS, KC_EQL , KC_LBRC, KC_RBRC, KC_BSLS,
                                   _______, _______, _______, KC_DEL , _______, _______, _______, _______
    ),
    [_ADJUST] = LAYOUT(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, RM_TOGG, RM_HUEU, RM_SATU, RM_VALU,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, RM_NEXT, RM_HUED, RM_SATD, RM_VALD,
                                   _______, _______, _______, _______, _______, _______, _______, _______
    )
};

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    // Check if LOWER layer is active
    if (layer_state_is(_LOWER)) {
        if (!clockwise) {
            // Go to last edit location: Ctrl + Shift + Backspace
            tap_code16(C(S(KC_BSPC)));
        }
    } else {
        // Default behavior for all other layers
        if (clockwise) {
            // Redo: Ctrl + Shift + Z (left bracket used as Z when remapped)
            tap_code16(C(S(KC_LBRC)));
        } else {
            // Undo: Ctrl + Z (left bracket used as Z when remapped)
            tap_code16(C(KC_LBRC));
        }
    }

    return false;
}
#endif

#ifdef OLED_ENABLE
bool oled_task_user(void) {
    if (!is_keyboard_master()) {
        static uint8_t  current_sprite = 0;  // 0 = idle, 1-4 = animation frames
        static uint32_t last_update    = 0;
        static uint32_t last_interval  = 400;
        uint32_t now = timer_read32();

        uint8_t wpm    = get_current_wpm();
        bool    is_idle = (wpm == 0);

        if (current_sprite == 0) {
            if (!is_idle) {
                // Typing started - begin animation from frame 1
                current_sprite = 1;
                last_update    = now;
            }
        } else {
            // Animating (frames 1-4)
            last_interval = (wpm > 100) ? 75 : (wpm > 50) ? 150 : (wpm > 20) ? 250 : 400;

            if (now - last_update > last_interval) {
                if (current_sprite == 4 && is_idle) {
                    // Full cycle done and still idle - return to sprite0
                    current_sprite = 0;
                } else {
                    current_sprite = (current_sprite % 4) + 1;
                }
                last_update = now;
            }
        }

        switch (current_sprite) {
            case 0: oled_write_raw_P(sprite0, sizeof(sprite0)); break;
            case 1: oled_write_raw_P(sprite1, sizeof(sprite1)); break;
            case 2: oled_write_raw_P(sprite2, sizeof(sprite2)); break;
            case 3: oled_write_raw_P(sprite3, sizeof(sprite3)); break;
            case 4: oled_write_raw_P(sprite4, sizeof(sprite4)); break;
        }
        return false;
    }
    return true;
}
#endif
