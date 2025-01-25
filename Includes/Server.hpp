#ifndef SERVER_HPP
#define SERVER_HPP

#include "utils.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Client;

class Channel;

class Bot;
class Server {
    
    private:
    
    // Server informations
    
        std::string _serverPswd;
        sockaddr_in _serverAdress;
        bool        _serverStatus;
        int         _serverPort;
        int         _serverFd;
    
    // Bot

        Bot *_serverBot;

    // Epoll informations

        int                 _epollFd;
        struct epoll_event  _epollEvent;
        struct epoll_event  _epollEventsArr[CLIENT_NB];

    // Clients informations
    
        std::map<int, Client *>     _clients;
        std::vector<struct pollfd>  _clientsFd;
    
    // Channels informations
    
        std::map<std::string, Channel *> _channels;
    
    // Error
    
        enum setupStep {
            CREATE_SOCKET,
            SET_NONBLOCKING,
            BIND_SOCKET,
            SET_OPTIONS,
            MANAGE_EPOLL,
            MANAGE_BOT
        };
        
    // Abstract
        
        Server();
    
    public:
    
    // Constructor

        // Set _serverFD = -1 | Security check for const usage (not your concern Romain)
        Server(std::string const &pswd, int const &port);
    
    // Destructor

        ~Server();

    // Socket(s) management

        void    setupSocketAndEvents();
        bool    createSocket();
        bool    setSocketNonBlockingMode();
        bool    bindSocketToAddress();
        bool    setSocketOptions();
        bool    createBot();

    // Epoll management

        bool    manageEpollAndEvents();

    // Loop

        void    serverLoop();
        void    handleClientEvent(int user_fd);
        void    handleNewClient();
        
    // Message and commands
        
        enum commands {
            NICKNAME, // Define user pseudo
            CAP, // Define user capabilities
            MODE,
            WHO,
            PASS, // Define user password
            USER, // Define user infos : USER <username> <hostname> <servername> <realname>
            JOIN, // Join a channel : JOIN <#channel>
            PART, // Leave a channel : PART <#channel>
            PRIVMSG, // Send a message : PRIVMSG <#channel> :<message>
            QUIT, // Quit the server : QUIT <message>
            TOPIC, // Set the topic : TOPIC <#channel> :<topic>
            INVITE, // Invite a user : INVITE <nickname> <#channel>
            KICK, // Kick a user : KICK <#channel> <nickname> :<reason>
            OPERATOR, // Set operator status : OPERATOR <#channel> <nickname>
            HELP // Help command
        };
    
        void                        handleMessage(std::string const &msg, int user_fd);
        void                        processClientCommand(const std::string &command, const std::string &args, int user_fd);
        Server::commands            defineCommand(const std::string &command);
        std::vector<std::string>    splitCommands(const std::string& message);
   
   // Server's cleaners

        void    closeFileDescriptors();
        void    clearClients();
        void    clearChannels();
        void    clearClientFromChannels(int user_fd);

    // Error(s) management
    
        void    manageSocketError(int step);
    
    // Setters
        void    setServerFd(int serverFd);
        void    setServerStatus(bool status);
        void    setEpollFd(int epollFd);

    // Getters
        int                                 getServerFd() const;
        int                                 getEpollFd() const;
        int                                 getServerPort() const;
        bool                                getServerStatus() const;
        struct epoll_event                  &getEpollEvent();
        struct epoll_event                  *getEpollEventsArr();
        std::map<int, Client *>             &getClients();
        std::map<std::string, Channel *>    &getServerChannels();
        Client                              *getClientByNickname(std::string const &nickname);
        std::string                         getServerPswd() const;
        Bot                                 *getServerBot() const;

    // Channel

        void    addServerChannel(std::string channelName, Channel *channel);
        void    removeServerChannel(std::string channelName);
    
    // Methods

        Channel *getChannelByName(std::string const &channelName);
};

#endif