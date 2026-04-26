#pragma once

// ============================================================================
//  Prerequisitos básicos de logging para el motor / proyecto
//  - Logging a OutputDebugStringW
//  - Niveles de log (Info, Warning, Error, Debug)
//  - Opción de escribir también a archivo
//  - Macros seguros para integrarlo fácilmente en el código
// ============================================================================

// Optimiza la inclusión de <windows.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

// Librerías del sistema
#include <windows.h>

// Librerías STD
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
    // ------------------------------------------------------------
    //  Enumeración de niveles de log
    // ------------------------------------------------------------
    enum class Level
    {
        Debug,
        Info,
        Warning,
        Error
    };

    // ------------------------------------------------------------
    //  Flags y recursos internos (header-only, con singletons estáticos)
    // ------------------------------------------------------------
    inline bool& FileLoggingEnabled() noexcept
    {
        static bool enabled = false;
        return enabled;
    }

    inline std::wofstream& LogFileStream() noexcept
    {
        static std::wofstream file;
        return file;
    }

    inline std::mutex& LogMutex() noexcept
    {
        static std::mutex m;
        return m;
    }

    // ------------------------------------------------------------
    //  Configuración de logging
    // ------------------------------------------------------------

    /**
     * @brief Habilita o deshabilita el logging a archivo.
     *
     * @param enabled true para habilitar, false para solo usar OutputDebugStringW.
     */
    inline void EnableFileLogging(bool enabled) noexcept
    {
        FileLoggingEnabled() = enabled;
    }

    /**
     * @brief Establece la ruta del archivo de log.
     *        Si ya había un archivo abierto, se cierra y se abre el nuevo.
     *
     * @param path Ruta del archivo (wide string).
     */
    inline void SetLogFilePath(const std::wstring& path) noexcept
    {
        std::lock_guard<std::mutex> lock(LogMutex());

        std::wofstream& file = LogFileStream();
        if (file.is_open())
        {
            file.close();
        }

        if (!path.empty())
        {
            file.open(path, std::ios::out | std::ios::app);
            if (!file.is_open())
            {
                ::OutputDebugStringW(L"[EngineLog] Failed to open log file.\n");
                FileLoggingEnabled() = false;
            }
        }
    }

    /**
     * @brief Devuelve un prefijo de texto según el nivel de log.
     */
    inline const wchar_t* LevelToPrefix(Level level) noexcept
    {
        switch (level)
        {
        case Level::Debug:   return L"[DEBUG]";
        case Level::Info:    return L"[INFO]";
        case Level::Warning: return L"[WARN]";
        case Level::Error:   return L"[ERROR]";
        default:             return L"[LOG]";
        }
    }

    /**
     * @brief (Opcional) Genera una marca de tiempo simple [HH:MM:SS].
     */
    inline std::wstring MakeTimestamp() noexcept
    {
        using namespace std::chrono;

        auto now = system_clock::now();
        std::time_t t = system_clock::to_time_t(now);

        std::tm localTm{};
#ifdef _WIN32
        localtime_s(&localTm, &t);
#else
        localTm = *std::localtime(&t);
#endif

        wchar_t buffer[16];
        std::wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%H:%M:%S", &localTm);
        return std::wstring(L"[") + buffer + L"]";
    }

    /**
     * @brief Envía un mensaje al panel de Output de Visual Studio.
     */
    inline void DebugOutput(const std::wstring& msg) noexcept
    {
        ::OutputDebugStringW(msg.c_str());
    }

    /**
     * @brief Log general: escribe en OutputDebugStringW y, si está habilitado,
     *        también en archivo.
     *
     * @param level    Nivel de log.
     * @param source   Texto que identifica origen (por ejemplo "Renderer::Init").
     * @param message  Mensaje principal.
     */
    inline void Log(Level level,
        const std::wstring& source,
        const std::wstring& message) noexcept
    {
        try
        {
            std::lock_guard<std::mutex> lock(LogMutex());

            std::wostringstream os;
            os << MakeTimestamp() << L" "
                << LevelToPrefix(level) << L" "
                << source << L" : " << message << L"\n";

            const std::wstring finalMsg = os.str();

            // OutputDebugString
            DebugOutput(finalMsg);

            // Archivo (si está habilitado y el archivo está abierto)
            if (FileLoggingEnabled())
            {
                std::wofstream& file = LogFileStream();
                if (file.is_open())
                {
                    file << finalMsg;
                    file.flush();
                }
            }
        }
        catch (...)
        {
            ::OutputDebugStringW(L"[EngineLog] Failed to log message.\n");
        }
    }
}

// ============================================================================
//  MACROS DE LOGGING
//  - Se envuelven en do { } while(0) para evitar problemas en if/else
//  - Crean el mensaje con wostringstream y llaman a EngineLog::Log
// ============================================================================

#define AO_LOG_INTERNAL(levelEnum, classObj, method, msgExpr)                        \
    do {                                                                             \
        try {                                                                        \
            std::wostringstream os_;                                                 \
            os_ << msgExpr;                                                          \
            std::wostringstream source_;                                             \
            source_ << classObj << L"::" << method;                                  \
            ::EngineLog::Log(::EngineLog::Level::levelEnum,                          \
                             source_.str(),                                          \
                             os_.str());                                             \
        } catch (...) {                                                              \
            ::OutputDebugStringW(L"[EngineLog] Exception while building log.\n");    \
        }                                                                            \
    } while (0)

// Niveles de log públicos
#define LOG_DEBUG(classObj, method, msg)   AO_LOG_INTERNAL(Debug,   classObj, method, msg)
#define LOG_INFO(classObj, method, msg)    AO_LOG_INTERNAL(Info,    classObj, method, msg)
#define LOG_WARN(classObj, method, msg)    AO_LOG_INTERNAL(Warning, classObj, method, msg)
#define LOG_ERROR(classObj, method, msg)   AO_LOG_INTERNAL(Error,   classObj, method, msg)

// ---------------------------------------------------------------------------
//  Compatibilidad con tus macros anteriores
// ---------------------------------------------------------------------------
#define MESSAGE(classObj, method, state)                                           \
    LOG_INFO(classObj, method, L"[CREATION OF RESOURCE : " << state << L"]")

#define ERROR_OLD(classObj, method, errorMSG)                                      \
    LOG_ERROR(classObj, method, errorMSG)

// Si quieres, puedes reemplazar tus usos de ERROR(...) por LOG_ERROR(...)
// y solo mantener ERROR_OLD como alias temporal.
// ---------------------------------------------------------------------------
