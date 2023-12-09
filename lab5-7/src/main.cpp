#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <string>
#include <signal.h>
// TODO: tree
#include <map>

std::string get_port(int id) {
    std::string port = "tcp://localhost:500";
    if (id < 10) {
        port += '0';
        port += std::to_string(id);
    }
    else
        port += std::to_string(id);
    return port;
}

pid_t create_timer(std::map<int, pid_t>& conteiner, int& id) {
    pid_t pid = fork();
    if (pid == 0) {
        execl("/home/kruyneg/Programming/OS/build/timer", "./timer", get_port(id).c_str(), nullptr);

        std::cout << "Я умер..." << std::endl;
    }
    else {
        conteiner.insert({id, pid});
    }
    return pid;
}

bool process_exist(pid_t& pid) {
    return kill(pid, 0) == 0;
}

int main() {
    zmqpp::context context;
    zmqpp::socket socket(context, zmqpp::socket_type::req);

    // TODO: tree
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
                catch(zmqpp::exception exc) {
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
                std::cout << "Ok:" << id << ": " << process_exist(servers[id]) << std::endl;
                auto res = process_exist(servers[id]);
            }
            else {
                std::cout << "Error: Not found" << std::endl;
            }
        }
        else if (query == "kill") {
            int id;
            std::cin >> id;
            kill(servers[id], SIGINT);
            servers.erase(id);
        }
    } while (query != "exit" && query != "quit");

    for (auto& elem : servers) {
        kill(elem.second, SIGINT);
    }
}
