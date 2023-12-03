//
// rarrayio.h - I/O routines for runtime reference counted arrays.
//
// Copyright (c) 2013-2023  Ramses van Zon
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

#include "rarraytypes.h"

namespace ra {    
namespace detail {
        
// We need to be able to get a reference in a pointer-to-pointer structure given indices.
//
// Deref<T,1>(T*   p, size_type* i) where i->{n1}        =  p[n1]
// Deref<T,2>(T**  p, size_type* i) where i->{n1,n2}     =  p[n1][n2]
// Deref<T,3>(T*** p, size_type* i) where i->{n1,n2,n3}  =  p[n1][n2][n3]
//...
template<typename T, rank_type R>
struct Deref {
    static inline auto access(typename PointerArray<T,R>::type p, const size_type* indices) -> T&
    {
        return Deref<T,R-1>::access(p[indices[0]-1], indices+1);
    }
};
template<typename T>
struct Deref<T,1>  // R=1 is special
{
    static inline auto access(typename PointerArray<T,1>::type p, const size_type* indices) -> T&
    {
        return p[indices[0]-1];
    }
};

// Convert a string to a value, needed for operator>> .
template<typename T>
struct StringToValue {
    static inline void get(const std::string& input, T& output) {
        std::stringstream str(input); // use streaming operators by default
        str >> output; // won't work with strings as they get truncated at first whitespace
    }
};
template<>
struct StringToValue<std::string> {
    static inline void get(const std::string& input, std::string& output) {
        output = input;
    }
};

enum class token { BRACEOPEN, BRACECLOSE, COMMA, DATASTRING, END };

inline auto toch(const token& Token) -> char {
     switch (Token) {
     case token::BRACEOPEN:  return '{';
     case token::BRACECLOSE: return '}';
     case token::COMMA:      return ',';
     case token::DATASTRING: return '$';
     case token::END:        return '.';
     // no default needed, cases are extensive, but to silence the Intel compiler:
     default: return '\0';
     }
 }

template<typename T, rank_type R> 
inline auto text_output(std::ostream &o, const rarray<T,R>& r) -> std::ostream&
{
    if (not r.empty()) {
        o << "{\n"; // new newline
        for (size_type i=0; i<r.extent(0); i++)  {
            if (i>0)
                o << ",\n"; // new newline
            o << r.at(i);
        }
        o << "\n}"; // new newline
    } else {
        for (int i=0; i<R; i++) 
            o << '{';
        for (int i=0; i<R; i++) 
            o << '}';
        o << "\n"; // new newline
    }
    return o;
}

template<typename T> 
inline auto text_output(std::ostream &o, const rarray<T,1>& r) -> std::ostream&
{
    if (not r.empty()) {
        o << '{';
        for (size_type i=0; i<r.extent(0); i++) {
            if (i) o << ',';
            std::stringstream strstr;
            std::string result;
            const T& val = r.at(i); // at() returns a rarray<T,0>, whose streaming operator is not refined. But we need the value only.
            strstr << val;
            result = strstr.str();
            if (result.find_first_of("{,}#") != std::string::npos
                and not
                (result[0]=='(' and result[result.size()-1]==')' and result.substr(1,result.size()-2).find_first_of(")") == std::string::npos) )
                o << '#' << result.size() << ':';
            o << result;
        }
        o << '}';
    } else 
        o << "{}";
    return o;
}

// helper routines to convert a string to any data type

static inline auto get_but_eat_newline(std::istream & in) -> char
{
    // helper function to read a character but omit leading and trailing newlines (not other whitespace).    
    char ch1='\n';
    while (ch1=='\n' && ! in.eof())
        in >> ch1; // eats white space unless noskipws is set
    return ch1;
}

static inline auto get_but_eat_whitespace(std::istream & in) -> char
{
    // helper function to read a character but omit leading and trailing newlines (not other whitespace).
    char ch1;
    in >> ch1;
    return ch1;
}

template<rank_type R> 
inline auto parse_shape(std::istream & in) -> std::pair<std::list<std::pair<token,std::string>>,size_type[R]>
{
    std::pair<std::list<std::pair<token,std::string>>,size_type[R]> wholeresult;
    std::list<std::pair<token,std::string>>& result = wholeresult.first;
    size_type* shape = wholeresult.second;
    size_t init_file_ptr = in.tellg();
    try {
        size_type current_shape[R];
        for (rank_type i=0; i<R; i++) {
            current_shape[i] = 1;
            shape[i] = 0;
            if (get_but_eat_newline(in) != '{')  // eat
                throw std::istream::failure("Format error");
            else
                result.push_back({token::BRACEOPEN,""});
        }
        int current_depth = R-1; // start scanning the deepest level
        while (current_depth>=0) {
            if (current_depth==R-1) {
                char         lastchar;
                std::string  word = "";
                do {
                    if (word=="") {
                        lastchar = get_but_eat_newline(in);
                    } else {
                        in.get(lastchar);
                    }
                    if (lastchar != ',' and lastchar != '}') {
                        word += lastchar;
                    }
                    if (word == "#") {
                        // special encoding, allowing for output that
                        // contains commands, newlines, and cruly braces
                        word="";
                        std::string skipstr;
                        do {
                            in.get(lastchar);
                            skipstr += lastchar;
                        } while (lastchar!=':');
                        int skip = atoi(skipstr.c_str());////
                        for (int i=0; i<skip; i++) { ////
                            char nextchar;
                            in.get(nextchar);
                            word += nextchar;
                        }
                    } else if (word == "(") {
                        // another special encoding for output that
                        // starts with a opening parenthesis (, and
                        // end with a closing parenthesis ), and
                        // contain anything but a ).
                        const int safeguardcount=1024*1024; ////
                        int count=0; ////
                        while (lastchar!=')' and count<safeguardcount) {
                            in.get(lastchar);
                            word += lastchar;
                            count++;
                        }
                        in.get(lastchar);
                    }
                    if (lastchar == ',') {
                        result.push_back({token::DATASTRING,word});
                        result.push_back({token::COMMA,""});
                        word="";
                        current_shape[current_depth]++;
                    }
                } while (lastchar != '}');                
                result.push_back({token::DATASTRING,word});
                result.push_back({token::BRACECLOSE,""});
                if (shape)
                    if (shape[current_depth] < current_shape[current_depth])
                        shape[current_depth] = current_shape[current_depth];
                current_depth--;
            } else {
                switch (get_but_eat_whitespace(in)) { 
                   case ',':
                    result.push_back({token::COMMA,""});
                    current_shape[current_depth]++;
                    break;
                  case '{':
                    result.push_back({token::BRACEOPEN,""});
                    current_depth++;
                    current_shape[current_depth] = 1;
                    break;
                  case '}':
                    result.push_back({token::BRACECLOSE,""});
                    if (shape)
                          if (shape[current_depth] < current_shape[current_depth])
                              shape[current_depth] = current_shape[current_depth];
                    current_depth--;
                    break;
                  default:
                    throw std::istream::failure("Format error");
                }
            }
        }       
        result.push_back({token::END,""});
    }
    catch (std::istream::failure& e) {
        in.seekg(init_file_ptr, in.beg);// upon failure, try to undo characters read in
        in.setstate(std::ios::failbit); // and set the fail bit
        throw;
    }

    return wholeresult;
}

template<typename T, rank_type R> 
inline void parse_strings(const std::pair<std::list<std::pair<token,std::string>>,size_type[R]> & tokens, typename PointerArray<T,R>::type p) 
{
    size_type index[R];
    int current_depth = -1;
    for (auto& tokenpair: tokens.first) {
        switch (tokenpair.first) {
        case token::BRACEOPEN:
            current_depth++;
            index[current_depth]=1;
            break;
        case token::BRACECLOSE:
            current_depth--;
            break;
        case token::COMMA:
            index[current_depth]++;
            break;
        case token::DATASTRING:
            StringToValue<T>::get(tokenpair.second, Deref<T,R>::access(p, index));
            break;
        case token::END:
            break;
            // no "default:" needed as the above cases are the exhaustive possibilities for an enum class token.
        }
        if (tokenpair.first == token::END) break;
    }
}

}

template<typename T, rank_type R> 
inline auto operator<<(std::ostream &o, const rarray<T, R>& r) -> std::ostream&
{
    if (R>1) {
        return detail::text_output(o,r);
    } else if (R==1) {
        if (not r.empty()) {
            o << "{"; 
            for (size_type i=0; i<r.extent(0); i++)  {
                if (i>0)
                    o << ",";
                o << r.data()[i];
            }
            o << "}"; 
        }
    }
    return o;
}

template<typename T,rank_type R> 
inline auto operator>>(std::istream &in, rarray<T,R>& r) -> std::istream&
{
    auto X = detail::parse_shape<R>(in);
    size_type* extent = X.second;    
    if (std::accumulate(extent,extent+R,1,std::multiplies<size_type>())
        <= r.size())
        r.reshape(extent, RESIZE::ALLOWED);
    else
        r = rarray<T,R>(extent);
    detail::parse_strings<T,R>(X, r.ptr_array());
    return in;
}

}

