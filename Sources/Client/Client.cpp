#include "../../Includes/Client.hpp"

// Constructor

Client::Client(int newClientFd, std::string hostname) : _isLogged(false), _isBot(false), _pswdTries(0), _clientFd(newClientFd), _hostname(hostname) {}

// Destructor

Client::~Client() {}

// Getters
        
int Client::getClientFd() const {
    return _clientFd;
}

std::string Client::getClientHostname() const {
    return _hostname;
}

std::string Client::getClientNickname() const {
    return _nickname;
}

std::string Client::getClientUsername() const {
    return _username;
}

std::string Client::getClientBuffer() const {
    return _buffer;
}

bool Client::getClientBotStatus() const {
    return _isBot;
}

bool Client::getClientLogStatus() const {
    return _isLogged;
}

int Client::getClientPswdTries() const {
    return _pswdTries;
}

bool Client::getOperatorStatus() const {
    return _isOperator;
}

std::string Client::getClientServername() const {
    return _servername;
}

std::string Client::getClientRealname() const {
    return _realname;
}

Channel *Client::getClientChannel() {
    return _clientChannels[0];
}
        
// Setters
    
void Client::setClientFd(int clientFd) {
    _clientFd = clientFd;
}

void Client::setClientIp(std::string clientIp) {
    _clientIp = clientIp;
}

void Client::setClientHostname(std::string hostname) {
    _hostname = hostname;
}

void Client::setClientNickname(std::string nickname) {
    _nickname = nickname;
}

void Client::setClientUsername(std::string username) {
    _username = username;
}

void Client::setClientBuffer(std::string msg) {
    _buffer += msg;
}

void Client::setClientBotStatus(bool status) {
    _isBot = status;
}

void Client::setClientLogStatus(bool status) {
    _isLogged = status;
}

void Client::setClientPswdTries(int tries) {
    _pswdTries = tries;
}

void Client::addClientPswdTry() {
    _pswdTries++;
}

void Client::setClientPassword(std::string password) {
    _password = password;
}

void Client::setClientOperatorStatus(bool status) {
    _isOperator = status;
}

void Client::addClientChannel(std::string channelName, Channel *channel) {
    _clientChannels[channelName] = channel;
}

void Client::removeClientChannel(std::string channelName) {
    _clientChannels.erase(channelName);
}

void Client::setClientServername(std::string servername) {
    _servername = servername;
}

void Client::setClientRealname(std::string realname) {
    _realname = realname;
}

// Methods

void Client::sendMessage(std::string const &msg) {
    std::string formatedMsg = msg + "\r\n";
    send(getClientFd(), formatedMsg.c_str(), formatedMsg.size(), 0);
    memset((void*)formatedMsg.c_str(), 0, formatedMsg.length());
}

void Client::sendFile(std::string const &file) {
    (void)file;
}

void Client::sendInvite(Client const *client) {
    (void)client;
}

void Client::sendErrorMessage(std::string const &msg) {
    std::string errorMsg = "[ERROR]: " + msg + "\r\n";
    sendMessage(errorMsg);
}