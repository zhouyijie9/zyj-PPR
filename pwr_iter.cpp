#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

struct Node {
    vector<int> outNodes;
};

int main()
{
    double alpha = 0.15;
    double threshold = 1e-6;

    string edge_path = "/home/zyj/zhou/dataset/a.txt";
    ifstream inFile(edge_path);
    if(!inFile)
    {
        cout << "open file " << edge_path << " failed." << endl;
        return 0;
    }
    int n, m;
    inFile >> n >> m;

    vector<double> gamma(n, 0); //gamma是一个1×n的向量，每个位置表示对应点的residue值
    //gamma[0] = 1.0;

    //================================
    //每个点的residue随机赋值0~1
    srand(time(NULL));
    int N = 999; //精度为小数点后面3位
    for(int i = 0; i < n; i++)
    {
        gamma[i] = rand() % (N + 1) / (float)(N + 1);
        cout << "点" << i << "的初始residue=" << gamma[i] << endl;
    }    
    //================================

    vector<double> ppr_value(n, 0); //每次迭代的结果累加到此向量中
    vector<vector<double> > Indicator(n, vector<double>(n, 0)); //Indicator是指示矩阵，为1表示此处有边
    vector<vector<double> > P(n, vector<double>(n, 0)); //P是转移矩阵

    // 向P中填入数据
    vector<Node> nodes(n); //nodes用来存放点向量
    int u, v;
    while (inFile >> u >> v)
    {
        nodes[u].outNodes.push_back(v);
        Indicator[u][v] = 1;
    }
    inFile.close();
    for(int i = 0; i < n; ++i)
    {
        int out_degree = nodes[i].outNodes.size();
        for(int j = 0; j < n; ++j)
        {
            P[i][j] = Indicator[i][j] / out_degree;
        }
    }

    //如果gamma向量中每个点的residue值都小于threshold，就可以停止迭代
    int iter_num = 0;
    int should_stop = 0;
    while(1)
    {
        iter_num++;
        
        for (int i = 0; i < n; i++)
        {
            ppr_value[i] += alpha * gamma[i];
        }

        vector<double> tmp_gamma(n, 0); //gamma是一个1×n的向量，每个位置表示对应点的residue值

        for (int i = 0; i < n; i++)
        {
            double tmp = 0;
            for (int j = 0; j < n; j++)
            {
                tmp += P[j][i] * gamma[j];
            }
            tmp_gamma[i] = (1 - alpha) * tmp;
            if (tmp_gamma[i] < threshold)
                should_stop++;
        }
        gamma = tmp_gamma;

        if(should_stop == n)
            break;
        else
            should_stop = 0;
    }
    cout << "iter_num = " << iter_num << endl;
    for (int i = 0; i < n; i++)
    {
        cout << ppr_value[i] << endl;
    }
    cout << endl;
}