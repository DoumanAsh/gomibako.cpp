#!/usr/bin/env python

from subprocess import check_output, check_call, CalledProcessError
from shutil import rmtree
from os import chdir, getcwd, path


def cmd_w_output(cmd):
    return check_output(cmd, shell=True, universal_newlines=True).strip()


def main():
    SCRIPT_DIR = path.dirname(path.realpath(__file__))
    chdir(SCRIPT_DIR)

    REPO = cmd_w_output("git config remote.origin.url")
    rmtree("html", True)

    check_call("git clone {} html".format(REPO))
    chdir("html")

    try:
        check_call("git checkout gh-pages")
    except OSError:
        check_call("git checkout --orphan gh-pages")

    check_call("git rm -rf .")
    chdir(SCRIPT_DIR)

    SHA = cmd_w_output("git log -1 --format=\"%s(%h %cd)\" --date=short")

    check_call("doxygen Doxygen.config")

    chdir("html")

    try:
        check_call("git add .")
        check_call("git commit -m \"Auto-update\" -m \"Commit: {}\"".format(SHA))
        check_call("git push origin HEAD")
    except CalledProcessError:
        pass

    chdir(SCRIPT_DIR)
    rmtree("html", True)


if __name__ == "__main__":
    OLD_DIR = getcwd()
    main()
    chdir(OLD_DIR)
