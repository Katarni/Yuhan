import pytest
from subprocess import run
import random


def generate_test():
    n = random.randint(3, 16)
    test = str(n) + "\n"
    q = random.randint(3, 16)
    test += str(q) + "\n"

    for i in range(q):
        t = random.randint(1, 2)
        l = random.randint(1, n)
        r = random.randint(l, n)
        test = str(t) + " " + str(l) + " " + str(r)

        if 1 == t:
            x = random.randint(1, 100)
            test += " " + str(x)

        test += "\n"

    return test


def get_program_output(test):
    return run("build/Yuhan-program-tests", encoding="utf-8", capture_output=True, input=test).stdout


def get_correct_output(test):
    return run("correct-programs/segment-tree", encoding="utf-8", capture_output=True, input=test).stdout


def test_segment_tree():
    for _ in range(30):
        test = generate_test()
        yuhan_program = get_program_output(test)
        correct_program = get_correct_output(test)

        if yuhan_program != correct_program:
            assert yuhan_program == correct_program

    assert 1
