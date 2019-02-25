// -------------------------------------------------
// BlinKit - stub Part
// -------------------------------------------------
//   File Name: uchar.h
// Description: ICU Stub
//      Author: Ziming Li
//     Created: 2019-02-14
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

#ifndef BLINKIT_STUB_ICU_UCHAR_H
#define BLINKIT_STUB_ICU_UCHAR_H

#pragma once

#include "utypes.h"

#define U_MASK(x) ((uint32_t)1<<(x))

typedef enum UCharCategory
{
    U_UNASSIGNED = 0,
    U_GENERAL_OTHER_TYPES = 0,
    U_UPPERCASE_LETTER = 1,
    U_LOWERCASE_LETTER = 2,
    U_TITLECASE_LETTER = 3,
    U_MODIFIER_LETTER = 4,
    U_OTHER_LETTER = 5,
    U_NON_SPACING_MARK = 6,
    U_ENCLOSING_MARK = 7,
    U_COMBINING_SPACING_MARK = 8,
    U_DECIMAL_DIGIT_NUMBER = 9,
    U_LETTER_NUMBER = 10,
    U_OTHER_NUMBER = 11,
    U_SPACE_SEPARATOR = 12,
    U_LINE_SEPARATOR = 13,
    U_PARAGRAPH_SEPARATOR = 14,
    U_CONTROL_CHAR = 15,
    U_FORMAT_CHAR = 16,
    U_PRIVATE_USE_CHAR = 17,
    U_SURROGATE = 18,
    U_DASH_PUNCTUATION = 19,
    U_START_PUNCTUATION = 20,
    U_END_PUNCTUATION = 21,
    U_CONNECTOR_PUNCTUATION = 22,
    U_OTHER_PUNCTUATION = 23,
    U_MATH_SYMBOL = 24,
    U_CURRENCY_SYMBOL = 25,
    U_MODIFIER_SYMBOL = 26,
    U_OTHER_SYMBOL = 27,
    U_INITIAL_PUNCTUATION = 28,
    U_FINAL_PUNCTUATION = 29,
    U_CHAR_CATEGORY_COUNT
} UCharCategory;

typedef enum UCharDirection {
    U_LEFT_TO_RIGHT = 0,
    U_RIGHT_TO_LEFT = 1,
    U_EUROPEAN_NUMBER = 2,
    U_EUROPEAN_NUMBER_SEPARATOR = 3,
    U_EUROPEAN_NUMBER_TERMINATOR = 4,
    U_ARABIC_NUMBER = 5,
    U_COMMON_NUMBER_SEPARATOR = 6,
    U_BLOCK_SEPARATOR = 7,
    U_SEGMENT_SEPARATOR = 8,
    U_WHITE_SPACE_NEUTRAL = 9,
    U_OTHER_NEUTRAL = 10,
    U_LEFT_TO_RIGHT_EMBEDDING = 11,
    U_LEFT_TO_RIGHT_OVERRIDE = 12,
    U_RIGHT_TO_LEFT_ARABIC = 13,
    U_RIGHT_TO_LEFT_EMBEDDING = 14,
    U_RIGHT_TO_LEFT_OVERRIDE = 15,
    U_POP_DIRECTIONAL_FORMAT = 16,
    U_DIR_NON_SPACING_MARK = 17,
    U_BOUNDARY_NEUTRAL = 18,
    U_FIRST_STRONG_ISOLATE = 19,
    U_LEFT_TO_RIGHT_ISOLATE = 20,
    U_RIGHT_TO_LEFT_ISOLATE = 21,
    U_POP_DIRECTIONAL_ISOLATE = 22,
    U_CHAR_DIRECTION_COUNT
} UCharDirection;

typedef enum UDecompositionType {
    U_DT_NONE,
    U_DT_CANONICAL,
    U_DT_COMPAT,
    U_DT_CIRCLE,
    U_DT_FINAL,
    U_DT_FONT,
    U_DT_FRACTION,
    U_DT_INITIAL,
    U_DT_ISOLATED,
    U_DT_MEDIAL,
    U_DT_NARROW,
    U_DT_NOBREAK,
    U_DT_SMALL,
    U_DT_SQUARE,
    U_DT_SUB,
    U_DT_SUPER,
    U_DT_VERTICAL,
    U_DT_WIDE,
    U_DT_COUNT
} UDecompositionType;

#endif // BLINKIT_STUB_ICU_UCHAR_H