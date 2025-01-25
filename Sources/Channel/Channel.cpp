#include "../../Includes/Channel.hpp"

// Constructor

Channel::Channel(std::string channelName) : _channelName(channelName) {
    _channelTopic = "";
    _channelPassword = "";
}

// Destructor

Channel::~Channel() {
    // for (std::vector<Client*>::iterator it = _channelClients.begin(); it != _channelClients.end(); ++it) {
    //     delete *it;
    // }
    // _channelClients.clear();

    // for (std::vector<Client*>::iterator it = _channelOperators.begin(); it != _channelOperators.end(); ++it) {
    //     delete *it;
    // }
    // _channelOperators.clear();

    // for (std::vector<Client*>::iterator it = _channelInvited.begin(); it != _channelInvited.end(); ++it) {
    //     delete *it;
    // }
    // _channelInvited.clear();
}

// Getters

std::string Channel::getChannelName() const {
    return _channelName;
}

std::string Channel::getChannelTopic() const {
    return _channelTopic;
}

std::string Channel::getChannelPassword() const {
    return _channelPassword;
}

std::vector<Client*> Channel::getChannelClients() const {
    return _channelClients;
}

std::vector<Client*> Channel::getChannelOperators() const {
    return _channelOperators;
}

bool Channel::getChannelInviteStatus() const {
    return _isRestricted;
}

bool Channel::getChannelProtectionStatus() const {
    return _isProtected;
}

bool Channel::getChannelLimitationStatus() const {
    return _isLimited;
}

bool Channel::getChannelTopicProtectionStatus() const {
    return _isTopicProtected;
}

size_t Channel::getChannelLimit() const {
    return _channelLimit;
}

Client* Channel::getBot() const {
    for (std::vector<Client*>::const_iterator it = _channelClients.begin(); it != _channelClients.end(); ++it) {
        if ((*it)->getClientBotStatus()) {
            return *it;
        }
    }
    return NULL;
}

// Setters

void Channel::setChannelInviteStatus(bool status) {
    _isRestricted = status;
}

void Channel::setChannelTopicProtectionStatus(bool status) {
    _isTopicProtected = status;
}

void Channel::setChannelProtectionStatus(bool status) {
    _isProtected = status;
}

void Channel::setChannelLimitationStatus(bool status) {
    _isLimited = status;
}

void Channel::setChannelName(std::string channelName) {
    _channelName = channelName;
}

void Channel::setChannelTopic(std::string channelTopic) {
    _channelTopic = channelTopic;
}

void Channel::setChannelPassword(std::string channelPassword) {
    _channelPassword = channelPassword;
}

void Channel::setChannelLimit(size_t limit) {
    _channelLimit = limit;
}

void Channel::addChannelOperators(Client *client) {
    _channelOperators.push_back(client);
}

void Channel::addChannelInvitedClient(Client *client) {
    _channelInvited.push_back(client);
}

void Channel::removeOperator(Client *client) {
    std::vector<Client*>::iterator it = std::find(_channelOperators.begin(), _channelOperators.end(), client);
    if (it != _channelOperators.end()) {
        _channelOperators.erase(it);
    }
}

// Methods

void Channel::addClient(Client *client) {
    _channelClients.push_back(client);
}

void Channel::removeClient(Client *client) {
    _channelClients.erase(std::remove(_channelClients.begin(), _channelClients.end(), client), _channelClients.end());
}

void Channel::restrictedBroadcast(std::string message, Client *sender) {
    for (std::vector<Client*>::iterator it = _channelClients.begin(); it != _channelClients.end(); it++) {
        if (*it != sender) {
            std::cout << YELLOW << "[SERVER]:" << RESET_COLOR << " Sending message to " << (*it)->getClientNickname() << std::endl;
            (*it)->sendMessage(message);
        }
    }
}

void Channel::broadcast(std::string message) {
    for (std::vector<Client*>::iterator it = _channelClients.begin(); it != _channelClients.end(); it++) {
            std::cout << YELLOW << "[SERVER]:" << RESET_COLOR << " Sending message to " << (*it)->getClientNickname() << std::endl;
            (*it)->sendMessage(message);
    }
}

void Channel::userBroadcast(std::string message, Client *sender) {
    for (std::vector<Client*>::iterator it = _channelClients.begin(); it != _channelClients.end(); it++) {
        if (*it == sender) {
            (*it)->sendMessage(message);
        }
    }
}

void Channel::sendFileToChannel(std::string message, Client *client) {
    for (std::vector<Client*>::iterator it = _channelClients.begin(); it != _channelClients.end(); it++) {
        if (*it != client) {
            (*it)->sendFile(message);
        }
    }
}

void Channel::sendInviteToChannel(Client *client) {
    for (std::vector<Client*>::iterator it = _channelClients.begin(); it != _channelClients.end(); it++) {
        (*it)->sendInvite(client);
    }
}

bool Channel::isOperator(const Client* client) const {
    return std::find(_channelOperators.begin(), _channelOperators.end(), client) != _channelOperators.end();
}

bool Channel::isInvited(const Client* client) const {
    return std::find(_channelInvited.begin(), _channelInvited.end(), client) != _channelInvited.end();
}

bool Channel::hasOperator() const {
    return !_channelOperators.empty();
}

bool Channel::isChannelClient(const std::string &nickname) {
    for (std::vector<Client*>::const_iterator it = _channelClients.begin(); it != _channelClients.end(); ++it) {
        if ((*it)->getClientNickname() == nickname) {
            return true;
        }
    }
    return false;
}