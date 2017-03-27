#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

int total_id;
int total_res;
int graph[10][10];
int front=-1, rear=-1;


struct transaction{

	string op;
	int id;
	char res_id;
	int res;
};

void add(int *queue, int vertex)
{
	if(rear == total_id - 1)
	{
		printf("Queue Overflow\n");
	}
	else
	{
		if (front == -1) 
		{
			front = 0;
		}
		rear = rear + 1;
		queue[rear] = vertex ;
	}
}
 
int isEmpty()
{
	if(front == -1 || front > rear )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
 
int del(int *queue)
{
	int element;
	if (front == -1 || front > rear)
	{
		printf("Queue Underflow\n");

	}
	else
	{
		element = queue[front];
		front = front+1;
		return element;
	}
}

int find_indegree_of_vertex(int graph[][10], int vertex)
{
	int count, total_indegree = 0;
	for(count = 0; count < total_id; count++)
	{
		if(graph[count][vertex] == 1)
		{
			total_indegree++;
		}
	}
	return total_indegree;
}

void topological_sort(int graph[][10])
{
	int queue[total_id];
	int i, vertex, count, topological_sort[total_id], indegree[total_id];

	for(i = 0; i < total_id; i++)
	{
		indegree[i] = find_indegree_of_vertex(graph , i);
		if(indegree[i] == 0)
		{
			add(queue , i);
		}
	}

	count = 0;
	while(!isEmpty() && count < total_id)
	{
		vertex = del(queue);
    		topological_sort[++count] = vertex;
		for(i = 0; i < total_id; i++)
		{
			if(graph[vertex][i] == 1)
			{
				graph[vertex][i] = 0;
				indegree[i] = indegree[i] - 1;
				if(indegree[i] == 0)
				{
					add(queue , i);
				}
			}
		}
	}
	if(count < total_id)
	{
		printf("Given schedule is not conflict serializable\n");
	}
	else{
		printf("Order of transactions\n");
		for(i = 1; i <= count; i++)
		{
			printf("%3d", topological_sort[i]+1);
		}
		printf("\n");
	}

}


int main(int argc, char **argv)
{

	char filename[50];
	strcpy(filename,argv[1]);
	vector<transaction> V;
	
	ifstream file(filename);
	string line;

	while(getline(file, line))
	{
    	stringstream linestream(line);
    	string data1;
    	char c;
    	int val1;

    	linestream >> data1 >> val1 >> c;
    	transaction A;
    	A.op = data1;
    	A.id = val1;
    	A.res_id = c;
    	A.res = 0;
    	V.push_back(A);
	}

	vector<char> B;
	for(int i=0;i<V.size();i++)
	{
		int flag=0;
		//string str = V[i].res_id;
		//const char * c = str.c_str();
		//cout<<"string of transaction is "<<V[i].res_id<<"\n";
		for(int j=0;j<B.size();j++)
		{
			if(B[j]==V[i].res_id)
				{
					flag = 1;
					break;
				}
		}
		//cout<<"string of transaction is "<< V[i].res_id <<" flag is "<<flag<<"\n";
		if(flag==0)
			B.push_back(V[i].res_id);
		//cout<<B[0]<<"\n";
	}

	int x=1;
	for(int i=0;i<B.size();i++)
	{
		for(int j=0;j<V.size();j++)
		{
			//string str = V[j].res_id;
			//const char * c = str.c_str();
			if(V[j].res_id==B[i])
				V[j].res=x;
		}
		x++;
	}

	total_id = 1;
	for(int i=0; i<V.size() ; i++)
	{
		if(V[i].id > total_id)
			total_id = V[i].id;
	}

	total_res = 1;
	for(int i=0; i<V.size() ; i++)
	{
		if(V[i].res > total_res)
			total_res = V[i].res;
	}

	for(int i=0;i<V.size();i++)
	{
		cout<<V[i].op<<"  "<<V[i].id<<"  "<<V[i].res<<"\n";
	}
	//cout<<"total res are"<<total_res;

	for(int i=0;i<total_id;i++)
	{
		for(int j=0;j<total_id;j++)
			graph[i][j]=0;
	}
	
	for(int i=1; i<= total_res ; i++)
	{
		for(int j=0 ;j<V.size() -1 ; j++)
		{
			if(V[j].res == i)
			{
				for(int k=j+1 ; k< V.size() ; k++)
				{
					if(  V[k].res == i and !(V[j].op=="R" and V[k].op=="R"))
					{
						graph[V[j].id -1][V[k].id-1]=1;

					}
				}
			}
		}
	}

	for(int i=0;i<total_id;i++)
	{
		for(int j=0;j<total_id;j++)
			if(i==j)
				graph[i][j]=0;
	}

	for(int i=0;i<total_id;i++)
	{
		for(int j=0;j<total_id;j++)
				cout<<graph[i][j]<<"  ";
		cout<<"\n";
	}

	topological_sort(graph);
	
	return 0;
}