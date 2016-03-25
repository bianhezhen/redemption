/*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*   Product name: redemption, a FLOSS RDP proxy
*   Copyright (C) Wallix 2010-2015
*   Author(s): Jonathan Poelen
*/

#ifndef REDEMPTION_SRC_UTILS_CONFIG_TYPES_TYPES_HPP
#define REDEMPTION_SRC_UTILS_CONFIG_TYPES_TYPES_HPP

#include "utils/parser.hpp"
#include "utils/fileutils.hpp"
#include "utils/underlying_cast.hpp"
#include "utils/array_view.hpp"

#include "configs/variant/capture_flags.hpp"
#include "configs/variant/keyboard_log_flags.hpp"
#include "configs/c_str_buf.hpp"

#include <iosfwd>
#include <type_traits>

#include <cstddef>
#include <cassert>

namespace configs {

struct null_fill { null_fill() {} };

struct StringCopier
{
    static void copy(char * dest, char const * src, std::size_t n) {
        const char * e = dest + n - 1;
        for (; dest != e && *src; ++src, ++dest) {
            *dest = *src;
        }
        *dest = 0;
    }
};

template<std::size_t N, class Copier>
class StaticStringBase
{
    StaticStringBase(StaticStringBase const &) = delete;

public:
    explicit StaticStringBase(null_fill) noexcept
    : str{}
    {}

    StaticStringBase() noexcept
    { this->str[0] = 0; }

    explicit StaticStringBase(const char * s)
    { *this = s; }

    StaticStringBase& operator=(const char * s) {
        Copier::copy(this->str, s, N);
        return *this;
    }

    StaticStringBase& operator=(std::string const & s) {
        Copier::copy(this->str, s.data(), N);
        return *this;
    }

    const char * c_str() const noexcept {
        return this->str;
    }

    bool empty() const {
        return (this->str[0] == '\0');
    }

    constexpr operator const char * () const noexcept {
        return this->str;
    }

    char operator[](std::size_t i) const noexcept {
        assert(i < N);
        return this->str[i];
    }

    char & operator[](std::size_t i) noexcept {
        assert(i < N);
        return this->str[i];
    }

    constexpr std::size_t max_size() const noexcept {
        return N - 1;
    }

    char * data() noexcept {
        return this->str;
    }

protected:
    char str[N];

    // disable deleted constructor
    friend class Inifile;
    StaticStringBase(StaticStringBase &&);
};


template<std::size_t N, class Copier>
char const * c_str(
    CStrBuf<StaticStringBase<N, Copier>>&,
    StaticStringBase<N, Copier> const & x
) {
    return x.c_str();
}

template<std::size_t N, class Copier>
void parse(StaticStringBase<N, Copier> & x, char const * value) {
    x = value;
}

template<std::size_t N, class Copier>
int copy_val(StaticStringBase<N, Copier> const & x, char * buff, std::size_t n) {
    return snprintf(buff, n, "%s", x.c_str());
}


template<std::size_t N>
using StaticString = StaticStringBase<N, StringCopier>;

template<std::size_t N>
struct StaticKeyString
{
    explicit StaticKeyString(null_fill = {}) noexcept
    : str{}
    {}

    explicit StaticKeyString(const char (&key)[33]) {
        // 32 = 33 - null terminal
        memcpy(this->str, key, 32);
        this->str[N] = 0;
    }

    StaticKeyString(StaticKeyString const &) = default;

    StaticKeyString & operator=(char (&key)[32]) {
        memcpy(this->str, key, 32);
        return *this;
    }

    StaticKeyString & operator=(array_view<unsigned char const> const & key) {
        assert(key.size() <= N);
        memcpy(this->str, key.data(), key.size());
        memset(this->str + key.size(), 0, N - key.size());
        return *this;
    }

    StaticKeyString & operator=(array_view<char const> const & key) {
        return *this = {reinterpret_cast<unsigned char const *>(key.data()), key.size()};
    }

    using array_key_type = unsigned char const (&)[32];
    constexpr operator array_key_type () const noexcept {
        return reinterpret_cast<array_key_type>(*this->str);
    }

    unsigned char operator[](std::size_t i) const noexcept {
        assert(i < N);
        return this->str[i];
    }

    unsigned char & operator[](std::size_t i) noexcept {
        assert(i < N);
        return this->str[i];
    }

    constexpr std::size_t max_size() const noexcept {
        return N - 1;
    }

    constexpr std::size_t size() const noexcept {
        return N - 1;
    }

    unsigned char const * data() const noexcept {
        return this->str;
    }

    unsigned char * data() noexcept {
        return this->str;
    }

protected:
    unsigned char str[N+1];

    // disable deleted constructor
    friend class Inifile;
    StaticKeyString(StaticKeyString &&);
};

template<std::size_t N>
struct CStrBuf<StaticKeyString<N>> {
    char buf[N*2 + 1];

    size_t size() const {
        return N * 2;
    }

    char * get() {
        return this->buf;
    }

    char const * get() const {
        return this->buf;
    }
};

template<std::size_t N>
void parse(StaticKeyString<N> & key, char const * value) {
    if (strlen(value) >= N * 2) {
        char   hexval[3] = { 0 };
        char * end;
        for (std::size_t i = 0; i < N; i++) {
            memcpy(hexval, value + i * 2, 2);
            key[i] = strtol(hexval, &end, 16);
        }
    }
}

template<std::size_t N>
int copy_val(StaticKeyString<N> const & key, char * buff, std::size_t n) {
    if (N * 2 <= n) {
        auto first = key.data();
        auto last = key.data() + N;
        for (; first != last; ++buff, ++first) {
            auto x = (unsigned(*first) & 0xf0) >> 4;
            *buff = x < 10 ? ('0' + x) : ('A' + x - 10);
            ++buff;
            x = unsigned(*first) & 0xf;
            *buff = x < 10 ? ('0' + x) : ('A' + x - 10);
        }
    }
    return int(N*2);
}


struct Ipv4Copier : StringCopier {};
using StaticIpString = StaticStringBase<16, Ipv4Copier>;

struct PathCopier
{
    static void copy(char * dest, char const * src, std::size_t n) {
        pathncpy(dest, src, n);
    }
};

template<std::size_t N>
using StaticPath = StaticStringBase<N, PathCopier>;


template<class T, T Min, T Max, T Default = Min>
struct Range {
    static_assert(Min < Max, "interval error");
    static_assert(Min <= Default && Default < Max, "interval error");

    explicit Range(T x = Default)
    : x_(x)
    {}

    Range & operator = (T x) {
        if (x < Min) {
            this->x_ = Min;
        }
        else if (x > Max) {
            this->x_ = Max;
        }
        else {
            this->x_ = x;
        }
        return *this;
    }
    T get() const { return this->x_; }
    operator T () const { return this->x_; }

private:
    T x_;
};


template<class T, T Min, T Max, T Default = Min>
struct SelectRange : Range<T, Min, Max> {
    using Range<T, Min, Max>::Range;
    using Range<T, Min, Max>::operator=;
};

template<class T, T Min, T Max, T Default>
struct CStrBuf<Range<T, Min, Max, Default>>
: CStrBuf<T>
{};


template<std::size_t N>
char const * c_str(CStrBuf<StaticKeyString<N>>& s, StaticKeyString<N> const & key) {
    s.get()[N*2] = 0;
    copy_val(key, s.get(), s.size());
    return s.get();
}

template<class T, T Min, T Max, T Default>
char const * c_str(CStrBuf<Range<T, Min, Max, Default>>& s, Range<T, Min, Max, Default> const & x) {
    return c_str(s, x.get());
}



#include "mk_enum_def.hpp"


enum class Level : unsigned { low, medium, high, NB };
ENUM_OPTION(Level, "low", "medium", "high");
MK_ENUM_IO(::configs::Level)

inline Level level_from_cstr(char const * value) {
    return
        0 == strcasecmp("medium", value) ? Level::medium
      : 0 == strcasecmp("high",value) ? Level::high
      : Level::low
    ;
}

inline char const * cstr_from_level(Level lvl) {
    return lvl == Level::high
        ? "high"
        : lvl == Level::medium
        ? "medium"
        : "low";
}

inline char const * enum_to_option(Level e) {
    return cstr_from_level(e);
}

inline void parse(Level & x, char const * value) { x = level_from_cstr(value); }

inline int copy_val(Level x, char * buff, std::size_t n) {
    return snprintf(buff, n, "%s", cstr_from_level(x));
}

inline char const * c_str(CStrBuf<Level>&, Level x) {
    return cstr_from_level(x);
}


enum class Language : unsigned { en, fr, NB };
MK_ENUM_FIELD(::configs::Language, "en", "fr")


enum class ClipboardEncodingType : unsigned { utf8, latin1, NB };
MK_ENUM_FIELD(::configs::ClipboardEncodingType, "utf-8", "latin1")

enum class ClipboardLogFlags : unsigned {
    none,
    syslog = 1 << 0,
    wrm = 1 << 1,
    FULL = ((1 << 2) - 1)
};
MK_PARSER_ENUM_FLAGS(::configs::ClipboardLogFlags)


enum class FileSystemLogFlags : unsigned {
    none,
    syslog = 1 << 0,
    wrm = 1 << 1,
    FULL = ((1 << 2) - 1)
};
MK_PARSER_ENUM_FLAGS(::configs::FileSystemLogFlags)


enum class ColorDepth : unsigned {
    depth8 = 8,
    depth15 = 15,
    depth16 = 16,
    depth24 = 24,
    //depth32,
};
ENUM_OPTION(ColorDepth, 8, 15, 16, 24);
MK_ENUM_IO(::configs::ColorDepth)

inline ColorDepth enum_to_option(ColorDepth e) {
    return e;
}

inline ColorDepth color_depth_from_cstr(char const * value) {
    switch (ulong_from_cstr(value)) {
        case 8: return ColorDepth::depth8;
        case 15: return ColorDepth::depth15;
        case 16: return ColorDepth::depth16;
        default:
        case 24: return ColorDepth::depth24;
        //case 32: x = ColorDepth::depth32;
    }
}

inline char const * cstr_from_color_depth(ColorDepth c) {
    switch (c) {
        case ColorDepth::depth8: return "8";
        case ColorDepth::depth15: return "15";
        case ColorDepth::depth16: return "16";
        default:
        case ColorDepth::depth24: return "24";
        //case 32: x = ColorDepth::depth32;
    }
}

inline void parse(ColorDepth & x, char const * value) {
    x = color_depth_from_cstr(value);
}

inline int copy_val(ColorDepth x, char * buff, std::size_t n) {
    return snprintf(buff, n, "%s", cstr_from_color_depth(x));
}

inline char const * c_str(CStrBuf<ColorDepth>&, ColorDepth x) {
    return cstr_from_color_depth(x);
}


enum class ServerNotification {
    nobody  = 0,
    syslog  = 1 << 0,
    user    = 1 << 1,
    admin   = 1 << 2,
    FULL    = ((1 << 3) - 1)
};
MK_PARSER_ENUM_FLAGS(::configs::ServerNotification)


enum class ServerCertCheck : unsigned {
    fails_if_no_match_or_missing,             // ensure match, ensure present
    fails_if_no_match_and_succeed_if_no_know, // ensure match, dont care missing
    succeed_if_exists_and_fails_if_missing,   // dont care match, ensure present
    always_succeed,                           // dont care match, dont care missing
    NB
};
MK_ENUM_FIELD(::configs::ServerCertCheck, "0", "1", "2", "3")


enum class TraceType : unsigned {
    localfile,
    localfile_hashed,
    cryptofile,
    NB
};
MK_ENUM_FIELD(::configs::TraceType, "0", "1", "2")


enum class KeyboardInputMaskingLevel : unsigned {
    unmasked,
    password_only,
    password_and_unidentified,
    NB
};
MK_ENUM_FIELD(::configs::KeyboardInputMaskingLevel, "0", "1", "2")


enum class SessionProbeOnLaunchFailure {
    ignore_and_continue,
    disconnect_user,
    retry_without_session_probe,
    NB
};
MK_ENUM_FIELD(::configs::SessionProbeOnLaunchFailure, "0", "1", "2")


#include "mk_enum_undef.hpp"

}

#endif
