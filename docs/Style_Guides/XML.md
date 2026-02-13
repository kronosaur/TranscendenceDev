# XML Game-Data Style Guidelines

## Compatibility
## Formatting

### Comments

While tlisp-style single-line comments are valid as far as the parser is concerned,
this codebase exclusively uses `<!-- xml -->` style comments for commenting XML,
except inside of or just before a tlisp code block.

XML style comments are also mandatory if you need to comment special characters or
XML syntax, as the parser still reads those after the tlisp semicolon comments.

## Design Types
