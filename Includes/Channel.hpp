#pragma once
#include "utils.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Client;

class Server;
class Channel {
    
    private:
    
    // Channel's informations
    
        std::string             _channelName;
        std::string             _channelTopic;
        std::string             _channelPassword;
        int                     _channelLimit;
        std::vector<Client*>    _channelClients;
        std::vector<Client*>    _channelOperators;
        std::vector<Client*>    _channelInvited;
        bool                    _isRestricted;
        bool                    _isLimited;
        bool                    _isProtected;
        bool                    _isTopicProtected;
    
    public:
    
    // Constructor
        
        Channel(std::string channelName);
    
    // Destructor
    
        virtual ~Channel();
    
    // Getters
        
        std::string             getChannelName() const;
        std::string             getChannelTopic() const;
        std::string             getChannelPassword() const;
        std::vector<Client*>    getChannelClients() const;
        std::vector<Client*>    getChannelOperators() const;
        bool                    getChannelInviteStatus() const;
        bool                    getChannelProtectionStatus() const;
        bool                    getChannelLimitationStatus() const;
        bool                    getChannelTopicProtectionStatus() const;
        size_t                  getChannelLimit() const;
        Client*                 getBot() const;

    // Setters
    
        void setChannelName(std::string channelName);
        void setChannelTopic(std::string channelTopic);
        void setChannelPassword(std::string channelPassword);
        void setChannelInviteStatus(bool status);
        void setChannelProtectionStatus(bool status);
        void setChannelLimitationStatus(bool status);
        void setChannelTopicProtectionStatus(bool status);
        void setChannelLimit(size_t limit);
        void addChannelOperators(Client *client);
        void removeOperator(Client *client);
        
    // Methods
    
        void addClient(Client *client);
        void removeClient(Client *client);
        void restrictedBroadcast(std::string message, Client *sender);
        void userBroadcast(std::string message, Client *sender);
        void broadcast(std::string message);
        void sendFileToChannel(std::string message, Client *sender);
        void sendInviteToChannel(Client *client);
        void addChannelInvitedClient(Client *client);
        bool isOperator(const Client *client) const;
        bool hasOperator() const;
        bool isInvited(const Client* client) const;
        bool isChannelClient(const std::string &nickname);
};