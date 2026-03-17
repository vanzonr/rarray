//
// rarrayio.h - I/O routines for runtime reference counted arrays.
//
// Copyright (c) 2013-2026  Ramses van Zon
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#ifndef HEADERSOURCES_RARRAYIO_H_  // EXCLUDE //
#define HEADERSOURCES_RARRAYIO_H_  // EXCLUDE //
#include <list>
#include <utility>
#include <string>
#include <array>
#include <functional>
#include "rarraytypes.h"
#include "rarray.h"  // EXCLUDE //

#ifndef RA_FORCE_static_inline
#define RA_FORCE_static_inline static
#endif

namespace ra {
namespace detail {

// We need to be able to get a reference in a pointer-to-pointer structure
// given indices.
//
// Deref<T,1>(T*   p, size_type* i) where i->{n1}        =  p[n1]
// Deref<T,2>(T**  p, size_type* i) where i->{n1,n2}     =  p[n1][n2]
// Deref<T,3>(T*** p, size_type* i) where i->{n1,n2,n3}  =  p[n1][n2][n3]
//...
template<typename T, rank_type R>
struct Deref {
    RA_FORCE_static_inline auto access(typename PointerArray<T, R>::type ptrarr,
                              const size_type* indices) -> T& {
        return Deref<T, R-1>::access(ptrarr[indices[0]-1], indices+1);
    }
};

template<typename T>
struct Deref<T, 1>  {  // R=1 is special
    RA_FORCE_static_inline auto access(typename PointerArray<T, 1>::type ptrarr,
                              const size_type* indices) -> T& {
        return ptrarr[indices[0]-1];
    }
};

// Convert a string to a value, needed for operator>> .
template<typename T>
struct StringToValue {
    RA_FORCE_static_inline void get(const std::string& input, T& output) {
        std::stringstream str(input);  // use streaming operators by default
        str >> output;
    }
};
template<>
struct StringToValue<std::string> {
    RA_FORCE_static_inline void get(const std::string& input, std::string& output) {
        output = input;
    }
};

enum class token : unsigned char { BRACEOPEN, BRACECLOSE, COMMA, DATASTRING, END };

inline auto toch(const token& Token) -> char {
    switch (Token) {
    case token::BRACEOPEN:  return '{';
    case token::BRACECLOSE: return '}';
    case token::COMMA:      return ',';
    case token::DATASTRING: return '$';
    case token::END:        return '.';
    // no default needed, but to silence the Intel compiler:
    default: return '\0';
    }
}

template<typename T, rank_type R>
inline auto text_output(std::ostream &out, const rarray<T, R>& rarr)
-> std::ostream& {
    if (!rarr.empty()) {
        out << "{\n";  // new newline
        for (size_type i = 0; i < rarr.extent(0); i++)  {
            if (i > 0) {
                out << ",\n";  // new newline
            }
            out << rarr.at(i);
        }
        out << "\n}";  // new newline
    } else {
        for (int i = 0; i < R; i++) {
            out << '{';
        }
        for (int i = 0; i < R; i++) {
            out << '}';
        }
        out << "\n";  // new newline
    }
    return out;
}

template<typename T>
inline auto text_output(std::ostream &out, const rarray<T, 1>& rarr)
-> std::ostream& {
    if (!rarr.empty()) {
        out << '{';
        for (size_type i = 0; i < rarr.extent(0); i++) {
            if (i > 0) {
                out << ',';
            }
            std::stringstream strstr;
            std::string result;
            const T& val = rarr.at(i);  // at() returns a rarray<T,0>, whose streaming operator is not refined. But we need the value only.
            strstr << val;
            result = strstr.str();
            if (result.find_first_of("{,}#") != std::string::npos
                && 
                (result[0] != '(' || result[result.size()-1] != ')'
                 || result.substr(1, result.size()-2).find_first_of(')') != std::string::npos) ) {
                out << '#' << result.size() << ':';
            }
            out << result;
        }
        out << '}';
    } else {
        out << "{}";
    }
    return out;
}

// helper routines to convert a string to any data type

RA_FORCE_static_inline auto get_but_eat_newline(std::istream& input) -> char {
    // helper function to read a character but omit leading and trailing newlines (not other whitespace).
    char ch1 = '\n';
    while (ch1 == '\n' && !input.eof()) {
        input >> ch1;  // eats white space unless noskipws is set
    }
    return ch1;
}

RA_FORCE_static_inline auto get_but_eat_whitespace(std::istream & input) -> char {
    // helper function to read a character but omit leading and trailing newlines (not other whitespace).
    char ch1;
    input >> ch1;
    return ch1;
}

template<rank_type R>
inline auto parse_shape(std::istream & input) -> std::pair<std::list<std::pair<token, std::string>>, std::array<size_type,R>> {
    std::pair<std::list<std::pair<token, std::string>>, std::array<size_type,R>> wholeresult;
    auto init_file_ptr = input.tellg();
    try {
        std::list<std::pair<token, std::string>>& result = wholeresult.first;
        size_type* shape = wholeresult.second.data();
        std::array<size_type,R> current_shape;
        for (rank_type dim = 0; dim < R; dim++) {
            current_shape[dim] = 1;
            shape[dim] = 0;
            if (get_but_eat_newline(input) != '{') { // eat
                throw std::istream::failure("Format error");
            } 
            result.emplace_back(token::BRACEOPEN, "");
        }
        int current_depth = R-1;  // start scanning the deepest level
        while (current_depth >= 0) {
            if (current_depth == R-1) {
                char         lastchar;
                std::string  word;
                do {
                    if (word.empty()) {
                        lastchar = get_but_eat_newline(input);
                    } else {
                        input.get(lastchar);
                    }
                    if (lastchar != ',' && lastchar != '}') {
                        word += lastchar;
                    }
                    if (word == "#") {
                        // special encoding, allowing for output that
                        // contains commands, newlines, and cruly braces
                        word = "";
                        std::string skipstr;
                        do {
                            input.get(lastchar);
                            skipstr += lastchar;
                        } while (lastchar != ':');
                        int skip = atoi(skipstr.c_str());  ////
                        for (int i = 0; i < skip; i++) {   ////
                            char nextchar;
                            input.get(nextchar);
                            word += nextchar;
                        }
                    } else if (word == "(") {
                        // another special encoding for output that
                        // starts with a opening parenthesis (, and
                        // end with a closing parenthesis ), and
                        // contain anything but a ).
                        const int safeguardcount = 1024*1024;  ////
                        int count = 0;  ////
                        while (lastchar != ')' && count < safeguardcount) {
                            input.get(lastchar);
                            word += lastchar;
                            count++;
                        }
                        input.get(lastchar);
                    }
                    if (lastchar == ',') {
                        result.emplace_back(token::DATASTRING, word);
                        result.emplace_back(token::COMMA, "");
                        word = "";
                        current_shape[current_depth]++;
                    }
                } while (lastchar != '}');
                result.emplace_back(token::DATASTRING, word);
                result.emplace_back(token::BRACECLOSE, "");
                if (shape != nullptr) {
                    if (shape[current_depth] < current_shape[current_depth]) {
                        shape[current_depth] = current_shape[current_depth];
                    }
                }
                current_depth--;
            } else {
                switch (get_but_eat_whitespace(input)) {
                   case ',':
                    result.emplace_back(token::COMMA, "");
                    current_shape[current_depth]++;
                    break;
                  case '{':
                    result.emplace_back(token::BRACEOPEN, "");
                    current_depth++;
                    current_shape[current_depth] = 1;
                    break;
                  case '}':
                    result.emplace_back(token::BRACECLOSE, "");
                    if (shape != nullptr) {
                        if (shape[current_depth] < current_shape[current_depth]) {
                              shape[current_depth] = current_shape[current_depth];
                        }
                    }
                    current_depth--;
                    break;
                  default:
                    throw std::istream::failure("Format error");
                }
            }
        }
        result.emplace_back(token::END, "");
    }
    catch (std::istream::failure& e) {
        input.seekg(init_file_ptr);  // upon failure, try to undo characters read in
        input.setstate(std::ios::failbit);   // and set the fail bit
        throw;
    }
    return wholeresult;
}

template<typename T, rank_type R>
inline void parse_strings(const std::pair<std::list<std::pair<token, std::string>>,
                          std::array<size_type,R>> & tokens,
                          typename PointerArray<T, R>::type ptrptr) {
    std::array<size_type,R> index;
    int current_depth = -1;
    for (auto& tokenpair : tokens.first) {
        switch (tokenpair.first) {
        case token::BRACEOPEN:
            current_depth++;
            index[current_depth] = 1;
            break;
        case token::BRACECLOSE:
            current_depth--;
            break;
        case token::COMMA:
            index[current_depth]++;
            break;
        case token::DATASTRING:
            StringToValue<T>::get(tokenpair.second, Deref<T, R>::access(ptrptr, index.data()));
            break;
        case token::END:
            break;
            // no "default:" needed as the above cases are the exhaustive possibilities for an enum class token.
        }
        if (tokenpair.first == token::END) {
            break;
        }
    }
}

}  // namespace detail

template<typename T, rank_type R>
inline auto operator<<(std::ostream &outstream, const rarray<T, R>& rarr) -> std::ostream& {
    return detail::text_output(outstream, rarr);
}

template<typename T, rank_type R>
inline auto operator>>(std::istream &instream, rarray<T, R>& rarr) -> std::istream& {
    auto parsed = detail::parse_shape<R>(instream);
    size_type* parsed_extent = parsed.second.data();
    if (std::accumulate(parsed_extent, parsed_extent+R, 1, std::multiplies<size_type>())
        <= rarr.size()) {
        rarr.reshape(parsed_extent, RESIZE::ALLOWED);
    } else {
        rarr = rarray<T, R>(parsed_extent);
    }
    detail::parse_strings<T, R>(parsed, rarr.ptr_array());
    return instream;
}

}  // namespace ra

// add support for std::format (C++20) and std::print (C++23)

#if __cplusplus >= 202002L

#if __has_include(<format>)

#include <format>

namespace std {
template<typename T, ra::rank_type R>
struct formatter<ra::rarray<T,R>> : formatter<string> {
    template<typename CTX>
    auto format(const ra::rarray<T,R>& r, CTX& ctx) const {
        stringstream s;
        s << r;
        return formatter<string>::format(s.str(), ctx);
    }
};
}

#endif

#endif

#endif // EXCLUDE //
