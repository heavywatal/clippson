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

template <bool Condition>
using enable_if_t = typename std::enable_if<Condition, std::nullptr_t>::type;

template <class T>
struct is_vector : std::false_type {};

template <class T>
struct is_vector<std::vector<T>> : std::true_type {};

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

inline std::string to_string(const nlohmann::json& x) {
    std::ostringstream oss;
    if (x.is_string()) {
        oss << x.get<std::string>();
    } else {
        oss << x;
    }
    return oss.str();
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

template <class T> inline
std::string doc_default(const T& x, const std::string& doc) {
    std::ostringstream oss;
    oss << doc << " (=" << x << ")";
    return oss.str();
}

struct any {
    bool operator()(const std::string&) const noexcept {return true;}
};

struct nonempty {
    bool operator()(const std::string& s) const noexcept {return !s.empty();}
};

template <class T, enable_if_t<std::is_integral<T>{}> = nullptr>
inline clipp::match::integers filter_type() {
    return clipp::match::integers{};
}

template <class T, enable_if_t<std::is_floating_point<T>{}> = nullptr>
inline clipp::match::numbers filter_type() {
    return clipp::match::numbers{};
}

template <class T, enable_if_t<is_vector<T>{}> = nullptr>
inline auto filter_type() -> decltype(filter_type<typename T::value_type>()) {
    return filter_type<typename T::value_type>();
}

template <class T, enable_if_t<!std::is_arithmetic<T>{} && !is_vector<T>{}> = nullptr>
inline nonempty filter_type() {
    return nonempty{};
}

template <class T> inline T
sto(const std::string& s) {return s;}

template <> inline const char*
sto(const std::string& s) {return s.c_str();}

template <> inline bool
sto<bool>(const std::string&) {return true;}

template <> inline int
sto<int>(const std::string& s) {return std::stoi(s);}

template <> inline long
sto<long>(const std::string& s) {return std::stol(s);}

template <> inline long long
sto<long long>(const std::string& s) {return std::stoll(s);}

template <> inline unsigned
sto<unsigned>(const std::string& s) {return static_cast<unsigned>(std::stoul(s));}

template <> inline unsigned long
sto<unsigned long>(const std::string& s) {return std::stoul(s);}

template <> inline unsigned long long
sto<unsigned long long>(const std::string& s) {return std::stoull(s);}

template <> inline double
sto<double>(const std::string& s) {return std::stod(s);}

template <class T, class X, enable_if_t<!is_vector<T>{}> = nullptr> inline
std::function<void(const char*)> set(X& target) {
    return [&target](const char* s){
        target = sto<T>(s);
    };
}

template <class T, class X, enable_if_t<is_vector<T>{}> = nullptr> inline
std::function<void(const char*)> set(X& target) {
    return [&target](const char* s){
        target.push_back(sto<typename T::value_type>(s));
    };
}

template <class T, class X, enable_if_t<std::is_arithmetic<T>{}> = nullptr> inline
std::function<void(void)> clear(X&) {
    return [](){};
}

template <class T, class X, enable_if_t<!std::is_arithmetic<T>{}> = nullptr> inline
std::function<void(void)> clear(X& target) {
    return [&target](){target.clear();};
}

} // namespace detail

template <class T> inline clipp::parameter
value(const std::string& label) {
    return clipp::value(detail::filter_type<T>(), label)
      .required(std::is_arithmetic<T>{})
      .repeatable(detail::is_vector<T>{});
}

template <class T, class Target, class... Rest> inline clipp::parameter
value(const std::string& label, Target& target, Rest&... rest) {
    return value<T>(label, rest...).call(detail::set<T>(target));
}

template <class T, class F> inline clipp::parameter
option(F&& flags) {
    return clipp::option(std::forward<F>(flags));
}

template <class T, class F, class Target, class... Rest> inline clipp::parameter
option(F&& flags, Target& target, Rest&... rest) {
    return option<T>(std::forward<F>(flags), rest...).call(detail::clear<T>(target));
}

template <class T, class F, detail::enable_if_t<!std::is_same<T, bool>{}> = nullptr, class... Targets>
inline clipp::group
group(F&& flags, const std::string& label, Targets&... targets) {
    return option<T>(std::forward<F>(flags), targets...)
           & value<T>(label, targets...);
}

template <class T, detail::enable_if_t<!std::is_same<T, bool>{}> = nullptr>
inline clipp::group
option(std::vector<std::string>&& flags, T* target, const std::string& doc="", const std::string& label="") {
    const auto key = detail::longest(flags);
    return clipp::one_of(
      group<T>("--" + key + "=", label, *target),
      group<T>(std::move(flags), label, *target)
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
      group<T>("--" + key + "=", label, target_js),
      group<T>(std::move(flags), label, target_js)
        % detail::doc_default(init, doc)
    );
}

template <class T, detail::enable_if_t<!std::is_same<T, bool>{} && !std::is_same<T, const char>{}> = nullptr>
inline clipp::group
option(nlohmann::json& obj, std::vector<std::string>&& flags, T* target, const std::string& doc="", const std::string& label="") {
    const auto key = detail::longest(flags);
    auto& target_js = obj[key] = *target;
    return clipp::one_of(
      group<T>("--" + key + "=", label, target_js, *target),
      group<T>(std::move(flags), label, target_js, *target)
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
        if (it->is_boolean()) {
            if (!it.value()) continue;
            args.push_back("--" + it.key());
        } else if (it->is_array()) {
            if (it.value().empty()) continue;
            args.push_back("--" + it.key());
            for (const auto& x: it.value()) {
                args.push_back(detail::to_string(x));
            }
        } else {
            args.push_back("--" + it.key());
            args.push_back(detail::to_string(it.value()));
        }
    }
    return args;
}

}  // namespace wtl

#endif /* WTL_CLIPPSON_HPP_ */
