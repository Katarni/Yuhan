import pytest
from subprocess import run


def get_program_output():
    return run("build/Yuhan-sintex-tests", encoding="utf-8", capture_output=True, input="sintex-test.y").stdout


def write_file(input_text):
    file = open("sintex-test.y", "w")
    file.write(input_text)
    file.close()


def read_file(input_file):
    file = open(input_file, "r")
    text = file.read()
    file.close()
    return text


def test_structs():
    file = read_file("inputs/structs.y")
    write_file(file)

    assert get_program_output() == "OK"


def test_literals():
    file = read_file("inputs/literals.y")
    write_file(file)

    assert get_program_output() == "OK"


def test_cycles():
    file = read_file("inputs/cycles.y")
    write_file(file)

    assert get_program_output() == "OK"
    

def test_if_else():
    file = read_file("inputs/if-else.y")
    write_file(file)

    assert get_program_output() == "OK"


def test_namespaces():
    file = read_file("inputs/namespaces.y")
    write_file(file)

    assert get_program_output() == "OK"


def test_functions():
    file = read_file("inputs/functions.y")
    write_file(file)

    assert get_program_output() == "OK"

