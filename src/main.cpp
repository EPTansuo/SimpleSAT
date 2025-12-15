#include "methods.hpp"
#include <log.hpp>
#include <iostream>
#include <parse.hpp>
#include <factory.hpp>
#include <app_options.h>


int main(int argc, char**argv) {
    
    auto opttions = ssat::parse_app_options(argc, argv);

    auto cnf_file = opttions.cnf_file;
    auto log_level = opttions.log_level;
    auto method = ssat::method_from_string(opttions.method);

    ssat::Logger::getInstance().setLogLevel(log_level);

    ssat::Formula formula;
    try {
        formula = ssat::parse_dimacs_file(cnf_file);
    } catch (const ssat::DimacsParseError& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    auto solver = ssat::make_solver(method);
    
    ssat::Result result = solver->solve(formula);
    
    std::cout << "Result: " << result << std::endl;
    LOG_INFO("Result: {}", result.toString());
    
    if(result == ssat::Result::SAT){
        return 10;
    }else if(result == ssat::Result::UNSAT){
        return 20;
    }else if(result == ssat::Result::UNKNOWN){
        return 30;
    }else{
        return 40;
    }
    
}
