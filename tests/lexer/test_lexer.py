import pytest
from subprocess import run


def get_program_output():
    output = run("build/Yuhan-lexer-tests", encoding="utf-8", capture_output=True, input="lexer-test.txt").stdout
    return output


def write_file(input_text):
    file = open("lexer-test.txt", "w")
    file.write(input_text)
    file.close()

def read_file(input_file):
    file = open(input_file, "r")
    text = file.read()
    file.close()
    return text

def test_simple_program():
    input_text = read_file("inputs/simple-program.txt")

    write_file(input_text)

    expected = read_file("expected/simple-program.txt") 

    assert expected == get_program_output()


def test_binary_operators():
    input_text = read_file("inputs/binary-operators.txt") 

    write_file(input_text)

    expected = read_file("expected/binary-operators.txt") 

    assert expected == get_program_output()


def test_literals():
    input_text = read_file("inputs/literals.txt") 

    write_file(input_text)

    expected = read_file("expected/literals.txt")

    assert expected == get_program_output()
