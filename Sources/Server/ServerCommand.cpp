#include "../../Includes/Server.hpp"

bool isCommand(const std::string &cmd) {
    if (cmd == "NICK" || cmd == "USER" || cmd == "JOIN" || cmd == "PART" || cmd == "PRIVMSG" || cmd == "QUIT" || cmd == "TOPIC" || cmd == "INVITE" || cmd == "KICK" || cmd == "PASS" || cmd == "CAP" || cmd == "MODE" || cmd == "WHO")
        return true;
    else
        return false;
}

void Server::handleMessage(std::string const &msg, int user_fd) {

    std::cout << MAGENTA << "[Hexchat]: " << RESET_COLOR << msg << std::endl;
    
    if (msg.empty()) {
        getClients()[user_fd]->sendErrorMessage("Empty message");
        return;
    }
    
    size_t  spacePos = msg.find(' ');
    std::string command = msg.substr(0, spacePos);

    if (isCommand(command)) {
        std::string args = (spacePos != std::string::npos) ? msg.substr(spacePos + 1) : "";
        processClientCommand(command, args, user_fd);
    } else {
        std::cout << YELLOW << "[SERVER]:" << RESET_COLOR << command << ": is not a command" << std::endl;
    }
}

Server::commands Server::defineCommand(const std::string &command) {
    if (command == "NICK") return NICKNAME;
    if (command == "USER") return USER;
    if (command == "JOIN") return JOIN;
    if (command == "PART") return PART;
    if (command == "PRIVMSG") return PRIVMSG;
    if (command == "QUIT") return QUIT;
    if (command == "TOPIC") return TOPIC;
    if (command == "INVITE") return INVITE;
    if (command == "KICK") return KICK;
    if (command == "PASS") return PASS;
    if (command == "CAP") return CAP;
    if (command == "MODE") return MODE;
    if (command == "WHO") return WHO;
    throw std::invalid_argument("Unknown command, try </help>");
}

void Server::processClientCommand(const std::string& command, const std::string& args, int user_fd) {
    try {
        commands cmd = defineCommand(command);
        switch (cmd) {
            case NICKNAME:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing /NICKNAME command with args: " << args << std::endl;
                getClients()[user_fd]->clientNicknameCommand(args);
                break;
            case MODE:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing /MODE command with args: " << args << std::endl;
                getClients()[user_fd]->clientModeCommand(args, this);
                break;
            case WHO:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing /WHO command with args: " << args << std::endl;
                break;
            case PASS:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing /PASS command with args: " << args << std::endl;
                getClients()[user_fd]->clientPassCommand(args, this);
                break;
            case USER:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing /USER command with args: " << args << RESET_COLOR << std::endl;
                getClients()[user_fd]->clientUserCommand(args);
                break;
            case JOIN:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing /JOIN command with args: " << args << RESET_COLOR << std::endl;
                getClients()[user_fd]->clientJoinCommand(args, this);
                break;
            case PART:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing /PART command with args: " << args << RESET_COLOR << std::endl;
                getClients()[user_fd]->clientPartCommand(args, this);
                break;
            case PRIVMSG:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing /PRIVMSG command with args: " << args << RESET_COLOR << std::endl;
                getClients()[user_fd]->clientPrivmsgCommand(args, this);
                break;
            case QUIT:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing /QUIT command with args: " << args << RESET_COLOR << std::endl;
                getClients()[user_fd]->clientQuitCommand(args, this);
                break;
            case TOPIC:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing /TOPIC command with args: " << args << RESET_COLOR << std::endl;
                getClients()[user_fd]->clientTopicCommand(args, this);
                break;
            case INVITE:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing /INVITE command with args: " << args << RESET_COLOR << std::endl;
                getClients()[user_fd]->clientInviteCommand(args, this);
                break;
            case KICK:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing /KICK command with args: " << args << RESET_COLOR << std::endl;
                getClients()[user_fd]->clientKickCommand(args, this);
                break;
            case CAP:
                std::cout << YELLOW << "[SERVER]: " << RESET_COLOR << "Processing /CAP command with args: " << args << RESET_COLOR << std::endl;
                break;
            default:
                std::string errorMsg = "Unknown command: " + command;
                send(user_fd, errorMsg.c_str(), errorMsg.length(), 0);
                break;
        }
    } catch (const std::invalid_argument& e) {
        std::string errorMsg = "Unknown command: " + command;
        send(user_fd, errorMsg.c_str(), errorMsg.length(), 0);
    }
}