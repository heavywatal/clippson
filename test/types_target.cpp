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
      wtl::option({"h", "help"}, &help, "Print help"),
      wtl::option({"b", "bool"}, &BOOL),
      wtl::option({"i", "int"}, &INT),
      wtl::option({"l", "long"}, &LONG),
      wtl::option({"u", "unsigned"}, &UNSIGNED),
      wtl::option({"s", "size_t"}, &SIZE_T),
      wtl::option({"d", "double"}, &DOUBLE),
      wtl::option({"c", "string"}, &STRING),
      wtl::option({"v", "vector"}, &VECTOR)
    ).doc("Notified to targets:");
    wtl::parse(cli, argc, argv);
    if (help) {
        auto fmt = wtl::doc_format();
        std::cout << clipp::documentation(cli, fmt) << "\n";
    }
    return 0;
}
