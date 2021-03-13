#ifndef UTIL_HPP_H3PRO0J7
#define UTIL_HPP_H3PRO0J7

// ANSI color escape sequences for colorful terminal output
#define COLOR_RED       "\033[31m"
#define COLOR_GREEN     "\033[32m"
#define COLOR_YELLOW    "\033[33m"
#define COLOR_BLUE      "\033[34m"
#define COLOR_MAGENTA   "\033[35m"
#define COLOR_CYAN      "\033[36m"
#define COLOR_WHITE     "\033[37m"
#define COLOR_GREY      "\033[90m"

#define COLOR_RESET     "\033[0m"

#define COLOR_BOLD      "\033[1m"

// stringify macro
#define __STRINGIFY(x) #x
#define STRINGIFY(x) __STRINGIFY(x)

// very basic logging
#define LOG(tag, ...)   debugf(COLOR_GREY "[" STRINGIFY(tag) "] " COLOR_RESET __VA_ARGS__ );
#define ERROR(tag, ...) debugf(COLOR_GREY "[" STRINGIFY(tag) "] " COLOR_RED COLOR_BOLD "ERROR: " COLOR_RESET __VA_ARGS__ );

#endif /* end of include guard: UTIL_HPP_H3PRO0J7 */
