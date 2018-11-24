#include <clippson/clippson.hpp>

#include <limits>
#include <iostream>

bool test_roundtrip(const clipp::group& cli, nlohmann::json& vm) {
    std::string current_values = vm.dump(2);
    auto args = wtl::arg_list(vm);
    for (const auto& x: args) {std::cout << " {" << x << "}";}
    std::cout << "\n";
    if (vm.find("--") != vm.end()) vm["--"].clear();
    wtl::parse(cli, args);
    return current_values == vm.dump(2);
}

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
    std::string current_values = vm.dump(2);
    if (help) {
        auto fmt = wtl::doc_format();
        std::cout << clipp::documentation(cli, fmt) << "\n";
        return 0;
    }
    std::cout << "Default values: " << default_values << "\n";
    std::cout << "Current values: " << current_values << "\n";
    if (!test_roundtrip(cli, vm)) return 1;
    return 0;
}
