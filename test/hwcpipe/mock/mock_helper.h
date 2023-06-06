/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

/**
 * @param RETURN_TYPE return type of mock function.
 * @param FUNC_NAME function name.
 * @param ARGS function arguments(function prototype).
 *
 * @par Example
 * @code
 * //To mock a function with the following prototype
 * //  void set_id (char* name, int id)
 * //MOCK() should be called as following
 *      MOCK(void, set_id, (char* name, int id));
 * @endcode
 */
#define MOCK(RETURN_TYPE, FUNC_NAME, ARGS)                                                                             \
    static RETURN_TYPE FUNC_NAME##_default_return_value;                                                               \
    static RETURN_TYPE FUNC_NAME##_return_value;                                                                       \
    RETURN_TYPE FUNC_NAME ARGS {                                                                                       \
        RETURN_TYPE ret_value = FUNC_NAME##_return_value;                                                              \
        FUNC_NAME##_return_value = FUNC_NAME##_default_return_value;                                                   \
        return ret_value;                                                                                              \
    }

/**
 * @param RETURN_TYPE return type of mock function.
 * @param CLASS_NAME Name of the class containing the mocked function.
 * @param FUNC_NAME function name.
 * @param DEFAULT_RET_VAL default value to return from function
 *                        if EXPECT_CALL() is not called.
 * @note this funciton must be called in a file scope but not in a
 *       class scope.
 */
#define MOCK_DEFAULT_RET(RETURN_TYPE, CLASS_NAME, FUNC_NAME, DEFAULT_RET_VAL)                                          \
    RETURN_TYPE CLASS_NAME::FUNC_NAME##_return_value = DEFAULT_RET_VAL;                                                \
    RETURN_TYPE CLASS_NAME::FUNC_NAME##_default_return_value = DEFAULT_RET_VAL;

/**
 * @param CLASS_NAME Name of the class containing the mocked function.
 * @param FUNC_NAME function Name
 * @param RETURN_VALUE Return value of expected call
 */
#define EXPECT_CALL(CLASS_NAME, FUNC_NAME, RETURN_VALUE) CLASS_NAME::FUNC_NAME##_return_value = RETURN_VALUE
