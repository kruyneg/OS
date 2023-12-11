#include <iostream>
#include <chrono>
#include <zmqpp/zmqpp.hpp>

int main(int argc, char** argv) {
    std::string my_port = argv[1];

    std::cout << "Hi! My port is " << my_port << std::endl;

    zmqpp::context context;
    zmqpp::socket socket(context, zmqpp::socket_type::reply);

    try {
        socket.bind(my_port);
    }
    catch (zmqpp::exception e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    std::chrono::milliseconds time {0};
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    bool is_stopped = true;

    while (true) {
        std::string message;
        socket.receive(message);

        std::stringstream response;
        if (message == "start") {
            start_time = std::chrono::steady_clock::now();
            is_stopped = false;
        }
        else if (message == "stop") {
            auto end_time = std::chrono::steady_clock::now();
            time += std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            is_stopped = true;
        }
        else if (message == "time") {
            auto end_time = std::chrono::steady_clock::now();
            if (is_stopped)
                response << time;
            else
                response << time + std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        }
        else if (message == "alive?") {
            response << "yes";
        }
        else {
            response << "Unknown subcommand";
        }

        // std::cout << my_port << " got: " << message << std::endl;
        socket.send(response.str());
    }
}
