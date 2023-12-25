// hardinclude.cc
//
// Replaces #include's in a file with those files, if they are in the
// file set specified on the command lines. Works recursively, i.e.,
// #include's in the included files are resolved as well. But each
// of the specificed header file is only included once.
// Writes result to standard out.
//
// Usage:
//   hardinclude [-c] INPUTFILE INCLUDEFILE1 [INCLUDEFILE2 ...]
//
// Copyright (c) 2017-2022  Ramses van Zon
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
#include <vector>

#ifndef ADDEDCOMMENTS
#define NOADDEDCOMMENTS
#endif

using std::string;
using std::vector;
using std::ifstream;
using std::istream;
using std::cin;
using std::cerr;
using std::cout;
using std::endl;

// check if a file exists
bool file_exists(const string& file_name) {
    return ifstream(file_name).is_open();
}

// check if string is one of a set of strings
const size_t NOTFOUND = -1;
size_t string_in_set(const string& str, const vector<string>& strset) {
    for (size_t i = 0; i < strset.size(); i++)
        if (strset[i] == str)
            return i;
    return NOTFOUND;
}


// check if a line is just a comment
bool iscommentline(const std::string& line) {
    size_t i = 0;
    while (i < line.size() && line[i] == ' ')
        i++;
    return line.substr(i, 2) == std::string("//");
}

// find the end of the code line, not counting trailing comments or spaces
// note: not prepared for quoted strings
size_t endofcodeline(const std::string& line) {
    size_t end;
    if (iscommentline(line)) {
        end = 0;
    } else {
        size_t lastslashi = line.size()-1;
        while (lastslashi > 0 && line[lastslashi] != '/')
            lastslashi--;
        if (lastslashi == 0 || line[lastslashi] != '/' || line[lastslashi-1] != '/'
            || line.find("  // namespace") != std::string::npos) {
            end = line.size();
        } else {
            end = lastslashi-1;
        }
    }
    // discard trailing spaces
    while (end > 0 && (line[end-1] == ' ' || line[end-1] == '\t')) end--;
    return end;
}

// trim whitespace
string trimline(const string& line) {
    size_t i = 0;
    while (i < line.size() && line[i] == ' ')
        i++;
    return line.substr(i);
}

// to process one file (allowing for recursive application)
void process_one_file(const string& inputfile, const vector<string>& includefiles,
                      vector<bool>& alreadyincluded, bool& headercommentsdone,
                      const vector<string>& systemincludefiles);

// start
int main(int argc, char** argv) {
    string         inputfile;
    vector<string> includefiles;
    vector<bool>   alreadyincluded(argc-2, false);
    bool           headercommentsdone = false;
    vector<string> systemincludefiles;

    // check command line arguments:
    if (argc < 3) {
        cerr << "ERROR: Not enough arguments\n"
                  << " Usage: " << argv[0]
                  << " inputfile includefile1 includefile2 ..."
                  << endl;
        return 1;
    } else {
        if (((argv[1][0] != '-') || (argv[1][1] != '\0')) && (!file_exists(argv[1]))) {
            cerr << "ERROR: File " << argv[1]
                 << " does not exist or is not readable." << endl;
            return 1;
        }
        for (int i = 2; i < argc; i++) {
            if (!file_exists(argv[i])) {
                cerr << "ERROR: File " << argv[i]
                          << " does not exist or is not readable." << endl;
                return 1;
            }
        }
        inputfile = argv[1];
        for (auto filenameptr=argv+2; filenameptr < argv+argc; filenameptr++) {
            includefiles.push_back(*filenameptr);
            std::ifstream f(*filenameptr);
            while (f.good() && !f.eof()) {
                string line;
                getline(f, line);
                if (line.substr(0, 10) == "#include <")
                    if (string_in_set(line, systemincludefiles) == NOTFOUND)
                        systemincludefiles.push_back(line);
            }
            f.close();
        }
    }

    // start processing:
    process_one_file(inputfile, includefiles, alreadyincluded, headercommentsdone, systemincludefiles);
}

void process_one_file(const string& inputfile, const vector<string>& includefiles,
                      vector<bool>& alreadyincluded, bool& headercommentsdone,
                      const vector<string>& systemincludefiles) {
    static const string INCLUDETAG("#include");
    ifstream infile(inputfile);  // note: doesn't throw on failuer e.g with inputfile=="-"
    istream& in = (inputfile == "-")?cin:infile;
    int insertline = -1;
    while (in.good() && !in.eof()) {
        // read in a line from the file
        string line;
        getline(in, line);
        if (!headercommentsdone && !iscommentline(line)) {
            insertline = 3;  // assumes a guard header
            headercommentsdone = true;
        }
        if (headercommentsdone && iscommentline(line))
            continue;
        auto include = NOTFOUND;
        string includefilename;
        // find occurance of "#include"
        size_t pos = line.find(INCLUDETAG);
        size_t continueat = 0;
        if (pos != string::npos) {
            // check if there are only spaces before it
            bool only_leading_space = true;
            for (size_t i = 0; i < pos; i++)
                if (line[i] != ' ' && line[i] != '\t') {
                    only_leading_space = false;
                    break;
                }
            if (only_leading_space) {
                pos += INCLUDETAG.size();
                while (pos < line.length() && (line[pos] == ' ' || line[pos] == '\t'))
                    pos++;
                if (line[pos] == '"') {  // || line[pos] == '<') {
                    char endchar = (line[pos] == '"')?'"':'>';
                    pos++;
                    continueat = line.find(endchar, pos+2);
                    if (continueat != string::npos) {
                        includefilename = string(&line[pos], &line[continueat]);
                        include = string_in_set(includefilename, includefiles);
                        if (line[continueat] == '"') continueat++;
                    }
                }
            }
        }
        if (include != NOTFOUND) {
          if (!alreadyincluded[include]) {
            #ifndef NOADDEDCOMMENTS
            if (continueat != line.size())
            cout << "// " << line.c_str()+continueat << "\n";
            cout << "//begin " << INCLUDETAG << " \"" << includefilename << "\"\n";
            #endif
            process_one_file(includefilename, includefiles, alreadyincluded, headercommentsdone, systemincludefiles);
            #ifndef NOADDEDCOMMENTS
            cout << "//end " << INCLUDETAG << " \"" << includefilename << "\"\n\n";
            #endif
            alreadyincluded[include] = true;
          } else {
            #ifndef NOADDEDCOMMENTS
            if (continueat != line.size())
            cout << "// " << line.c_str()+continueat << "\n";
            cout << "//" << INCLUDETAG << " \"" << includefilename << "\" was already done above\n";
            #endif
          }
        } else {
            auto tohere = endofcodeline(line);
            if (!iscommentline(line)) {
                if (line.size() < 12 || tohere < 10 || line.substr(tohere-10, 10) != "// EXCLUDE") {
                    if (trimline(line.substr(0, tohere)) != "") {
                        if (string_in_set(line, systemincludefiles) == NOTFOUND) {
                            if (insertline > 0) {
                                insertline--;
                                if (insertline == 0)
                                    for (const auto& sysline : systemincludefiles)
                                        cout << sysline << '\n';
                            }
                            cout << line.substr(0, tohere) << '\n';
                        }
                    }
                }
            } else {
                cout << line << '\n';
            }
        }
    }
}
