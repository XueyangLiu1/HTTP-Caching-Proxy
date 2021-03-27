#include "response.hpp"

Response::Response(vector<char> v){
  response.insert(response.begin(), v.begin(), v.end());
}

string Response::getCode(){
  size_t f1=response.find(' ');
  size_t f2=response.find(' ',f1+1);
  string code=response.substr(f1+1,f2-f1-1);
  return code;
}

string Response::getDate(){
  return findHelper("Date");
}

string Response::getCacheControl (){
  return findHelper("Cache-Control");
}

string Response::getLastModify(){
  return findHelper("Last-Modified");
}

string Response::getExpire(){
  return findHelper("Expires");
}

string Response::getEtag(){
  return findHelper("ETag");
}

string Response::findHelper(string toFind){
  size_t f1=response.find(toFind);
  if(f1==string::npos){
    return "";
  }
  size_t f2=response.find("\r\n",f1+1);
  size_t len=toFind.size();
  string ans=response.substr(f1+len+2,f2-f1-len-2);
  return ans;
}

bool Response::isPrivate(){
  return checkHelper("private");
}

bool Response::isNoStore(){
  return checkHelper("no-store");
}

bool Response::isNoCache(){
  return checkHelper("no-cache");
}

bool Response::checkHelper(string toCheck){
  string cacheControl=getCacheControl();
  cout<<cacheControl<<endl;
  if(cacheControl!=""){
    size_t f=cacheControl.find(toCheck);
    if(f!=string::npos){
      return true;
    }
  }
  return false;
}

string Response::getMaxAge(){
  string cacheControl=getCacheControl();
  if(cacheControl!=""){
    size_t f1=cacheControl.find("max-age");
    if(f1!=string::npos){
      size_t f2=cacheControl.find(" ",f1+1);
      if(f2==string::npos){
        f2=cacheControl.find("\r\n",f1+1);
      }
      return cacheControl.substr(f1+8,f2-f1-8);
    }
  }
  return "";
}



time_t Response::getUTCTime(string strTime){
  tm mytime;
  size_t f=strTime.find("GMT");
  if(f!=string::npos){
    strTime.erase(f-1,4);
  }
  strptime(strTime.c_str(),"%a, %d %b %Y %H:%M:%S", &mytime);
  time_t tm=mktime(&mytime);
  return tm;
}

//question here: how to adjust time zone
double Response::getAge(){
  string date=getDate();
  time_t start=getUTCTime(date);
  cout<<"start: "<<start<<endl;
  time_t now=time(NULL)-28800;
  cout<<"now: "<<now<<endl;
  return difftime(now,start);
}

//this had better rewrite
bool Response::isFresh(int thread_id){
  //if max-age exist, compare getAge() and max-age
  string maxage=getMaxAge();
  string expire=getExpire();
  string lastModify=getLastModify();
  if(maxage!=""){
    cout<<"use max age"<<endl;
    long maxageLong=stol(maxage);
    double currage=getAge();
    bool fresh=maxageLong>currage;
    if(!fresh){
      writeRequireValidLog(thread_id);
    }
    return fresh;
  }
  //else compare expiresTime and time()
  else if(expire!=""){
    cout<<"use expire"<<endl;
    time_t expireTime=getUTCTime(expire);
    time_t now=time(NULL)-28800;
    bool fresh=now<expireTime;
    if(!fresh){
      string message=generateLogMsg(thread_id,"in cache, but expired at"+expire);
      writeToLog(message);
    }
    return fresh;
  }
  //else compare last-modified and then calculate freshness time
  else if(lastModify!=""){
    cout<<"use last modify"<<endl;
    string date=getDate();
    time_t dateTime=getUTCTime(date);
    time_t lastModifyTime=getUTCTime(lastModify);
    double freshTime=difftime(dateTime,lastModifyTime)/10.0;
    cout<<"fresh: "<<freshTime<<endl;
    double currage=getAge();
    cout<<"curr age: "<<currage<<endl;
    bool fresh=currage<freshTime;
    if(!fresh){
      writeRequireValidLog(thread_id);
    }
    return fresh;
  }
  //if no, return false
  else{
    cout<<"no apply"<<endl;
    return false;
  }
}


bool Response::needRevalidate(int thread_id){
  string cacheControl=getCacheControl();
  if(isNoCache()){
    writeRequireValidLog(thread_id);
    return true;
  }
  return !isFresh(thread_id);
}

void Response::writeRequireValidLog(int thread_id){
  string message=generateLogMsg(thread_id,"in cache, requires validation");
  writeToLog(message);
}

string Response::getFirstLine(){
  size_t f=response.find("\n");
  return response.substr(0,f-1);
}

int Response::getContentLength(){
  string len=findHelper("Content-Length");
  if(len==""){
    return -1;
  }
  int i_len=stoi(len);
  return i_len;
}

/**
 * check whether encoded
 * @return true encoded, false not
 */
bool Response::isChunked(){
  string isChunk=findHelper("Transfer-Encoding");
  cout<<isChunk<<endl;
  if(isChunk.find("chunked")!=string::npos){
    return true;
  }
  else{
    return false;
  }
}
