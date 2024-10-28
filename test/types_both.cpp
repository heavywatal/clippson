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
    auto cli = (
      clippson::option(&vm, {"h", "help"}, &help, "Print help"),
      clippson::option(&vm, {"b", "bool"}, &BOOL),
      clippson::option(&vm, {"i", "int"}, &INT),
      clippson::option(&vm, {"l", "long"}, &LONG),
      clippson::option(&vm, {"u", "unsigned"}, &UNSIGNED),
      clippson::option(&vm, {"s", "size_t"}, &SIZE_T),
      clippson::option(&vm, {"d", "double"}, &DOUBLE),
      clippson::option(&vm, {"c", "string"}, &STRING),
      clippson::option(&vm, {"v", "vector"}, &VECTOR)
    ).doc("Notified to both json and targets:");
    std::string default_values = vm.dump(2);
    clippson::parse(cli, argc, argv);
    std::string current_values = vm.dump(2);
    if (help) {
        auto fmt = clippson::doc_format();
        std::cout << clipp::documentation(cli, fmt) << "\n";
    }
    std::cout << "Default values: " << default_values << "\n";
    std::cout << "Current values: " << current_values << "\n";
    if (!test_roundtrip(cli, vm)) return 1;
    return 0;
}
