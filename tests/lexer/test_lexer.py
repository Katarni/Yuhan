import pytest
from subprocess import run


def get_program_output():
    return run("build/Yuhan-lexer-tests", encoding="utf-8", capture_output=True, input="lexer-test.txt").stdout


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


def test_some_interesting_symbols():
    input_text = read_file("inputs/some-interesting-symbols.txt") 

    write_file(input_text)

    expected = read_file("expected/some-interesting-symbols.txt")

    assert expected == get_program_output()


def test_reserved_words():
    input_text = read_file("inputs/reserved-words.txt") 

    write_file(input_text)

    expected = read_file("expected/reserved-words.txt")

    assert expected == get_program_output()
    

def test_identificators():
    input_text = read_file("inputs/identificators.txt")

    write_file(input_text)

    expected = read_file("expected/identificators.txt") 

    assert expected == get_program_output()

