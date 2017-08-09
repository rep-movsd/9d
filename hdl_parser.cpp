#include "config.h"
#include "scanner.h"

#include <exception>
#include <iostream>
#include <stdarg.h>
#include <string.h>

constexpr int MAX_SUBSCRIPT = 1024;

// Read a text file into a vector of chars
void read_file(const char *path, std::vector<char> &dest) {
    if (FILE *fp = fopen(path, "r")) {
        char buf[1024];
        while (size_t len = fread(buf, 1, sizeof(buf), fp))
            dest.insert(dest.end(), buf, buf + len);
        fclose(fp);
    } else {
        fprintf(stderr, "Unexpected error - could not open file %s", path);
    }
}

static const char *KEYWORD_LIST[] = {"CHIP", "IN", "OUT", "PARTS", "CLOCKED", "PARTS", "BUILTIN"};

struct Parser {
    scanner::Scanner sc;
    Parser(std::vector<char> s)
        : sc(std::move(s), scanner::DEFAULT_MODE) {}

    static bool is_keyword(const std::string &token_text) {
        for (size_t i = 0; i < ARRAY_SIZE(KEYWORD_LIST); ++i) {
            if (strcmp(token_text.c_str(), KEYWORD_LIST[i]) == 0) {
                return true;
            }
        }
        return false;
    }

    struct Range {
        int start;
        int end; // inclusive. start == end for 1 channel input or output

        void dump(std::ostream &o) const {
            if (start != end) {
                o << "[" << start << " .. " << end << "]";
            } else {
                o << "[" << start << "]";
            }
        }
    };

    using UnsubscriptedPin = std::string;

    struct MaybeSubscriptedEndpoint {
        UnsubscriptedPin pin;
        optional<Range> subscript;

        void dump(std::ostream &o) const {
            o << pin;
            if (subscript) {
                subscript.value().dump(o);
            }
        }
    };

    struct Connection {
        MaybeSubscriptedEndpoint left;
        MaybeSubscriptedEndpoint right;

        void dump(std::ostream &o) const {
            left.dump(o);
            o << " = ";
            right.dump(o);
        }
    };

    struct Part {
        std::string chip;
        std::vector<Connection> connections;

        void dump(std::ostream &o) const {
            o << chip << "(";
            for (auto &conn : connections) {
                conn.dump(o);
                o << ", ";
            }
            o << ")";
        }
    };

    using PartsList = std::vector<Part>;

    using ChipName = std::string;

    using BuiltinChipsList = std::vector<ChipName>;
    using ClockedPinList = std::vector<UnsubscriptedPin>;

    struct PinDeclaration {
        std::string pin;
        uint32_t width = 1;

        void dump(std::ostream &o) const {
            o << pin;
            if (width > 1) {
                o << "[" << width << "]";
            }
        }
    };

    struct Chip {
        std::string name;
        std::vector<PinDeclaration> in_pins;
        std::vector<PinDeclaration> out_pins;
        std::vector<Part> parts;
        BuiltinChipsList builtin_chips;
        ClockedPinList clocked_pins;

        void dump(std::ostream &o) const {
            o << "CHIP " << name << " {\n";

            auto print_decls = [&](const std::vector<Parser::PinDeclaration> &decls) {
                for (auto &d : decls) {
                    o << d.pin;
                    if (d.width > 1) {
                        o << "[" << d.width << "]";
                    }
                    o << ",";
                }
                o << ";";
            };

            o << "IN ";
            print_decls(in_pins);
            o << "\n";

            o << "OUT ";
            print_decls(out_pins);
            o << "\n";

            o << "BUILTIN ";

            for (auto &chip : builtin_chips) {
                o << chip << ", ";
            }
            o << ";\n";

            for (auto &pin : clocked_pins) {
                o << pin << ", ";
            }
            o << ";\n";

            o << "PARTS: \n";

            for (auto &p : parts) {
                p.dump(o);
                o << ";\n";
            }
            o << "}\n";
        }
    };

    Chip parse() {
        sc.next();
        return chip();
    }

    void error(const char *fmt, ...) {
        char buf[1024] = {};
        va_list va;
        va_start(va, fmt);
        int count = snprintf(buf, sizeof(buf), "hdl parse error: Line: %i, Col: %i\n\t", sc.line, sc.col);
        vsnprintf(buf + count, sizeof(buf) - count, fmt, va);
        va_end(va);

        fprintf(stderr, "%s: ", buf);
        throw std::runtime_error(std::string(buf));
    }

    void expect(int token) {
        if (token != sc.current_tok) {
            const char *desc;
            char b[2] = {};
            if (token < 0) {
                desc = scanner::desc(token);
            } else {
                b[1] = (char)token;
                desc = b;
            }
            error("Expected: %s", desc);
        }
    }

    void skip(int c) {
        if (sc.current_tok != c) {
            error("Expected: '%c'", (char)c);
        }
        sc.next();
    }

    Chip chip() {
        Chip c;

        auto skip_keyword = [this](const char *keyword) {
            if (!check_keyword(keyword)) {
                error((std::string("Expected keyword ") + keyword).c_str());
            }
            sc.next();
        };

        skip_keyword("CHIP");

        c.name = identifier();

        skip('{');

        skip_keyword("IN");

        c.in_pins = in_or_out_pin_decls();

        skip_keyword("OUT");

        c.out_pins = in_or_out_pin_decls();
        c.builtin_chips = maybe_builtin_chip_list();
        c.clocked_pins = maybe_clocked_pin_list();

        skip_keyword("PARTS");
        skip(':');

        c.parts = parts_list();

        skip('}');

        if (sc.current_tok != scanner::EOFS) {
            error("Expected EOF");
        }

        return c;
    }

    std::vector<PinDeclaration> in_or_out_pin_decls() {
        std::vector<PinDeclaration> decls;
        decls.push_back(pin_declaration());
        while (sc.current_tok == ',') {
            sc.next();
            decls.push_back(pin_declaration());
        }
        skip(';');
        return decls;
    }

    PinDeclaration pin_declaration() {
        PinDeclaration p;
        p.pin = unsubscripted_endpoint();
        if (sc.current_tok == '[') {
            sc.next();
            p.width = integer();
            skip(']');
        }
        return p;
    }

    ClockedPinList maybe_clocked_pin_list() {
        if (!check_keyword("CLOCKED")) {
            return ClockedPinList();
        }
        sc.next();
        return clocked_pin_list();
    }

    ClockedPinList clocked_pin_list() {
        ClockedPinList pins;
        pins.push_back(identifier());
        while (sc.current_tok == ',') {
            sc.next();
            pins.push_back(identifier());
        }
        skip(';');
        return pins;
    }

    BuiltinChipsList maybe_builtin_chip_list() {
        if (!check_keyword("BUILTIN")) {
            return BuiltinChipsList();
        }
        sc.next();
        return builtin_chips_list();
    }

    BuiltinChipsList builtin_chips_list() {
        BuiltinChipsList chips;
        chips.push_back(identifier());
        while (sc.current_tok == ',') {
            sc.next();
            chips.push_back(identifier());
        }
        skip(';');
        return chips;
    }

    std::string identifier() {
        expect(scanner::IDENT);
        std::string s;
        sc.token_text(s);

        if (is_keyword(s)) {
            error("Expected identifier, found keyword");
        }
        sc.next();

        return s;
    }

    bool check_keyword(const char *keyword) {
        if (sc.current_tok != scanner::IDENT) {
            return false;
        }
        std::string s;
        sc.token_text(s);

        return strcmp(s.c_str(), keyword) == 0;
    }

    PartsList parts_list() {
        PartsList v;
        v.push_back(part());

        skip(';');

        while (sc.current_tok == scanner::IDENT) {
            v.push_back(part());
            skip(';');
        }
        return v;
    }

    Part part() {
        expect(scanner::IDENT);
        Part p;
        sc.token_text(p.chip);

        sc.next();
        skip('(');
        p.connections = connection_list();
        skip(')');
        return p;
    }

    std::vector<Connection> connection_list() {
        std::vector<Connection> v;
        v.push_back(connection());
        while (sc.current_tok == ',') {
            sc.next();
            v.push_back(connection());
        }
        return v;
    }

    Connection connection() {
        Connection conn;
        conn.left = maybe_subscripted_endpoint();
        skip('=');
        conn.right = maybe_subscripted_endpoint();
        return conn;
    }

    MaybeSubscriptedEndpoint maybe_subscripted_endpoint() {
        auto pin = unsubscripted_endpoint();
        if (sc.current_tok != '[') {
            return MaybeSubscriptedEndpoint{pin, nullopt};
        }
        sc.next();
        Range r = {};
        r.start = integer();
        if (sc.current_tok == '.') {
            sc.next();
            skip('.');
            r.end = integer();
            skip(']');
        } else if (sc.current_tok == ']') {
            sc.next();
            r.end = r.start;
        }
        return MaybeSubscriptedEndpoint{pin, r};
    }

    UnsubscriptedPin unsubscripted_endpoint() { return identifier(); }

    int integer() {
        expect(scanner::INT);
        if (sc.current_int < 0 || sc.current_int >= MAX_SUBSCRIPT) {
            error("Expected positive integer in valid range");
        }
        int n = sc.current_int;
        sc.next();
        return n;
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
    p.parse().dump(std::cout);
}
