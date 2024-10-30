import pytest
from subprocess import run


def get_program_output():
    output = run("build/Yuhan-lexer-tests", encoding="utf-8", capture_output=True, input="lexer-test.txt").stdout
    return output


def write_file(input_text):
    file = open("lexer-test.txt", "w")
    file.write(input_text)
    file.close()


def test_lexer_1():
    input_text = """int main() {
    int a = 0;
}"""

    write_file(input_text)

    expected = """reserved word int 0:0
identifier main 0:4
open parenthesis ( 0:8
close parenthesis ) 0:9
open curly brace { 0:11
reserved word int 1:4
identifier a 1:8
lvalue binary operator = 1:10
numeric literal 0 1:12
semicolon ; 1:13
close curly brace } 2:0
"""

    assert expected == get_program_output()
