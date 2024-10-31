import pytest
from subprocess import run


def get_program_output(test_word):
    return run("build/Yuhan-trie-tests", encoding="utf-8", capture_output=True, input=test_word).stdout


def test_one():
    assert get_program_output("int") == "true"


def test_two():
    assert get_program_output("wwwww") == "false"


def test_three():
    assert get_program_output("return") == "true"


def test_four():
    assert get_program_output("ggg\t\n\b") == "false"


def test_five():
    assert get_program_output("ggg\\\\  \" ") == "false"

