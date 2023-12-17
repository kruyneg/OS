#include <vector>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <iostream>

using graph = std::vector<std::vector<int>>;

graph create_graph(boost::property_tree::ptree& tree, std::vector<int>& ends) {
    graph res(tree.size());
    for (auto [id, child] : tree) {
        std::stringstream s {child.get<std::string>("next", "")};

        int u = std::stoi(id) - 1;
        if (s.str().empty()) {
            ends.push_back(u);
            continue;
        }

        int v;
        while (s >> v) {
            --v;
            res[v].push_back(u);

            if (s.peek() == ',') {
                s.ignore();
            }
        }
    }
    return res;
}

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


bool check_cycle(graph& g, std::vector<int>& starts) {
    std::vector<int> visited(g.size());
    bool res = true;
    for (int i = 0; i < starts.size(); ++i) {
        res &= !find_cycle(g, starts[i], visited);
    }
    return res;
}

bool check_connectivity(graph& g, std::vector<int>& starts) {
    std::vector<int> visited(g.size());
    bool res = true;
    dfs(g, starts[0], visited);
    for (int i = 1; i < starts.size(); ++i) {
        res &= !find_components(g, starts[i], visited);
    }
    return res;
}

int main(int argc, char** argv) {
    boost::property_tree::ptree dag_ptree;
    boost::property_tree::ini_parser::read_ini("/home/kruyneg/Programming/OS/course_work/src/dag.ini", dag_ptree);

    // for (auto elem : dag_ptree) {
    //     std::cout << elem.first << std::endl;
    //     for (auto el : elem.second) {
    //         std::cout << el.first << std::endl;
    //         std:: cout << elem.second.get<std::string>(el.first) << ' ';
    //         std::cout << elem.second.get<std::string>(el.first, "0") << ' ';
    //         std::cout << std::endl;
    //     }
    // }

    std::vector<int> end_points, start_points;
    graph g = create_graph(dag_ptree, end_points);

    for (int i = 0; i < g.size(); ++i) {
        if (g[i].empty())
            start_points.push_back(i);
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

}
