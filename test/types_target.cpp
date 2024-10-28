#include <clippson/clippson.hpp>

#include <limits>
#include <iostream>

int main(int argc, char* argv[]) {
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
      clippson::option({"h", "help"}, &help, "Print help"),
      clippson::option({"b", "bool"}, &BOOL),
      clippson::option({"i", "int"}, &INT),
      clippson::option({"l", "long"}, &LONG),
      clippson::option({"u", "unsigned"}, &UNSIGNED),
      clippson::option({"s", "size_t"}, &SIZE_T),
      clippson::option({"d", "double"}, &DOUBLE),
      clippson::option({"c", "string"}, &STRING),
      clippson::option({"v", "vector"}, &VECTOR)
    ).doc("Notified to targets:");
    clippson::parse(cli, argc, argv);
    if (help) {
        auto fmt = clippson::doc_format();
        std::cout << clipp::documentation(cli, fmt) << "\n";
    }
    return 0;
}
