// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $


/*

This program was completed using pair programming.
Partner:  Kody Fong (kbfong@ucsc.edu)
Partner:  Andy Huang (ahuang44@ucsc.edu)
*/
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cassert>
#include <regex>
#include <fstream>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;
const string cin_name = "-";

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}
//matchlines.cpp
void parseFile(const string &infilename, istream &infile, 
    str_str_map& listmap, str_str_map::iterator& iter){
    regex comment_regex {R"(^\s*(#.*)?$)"};
    regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
    regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};
    //counter for line number to be printed
    int lineNum=0;
    for (;;) {
        lineNum++;
        string line;
        getline (infile, line);
        if (infile.eof()) break;
        //print with proper format for file name, 
        //line# and the instruction
        cout << infilename << ": " << lineNum<< ": " << line << endl;
        smatch result;
        if (regex_search (line, result, comment_regex)) {
            //dont do anything for comments
            continue;
        }
        if (regex_search (line, result, key_value_regex)) {
            //result holds key and value that are captured by regex
            // if both captures are empty, there is only a "="
            if(result[1] == "" and result[2]==""){
                //start at the top and loop until end
                iter=listmap.begin();
                while(iter!=listmap.end()){
                    //print all keys and values in "key = value" format
                    cout << iter->first <<" = "<< iter->second <<endl;
                    ++iter;
                }
            }
            //if there is only a key captured "key = "
            else if(result[2]==""){
                //find the key. if == end(), not found
                if(listmap.find(result[1]) != listmap.end()){
                    //delete at position where we found result[1]
                    listmap.erase(listmap.find(result[1]));
                }
            }
            //if there is only a value captured " = value"
            else if(result[1]==""){
                //loop through listmap
                iter = listmap.begin();
                while(iter != listmap.end()){
                    //whenever we find the value, 
                    //print out key value pair.
                    if(iter->second==result[2]){
                        cout<<iter->first<<" = "<< iter->second<<endl;
                    }
                    ++iter;
                }
            }
            //key and value
            else{
                //print "key = value"
                cout << result[1] << " = " << result[2] << endl;
                //create a new pair (key,value) and insert into listmap
                str_str_pair newPair(result[1],result[2]);
                listmap.insert(newPair);
            }
                
        }else if (regex_search (line, result, trimmed_regex)) {
            //no equals sign thus looking up a key
            str_str_map::iterator search=listmap.find(result[1]);
            //if search != end we can print the found key value pair
            if(search!=listmap.end()){
                cout << result[1] << " = " << search->second << endl;
            }
            else{
                //key was not found
                cout << result[1] << ": key not found" << endl;
            }
            
        }else {
            assert (false and "This can not happen.");
        }
    }   
    
    
    
}
// based off catfile.cpp 
// main calls method parseFile like catfile's main calls method catfile 
// all listmap operations and printing are done in parseFile subroutine
// main reads files based on args and passes it on
int main (int argc, char** argv) {
    sys_info::execname (argv[0]);
    scan_options (argc, argv);
    str_str_map listmap;
    str_str_map::iterator iter;
    int status =0;
    string progname (basename (argv[0]));
    vector<string> filenames (&argv[1], &argv[argc]);
    if (filenames.size() == 0) filenames.push_back (cin_name);
    for (const auto& filename: filenames) {
        if (filename == cin_name) {
            parseFile(filename,cin,listmap,iter);
        }
        else {
            ifstream infile (filename);
            if (infile.fail()) {
                status = 1;
                cerr << progname << ": " << filename << ": " 
                << strerror (errno) << endl;
            }else {
                parseFile(filename, infile, listmap, iter);
                infile.close();
            }
        }
    }
    
   return status;
}

