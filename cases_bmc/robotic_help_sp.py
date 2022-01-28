import sys
import numpy as np
import os.path
from os import path
#### change this to the side length
# X=20
# num_obs=120

X=60
num_obs=2000


print("X=",X)

# static obs
sp_obs=[[1,3], [1,5], [1,6], [1,8], [1,9]
        ,[2,6], [2,8], [2,9]
        ,[3,1], [3,4], [3,5] ,[3,6], [3,8]
        ,[4,5], [4,6], [4,9]
        ,[5,0], [5,4]
        ,[6,3], [6,4], [6,6]
        ,[7,4], [7,6]
        ,[8,5], [8,6]
        ,[9,1]]
print(sp_obs)
### 26 static obs



# x_random=np.random.choice(range(10,X), num_obs, replace=False)
# y_random=np.random.choice(range(10,X), num_obs, replace=False)


## generate unique obstacles
while(len(sp_obs) < num_obs):
    x_random=np.random.choice(range(10,X), 1)
    y_random=np.random.choice(range(10,X), 1)
    if([x_random, y_random] not in sp_obs):
        sp_obs.append([x_random[0], y_random[0]])


print(sp_obs)


r = open("temp.smv", "w")
# P_bool.write(text)

counter=0
r.write("FAIL scenarios: \n")
for i in range(0,len(sp_obs)):
    pos_x=(sp_obs[i][0])
    pos_y=(sp_obs[i][1])
    x="x_axis="+str(pos_x)
    y="y_axis="+str(pos_y)
    r.write("| ("+x+ " & "+y+")")
    counter+=1
    if(counter==8):
        r.write("\n")
        counter=0


r.write("\n\n")


LEFT=set()
RIGHT=set()
UP=set()
DOWN=set()



for i in range(0,len(sp_obs)):
    pos_x=(sp_obs[i][0])
    pos_y=(sp_obs[i][1])
    x="x_axis="+str(pos_x)
    y="y_axis="+str(pos_y)

    # print("(",x, "&",y,"): act;")
    # r.write("("+x+ " & "+y+"): act; \n")

    if((pos_x-1)>=0):
        x_left="x_axis="+str(pos_x-1)
        RIGHT.add("("+x_left+ " & "+y+")")
        # if((pos_x-1)==0):
        #     # print("(",x_left, " & ",y,"): {1,2};")
        #     # r.write("("+x_left+" & "+y+"): {1,2}; \n")
        #     UP.add("("+x_left+ " & "+y+")")
        #     DOWN.add("("+x_left+ " & "+y+")")
        #
        #
        # else:
        #     # print("(",x_left, " & ",y,"): {1,2,3};")
        #     # r.write("("+x_left+ " & "+y+"): {1,2,3}; \n")
        #     UP.add("("+x_left+ " & "+y+")")
        #     DOWN.add("("+x_left+ " & "+y+")")
        #     LEFT.add("("+x_left+ " & "+y+")")



    if((pos_x+1)<X):
        x_right="x_axis="+str(pos_x+1)
        LEFT.add("("+x_right+ " & "+y+")")
        # if((pos_x+1)==9):
        #     # print("(",x_right, " & ",y,"): {1,2};")
        #     # r.write("("+x_right+ " & "+y+"): {1,2};\n")
        #     UP.add("("+x_right+ " & "+y+")")
        #     DOWN.add("("+x_right+ " & "+y+")")
        #
        #
        # else:
        #     # print("(",x_right, " & ",y,"): {1,2,4};")
        #     # r.write("("+x_right+ " & "+y+"): {1,2,4};\n")
        #     UP.add("("+x_right+ " & "+y+")")
        #     DOWN.add("("+x_right+ " & "+y+")")
        #     RIGHT.add("("+x_right+ " & "+y+")")

    if((pos_y-1)>=0):
        y_down="y_axis="+str(pos_y-1)
        UP.add("("+x+ " & "+y_down+")")
        # if((pos_y-1)==0):
        #     # print("(",x, "&",y_down,"): {3,4};")
        #     # r.write("("+x+ " & "+y_down+"): {3,4};\n")
        #     LEFT.add("("+x+ " & "+y_down+")")
        #     RIGHT.add("("+x+ " & "+y_down+")")
        #
        # else:
        #     # print("(",x, "&",y_down,"): {2,3,4};")
        #     # r.write("("+x+ " & "+y_down+"): {2,3,4};\n")
        #     LEFT.add("("+x+ " & "+y_down+")")
        #     RIGHT.add("("+x+ " & "+y_down+")")
        #     DOWN.add("("+x+ " & "+y_down+")")

    if((pos_y+1)<X):
        y_up="y_axis="+str(pos_y+1)
        DOWN.add("("+x+ " & "+y_up+")")

        # if((pos_y+1)==9):
        #     # print("(",x, "&",y_up,"): {3,4};")
        #     # r.write("("+x+ " & "+y_up+"): {3,4};\n")
        #     LEFT.add("("+x+ " & "+y_up+")")
        #     RIGHT.add("("+x+ " & "+y_up+")")
        #
        # else:
        #     # print("(",x, "&",y_up,"): {1,3,4};")
        #     # r.write("("+x+ " & "+y_up+"): {1,3,4};\n")
        #     LEFT.add("("+x+ " & "+y_up+")")
        #     RIGHT.add("("+x+ " & "+y_up+")")
        #     UP.add("("+x+ " & "+y_up+")")


counter=0
r.write("LEFT CONSTRAINTS: \n")
for left in LEFT:
    r.write(" & !"+left)
    counter+=1
    if(counter==8):
        r.write("\n")
        counter=0

r.write("\n\n\n\n")

counter=0
r.write("RIGHT CONSTRAINTS: \n")
for right in RIGHT:
    r.write(" & !"+right)
    counter+=1
    if(counter==8):
        r.write("\n")
        counter=0
r.write("\n\n\n\n")

counter=0
r.write("UP CONSTRAINTS: \n")
for up in UP:
    r.write(" & !"+up)
    counter+=1
    if(counter==8):
        r.write("\n")
        counter=0
r.write("\n\n\n\n")

counter=0
r.write("DOWN CONSTRAINTS: \n")
for down in DOWN:
    r.write(" & !"+down)
    counter+=1
    if(counter==8):
        r.write("\n")
        counter=0




# r.write("-----x \n")
# ###
# for i in range(1,X):
#     x="(x_axis="+str(i)
#     left=str(i-1)
#     right=str(i+1)
#     r.write(x+ " & LEFT & RIGHT) : {"+left+","+right+"}; \n")
#     r.write(x+ " & LEFT & !RIGHT) : {"+left+"}; \n")
#     r.write(x+ " & !LEFT & RIGHT) : {"+right+"}; \n")
#
#
# r.write("-----y \n")
# ###
# for i in range(1,X):
#     y="(y_axis="+str(i)
#     down=str(i-1)
#     up=str(i+1)
#     r.write(y+ " & UP & DOWN) : {"+up+","+down+"}; \n")
#     r.write(y+ " & UP & !DOWN) : {"+up+"}; \n")
#     r.write(y+ " & !UP & DOWN) : {"+down+"}; \n")






r.close()
