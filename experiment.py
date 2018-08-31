from subprocess import call
import os
import shutil

output_file = "./experiments.txt"

def run(data_num, cpu_rate, t_lim):
    print("building")
    shutil.rmtree("./build")
    shutil.rmtree("./bin")
    os.mkdir("./build")
    os.mkdir("./bin")
    call(["cmake", ".."], cwd="./build")
    call(["make"], cwd="./build")
    print("runing online")
    status = call(["./solve", data_num, "0", str(cpu_rate), str(t_lim)], cwd="./bin")
    if status != 0 :
        return 


    print("runing move")
    call(["./solve", data_num, "2", str(cpu_rate), str(t_lim)], cwd="./bin")

    print("runing offline")
    call(["./solve", data_num, "1", str(cpu_rate), str(t_lim)], cwd="./bin")

    filenames = ["./submit_final_"+data_num+"_tmp1_s.csv", "./submit_final_"+data_num+"_tmp1_o.csv"]
    with open("./submit_final_output_" + data_num + ".csv", "w") as outfile:
        for fname in filenames:
            with open(fname) as infile:
                for line in infile:
                    outfile.write(line)

    call(["java", "-jar", "Evaluation.jar", 
        "./dataset/app_resources.csv",
        "./dataset/machine_resources." + data_num + ".csv",
        "./dataset/instance_deploy." + data_num + ".csv",
        "./dataset/app_interference.csv",
        "./dataset/job_info." + data_num + ".csv",
        "./submit_final_output_" + data_num + ".csv"
        ])
    
    with open("./result.txt", "r") as f:
        score = float(f.readline())
        with open(output_file, "a") as ff:
            ff.write("data_num: {}, cpu_rate: {:.5f}, time_lim: {}, score: {:.5f}\n"
                .format(data_num, cpu_rate, t_lim, score))

run("a", 2.01, 60*5)