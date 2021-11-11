import os

if __name__ == "__main__":
    os.system("make splc")
    
    for file in os.listdir("./test-self/"):
        if file.endswith(".spl"):
            os.system("./bin/splc ./test-self/{}".format(file))
            
    for out_file in os.listdir("./test-self/"):
        if out_file.endswith(".out"):
            ans_list=[]
            with open("./test-self/ans/{}".format(out_file)) as ans:
                ans_text=ans.readlines()
                for line in ans_text:
                    ans_list.append(line.split(":")[0])
                    
            out_list=[]
            with open("./test-self/{}".format(out_file)) as out:
                out_text=out.readlines()
                for line in out_text:
                    out_list.append(line.split(":")[0])
                    
            for i in range(len(ans_list)):
                if ans_list[i]!=out_list[i]:
                    print("{} line {} is wrong.".format(out_file, i+1))
                