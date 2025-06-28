#pragma once

typedef enum eReturnCode {
    RET_OK = 0,
    RET_ERROR = -1,
    RET_NOT_FOUND = -2,
    RET_INVALID_ARGUMENT = -3,
    RET_PERMISSION_DENIED = -4,
    RET_TIMEOUT = -5,
    RET_ALREADY_EXISTS = -6,
    RET_UNSUPPORTED_OPERATION = -7
} ReturnCode;

