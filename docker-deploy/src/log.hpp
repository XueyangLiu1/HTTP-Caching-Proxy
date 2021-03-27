#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <thread>
#include <mutex>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;
// extern std::ofstream file;
// extern std::mutex thread_mutex;
void writeRequestLog(string request,string hostname,int thread_id);
void writeReceiveLog(string response,string hostname,int thread_id);
void writeToLog(string &str);
std::string generateLogMsg(int thread_id, std::string errInfo);
void tryOpenFile(string filepath);
void mylock();
std::string currTime();


#endif
