#pragma once

// #define Log(loglevel, format, ...) \
//   if(logverbose >= loglevel) \
//   printf("\33[1;35m[%s:%d,%s] " format "\33[0m\n", \
//       __FILE__, __LINE__, __func__, ## __VA_ARGS__)

// #undef panic
// #define panic(format, ...) \
//   do { \
//     Log("\33[1;31mpanic: " format, ## __VA_ARGS__); \
//     exit(1); \
//   } while (0)

#include <iostream>
#include <string>
#include <mutex>
#include <format>


namespace ssat{

enum class LogLevel {
  DETAIL = 0,
  DEBUG,
  INFO,
  WARN,
  ERROR,
  NONE
};



class Logger{
public :
  static Logger& getInstance(){
    static Logger instance;
    return instance;
  }
  void setLogLevel(LogLevel level) {
      std::lock_guard<std::mutex> lock(mutex_);
      currentLevel_ = level;
  }

  void setLogLevel(std::string level){
      if(level == "DETAIL"){
          setLogLevel(LogLevel::DETAIL);
      }else if(level == "DEBUG"){
          setLogLevel(LogLevel::DEBUG);
      }else if(level == "INFO"){
          setLogLevel(LogLevel::INFO);
      }else if(level == "WARN"){
          setLogLevel(LogLevel::WARN);
      }else if(level == "ERROR"){
          setLogLevel(LogLevel::ERROR);
      }else if(level == "NONE"){
          setLogLevel(LogLevel::NONE);
      }else{
          std::cerr<< "Invalid log level: " << level << std::endl;
      }
  }
  template <typename... Args>
  void log(LogLevel level, std::string_view format_str, Args&&... args) const {
      std::lock_guard<std::mutex> lock(mutex_);
      if (level < currentLevel_) {
          return;
      }
      std::string message = std::vformat(format_str, std::make_format_args(args...));
      if(level == LogLevel::ERROR || level == LogLevel::WARN){
          std::cerr << levelToString(level) << ": " << message << std::endl;
          return;
      }
      std::cout << levelToString(level) << ": " << message << std::endl;
  }


  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
private:
  Logger() : currentLevel_(LogLevel::DETAIL) {}
  LogLevel currentLevel_;
  mutable std::mutex mutex_;
  std::string levelToString(LogLevel level) const {
      switch(level) {
        case LogLevel::DETAIL: return "\33[1;35m[LOG:DETAIL]\33[0m";
        case LogLevel::DEBUG : return "\33[1;34m[LOG:DEBUG ]\33[0m";
        case LogLevel::INFO  : return "\33[1;34m[LOG:INFO  ]\33[0m";
        case LogLevel::WARN  : return "\33[1;34m[LOG:WARN  ]\33[0m";
        case LogLevel::ERROR : return "\33[1;34m[LOG:ERROR ]\33[0m";
        default:               return "";
      }
  }

};

template <typename... Args>
inline void Log(LogLevel level, std::string_view format_str, Args&&... args) {
    Logger::getInstance().log(level, format_str, std::forward<Args>(args)...);
}
#define LOG_DEBUG(fmt, ...) ssat::Log(ssat::LogLevel::DEBUG,   "\e[0;32m[{}:{},{}]\e[0m " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  ssat::Log(ssat::LogLevel::INFO,    "\e[0;32m[{}:{},{}]\e[0m " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  ssat::Log(ssat::LogLevel::WARN,    "\e[0;32m[{}:{},{}]\e[0m " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) ssat::Log(ssat::LogLevel::ERROR,   "\e[0;32m[{}:{},{}]\e[0m " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LOG_DETAIL(fmt, ...) ssat::Log(ssat::LogLevel::DETAIL, "\e[0;32m[{}:{},{}]\e[0m " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
}

