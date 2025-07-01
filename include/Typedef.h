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
} eReturnCode_t;

typedef enum eUserStatus {
    USER_STATUS_DOES_NOT_EXIST = -1,    // User does not exist - File does not exist, account not created
    USER_STATUS_ACTIVE = 0,       // User is logged in and active - Logged in and session exists
    USER_STATUS_INACTIVE = 1,     // User is not logged in - File exists but not loaded into memory
    USER_STATUS_BANNED = 2,       // User is banned - No access to the system
    USER_STATUS_PENDING = 3       // User is pending - Account created but not activated
} eUserStatus_t;

typedef enum eRangeType {
    RANGE_NULL = -1,            // Invalid range type - One of more objects cannot be found
    RANGE_SAME_ROOM = 0,        // Both objects are in the same room
    RANGE_ADJACENT_ROOM = 1,    // Objects are in adjacent rooms (connected by a door or passage)
    RANGE_SAME_AREA = 2,        // Objects are in the same area
    RANGE_SAME_REGION = 3,      // Objects are in the same region
    RANGE_GLOBAL = 4            // Objects are in different regions (global range)
} eRangeType_t;