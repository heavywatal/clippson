#pragma once
#ifndef WTL_CLIPPSON_HPP_
#define WTL_CLIPPSON_HPP_

#include "clipp.h"
#include "json.hpp"

#include <stdexcept>
#include <type_traits>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>

namespace wtl {

namespace detail {

inline std::string to_string(const nlohmann::json& x) {
    std::ostringstream oss;
    if (x.is_string()) {
        oss << x.get<std::string>();
    } else {
        oss << x;
    }
    return oss.str();
}

struct nonempty {
    bool operator()(const std::string& s) const noexcept {return !s.empty();}
};

template <bool Condition>
using enable_if_t = typename std::enable_if<Condition, std::nullptr_t>::type;

template <class T, enable_if_t<std::is_integral<T>{}> = nullptr>
inline clipp::match::integers filter_type() {
    return clipp::match::integers{};
}

template <class T, enable_if_t<std::is_floating_point<T>{}> = nullptr>
inline clipp::match::numbers filter_type() {
    return clipp::match::numbers{};
}

template <class T, enable_if_t<!std::is_integral<T>{} && !std::is_floating_point<T>{}> = nullptr>
inline nonempty filter_type() {
    return nonempty{};
}

template <class T> inline
std::ostream& operator<<(std::ostream& ost, const std::vector<T>& v) {
    if (v.empty()) return ost << "[]";
    auto it = v.begin();
    ost << "[" << *it;
    for (++it; it != v.end(); ++it) {
        ost << "," << *it;
    }
    return ost << "]";
}

template <class T> inline
std::string doc_default(const T& x, const std::string& doc) {
    std::ostringstream oss;
    oss << doc << " (=" << x << ")";
    return oss.str();
}

template <class T>
struct is_vector : std::false_type {};

template <class T>
struct is_vector<std::vector<T>> : std::true_type {};

template <class T, enable_if_t<!is_vector<T>{}> = nullptr> inline
std::function<void(const char*)> set(nlohmann::json& target) {
    return [&target](const char* s){
        target = clipp::detail::make<T>::from(s);
    };
}

template <class T, enable_if_t<is_vector<T>{}> = nullptr> inline
std::function<void(const char*)> set(nlohmann::json& target) {
    return [&target](const char* s){
        target.push_back(clipp::detail::make<typename T::value_type>::from(s));
    };
}

template <class T> inline clipp::parameter
value(const std::string label="") {
    return clipp::value(detail::filter_type<T>(), label)
      .repeatable(detail::is_vector<T>{});
}

template <class T> inline clipp::parameter
value(nlohmann::json& target, const std::string label="") {
    return value<T>(label).call(detail::set<T>(target));
}

template <> inline clipp::parameter
value<const char*>(nlohmann::json& target, const std::string label) {
    return value<const char*>(label).call(detail::set<std::string>(target));
}

inline std::string lstrip(const std::string& s) {
    return s.substr(s.find_first_not_of('-'));
}

inline size_t length(const std::string& s) {
    return s.size() - s.find_first_not_of('-');
}

inline std::string longest(const std::vector<std::string>& args) {
    auto it = std::max_element(args.begin(), args.end(),
                  [](const std::string& lhs, const std::string& rhs) {
                      return length(lhs) < length(rhs);
                  });
    return lstrip(*it);
}

} // namespace detail

template <class T, detail::enable_if_t<!std::is_same<T, bool>{}> = nullptr>
inline clipp::group
option(std::vector<std::string>&& flags, T* target, const std::string& doc="", const std::string& label="") {
    const auto key = detail::longest(flags);
    return clipp::one_of(
      (clipp::option("--" + key + "=") & detail::value<T>(label).set(*target)),
      (clipp::option(std::move(flags)) & detail::value<T>(label).set(*target))
        % detail::doc_default(*target, doc)
   );
}

inline clipp::parameter
option(std::vector<std::string>&& flags, bool* target, const std::string& doc=" ") {
    return clipp::option(std::move(flags)).set(*target).doc(doc);
}

template <class T, detail::enable_if_t<!std::is_same<T, bool>{}> = nullptr>
inline clipp::group
option(nlohmann::json& obj, std::vector<std::string>&& flags, const T init, const std::string& doc="", const std::string& label="") {
    const auto key = detail::longest(flags);
    auto& target_js = obj[key] = init;
    return clipp::one_of(
      (clipp::option("--" + key + "=") & detail::value<T>(target_js, label)),
      (clipp::option(std::move(flags)) & detail::value<T>(target_js, label))
        % detail::doc_default(init, doc)
    );
}

template <class T, detail::enable_if_t<!std::is_same<T, bool>{} && !std::is_same<T, const char>{}> = nullptr>
inline clipp::group
option(nlohmann::json& obj, std::vector<std::string>&& flags, T* target, const std::string& doc="", const std::string& label="") {
    const auto key = detail::longest(flags);
    auto& target_js = obj[key] = *target;
    return clipp::one_of(
      (clipp::option("--" + key + "=") & detail::value<T>(target_js, label).set(*target)),
      (clipp::option(std::move(flags)) & detail::value<T>(target_js, label).set(*target))
        % detail::doc_default(*target, doc)
    );
}

inline clipp::parameter
option(nlohmann::json& obj, std::vector<std::string>&& flags, const bool init=false, const std::string& doc=" ") {
    const auto key = detail::longest(flags);
    auto& target_js = obj[key] = init;
    return clipp::option(std::move(flags)).call(detail::set<bool>(target_js)).doc(doc);
}

inline clipp::parameter
option(nlohmann::json& obj, std::vector<std::string>&& flags, bool* target, const std::string& doc=" ") {
    const auto key = detail::longest(flags);
    auto& target_js = obj[key] = *target;
    return clipp::option(std::move(flags)).call(detail::set<bool>(target_js)).set(*target).doc(doc);
}

inline clipp::doc_formatting doc_format() {
    return clipp::doc_formatting{}
      .first_column(0)
      .doc_column(24)
      .last_column(80)
      .indent_size(2)
      .flag_separator(",")
      .empty_label("arg")
    ;
}

template <class Stream> inline
Stream& debug_print(Stream& ost, const clipp::parsing_result& parsed, const clipp::group& cli, const clipp::doc_formatting& fmt = doc_format()) {
    ost << "\nParsing result:\n";
    clipp::debug::print(ost, parsed);
    ost << "\nUsage:\n";
    ost << clipp::documentation(cli, fmt) << "\n";
    return ost;
}

template <class... Args> inline
clipp::parsing_result parse(const clipp::group& cli, Args&&... args) {
    auto parsed = clipp::parse(std::forward<Args>(args)..., cli);
    if (!parsed) {
        std::ostringstream oss;
        debug_print(oss, parsed, cli);
        throw std::runtime_error(oss.str());
    }
    return parsed;
}

inline clipp::arg_list arg_list(const nlohmann::json& obj) {
    clipp::arg_list args;
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        if (it->is_boolean() && !it.value()) continue;
        if (it->is_array() && it.value().empty()) continue;
        args.push_back("--" + it.key());
        if (it->is_array()) {
            for (const auto& x: it.value()) {
                args.push_back(detail::to_string(x));
            }
        } else {
            args.push_back(detail::to_string(it.value()));
        }
    }
    return args;
}

}  // namespace wtl

#endif /* WTL_CLIPPSON_HPP_ */
