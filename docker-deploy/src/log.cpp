#include "log.hpp"

std::ofstream file;
std::mutex thread_mutex;

void writeRequestLog(string request,string hostname,int thread_id){
    string message=generateLogMsg(thread_id,"Requesting "+request+" from "+hostname);
    writeToLog(message);
}

void writeReceiveLog(string response,string hostname,int thread_id){
    string message=generateLogMsg(thread_id,"Received "+response+" from "+hostname);
    writeToLog(message);
}

void writeToLog(string &str){
    std::lock_guard<std::mutex> lck(thread_mutex);
    file << str;
    file.flush();
}

std::string generateLogMsg(int thread_id, std::string errInfo){
    std::stringstream stream;
    stream << thread_id << ": " << errInfo << "\n";
    string log_msg = stream.str();
    stream.str("");
    return log_msg;
}

void tryOpenFile(string filepath){
  try {
      file.open(filepath, std::ostream::out);
  } catch (std::exception &e) {
      cout << e.what() << std::endl;
      exit(EXIT_FAILURE);
  }
}

std::string currTime() {
  time_t currTime = time(0);
  struct tm * nowTime = gmtime(&currTime);
  const char * t = asctime(nowTime);
  string tme(t);
  return tme.substr(0,tme.size()-1);
}

