#include <clippson/clippson.hpp>

#include <limits>
#include <iostream>

int main(int argc, char* argv[]) {
    nlohmann::json vm;
    bool help = false;
    bool BOOL = false;
    int INT = std::numeric_limits<int>::max();
    long LONG = std::numeric_limits<long>::max();
    unsigned UNSIGNED = std::numeric_limits<unsigned>::max();
    size_t SIZE_T = std::numeric_limits<size_t>::max();
    double DOUBLE = std::numeric_limits<double>::max();
    std::string STRING = "Hello, world!";
    std::vector<int> VECTOR = {0, 1};
    auto cli = wtl::option(&vm, {"h", "help"}, &help, "Print help") | (
      wtl::value<bool>(&vm, "bool", &BOOL),
      wtl::value<int>(&vm, "int", &INT),
      wtl::value<long>(&vm, "long", &LONG),
      wtl::value<unsigned>(&vm, "unsigned", &UNSIGNED),
      wtl::value<size_t>(&vm, "size_t", &SIZE_T),
      wtl::value<double>(&vm, "double", &DOUBLE),
      wtl::value<std::string>(&vm, "string", &STRING),
      wtl::value<std::vector<int>>(&vm, "vector", &VECTOR)
    ).doc("Positional targets");
    std::string default_values = vm.dump(2);
    wtl::parse(cli, argc, argv);
    if (help) {
        auto fmt = wtl::doc_format();
        std::cout << clipp::documentation(cli, fmt) << "\n";
        return 0;
    }
    std::cout << "Default values: " << default_values << "\n";
    std::cout << "Current values: " << vm.dump(2) << "\n";
    auto args = wtl::arg_list(vm);
    std::cout << argv[0];
    for (const auto& x: args) {std::cout << " {" << x << "}";}
    std::cout << "\n";
    if (vm.find("--") != vm.end()) vm["--"].clear();
    wtl::parse(cli, args);
    std::cout << "Round trip: " << vm.dump(2) << "\n";
    return 0;
}
