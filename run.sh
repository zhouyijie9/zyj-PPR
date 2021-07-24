make clean
make
edegpath=/home/zyj/zhou/dataset/web-Google1.txt # 原图边文件
name=google_yasuo # 生成的文件名
threshold=0.000001 # 收敛阈值
CLUSTER_THRESHOLD=40 # 压缩参数
VIRTUAL_THRESHOLD=2 #可能是节省的边的数量

# 写入文件
# destdir="./out/stf_yasuo.txt"
# echo "edgefile:$edegpath" > "${destdir}"        # 截断文件后，追加文本
# echo "threshold:$threshold" >> "${destdir}" # 将文本附加
# echo "name:$name" >> "${destdir}" # 将文本附加

# 压缩
./virtual_node_miner  ${edegpath} ./out/${name}.e ./out/${name}.v $CLUSTER_THRESHOLD $VIRTUAL_THRESHOLD

# 压缩图计算
#g++ ppr_yasuo.cpp
#./a.out $threshold ${edegpath} ${name}
#echo -e "压缩图计算完成...\n"

# 原图计算
#g++ ppr.cpp
#./a.out $edegpath $threshold
#echo -e "原图计算完成...\n"