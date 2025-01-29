#include "solver.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <unistd.h>
#include <thread>
#include <fstream>
using namespace boost;


typedef adjacency_list<vecS, vecS, directedS> Graph;

namespace ssat{

// 生成 DOT 字符串的函数
sapy::PString Solver::_BDD_to_dot(const BDD& bdd) const{
    sapy::PString dot = "digraph G {\n";



    // sapy::PSet visited;
    sapy::PSet edges;
    // _traverse_BDD(bdd, edges, visited);
    for(BDDNode bddnode: bdd){
        LOG_DETAIL("Visit node: {}", bddnode.first.toString());
        Variable varname = bddnode.first;
        BDDNodeVar nvar = bddnode.second;
        if(nvar[1].is<BDD>()){
            edges.add(sapy::PList{varname, nvar[1].unwrap<BDD>().begin()->first});
        }else{
            edges.add(sapy::PList{varname, nvar[1]});
        }
        if(nvar[2].is<BDD>()){
            edges.add(sapy::PList{varname, nvar[2].unwrap<BDD>().begin()->first});
        }else{
            edges.add(sapy::PList{varname, nvar[2]});
        }
    }
    

    for(auto edge_wrap: edges) {
        sapy::PList edge = edge_wrap;
        const sapy::PString quote_l = digit_variable_ ? "\"\\\"" : "";
        const sapy::PString quote_r = digit_variable_ ? "\\\"\"" : "";
        sapy::PString out = quote_l + edge[0].toString() +  quote_r;
        sapy::PString in = edge[1].is<Variable>() ? quote_l + edge[1].toString() +  quote_r:
                           edge[1].toString();

        dot += out + " -> " + in + ";\n";
    }

    dot += "}\n";
    LOG_INFO("DOT: {}", dot);
    return dot;
}

void Solver::_show_dot(const sapy::PString& dot, bool blocking) const {

    char temp_filename[] = "/tmp/xdot_temp_XXXXXX";
    int fd = mkstemp(temp_filename); 
    if (fd == -1) {
        std::cerr << "Error: Failed to create temporary file." << std::endl;
        return;
    }
    close(fd);  // use fstream instead of FILE*

    std::ofstream temp_file(temp_filename);
    if (!temp_file) {
        std::cerr << "Error: Failed to open temporary file for writing." << std::endl;
        return;
    }
    temp_file << dot;
    temp_file.close();

    auto show_dot_task = [temp_filename]() {
        std::string command = "xdot " + std::string(temp_filename);
        int result = system(command.c_str());
        if (result != 0) {
            std::cerr << "Error: Failed to execute xdot." << std::endl;
        }
        unlink(temp_filename);
    };

    if (blocking) {
        show_dot_task();
    } else {
        std::thread(show_dot_task).detach();
    }
}

void Solver::_show_BDD(const BDD& bdd, bool block) const{
    _show_dot(_BDD_to_dot(bdd), block);
}

}