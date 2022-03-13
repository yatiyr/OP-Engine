import pkg_resources
import subprocess

def CheckPackages(packages):
    existingPackages = [pkg.key for pkg in pkg_resources.working_set]

    for pkg in packages:
        if pkg not in existingPackages:
            print(f"{pkg} is being installed...")
            subprocess.check_call(["python","-m","pip","install", pkg])


def Run():
    CheckPackages(["requests", "fake-useragent"])