#include "sapy/pstring.h"
#include "sapy/plist.h"
#include <codecvt>
#include <locale>
#include <stdexcept>
#include <iostream>
#include <format>
#include <vector>
#include <string>

namespace sapy
{

PString::PString() {}

PString::PString(const char *str)
{
    _data = fromUTF8ToUTF32(std::string(str));
}

PString::PString(const std::u32string &str)
    : _data(str) {}

PString::PString(const std::string &str)
{
    _data = fromUTF8ToUTF32(str);
}

PString::PString(char32_t c)
{
    _data.push_back(c);
}

PString::~PString() {}

PString PString::toString() const
{
    return PString("\"") + *this + "\"";
}

std::string PString::toStdString() const
{
    return fromUTF32ToUTF8(_data);
}

int PString::toInt() const
{
    return std::stoi(toStdString());
}

size_t PString::length() const
{
    return _data.size();
}

PString PString::lstrip(const PString &__strp_str) const
{
    size_t pos = _data.find_first_not_of(__strp_str._data);
    return PString(_data.substr(pos));
}

PString PString::rstrip(const PString &__strp_str) const
{
    size_t pos = _data.find_last_not_of(__strp_str._data);
    return PString(_data.substr(0, pos + 1));
}
PString PString::strip(const PString &__strp_str) const
{
    return lstrip(__strp_str).rstrip(__strp_str);
}

bool PString::startswith(const PString &other) const
{
    return _data.rfind(other._data, 0) == 0;
}

bool PString::endswith(const PString &other) const
{

    if (other._data.size() > _data.size())
    {
        return false;
    }
    return _data.compare(_data.size() - other._data.size(),
                            other._data.size(),
                            other._data) == 0;
}

PString PString::lower() const
{
    std::u32string lowerStr;
    for (char32_t c : _data)
    {
        lowerStr.push_back(static_cast<char32_t>(std::towlower(static_cast<wint_t>(c))));
    }
    return PString(lowerStr);
}

PString PString::upper() const
{
    std::u32string upperStr;
    for (char32_t c : _data)
    {
        upperStr.push_back(static_cast<char32_t>(std::towupper(static_cast<wint_t>(c))));
    }
    return PString(upperStr);
}

PList PString::split(const PString &sep, size_t maxsplit) const
{

    size_t pos = 0;
    size_t prevPos = 0;
    size_t cnt = 0;
    PList result;
    if (sep.length() == 0)
    {
        result.append(*this);
        return result;
    }
    while (cnt++ < maxsplit && (pos = _data.find(sep._data, prevPos)) != std::u32string::npos)
    {
        result.append(PString(_data.substr(prevPos, pos - prevPos)));
        prevPos = pos + sep._data.size();
    }
    if (prevPos <= _data.size())
    {
        result.append(PString(_data.substr(prevPos)));
    }
    return result;
}

PString PString::captilize() const
{
    if (_data.empty())
    {
        return PString();
    }
    return PString(std::towupper(_data[0])) + PString(_data.substr(1)).lower();
}

PString PString::casefold() const
{
    return lower(); // TODO
}

PString PString::center(size_t width, char fillchar) const
{
    if (width <= _data.size())
    {
        return *this;
    }
    size_t padSize = width - _data.size();
    size_t padLeft = padSize / 2;
    size_t padRight = padSize - padLeft;
    return PString(std::u32string(padLeft, fillchar) + _data + std::u32string(padRight, fillchar));
}

PString PString::substr(size_t start, size_t end) const
{
    if (end == std::u32string::npos)
    {
        return PString(_data.substr(start));
    }
    return PString(_data.substr(start, end - start));
}

size_t PString::count(const PString &sub, size_t start, size_t end) const
{
    size_t count = 0;
    size_t pos = start;
    while ((pos = _data.substr(start, end - start).find(sub._data, pos)) != std::u32string::npos)
    {
        if (pos >= end)
            break;
        count++;
        pos += sub._data.length();
    }
    return count;
}

int PString::find(const PString &sub, size_t start, size_t end) const
{
    size_t pos = _data.substr(start, end - start).find(sub._data);
    if (pos == std::u32string::npos)
        return -1;
    return pos;
}

int PString::index(const PString &sub, size_t start, size_t end) const
{
    size_t pos = find(sub, start, end);
    if (pos == -1)
        throw std::runtime_error("substring not found");
    return pos;
}

bool PString::isalnum() const
{
    if (_data.length() == 0)
        return false;
    for (auto c : _data)
    {
        if (!std::iswalnum(c))
            return false;
    }
    return true;
}

bool PString::isalpha() const
{
    if (_data.length() == 0)
        return false;
    for (auto c : _data)
    {
        if (!std::iswalpha(c))
            return false;
    }
    return true;
}

bool PString::isascii() const
{
    if (_data.length() == 0)
        return false;
    for (auto c : _data)
    {
        if (c > 0x7f)
            return false;
    }
    return true;
}

bool PString::isdecimal() const
{
    if (_data.length() == 0)
        return false;
    for (auto c : _data)
    {
        if (!std::iswdigit(c))
            return false; // TODO
    }
    return true;
}

bool PString::isdigit() const
{
    if (_data.length() == 0)
        return false;
    for (auto c : _data)
    {
        if (!std::iswdigit(c))
            return false;
    }
    return true;
}

bool PString::isidentifier() const
{
    if (_data.length() == 0)
        return false;
    if (std::iswdigit(_data[0]))
        return false;
    for (auto c : _data.substr(1))
    {
        if (!std::iswalnum(c) && c != '_')
            return false;
    }
    return true;
}

bool PString::islower() const
{
    if (_data.length() == 0)
        return false;
    bool cased = false;
    for (auto c : _data)
    {
        if (std::iswalpha(c))
        {
            if (!std::iswlower(c))
                return false;
            cased = true;
        }
    }
    return cased;
}

bool PString::isnumeric() const
{
    if (_data.length() == 0)
        return false;
    for (auto c : _data)
    {
        if (!std::iswdigit(c))
            return false; // TODO
    }
    return true;
}

bool PString::isprintable() const
{
    if (_data.length() == 0)
        return true;
    for (auto c : _data)
    {
        if (!std::iswprint(c))
            return false;
    }
    return true;
}

bool PString::isspace() const
{
    if (_data.length() == 0)
        return false;
    for (auto c : _data)
    {
        if (!std::iswspace(c))
            return false;
    }
    return true;
}

bool PString::istitle() const
{
    bool cased = false;
    for (size_t i = 0; i < _data.size(); i++)
    {
        if (!std::iswalpha(_data[i]))
        {
            cased = false;
            continue;
        }
        if (std::iswupper(_data[i]))
        {
            if (cased)
                return false;
            cased = true;
        }
        else if (std::iswlower(_data[i]))
        {
            if (!cased)
                return false;
        }
    }
    return cased;
}

bool PString::isupper() const
{
    if (_data.length() == 0)
        return false;
    bool cased = false;
    for (auto c : _data)
    {
        if (std::iswalpha(c))
        {
            if (!std::iswupper(c))
                return false;
            cased = true;
        }
    }
    return cased;
}

PString PString::ljust(size_t width, char fillchar) const
{
    if (width <= _data.size())
    {
        return *this;
    }
    size_t padSize = width - _data.size();
    return PString(_data + std::u32string(padSize, fillchar));
}

PString PString::rjust(size_t width, char fillchar) const
{
    if (width <= _data.size())
    {
        return *this;
    }
    size_t padSize = width - _data.size();
    return PString(std::u32string(padSize, fillchar) + _data);
}

PDict PString::maketrans(PDict &x)
{
    return x; // TODO
}

PDict PString::maketrans(const PString &x, const PString &y)
{
    PDict result;
    if (x.length() != y.length())
    {
        throw std::runtime_error("the first two maketrans arguments must have equal length");
    }
    for (size_t i = 0; i < x.length(); i++)
    {
        result[x[i]] = y[i];
    }
    return result;
}

PDict PString::maketrans(const PString &x, const PString &y, const PString &z)
{
    PDict result;
    if (x.length() != y.length() || x.length() != z.length())
    {
        throw std::runtime_error("the first two maketrans arguments must have equal length");
    }
    for (size_t i = 0; i < x.length(); i++)
    {
        result[x[i]] = y[i];
    }
    for (size_t i = 0; i < z.length(); i++)
    {
        result[z[i]] = PString();
    }
    return result;
}


PList PString::partition(const PString &sep) const
{
    PList result;
    size_t pos = find(sep);
    if (pos == -1)
    {
        result.append(*this, PString(), PString());
        return result;
    }
    result.append(substr(0, pos), sep, substr(pos + sep.length()));
    return result;
}

PString PString::remoteprefix(const PString &prefix) const
{
    if (startswith(prefix))
    {
        return substr(prefix.length());
    }
    return *this;
}

PString PString::removesuffix(const PString &suffix) const
{
    if (endswith(suffix))
    {
        return substr(0, length() - suffix.length());
    }
    return *this;
}

PString PString::replace(const PString &old, const PString &new_, size_t count) const
{
    PString result;
    size_t pos = 0;
    size_t prevPos = 0;
    while ((pos = _data.find(old._data, prevPos)) != std::u32string::npos)
    {
        result += PString(_data.substr(prevPos, pos - prevPos));
        result += new_;
        prevPos = pos + old._data.size();
        if (count != std::u32string::npos)
        {
            count--;
            if (count == 0)
            {
                break;
            }
        }
    }
    if (prevPos <= _data.size())
    {
        result += PString(_data.substr(prevPos));
    }
    return result;
}

int PString::rfind(const PString &sub, size_t start, size_t end) const
{
    size_t pos = _data.substr(start, end - start).rfind(sub._data);
    if (pos == std::u32string::npos)
        return -1;
    return pos;
}

int PString::rindex(const PString &sub, size_t start, size_t end) const
{
    size_t pos = find(sub, start, end);
    if (pos == -1)
        throw std::runtime_error("substring not found");
    return pos;
}

PList PString::rpartition(const PString &sep) const
{
    PList result;
    size_t pos = rfind(sep);
    if (pos == -1)
    {
        result.append(PString(), PString(), *this);
        return result;
    }
    result.append(substr(0, pos), sep, substr(pos + sep.length()));
    return result;
}

PList PString::rsplit(const PString &sep, size_t maxsplit) const
{
    size_t pos = 0;
    size_t prevPos = _data.size();
    size_t cnt = 0;
    PList result;
    std::vector<PString> result_rev;
    if (sep.length() == 0)
    {
        result.append(*this);
        return result;
    }
    while (cnt++ < maxsplit && (pos = _data.rfind(sep._data, prevPos - 1)) != std::u32string::npos)
    {
        result_rev.emplace_back(PString(_data.substr(pos + sep.length(), prevPos - pos - sep.length())));
        prevPos = pos;
    }
    if (prevPos >= 0)
    {
        result_rev.emplace_back(PString(_data.substr(0, prevPos)));
    }
    for (auto it = result_rev.rbegin(); it != result_rev.rend(); it++)
    {
        result.append(*it);
    }
    return result;
}

PList PString::splitlines(bool keepends) const
{
    PList result;
    size_t pos = 0;
    size_t prevPos = 0;
    // support \r \n \r\n \v \f \x1c \x1d \x1e \x85 \u2028 \u2029
    while ((pos = _data.find_first_of(U"\r\n\v\f\x1c\x1d\x1e\x85\u2028\u2029", prevPos)) != std::u32string::npos)
    {
        if (_data[pos] == U'\r' && pos + 1 < _data.size() && _data[pos + 1] == U'\n')
        {
            if (keepends){
                result.append(PString(_data.substr(prevPos, pos - prevPos + 2)));
            }
            else{
                result.append(PString(_data.substr(prevPos, pos - prevPos)));
            }
            pos++;
        }
        else{
            if (keepends){
                result.append(PString(_data.substr(prevPos, pos - prevPos + 1)));
            }
            else{
                result.append(PString(_data.substr(prevPos, pos - prevPos)));
            }
        }
        prevPos = pos + 1;
    }
    if (prevPos <= _data.size())
    {
        result.append(PString(_data.substr(prevPos)));
    }
    return result;
}

PString PString::swapcase() const
{
    std::u32string swapcaseStr;
    for (char32_t c : _data)
    {
        if (std::iswlower(c))
        {
            swapcaseStr.push_back(std::towupper(c));
        }
        else if (std::iswupper(c))
        {
            swapcaseStr.push_back(std::towlower(c));
        }
        else
        {
            swapcaseStr.push_back(c);
        }
    }
    return PString(swapcaseStr);
}

PString PString::title() const
{
    std::u32string titleStr;
    bool start = false;
    for (char32_t c : _data)
    {
        if(!std::iswalpha(c)){
            titleStr.push_back(c);
            start = false;
            continue;
        }else{
            if(!start){
                titleStr.push_back(std::towupper(c));
                start = true;
            }else{
                titleStr.push_back(std::towlower(c));
            }
        }
    }
    return PString(titleStr);
}

PString PString::translate(const PDict &table) const
{
    PString result;
    for (auto c : _data)
    {
        auto ch = PString(c);
        if(table.contain(ch)){
            result += table[ch].unwrap<PString>();
        }else{
            result += c;
        }
    }
    return result;
}

PString PString::zfill(size_t width) const
{
    if (width <= _data.size())
    {
        return *this;
    }
    size_t padSize = width - _data.size();
    if (_data[0] == U'-' || _data[0] == U'+')
    {
        return PString(_data.substr(0, 1) + std::u32string(padSize, U'0') + _data.substr(1));
    }
    return PString(std::u32string(padSize, U'0') + _data);
}

PString PString::expandtabs(size_t tabsize) const
{
    PString result;
    size_t cnt = 0;
    for (auto c : _data)
    {
        if (c == U'\t'){
            if(cnt >= tabsize){
                result += PString(" ")*tabsize;
            }else{
                result += PString(" ")*(tabsize - cnt);
            }
            cnt = 0;
        }
        else
        {
            result += c;
            if(c == U'\n' || c == U'\r'){
                cnt = 0;
            }else{
                cnt++;
            }
        }
    }
    return result;
}

PBytes PString::encode(const PString &encoding, const PString &errors) const {
    std::vector<uint8_t> result;

    std::string encoding_str = encoding.toStdString();

    if (encoding_str == "utf-8") {
        // UTF-32 to UTF-8 conversion
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        try {
            std::string utf8_str = converter.to_bytes(_data);
            result.assign(utf8_str.begin(), utf8_str.end());
        } catch (const std::range_error &e) {
            if (errors == "strict") {
                throw std::runtime_error("Encoding error: " + std::string(e.what()));
            } else if (errors == "ignore") {
                // Ignore errors, return partial result
            } else {
                throw std::runtime_error("Unsupported error handling mode: " + errors.toStdString());
            }
        }

    } else if (encoding_str == "utf-16") {
        // UTF-32 to UTF-16 conversion with BOM (Little Endian)
        std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> converter;
        try {
            // Add BOM for UTF-16 Little Endian
            result.push_back(0xFF);
            result.push_back(0xFE);

            for (char32_t ch : _data) {
                if (ch <= 0xFFFF) { // BMP character
                    result.push_back(static_cast<uint8_t>(ch & 0xFF));
                    result.push_back(static_cast<uint8_t>((ch >> 8) & 0xFF));
                } else { // Non-BMP character, encode as surrogate pair
                    char32_t surrogate = ch - 0x10000;
                    char16_t high = static_cast<char16_t>((surrogate >> 10) + 0xD800);
                    char16_t low = static_cast<char16_t>((surrogate & 0x3FF) + 0xDC00);

                    result.push_back(static_cast<uint8_t>(high & 0xFF));
                    result.push_back(static_cast<uint8_t>((high >> 8) & 0xFF));
                    result.push_back(static_cast<uint8_t>(low & 0xFF));
                    result.push_back(static_cast<uint8_t>((low >> 8) & 0xFF));
                }
            }
        } catch (const std::range_error &e) {
            if (errors == "strict") {
                throw std::runtime_error("Encoding error: " + std::string(e.what()));
            } else if (errors == "ignore") {
                // Ignore errors, return partial result
            } else {
                throw std::runtime_error("Unsupported error handling mode: " + errors.toStdString());
            }
        }

    } else if (encoding_str == "utf-32") {
        // UTF-32 encoding with BOM (Little Endian)
        result.push_back(0xFF);
        result.push_back(0xFE);
        result.push_back(0x00);
        result.push_back(0x00);

        for (char32_t ch : _data) {
            uint8_t bytes[4];
            bytes[0] = static_cast<uint8_t>(ch & 0xFF);         // Lowest byte
            bytes[1] = static_cast<uint8_t>((ch >> 8) & 0xFF);
            bytes[2] = static_cast<uint8_t>((ch >> 16) & 0xFF);
            bytes[3] = static_cast<uint8_t>((ch >> 24) & 0xFF); // Highest byte
            result.insert(result.end(), bytes, bytes + 4);
        }
    } else {
        throw std::runtime_error("Unsupported encoding: " + encoding.toStdString());
    }

    return PBytes(result);
}


std::u32string PString::fromUTF8ToUTF32(const std::string &utf8Str)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.from_bytes(utf8Str);
}

std::string PString::fromUTF32ToUTF8(const std::u32string &utf32Str)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.to_bytes(utf32Str);
}

bool PString::operator==(const PString &other) const
{
    return _data == other._data;
}

bool PString::operator!=(const PString &other) const
{
    return !(*this == other);
}

bool PString::operator<(const PString &other) const
{
    return _data < other._data;
}

bool PString::operator>(const PString &other) const
{
    return _data > other._data;
}

const PString PString::operator[](size_t index) const
{
    if (index >= _data.size())
    {
        throw std::out_of_range("PString: index out of range");
    }

    auto singleChar32 = _data[index];
    return PString(singleChar32);
}

PString PString::operator+(const PString &other) const
{
    return PString(_data + other._data);
}

PString PString::operator+(char32_t other) const
{
    return PString(_data + std::u32string(1, other));
}

PString PString::operator+(const char *other) const
{
    return *this + PString(other);
}

PString PString::operator+(const std::string &other) const
{
    return *this + PString(other);
}

PString PString::operator+(char other) const
{
    std::u32string tmp = _data + std::u32string(1, static_cast<char32_t>(other));
    return PString(tmp);
}

PString PString::operator*(size_t n) const
{
    std::u32string newStr;
    newStr.reserve(_data.size() * n);
    for (size_t i = 0; i < n; i++)
    {
        newStr += _data;
    }
    return PString(newStr);
}

PString &PString::operator*=(size_t n)
{
    std::u32string newStr;
    newStr.reserve(_data.size() * n);
    for (size_t i = 0; i < n; i++)
    {
        newStr += _data;
    }
    _data = newStr;
    return *this;
}

// +=
PString &PString::operator+=(const PString &other)
{
    _data += other._data;
    return *this;
}

PString &PString::operator+=(char32_t other)
{
    _data.push_back(other);
    return *this;
}

PString &PString::operator+=(const char *other)
{
    *this = *this + PString(other);
    return *this;
}

PString &PString::operator+=(char other)
{
    _data.push_back(static_cast<char32_t>(other));
    return *this;
}

PString &PString::operator+=(const PAnyWrapper &other)
{
    *this += other.toString();
    return *this;
}

PString &PString::operator+=(const std::string &other)
{
    *this = *this + PString(other);
    return *this;
}

PIterator<PString> PString::begin()
{
    return _data.begin();
}
PIterator<PString> PString::end()
{
    return _data.end();
}

const PIterator<PString> PString::cbegin() const
{
    return _data.cbegin();
}

const PIterator<PString> PString::cend() const
{
    return _data.cend();
}

size_t PString::hash() const
{
    std::hash<std::u32string> hasher;
    return hasher(_data);
}

void PString::_print(std::ostream &os) const
{
    os << toStdString();
}

PString operator+(const char *lhs, const PString &rhs)
{
    return PString(lhs) + rhs;
}

PString operator+(const std::string &lhs, const PString &rhs)
{
    return PString(lhs) + rhs;
}




} // namespace sapy
