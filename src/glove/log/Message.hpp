#pragma once

#include <unordered_map>
#include <thread>
#include <chrono>

#include "glove/BlueDwarfApi.hpp"
#include "glove/CommonTypes.hpp"

namespace BlueDwarf {
enum class LogLevel:int { Error = 0, Warning = 1, Info = 2, Debug = 3, Trace = 4};
std::ostream& operator<<(std::ostream& stream, const LogLevel& logLevel);

struct BD_API_EXPORT Message {
  public:
    Message(LogLevel logLevel, const std::string& message, bool isExceptionMessage)
        : logLevel(logLevel), message(message), isExceptionMessage(isExceptionMessage) {}
    Message(LogLevel logLevel, const std::string& message) : Message(logLevel, message, false) {}
    Message(LogLevel logLevel, const std::exception& exception) : Message(logLevel, exception.what(), true) {}

    LogLevel GetLogLevel() const { return logLevel; }
    const std::string& GetMessage() const { return message; }
    std::chrono::high_resolution_clock::time_point GetTimestamp() const { return timestamp; }
    bool IsExceptionMessage() const { return isExceptionMessage; }
    std::thread::id GetThreadId() const { return threadId; }

    const std::string& GetSource() const { return source; }
    void SetSource(const std::string& source) const { this->source = source; }

    const std::string& GetText() const { return text.empty() ? message : text; }
    void SetText(const std::string& text) const { this->text = text; }

    void SetProperty(const std::string& key, std::string value) const { customProperties[std::hash<std::string>()(key)] = value; }
    const std::string& GetProperty(std::string key) const { return customProperties[std::hash<std::string>()(key)]; }

  private:
    LogLevel logLevel;
    std::string message;
    std::chrono::high_resolution_clock::time_point timestamp = std::chrono::high_resolution_clock::now();
    bool isExceptionMessage = false;
    std::thread::id threadId = std::this_thread::get_id();

    mutable std::string source;
    mutable std::string text;

    mutable std::unordered_map<std::size_t, std::string> customProperties;
};
}