import pytest
from subprocess import run


def get_program_output():
    output = run("build/Yuhan-lexer-tests", encoding="utf-8", capture_output=True, input="lexer-test.txt").stdout
    return output


def write_file(input_text):
    file = open("lexer-test.txt", "w")
    file.write(input_text)
    file.close()


def test_simple_program():
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


def test_binary_operators():
    input_text = """a += b;
c <= "fsdf" 
f != f
j += 8
d* = 4
d &0 4
g /= f
d <<= 'd'
"""

    write_file(input_text)

    expected = """identifier a 0:0
lvalue binary operator += 0:2
identifier b 0:5
semicolon ; 0:6
identifier c 1:0
rvalue binary operator <= 1:2
string literal fsdf 1:5
identifier f 2:0
rvalue binary operator != 2:2
identifier f 2:5
identifier j 3:0
lvalue binary operator += 3:2
numeric literal 8 3:5
identifier d 4:0
rvalue binary operator * 4:1
lvalue binary operator = 4:3
numeric literal 4 4:5
identifier d 5:0
rvalue binary operator & 5:2
numeric literal 0 5:3
numeric literal 4 5:5
identifier g 6:0
lvalue binary operator /= 6:2
identifier f 6:5
identifier d 7:0
lvalue binary operator <<= 7:2
char literal d 7:6
"""

    assert expected == get_program_output()


def test_literals():
    input_text = """6.6
7.0
4e3
4er
4e
"eee"
5.
'g'
'h"ddd' 
"""

    write_file(input_text)

    expected = """float literal 6.6 0:0
float literal 7.0 1:0
exponential literal 4e3 2:0
another 4er 3:0
another 4e 4:0
string literal eee 5:0
another 5. 6:0
char literal g 7:0
another h\"ddd 8:0
"""

    assert expected == get_program_output()
