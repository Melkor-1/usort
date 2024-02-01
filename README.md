# usort - Text File Sorting Utility

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://https://github.com/Melkor-1/usort/edit/main/LICENSE)

usort is a command-line utility written in C that allows you to sort the lines of a text file in various ways. It provides options for sorting numerically, in reverse order, and even ignoring case when performing lexicographical sorting. Additionally, you can specify an output file to save the sorted content.

## Features
Sorts lines of a text file in lexicographical, numerical, or case-insensitive lexicographical order.
Supports sorting in reverse order.
Allows specifying an output file for saving the sorted content.
Provides a help option (-h or --help) to display usage information.

### Usage

~~~
usort options filename
~~~

#### Options

-n, --numeric: Sorts lines by numerical order.

-r, --reverse: Sorts lines in reverse order.

-f, --ignore-case: Folds upper case to lower case characters for case-insensitive sorting.

-o, --output=FILE: Writes the sorted output to the specified file.

-h, --help: Displays usage information.

### Examples

Sort lines lexicographically:
~~~
usort input.txt
~~~

Sort lines numerically:
~~~
usort -n input.txt
~~~

Sort lines in reverse order:
~~~
usort -r input.txt
~~~

Sort lines case-insensitively:
~~~
usort -f input.txt
~~~

Write sorted output to a file:
~~~
usort -o sorted_output.txt input.txt
~~~

## Building
To build the usort executable, clone the git repository and run:
~~~
make
~~~
