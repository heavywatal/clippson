#include <clippson/clippson.hpp>

#include <limits>
#include <iostream>

struct Parameters {
    bool BOOL = false;
    int INT = std::numeric_limits<int>::max();
    long LONG = std::numeric_limits<long>::max();
    unsigned UNSIGNED = std::numeric_limits<unsigned>::max();
    size_t SIZE_T = std::numeric_limits<size_t>::max();
    double DOUBLE = 0.0;
    std::string STRING = "Hello, world!";
    std::vector<int> VECTOR = {0, 1};

    clipp::group cli(nlohmann::json& vm) {
        return clipp::with_prefixes_short_long("-", "--",
          wtl::option(vm, {"b", "bool"}, &BOOL),
          wtl::option(vm, {"i", "int"}, &INT),
          wtl::option(vm, {"l", "long"}, &LONG),
          wtl::option(vm, {"u", "unsigned"}, &UNSIGNED),
          wtl::option(vm, {"s", "size_t"}, &SIZE_T),
          wtl::option(vm, {"d", "double"}, &DOUBLE),
          wtl::option(vm, {"c", "string"}, &STRING),
          wtl::option(vm, {"v", "vector"}, &VECTOR)
        ).doc("Notified to both json and targets:");
    }
};

int main(int argc, char* argv[]) {
    const auto program = argv[0];
    std::vector<std::string> arguments(argv + 1, argv + argc);

    bool help = false;
    int answer = 42;
    auto to_targets = clipp::with_prefixes_short_long("-", "--",
      wtl::option({"h", "help"}, &help, "Print help"),
      wtl::option({"a", "answer"}, &answer, "Answer")
    ).doc("Notified to targets:");

    nlohmann::json vm;
    auto to_json = clipp::with_prefixes_short_long("-", "--",
      wtl::option(vm, {"version"}, false, "Print version"),
      wtl::option(vm, {"whoami"}, 24601)
    ).doc("Notified to json:");

    Parameters params;
    auto to_json_and_targets = params.cli(vm);

    auto cli = (
      to_targets,
      to_json,
      to_json_and_targets
    );
    std::string default_values = vm.dump(2);
    auto parsed = clipp::parse(arguments, cli);
    clipp::debug::print(std::cerr, parsed);
    if (!parsed) {
        wtl::usage(std::cout, cli, program)
          << "Error: unknown argument\n";
        return 1;
    }
    if (help) {
        wtl::usage(std::cout, cli, program);
        return 0;
    }
    if (vm["version"]) {
        std::cout << "clipp 1.2.0\n";
        return 0;
    }
    std::cout << "Default values:\n"
              << default_values << "\n";
    std::cout << "Current values:\n"
              << vm.dump(2) << "\n";
    std::cout << "Answer: " << answer << "\n";
}
