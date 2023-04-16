#### change this to the side length
X=60
print("X=",X)

###
low=5
for i in range(0,(X-low)):
    obs="("
    for j in range(low,X):
        obs= obs+"(y_axis="+str(j)+")"
        if(j!=(X-1)):
            obs=obs+" | "
        else:
            obs=obs+")"
    # print(obs)
    x_axis= "(x_axis="+str(i)
    print(x_axis, "&", obs,"): act;")
    low=low+1

print()
print("-----1")

###
high=0
for i in range(5,X):
    obs="("
    for j in range(0,high+1):
        obs= obs+"(y_axis="+str(j)+")"
        if(j!=high):
            obs=obs+" | "
        else:
            obs=obs+")"
    # print(obs)
    x_axis= "(x_axis="+str(i)
    print(x_axis, "&", obs,"): act;")
    high=high+1

print()
print("-----2")

###
for i in range(0,X-3):
    x="(x_axis="+str(i)
    y= "y_axis="+str(i+3)+")"
    print(x, "&", y, ":{2,4};")

print()
print("-----3")
###
for i in range(0,X-3):
    x="(x_axis="+str(i+3)
    y= "y_axis="+str(i)+")"
    print(x, "&", y, ":{1,3};")



print()
print("-----4")
###
for i in range(1,X):
    x="(x_axis="+str(i)
    left=i-1
    right=i+1
    print(x, "& act=3) : ",left,";")
    print(x, "& act=4) : ",right,";")

print()
print("-----5")
###
for i in range(1,X):
    y="(y_axis="+str(i)
    down=i-1
    up=i+1
    print(y, "& act=2) : ",down,";")
    print(y, "& act=1) : ",up,";")
