#define CLIENT_NB 10

// Custom

#include "Color.hpp"

// Include .cpp

#include <string>
#include <iostream>
#include <exception>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>

// Containers

#include <vector>
#include <map>

// Include .c

#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <poll.h>
#include <netdb.h>