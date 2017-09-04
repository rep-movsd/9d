#pragma once

// Aliases filesystem namespace to namespace `fs`

// clang-format off
#if defined(_MSC_VER)
    #include <filesystem>
    namespace fs = std::experimental::filesystem::v1;
#elif __has_include(<filesystem>)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #warning "No filesystem header"
#endif

// Aliases std optional template to template `optional`

#if defined(_MSC_VER)
    #include <optional>
    template <typename T> using optional = std::optional<T>;
    constexpr auto nullopt = std::nullopt;
#elif __has_include(<optional>)
    #include <optional>
    template <typename T> using optional = std::optional<T>;
    constexpr auto nullopt = std::nullopt;
#elif __has_include(<experimental/optional>)
    #include <experimental/optional>
    template <typename T> using optional = std::experimental::optional<T>;
    constexpr auto nullopt = std::experimental::nullopt;
#else
    #error "No optional header"
#endif
// clang-format on

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define ARRAY_BEGIN(arr) arr
#define ARRAY_END(arr) ((arr) + ARRAY_SIZE(arr))

/// Given `o` is an std::optional. Aborts if optional does not contain a
/// value. Otherwise returns that value.
#define MUST_OPTIONAL(o) _must(o, __FILE__, __LINE__)

template <typename T> T _must(optional<T> o, const char *file, int line) {
    if (!bool(o)) {
        fprintf(stderr, "Must optional failed - %s:%i", file, line);
        abort();
    }
    return o.value();
}
