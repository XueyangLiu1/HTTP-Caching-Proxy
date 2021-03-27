#include "cache.hpp"
#include "log.hpp"
#include "server.h"
#include "MyException.h"
#include "thread_info.h"
#include <string>
#include <pthread.h>

#define PORT "12345"

using namespace std;

string getResponseGet(Request request,Cache &cache,int thread_id);
void stay_connect(int server_fd, int browser_fd);
void *process_request(void * information);
string check502(string response);
