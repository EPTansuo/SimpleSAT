#!/bin/env python

import os
import time
from pathlib import Path

def get_cnf_files(folder_path):
    cnf_files = []
    for filename in os.listdir(folder_path):
        file_path = os.path.join(folder_path, filename)
        if os.path.isfile(file_path) and filename.endswith(".cnf"):
            cnf_files.append(file_path)
    return cnf_files

def main():

    size = 100
    

    result_pass = 0
    result_faild = 1
    result_unknow = 3

    dut = "../build/SimpleSAT -m CDCL"
    results = []
    durations = []

    cnf_files = get_cnf_files(os.getcwd() + "/aim")

    #cnf_filter_files = [cnf_file for cnf_file in cnf_files if "50-1" in cnf_file or "50-2" in cnf_file ]#or "100" in cnf_file ]  
    cnf_filter_files = cnf_files

    cnf_files = cnf_filter_files
    
    cnf_files = sorted(cnf_files, key=lambda x: (int (Path(x).name.split('-')[1]), x))
    for cnf_file in cnf_files:
        if(size == 0):
            results.append(3)
            continue
        size -= 1

        print(f"Testing {cnf_file}... ", end="")
        start_time = time.time() 
        ret = os.system(f"{dut} {cnf_file}")
        end_time = time.time()
        elapsed = end_time - start_time
        durations.append(elapsed) 

        ret >>= 8   

        expect =  10 if cnf_file.split("-")[3].startswith("yes") else 20
        
        if( ret != 10 and ret != 20):
            results.append(result_unknow)
        elif ret == expect:
            results.append(result_pass)
        else:
            results.append(result_faild)
    
    print("\n")

    cnf_files_basename = [os.path.basename(cnf_file) for cnf_file in cnf_files]
    max_legth = max([len(cnf_file) for cnf_file in cnf_files_basename])

    cnf_files_basename_align = [cnf_file.ljust(max_legth) for cnf_file in cnf_files_basename]


    for result,cnf_file, duration in zip(results,cnf_files_basename_align, durations):
        
        print(f"{os.path.basename(cnf_file)}: ", end="")
        result_str=""
        if result == result_pass:
            result_str = ("[\33[1;32mPASS\33[0m]")
        #elif result == 2:
        #    print("[\33[1;33mERROR\33[0m]")
        elif result == result_faild:
            result_str = ("[\33[1;31mFAIL\33[0m]")
        else:
            result_str = ("[\33[1;34mUNKNOWN\33[0m]")

        print(f"{result_str}  {duration:.5f} s")


if __name__ == "__main__":
    main()
