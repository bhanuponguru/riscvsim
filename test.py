import subprocess
import os

def test():
    for i in range(1,4):
        inp=open(f"tests/test{i}/input.txt","r")
        out=open(f"tests/test{i}/output.txt","w")
        error=open(f"tests/test{i}/error.log","w")
        exitcode=subprocess.run(["./riscv_sim"],stdin=inp,stdout=out,stderr=error)
        out.close()
        inp.close()
        error.close()

def main():
    test()

if __name__ == "__main__":
    main()