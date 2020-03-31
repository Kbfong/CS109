// $Id: cixd.cpp,v 1.8 2019-04-05 15:04:28-07 - - $

/*

This program was completed using pair programming.
Partner:  Kody Fong (kbfong@ucsc.edu)
Partner:  Andy Huang (ahuang44@ucsc.edu)
*/



#include <iostream>
#include <fstream>      // std::ifstream
#include <string>
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




void reply_ls (accepted_socket& client_sock, cix_header& header) {
   const char* ls_cmd = "ls -l 2>&1";
   FILE* ls_pipe = popen (ls_cmd, "r");
   if (ls_pipe == NULL) { 
      outlog << "ls -l: popen failed: " << strerror (errno) << endl;
      header.command = cix_command::NAK;
      header.nbytes = errno;
      send_packet (client_sock, &header, sizeof header);
      return;
   }
   string ls_output;
   char buffer[0x1000];
   for (;;) {
      char* rc = fgets (buffer, sizeof buffer, ls_pipe);
      if (rc == nullptr) break;
      ls_output.append (buffer);
   }
   int status = pclose (ls_pipe);
   if (status < 0) outlog << ls_cmd << ": " << strerror (errno) 
    << endl;
    else outlog << ls_cmd << ": exit " << (status >> 8)
                          << " signal " << (status & 0x7F)
                          << " core " << (status >> 7 & 1) << endl;
   header.command = cix_command::LSOUT;
   header.nbytes = ls_output.size();
   memset (header.filename, 0, FILENAME_SIZE);
   outlog << "sending header " << header << endl;
   send_packet (client_sock, &header, sizeof header);
   send_packet (client_sock, ls_output.c_str(), ls_output.size());
   outlog << "sent " << ls_output.size() << " bytes" << endl;
}


void reply_rm(accepted_socket& client_sock, cix_header& header) {

   //deletes a file
   if (unlink(header.filename) == 0){
     
   //file was successfully removed
   header.command = cix_command::ACK; 
   }
   else{
   //no payload, file failure
   header.command = cix_command::NAK;
   }
   
   header.nbytes = 0;
   send_packet(client_sock, &header, sizeof header);
}


void reply_put(accepted_socket& client_sock, cix_header& header) {
    //creates new file with filename given in header
    ofstream file( header.filename, std::ios::out | std::ios::binary);
    char *buffer = new char [header.nbytes + 1];
    //gets data into buffer
    recv_packet(client_sock, buffer, header.nbytes); 
    buffer[header.nbytes] = '\0'; //null terminator
    
    //if the file exists
    if(file) {
        header.command = cix_command::ACK;
        file.write(buffer, header.nbytes);
    }
    else{ //files doesn't exist resulting in failure
        header.command = cix_command::NAK;
    }
    send_packet (client_sock, &header, sizeof header);
    header.nbytes = 0;
}

//used the this as a reference:
//www.cplusplus.com/reference/istream/istream/seekg/
//gets the file from the server if possible
void reply_get(accepted_socket& client_sock, cix_header& header) {
    
ifstream file(header.filename, std::ios::binary);

//if file exists
if(file) {
    //gets the length of the file
    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);
    
    char *buffer = new char [length];
    file.read(buffer,length); //reads the data
    
    header.command = cix_command::FILEOUT;
    
    //sets number of bytes to the length
    header.nbytes = length;
    send_packet (client_sock, &header, sizeof header);
    send_packet (client_sock, buffer, length);
    
}

//files don't exist, file failure
else {
    
    header.command = cix_command::NAK;
    header.nbytes = 0;
    send_packet (client_sock, &header, sizeof header);
}

file.close(); //closes the file
    
}

void run_server (accepted_socket& client_sock) {
   outlog.execname (outlog.execname() + "-server");
   outlog << "connected to " << to_string (client_sock) << endl;
   try {   
      for (;;) {
         cix_header header; 
         recv_packet (client_sock, &header, sizeof header);
         outlog << "received header " << header << endl;
         switch (header.command) {
            case cix_command::LS: 
               reply_ls (client_sock, header);
               break;
               
            case cix_command::GET:
               reply_get(client_sock, header);
               break;
            
            case cix_command::PUT:
               reply_put(client_sock, header);
               break;
               
            case cix_command::RM:
               reply_rm(client_sock, header);
               break;
            
            
            default:
               outlog << "invalid client header:" << header << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cix_exit& error) {
      outlog << "caught cix_exit" << endl;
   }
   outlog << "finishing" << endl;
   throw cix_exit();
}

void fork_cixserver (server_socket& server, accepted_socket& accept) {
   pid_t pid = fork();
   if (pid == 0) { // child
      server.close();
      run_server (accept);
      throw cix_exit();
   }else {
      accept.close();
      if (pid < 0) {
         outlog << "fork failed: " << strerror (errno) << endl;
      }else {
         outlog << "forked cixserver pid " << pid << endl;
      }
   }
}


void reap_zombies() {
   for (;;) {
      int status;
      pid_t child = waitpid (-1, &status, WNOHANG);
      if (child <= 0) break;
      outlog << "child " << child
             << " exit " << (status >> 8)
             << " signal " << (status & 0x7F)
             << " core " << (status >> 7 & 1) << endl;
   }
}

void signal_handler (int signal) {
   outlog << "signal_handler: caught " << strsignal (signal) << endl;
   reap_zombies();
}

void signal_action (int signal, void (*handler) (int)) {
   struct sigaction action;
   action.sa_handler = handler;
   sigfillset (&action.sa_mask);
   action.sa_flags = 0;
   int rc = sigaction (signal, &action, nullptr);
   if (rc < 0) outlog << "sigaction " << strsignal (signal)
                      << " failed: " << strerror (errno) << endl;
}


int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   signal_action (SIGCHLD, signal_handler);
   in_port_t port = get_cix_server_port (args, 0);
   try {
      server_socket listener (port);
      for (;;) {
         outlog << to_string (hostinfo()) << " accepting port "
             << to_string (port) << endl;
         accepted_socket client_sock;
         for (;;) {
            try {
               listener.accept (client_sock);
               break;
            }catch (socket_sys_error& error) {
               switch (error.sys_errno) {
                  case EINTR:
                     outlog << "listener.accept caught "
                         << strerror (EINTR) << endl;
                     break;
                  default:
                     throw;
               }
            }
         }
         outlog << "accepted " << to_string (client_sock) << endl;
         try {
            fork_cixserver (listener, client_sock);
            reap_zombies();
         }catch (socket_error& error) {
            outlog << error.what() << endl;
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
