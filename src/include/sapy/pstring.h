#pragma once 

#include "sapy/pobject.h"
#include <vector>
#include <cstdint>
#include <string>
#include <codecvt>
#include <locale>
#include <stdexcept>
#include <iostream>
#include <format>
#include "sapy/pbytes.h"
#include "sapy/piterator.h"
#include "sapy/plist.h"
#include "sapy/panywrapper.h"
#include "sapy/pdict.h"
namespace sapy{

class PList;
class PAnyWrapper;
class PDict;
class PBytes;

class PString : public PObject {
public:
    PString();                           
    PString(const PString&) = default;   
    PString(const char* str);            
    PString(const std::u32string& str);  
    PString(const std::string& str);     
    PString(char32_t c);                
    virtual ~PString();

    virtual PString toString() const override;
    std::string toStdString() const;
    int toInt() const;
    size_t length() const;
    
    PString substr(size_t start, size_t end=std::u32string::npos) const;
    
    

    PString captilize() const;
    PString casefold() const;
    PString center(size_t width, char fillchar=' ') const;
    size_t count(const PString& sub, size_t start=0, size_t end=std::u32string::npos) const;
    PBytes encode(const PString& encoding="utf-8", const PString& errors="strict") const;
    bool endswith(const PString& other) const;
    PString expandtabs(size_t tabsize=8) const;
    int find(const PString& sub, size_t start=0, size_t end=std::u32string::npos) const;
    
    template <typename... Args>
    inline PString format(Args&&... args) const{
        return PString(std::vformat(this->toStdString(), std::make_format_args(args...)));
    }
    

    int index(const PString& sub, size_t start=0, size_t end=std::u32string::npos) const;
    bool isalnum() const;
    bool isalpha() const;
    bool isascii() const;
    bool isdecimal() const;
    bool isdigit() const;
    bool isidentifier() const;
    bool islower() const;
    bool isnumeric() const;
    bool isprintable() const;
    bool isspace() const;
    bool istitle() const;
    bool isupper() const;

    template <typename Iterable>
    inline PString join(const Iterable& it) const{
        PString result;
        for (auto iter = it.cbegin(); iter != it.cend(); iter++) {
            result += *iter;
            if (iter + 1 != it.cend()) {
                result += *this;
            }
        }
        return result;
    }

    PString ljust(size_t width, char fillchar=' ') const;
    PString lower() const;
    PString lstrip(const PString& __strp_str = "\r\n\t ") const;
    static PDict maketrans(PDict& x);
    static PDict maketrans(const PString& x, const PString& y);
    static PDict maketrans(const PString& x, const PString& y, const PString &z);
    PList partition(const PString& sep) const;
    PString remoteprefix(const PString& prefix) const;
    PString removesuffix(const PString& suffix) const;
    PString replace(const PString& old, const PString& new_, size_t count=-1) const;
    int rfind(const PString& sub, size_t start=0, size_t end=std::u32string::npos) const;
    int rindex(const PString& sub, size_t start=0, size_t end=std::u32string::npos) const;
    PString rjust(size_t width, char fillchar=' ') const;
    PList rpartition(const PString& sep) const;
    PList rsplit(const PString& sep = "", size_t maxsplit = -1) const;
    PString rstrip(const PString& __strp_str = "\r\n\t ") const;
    PList split(const PString& sep = "", size_t maxsplit = -1)const;
    PList splitlines(bool keepends=false) const;
    bool startswith(const PString& other) const;
    PString strip(const PString& __strp_str = "\r\n\t ") const;
    PString swapcase() const;
    PString title() const;
    PString translate (const PDict& table) const; 
    PString upper() const;
    PString zfill(size_t width) const;

    static std::u32string fromUTF8ToUTF32(const std::string& utf8Str);
    static std::string fromUTF32ToUTF8(const std::u32string& utf32Str);

    operator const std::u32string&() const {
        return _data;
    }
    operator std::string() const {
        return std::move(fromUTF32ToUTF8(_data));
    }


    bool operator==(const PString& other) const;
    bool operator!=(const PString& other) const;
    bool operator<(const PString& other) const;
    inline bool operator>=(const PString& other) const {
        return !(*this < other);
    }
    bool operator>(const PString& other) const;
    inline bool operator<=(const PString& other) const {
        return !(*this > other);
    }
    
    const PString operator[](size_t index) const;

    
    PString operator+(const PString& other) const;
    PString operator+(char32_t other) const;
    PString operator+(const char* other) const;
    PString operator+(const std::string& other) const;
    PString operator+(char other) const;

    
    PString operator*(size_t n) const;
    PString& operator*=(size_t n);

    // +=
    PString& operator+=(const PString& other);
    PString& operator+=(char32_t other);
    PString& operator+=(const char* other);
    PString& operator+=(char other);
    PString& operator+=(const PAnyWrapper& other);
    PString& operator+=(const std::string& other);
    
    using iterator = std::u32string::iterator;
    using const_iterator = std::u32string::const_iterator;
    using value_type = PString;  
    using const_value_type = const PString;

    PIterator<PString> begin();
    PIterator<PString> end();
    const PIterator<PString> cbegin() const;
    const PIterator<PString> cend() const;
    
    size_t hash() const override;

private:
    virtual void _print(std::ostream& os) const override;
    std::u32string _data;
};


PString operator+(const char* lhs, const PString& rhs);
PString operator+(const std::string& lhs, const PString& rhs);


}


template <>
struct std::formatter<sapy::PString> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    auto format(const sapy::PString& pstr, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", static_cast<std::string>(pstr));
    }
};