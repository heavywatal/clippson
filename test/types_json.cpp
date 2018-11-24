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
    auto cli = (
      wtl::option(&vm, {"h", "help"}, false, "Print help"),
      wtl::option(&vm, {"b", "bool"}, false),
      wtl::option(&vm, {"i", "int"}, std::numeric_limits<int>::max()),
      wtl::option(&vm, {"l", "long"}, std::numeric_limits<long>::max()),
      wtl::option(&vm, {"u", "unsigned"}, std::numeric_limits<unsigned>::max()),
      wtl::option(&vm, {"s", "size_t"}, std::numeric_limits<size_t>::max()),
      wtl::option(&vm, {"d", "double"}, std::numeric_limits<double>::max()),
      wtl::option(&vm, {"c", "string"}, "Hello, world!"),
      wtl::option(&vm, {"v", "vector"}, std::vector<int>({0, 1}))
    ).doc("Notified to json:");
    std::string default_values = vm.dump(2);
    wtl::parse(cli, argc, argv);
    std::string current_values = vm.dump(2);
    if (vm["help"]) {
        auto fmt = wtl::doc_format();
        std::cout << clipp::documentation(cli, fmt) << "\n";
    }
    std::cout << "Default values: " << default_values << "\n";
    std::cout << "Current values: " << current_values << "\n";
    if (!test_roundtrip(cli, vm)) return 1;
    return 0;
}
