/**
 * @file utils.c
 * @author Modar Nasser
 * @brief Utilities functions
 * @date 2020-11-11
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <errno.h>
#include <limits.h>
#include "utils.h"

int isUInt(char* str) {
    errno = 0;
    char* endptr;
    strtoul(str, &endptr, 10);
    return !((str == endptr) || (errno == ERANGE) || (*endptr != '\0'));
}
