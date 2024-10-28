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
    auto cli = (
      clippson::option(&vm, {"h", "help"}, false, "Print help"),
      clippson::option(&vm, {"b", "bool"}, false),
      clippson::option(&vm, {"i", "int"}, std::numeric_limits<int>::max()),
      clippson::option(&vm, {"l", "long"}, std::numeric_limits<long>::max()),
      clippson::option(&vm, {"u", "unsigned"}, std::numeric_limits<unsigned>::max()),
      clippson::option(&vm, {"s", "size_t"}, std::numeric_limits<size_t>::max()),
      clippson::option(&vm, {"d", "double"}, std::numeric_limits<double>::max()),
      clippson::option(&vm, {"c", "string"}, "Hello, world!"),
      clippson::option(&vm, {"v", "vector"}, std::vector<int>({0, 1}))
    ).doc("Notified to json:");
    std::string default_values = vm.dump(2);
    clippson::parse(cli, argc, argv);
    std::string current_values = vm.dump(2);
    if (vm["help"]) {
        auto fmt = clippson::doc_format();
        std::cout << clipp::documentation(cli, fmt) << "\n";
    }
    std::cout << "Default values: " << default_values << "\n";
    std::cout << "Current values: " << current_values << "\n";
    if (!test_roundtrip(cli, vm)) return 1;
    return 0;
}
