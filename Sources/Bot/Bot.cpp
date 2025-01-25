#include "../../Includes/Bot.hpp"
#include <iostream>

// Bot constructor

Bot::Bot(int newClientFd, std::string hostname) : Client(newClientFd, hostname) {
    setClientNickname("Bot");
    setClientUsername("Bot");
    setClientRealname("Bot");
    setClientLogStatus(true);
    setClientBotStatus(true);
}

Bot::botCommands Bot::botDefineCommand(const std::string &command) {
    if (command == "!hello") return HELLO;
    if (command == "!leave") return LEAVE;
    if (command == "!help") return HELP;
    throw std::invalid_argument("try <!help>");
}

// Bot commands

void Bot::botHelloCommand(Channel *channel, Client *target) {
    std::string response = "Hello " + target->getClientNickname() + "\r\n";
    channel->restrictedBroadcast(":" + this->getClientNickname() + " PRIVMSG " + channel->getChannelName() + " :" + response + "\r\n", this);
}

void Bot::botHelpCommand(Channel *channel) {
    std::string response = "Bot command: <!hello> <!leave> <!help>\r\n";
    channel->restrictedBroadcast(":" + this->getClientNickname() + " PRIVMSG " + channel->getChannelName() + " :" + response + "\r\n", this);
}

void Bot::handleBotCommand(const std::string &message, Channel *channel, Client *target, Server *server) {
    
    size_t spacePos = message.find(' ');
    std::string command = (spacePos == std::string::npos) ? message : message.substr(0, spacePos);
    std::cout << "bot command: " << command << std::endl;
    std::string args = (spacePos == std::string::npos) ? "" : message.substr(spacePos + 1);
    std::cout << "bot args: " << args << std::endl;

    try {
        botCommands flag = botDefineCommand(command);
        switch (flag) {
            case HELLO:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing !hello bot command with args: " << args << std::endl;
                botHelloCommand(channel, target);
                break;
            case LEAVE:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing !leave bot command with args: " << args << std::endl;
                this->clientPartCommand(channel->getChannelName() + " :Leaving", server);
                break;
            case HELP:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing !help bot command with args: " << args << std::endl;
                botHelpCommand(channel);
                break;
        }
    } catch (const std::invalid_argument& e) {
        std::string errorMsg = "Unknown bot command: " + command;
        send(target->getClientFd(), errorMsg.c_str(), errorMsg.length(), 0);
    }
}