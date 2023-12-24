#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <wait.h>

#include <iostream>
#include <thread>
#include <mutex>

using graph = std::vector<std::unordered_set<int>>;

// Создание графа и множества конечных джобов из property tree 
graph create_graph(boost::property_tree::ptree& tree, std::unordered_set<int>& ends) {
    graph res(tree.size());
    for (auto [id, child] : tree) {
        std::stringstream s {child.get<std::string>("next", "")};

        int u = std::stoi(id) - 1;
        if (s.str().empty()) {
            ends.insert(u);
            continue;
        }

        int v;
        while (s >> v) {
            --v;
            res[v].insert(u);

            if (s.peek() == ',') {
                s.ignore();
            }
        }
    }
    return res;
}
/* поиск цикла в графе
* @return true если нашел, false если не нашел.
*/
bool find_cycle(graph& g, int u, std::vector<int>& visited) {
    if (visited[u] == 1) {
        return true;
    }
    if (visited[u] == 2) {
        return false;
    }
    visited[u] = 1;
    bool res = false;
    for (int v : g[u]) {
        res |= find_cycle(g, v, visited);
    }
    visited[u] = 2;
    return res;
}

// обход дерева в глубину и отметка посещённых вершин
void dfs(graph& g, int u, std::vector<int>& visited) {
    if (visited[u] == 1) {
        return;
    }
    visited[u] = 1;
    bool res = true;
    for (int v : g[u]) {
        dfs(g, v, visited);
    }
}

/* ищет компоненты отделенные от графа
* @return true, если текущий компонент не связан с графом, false, если связь есть.
*/
bool find_components(graph& g, int u, std::vector<int>& visited) {
    if (visited[u] == 1) {
        return false;
    }
    if (g[u].empty()) {
        return true;
    }
    visited[u] = 1;
    bool res = true;
    for (int v : g[u]) {
        res &= find_components(g, v, visited);
    }
    return res;
}

// проверка графа на отсутствие циклов
bool check_cycle(graph& g, std::unordered_set<int>& starts) {
    std::vector<int> visited(g.size());
    bool res = true;
    for (int start : starts) {
        res &= !find_cycle(g, start, visited);
    }
    return res;
}
// проверка графа на одну компоненту связности
bool check_connectivity(graph& g, std::unordered_set<int>& starts) {
    std::vector<int> visited(g.size());
    bool res = true;
    dfs(g, *(starts.begin()), visited);
    for (auto itr = std::next(starts.begin(), 1); itr != starts.end(); ++itr) {
        res &= !find_components(g, *itr, visited);
    }
    return res;
}

/* создание джобы (процесса)
* @param Команда  путь к выполняемому файлу, который необходимо запустить.
* @return pid созданного процесса.
*/
pid_t create_job(std::string command) {
    std::stringstream ss {command};
    std::vector<std::string> strargs;
    std::vector<const char*> args;
    std::string str;
    while (ss >> str) {
        strargs.push_back(str);
        args.push_back(str.c_str());
    }

    pid_t pid = fork();
    if (pid == 0) {
        execv(args[0], const_cast<char * const *>(args.data()));
        pid = -2;
        std::cerr << "exec error" << std::endl;
        // TODO: отправка ошибки
        exit(-2);
    }

    std::cout << "create " << pid << ": " << command << std::endl;
    return pid;
}
/* убивает все запущенные процессы
* используется в случае ошибки
*/
void killall_jobs(std::queue<std::pair<int, pid_t>>& q) {
    while(!q.empty()) {
        auto [id, pid] = q.front();
        q.pop();
        kill(pid, SIGTERM);
    }
}

int main(int argc, char** argv) {
    // execl("/bin/sh", "sh", "-c", "ls ../src", nullptr);
    // execl("/bin/sh", "sh", "-c", "/home/kruyneg/Programming/OOP/build/lab_07", nullptr);
    // чтение из файла
    boost::property_tree::ptree dag_ptree;
    boost::property_tree::ini_parser::read_ini("/home/kruyneg/Programming/OS/course_work/src/dag.ini", dag_ptree);

#ifdef _DEBUG
    for (auto elem : dag_ptree) {
        std::cout << elem.first << std::endl;
        for (auto el : elem.second) {
            std::cout << "  " <<  el.first << std::endl;
            std:: cout << "    " << elem.second.get<std::string>(el.first) << std::endl;
            std::cout << std::endl;
        }
    }
#endif

    std::unordered_set<int> end_points, start_points;
    graph g = create_graph(dag_ptree, end_points);

    for (int i = 0; i < g.size(); ++i) {
        if (g[i].empty())
            start_points.insert(i);
    }
    
    if (start_points.empty()) {
        std::cerr << "Error: Отсутствуют начальные джобы" << std::endl;
        return -1;
    }
    if (end_points.empty()) {
        std::cerr << "Error: Отсутствуют завершающие джобы" << std::endl;
        return -1;
    }

    if (!check_cycle(g, end_points)) {
        std::cerr << "Error: В графе есть циклы" << std::endl;
        return -1;
    }
    
    if (!check_connectivity(g, end_points)) {
        std::cerr << "Error: В графе больше одной компоненты связности" << std::endl;
        return -1;
    }

    // TODO: мьютексы

    std::unordered_map<std::string, bool> mutex_vals;
    std::unordered_map<int, std::string> mutex_names;

    for (auto [id, node] : dag_ptree) {
        std::string mutex_name = node.get<std::string>("mutex", "");
        if (!mutex_name.empty()) {
            mutex_names[std::stoi(id) - 1] = mutex_name;
            mutex_vals[mutex_name] = true;
        }
    }

    // mutex

    std::queue<std::pair<int, pid_t>> waitq;
    std::mutex qmtx;

    std::thread wait_thread([&](){
        while (!end_points.empty() || !waitq.empty()) {
            if (!waitq.empty()) {
                pid_t pid;
                int id;
                {
                    std::lock_guard<std::mutex> lock(qmtx);
                    id = waitq.front().first;
                    pid = waitq.front().second;
                    waitq.pop();
                }
                if (pid != -1) {
                    int sig;

                    std::cout << "wait " << pid << std::endl;
                    waitpid(pid, &sig, 0);
                    if (WIFSIGNALED(sig)) {
                        std::cout << "signal is heared" << std::endl;
                        // std::lock_guard<std::mutex> lock(qmtx);
                        // end_points = {};
                        // killall_jobs(waitq);
                    }
                }

                // std::cout << id << ' ' << pid << std::endl;

                {
                    std::lock_guard<std::mutex> lock(qmtx);
                    mutex_vals[mutex_names[id]] = true;
                }

                for (int i = 0; i < g.size(); ++i) {
                    if (g[i].count(id)) {
                        g[i].erase(id);
                        if (g[i].empty()) {
                            std::lock_guard<std::mutex> lock(qmtx);
                            start_points.insert(i);
                        }
                    }
                }
                end_points.erase(id);
            }
        }
    });

    while (!end_points.empty()) {
        std::vector<int> erase_id;
        std::lock_guard<std::mutex> lock(qmtx);
        for (int id : start_points) {
            if ((mutex_names.count(id) &&  mutex_vals[mutex_names[id]]) || !mutex_names.count(id)) {
                std::string command = dag_ptree.get_child(std::to_string(id + 1)).get<std::string>("command");
                if (command.front() != '.' && command.front() != '/') {
                    int success = std::system(command.c_str());
                    if (!success) {
                        killall_jobs(waitq);
                    }
                    erase_id.push_back(id);
                    waitq.push({id, -1});
                }
                else {
                    pid_t pid = create_job(command);

                    if (mutex_names.count(id))
                        mutex_vals[mutex_names[id]] = false;

                    erase_id.push_back(id);

                    // std::lock_guard<std::mutex> lock(qmtx);
                    waitq.push({id, pid});
                }
            }
        }
        for (int id : erase_id) {
            start_points.erase(id);
        }
    }

    killall_jobs(waitq);

    wait_thread.join();
}
