#pragma once
#include <iostream>
#include <mutex>

#define _INTERNAL_LOG(msg) (slog::g_logger.begin_msg() << msg << std::endl).end_msg()

#ifdef _OPTION_LOG_ERROR
  #define LOG_ERROR(msg) _INTERNAL_LOG("[\033[31mError\033[0m]\t" << msg)
#else
  #define LOG_ERROR(msg)
#endif

#ifdef _OPTION_LOG_WARN
  #define LOG_WARN(msg) _INTERNAL_LOG("[\033[33mWarn\033[0m]\t" << msg)
#else
  #define LOG_WARN(msg)
#endif

#ifdef _OPTION_LOG_INFO
  #define LOG_INFO(msg) _INTERNAL_LOG("[\033[36mInfo\033[0m]\t" << msg)
#else
  #define LOG_INFO(msg)
#endif

#ifdef _OPTION_LOG_TRACE
#define LOG_TRACE(msg) _INTERNAL_LOG("[\033[35mTrace\033[0m]\t" << msg)
#else
  #define LOG_TRACE(msg)
#endif


namespace slog {

  class Logger {
    std::mutex m_mtx;
    std::ostream* m_out;
     
  public:  
    Logger(std::ostream& stream = std::cout) 
      : m_mtx(), m_out(&stream) {}

    Logger& begin_msg();
    void end_msg();
    
    template<typename T>
    Logger& operator<<(const T& value) {
      *m_out << value;
      return *this;
    }
    
    Logger& operator<<(std::ostream& (*manip)(std::ostream&)) {
      *m_out << manip;
      return *this;
    }
  };

  extern Logger g_logger;  
}