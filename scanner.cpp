#include "scanner.h"

namespace scanner {

Scanner::Scanner(std::vector<char> text, int mode)
    : _text(std::move(text))
    , mode(mode)
    , line(1)
    , col(1)
    , offset(0)
    , token_start(-1)
    , current_tok(INVALID) {}

#define SET_TOK_AND_RET(tok)                                                                                 \
    do {                                                                                                     \
        current_tok = tok;                                                                                   \
        return tok;                                                                                          \
    } while (0)

static inline char escape_code(char c) {
    switch (c) {
    case 'n':
        return '\n';
    case 't':
        return '\t';
    case 'r':
        return '\r';
    case '"':
        return '"';
    case '\\':
        return '\\';
    default:
        return c;
    }
}

int Scanner::next() {
    const char *p = _text.data();
    const char *e = _text.data() + _text.size();
    p += offset;
    if (p == e) {
        SET_TOK_AND_RET(EOFS);
    }
    if (!(mode & SCAN_SPACES)) {
        while (p != e && (*p == ' ' || *p == '\t' || *p == '\n')) {
            if (*p == '\n') {
                ++line;
                col = 0;
            }
            ++p;
            ++offset;
            ++col;
        }
        if (p == e) {
            SET_TOK_AND_RET(EOFS);
        }
    }

    if (((mode & SCAN_COMMENTS) || (mode & SKIP_COMMENTS)) && *p == '/' && (p + 1) != e && p[1] == '/') {
        token_start = p - _text.data();
        while (p != e && *p != '\n') {
            ++p;
            ++offset;
            ++col;
        }
        if (p != e) {
            ++p;
            ++offset;
            col = 1;
            ++line;
        }

        if ((mode & SCAN_COMMENTS)) {
            SET_TOK_AND_RET(COMMENT);
        } else {
            return next();
        }
    }

    if (*p == '"' && (mode & SCAN_STRINGS)) {
        token_start = p - _text.data();
        do {
            if (*p == '\\') {
                ++p;
                ++offset;
                ++col;
                if (p == e) {
                    break;
                }
            } else if (*p == '\n') {
                ++line;
                col = 0;
            }
            ++p;
            ++offset;
            ++col;
        } while (p != e && *p != '"');
        if (p == e) {
            return INVALID + STRING;
        }
        ++col;
        ++offset;
        SET_TOK_AND_RET(STRING);
    }
    if (*p >= '0' && *p <= '9' && (mode & (SCAN_INTS | SCAN_FLOATS))) {
        const char *endp;
        int ret = INT;
        token_start = p - _text.data();
        current_int = strtol(p, (char **)&endp, 0);
        if (endp != e && *endp == '.' && (mode & SCAN_FLOATS)) {
            current_float = strtod(p, (char **)&endp);
            ret = FLOAT;
        }
        offset = endp - _text.data();
        col += endp - p;
        SET_TOK_AND_RET(ret);
    }
    if (((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z') || (*p == '_')) && (mode & SCAN_IDENTS)) {
        token_start = p - _text.data();
        do {
            ++p;
            ++offset;
            ++col;
        } while (p != e && ((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z') || (*p == '_') ||
                            (*p >= '0' && *p <= '9')));
        SET_TOK_AND_RET(IDENT);
    }
    if (*p == '\n') {
        ++line;
        col = 0;
    }
    token_start = p - _text.data();
    ++offset;
    ++col;
    if ((mode & SCAN_SPACES) && (*p == ' ' || *p == '\t' || *p == '\n')) {
        SET_TOK_AND_RET(*p);
    }
    if (*p == '\\' && (mode & SCAN_ESCAPES)) {
        ++p;
        ++offset;
        if (p == e) {
            SET_TOK_AND_RET(EOFS);
        }
        if (*p == '\n' || *p == '\t' || *p == ' ' || *p == '\\') {
            return next();
        }
        ++col;
        char c = escape_code(*p);
        SET_TOK_AND_RET(c);
    }
    SET_TOK_AND_RET(*p);
}

char char_token[] = "xxxxxxxxxxxx";

const char *desc(int token) {
    if (token <= INVALID) {
        sprintf(char_token, "%s", "INVALID");
        return char_token;
    }

    switch (token) {
    case INT:
        return "INT";
    case FLOAT:
        return "FLOAT";
    case STRING:
        return "STRING";
    case IDENT:
        return "IDENT";
    case SPACE:
        return "SPACE";
    case COMMENT:
        return "COMMENT";
    case '\n':
        return "\\n";
    case ' ':
        return " ";
    default:
        assert(0 <= token && token <= 127);
        snprintf(char_token, 2, "%c", (char)token);
        return char_token;
    }
}

void Scanner::token_text(std::string &b) {
    for (int i = token_start; i < offset; ++i) {
        b += _text[i];
    }
}

/*
void string_token(Buffer &b, Buffer &raw) {
    for (const char *i = c_str(raw) + 1; *i; ++i) {
        if (i[0] == '\\') {
            char c = i[1];
            b += escape_code(c);
            ++i;
        } else if (i[0] == '"') {
            break;
        } else {
            b += i[0];
        }
    }
}
*/

} // namespace scanner
