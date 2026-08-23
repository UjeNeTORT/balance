from pathlib import Path
from subprocess import Popen, PIPE
import argparse

def create_build_dir(build_dir, test):
    Path(build_dir + "/tests/" + test).parent.mkdir(parents=True, exist_ok=True)

def compile(compiler, build_dir, test):
    # compile_res = Popen([compiler, "-S", "-o", build_dir + "/tests/" + test + ".s",
    #                                            "./" + test + ".sy"])
    compile_res = Popen([compiler, "./" + test + ".sy",
                                   build_dir + "/tests/" + test + ".s"])

    compile_res.wait()
    if compile_res.returncode != 0:
        print("[FAIL] [COMPILE FAIL]")
        exit(1)

def link(asm_linker, build_dir, test, runtime):
    link_res = Popen([asm_linker, "-static", build_dir + "/tests/" + test + ".s",
                      runtime, "-Wl,--undefined=before_main",
                      "-o", build_dir + "/tests/" + test])

    link_res.wait()
    if link_res.returncode != 0:
        print("[FAIL] [LINK FAIL]")
        exit(2)

def run_test(interpreter, build_dir, test):
    try:
        with open(test + ".in", "rb") as file:
            stdin = file.read()
    except:
        stdin = bytes()

    with open(test + ".out", "rb") as file:
        stdout = file.read()

    p = Popen([interpreter, build_dir + "/tests/" + test],
              stdin=PIPE, stdout=PIPE, stderr=PIPE)

    stdout_data, stderr_data = p.communicate(input=stdin)
    ret = p.returncode

    p.wait()

    print("stderr:\n")
    print(stderr_data)

    fail = False
    if stdout != stdout_data:
        print("\tstdout doesn't match")
        print("Expected:")
        print(stdout)
        print("Got:")
        print(stdout_data)
        fail = True

    if ret != 0:
        print(f"\t[WARN] return code != 0: got {ret}")

    if fail:
        print(f"\t[FAIL]")
        exit(3)
    else:
        print(f"\t[PASS]")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("test", help="test name")
    parser.add_argument("--compile-only", action='store_true', default=False,
                                            help="compile and not run")
    parser.add_argument("-c", "--compiler", default="../build/compiler",
                                            help="compiler executable path")
    parser.add_argument("-l", "--asm-linker", default="riscv64-linux-gnu-gcc",
                                            help="assembler/linker executable path")
    parser.add_argument("-i", "--interpreter", default="qemu-riscv64",
                                            help="interpreter executable path")
    parser.add_argument("-r", "--runtime", default="runtime/sylib.c",
                                            help="SySy runtime source file")
    parser.add_argument("-b", "--build-dir", default="./build",
                                            help="build directory")

    args = parser.parse_args()

    create_build_dir(args.build_dir, args.test)
    compile(args.compiler, args.build_dir, args.test)
    link(args.asm_linker, args.build_dir, args.test, args.runtime)

    if not args.compile_only:
        run_test(args.interpreter, args.build_dir, args.test)

