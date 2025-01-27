#include <iostream>
#include <string>
#include <solver.hpp>

using namespace ssat;

int main(int argc, char**argv) {
    Logger::getInstance().setLogLevel(LogLevel::NONE);

    //Solver solver("/home/han/Disk/Document/PROJECT/C++/SAT/sat_v.cnf");
    Solver solver; 
    if(argc == 1){
        solver.readCNF("/home/han/Disk/Document/PROJECT/C++/SAT/sat_v.cnf");
    }
    else{
        solver.readCNF(argv[1]);
        LOG_INFO("Read file: {}", argv[1]);
    }

    std::cout << solver << std::endl;
    std::cout << "Val cnt: " << solver.getValCnt() << std::endl;
    Result result = solver.solve(Solver::DPLL_CLASSIC);
    // std::cout << solver << std::endl;
    std::cout << "Result: " << result << std::endl;
    LOG_INFO("Result: {}", result.toString());
    
    if(result == Result::SAT){
        return 10;
    }else if(result == Result::UNSAT){
        return 20;
    }else if(result == Result::UNKNOWN){
        return 30;
    }else{
        return 40;
    }
    
}
