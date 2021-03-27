#include "request.hpp"

Request::Request(vector<char> v){
    string rqst;
    rqst.insert(rqst.begin(), v.begin(), v.end());
    wholeRequest=rqst;
    size_t f=rqst.find("HTTP/1.1");
    request=rqst.substr(0,f+8);
    findMethod();
    findURI();
    findHostPort();
}

void Request::findMethod(){
  size_t found=request.find(' ');
  method=request.substr(0,found);
}  

void Request::findURI(){
  size_t f1=request.find(' ');
  size_t f2=request.find(' ',f1+1);
  uri=request.substr(f1+1,f2-f1-1);
}  

void Request::findHostPort(){
  size_t f1=uri.find("//");
  if(f1==string::npos){
    f1=0;
  }
  else{
    f1=f1+2;
  }
  size_t f2=uri.find(":",f1);
  size_t f3=uri.find("/",f1);
  if(f3==string::npos){
    f3=uri.size()+1;
  }
  if(f2!=string::npos){
    hostname=uri.substr(f1,f2-f1);
    port=uri.substr(f2+1,f3-f2-1);
  }
  else{
    hostname=uri.substr(f1,f3-f1);
    if(method=="CONNECT"){
      port="443";
    }
    else{
      port="80";
    }
  }
}

bool Request::isValid(){
  if(method!="GET"&&method!="POST"&&method!="CONNECT"){
    return false;
  }
  else{
    return true;
  }
}

string Request::getHeader(){
  size_t f=wholeRequest.find("\r\n\r\n");
  return wholeRequest.substr(0,f);
}

int Request::getContentLen() {
    size_t pos = wholeRequest.find("Content-Length: ");
    if (pos == std::string::npos) {
        return -1;
    }
    size_t header = wholeRequest.find("\r\n\r\n");
    int rest_len = wholeRequest.size() - int(header) - 8;
    size_t end = wholeRequest.find("\r\n", pos);
    int content_len = stoi(wholeRequest.substr(pos + 16, end - pos - 16));
    return content_len - rest_len - 4;
}
