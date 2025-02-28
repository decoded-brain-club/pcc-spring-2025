#pragma once
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <mutex>
#include <vector>
#include <memory>
#include <string_view>
#include <chrono>
#include <iostream>
#include <fstream>
#include <functional>
#include <array>
#include <type_traits>

enum class LogLevel : uint8_t { Trace, Debug, Info, Warn, Error, Fatal };

constexpr std::string_view level_to_string(LogLevel level) {
    static constexpr std::array<std::string_view, 6> level_strings = {
        "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
    };
    const auto index = static_cast<std::underlying_type_t<LogLevel>>(level);
    return level_strings[index];
}

struct Sink {
    std::move_only_function<void(std::string_view)> writer;
    std::string format;
    LogLevel level;

    template<typename T>
    Sink(T&& impl, std::string fmt, LogLevel lvl)
        : format(std::move(fmt)),
          level(lvl),
          writer([impl = std::forward<T>(impl)](std::string_view msg) mutable {
              impl.write(msg);
          }) {}
};

class ConsoleSink {
public:
    void write(std::string_view msg) {
        fmt::println("{}", msg);
    }
};

class FileSink {
public:
    explicit FileSink(std::string_view filename)
        : file_(filename.data(), std::ios::app) {}

    void write(std::string_view msg) {
        file_ << msg << '\n';
    }

private:
    std::ofstream file_;
};

class Logger {
public:
    void set_level(LogLevel level) { min_level_ = level; }

    template<typename T>
    void add_sink(T&& sink, std::string fmt, LogLevel lvl) {
        std::lock_guard lock(mutex_);
        sinks_.emplace_back(std::forward<T>(sink), std::move(fmt), lvl);
    }

    template<typename... Args>
    void log(LogLevel lvl, std::string_view fmt, Args&&... args) {
        if (lvl < min_level_) return;

        const auto msg = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
        const auto now = std::chrono::system_clock::now();

        std::lock_guard lock(mutex_);
        for (auto& sink : sinks_) {
            if (lvl >= sink.level) {
                // try {
                    auto formatted = fmt::format(
                        fmt::runtime(sink.format),
                        fmt::arg("timestamp", now),
                        fmt::arg("level", level_to_string(lvl)),
                        fmt::arg("message", msg)
                    );
                    sink.writer(formatted);
                //} catch (...) {}
            }
        }
    }

    template<typename... Args> void trace(std::string_view fmt, Args&&... args) { log(LogLevel::Trace, fmt, std::forward<Args>(args)...); }
    template<typename... Args> void debug(std::string_view fmt, Args&&... args) { log(LogLevel::Debug, fmt, std::forward<Args>(args)...); }
    template<typename... Args> void info(std::string_view fmt, Args&&... args) { log(LogLevel::Info, fmt, std::forward<Args>(args)...); }
    template<typename... Args> void warn(std::string_view fmt, Args&&... args) { log(LogLevel::Warn, fmt, std::forward<Args>(args)...); }
    template<typename... Args> void error(std::string_view fmt, Args&&... args) { log(LogLevel::Error, fmt, std::forward<Args>(args)...); }
    template<typename... Args> void fatal(std::string_view fmt, Args&&... args) { log(LogLevel::Fatal, fmt, std::forward<Args>(args)...); }

private:
    std::vector<Sink> sinks_;
    std::mutex mutex_;
    LogLevel min_level_ = LogLevel::Info;
};

inline Logger g_logger;