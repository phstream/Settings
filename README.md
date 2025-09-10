´´´
_ _|  \  |_ _|
  |    \ |  |
  |  |\  |  |  INI-File Parser Version 0.2.0
___|_| \_|___| Author: Peter Hillerström 2025, License: MIT
 ```
# Settings
## 🚧 Work in progress  🚧
Settings Handler with INI-reader/writer.

## INI File Format Specification

This paragrafh defines the behavior and expectations of this INI Parser. It aims to clarify parsing rules, escaping, and formatting for human-readable configuration files compatible across platforms.

### General Structure

An INI file consists of sections, keys, values, and comments.

#### Sections

* Format: `[section_name]`
* Case-insensitive.
* Must be on a line by itself.
* Whitespace before/after section name is ignored.

#### Keys and Values

* Format: `key = value` or `key: value`
* Case-insensitive keys.
* Whitespace around `=` or `:` is ignored.
* Values are read as strings by default.
* Quotes around values are optional but recommended for safety.
* If quotes are present, escape sequences are processed.
* New keys are always added at the end of a section.

#### Comments

* Start with `#` or `;` **at the beginning** of a line.
* Inline comments (after values) are allowed.
* Comments are ignored during read/parse.
* Default comments are only added when a key is newly written.
* When updating an existing key, old inline comments are removed.

### Escape Sequences (within quoted strings)

The following escape sequences are supported:

| Escape   | Meaning                    |
| -------- | -------------------------- |
| `\\`     | Backslash (`\`)            |
| `\'`     | Apostrophe (`'`) TBC       |
| `\"`     | Double quote (`"`)         |
| `\t`     | Tab                        |
| `\r`     | Carriage return            |
| `\n`     | Newline                    |
| `\xNN`   | hex coded character        |

### Parsing Rules

* Lines are trimmed of leading whitespace before parsing.
* A value is terminated by EOL or comment if unquoted.
* If quoted, the parser reads until the matching quote and processes escapes.
* Invalid or malformed lines are ignored or logged.

### Writing Rules

* Values are written as-is unless quoting is required due to special characters.
* Updated files maintain section and key order.
* Indentation is not preserved.
* New sections go at the end of the file.

### Best Practices

* Use comments sparingly and place above the relevant key for clarity.
* Avoid overly long lines or excessive spacing.

### License

MIT License.

### Author

Peter Hillerström, 2025

### References

* [https://en.wikipedia.org/wiki/INI\_file](https://en.wikipedia.org/wiki/INI_file)
* [https://www.w3schools.io/file/ini-extension-introduction/](https://www.w3schools.io/file/ini-extension-introduction/) (non-authoritative)
