#include QMK_KEYBOARD_H

#define __ _______

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP, // Send two single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

// Tap dance enums
enum {
    TD_GRAVE_ESC,
    SOME_OTHER_DANCE
};

td_state_t cur_dance(tap_dance_state_t *state);

// For the x tap dance. Put it here so it can be used in any keymap
void x_finished(tap_dance_state_t *state, void *user_data);
void x_reset(tap_dance_state_t *state, void *user_data);


    /////////////////////////////
   /////      Keymaps      /////
  /////////////////////////////

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [0] = LAYOUT_5x6_5(
       KC_ESC ,    KC_1,    KC_2,    KC_3,   KC_4,  KC_5,     KC_6,  KC_7,  KC_8,    KC_9,   KC_0,     KC_RBRC,
       KC_TAB ,    KC_Q,    KC_W,    KC_E,   KC_R,  KC_T,     KC_Y,  KC_U,  KC_I,    KC_O,   KC_P,     KC_LBRC,
       KC_LCTL,    KC_A,    KC_S,    KC_D,   KC_F,  KC_G,     KC_H,  KC_J,  KC_K,    KC_L,   KC_SCLN, KC_QUOTE,
       KC_LSFT,    KC_Z,    KC_X,    KC_C,   KC_V,  KC_B,     KC_N,  KC_M,  KC_COMM, KC_DOT, KC_SLSH,  KC_RSFT,

           __ ,     __ ,   KC_LSFT , KC_LCTL , KC_LGUI,       KC_1 ,  KC_2,  KC_3,  KC_6,     KC_7 ,
                                     KC_LALT , MO(1)  ,       KC_4,  KC_5 ),

  [1] = LAYOUT_5x6_5(
            __,   KC_F1,   KC_F2,   KC_F3,  KC_F4,  KC_F5,     KC_F6,   KC_F7,    KC_F8,    KC_F9,   KC_F10,  __,
            __,      __,    KC_7,    KC_8,   KC_9, KC_PSCR,    KC_HOME, KC_PGUP,  KC_UP,    KC_PGDN, KC_END,  __,
            __,    KC_0,    KC_4,    KC_5,   KC_6, KC_DEL,     KC_BSPC, KC_LEFT,  KC_ENTER, KC_RGHT, __,      __,
            __,    KC_0,    KC_1,    KC_2,   KC_3,     __,     __,      KC_TAB,   KC_DOWN,  __,      __,      __,

       QK_BOOT,     __ ,       __  ,      __  ,    __  ,       __      ,    __  ,    __,         KC_F11,  KC_F12,
                                          __  ,    __  ,       __      ,    __ ),
 };



    /////////////////////////////
   /////    Tap Dances     /////
  /////////////////////////////

td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (!state->pressed) return TD_SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        // TD_DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
        // action when hitting 'pp'. Suggested use case for this return value is when you want to send two
        // keystrokes of the key, and not the 'double tap' action/macro.
        if (state->interrupted) return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed) return TD_DOUBLE_HOLD;
        else return TD_DOUBLE_TAP;
    }

    // Assumes no one is trying to type the same letter three times (at least not quickly).
    // If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add
    // an exception here to return a 'TD_TRIPLE_SINGLE_TAP', and define that enum just like 'TD_DOUBLE_SINGLE_TAP'
    if (state->count == 3) {
        if (!state->pressed) return TD_TRIPLE_TAP;
        else return TD_TRIPLE_HOLD;
    } else return TD_UNKNOWN;
}


////
/// Esc on tap, Grave on hold
//

static td_tap_t grave_esc_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};
void grave_esc_finished(tap_dance_state_t *state, void *user_data) {
    grave_esc_tap_state.state = cur_dance(state);
    switch (grave_esc_tap_state.state) {
        case TD_SINGLE_TAP: register_code(KC_ESC); break;
        case TD_SINGLE_HOLD: register_code(KC_GRV); break;
        case TD_DOUBLE_TAP: register_code(KC_ESC); break;
        // Last case is for fast typing. Assuming your key is `f`:
        // For example, when typing the word `buffer`, and you want to make sure that you send `ff` and not `Esc`.
        // In order to type `ff` when typing fast, the next character will have to be hit within the `TAPPING_TERM`, which by default is 200ms.
        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_ESC); register_code(KC_ESC); break;
        default: break;
    }
}

void grave_esc_reset(tap_dance_state_t *state, void *user_data) {
    switch (grave_esc_tap_state.state) {
        case TD_SINGLE_TAP: unregister_code(KC_ESC); break;
        case TD_SINGLE_HOLD: unregister_code(KC_GRV); break;
        case TD_DOUBLE_TAP: unregister_code(KC_ESC); break;
        case TD_DOUBLE_SINGLE_TAP: unregister_code(KC_ESC); break;
        default: break;
    }
    grave_esc_tap_state.state = TD_NONE;
}


////
/// Tab on tap, Layer 2 on hold
//

static td_tap_t tab_layer_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};
void tab_layer_finished(tap_dance_state_t *state, void *user_data) {
    tab_layer_tap_state.state = cur_dance(state);
    switch (tab_layer_tap_state.state) {
            case TD_SINGLE_TAP: register_code(KC_TAB); break;
        case TD_SINGLE_HOLD: layer_on(2); break;
        case TD_DOUBLE_TAP: register_code(KC_TAB); break;
        // Last case is for fast typing. Assuming your key is `f`:
        // For example, when typing the word `buffer`, and you want to make sure that you send `ff` and not `Esc`.
        // In order to type `ff` when typing fast, the next character will have to be hit within the `TAPPING_TERM`, which by default is 200ms.
        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_TAB); register_code(KC_TAB); break;
        default: break;
    }
}

void tab_layer_reset(tap_dance_state_t *state, void *user_data) {
    switch (tab_layer_tap_state.state) {
        case TD_SINGLE_TAP: unregister_code(KC_TAB); break;
        case TD_SINGLE_HOLD: layer_off(2);; break;
        case TD_DOUBLE_TAP: unregister_code(KC_TAB); break;
        case TD_DOUBLE_SINGLE_TAP: unregister_code(KC_TAB); break;
        default: break;
    }
    tab_layer_tap_state.state = TD_NONE;
}


////
/// Win on tap, Alt on hold
//

static td_tap_t win_alt_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};
void win_alt_finished(tap_dance_state_t *state, void *user_data) {
    win_alt_tap_state.state = cur_dance(state);
    switch (win_alt_tap_state.state) {
        case TD_SINGLE_TAP: register_code(KC_LGUI); break;
        case TD_SINGLE_HOLD: register_code(KC_LALT); break;
        case TD_DOUBLE_TAP: register_code(KC_LGUI); break;
        // Last case is for fast typing. Assuming your key is `f`:
        // For example, when typing the word `buffer`, and you want to make sure that you send `ff` and not `Esc`.
        // In order to type `ff` when typing fast, the next character will have to be hit within the `TAPPING_TERM`, which by default is 200ms.
        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_LGUI); register_code(KC_LGUI); break;
        default: break;
    }
}

void win_alt_reset(tap_dance_state_t *state, void *user_data) {
    switch (win_alt_tap_state.state) {
        case TD_SINGLE_TAP: unregister_code(KC_LGUI); break;
        case TD_SINGLE_HOLD: unregister_code(KC_LALT); break;
        case TD_DOUBLE_TAP: unregister_code(KC_LGUI); break;
        case TD_DOUBLE_SINGLE_TAP: unregister_code(KC_LGUI); break;
        default: break;
    }
    win_alt_tap_state.state = TD_NONE;
}


////
/// Space on tap, Shift on hold
//

static td_tap_t space_shift_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};
void space_shift_finished(tap_dance_state_t *state, void *user_data) {
    space_shift_tap_state.state = cur_dance(state);
    switch (space_shift_tap_state.state) {
        case TD_SINGLE_TAP: register_code(KC_SPACE); break;
        case TD_SINGLE_HOLD: register_code(KC_LEFT_SHIFT); break;
        case TD_DOUBLE_TAP: register_code(KC_SPACE); break;
        // Last case is for fast typing. Assuming your key is `f`:
        // For example, when typing the word `buffer`, and you want to make sure that you send `ff` and not `Esc`.
        // In order to type `ff` when typing fast, the next character will have to be hit within the `TAPPING_TERM`, which by default is 200ms.
        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_SPACE); register_code(KC_SPACE); break;
        default: break;
    }
}

void space_shift_reset(tap_dance_state_t *state, void *user_data) {
    switch (space_shift_tap_state.state) {
        case TD_SINGLE_TAP: unregister_code(KC_SPACE); break;
        case TD_SINGLE_HOLD: unregister_code(KC_LEFT_SHIFT); break;
        case TD_DOUBLE_TAP: unregister_code(KC_SPACE); break;
        case TD_DOUBLE_SINGLE_TAP: unregister_code(KC_SPACE); break;
        default: break;
    }
    space_shift_tap_state.state = TD_NONE;
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_GRAVE_ESC] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, grave_esc_finished, grave_esc_reset),
    [TD_TAB_LAYER] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, tab_layer_finished, tab_layer_reset),
    [TD_WIN_ALT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, win_alt_finished, win_alt_reset),
    [TD_SPACE_SHIFT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, space_shift_finished, space_shift_reset),

};
