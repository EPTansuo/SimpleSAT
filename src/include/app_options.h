#include <string>

// We can accelerate the building process
namespace ssat{

struct AppOptions
{
    std::string cnf_file;
    std::string log_level;
    std::string method;
};

AppOptions parse_app_options(int argc, char** argv);

};