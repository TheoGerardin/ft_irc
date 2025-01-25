#pragma once
#include "utils.hpp"
#include "../Includes/Channel.hpp"
#include "../Includes/Server.hpp"

class Channel;

class Server;
class Client {
    
    private:
    
    // Client's status
    
        bool    _isLogged;
        bool    _isBot;
        bool    _isOperator;
        int     _pswdTries;
    
    // Client's connexion
    
        int         _clientFd;
        std::string _clientIp;
        std::string _password;

    // Client's informations

        std::string _hostname;
        std::string _nickname;
        std::string _username;
        std::string _realname;
        std::string _servername;
        std::map< std::string, Channel * > _clientChannels;
    
    // Client's buffer
    
        std::string _buffer;

    public:
    
    // Constructor
        
        Client(int newClientFd, std::string hostname);

    // Destructor

        virtual ~Client();

    // Getters
        
        std::string getClientHostname() const;
        std::string getClientNickname() const;
        std::string getClientBuffer() const;
        std::string getClientUsername() const;
        std::string getClientRealname() const;
        std::string getClientServername() const;
        bool        getClientBotStatus() const;
        bool        getClientLogStatus() const;
        bool        getOperatorStatus() const;
        int         getClientFd() const;
        int         getClientPswdTries() const;
        Channel     *getClientChannel();
        
    // Setters
    
        void setClientFd(int clientFd);
        void setClientIp(std::string clientIp);
        void setClientHostname(std::string hostname);
        void setClientNickname(std::string nickname);
        void setClientServername(std::string nickname);
        void setClientUsername(std::string username);
        void setClientRealname(std::string realname);
        void setClientBuffer(std::string buffer);
        void setClientBotStatus(bool status);
        void setClientLogStatus(bool status);
        void setClientOperatorStatus(bool status);
        void addClientChannel(std::string channelName, Channel *channel);
        void removeClientChannel(std::string channelName);
        void setClientPswdTries(int tries);
        void addClientPswdTry();
        void setClientPassword(std::string password);

    // Command

        bool                        isValidName(const std::string &name, size_t max_length);
        std::vector<std::string>    getArgsVector(const std::string &args);
        void                        clientNicknameCommand(const std::string &args);
        void                        clientUserCommand(const std::string &args);
        void                        clientJoinCommand(const std::string &args, Server *server);
        void                        clientPartCommand(const std::string &args, Server *server);
        void                        clientPrivmsgCommand(const std::string &args, Server *server);
        void                        clientQuitCommand(const std::string &args, Server *server);
        void                        clientTopicCommand(const std::string &args, Server *server);
        void                        clientInviteCommand(const std::string &args, Server *server);
        void                        clientKickCommand(const std::string &args, Server *server);
        void                        clientPassCommand(const std::string &args, Server *server);
        void                        clientModeCommand(const std::string &args, Server *server);
        
    // Methods

        void sendMessage(std::string const &msg);
        void sendErrorMessage(std::string const &msg);
        void sendFile(std::string const &file);
        void sendInvite(Client const *client);
};