#ifndef __RESPONSE_HPP__
#define __RESPONSE_HPP__

#include "request.hpp"
#include "log.hpp"
#include <ctime>

class Response{
private:
  string response;
  string findHelper(string toFind);
public:
  Response():response(){}
  Response(vector<char> v);
  Response(string _response):response(_response){}
  string getResponse(){return response;}
  string getCode();
  string getDate();
  string getCacheControl();
  string getLastModify();
  string getExpire();
  string getEtag();
  string getFirstLine();
  time_t getUTCTime(string strTime);
  double getAge();
  bool isFresh(int thread_id);
  bool needRevalidate(int thread_id);
  void writeRequireValidLog(int thread_id);
  bool isPrivate();
  bool isNoStore();
  string getMaxAge();
  bool isNoCache();
  bool checkHelper(string toCheck);
  int getContentLength();
  bool isChunked();
};



#endif
