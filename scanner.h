#pragma once

#include <assert.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

namespace scanner {

/// Each of these represent a class of tokens
const int INT = -1;
const int FLOAT = -2;
const int STRING = -3;
const int IDENT = -4;
const int SPACE = -5;
const int COMMENT = -6;
const int EOFS = -7;
const int ESCAPE = -8;
const int INVALID = -9;

enum Modes {
    SCAN_INTS = 1 << -INT,         // Scan integers?
    SCAN_FLOATS = 1 << -FLOAT,     // Scan floats?
    SCAN_STRINGS = 1 << -STRING,   // Scan string literals (e.g "Hello\n")
    SCAN_IDENTS = 1 << -IDENT,     // Scan identifiers (e.g hello_911)
    SCAN_SPACES = 1 << -SPACE,     // Scan white spaces
    SCAN_COMMENTS = 1 << -COMMENT, // Scan comments
    SCAN_ESCAPES = 1 << -ESCAPE,   // This will inteprete escape codes all throughout the string
};

/// This default mode is like scanning most simple languages
const int DEFAULT_MODE = SCAN_INTS | SCAN_FLOATS | SCAN_STRINGS | SCAN_IDENTS;

/// This mode treats the whole text as if being inside a string literal. It
/// scans characters one by one and also interpretes escape code
const int WHOLESTRING_MODE = SCAN_SPACES | SCAN_ESCAPES;

/// Contains the current state of the scanner. As you see, most of the fields
/// are public.
struct Scanner {
    std::vector<char> _text; // The text (this is mutable due to using c_str)
    int mode;                // The mode
    int line;                // Contains the line number if the *next* token. Starts with 1.
    int col;                 // Contains the column of the current line
    int offset;              // Start position for next token (not needed by user)
    int token_start;         // Contains the index of the starting position of the next token
    int current_tok;         // Contains the last token-class that next() returned
    long current_int;        // If next() returns INT, then contains that integer
    double current_float;    // If next() returns FLOAT, then contains that float

    Scanner(std::vector<char> text, int mode = DEFAULT_MODE);

    /// Fills the buffer with the current token text
    void token_text(std::string &out);

    /// Returns a number, which, if negative, denotes the next token's clas If
    /// positive, it is equal to the next character. This happens when none of
    /// the classes match.
    int next();

    /// Pushes into `b` all the characters from `raw` while decoding the
    /// escape codes in `raw` (e.g - The sequence of characters '\' and 'n' in
    /// `raw` will be pushed as a single newline character into `b`.
    void string_token(const std::string &b, std::string &target);
};

/// Returns a description of the given token id
const char *desc(int token);

} // namespace scanner
