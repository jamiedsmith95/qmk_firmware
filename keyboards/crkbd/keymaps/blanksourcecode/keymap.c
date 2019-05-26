#include QMK_KEYBOARD_H
#include "bootloader.h"
#ifdef PROTOCOL_LUFA
  #include "lufa.h"
  #include "split_util.h"
#endif
#ifdef SSD1306OLED
  #include "ssd1306.h"
#endif

extern keymap_config_t keymap_config;

extern uint8_t is_master;

// Layer defines
#define _COLEMAKDH 0
#define _CODING 1
#define _ARROWS 2
#define _MOUSE 3
#define _NUMPAD 4
#define _RGB 5
#define _GAMING 6
#define _TEMPLATE 7

// Shorthand Macros
#define KC_TD(a) TD(a)
#define KC_LGZ LGUI_T(KC_Z)
#define KC_LAX LALT_T(KC_X)
#define KC_SHL LSFT_T(KC_LEFT)
#define KC_SHR LSFT_T(KC_RGHT)
#define KC_CTLL LCTL_T(KC_LEFT)
#define KC_CTLR LCTL_T(KC_RGHT)
#define KC_SDEL LSFT_T(KC_DEL)
#define KC_ASPC LT(_ARROWS,KC_SPC)
#define KC_CESC LCTL_T(KC_ESC)

// Custom keycodes
enum {
  SHIFT_TAB = SAFE_RANGE,
  REDO_NUM_F11
};

// Tap-dance built-ins
void tapdance_oneach (qk_tap_dance_state_t *state, void *user_data);
void tapdance_onfinished (qk_tap_dance_state_t *state, void *user_data);
void tapdance_onreset (qk_tap_dance_state_t *state, void *user_data);

int cur_dance (qk_tap_dance_state_t *state);
typedef struct {
  bool is_press_action;
  int state;
} tap;

typedef struct {
  int single_tap;
  int single_hold;
  int double_tap;
  bool is_double_tap_layer_lock;
  bool use_instant_layer;
} tri_layer_action;

enum {
  SINGLE_TAP = 1,
  SINGLE_HOLD = 2,
  DOUBLE_TAP = 3,
  DOUBLE_HOLD = 4,
  DOUBLE_SINGLE_TAP = 5,
  TRIPLE_TAP = 6,
  TRIPLE_HOLD = 7
};

// Tap-dance defines
enum {
  TD_ENT = 0,
  TD_NUM,
  TD_SPC,
};

static tri_layer_action tri_layer_actions [] = {
  [TD_ENT] = { .single_tap = KC_ENT, .single_hold = _RGB, .double_tap = _RGB, .is_double_tap_layer_lock = true, .use_instant_layer = false },
  [TD_NUM] = { .single_tap = KC_F10, .single_hold = _NUMPAD, .double_tap = KC_F5, .is_double_tap_layer_lock = false, .use_instant_layer = true  },
  [TD_SPC] = { .single_tap = KC_F11, .single_hold = _CODING, .double_tap = KC_SPC, .is_double_tap_layer_lock = false, .use_instant_layer = true  }
};

static tap tap_states [] = {
  [TD_ENT] = { .is_press_action = true, .state = 0 },
  [TD_NUM] = { .is_press_action = true, .state = 0 },
  [TD_SPC] = { .is_press_action = true, .state = 0 }
};

qk_tap_dance_action_t tap_dance_actions[] = {
  [TD_ENT] = { .fn = { tapdance_oneach, tapdance_onfinished, tapdance_onreset }, .user_data = (void*)TD_ENT },
  [TD_NUM] = { .fn = { tapdance_oneach, tapdance_onfinished, tapdance_onreset }, .user_data = (void*)TD_NUM },
  [TD_SPC] = { .fn = { tapdance_oneach, tapdance_onfinished, tapdance_onreset }, .user_data = (void*)TD_SPC }
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_COLEMAKDH] = LAYOUT_kc( \
  //,-----------------------------------------.                ,-----------------------------------------.
        TAB,   Q,     W,     F,     P,     B,                      J,     L,     U,     Y,  SCLN,  BSPC,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
       BSPC,   A,     R,     S,     T,     G,                      M,     N,     E,     I,    O,   QUOT,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
       CESC,  LGZ,  LAX,     C,     D,     V,                      K,     H,  COMM,   DOT,  SLSH,  RSFT,\
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                          TD(TD_NUM),TD(TD_SPC),SDEL,      ENT,  ASPC,  TRNS \
                              //`--------------------'  `--------------------'
  ),

  [_CODING] = LAYOUT_kc( \
  //,-----------------------------------------.                ,-----------------------------------------.
        F1,  TILD,  TRNS,  TRNS,  PPLS,   EQL,                   PIPE,  LCBR,  RCBR,  CIRC,  COLN,  TRNS,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      TRNS,   GRV,   DLR,  ASTR,  PMNS,    GT,                   AMPR,  LPRN,  RPRN,  LBRC,  RBRC,  DQUO,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      TRNS,  TRNS,  TRNS,  PERC,  UNDS,    LT,                   EXLM,   EQL,    LT,    GT,  QUES,  TRNS,\
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                  TRNS,  TRNS,  TRNS,     TRNS,  TRNS,  TRNS \
                              //`--------------------'  `--------------------'
  ),

  [_ARROWS] = LAYOUT_kc( \
  //,-----------------------------------------.                ,-----------------------------------------.
      TRNS,  TRNS,  TRNS,  TRNS,  TRNS,  TRNS,                   TRNS,  CTLL,   UP,   CTLR,  TRNS,  TRNS,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      TRNS,  TRNS,  TRNS,  TRNS,  TRNS,  TRNS,                   HOME,  LEFT,  DOWN,  RGHT,   END,  TRNS,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      TRNS,  TRNS,  TRNS,  TRNS,  TRNS,  TRNS,                   TRNS,   SHL,  TRNS,   SHR,  TRNS,  TRNS,\
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                  TRNS,  TRNS,  TRNS,     TRNS,  TRNS,  TRNS \
                              //`--------------------'  `--------------------'
  ),

  [_NUMPAD] = LAYOUT_kc( \
  //,-----------------------------------------.                ,-----------------------------------------.
      TRNS,  TRNS,  TRNS,  TRNS,  TRNS,  TRNS,                   TRNS,    P7,    P8,    P9,  PMNS,  TRNS,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      TRNS,  TRNS,  TRNS,  TRNS,  TRNS,  TRNS,                   TRNS,    P4,    P5,    P6,  PPLS,  TRNS,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      TRNS,  TRNS,  TRNS,  TRNS,  TRNS,  TRNS,                   TRNS,    P1,    P2,    P3,   EQL,  TRNS,\
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                  TRNS,  TRNS,  TRNS,     TRNS,    P0,  PDOT \
                              //`--------------------'  `--------------------'
  ),

  [_TEMPLATE] = LAYOUT_kc( \
  //,-----------------------------------------.                ,-----------------------------------------.
      TRNS,  TRNS,  TRNS,  TRNS,  TRNS,  TRNS,                   TRNS,  TRNS,  TRNS,  TRNS,  TRNS,  TRNS,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      TRNS,  TRNS,  TRNS,  TRNS,  TRNS,  TRNS,                   TRNS,  TRNS,  TRNS,  TRNS,  TRNS,  TRNS,\
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      TRNS,  TRNS,  TRNS,  TRNS,  TRNS,  TRNS,                   TRNS,  TRNS,  TRNS,  TRNS,  TRNS,  TRNS,\
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                  TRNS,  TRNS,  TRNS,     TRNS,  TRNS,  TRNS \
                              //`--------------------'  `--------------------'
  ),
};

int RGB_current_mode;

void matrix_init_user(void) {
    iota_gfx_init(!has_usb());
}

// When add source files to SRC in rules.mk, you can use functions.
const char *read_layer_state(void);
const char *read_logo(void);
void set_keylog(uint16_t keycode, keyrecord_t *record);
const char *read_keylog(void);
const char *read_keylogs(void);

// const char *read_mode_icon(bool swap);
// const char *read_host_led_state(void);
// void set_timelog(void);
// const char *read_timelog(void);

void matrix_scan_user(void) {
   iota_gfx_task();
}

void matrix_render_user(struct CharacterMatrix *matrix) {
  if (is_master) {
    // If you want to change the display of OLED, you need to change here
    matrix_write_ln(matrix, read_layer_state());
    matrix_write_ln(matrix, read_keylog());
    matrix_write_ln(matrix, read_keylogs());
    //matrix_write_ln(matrix, read_mode_icon(keymap_config.swap_lalt_lgui));
    //matrix_write_ln(matrix, read_host_led_state());
    //matrix_write_ln(matrix, read_timelog());
  } else {
    matrix_write(matrix, read_logo());
  }
}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
  if (memcmp(dest->display, source->display, sizeof(dest->display))) {
    memcpy(dest->display, source->display, sizeof(dest->display));
    dest->dirty = true;
  }
}

void iota_gfx_task_user(void) {
  struct CharacterMatrix matrix;
  matrix_clear(&matrix);
  matrix_render_user(&matrix);
  matrix_update(&display, &matrix);
}

// Base layer color values
uint16_t h = 240;
uint8_t s = 255;
uint8_t v = 255;
bool is_in_rgb_mode = false;

void eeconfig_init_user(void) {
  rgblight_enable();
  rgblight_set_effect_range(0, 27);
  rgblight_sethsv_noeeprom(h, s, v);
}

uint32_t layer_state_set_user(uint32_t state) {
  static uint8_t old_layer = 255;
  uint8_t new_layer = biton32(state);

  if (old_layer != new_layer) {
    if (old_layer == _RGB) {
      is_in_rgb_mode = false;
      if (rgblight_get_mode() != false) {
        uint16_t newH = rgblight_get_hue();
        uint8_t newS = rgblight_get_sat();
        uint8_t newV = rgblight_get_val();

        if (newH != 300 || newS != 255 || newV != v) {
          h = newH;
          s = newS;
          v = newV;
        }
      }
    }

    switch (new_layer) {
      case _COLEMAKDH:
        rgblight_sethsv_noeeprom(h, s, v);
        break;
      case _CODING:
        rgblight_sethsv_noeeprom(0, 255, v);
        break;
      case _ARROWS:
        rgblight_sethsv_noeeprom(120, 255, v);
        break;
      case _MOUSE:
        rgblight_sethsv_noeeprom(180, 255, v);
        break;
      case _NUMPAD:
        rgblight_sethsv_noeeprom(60, 255, v);
        // Ensure numlock is on so we can actually use the number keys
        if (!(host_keyboard_leds() & (1<<USB_LED_NUM_LOCK))) {
          register_code(KC_NUMLOCK);
          unregister_code(KC_NUMLOCK);
        }
        break;
      case _RGB:
        rgblight_sethsv_noeeprom(300, 255, v);
        break;
      case _GAMING:
        rgblight_sethsv_noeeprom(120, 85, v);
        break;
    }

    old_layer = new_layer;
  }

  return state;
}

bool is_ctrl_tabbing = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case RGB_HUI:
    case RGB_HUD:
    case RGB_SAI:
    case RGB_SAD:
    case RGB_VAI:
    case RGB_VAD: {
      if (record->event.pressed) {
        uint8_t new_layer = biton32(layer_state);
        if (new_layer == _RGB && !is_in_rgb_mode) {
          is_in_rgb_mode = true;
          rgblight_sethsv_noeeprom(h, s, v);
        }
      }
      return true;
    }

    case SHIFT_TAB: {
      const uint8_t is_ctrl = (get_mods() & (MOD_BIT(KC_LCTL) | MOD_BIT(KC_RCTL)));
      const uint8_t is_shift = (get_mods() & (MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT)));

      if (record->event.pressed) {
        if (is_ctrl) {
          // CTRL + Tab gets converted to SHIFT + Tab
          is_ctrl_tabbing = true;
          unregister_code(KC_LCTRL);
          register_code(KC_LSFT);
          register_code(KC_TAB);
        } else {
          register_code(KC_TAB);
        }
      } else {
        if (is_ctrl_tabbing) {
          // Turn off the SHIFT + Tab
          is_ctrl_tabbing = false;
          unregister_code(KC_LSFT);
          unregister_code(KC_TAB);
          if (is_shift) {
            // Re-register CTRL if the mod button was still held down
            register_code(KC_LCTRL);
          }
        } else {
          unregister_code(KC_TAB);
        }
      }

      return false;
    }

    case TD(TD_NUM):
    case LT(4,KC_F11): {
      const uint8_t is_ctrl = (get_mods() & (MOD_BIT(KC_LCTL) | MOD_BIT(KC_RCTL)));
      if (record->event.pressed) {
        if (is_ctrl) {
          // Redo
          register_code(KC_Y);
          return false;
        }
      } else {
        if (is_ctrl) {
          // Redo
          unregister_code(KC_Y);
          return false;
        }
      }
    }
  }

  return true;
}

int cur_dance (qk_tap_dance_state_t *state) {
  if (state->count == 1) {
    if (!state->pressed)  return SINGLE_TAP;
    else return SINGLE_HOLD;
  }
  else if (state->count == 2) {
    if (state->interrupted) return DOUBLE_SINGLE_TAP;
    else if (state->pressed) return DOUBLE_HOLD;
    else return DOUBLE_TAP;
  }
  else return 8;
}

void tapdance_oneach (qk_tap_dance_state_t *state, void *user_data) {
  int id = (int)user_data;
  tri_layer_action action = tri_layer_actions[id];

  if (action.use_instant_layer && state->pressed) {
    layer_on(action.single_hold);
  }
}

void tapdance_onfinished (qk_tap_dance_state_t *state, void *user_data) {
  int id = (int)user_data;
  tap_states[id].state = cur_dance(state);
  tri_layer_action action = tri_layer_actions[id];

  switch (tap_states[id].state) {
    case SINGLE_TAP: register_code(action.single_tap); break;
    case SINGLE_HOLD: layer_on(action.single_hold); break;
    case DOUBLE_TAP: {
      if (action.is_double_tap_layer_lock) {
        layer_on(action.double_tap);
      } else {
        register_code(action.double_tap);
      }
      break;
    }
  }
}

void tapdance_onreset (qk_tap_dance_state_t *state, void *user_data) {
  int id = (int)user_data;
  tri_layer_action action = tri_layer_actions[id];

  switch (tap_states[id].state) {
    case SINGLE_TAP: unregister_code(action.single_tap); break;
    case SINGLE_HOLD: layer_off(action.single_hold); break;
    case DOUBLE_TAP: {
      if (action.is_double_tap_layer_lock) {
      } else {
        unregister_code(action.double_tap);
      }
      break;
    }
  }

  if (action.use_instant_layer) {
    layer_off(action.single_hold);
  }

  tap_states[id].state = 0;
}
