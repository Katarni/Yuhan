import pytest
from subprocess import run


def get_program_output():
    return run("build/Yuhan-sintex-tests", encoding="utf-8", capture_output=True, input="sintex-test.txt").stdout


def write_file(input_text):
    file = open("sintex-test.txt", "w")
    file.write(input_text)
    file.close()


def read_file(input_file):
    file = open(input_file, "r")
    text = file.read()
    file.close()
    return text


def test_correct_program_1():
    file = read_file("inputs/correct-program-1.txt")
    write_file(file)

    assert get_program_output() == "OK"

