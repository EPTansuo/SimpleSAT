#!/bin/env python

import os


def get_cnf_files(folder_path):
    cnf_files = []
    for filename in os.listdir(folder_path):
        file_path = os.path.join(folder_path, filename)
        if os.path.isfile(file_path) and filename.endswith(".cnf"):
            cnf_files.append(file_path)
    return cnf_files

def main():

    size = 100


    dut = "../build/simple_sat"
    results = []

    cnf_files = get_cnf_files(os.getcwd() + "/aim")

    cnf_filter_files = [cnf_file for cnf_file in cnf_files if "50" in cnf_file ]#or "100" in cnf_file ]

    cnf_files = cnf_filter_files

    for cnf_file in cnf_files:
        if(size == 0):
            results.append(3)
            continue
        size -= 1

        print(f"Testing {cnf_file}... ", end="")
        ret = os.system(f"{dut} {cnf_file}")
        ret >>= 8   

        expect =  1 if cnf_file.split("-")[3].startswith("yes") else 0
        
        if ret == expect:
            results.append(0)
        elif ret == 2:
            results.append(2)
        else:
            results.append(1)
    
    print("\n")

    cnf_files_basename = [os.path.basename(cnf_file) for cnf_file in cnf_files]
    max_legth = max([len(cnf_file) for cnf_file in cnf_files_basename])

    cnf_files_basename_align = [cnf_file.ljust(max_legth) for cnf_file in cnf_files_basename]


    for result,cnf_file in zip(results,cnf_files_basename_align):
        
        print(f"{os.path.basename(cnf_file)}: ", end="")
        if result == 0:
            print("[\33[1;32mPASS\33[0m]")
        elif result == 2:
            print("[\33[1;33mERROR\33[0m]")
        elif result == 1:
            print("[\33[1;31mFAIL\33[0m]")
        else:
            print("[\33[1;34mUNKNOWN\33[0m]")



if __name__ == "__main__":
    main()
