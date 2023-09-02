#!/usr/bin/env python3

"""\
BetterTesting is a CLI/GUI tool that helps you test your code, better!
"""


__version__ = "1.0.1"
__author__ = "William Huynh"
__copyright__ = "Copyright 2023, Reaver Ltd."


import argparse
import os
import subprocess
import queue
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed

# "File" will suggest the absolute path to the file, including the extension.
SCRIPT_LOCATION = Path(__file__).resolve().parent
MAKEFILE = SCRIPT_LOCATION.joinpath("../../..").resolve()
OUTPUT_FOLDER = SCRIPT_LOCATION.joinpath("../bin/output").resolve()
J_UNIT_OUTPUT_FILE = SCRIPT_LOCATION.joinpath("../bin/junit.xml").resolve()
COMPILER_TEST_FOLDER = SCRIPT_LOCATION.joinpath(
    "../../../compiler_tests").resolve()
COMPILER_FILE = SCRIPT_LOCATION.joinpath("../../../bin/c_compiler").resolve()


def fail_testcase(
    init_message: tuple[str, str],
    message: str,
    log_queue: queue.Queue
):
    init_print_message, init_xml_message = init_message
    print_message = f"\t> {message}"
    xml_message = (
        f'<error type="error" message="{message}">{message}</error>\n'
        '</testcase>\n'
    )
    log_queue.put((init_print_message + print_message,
                   init_xml_message + xml_message))


def run_test(driver: Path, log_queue: queue.Queue) -> int:
    """
    Run an instance of a test case.

    Returns:
    1 if passed, 0 otherwise. This is to increment the pass counter.
    """

    # Determine the relative path to the file wrt. COMPILER_TEST_FOLDER.
    relative_path = driver.relative_to(COMPILER_TEST_FOLDER)

    # Construct the path where logs would be stored.
    log_path = Path(OUTPUT_FOLDER).joinpath(
        relative_path).with_suffix(".log")

    # Ensure the directory exists.
    log_path.parent.mkdir(parents=True, exist_ok=True)

    to_assemble = driver.with_suffix("").with_suffix(".c")
    init_message = (str(to_assemble) + "\n",
                    f'<testcase name="{to_assemble}">\n')

    for suffix  in [".s", ".o", ""]:
        log_path.with_suffix(suffix).unlink(missing_ok=True)

    # Compile
    compiler_result = subprocess.run(
        [
            COMPILER_FILE,
            "-S", str(to_assemble),
            "-o", f"{log_path}.s",
        ],
        stderr=open(f"{log_path}.compiler.stderr.log", "w"),
        stdout=open(f"{log_path}.compiler.stdout.log", "w")
    )

    if compiler_result.returncode != 0:
        fail_testcase(
            init_message,
            f"Fail: see {log_path}.compiler.stderr.log "
            f"and {log_path}.compiler.stdout.log",
            J_UNIT_OUTPUT_FILE
        )
        return 0

    # Assemble
    assembler_result = subprocess.run(
        [
            "riscv64-unknown-elf-gcc",
            "-march=rv32imfd", "-mabi=ilp32d",
            "-o", f"{log_path}.o",
            "-c", f"{log_path}.s"
        ],
        stderr=open(f"{log_path}.assembler.stderr.log", "w"),
        stdout=open(f"{log_path}.assembler.stdout.log", "w")
    )

    if assembler_result.returncode != 0:
        fail_testcase(
            init_message,
            f"Fail: see {log_path}.assembler.stderr.log "
            f"and {log_path}.assembler.stdout.log",
            log_queue
        )
        return 0

    # Link
    linker_result = subprocess.run(
        [
            "riscv64-unknown-elf-gcc",
            "-march=rv32imfd", "-mabi=ilp32d", "-static",
            "-o", f"{log_path}",
            f"{log_path}.o", str(driver)
        ],
        stderr=open(f"{log_path}.linker.stderr.log", "w"),
        stdout=open(f"{log_path}.linker.stdout.log", "w")
    )

    if linker_result.returncode != 0:
        fail_testcase(
            init_message,
            f"Fail: see {log_path}.linker.stderr.log "
            f"and {log_path}.linker.stdout.log",
            log_queue
        )
        return 0

    # Simulate
    simulation_result = subprocess.run(
        ["spike", "pk", log_path],
        stdout=open(f"{log_path}.simulation.log", "w")
    )

    if simulation_result.returncode != 0:
        fail_testcase(
            init_message,
            f"Fail: simulation did not exit with exitcode 0",
            log_queue
        )
        return 0
    else:
        init_print_message, init_xml_message = init_message
        log_queue.put((init_print_message + "\t> Pass",
                       init_xml_message + "</testcase>\n"))

    return 1


def main(multithreading: bool):
    Path(OUTPUT_FOLDER).mkdir(parents=True, exist_ok=True)

    subprocess.run(["make", "-C", MAKEFILE, "bin/c_compiler"])

    with open(J_UNIT_OUTPUT_FILE, "w") as f:
        f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
        f.write('<testsuite name="Integration test">\n')

    drivers = list(Path(COMPILER_TEST_FOLDER).rglob("*_driver.c"))
    drivers = sorted(drivers, key=lambda p: (p.parent.name, p.name))
    log_queue = queue.Queue()
    results = []

    def empty_log_queue():
        while not log_queue.empty():
            print_msg, xml_message = log_queue.get()
            print(print_msg)
            with open(J_UNIT_OUTPUT_FILE, "a") as xml_file:
                xml_file.write(xml_message)

    if multithreading:
        with ThreadPoolExecutor() as executor:
            futures = [executor.submit(run_test, driver, log_queue)
                    for driver in drivers]

            for future in as_completed(futures):
                results.append(future.result())
                empty_log_queue()

    else:
        for driver in drivers:
            result = run_test(driver, log_queue)
            results.append(result)
            empty_log_queue()

    passing = sum(results)
    total = len(drivers)

    with open(J_UNIT_OUTPUT_FILE, "a") as f:
        f.write('</testsuite>\n')

    print(f"\n Passed {passing}/{total} testcases")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-m", "--multithreading",
        action="store_true",
        default=False,
        help="Use multiple threads to run tests. This will make it faster, "
        "but order is not guaranteed. Should only be used for speed."
    )
    parser.add_argument(
        "--version",
        action="version",
        version=f"BetterTesting {__version__}"
    )
    args = parser.parse_args()

    try:
        main(args.multithreading)
    except Exception as e:
        print(f"Exception encountered: {e}")
    finally:
        # This solves dodgy terminal behaviour on multithreading
        os.system("stty echo")
