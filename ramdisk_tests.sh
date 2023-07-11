

# x=1
# y=1
# z=""
# while [ $y -le 10000 ]
# do
#     while [ $x -le 10000 ]
#     do
#         echo $x >> test.txt
#         ((x++))
#     done
#     while read -r line; 
#     do
#         z=${line}
#     done < "test.txt"
#     ((y++))
# done

# > test.txt



time diskutil erasevolume apfs "RAMDisk" `hdiutil attach -nomount ram://100000`
touch /Volumes/RAMDisk/test.txt

x=1
y=1
z=""
while [ $y -le 10000 ]
do
    while [ $x -le 10000 ]
    do
        echo $x >> /Volumes/RAMDisk/test.txt
        ((x++))
    done
    while read -r line; 
    do
        z=${line}
    done < "/Volumes/RAMDisk/test.txt"
    ((y++))
done

time diskutil unmount /Volumes/RAMDisk