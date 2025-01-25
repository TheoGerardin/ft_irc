#include "../../Includes/Client.hpp"
#include "../../Includes/Bot.hpp"

// Client Commands utils

bool Client::isValidName(const std::string& name, size_t max_length) {
    if (name.length() > max_length || name.length() < 1) {
        return false;
    }
    for (size_t i = 0; i < name.length(); ++i) {
        if (!isalnum(name[i]) && name[i] != '_' && name[i] != '-') {
            return false;
        }
    }
    return true;
}

std::vector<std::string> Client::getArgsVector(const std::string &args) {
    std::vector<std::string> argsVector;
    std::istringstream iss(args);
    std::string token;
    while (iss >> token) {
        argsVector.push_back(token);
    }
    return argsVector;
}

bool isBotCommand(const std::string &cmd) {
    if (cmd == "!hello" || cmd == "!leave" || cmd == "!help")
        return true;
    else
        return false;
}

// Client commands

void Client::clientPassCommand(const std::string &args, Server *server) {

    std::vector<std::string> arguments = getArgsVector(args);
    
    if (getClientLogStatus()) {
        sendErrorMessage("You are already logged in.");
        return;
    }
    if (arguments.size() != 1) {
        sendErrorMessage("[USAGE]: /pass <password>");
        return;
    }
    if (getClientPswdTries() >= 3) {
        sendErrorMessage("You have reached the maximum number of password tries.");
        close(getClientFd());
        epoll_ctl(server->getEpollFd(), EPOLL_CTL_DEL, getClientFd(), NULL);
        server->getClients().erase(getClientFd());
        delete this;
        return;
    }

    if (arguments[0] != server->getServerPswd()) {
        sendErrorMessage("Incorrect password.");
        addClientPswdTry();
        setClientLogStatus(false);
        return;
    } else {
        setClientPassword(arguments[0]);
        setClientLogStatus(true);
        if (getClientNickname().empty()) {
            std::stringstream ss;
            ss << getClientFd();
            setClientNickname(ss.str());
            std::cout << GREEN << "[COMMAND]: Nickname set to " + getClientNickname() << RESET_COLOR << std::endl;
        }
        sendMessage("You are now successefully logged in.");
        std::cout << GREEN << "[COMMAND]: Password set." << RESET_COLOR << std::endl;   
    }
}

void Client::clientNicknameCommand(const std::string &args) {

    std::vector<std::string> arguments = getArgsVector(args);

    if (arguments.size() != 1) {
        sendErrorMessage("[USAGE]: /nickname <nickname>");
        return;
    }

    if (!isValidName(arguments[0], 9)) {
        sendErrorMessage("[USAGE]: Nickname must be between 1 and 9 characters and contain only letters, digits, '_', and '-'.");
        return;
    }

    sendMessage(":" + getClientNickname() + " NICK " + arguments[0] + "\r\n");
    setClientNickname(args);
    std::cout << GREEN << "[COMMAND]: Nickname changed to " + getClientNickname() << RESET_COLOR << std::endl;
}

void Client::clientUserCommand(const std::string &args) {
    
    std::vector<std::string> argsVector = getArgsVector(args);
    
    if (argsVector.size() != 4) {
        sendErrorMessage("[USAGE]: /username <username> <hostname> <servername> <realname>");
        return;
    }

    setClientUsername(argsVector[0]);
    std::string successMsg = "[USERNAME]: " + getClientUsername() + "\n";
    setClientHostname(argsVector[1]);
    successMsg += "[HOSTNAME]: " + getClientHostname() + "\n";
    setClientServername(argsVector[2]);
    successMsg += "[SERVERNAME]: " + getClientServername() + "\n";
    setClientRealname(argsVector[3]);
    successMsg += "[REALNAME]: " + getClientRealname() + "\n";
    setClientPswdTries(0);
    
    successMsg += "[COMMAND]: Client infos updated\n";
    std::cout << GREEN << successMsg << RESET_COLOR << std::endl;
}

void Client::clientJoinCommand(const std::string &args, Server *server) {
    
    if (!getClientLogStatus()) {
        sendErrorMessage("You are not logged in. Please set your password first.");
        return;
    }
    
    std::vector <std::string> arguments = getArgsVector(args);
    if (arguments.size() < 1 || arguments.size() > 1){
        sendErrorMessage("[USAGE]: /join <#channel>");
        return ;
    }
    
    if (server->getServerChannels().find(arguments[0]) != server->getServerChannels().end()) {

        Channel* channel = server->getServerChannels()[arguments[0]];

        if(channel->isChannelClient(getClientNickname())) {
            sendErrorMessage("Already a client of this channel");
            return;
        }

        if (channel->getChannelProtectionStatus()) {
            if (arguments.size() < 2) {
                sendErrorMessage("[USAGE]: /join <channel> <password>");
                return;
            }
            if (channel->getChannelPassword() != arguments[1]) {
                sendErrorMessage("Incorrect password.");
                return;
            }
        }

        if (channel->getChannelLimitationStatus()) {
            if (channel->getChannelClients().size() >= channel->getChannelLimit()) {
                sendErrorMessage("Channel is full.");
                return;
            }
        }
        
        if (channel->getChannelInviteStatus()) {
            if (channel->isInvited(this)) {
                addClientChannel(arguments[0], channel);
                channel->addClient(this);
                channel->broadcast(":" + getClientNickname() + " JOIN " + channel->getChannelName() + "\r\n");
            }
            else {
                sendErrorMessage("You are not invited to " + channel->getChannelName() + ".");
                return;
            }
        } else {
            addClientChannel(arguments[0], channel);
            channel->addClient(this);
            channel->broadcast(":" + getClientNickname() + " JOIN " + channel->getChannelName() + "\r\n");   
        }
    }
    
    else {
        if (arguments[0][0] != '#' && arguments[0][0] != '&') {
            sendErrorMessage("[USAGE]: Channel name must start with a '#' or '&' character.");
            return;
        }
        if (!isValidName(&arguments[0][1], 9)) {
            sendErrorMessage("[USAGE]: Channel name must be between 1 and 9 characters and contain only letters, digits, '_', and '-'.");
            return;
        }
        server->addServerChannel(arguments[0], new Channel(args));
        Channel* channel = server->getServerChannels()[args];
        addClientChannel(arguments[0], channel);
        channel->addClient(this);
        channel->addChannelOperators(this);
        channel->broadcast(":" + getClientNickname() + " JOIN " + channel->getChannelName() + "\r\n");
        channel->setChannelLimitationStatus(false);
        channel->setChannelInviteStatus(false);
        channel->setChannelProtectionStatus(false);
        channel->setChannelTopicProtectionStatus(false);
        channel->setChannelTopic("");
        sendMessage(":" + getClientNickname() + " MODE " + channel->getChannelName() + " +o " + getClientNickname() + "\r\n");
    }

    std::cout << GREEN << "[COMMAND]: " << getClientNickname() << " joined " << arguments[0] << RESET_COLOR << std::endl;
}

void Client::clientPartCommand(const std::string &args, Server *server) {

    if (!getClientLogStatus()) {
        sendErrorMessage("You are not logged in. Please set your password first.");
        return;
    }
    
    std::vector <std::string> arguments = getArgsVector(args);
    if (arguments.size() < 1){
        sendErrorMessage("[USAGE]: /part <channel>");
        return ;
    }
    
    std::string channelName = arguments[0];
    if (channelName[0] != '#' && channelName[0] != '&') {
        sendErrorMessage("[USAGE]: Channel name must start with a '#' or '&' character.");
        return;
    }
    if (server->getServerChannels().find(channelName) == server->getServerChannels().end()) {
        sendErrorMessage("[ERROR]: Channel doesn't exist.");
        return;
    }
    
    Channel *channel = server->getServerChannels()[channelName];
    channel->removeClient(this);
    removeClientChannel(channelName);
    channel->removeOperator(this);
    channel->broadcast(":" + getClientNickname() + " PART " + channel->getChannelName() + "\r\n");
    sendMessage(":" + getClientNickname() + " PART " + channel->getChannelName() + "\r\n");
    
    if (!channel->hasOperator()) {
        std::vector<Client*> clientsToRemove = channel->getChannelClients();
        for (std::vector<Client*>::iterator it = clientsToRemove.begin(); it != clientsToRemove.end(); ++it) {
            channel->removeClient(*it);
            (*it)->sendMessage(":" + (*it)->getClientNickname() + " PART " + channel->getChannelName() + "\r\n");
            (*it)->removeClientChannel(channelName);
        }
        server->removeServerChannel(channelName);
    }
}

void    Client::clientPrivmsgCommand(const std::string &args, Server *server) {
    
    if (!getClientLogStatus()) {
        sendErrorMessage("You are not logged in. Please set your password first.");
        return;
    }
    
    std::vector<std::string> arguments = getArgsVector(args);
    
    if (arguments.size() < 2) {
        sendErrorMessage("[USAGE]: /privmsg <target> <message>");
        return;
    }

    std::string target = arguments[0];
    std::string message = args.substr(target.length() + 1);
    if (!message.empty() && message[0] == ':') {
        message = message.substr(1);
    }

    if (target[0] == '#' || target[0] == '&') {
        
        std::cout << YELLOW << message << std::endl;
        Channel *channel = server->getChannelByName(target);
        if (channel == NULL) {
            sendErrorMessage("Channel " + target + " does not exist.");
            return;
        }
        
        std::vector<Client*> clients = channel->getChannelClients();
        
        if (std::find(clients.begin(), clients.end(), this) != clients.end()) {
            
            channel->restrictedBroadcast(":" + getClientNickname() + " PRIVMSG " + channel->getChannelName() + " :" + message + "\r\n", this);
            
            if (isBotCommand(message)) {
                Bot *bot = server->getServerBot();
                if (!channel->getBot() && message != "!leave") {
                    bot->clientJoinCommand(channel->getChannelName(), server);
                }
                bot->handleBotCommand(message, channel, this, server);
            }
            
        } else {
            sendErrorMessage("You are not registered on " + target + ".");
            return ;
        }
    }
    else {
        Client *targetClient = server->getClientByNickname(target);
        if (!targetClient) {
            sendErrorMessage("Client " + target + " does not exist.");
            return;
        }

        if (arguments[1] == ":DCC") {
            targetClient->sendMessage(":" + getClientNickname() + " PRIVMSG " + target + " :\x01" + message + "\x01\r\n");
            return;
        }

        targetClient->sendMessage(":" + getClientNickname() + " PRIVMSG " + target + " :" + message + "\r\n");
    }
}

void Client::clientQuitCommand(const std::string &args, Server *server) {
    
    std::cout << GREEN << "[COMMAND]:" << RESET_COLOR << " Client " << getClientNickname() << " has been disconnected." << std::endl;
    
    sendMessage(args);
    close(getClientFd());
    epoll_ctl(server->getEpollFd(), EPOLL_CTL_DEL, getClientFd(), NULL);
    
    std::map<std::string, Channel*> &channels = server->getServerChannels();
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); it++) {
        Channel *channel = it->second;
        channel->removeClient(this);
        channel->removeOperator(this);
    } 

    server->getClients().erase(getClientFd());
    delete this;
}

void Client::clientTopicCommand(const std::string &args, Server *server) {
    
    if (!getClientLogStatus()) {
        sendErrorMessage("You are not logged in. Please set your password first.");
        return;
    }
    
    std::vector<std::string> arguments = getArgsVector(args);
    if (arguments.size() < 1) {
        sendErrorMessage("[USAGE]: /TOPIC <#ChannelName> optionnal:<topic>");
        return;
    }

    std::string channelName = arguments[0];
    
    Channel* channel = server->getChannelByName(channelName);
    if (!channel) {
        sendErrorMessage("Channel " + channelName + " does not exist.");
        return;
    }
    
    if (arguments.size() == 1) {
        channel->userBroadcast(":" + getClientNickname() + " NOTICE " + channel->getChannelName() + " :Topic of " + channel->getChannelName() + " is " + channel->getChannelTopic() + "\r\n", this);
        return;
    }

    if (arguments.size() != 2) {
        sendErrorMessage("[USAGE]: /TOPIC <#ChannelName> <topic>");
        return;
    }
    
    std::string topic = &arguments[1][0];

    if (channel->getChannelTopicProtectionStatus()) {
        if (!channel->isOperator(this)) {
            channel->userBroadcast("Command only available for operators.", this);
            return;  
        }
    }

    if (topic.length() > 255) {
        channel->userBroadcast("Topic must be 255 characters max.", this);
        return;
    }
    
    channel->setChannelTopic(topic);
    std::string topicMsg = " :" + topic;
    channel->broadcast(":" + getClientNickname() + " TOPIC " + channel->getChannelName() + " :" + topic + "\r\n");
    std::cout << GREEN << "[COMMAND]: " << channel->getChannelName() << " topic set to " + topic << RESET_COLOR << std::endl;
}

void Client::clientInviteCommand(const std::string &args, Server *server) {

    if (!getClientLogStatus()) {
        sendErrorMessage("You are not logged in. Please set your password first.");
        return;
    }
    
    std::vector<std::string> arguments = getArgsVector(args);
    
    if (arguments.size() != 2) {
        sendErrorMessage("[USAGE]: /invite <nickname> <channel>");
        return;
    }

    std::string nickname = arguments[0];
    std::string channelName = arguments[1];

    Channel* channel = server->getChannelByName(channelName);
    if (!channel) {
        sendErrorMessage("Channel " + channelName + " does not exist.");
        return;
    }
    
    if (!channel->isOperator(this)) {
        sendErrorMessage("You are not an operator of channel " + channelName + ".");
        return;
    }

    Client* invitedClient = server->getClientByNickname(nickname);
    if (!invitedClient) {
        sendErrorMessage("Client " + nickname + " does not exist.");
        return;
    }
    
    std::cout << "invitation to " << invitedClient->getClientNickname() << std::endl;
    channel->addChannelInvitedClient(invitedClient);
    std::string inviteMsg = ":" + getClientNickname() + " INVITE " + invitedClient->getClientNickname() + " " + channelName + "\r\n";
    invitedClient->sendMessage(inviteMsg);
    sendMessage(inviteMsg);
}

void Client::clientKickCommand(const std::string &args, Server *server) {

    if (!_isLogged) {
        sendErrorMessage("You are not logged in. Please set your nickname and password first.");
        return;
    }
    
    std::vector<std::string> arguments = getArgsVector(args);
    
    if (arguments.size() < 3) {
        sendErrorMessage("[USAGE]: /kick <nickname> <channel> <reason>");
        return;
    }

    std::string channelName = arguments[0];
    std::string nickname = arguments[1];
    std::string reason;
    for (std::vector<std::string>::size_type i = 2; i < arguments.size(); ++i) {
        if (i > 2)
            reason += " ";
        reason += arguments[i];
    }

    Channel* channel = server->getChannelByName(channelName);
    if (!channel) {
        sendErrorMessage("Channel " + channelName + " does not exist.");
        return;
    }
    
    if (!channel->isOperator(this)) {
        sendErrorMessage("You are not an operator of channel " + channelName + ".");
        return;
    }

    Client* kickedClient = server->getClientByNickname(nickname);
    if (!kickedClient) {
        sendErrorMessage("[USAGE]: Client " + nickname + " does not exist.");
        return;
    }

    if (kickedClient == this) {
        sendErrorMessage("[USAGE]: You cannot kick yourself.");
        return;
    }

    std::string kickMsg = ":" + getClientNickname() + " KICK " + channelName + " " + nickname + " :" + reason + "\r\n";
    channel->removeClient(kickedClient);
    kickedClient->removeClientChannel(channelName);
    channel->broadcast(kickMsg);
    kickedClient->sendMessage(kickMsg);
}

void Client::clientModeCommand(const std::string &args, Server *server) {

    std::vector<std::string> arguments = getArgsVector(args);
    if (arguments.size() < 2) {
        sendErrorMessage("[USAGE]: /mode <channel> <mode> <nickname>");
        return;
    }

    Channel* channel = server->getChannelByName(arguments[0]);
    if (!channel) {
        sendErrorMessage("Channel " + arguments[0] + " does not exist.");
        return;
    }

    if (!channel->isOperator(this)) {
        sendErrorMessage("You are not an operator of channel " + channel->getChannelName() + ".");
        return;
    }
    
    if (arguments[1] == "+i") {
        if (arguments.size() > 2) {
            sendErrorMessage("[USAGE]: /mode <#channel> +i");
            return;
        }
        channel->setChannelInviteStatus(true);
        sendMessage(":" + getClientNickname() + " MODE " + channel->getChannelName() + " +i\r\n");
        return;
    } else if (arguments[1] == "-i") {
        if (arguments.size() > 2) {
            sendErrorMessage("[USAGE]: /mode <#channel> -i");
            return;
        }
        channel->setChannelInviteStatus(false);
        sendMessage(":" + getClientNickname() + " MODE " + channel->getChannelName() + " -i\r\n");
        return;
    }

    if (arguments[1] == "+o") {
        if (arguments.size() < 3 || arguments.size() > 3) {
            sendErrorMessage("[USAGE]: /mode +o <user>");
            return;
        }
        Client *targetClient = server->getClientByNickname(arguments[2]);
        if (!targetClient) {
            sendErrorMessage("Client " + arguments[2] + " does not exist.");
            return;
        }
        if (channel->isOperator(targetClient)) {
            sendErrorMessage("Client " + arguments[2] + " is already an operator of " + channel->getChannelName() + ".");
            return;
        }
        channel->addChannelOperators(targetClient);
        sendMessage(":" + getClientNickname() + " MODE " + channel->getChannelName() + " +o " + targetClient->getClientNickname() + "\r\n");
        return;
    } else if (arguments[1] == "-o") {
        if (arguments.size() < 3 || arguments.size() > 3) {
            sendErrorMessage("[USAGE]: /mode +o <user>");
            return;
        }
        Client *targetClient = server->getClientByNickname(arguments[2]);
        if (!targetClient) {
            sendErrorMessage("Client " + arguments[2] + " does not exist.");
            return;
        }
        channel->removeOperator(targetClient);
        sendMessage(":" + getClientNickname() + " MODE " + channel->getChannelName() + " -o " + targetClient->getClientNickname() + "\r\n");
        
        if (!channel->hasOperator()) {
            std::string channelName = channel->getChannelName();
            std::vector<Client*> clientsToRemove = channel->getChannelClients();
            for (std::vector<Client*>::iterator it = clientsToRemove.begin(); it != clientsToRemove.end(); ++it) {
                channel->removeClient(*it);
                (*it)->sendMessage(":" + (*it)->getClientNickname() + " PART " + channel->getChannelName() + "\r\n");
                (*it)->removeClientChannel(channelName);
            }
        server->removeServerChannel(channelName);
        return;
        }
    }
    
    else if (arguments[1] == "+k") {
        if (arguments.size() < 3 || arguments.size() > 3) {
            sendErrorMessage("Missing password.");
            return;
        }
        channel->setChannelPassword(arguments[2]);
        channel->setChannelProtectionStatus(true);
        sendMessage(":" + getClientNickname() + " MODE " + channel->getChannelName() + " +k " + arguments[2] + "\r\n");
        return;
    } else if (arguments[1] == "-k") {
        if (arguments.size() > 2) {
            sendErrorMessage("[USAGE]: /mode <#channel> -k");
            return;
        }
        channel->setChannelPassword("");
        channel->setChannelProtectionStatus(false);
        sendMessage(":" + getClientNickname() + " MODE " + channel->getChannelName() + " -k\r\n");
        return;
    }

    else if (arguments[1] == "+l") {
        
        if (arguments.size() < 3 || arguments.size() > 3) {
            sendErrorMessage("[USAGE]: /mode <#channel> +l <limit>");
            return;
        }

        std::string limitStr = arguments[2];
        bool isNumber = true;
        for (size_t i = 0; i < limitStr.size(); ++i) {
            if (!std::isdigit(limitStr[i])) {
                isNumber = false;
                break;
            }
        }
        
        if (!isNumber) {
            sendErrorMessage("[USAGE]: Limit must be a number.");
            return;
        }
        
        int limit = std::atoi(limitStr.c_str());
        if (limit < 0 || limit > 10) {
            sendErrorMessage("[USAGE]: Limit must be between 0 and 10.");
            return;
        }

        channel->setChannelLimitationStatus(true);
        channel->setChannelLimit(static_cast<size_t>(limit));
        sendMessage(":" + getClientNickname() + " MODE " + channel->getChannelName() + " +l " + arguments[2] + "\r\n");
        return;
    } else if (arguments[1] == "-l") {
        if (arguments.size() > 2) {
            sendErrorMessage("[USAGE]: /mode <#channel> -l");
            return;
        }
        channel->setChannelLimitationStatus(false);
        sendMessage(":" + getClientNickname() + " MODE " + channel->getChannelName() + " -l\r\n");
        return;
    }

    else if (arguments[1] == "+t") {
        if (arguments.size() > 2) {
            sendErrorMessage("[USAGE]: /mode <#channel> +t");
            return;
        }
        channel->setChannelTopicProtectionStatus(true);
        sendMessage(":" + getClientNickname() + " MODE " + channel->getChannelName() + " +t\r\n");
        return;
    } else if (arguments[1] == "-t") {
        if (arguments.size() > 2) {
            sendErrorMessage("[USAGE]: /mode <#channel> -t");
            return;
        }
        channel->setChannelTopicProtectionStatus(false);
        sendMessage(":" + getClientNickname() + " MODE " + channel->getChannelName() + " -t\r\n");
        return;
    }
}
