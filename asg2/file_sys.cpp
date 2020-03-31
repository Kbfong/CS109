// $Id: file_sys.cpp,v 1.6 2018-06-27 14:44:57-07 - - $
/*
This program was completed using pair programming.
Partner:  Kody Fong (kbfong@ucsc.edu)
Partner:  Andy Huang (ahuang44@ucsc.edu)
*/
#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
  << ", prompt = \"" << prompt() << "\"");
   //creates a inode that is of type directory
  this->root = make_shared<inode>(file_type::DIRECTORY_TYPE); 
  //the parent root is equal to the root since its the initial root
  this->root->parent = root; 
  //setdefaultdir=dirInit.set the contents initially to "." & ".."
  this->root->contents->dirInit(root); 
  //sets the name of the directory to "/" because that is the root
  this->root->setName("/"); 
  //the current working directory should be the root
  this-> cwd = root; 
  
}

const string& inode_state::prompt() const { return prompt_; }

//void inode_state::set_prompt(const string& newPrompt){
 // prompt_ = newPrompt; //sets the prompt
//}
// function to update the cwd
//void inode_state::set_cwd(inode_ptr inputDir){
  //this->cwd = inputDir;
//}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}



file_error::file_error (const string& what):
            runtime_error (what) {
}
//plain_file
size_t plain_file::size() const {
  int size = 0;
  //iterate through data 
  for(unsigned int i =0; i< data.size(); i++){
    //add the length of every string in data to size counter
    string temp=data[i];
    size += temp.length();
  }
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   this->data.clear();
   this->data = words;
}

void plain_file::remove (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkfile (const string&) {
   throw file_error ("is a plain file");
}

void plain_file::insertDirent(inode_ptr, string){
   throw file_error ("is a plain file");
}
inode_ptr plain_file::findDirent(string){
  throw file_error ("is a plain file");
  }

void plain_file::dirInit(inode_ptr){
   throw file_error ("is a plain file");
}

void plain_file::setDir(inode_ptr, inode_ptr){
   throw file_error ("is a plain file"); 
}

//map<string,inode_ptr>& plain_file::getDirents() {
 // throw file_error ("is a plain file");
//} 

void plain_file::printFile(){
  string toPrint;
  for(unsigned int i = 0; i< data.size(); i++){
    //check if last thing in data
    //if it is, add data[i] but no space after else with a space
    if(i == this->data.size()-1){
      toPrint += this->data[i];      
    }
    else{
      toPrint += this->data[i] + " ";
    }
  }
  //print it out
  cout << toPrint << endl;
}

void plain_file::printDirents(inode_ptr, string) {
   throw file_error ("is a plain file");
} 



//directory

size_t directory::size() const {
   //DEBUGF ('i', "size = " << size);
   return dirents.size();
}

const wordvec& directory::readfile() const {
   throw file_error ("is a directory");
}

void directory::writefile (const wordvec&) {
   throw file_error ("is a directory");
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   return nullptr;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   return nullptr;
}

void directory::insertDirent(inode_ptr dir, string name){
  map<string, inode_ptr>::iterator i = dirents.find(name);
  if(i==dirents.end()){
    //not found, insert
    dirents.insert(pair<string, inode_ptr>(name,dir));
  }else{
    //found thus delete then insert new
    dirents.erase(i);
    dirents.insert(pair<string, inode_ptr>(name,dir));
  }
}

//setup initial state of directories "." and ".."
void directory::dirInit(inode_ptr dir){
  insertDirent(dir, ".");
  insertDirent(dir->getParent(), "..");
}

void directory::setDir(inode_ptr cwd, inode_ptr parentDir){
  map<string, inode_ptr>::iterator i = dirents.begin();
  i->second = cwd;
  i++;
  i->second = parentDir;
}

void directory::printFile(){
  throw file_error ("is a directory");
}

void directory::printDirents(inode_ptr currDir, string path) {
  //check if directory we are working on is the root "/"
   if(currDir->getName()=="/"){
     //print "/:"
     cout << "/:" << endl;
   }else{
     //else print path followed by a colon 
     cout << path << ":" << endl;
   }
  map<string, inode_ptr>::iterator i;
  for(i = dirents.begin(); i != dirents.end(); i++){
    cout << "   " << i->second->get_inode_nr() << "   " <<   
        i->second->getBase()->size() << "    " << i->first << endl;
  }
}


inode_ptr directory::findDirent(string name){

  map<string, inode_ptr>::iterator i = dirents.find(name);
  if(i == dirents.end()){
    return nullptr;

  }
  else{

    return i->second;
  }
  

}
