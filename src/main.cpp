#include <iostream>
#include <string>
#include <solver.hpp>
#include <argparse/argparse.hpp>

using namespace ssat;

int main(int argc, char**argv) {
    argparse::ArgumentParser program("SimpleSAT");
    program.add_argument("cnf_file")               
           .help("cnf file path")             
           .required(); 
    
    program.add_argument("--log")               
           .help("log level: NONE|ERROR|WARN|INFO|DEBUG|DETAIL")                  
           .default_value(std::string("INFO"))
           .required()
           .action([](const std::string& value) {
               static const std::vector<std::string> choices = {"DETAIL", "DEBUG", "INFO", "WARN", "ERROR", "NONE"};
               if (std::find(choices.begin(), choices.end(), value) == choices.end()) {
                   throw std::runtime_error("Invalid value for --log: " + value);
               }
               return value;
           });
    
    

    
    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        std::cout << program;
        exit(0);
    }

    auto log_level = program.get<std::string>("--log");
    auto cnf = program.get<std::string>("cnf_file");

    Logger::getInstance().setLogLevel(log_level);

    Solver solver; 
    solver.readCNF(cnf);
    LOG_INFO("Read file: {}", argv[1]);

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
