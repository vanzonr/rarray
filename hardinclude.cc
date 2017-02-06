// hardinclude.cc
//
// Replaces #include's in a file with those files, if they are in the
// file set specified on the command lines.  Writes result to standard
// out.
//
// Usage:
//   hardinclude INPUTFILE INCLUDEFILE1 [INCLUDEFILE2 ...]
//
// Copyright (c) 2017  Ramses van Zon
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

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cstring>

using namespace std;

// check if a file exists
bool file_exists(const char* file_name)
{
    assert(file_name);
    return ifstream(file_name).is_open();
}

// check if string is one of a set of strings
bool string_in_set(const char* str, char** strset, size_t nset)
{
    assert(str);
    assert(strset);
    const size_t n = strlen(str);
    for (size_t i = 0; i < nset; i++) {
        assert(strset[i]);
        if (strcmp(str,strset[i]) == 0) {
            return true;
        }
    }
    return false;
}

// dump a file to console
void dump(const char* s)
{
    ifstream f(s);
    while (f.good() and !f.eof()) {
        string s;
        getline(f,s);
        std::cout << s << '\n';
    }   
}

// start 
int main(int argc, char** argv)
{
    char*  inputfile;
    char** includefiles;
    
    // check command line arguments:
    if (argc<3) {
        cerr << "ERROR: Not enough arguments\n"
                  << " Usage: " << argv[0]
                  << " inputfile includefile1 includefile2 ..."
                  << endl;
        return 1;
    } else {
        for (int i = 1; i < argc; i++) {
            if (not file_exists(argv[i])) {
                cerr << "ERROR: File " << argv[i]
                          << " does not exist or is not readable." << endl;
                return 1;                
            }
        }
        inputfile = argv[1];
        includefiles = argv+2;
    }

    // start processing:
    ifstream in(inputfile);

    while (in.good() && !in.eof()) {
        // read in a line from the file
        string line;
        getline(in, line);
        bool include = false;
        string includefilename;
        // find occurance of "#include"        
        size_t pos = line.find("#include");
        size_t endpos = 0;
        if (pos != string::npos) {
            // check if there are only spaces before it
            bool only_leading_space = true;
            for (size_t i = 0; i < pos; i++) {
                if (line[i]!=' ' && line[i]!='\t') {
                    only_leading_space = false;
                    break;
                }
            }
            if (only_leading_space) {
                size_t posend = line.length();
                pos += strlen("#include");
                while (pos < posend && (line[pos] == ' ' || line[pos] == '\t'))
                    pos++;
                if (line[pos] == '"') { // || line[pos] == '<') {
                    char endchar = (line[pos] == '"')?'"':'>';
                    pos ++;
                    endpos = line.find(endchar, pos+2);
                    if (endpos != string::npos) {
                        includefilename = string(&line[pos],&line[endpos]);
                        include = string_in_set(includefilename.c_str(),includefiles,argc-2);
                        if (line[endpos]=='"') endpos++;
                    }
                }
            }
        }
        if (include) {
            cout << "// " << line.c_str()+endpos << "\n";
            cout << "//#include \"" << includefilename << "\"\n";
            dump(includefilename.c_str());
            cout << "//end of #include \"" << includefilename << "\"\n";
        } else {
            cout << line << '\n';
        }
    }
    return 0;
}
