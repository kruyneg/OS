#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <string>
#include <map>

std::string get_port(int id) {
    std::string port = "tcp://localhost:";
    port += std::to_string(50000 + id);
    return port;
}

pid_t create_timer(std::map<int, pid_t>& conteiner, int& id) {
    pid_t pid = fork();
    if (pid == -1) {
        std::cout << "Error: can't fork" << std::endl;
    }
    if (pid == 0) {
        execl("/home/kruyneg/Programming/OS/build/timer", "./timer", get_port(id).c_str(), nullptr);

        std::cout << "Error: execl" << std::endl;
    }
    else {
        conteiner.insert({id, pid});
    }
    return pid;
}

int main() {
    zmqpp::context context;
    zmqpp::socket socket(context, zmqpp::socket_type::req);
    socket.set(zmqpp::socket_option::receive_timeout, 1000);
    socket.set(zmqpp::socket_option::linger, 1000);

    std::map<int, pid_t> servers;

    std::string query;
    do {
        std::cin >> query;
        if (query == "create") {
            int id;
            std::cin >> id;

            create_timer(servers, id);
        }
        else if (query == "exec") {
            int id;
            std::cin >> id;
            std::string subcommand;
            std::cin >> subcommand;
            if (servers.count(id)) {
                try {
                    socket.connect(get_port(id));
                    
                    socket.send(subcommand);

                    std::string buffer;
                    socket.receive(buffer);

                    std::cout << "Ok:" << id;
                    if (!buffer.empty())
                        std::cout << ": " << buffer;
                    std::cout << std::endl;

                    socket.disconnect(get_port(id));
                }
                catch(zmqpp::exception& exc) {
                    std::cout << "Exception: " << exc.what() << std::endl;
                }
            }
            else {
                std::cout << "Error: Not found" << std::endl;
            }
        }
        else if (query == "ping") {
            int id;
            std::cin >> id;
            if (servers.count(id)) {
                try {
                    socket.connect(get_port(id));
                    socket.send("alive?");

                    std::string buffer;
                    if (socket.receive(buffer, false)) {
                        std::cout << "Ok:" << id << ": 1" << std::endl;
                        socket.disconnect(get_port(id));
                    }
                    else {
                        std::cout << "Ok:" << id << ": 0" << std::endl;
                        socket.close();
                        socket = zmqpp::socket(context, zmqpp::socket_type::req);
                        socket.set(zmqpp::socket_option::receive_timeout, 1000);
                        socket.set(zmqpp::socket_option::linger, 1000);
                    }
                }
                catch (zmqpp::exception& exc) {
                    std::cout << "Ok:" << id << " 0 " << exc.what() << std::endl;
                }
            }
            else {
                std::cout << "Error: Not found" << std::endl;
            }
        }
        else if (query == "kill") {
            int id;
            std::cin >> id;
            if (servers.count(id)) {
                if (kill(servers[id], SIGINT) == -1) {
                    std::cout << "Error: can't kill" << std::endl;
                }
                servers.erase(id);
            }
            else {
                std::cout << "Error: Not found" << std::endl;
            }
        }
    } while (query != "exit" && query != "quit");

    for (auto& elem : servers) {
        kill(elem.second, SIGINT);
    }
    socket.close();
    return 0;
}
