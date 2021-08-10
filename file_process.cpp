#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <unordered_map>
using namespace std;

// 读取第3行的【# Nodes: 281903 Edges: 2312497】中两个数字
string get_n_m_form_line3(string fileName, int &m, int &n)
{
    ifstream infile;
    infile.open(fileName);

    string line_str;
    int curr_line = 1;
	while (getline(infile, line_str))
	{
		if(curr_line == 3)
            break;
        else
            curr_line++;
	}
    //cout << line_str << endl;
	infile.close();

    string n_str, m_str;
    int lianxushuzi = 0;
    for(int i = 0; i < line_str.size(); i++)
    {
        if(line_str[i] >= '0' && line_str[i] <= '9')
        {
            if (lianxushuzi == 0)
            {
                lianxushuzi = 1;
                n_str += line_str[i];
            }
            else if (lianxushuzi == 1)
                n_str += line_str[i];
            else if (lianxushuzi == 2)
                m_str += line_str[i];
        }
        else
            if(lianxushuzi == 1)
                lianxushuzi = 2;
    }
    string n_m_str = n_str + ' ' + m_str + '\n';
    m = atoi(m_str.c_str());
    n = atoi(n_str.c_str());

    return n_m_str;
}

// 删除前lineNum行
void DelLineData(string fileName, int lineNum)
{
	ifstream infile;
	infile.open(fileName);
	
	string strFileData = "";
	int line = 1;
	string line_str;
	while(getline(infile, line_str))
	{
        if (line > lineNum)
		{
			strFileData += line_str;
			strFileData += '\n';
		}
		line++;
	}
	infile.close();
 
	//写入文件
	ofstream outfile;
	outfile.open(fileName);
	outfile.flush();
	outfile << strFileData;
	outfile.close();
}

// 在第一行插入代表n+m的字符串
void InsertData(string fileName, string insertData)
{
	ifstream infile;
	infile.open(fileName);
 
	string strFileData = "";
    string line_str;
    strFileData += insertData;
    while (getline(infile, line_str))
    {
        strFileData += line_str;
        strFileData += '\n';
    }
    infile.close();
 
	//写入文件
	ofstream outfile;
	outfile.open(fileName);
	outfile.flush();
	outfile<<strFileData;
	outfile.close();
}

struct Node {
    vector<int> outNodes;
};

void start_from_0_and_make_every_node_degree_not_0(string fileName)
{
    vector<Node> nodes;

    //先读文件
    ifstream inFile(fileName);
    if(!inFile)
    {
        cout << "open file " << fileName << " failed." << endl;
        return;
    }
    int n, m;
    inFile >> n >> m;
    nodes.resize(n);
    
    int u, v;
    
    int curr_n = 0;
    unordered_map<int, int> vertex_map; //原id: 新id
    unordered_map<int, int> vertex_reverse_map; // 新id: 原id

    while(inFile >> u >> v)
    {
        // 重新编号：对每个id做一个map映射，映射成0开始的连续编号，写入结果时再映射回最初编号
        if(vertex_map.find(u) == vertex_map.end()) {
            vertex_map[u] = curr_n;
            vertex_reverse_map[curr_n] = u;
            u = curr_n++;
        }
        else {
            u = vertex_map[u];
        }

        if(vertex_map.find(v) == vertex_map.end()){
            vertex_map[v] = curr_n;
            vertex_reverse_map[curr_n] = v;
            v = curr_n++;
        }
        else {
            v = vertex_map[v];
        }

        nodes[u].outNodes.emplace_back(v);
        //然后取vertex_reverse_map[key值]可以得到实际图中的点
    }
    inFile.close();

    // 让出度为0的点随机指向一个点
    for(int u = 0; u < n; u++)
    {
        int outDegree = nodes[u].outNodes.size();
        if (outDegree == 0)
        {
            srand(time(NULL));
            int random_outNode = rand() % n; // 把随机范围限制在[0, vertex_num-1] 之间
            nodes[u].outNodes.emplace_back(random_outNode);
            m++;
        }
    }

    //写入文件
	ofstream outfile;
	outfile.open(fileName);
	outfile.flush();
    outfile << n << ' ' << m;
    for (int i = 0; i < n; ++i)
    {
        int od = nodes[i].outNodes.size();
        for (int j = 0; j < od; ++j)
        {
            outfile << '\n' << i << ' ' << nodes[i].outNodes[j];
        }
    }
	outfile.close();
}


int main()
{
    int m = 0;
    int n = 0;
    string fileName = "/home/zyj/zhou/dataset/web-uk-2005-all1.mtx";

    // 从第三行中获取n和m
    // string str_insert_to_line1 = get_n_m_form_line3(fileName, m, n); 
    string str_insert_to_line1 = "39459925 936364282\n"; 

    // 删除前4行
    DelLineData(fileName, 3);
    
    // 把str_insert_to_line1插入第一行
    InsertData(fileName, str_insert_to_line1);

    // 预处理数据集，让点的下标从0开始，然后让出度为0的点随机指向一个点，这个点可以包括它自己
    start_from_0_and_make_every_node_degree_not_0(fileName);

    return 0;
}

