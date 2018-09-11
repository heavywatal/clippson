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
};

inline clipp::group test_types(nlohmann::json& vm, Parameters& p) {
    return clipp::with_prefixes_short_long("-", "--",
      wtl::option_bool(vm, {"b", "bool"}),
      wtl::option(vm, {"i", "int"}, &p.INT),
      wtl::option(vm, {"l", "long"}, &p.LONG),
      wtl::option(vm, {"u", "unsigned"}, &p.UNSIGNED),
      wtl::option(vm, {"s", "size_t"}, &p.SIZE_T),
      wtl::option(vm, {"d", "double"}, &p.DOUBLE),
      wtl::option(vm, {"c", "string"}, &p.STRING)
    ).doc("Supported types:");
}

int main(int argc, char* argv[]) {
    const auto program = argv[0];
    std::vector<std::string> arguments(argv + 1, argv + argc);

    bool help = false;
    bool version = false;
    int answer = 42;
    clipp::group general_options = clipp::with_prefixes_short_long("-", "--",
      wtl::option_bool({"h", "help"}, &help, "Print help"),
      wtl::option_bool({"version"}, &version, "Print version"),
      wtl::option({"a", "answer"}, &answer, "Answer")
    ).doc("Not stored in json:");

    nlohmann::json vm;
    Parameters p;
    auto cli = (
      general_options,
      test_types(vm, p)
    );
    auto parsed = clipp::parse(arguments, cli);
    if (!parsed) {
        wtl::usage(std::cout, cli, program)
          << "Error: unknown argument\n";
        return 1;
    }
    clipp::debug::print(std::cerr, parsed);
    if (help) {
        wtl::usage(std::cout, cli, program);
        return 0;
    }
    if (version) {
        std::cout << "clipp 1.2.0\n";
        return 0;
    }
    std::cerr << "Answer: " << answer << "\n";
    std::cout << vm.dump(2) << "\n";
}
