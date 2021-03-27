#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__
#include <iostream>
#include <string>
#include "server.h"

using namespace std;

class Request{
private:
  string wholeRequest;
  string request;
  string method;
  string hostname;
  string port;
  string uri;
  void findMethod();
  void findHostPort();
  void findURI();
public:
  Request(vector<char> v);
  Request(string _request):wholeRequest(_request){
    size_t f=wholeRequest.find("HTTP/1.1");
    request=wholeRequest.substr(0,f+8);
    findMethod();
    findURI();
    findHostPort();
  }
  string getRequest(){
    return wholeRequest;
  }
  string getMethod(){
    return method;
  }
  string getHostname(){
    return hostname;
  }
  string getPort(){
    return port;
  }
  string getURI(){
    return uri;
  }
  string getFirstLine(){
    return request;
  }
  string getWholeRequest(){
    return wholeRequest;
  }
  bool isValid();
  string getHeader();
  int getContentLen();
};

#endif