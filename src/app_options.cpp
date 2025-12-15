#include <app_options.h>
#include <argparse/argparse.hpp>
#include <log.hpp>
#include <methods.hpp>

namespace ssat {

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

AppOptions parse_app_options(int argc, char** argv){
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
            .default_value("DPLL");
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
    auto method = program.get<std::string>("-m");
    return AppOptions{cnf_file, log_level, method};
}

}