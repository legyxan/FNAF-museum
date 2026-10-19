#pragma once
#include <string>
#include <windows.h>

// Валидный ли UTF-8?
inline bool isUtf8Str(const std::string& s) {
    if (s.empty()) return false;
    int wlen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
        s.c_str(), (int)s.size(), NULL, 0);
    return wlen > 0;
}

// UTF-8 → Windows-1251
inline std::string utf8ToWin1251(const std::string& s) {
    if (s.empty()) return s;
    int wlen = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), NULL, 0);
    if (wlen <= 0) return s;
    std::wstring w(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &w[0], wlen);
    int len = WideCharToMultiByte(1251, 0, w.c_str(), (int)w.size(), NULL, 0, NULL, NULL);
    if (len <= 0) return s;
    std::string r(len, '\0');
    WideCharToMultiByte(1251, 0, w.c_str(), (int)w.size(), &r[0], len, NULL, NULL);
    return r;
}

// Windows-1251 → UTF-8
inline std::string win1251ToUtf8(const std::string& s) {
    if (s.empty()) return s;
    int wlen = MultiByteToWideChar(1251, 0, s.c_str(), (int)s.size(), NULL, 0);
    if (wlen <= 0) return s;
    std::wstring w(wlen, L'\0');
    MultiByteToWideChar(1251, 0, s.c_str(), (int)s.size(), &w[0], wlen);
    int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), NULL, 0, NULL, NULL);
    if (len <= 0) return s;
    std::string r(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), &r[0], len, NULL, NULL);
    return r;
}

// Универсальный «фикс» для вывода: если UTF-8 — конвертируем в 1251, если 1251 — оставляем
inline std::string fix(const std::string& s) {
    if (s.empty()) return s;
    if (isUtf8Str(s)) return utf8ToWin1251(s);
    return s;
}