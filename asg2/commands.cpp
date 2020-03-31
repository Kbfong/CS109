// $Id: commands.cpp,v 1.17 2018-01-25 14:02:55-08 - - $


/*

This program was completed using pair programming.
Partner:  Kody Fong (kbfong@ucsc.edu)
Partner:  Andy Huang (ahuang44@ucsc.edu)
*/
#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

void fn_cat (inode_state& state, const wordvec& words){
  
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
  int exitStatus = 0;
         if(words.size() > 1){
        
      
      string exitToken = words.at(1);
      for (unsigned int i = 0; i < exitToken.size(); i++) {
         if (!isdigit(exitToken.at(i))){
           
            exitStatus = 127;
            break;
         }
      }
        if(exitStatus != 127){
          exitStatus = stoi(exitToken);
        }
        
    }
      
      exit_status::set(exitStatus);
      //inode_ptr root = state.getRoot();
     // root->getBase()->removeAll();
      throw ysh_exit();
}



void fn_ls (inode_state& state, const wordvec& words){
  
    inode_ptr currDir = state.getCwd();
   if (words.size() <= 1) {
      currDir->getBase()->printDirents(currDir, currDir->getName());
   }
   else {
      inode_ptr root = state.getRoot();
      for (size_t i = 1; i < words.size(); i++) {
         wordvec pathName = split(words.at(i), "/");
         string path = words.at(i);
         inode_ptr temp = currDir;
         for (size_t j = 0; j < pathName.size(); j++) {
            inode_ptr valid =
               temp->getBase()->findDirent(pathName[j]);
            if (valid == nullptr) {
               throw command_error("ls Error: "
                  + pathName[j] + " does not exist");
               return;
            }
            if (!valid->isFile ) {
               throw command_error("ls Error: "
                  + pathName[j] + " is not a directory");
               return;
            }
            temp = valid;
         }
         temp->getBase()->printDirents(temp, path);
      }
   }
  
   DEBUGF ('c', state);
   DEBUGF ('c', words);
  
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words){
    
   string filePrompt;

   if(words.size() > 1){
     
   for(unsigned int i = 1; i < words.size(); i++){
         
     filePrompt += words.at(i) + " ";
  }
   
   state.inode_state::setPrompt(filePrompt);
}

   
      
     
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words){
   cout << state.getCwd()->getName() << endl;
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

