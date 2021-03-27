#ifndef __CACHE_H__
#define __CACHE_H__

#include "response.hpp"
#include <map>
#include <algorithm>
#include "log.hpp"
#include <mutex>

class Cache{
  private:
    class Node{
      public:
        string key;
        string value;
        Node* prev;
        Node* next;
        Node():key(),value(),prev(NULL),next(NULL){}
        Node(string _key,string _value):key(_key),value(_value),prev(NULL),next(NULL){}
    };
    map<string,Node*> mycache;
    Node* head;
    Node* tail;
    int capacity;
    int size;
  public:
    Cache(int _capacity):mycache(),capacity(_capacity),size(0){
      head=new Node();
      tail=new Node();
      head->next=tail;
      tail->prev=head;
    }
    Cache(const Cache & rhs);
    Cache& operator =(const Cache & rhs);
    ~Cache();
    string get(string key);
    void put(string key,string value);
    void addToHead(Node *node);
    void removeNode(Node *node);
    void moveToHead(Node *node);
    void removeTail();
    bool storeResponse(string uri,Response rsp,int id);
    string revalidate(Request request, Response response, int socket,int id);
    string checkIfNoneMatch(Request request,Response response,int socket,string etag,int id);
    string checkIfModifiedSince(Request request,Response response,int socket,string lastModify,int id);
    string reSendRequest(Request request, int socket,int id);
    string checkValidate(Request request,Response response,int socket, string type,string content,int id);
    friend ostream& operator<< (ostream &s, const Cache& rhs);
};

#endif
