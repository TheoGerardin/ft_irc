#include "../../Includes/Server.hpp"
#include "../../Includes/Bot.hpp"

// Socket(s) management

bool Server::bindSocketToAddress() {
    
    // Instance of of sockaddr_in called _serverAdress
    
    _serverAdress.sin_family = AF_INET;
    _serverAdress.sin_port = htons(getServerPort());
    _serverAdress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address and port
    
    if (bind(getServerFd(), (struct sockaddr *)&_serverAdress, sizeof(_serverAdress)) < 0)
        return false;
    return true;
}

bool Server::setSocketNonBlockingMode() {
    
    if (fcntl(getServerFd(), F_SETFL, O_NONBLOCK))
        return false;
    return true;
}

// Epoll and events

bool    Server::manageEpollAndEvents() {
    
    // Create epoll instance 
    
    setEpollFd(epoll_create1(0));
    if (getEpollFd() < 0)
        return false;
    
    // Configure epoll instance

    this->_epollEvent.events = EPOLLIN;
    this->_epollEvent.data.fd = getServerFd();

    // Adding server's socket to epoll instance

    if (epoll_ctl(getEpollFd(), EPOLL_CTL_ADD, getServerFd(), &(this->_epollEvent)) < 0)
        return false;
    return true;
}

bool Server::createSocket() {
    setServerFd(socket(AF_INET, SOCK_STREAM, 0));
    if (getServerFd() < 0)
        return false;
    return true;
}

bool Server::setSocketOptions() {
	int options = 1;
	setsockopt(getServerFd(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &options, sizeof(options));
	if (listen(getServerFd(), CLIENT_NB) < 0)
        return false;
    return true;
}

bool Server::createBot() {
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = _serverBot->getClientFd();

    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, _serverBot->getClientFd(), &event) == -1) {
        std::cerr << "Failed to add bot to epoll" << std::endl;
        std::cerr << strerror(errno) << std::endl;
        closeFileDescriptors();
        setServerStatus(false);
        return false;
    }
    return true;
}

// Error management

void Server::manageSocketError(int step) {
    std::string errorMsg;
    switch (step) {
        case CREATE_SOCKET:
            errorMsg = "[SERVER]: [ERROR]: Cannot create the server socket!";
            break;
        case SET_NONBLOCKING:
            errorMsg = "[SERVER]: [ERROR]: Cannot set server's socket to non-block!";
            break;
        case BIND_SOCKET:
            errorMsg = "[SERVER]: [ERROR]: Cannot bind the socket!";
            break;
        case SET_OPTIONS:
            errorMsg = "[SERVER]: [ERROR]: Cannot set socket options!";
            break;
        case MANAGE_EPOLL:
            errorMsg = "[SERVER]: [ERROR]: Cannot manage epoll and events!";
            break;
        case MANAGE_BOT:
            errorMsg = "[SERVER]: [ERROR]: Cannot create the bot!";
            break;
    }
    std::cerr << RED << errorMsg << RESET_COLOR << std::endl;
    std::cerr << strerror(errno) << std::endl;
    closeFileDescriptors();
    setServerStatus(false);
}

void Server::setupSocketAndEvents() {
    bool flag;

    for (int step = CREATE_SOCKET; step <= MANAGE_EPOLL; ++step) {
        switch (step) {
            case CREATE_SOCKET:
                flag = createSocket();
                break;
            case SET_NONBLOCKING:
                flag = setSocketNonBlockingMode();
                break;
            case BIND_SOCKET:
                flag = bindSocketToAddress();
                break;
            case SET_OPTIONS:
                flag = setSocketOptions();
                break;
            case MANAGE_EPOLL:
                flag = manageEpollAndEvents();
                break;
            case MANAGE_BOT:
                flag = createBot();
                break;
        }
        if (!flag) {
            manageSocketError(step);
            return;
        }
    }
    setServerStatus(true);
    std::cout << GREEN << "[SERVER]: Successfully created!" << RESET_COLOR << std::endl;
}