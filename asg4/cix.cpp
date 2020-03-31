// $Id: cix.cpp,v 1.9 2019-04-05 15:04:28-07 - - $


/*

This program was completed using pair programming.
Partner:  Kody Fong (kbfong@ucsc.edu)
Partner:  Andy Huang (ahuang44@ucsc.edu)
*/


#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog (cout);
struct cix_exit: public exception {};

//added get,put and rm
unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"get", cix_command::GET},
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS  },
   {"put"  , cix_command::PUT  },
   {"rm"  , cix_command::RM  },
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cix_help() {
   cout << help;
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      auto buffer = make_unique<char[]> (header.nbytes + 1);
      recv_packet (server, buffer.get(), header.nbytes);
      outlog << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer.get();
   }
}

void cix_rm(client_socket& server, string filename){
    
    cix_header header;
    snprintf(header.filename, filename.length() + 1, filename.c_str());
    header.command = cix_command::RM;
    header.nbytes = 0;
    outlog << "sending header" << header << endl;
    send_packet(server, &header, sizeof header);
    recv_packet(server, &header, sizeof header);
    outlog << "received header" << header << endl;
    
    if(header.command == cix_command::ACK){
        outlog << "sent RM, successfully deleted file: " <<
        filename << endl;
        
    }
    if(header.command == cix_command::NAK){
        
        outlog << "sent RM, server did not return ACK" << endl;
        outlog << "server returned " << header << endl;
        outlog << "unable to delete file: " << filename << endl;
        
    }
    
}


void cix_get(client_socket& server, string filename){

    cix_header header;
    snprintf(header.filename, filename.length() + 1, filename.c_str());
    header.command = cix_command::GET;
    header.nbytes = 0;
    outlog << "sending header" << header << endl;
    send_packet(server, &header, sizeof header);
    recv_packet(server, &header, sizeof header);
    outlog << "received header" << header << endl;
    
    if(header.command != cix_command::FILEOUT){
    
        outlog << filename << "is not on the server" << endl;
        outlog << "server returned " << header << endl;
        
    }
    else{
        
        auto buffer = make_unique<char[]> (header.nbytes + 1);
        recv_packet (server, buffer.get(), header.nbytes);
        outlog << "received " << header.nbytes << " bytes" << endl;
        buffer[header.nbytes] = '\0';
        cout << buffer.get();
        
        std::ofstream out(header.filename, std::ofstream::binary);
        out.write(buffer.get(), header.nbytes);
        out.close();
    }
    
    
}

//used www.cplusplus.com/reference/istream/istream/seekg/
//as a reference 
void cix_put(client_socket& server, string filename){
    
    cix_header header;
    snprintf(header.filename, filename.length()+1, filename.c_str());
    ifstream in(filename, std::ifstream::binary);
    
    //if a valid file
    if(in) {
        
        in.seekg(0,in.end);
        int length = in.tellg();
        in.seekg(0, in.beg);
        char *buffer = new char [length];
        in.read(buffer,length);
        
        header.command = cix_command::PUT;
        header.nbytes = length;
        send_packet (server, &header, sizeof header);
        send_packet (server, buffer, length);
        recv_packet (server, &header, sizeof header);
        
    }
    //file failure
    else{
        
        outlog << "sent PUT, server did not return ACK" << endl;
        outlog << "server returned " << header << endl;
        outlog << filename 
        << " was not put on server successfully" << endl; 
    }
    
    if(header.command == cix_command::ACK){
        outlog<< "ACK received: " << filename << " put on server" 
        << endl;
        
        
    }
    
    if(header.command == cix_command::NAK){
        
        outlog<< "NAK received: " << filename << 
        "can't be put on server" << endl;
    }
    
    in.close();
    
}



void usage() {
   cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   vector<string> lineTokens;
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   outlog << to_string (hostinfo()) << endl;
   try {
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         //line splitting to get filename
         string t;
         istringstream in(line);
         while (getline(in, t, ' ')) {
            lineTokens.push_back(t);
        }

         outlog << "command " << line << endl;
         const auto& itor = command_map.find (lineTokens[0]);
         cix_command cmd = itor == command_map.end()
                         ? cix_command::ERROR : itor->second;
         switch (cmd) {
            case cix_command::EXIT:
               throw cix_exit();
               lineTokens.clear();
               break;
            case cix_command::HELP:
               cix_help();
               lineTokens.clear();
               break;
            case cix_command::LS:
               cix_ls (server);
               lineTokens.clear();
               break;
            case cix_command::PUT:
               cix_put (server, lineTokens[1]);
               lineTokens.clear();
               break;
            case cix_command::GET:
               cix_get (server, lineTokens[1]);
               lineTokens.clear();
               break;
            case cix_command::RM:
               cix_rm (server, lineTokens[1]);
               lineTokens.clear();
               break;
            default:
               outlog << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cix_exit& error) {
      outlog << "caught cix_exit" << endl;
   }
   outlog << "finishing" << endl;
   return 0;
}

