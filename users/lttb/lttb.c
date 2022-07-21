#include QMK_KEYBOARD_H

#include "../callum/swapper.h"
#include "./features/achordion.h"
#include "./features/select_word.h"

#include "lttb.h"

enum combo_events {
  MOUSE_KEYS_COMBO,
  SFT_COMBO_L,
  SFT_COMBO_R,
  CMD_COMBO_L,
  CMD_COMBO_R,
  SFT_CMD_COMBO_L,
  SFT_CMD_COMBO_R,
  COMBO_LENGTH
};
uint16_t COMBO_LEN = COMBO_LENGTH;

const uint16_t PROGMEM mouse_keys_combo[] = {KC_S, KC_F,  COMBO_END};
const uint16_t PROGMEM sft_combo_l[] = {KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM sft_combo_r[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM cmd_combo_l[] = {KC_S, KC_D, COMBO_END};
const uint16_t PROGMEM cmd_combo_r[] = {KC_K, KC_L, COMBO_END};
const uint16_t PROGMEM sft_cmd_combo_l[] = {KC_S, KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM sft_cmd_combo_r[] = {KC_J, KC_K, KC_L, COMBO_END};

combo_t key_combos[] = {
    [MOUSE_KEYS_COMBO] = COMBO(mouse_keys_combo, TG(1)),
    [SFT_COMBO_L] = COMBO(sft_combo_l, OSM(MOD_LSFT)),
    [SFT_COMBO_R] = COMBO(sft_combo_r, OSM(MOD_LSFT)),
    [CMD_COMBO_L] = COMBO(cmd_combo_l, OSM(MOD_LGUI)),
    [CMD_COMBO_R] = COMBO(cmd_combo_r, OSM(MOD_LGUI)),
    [SFT_CMD_COMBO_L] = COMBO(sft_cmd_combo_l, OSM(MOD_LGUI | MOD_LSFT)),
    [SFT_CMD_COMBO_R] = COMBO(sft_cmd_combo_r, OSM(MOD_LGUI | MOD_LSFT)),
};

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case OS_CTL:
           return 500;
        case OS_ALT:
           return 500;
        case OS_GUI:
           return 500;
        case OS_SFT:
           return 500;
        default:
            return TAPPING_TERM;
    }
}

bool sw_appl_active = false;
bool sw_wind_active = false;
bool sw_lang_active = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_select_word(keycode, record, UK_SELWRD)) { return false; }

    update_swapper(
        &sw_appl_active, KC_LGUI, KC_TAB, SW_APPL,
        keycode, record
    );
    update_swapper(
        &sw_wind_active, KC_LGUI, KC_GRV, SW_WIND,
        keycode, record
    );
    update_swapper(
        &sw_lang_active, KC_LGUI, KC_SPC, SW_LANG,
        keycode, record
    );

    switch (keycode) {
        case SW_APPL:
        case SW_WIND:
        case SW_LANG:
            return true;

        case OPT_T(MT_COLN):
            if (record->tap.count && record->event.pressed) {
                tap_code16(KC_COLN);
                return false;
            }
            break;
        case SFT_T(MT_LPRN):
            if (record->tap.count && record->event.pressed) {
                tap_code16(KC_LPRN);
                return false;
            }
            break;
        case CMD_T(MT_RPRN):
            if (record->tap.count && record->event.pressed) {
                tap_code16(KC_RPRN);
                return false;
            }
            break;
    }

    if (!process_achordion(keycode, record)) { return false; }

    return true;
}

bool get_tapping_force_hold(uint16_t keycode, keyrecord_t* record) {
  // If you quickly hold a tap-hold key after tapping it, the tap action is
  // repeated. Key repeating is useful e.g. for Vim navigation keys, but can
  // lead to missed triggers in fast typing. Here, returning true means we
  // instead want to "force hold" and disable key repeating.
  switch (keycode) {
    case SFT_T(KC_J):
    case CMD_T(KC_K):
    case CMD_T(KC_D):
        return false;  // Enable key repeating.
    default:
        return true;  // Otherwise, force hold and disable key repeating.
  }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, _SYM, _NAV, _NUM);
}

void matrix_scan_user(void) {
  achordion_task();
}

bool achordion_chord(
    uint16_t tap_hold_keycode,
    keyrecord_t* tap_hold_record,
    uint16_t other_keycode,
    keyrecord_t* other_record
) {
    switch (tap_hold_keycode) {
        case LT(_NAV, KC_SPC):
        case LT(_SYM, KC_ENT):
            return true;
    }

    return achordion_opposite_hands(tap_hold_record, other_record);
}

uint16_t achordion_timeout(uint16_t tap_hold_keycode) {
  switch (tap_hold_keycode) {
        case LT(_NAV, KC_SPC):
        case LT(_SYM, KC_ENT):
            return 0;  // Bypass Achordion for these keys.
  }

  return 800;  // Otherwise use a timeout of 800 ms.
}

// bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
//     switch (keycode) {
//         // case LT(_NAV, KC_SPC):
//         case LT(_SYM, KC_ENT):
//             // Immediately select the hold action when another key is pressed.
//             return true;
//         default:
//             // Do not select the hold action when another key is pressed.
//             return false;
//     }
// }

bool get_ignore_mod_tap_interrupt(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(_SYM, KC_ENT):
            return false;
        default:
            return true;
    }
}
