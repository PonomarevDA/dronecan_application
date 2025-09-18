/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef APPLICATION_PARAMS_H_
#define APPLICATION_PARAMS_H_

#include <stdint.h>

#ifndef MAX_PARAM_NAME_LENGTH
#define MAX_PARAM_NAME_LENGTH 32
#endif

#ifndef MAX_STRING_LENGTH
#define MAX_STRING_LENGTH 56
#endif
static_assert(MAX_STRING_LENGTH % 8 == 0, "String size must be a multiple of 8");


typedef int32_t IntegerParamValue_t;
typedef uint16_t ParamIndex_t;
typedef uint8_t StringParamValue_t[MAX_STRING_LENGTH];

/* --- Function pointer typedefs --- */
typedef const char *(*ParamsGetNameFn)(ParamIndex_t param_idx);
typedef bool (*ParamIsIntegerFn)(ParamIndex_t param_idx);
typedef bool (*ParamIsStringFn)(ParamIndex_t param_idx);
typedef ParamIndex_t (*ParamsFindFn)(const uint8_t *name, uint16_t len);

typedef void (*ParamsSetIntegerValueFn)(ParamIndex_t param_idx,
                                        IntegerParamValue_t param_value);
typedef IntegerParamValue_t (*ParamsGetIntegerValueFn)(ParamIndex_t param_idx);

typedef IntegerParamValue_t (*ParamsGetIntegerMinFn)(ParamIndex_t param_idx);
typedef IntegerParamValue_t (*ParamsGetIntegerMaxFn)(ParamIndex_t param_idx);
typedef IntegerParamValue_t (*ParamsGetIntegerDefFn)(ParamIndex_t param_idx);

typedef uint8_t (*ParamsSetStringValueFn)(ParamIndex_t param_idx,
                                          uint8_t str_len,
                                          const StringParamValue_t param_value);
typedef StringParamValue_t *(*ParamsGetStringValueFn)(ParamIndex_t param_idx);

typedef int8_t (*ParamsSaveFn)(void);
typedef int8_t (*ParamsResetToDefaultFn)(void);

typedef struct {
    ParamsSetIntegerValueFn   setValue;
    ParamsGetIntegerValueFn   getValue;
    ParamsGetIntegerMinFn     getMin;
    ParamsGetIntegerMaxFn     getMax;
    ParamsGetIntegerDefFn     getDef;
} IntegerApi;

typedef struct {
    ParamsSetStringValueFn    setValue;
    ParamsGetStringValueFn    getValue;
} StringApi;

/* --- Struct grouping the API --- */
typedef struct {
    ParamsGetNameFn           getName;
    ParamIsIntegerFn          isInteger;
    ParamIsStringFn           isString;
    ParamsFindFn              find;

    IntegerApi                integer;
    StringApi                 string;

    ParamsSaveFn              save;
    ParamsResetToDefaultFn    resetToDefault;
} ParamsApi;

#endif  // APPLICATION_PARAMS_H_
