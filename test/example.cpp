#include <clippson/clippson.hpp>

#include <iostream>

struct Parameters {
    unsigned taxicab = 1729;
    std::string say = "Hello, world!";
    std::vector<int> vec = {0, 1};

    clipp::group cli(nlohmann::json* vm) {
        return (
          wtl::option(vm, {"u", "unsigned"}, &taxicab),
          wtl::option(vm, {"c", "string"}, &say),
          wtl::option(vm, {"v", "vector"}, &vec)
        ).doc("Notified to both json and targets:");
    }
};

int main(int argc, char* argv[]) {
    bool help = false;
    int answer = 42;
    auto to_targets = (
      wtl::option({"h", "help"}, &help, "Print help"),
      wtl::option({"a", "answer"}, &answer, "Answer")
    ).doc("Notified to targets:");

    nlohmann::json vm;
    auto to_json = (
      wtl::option(&vm, {"version"}, false, "Print version"),
      wtl::option(&vm, {"whoami"}, {"24601", "Jean", "Javert"}, "24601"),
      wtl::option(&vm, {"year", "y"}, 2112)
    ).doc("Notified to json:");

    Parameters params;
    auto to_json_and_targets = params.cli(&vm);

    int nsam = 0;
    auto positional = (
      wtl::value<int>(&vm, "nsam", &nsam).doc("Number of samples"),
      wtl::value<std::string>(&vm, "howmany") % "tears"
    ).doc("Positional (required):");

    auto cli = clipp::joinable(
      // positional,
      to_targets,
      to_json,
      to_json_and_targets
    );
    std::string default_values = vm.dump(2);
    wtl::parse(cli, argc, argv);
    if (help) {
        auto fmt = wtl::doc_format();
        std::cout << clipp::documentation(cli, fmt) << "\n";
        return 0;
    }
    if (vm["version"]) {
        std::cout << "clipp 1.2.2\n";
        return 0;
    }
    std::cout << "Default values: " << default_values << "\n";
    std::cout << "Current values: " << vm.dump(2) << "\n";
    return 0;
}
