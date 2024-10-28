#include <clippson/clippson.hpp>

#include <limits>
#include <iostream>

bool test_roundtrip(const clipp::group& cli, nlohmann::json& vm) {
    std::string current_values = vm.dump(2);
    auto args = clippson::arg_list(vm);
    for (const auto& x: args) {std::cout << " {" << x << "}";}
    std::cout << "\n";
    if (vm.find("--") != vm.end()) vm["--"].clear();
    clippson::parse(cli, args);
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
    auto cli = clippson::option(&vm, {"h", "help"}, &help, "Print help") | (
      clippson::value<bool>(&vm, "bool", &BOOL),
      clippson::value<int>(&vm, "int", &INT),
      clippson::value<long>(&vm, "long", &LONG),
      clippson::value<unsigned>(&vm, "unsigned", &UNSIGNED),
      clippson::value<size_t>(&vm, "size_t", &SIZE_T),
      clippson::value<double>(&vm, "double", &DOUBLE),
      clippson::value<std::string>(&vm, "string", &STRING),
      clippson::value<std::vector<int>>(&vm, "vector", &VECTOR)
    ).doc("Positional targets");
    std::string default_values = vm.dump(2);
    clippson::parse(cli, argc, argv);
    std::string current_values = vm.dump(2);
    if (help) {
        auto fmt = clippson::doc_format();
        std::cout << clipp::documentation(cli, fmt) << "\n";
        return 0;
    }
    std::cout << "Default values: " << default_values << "\n";
    std::cout << "Current values: " << current_values << "\n";
    if (!test_roundtrip(cli, vm)) return 1;
    return 0;
}
