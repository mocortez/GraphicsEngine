#pragma once

/**
 * ============================================================================
 * Prerrequisitos y Sistema de Logging Optimizado
 * Diseñado para alto rendimiento en Render Loops.
 * ============================================================================
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

 // Librerías del sistema
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <cmath>

namespace EngineLog
{
    enum class Level { Debug, Info, Warning, Error };

    // Recursos internos con singletons estáticos
    inline bool& FileLoggingEnabled() noexcept {
        static bool enabled = false;
        return enabled;
    }

    inline std::wofstream& LogFileStream() noexcept {
        static std::wofstream file;
        return file;
    }

    inline std::mutex& LogMutex() noexcept {
        static std::mutex m;
        return m;
    }

    inline void EnableFileLogging(bool enabled) noexcept {
        FileLoggingEnabled() = enabled;
    }

    inline void SetLogFilePath(const std::wstring& path) noexcept {
        std::lock_guard<std::mutex> lock(LogMutex());
        std::wofstream& file = LogFileStream();
        if (file.is_open()) file.close();

        if (!path.empty()) {
            file.open(path, std::ios::out | std::ios::app);
            if (!file.is_open()) {
                ::OutputDebugStringW(L"[EngineLog] Failed to open log file.\n");
                FileLoggingEnabled() = false;
            }
        }
    }

    inline const wchar_t* LevelToPrefix(Level level) noexcept {
        switch (level) {
        case Level::Debug:   return L"[DEBUG]";
        case Level::Info:    return L"[INFO]";
        case Level::Warning: return L"[WARN]";
        case Level::Error:   return L"[ERROR]";
        default:             return L"[LOG]";
        }
    }

    inline std::wstring MakeTimestamp() noexcept {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm localTm{};
        localtime_s(&localTm, &t);
        wchar_t buffer[16];
        std::wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%H:%M:%S", &localTm);
        return std::wstring(L"[") + buffer + L"]";
    }

    inline void Log(Level level, const std::wstring& source, const std::wstring& message) noexcept {
        try {
            std::lock_guard<std::mutex> lock(LogMutex());
            std::wostringstream os;
            os << MakeTimestamp() << L" " << LevelToPrefix(level) << L" [" << source << L"] : " << message << L"\n";
            const std::wstring finalMsg = os.str();

            ::OutputDebugStringW(finalMsg.c_str());

            if (FileLoggingEnabled()) {
                std::wofstream& file = LogFileStream();
                if (file.is_open()) {
                    file << finalMsg;
                    file.flush();
                }
            }
        }
        catch (...) {}
    }
}

// ============================================================================
//  MACROS DE LOGGING OPTIMIZADAS
// ============================================================================

#define AO_LOG_INTERNAL(levelEnum, classObj, method, msgExpr) \
    do { \
        std::wostringstream os_; \
        os_ << msgExpr; \
        std::wostringstream source_; \
        source_ << classObj << L"::" << method; \
        ::EngineLog::Log(::EngineLog::Level::levelEnum, source_.str(), os_.str()); \
    } while (0)

// En modo Release, el LOG_DEBUG se elimina completamente del ejecutable
#ifdef _DEBUG
#define LOG_DEBUG(classObj, method, msg) AO_LOG_INTERNAL(Debug, classObj, method, msg)
#else
#define LOG_DEBUG(classObj, method, msg) ((void)0)
#endif

#define LOG_INFO(classObj, method, msg)    AO_LOG_INTERNAL(Info,    classObj, method, msg)
#define LOG_WARN(classObj, method, msg)    AO_LOG_INTERNAL(Warning, classObj, method, msg)
#define LOG_ERROR(classObj, method, msg)   AO_LOG_INTERNAL(Error,   classObj, method, msg)

// Compatibilidad
#define MESSAGE(classObj, method, state)   LOG_INFO(classObj, method, L"RESOURCE_STATE: " << state)
#define ERROR_OLD(classObj, method, error) LOG_ERROR(classObj, method, error)
