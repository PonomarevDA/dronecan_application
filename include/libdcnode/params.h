/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBDCNODE_PARAMS_H_
#define LIBDCNODE_PARAMS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#ifndef MAX_STRING_LENGTH
#define MAX_STRING_LENGTH 56
#endif
static_assert(MAX_STRING_LENGTH % 8 == 0, "String size must be a multiple of 8");


typedef int32_t IntegerParamValue_t;
typedef uint16_t ParamIndex_t;
typedef uint8_t StringParamValue_t[MAX_STRING_LENGTH];

/* Core metadata */
typedef const char*         (*ParamsGetNameFn)(ParamIndex_t param_idx);
typedef bool                (*ParamsIsIntegerFn)(ParamIndex_t param_idx);
typedef bool                (*ParamsIsStringFn)(ParamIndex_t param_idx);
typedef ParamIndex_t        (*ParamsFindFn)(const uint8_t *name, uint16_t len);

/* Persistence */
typedef int8_t              (*ParamsSaveFn)(void);
typedef int8_t              (*ParamsResetToDefaultFn)(void);

/* Integer API */
typedef void                (*ParamsSetIntegerValueFn)(ParamIndex_t param_idx,
                                                       IntegerParamValue_t param_value);
typedef IntegerParamValue_t (*ParamsGetIntegerValueFn)(ParamIndex_t param_idx);
typedef IntegerParamValue_t (*ParamsGetIntegerMinFn)(ParamIndex_t param_idx);
typedef IntegerParamValue_t (*ParamsGetIntegerMaxFn)(ParamIndex_t param_idx);
typedef IntegerParamValue_t (*ParamsGetIntegerDefFn)(ParamIndex_t param_idx);

/* String API */
typedef uint8_t             (*ParamsSetStringValueFn)(ParamIndex_t param_idx,
                                                      uint8_t str_len,
                                                      const StringParamValue_t param_value);
typedef StringParamValue_t* (*ParamsGetStringValueFn)(ParamIndex_t param_idx);

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

typedef struct {
    ParamsGetNameFn           getName;
    ParamsIsIntegerFn         isInteger;
    ParamsIsStringFn          isString;
    ParamsFindFn              find;

    ParamsSaveFn              save;
    ParamsResetToDefaultFn    resetToDefault;

    IntegerApi                integer;
    StringApi                 string;
} ParamsApi;

#ifdef __cplusplus
} // extern "C"
#endif

#endif  // LIBDCNODE_PARAMS_H_
