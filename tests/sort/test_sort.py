import pytest
from subprocess import run
import random


def generate_test():
    n = 25
    test = ""
    for i in range(n):
        test = str(random.randint(1, 100)) + " "

    test += "\n"

    return test


def get_program_output(test):
    return run("build/Yuhan-program-tests", encoding="utf-8", capture_output=True, input=test).stdout


def get_correct_output(test):
    return run("./sort", encoding="utf-8", capture_output=True, input=test).stdout


def test_sort():
    for _ in range(30):
        test = generate_test()
        yuhan_program = get_program_output(test)
        correct_program = get_correct_output(test)

        if yuhan_program != correct_program:
            assert yuhan_program == correct_program

    assert 1
