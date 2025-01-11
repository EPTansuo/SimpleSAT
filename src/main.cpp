#include <iostream>
#include <string>
#include <solver.hpp>

using namespace ssat;

int main(int argc, char**argv) {
    Logger::getInstance().setLogLevel(LogLevel::INFO);

    //Solver solver("/home/han/Disk/Document/PROJECT/C++/SAT/sat_v.cnf");
    Solver solver; 
    if(argc == 1){
        solver.readCNF("/home/han/Disk/Document/PROJECT/C++/SAT/sat_v.cnf");
    }
    else{
        solver.readCNF(argv[1]);
    }

    Result result = solver.solve();
    std::cout << solver << std::endl;
    std::cout << "Result: " << result << std::endl;
    if(result == Result::ERROR){
        return 2;
    }
    return result == Result::SAT;
}
