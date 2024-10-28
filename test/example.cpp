#include <clippson/clippson.hpp>

#include <iostream>

int main(int argc, char* argv[]) {
    bool help = false;
    int answer = 42;
    auto to_targets = (
      clippson::option({"h", "help"}, &help, "Print help"),
      clippson::option({"a", "answer"}, &answer, "Answer")
    ).doc("Notified to targets:");

    nlohmann::json vm;
    auto to_json = (
      clippson::option(&vm, {"version"}, false, "Print version"),
      clippson::option(&vm, {"ratio"}, 1.618)
    ).doc("Notified to json:");

    int taxicab = 1729;
    std::vector<int> seq = {1, 1, 2};
    auto to_json_and_targets = (
      clippson::option(&vm, {"taxicab"}, &taxicab),
      clippson::option(&vm, {"sequence"}, &seq)
    ).doc("Notified to both json and targets:");

    auto with_choices = (
      clippson::option(&vm, {"year", "y"}, {1984, 2112}, 1984),
      clippson::option(&vm, {"whoami"}, {"24601", "Jean", "Javert"}, "24601")
    ).doc("Options with choices:");

    unsigned nsam = 0;
    auto positional = (
      clippson::value<unsigned>(&vm, "nsam", &nsam).doc("Number of samples"),
      clippson::value<std::string>(&vm, "howmany") % "tears"
    ).doc("Positional (required):");

    auto cli = clipp::joinable(
      // positional,
      to_targets,
      to_json,
      to_json_and_targets,
      with_choices
    );
    std::string default_values = vm.dump(2);
    clippson::parse(cli, argc, argv);
    if (help) {
        auto fmt = clippson::doc_format();
        std::cout << clipp::documentation(cli, fmt) << "\n";
    }
    if (vm["version"]) {
        std::cout << "clipp 1.2.2\n";
    }
    std::cout << "Default values: " << default_values << "\n";
    std::cout << "Current values: " << vm.dump(2) << "\n";
    return 0;
}
