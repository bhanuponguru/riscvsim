import subprocess
import os

def test():
    count=0
    total=0
    for i in range(1,3):
        inp=open(f"tests/test{i}/input.txt","r")
        out=open(f"tests/test{i}/output.txt","w")
        error=open(f"tests/test{i}/error.log","w")
        exitcode=subprocess.run(["./riscv_sim"],stdin=inp,stdout=out,stderr=error)
        out.close()
        inp.close()
        error.close()
        passed=True
        expected_input_output=open(f"tests/test{i}/expected_input{i}.output","r")
        input_output=open(f"tests/test{i}/input{i}.output","r")
        expected_output=expected_input_output.read()
        output=input_output.read()
        expected_input_output.close()
        input_output.close()
        if expected_output!=output:
            passed=False
            print(f"Test {i} failed")
            print(f"Expected output: {expected_output}")
            print(f"Output: {output}")
        expected_filename_ext=open(f"tests/test{i}/expected_filename.ext","r")
        filename_ext=open(f"tests/test{i}/filename.ext","r")
        expected_dump=expected_filename_ext.read()
        dump=filename_ext.read()
        expected_filename_ext.close()
        filename_ext.close()
        if dump!=expected_dump:
            passed=False
            print(f"Test {i} failed")
            print(f"Expected dump: {expected_dump}")
            print(f"Dump: {dump}")
        expected_output_file=open(f"tests/test{i}/expected_output.txt","r")
        output_file=open(f"tests/test{i}/output.txt","r")
        expected_output=expected_output_file.read()
        output=output_file.read()
        expected_output_file.close()
        output_file.close()
        if expected_output!=output:
            passed=False
            print(f"Test {i} failed")
            print(f"Expected output: {expected_output}")
            print(f"Output: {output}")
        if passed: count+=1
        total+=1
    print(f"Tests passed: {count}/{total}")

def main():
    test()

if __name__ == "__main__":
    main()