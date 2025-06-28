#include "Manager.h"
#include "UserManager.h"
#include "SessionManager.h"

static std::unique_ptr<UserManager> m_userManager;
static std::unique_ptr<SessionManager> m_sessionManager;


void* GetManager(ManagerType type) {
    switch (type) {
        case ManagerType::UserManager:
            return m_userManager ? m_userManager.get() : nullptr;
        case ManagerType::SessionManager:
            return m_sessionManager ? m_sessionManager.get() : nullptr;
        case ManagerType::MudServer:
        default:
            return nullptr;
    }
}

eReturnCode ReleaseManager(ManagerType type, void* manager) {
    switch (type) {
        case ManagerType::UserManager:
            if (m_userManager && m_userManager.get() == manager) {
                m_userManager.reset();
                return eReturnCode::RET_OK;
            }
            break;
        case ManagerType::SessionManager:
            if (m_sessionManager && m_sessionManager.get() == manager) {
                m_sessionManager.reset();
                return eReturnCode::RET_OK;
            }
            break;
        case ManagerType::MudServer:
        default:
            return eReturnCode::RET_OK;
    }
    return eReturnCode::RET_NOT_FOUND;
}

eReturnCode RegisterManager(ManagerType type, void* manager) {
    switch (type) {
        case ManagerType::UserManager:
            if (!m_userManager) {
                m_userManager.reset(static_cast<UserManager*>(manager));
                return eReturnCode::RET_OK;
            }
            break;
        case ManagerType::SessionManager:
            if (!m_sessionManager) {
                m_sessionManager.reset(static_cast<SessionManager*>(manager));
                return eReturnCode::RET_OK;
            }
            break;
        case ManagerType::MudServer:
        default:
            return eReturnCode::RET_INVALID_ARGUMENT;
    }
    return eReturnCode::RET_ALREADY_EXISTS;
}
