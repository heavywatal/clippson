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
#include <charconv>
#include <sstream>

namespace wtl {

namespace detail {

template <class T>
struct is_vector : std::false_type {};

template <class T>
struct is_vector<std::vector<T>> : std::true_type {};

template <class T>
inline constexpr bool is_vector_v = is_vector<T>::value;

inline std::string to_string(const nlohmann::json& x) {
    if (x.is_string()) {
        return x.get<std::string>();
    }
    return x.dump(-1);
}

inline std::string_view lstrip(std::string_view s) {
    return s.substr(s.find_first_not_of('-'));
}

inline auto length(std::string_view s) {
    return s.size() - s.find_first_not_of('-');
}

inline std::string longest(const std::vector<std::string>& args) {
    auto it = std::max_element(args.begin(), args.end(),
                  [](std::string_view lhs, std::string_view rhs) {
                      return length(lhs) < length(rhs);
                  });
    return std::string{lstrip(*it)};
}

template <class T> inline
std::string doc_default(const T& x, std::string_view doc) {
    std::ostringstream oss;
    oss << doc << " (=" << nlohmann::json(x) << ")";
    return oss.str();
}

template <class T> inline
std::conditional_t<std::is_convertible_v<T, std::string_view>, std::string_view, T>
sto(std::string_view sv) {
  T x{};
  std::from_chars(std::begin(sv), std::end(sv), x);
  return x;
}

template <> inline std::string_view
sto<std::string>(std::string_view sv) {return sv;}

template <> inline std::string_view
sto<const char*>(std::string_view sv) {return sv;}

template <> inline bool
sto<bool>(std::string_view) {return true;}

template <> inline double
sto<double>(std::string_view sv) {return std::stod(std::string{sv});}

template <class T, class X> inline void
split(std::string_view src, X* dst, std::string_view delimiter=",") {
    if (src.empty()) return;
    for (size_t start = 0, pos = 0; pos != src.npos; start = pos + 1u) {
        pos = src.find_first_of(delimiter, start);
        dst->push_back(sto<T>(src.substr(start, pos - start)));
    }
}

template <class T>
struct Filter {
    bool operator()(std::string_view s) const noexcept {
        try {
            if constexpr (is_vector_v<T>) {
                T v;
                split<typename T::value_type>(s, &v, ",");
            } else {
                sto<T>(s);
            }
        } catch (...) {
            return false;
        }
        return true;
    }
};

template <class T, class X> inline
std::function<void(const char*)> set(X* target) {
    return [target](const char* s){
        if constexpr (is_vector_v<T>) {
            target->clear();
            split<typename T::value_type>(s, target);
        } else {
            *target = sto<T>(s);
        }
    };
}

template <class T, class X> inline
std::function<void(const char*)> append_positional(X* target) {
    return [target](const char* s){
        if constexpr (is_vector_v<T>) {
            auto v = nlohmann::json::array();
            split<typename T::value_type>(s, &v);
            target->operator[]("--").push_back(std::move(v));
        } else {
            target->operator[]("--").push_back(sto<T>(s));
        }
    };
}

template <class T, class X> inline
std::function<void(void)> clear(X* target) {
    if constexpr (std::is_arithmetic_v<T>) {
        return [](){};
    } else {
        return [target](){target->clear();};
    }
}

} // namespace detail

template <class T> inline clipp::parameter
value(std::string_view label) {
    return clipp::value(detail::Filter<T>{}, std::string{label})
             .required(std::is_arithmetic_v<T>);
}

template <class T, class Target, class... Rest> inline clipp::parameter
value(std::string_view label, Target* target, Rest*... rest) {
    return value<T>(label, rest...).call(detail::set<T>(target));
}

template <class T> inline clipp::parameter
value(nlohmann::json* obj, std::string_view label) {
    return clipp::value(detail::Filter<T>{}, std::string{label})
             .call(detail::append_positional<T>(obj));
}

template <class T, class Target, class... Rest> inline clipp::parameter
value(nlohmann::json* obj, std::string_view label, Target* target, Rest*... rest) {
    return value<T>(obj, label, rest...).call(detail::set<T>(target));
}

template <class T> inline clipp::parameter
command(const T& x) {
    if constexpr (std::is_convertible_v<T, std::string_view>) {
        return clipp::command(x);
    } else {
        return clipp::command(std::to_string(x));
    }
}

template <class T, class Target, class... Rest> inline clipp::parameter
command(const T& x, Target* target, Rest*... rest) {
    return command(x, rest...).call(detail::set<T>(target));
}

template <class T, class... Targets> inline clipp::group
commands(T&& choices, Targets*... targets) {
    clipp::group g;
    for (auto& x: choices) {
        g.push_back(command(x, targets...));
    }
    g.exclusive(true);
    return g;
}

template <class T = std::nullptr_t, class F> inline clipp::parameter
option(F&& flags) {
    return clipp::with_prefixes_short_long("-", "--",
        clipp::option(std::forward<F>(flags))
    );
}

template <class T, class F, class Target, class... Rest> inline clipp::parameter
option(F&& flags, Target* target, Rest*... rest) {
    return option<T>(std::forward<F>(flags), rest...).call(detail::clear<T>(target));
}

template <class T, class F, class... Targets>
inline clipp::group
group(F&& flags, std::string_view label, Targets*... targets) {
    return option<T>(std::forward<F>(flags), targets...)
           & value<T>(label, targets...);
}

template <class T>
inline auto
option(std::vector<std::string>&& flags,
       T* target, std::string_view doc="", std::string_view label="") {
    if constexpr (std::is_same_v<T, bool>) {
        return option<bool>(std::move(flags)).set(*target).doc(std::string{doc});
    } else {
        const auto key = detail::longest(flags);
        return (
          group<T>(key + "=", label, target),
          group<T>(std::move(flags), label, target)
            % detail::doc_default(*target, doc)
        );
    }
}

template <class T>
inline auto
option(nlohmann::json* obj, std::vector<std::string>&& flags,
       const T init, std::string_view doc="", std::string_view label="") {
    const auto key = detail::longest(flags);
    auto& target_js = (*obj)[key] = init;
    if constexpr (std::is_same_v<T, bool>) {
        return option<bool>(std::move(flags)).call(detail::set<bool>(&target_js)).doc(std::string{doc});
    } else {
        return (
          group<T>(key + "=", label, &target_js),
          group<T>(std::move(flags), label, &target_js)
            % detail::doc_default(init, doc)
        );
    }
}

template <class T, std::enable_if_t<!std::is_same_v<T, const char>>* = nullptr>
inline auto
option(nlohmann::json* obj, std::vector<std::string>&& flags,
       T* target, std::string_view doc="", std::string_view label="") {
    const auto key = detail::longest(flags);
    auto& target_js = (*obj)[key] = *target;
    if constexpr (std::is_same_v<T, bool>) {
        return option<bool>(std::move(flags)).call(detail::set<bool>(&target_js)).set(*target).doc(std::string{doc});
    } else {
        return (
          group<T>(key + "=", label, &target_js, target),
          group<T>(std::move(flags), label, &target_js, target)
            % detail::doc_default(*target, doc)
        );
    }
}

template <class T>
inline clipp::group
option(std::vector<std::string>&& flags,
       std::vector<T> choices, T* target, std::string_view doc="") {
    const auto key = detail::longest(flags);
    auto cmds = commands(std::move(choices), target);
    return (
      (option(key + "=") & cmds),
      (option(std::move(flags)) & cmds)
        % detail::doc_default(*target, doc)
    );
}

template <class T>
inline clipp::group
option(nlohmann::json* obj, std::vector<std::string>&& flags,
       std::vector<T> choices, const T init, std::string_view doc="") {
    const auto key = detail::longest(flags);
    auto& target_js = (*obj)[key] = init;
    auto cmds = commands(std::move(choices), &target_js);
    return (
      (option(key + "=") & cmds),
      (option(std::move(flags)) & cmds)
        % detail::doc_default(init, doc)
    );
}

template <class T, std::enable_if_t<!std::is_same_v<T, const char>>* = nullptr>
inline clipp::group
option(nlohmann::json* obj, std::vector<std::string>&& flags,
       std::vector<T> choices, T* target, std::string_view doc="") {
    const auto key = detail::longest(flags);
    auto& target_js = (*obj)[key] = *target;
    auto cmds = commands(std::move(choices), &target_js, target);
    return (
      (option(key + "=") & cmds),
      (option(std::move(flags)) & cmds)
        % detail::doc_default(*target, doc)
    );
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
            if (it.key() == "--") {
                for (const auto& x: it.value()) {
                    args.push_back(detail::to_string(x));
                }
                continue;
            }
            args.push_back("--" + it.key());
            args.push_back(detail::to_string(it.value()));
        } else {
            args.push_back("--" + it.key());
            args.push_back(detail::to_string(it.value()));
        }
    }
    return args;
}

}  // namespace wtl

#endif /* WTL_CLIPPSON_HPP_ */
