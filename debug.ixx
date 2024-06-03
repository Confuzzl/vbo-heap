export module debug;

export import <format>;
export import <iostream>;

export void println(const std::string &str = "") { std::cout << str << "\n"; }
export template <typename... Args>
void println(std::format_string<Args...> str, Args &&...args) {
  println(std::format(str, std::forward<Args>(args)...));
}
export template <typename T> void println(const T &s) { println("{}", s); }