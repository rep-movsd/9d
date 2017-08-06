#include "scanner.h"

#include <experimental/filesystem>
#include <string.h>

namespace fs = std::experimental::filesystem;

#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])

// Read a text file into a vector of chars
void read_file(const fs::path &path, std::vector<char> &dest) {
    auto path_str8 = path.u8string();

    if (!fs::exists(path)) {
        fprintf(stderr, "File: %s does not exist", path_str8.c_str());
        abort();
    }

    FILE *f = fopen(path_str8.c_str(), "rb");

    if (f == nullptr) {
        fprintf(stderr, "Unexpected error - could not open file %s", path.c_str());
        abort();
    }

    const size_t size = fs::file_size(path);

    dest.clear();
    dest.resize(size);

    size_t read_count = fread(dest.data(), 1, size, f);
    if (read_count != size) {
        fprintf(stderr, "Failed to read full file: %s", path_str8.c_str());
        abort();
    }

    fclose(f);
}

static const char *KEYWORD_LIST[] = {"CHIP", "IN", "OUT", "PARTS", "CLOCKED", "PARTS", "BUILTIN"};

struct Parser {
    scanner::Scanner sc;
    Parser(std::vector<char> s)
        : sc(std::move(s), scanner::DEFAULT_MODE | scanner::SCAN_COMMENTS) {}

    static bool is_keyword(const std::string &token_text) {
        for (size_t i = 0; i < ARRAY_SIZE(KEYWORD_LIST); ++i) {
            if (strcmp(token_text.c_str(), KEYWORD_LIST[i]) == 0) {
                return true;
            }
            return false;
        }
    }

    void parse() {
        std::string token_str;
        while (sc.next() != scanner::EOFS) {
            token_str.clear();
            switch (sc.current_tok) {
            case scanner::IDENT:
                sc.token_text(token_str);
                printf("%s: %s\n", is_keyword(token_str) ? "Keyword" : "Identifier", token_str.c_str());
                break;

            case scanner::INT:
                printf("INT: %li\n", sc.current_int);
                break;

            case scanner::COMMENT:
                sc.token_text(token_str);
                printf("COMMENT: %s\n", token_str.c_str());
                break;

            default:
                printf("default: %s\n", scanner::desc(sc.current_tok));
            }
        }
    }
};

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hdl file>\n", argv[0]);
        return 1;
    }

    std::vector<char> s;
    read_file(argv[1], s);

    Parser p(std::move(s));
    p.parse();
}
