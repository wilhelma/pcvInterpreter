/**
 *
 *    @file  Logger.h
 *   @brief  
 *
 *    @date  08/25/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  LOGGER_H_
#define  LOGGER_H_

#include <chrono>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <utility>
#include <unordered_map>
#include <type_traits>

/// Log severity levels.
/// @todo Introduce an ordering.
enum class LogSeverity: unsigned char {
    trace,
    debug,
    info,
    warning,
    error,
    fatal,
    disable
};

/// Smaller-than operator for LogSeverity levels.
/// @param lhs The left-hand side.
/// @param rhs The right-hand side.
constexpr inline const bool operator<(LogSeverity lhs, LogSeverity rhs) noexcept {
    using underlying_type = std::underlying_type_t<LogSeverity>;
    return static_cast<underlying_type>(lhs) < static_cast<underlying_type>(rhs);
}

/// Convert a `bool` to a `std::true_type` or `std::false_type`.
/// @tparam is_true The bool to convert.
template <bool is_true>
inline constexpr std::integral_constant<bool, is_true> bool_to_bool_type() noexcept
{ return std::integral_constant<bool, is_true>{}; }
//
//template <bool IsValid>
//struct Stream {
//};
//
///// Specialization: actual stream.
//template <>
//struct Stream<true> {
//    template <typename T>
//    constexpr const Stream<true>& operator<<(T&& input_string) const {
//        std::cerr << input_string;
//        return (*this);
//    }
//};
//
///// Specialization: dumb stream.
//template <>
//struct Stream<false> {
//    template <typename T>
//    constexpr const Stream<false>& operator<<(T&& input_string) const noexcept
//    { return (*this); }
//};

/// @brief Dumb stream to suppress output.
/// @attention It _must_ inherit from `std::basic_ostream` to properly handle `std::endl`.
struct NullStream : std::basic_ostream<char> {};

/// Dumb input operator for the dumb NullStream.
/// @param s            The stream to act onto.
/// @param input_string The input (string or whatever) to swallow.
template <typename T>
constexpr inline NullStream& operator<<(NullStream& s, T&& input_string) noexcept
{ return s; }

/// Easy, self-contained logger supporting severity levels.
/// @tparam MinLogSeverity Severity of the logger: controls the verbosity.
template <LogSeverity MinLogSeverity>
class Logger {
public:
    /// Constructor.
    explicit Logger() :
        LoggerStartTime_(std::chrono::system_clock::now())
    {
        std::time_t now_c = std::chrono::system_clock::to_time_t(LoggerStartTime_);
        log<LogSeverity::info>() << "Starting logger on "
            << std::put_time(std::localtime(&now_c), "%F at %T") << std::endl;
    }


    /// _Default_ destructor.
    ~Logger()                  = default;
 
    /// _Deleted_ copy constructor.
    Logger(const Logger&)            = delete;
    /// _Deleted_ move constructor.
    Logger(Logger&&)                 = delete;
    /// _Deleted_ copy assignment operator.
    Logger& operator=(const Logger&) = delete;
    /// _Deleted_ move assignment operator.
    Logger& operator=(Logger&&)      = delete;

    /// Returns the time the logger was started at.
    constexpr decltype(auto) getStartTime() const noexcept {
        return LoggerStartTime_;
    }

    /// Cosmetics helper function for `trace` logs.
    constexpr decltype(auto) trace()
    { return log<LogSeverity::trace>(); }

    /// Cosmetics helper function for `debug` logs.
    constexpr decltype(auto) debug()
    { return log<LogSeverity::debug>(); }

    /// Cosmetics helper function for `info` logs.
    constexpr decltype(auto) info()
    { return log<LogSeverity::info>(); }

    /// Cosmetics helper function for `warning` logs.
    constexpr decltype(auto) warning()
    { return log<LogSeverity::warning>(); }

    /// Cosmetics helper function for `error` logs.
    constexpr decltype(auto) error()
    { return log<LogSeverity::error>(); }

    /// Cosmetics helper function for `fatal` logs.
    constexpr decltype(auto) fatal()
    { return log<LogSeverity::fatal>(); }

private:
    /// @todo description
    /// @tparam S The requested log severity.
    template <LogSeverity S, typename = std::enable_if_t<
        std::is_same<std::integral_constant<bool, (S < MinLogSeverity)>, std::true_type>::value>> 
    constexpr NullStream& log() noexcept
    { return NullStream_; }

    /// @todo description
    /// @tparam S The requested log severity.
    template <LogSeverity S, typename = std::enable_if_t<
        std::is_same<std::integral_constant<bool, !(S < MinLogSeverity)>, std::true_type>::value>> 
    std::basic_ostream<char>& log() noexcept
    { return std::cerr << LogSeverityString_.at(S); }

    /// Map from the severity level to its output string.
    const std::unordered_map<LogSeverity, const std::string> LogSeverityString_ {
        {LogSeverity::trace,   "[Trace]   "},
        {LogSeverity::debug,   "[Debug]   "},
        {LogSeverity::info,    "[Info]    "},
        {LogSeverity::warning, "[Warning] "},
        {LogSeverity::error,   "[Error]   "},
        {LogSeverity::fatal,   "[Fatal]   "},
    };

    /// @brief Instantiation of the dumb stream.
    /// @todo  Find a better design that allows to get rid of this.
    NullStream NullStream_;

    /// The time the logger was started at.
    std::chrono::system_clock::time_point LoggerStartTime_;
};


#endif
