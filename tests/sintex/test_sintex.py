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


def test_structs():
    file = read_file("inputs/structs.txt")
    write_file(file)

    assert get_program_output() == "OK"


def test_literals():
    file = read_file("inputs/literals.txt")
    write_file(file)

    assert get_program_output() == "OK"


def test_cycles():
    file = read_file("inputs/cycles.txt")
    write_file(file)

    assert get_program_output() == "OK"
    

def test_if_else():
    file = read_file("inputs/if-else.txt")
    write_file(file)

    assert get_program_output() == "OK"


def test_namespaces():
    file = read_file("inputs/namespaces.txt")
    write_file(file)

    assert get_program_output() == "OK"


def test_functions():
    file = read_file("inputs/functions.txt")
    write_file(file)

    assert get_program_output() == "OK"

