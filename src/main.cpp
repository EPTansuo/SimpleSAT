#include <log.hpp>
#include <iostream>
#include <argparse/argparse.hpp>
#include <parse.hpp>
#include <factory.hpp>

template <class Argument, class Tuple>
Argument& choices_from_tuple(Argument& arg, const Tuple& tup) {
    std::apply([&](auto&&... xs) { arg.choices(xs...); }, tup);
    return arg;
}
template <class Range>
std::string join(std::string_view sep, const Range& range) {
    std::string result;
    bool first = true;
    for (const auto& item : range) {
        if (!first) {
            result += sep;
        }
        result += item;
        first = false;
    }
    return result;
}

int main(int argc, char**argv) {
    argparse::ArgumentParser program("SimpleSAT","0.0.2");
    program.add_argument("cnf_file")               
           .help("cnf file path")             
           .required(); 
    
    auto &log_arg = program.add_argument("--log","-l")     
           .help("Log level: " + join("|", ssat::Logger::logLevelNames))
           .default_value("INFO");
    choices_from_tuple(log_arg, ssat::Logger::logLevelChoicesTuple);
           

    auto &method_arg = program.add_argument("--method","-m")
            .help("SAT method: " + join("|", ssat::MethodNames))
            .default_value(ssat::MethodNames[0]);
    choices_from_tuple(method_arg, ssat::MethodChoicesTuple);

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        std::cout << program;
        exit(0);
    }

    auto log_level = program.get<std::string>("--log");
    auto cnf_file = program.get<std::string>("cnf_file");
    auto method = ssat::method_from_string(program.get<std::string>("-m"));

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
