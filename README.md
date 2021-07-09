# 增量PPR

### file_process.cpp
web_stanford.txt前10行：
```
# Directed graph (each unordered pair of nodes is saved once): web-Stanford.txt 
# Stanford web graph from 2002
# Nodes: 281903 Edges: 2312497
# FromNodeId	ToNodeId
1	6548
1	15409
6548	57031
15409	13102
2	17794
2	25202
```
我想要的格式：第一行点数+边数，随后每行代表一条边。file_process.cpp进行了实现。

### pwr_iter.cpp 公式：

<!-- $\vec{\gamma_s}^{(0)}=\vec{e_s}$ -->
![](https://latex.codecogs.com/svg.latex?\vec{\gamma_s}^{(0)}=\vec{e_s})

<!-- $\vec{\gamma_s}^{(j+1)}=(1-\alpha)\cdot\vec{\gamma_s}^{(j)}\cdot\mathbf{P}=(1-\alpha)^{j+1}\cdot\vec{e_s}\cdot\mathbf{P}^{j+1}$ -->
![](https://latex.codecogs.com/svg.latex?\vec{\gamma_s}^{(j+1)}=(1-\alpha)\cdot\vec{\gamma_s}^{(j)}\cdot\mathbf{P}=(1-\alpha)^{j+1}\cdot\vec{e_s}\cdot\mathbf{P}^{j+1})


<!-- $\vec{\pi_s}=\sum\limits_{j=0}^\infty\alpha\cdot\vec{\gamma_s}^{(j)}$ -->
![](https://latex.codecogs.com/svg.latex?\vec{\pi_s}=\sum\limits_{j=0}^\infty\alpha\cdot\vec{\gamma_s}^{(j)})


<!-- $\vec{\pi_s}^{(j+1)}=\sum\limits_{k=0}^j\alpha\cdot\vec{\gamma_s}^{(k)}$ -->
![](https://latex.codecogs.com/svg.latex?\vec{\pi_s}^{(j+1)}=\sum\limits_{k=0}^j\alpha\cdot\vec{\gamma_s}^{(k)})


### pwr_iter2.cpp 公式

<!-- $PPR(i)=(1-d)r_i+d\sum\limits_{j\in in(i)}\frac{PPR(j)}{|out(i)|},$ -->

![](https://latex.codecogs.com/svg.latex?PPR(i)=(1-d)r_i+d\sum\limits_{j\in&space;in(i)}\frac{PPR(j)}{|out(i)|},)

<!-- $r_i=\begin{cases}1,\quad i=u\\0,\quad i\neq u\end{cases}$ -->
![](https://latex.codecogs.com/svg.latex?r_i=\begin{cases}1,\quad&space;i=u\\\\0,\quad&space;i\neq&space;u\end{cases})



