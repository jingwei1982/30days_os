with open("hankaku.txt",'r') as f:
    count=0
    for line in f:
        if '*' in line or '.' in line:
            count+=1
            sum=0
            for i in range(8):
                if line[i]=="*":
                    sum+=1<<(7-i)
            print('0x%x,' %sum,end="")
            if count%16==0:
                print(" ",end="\n")