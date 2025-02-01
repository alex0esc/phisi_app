#include "logger.hpp"

namespace slog {
    
  Logger g_logger = Logger();

  Logger& Logger::begin_msg() {
    m_mtx.lock();
    return *this;
  }

  void Logger::end_msg() {
    m_mtx.unlock();
  }
    
}