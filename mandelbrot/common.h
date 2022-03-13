#pragma once

#include <QString>

template <typename... Args>
QString format(const QString &format, Args... args) {
  int size = std::snprintf(nullptr, 0, format.toLatin1().data(), args...) +
             1; // Extra space for '\0'

  if (size <= 0)
    return "";

  char buf[size];
  std::snprintf(buf, size, format.toLatin1().data(), args...);
  return QString(buf); // We don't want the '\0' inside
}
