#pragma once

#include "Typedef.h"

typedef enum class ManagerType {
    UserManager,
    SessionManager,
    MudServer
} ManagerType;

void* GetManager(ManagerType type);
eReturnCode ReleaseManager(ManagerType type, void* manager);
eReturnCode RegisterManager(ManagerType type, void* manager);
