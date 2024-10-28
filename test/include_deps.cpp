#include <clippson/clipp.h>
#include <clippson/json.hpp>

#include <iostream>

int main(int argc, char* argv[]) {
    bool help;
    bool verbose;
    auto cli = (
        clipp::option("-h", "--help").set(help).doc("print help"),
        clipp::option("-v", "--verbose").set(verbose)
    );
    clipp::parse(argc, argv, cli);
    nlohmann::json vm;
    vm["help"] = help;
    vm["verbose"] = verbose;
    std::cout << vm << std::endl;
    return 0;
}
