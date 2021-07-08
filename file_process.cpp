#include <iostream>
#include <fstream>
using namespace std;

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

// 读取第3行的【# Nodes: 281903 Edges: 2312497】中两个数字
string get_n_m_form_line3(string fileName)
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
    return n_m_str;
}



int main()
{
    string fileName = "/home/zyj/zhou/dataset/web-Stanford1.txt";

    string str_insert_to_line1 = get_n_m_form_line3(fileName); // 从第三行中获取n和m

    // 删除前4行
    DelLineData(fileName, 4);
    
    // 把str_insert_to_line1插入第一行
    InsertData(fileName, str_insert_to_line1);

    return 0;
}

