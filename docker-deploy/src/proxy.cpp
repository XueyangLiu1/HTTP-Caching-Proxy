#include "proxy.hpp"
#include <thread>
pthread_mutex_t thread_mtx = PTHREAD_MUTEX_INITIALIZER;

string check502(string response){
    string bad="HTTP/1.1 502 Bad Gateway\r\n\r\n";
    size_t f1=response.find("HTTP/1.1");
    if(f1==string::npos){
        return bad;
    }
    if(response.find("\r\n\r\n")==string::npos){  
        return bad;
    }
    return "";
}

string getResponseGet(Request request,Cache &cache,int thread_id){
    string hostname=request.getHostname();
    string port=request.getPort();
    Client client(hostname.c_str(),port.c_str()); 
    int client_socket=client.socket_fd;
    string uri=request.getURI();
    string response=cache.get(uri);
    if(response!=""){
        cout<<"in cache"<<endl;
        Response rsp(response);
        if(rsp.needRevalidate(thread_id)){
            response=cache.revalidate(request,rsp,client_socket,thread_id);
        }
        else{
            string message=generateLogMsg(thread_id,"in cache, valid");
            writeToLog(message);
        }
    }
    else{
        cout<<"not in cache"<<endl;
        string message=generateLogMsg(thread_id,"not in cache");
        writeToLog(message);
        string requestContent = request.getRequest();
        //cout<<requestContent<<endl;
        sendString(client_socket,requestContent);
        writeRequestLog(request.getFirstLine(),hostname,thread_id);
        vector<char> r;
        client.my_recv(r);
        // response=recvString(client_socket);
        // cout<<"received"<<endl;
        //cout<<response<<endl;
        // string bad=check502(response);
        // if(bad!=""){
        //     return bad;
        // }
        Response rsp(r);
        writeReceiveLog(rsp.getFirstLine(),hostname,thread_id);
        response = string(r.begin(),r.end());
        cache.storeResponse(uri,rsp,thread_id);
        cout<<"store in cache"<<endl;
    }
    return response;
}

void stay_connect(int browser_fd, int server_fd, int thread_id) {
    send(browser_fd, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);
    string logMsg = generateLogMsg(thread_id,"Responding \"HTTP/1.1 200 OK\"");
    writeToLog(logMsg);
    fd_set readfds;
    int nfds = server_fd > browser_fd ? server_fd + 1 : browser_fd + 1;
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        FD_SET(browser_fd, &readfds);
        select(nfds, &readfds, NULL, NULL, NULL);
        int fd[2] = {server_fd, browser_fd};
        int len;
        for (int i = 0; i < 2; i++) {
            char message[65536] = {0};
            if (FD_ISSET(fd[i], &readfds)) {
                len = recv(fd[i], message, sizeof(message), 0);
                if (len <= 0) {
                    return;
                }else {
                    if (send(fd[1 - i], message, len, 0) <= 0) {
                        return;
                    }
                }
            }
        }
    }
}

void post(int browser_fd, int server_fd, Request request, int thread_id){
    string req_msg = request.getWholeRequest();
    int post_len = request.getContentLen();  //get length of client request
    if (post_len != -1) {
        sendString(server_fd,req_msg);
        char response[65536] = {0};
        int response_len = recv(server_fd,response,sizeof(response),MSG_WAITALL);
        if (response_len != 0) {
            Response res(response);
            string log_msg = generateLogMsg(thread_id,"Received \""+res.getFirstLine()
                                    +"\" from "+request.getHostname());
            writeToLog(log_msg);
            //cout << "received response: " << response << endl;
            send(browser_fd, response, response_len, 0);
            string log_msg2 = generateLogMsg(thread_id,"Responding \""+res.getFirstLine()+"\"");
            writeToLog(log_msg2);
            cout << "post successfully"<<endl;
        }else {
            cout << "failed to post"<<endl;
        }
    }
}

void *process_request(void * information){
    Thread_info *info = (Thread_info *)information;
    int browser_fd = info->browser_fd;
    string ip_addr = info->ip_addr;
    int thread_id = info->thread_id;
    Cache *cache = info->cache;
    char request[65536] = {0};
    int len = recv(browser_fd, request, sizeof(request),0);
    if(len<=0){
        std::string errMsg = "WARNING invalid request from "+ip_addr+" @ "+currTime();
        string log_msg = generateLogMsg(thread_id,errMsg);
        writeToLog(log_msg);
        return NULL;
    }
    string req = string(request,len);
    cout<<"Thread: "<<thread_id<<" is created"<<endl;
    Request p(req);
    string method = p.getMethod();
    string hostname=p.getHostname();
    string port=p.getPort();
    cout<<thread_id << ": "<<p.getFirstLine()<<endl;
    if(p.isValid()){
        std::string errMsg = p.getFirstLine()+" from "+ip_addr+" @ "+currTime();
        string log_msg = generateLogMsg(thread_id,errMsg);
        writeToLog(log_msg);
    }else{
        std::string errMsg = std::string("Responding \"HTTP/1.1 400 Bad Request\"")+" @ "+currTime();
        sendString(browser_fd,errMsg);
        string log_msg = generateLogMsg(thread_id,errMsg);
        writeToLog(log_msg);
    }
    if(method=="GET"){
        try{
            cout<<thread_id << ": "<<"try get response"<<endl;
            string response=getResponseGet(p,*cache,thread_id);
            //cout<<response<<endl;
            Response rsp(response);
            string message=generateLogMsg(thread_id,"Responding "+rsp.getFirstLine());
            writeToLog(message);
            sendString(browser_fd,response);
            cout<<thread_id << ": "<<"response send"<<endl;
        }catch(MyException e){
            string log_msg = generateLogMsg(thread_id, e.what());
            writeToLog(log_msg);
            cerr<<thread_id<<": "<<e.what()<<endl;
        }
        // the code below is the version without cache
        // try{
        //     //cout<<port<<endl;
        //     Client client(hostname.c_str(),port.c_str());
        //     send(client.socket_fd,request,len,0);
        //     vector<char> rsp;
        //     client.my_recv(rsp);
        //     my_sendTo(browser_fd,rsp);
        //     //string response = client.my_recv();
        //     // string response(rsp.begin(),rsp.end());
        //     // sendString(browser_fd,response);
        // }
        // catch(MyException e){
        //     string log_msg = generateLogMsg(thread_id, e.what());
        //     writeToLog(log_msg);
        //     cerr<<thread_id<<": "<<e.what()<<endl;
        // }    
    }else if(method=="CONNECT"){
        try{
            //cout<<thread_id << ": "<<port<<endl;
            Client client(hostname.c_str(),port.c_str());
            stay_connect(browser_fd,client.socket_fd,thread_id);
            string log_msg = generateLogMsg(thread_id, "Tunnel closed");
            writeToLog(log_msg);
        }
        catch(MyException e){
            string log_msg = generateLogMsg(thread_id, e.what());
            writeToLog(log_msg);
            cerr<<thread_id<<": "<<e.what()<<endl;
        }
    }else if(method=="POST"){
        try{
            Client client(hostname.c_str(),port.c_str());
            post(browser_fd,client.socket_fd,p,thread_id);
        }catch(MyException e){
            string log_msg = generateLogMsg(thread_id, e.what());
            writeToLog(log_msg);
            cerr<<thread_id<<": "<<e.what()<<endl;
        }
    }
    return NULL;
}



