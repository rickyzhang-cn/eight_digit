#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 362880

struct node{
	int status[3][3];	//九宫格的状态
	int f,g,h;	//f(n)=g(n)+h(n);
	int row_0,col_0;	//0所在的行和列
	struct node *father;	//父亲节点
};

//table其实就是一个栈
struct table{
	struct node *p[MAXSIZE]; 
	long length;
};

void astar_search(int (*start)[3],int (*target)[3]);	//A*算法实现函数
void display(struct node *p);	//显示node的九宫格状态，3*3的格式
void table_free(struct table *p_open,struct table *p_close);	//释放open和close表中的内存
void open_to_close(struct table *p_close,struct table *p_open);	//将一个node从open表move到close表中
int check_is_found(struct node *p,int (*target)[3]);	//检查target状态是否找到
void open_table_sort(struct table *p_open);	//根据node中的f值对open进行逆序排序
void init_table(struct table *p_open,struct table *p_close,int (*start)[3]);	//根据start节点初始化open和close表
void mv_up(struct node *dst,struct node *src);	//node节点中的0上移
void mv_down(struct node *dst,struct node *src);	//node节点中的0下移
void mv_left(struct node *dst,struct node *src);	//node节点中的0左移
void mv_right(struct node *dst,struct node *src);	//node节点中的0右移
void cp_proc(struct node *dst,struct node *src);	//通过src节点生成dst节点，复制status状态
void add_to_open(struct node *p_node,struct table *p_open,struct table *p_close);	//将一个生成的节点移到open表中
int m(struct node *p,int target[][3]);	//计算node n的h(n)的值
void child_generator(struct node *p_node,struct table *p_open,struct table *p_close); //通过种子node生成新的node
int check_is_solvable(int start[][3],int target[][3]);	//检查start节点到target节点是否有解
int is_exist(struct node *p_node,struct table *p_open,struct table *p_close);	//新生成的node是否已经存在
int node_cmp(struct node *p,struct node *q);	//node比较子函数

int target[3][3];	//将target的状态作为全局变量

int main(int argc,char *argv[])
{
	int i;
	int is_solvable;
	int start[3][3];
	printf("please input the start status:\n");
	for(i=0;i<3;i++)
		scanf("%d,%d,%d",start[i],start[i]+1,start[i]+2);
	printf("please input the target status:\n");
	for(i=0;i<3;i++)
		scanf("%d,%d,%d",target[i],target[i]+1,target[i]+2);
	is_solvable=check_is_solvable(start,target);
	if(!is_solvable)
	{
		printf("this problem is unsolvable\n");
		return 1;
	}
	astar_search(start,target);
	return 0;
}

int check_is_solvable(int start[][3],int target[][3])
{
	int i,j;
	int count_start=0,count_target=0;
	int start_num[9],target_num[9];
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
		{
			start_num[i*3+j]=start[i][j];
			target_num[i*3+j]=target[i][j];
		}
	for(i=0;i<9;i++)
		for(j=0;j<i;j++)
		{
			if(start_num[j]<start_num[i] && start_num[j]!=0)
				count_start++;
			if(target_num[j]<target_num[i] && target_num[j]!=0)
				count_target++;
		}

	if((count_start+count_target)%2 == 0)
		return 1;
	else
		return 0;
}

void astar_search(int (*start)[3],int (*target)[3])
{
	int bingo=0;
	struct node *current;
	struct table open,close;
	init_table(&open,&close,start);

//	printf("can go here\n");
//	printf("%ld,%ld\n",open.length,close.length);
//	display(open.p[0]);
//	display(close.p[0]);
	
	while(open.length!=0)
	{
		open_table_sort(&open);
		open_to_close(&close,&open);
		current=close.p[close.length-1];
		bingo=check_is_found(current,target);
		if(bingo==1)
			break;
		child_generator(current,&open,&close);
	}
	printf("\n^_^the result starts here^_^\n");
	while(current->father!=NULL)
	{
		display(current);
		printf(" ^ \n");
		printf(" ^ \n");
		printf(" | \n");
		current=current->father;
	}
	display(current);
	table_free(&open,&close);
}

void display(struct node *p)
{
	int i;
	for(i=0;i<3;i++)
		printf("%d,%d,%d\n",p->status[i][0],p->status[i][1],p->status[i][2]);
}

void table_free(struct table *p_open,struct table *p_close)
{
	int open_len=p_open->length;
	int close_len=p_close->length;
	int i,j;
	for(i=0;i<open_len;i++)
		free(p_open->p[i]);
	for(j=0;j<close_len;j++)
		free(p_close->p[j]);
}

void open_to_close(struct table *p_close,struct table *p_open)
{
	int open_len=p_open->length;
	int close_len=p_close->length;
	p_close->p[close_len]=p_open->p[open_len-1];
	p_open->length--;
	p_close->length++;
}

int check_is_found(struct node *p,int (*target)[3])
{
	int i,j;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			if(p->status[i][j]!=target[i][j])
				return 0;
	return 1;
}

void open_table_sort(struct table *p_open)
{
	int i,j;
	int len=p_open->length;
	for(i=len;i>0;i--)
		for(j=0;j<i-1;j++)
		{
			struct node *temp;
			int p=p_open->p[j]->f;
			int q=p_open->p[j+1]->f;
			if(p<q)
			{
				temp=p_open->p[j];
				p_open->p[j]=p_open->p[j+1];
				p_open->p[j+1]=temp;
			}
		}
}

void init_table(struct table *p_open,struct table *p_close,int (*start)[3])
{
	int i,j;
	int row,col;
	struct node *start_node=(struct node *)malloc(sizeof(struct node));
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
		{
			start_node->status[i][j]=start[i][j];
			if(start[i][j]==0)
			{
				row=i;col=j;
			}
		}
	start_node->father=NULL;
	start_node->row_0=row;
	start_node->col_0=col;
	start_node->g=0;
	p_close->p[0]=start_node;
	p_close->length=1;
	p_open->length=0;
	child_generator(start_node,p_open,p_close);
}

void child_generator(struct node *p_node,struct table *p_open,struct table *p_close)
{
	int row=p_node->row_0,col=p_node->col_0;
	struct node *p,*q;
	switch(row)
	{
		case 0:
			p=(struct node *)malloc(sizeof(struct node));
			mv_down(p,p_node);
			add_to_open(p,p_open,p_close);
			break;
		case 1:
			p=(struct node *)malloc(sizeof(struct node));
			q=(struct node *)malloc(sizeof(struct node));
			mv_up(p,p_node);
			mv_down(q,p_node);
			add_to_open(p,p_open,p_close);
			add_to_open(q,p_open,p_close);
			break;
		case 2:
			p=(struct node *)malloc(sizeof(struct node));
			mv_up(p,p_node);
			add_to_open(p,p_open,p_close);
			break;
	}
	switch(col)
	{
		case 0:
			p=(struct node *)malloc(sizeof(struct node));
			mv_right(p,p_node);
			add_to_open(p,p_open,p_close);
			break;
		case 1:
			p=(struct node *)malloc(sizeof(struct node));
			q=(struct node *)malloc(sizeof(struct node));
			mv_left(p,p_node);
			mv_right(q,p_node);
			add_to_open(p,p_open,p_close);
			add_to_open(q,p_open,p_close);
			break;
		case 2:
			p=(struct node *)malloc(sizeof(struct node));
			mv_left(p,p_node);
			add_to_open(p,p_open,p_close);
			break;
	}
}

void mv_up(struct node *dst,struct node *src)
{
	int row=src->row_0,col=src->col_0;
	cp_proc(dst,src);
	dst->status[row][col]=dst->status[row-1][col];
	dst->status[row-1][col]=0;
	dst->row_0=row-1;
	dst->col_0=col;
}

void mv_down(struct node *dst,struct node *src)
{
	int row=src->row_0,col=src->col_0;
	cp_proc(dst,src);
	dst->status[row][col]=dst->status[row+1][col];
	dst->status[row+1][col]=0;
	dst->row_0=row+1;
	dst->col_0=col;
}

void mv_left(struct node *dst,struct node *src)
{
	int row=src->row_0,col=src->col_0;
	cp_proc(dst,src);
	dst->status[row][col]=dst->status[row][col-1];
	dst->status[row][col-1]=0;
	dst->row_0=row;
	dst->col_0=col-1;
}

void mv_right(struct node *dst,struct node *src)
{
	int row=src->row_0,col=src->col_0;
	cp_proc(dst,src);
	dst->status[row][col]=dst->status[row][col+1];
	dst->status[row][col+1]=0;
	dst->row_0=row;
	dst->col_0=col+1;
}

void cp_proc(struct node *dst,struct node *src)
{
	int i,j;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
		{
			dst->status[i][j]=src->status[i][j];
		}
	dst->g=src->g+1;
	dst->father=src;
}

void add_to_open(struct node *p_node,struct table *p_open,struct table *p_close)
{
	int len=p_open->length;
	int r=is_exist(p_node,p_open,p_close);
	if(r==1)
	{
		free(p_node);
//		printf("don't add to open\n");
		return;
	}
	p_node->h=m(p_node,target);
	p_node->f=p_node->g+p_node->h;
	p_open->p[len]=p_node;
	(p_open->length)++;
}

int is_exist(struct node *p_node,struct table *p_open,struct table *p_close)
{
	int i,j;
	int open_len=p_open->length,close_len=p_close->length;
	for(i=0;i<open_len;i++)
	{
		int r=node_cmp(p_node,p_open->p[i]);
		if(r==1)
			return 1;
	}
	for(j=0;j<close_len;j++)
	{
		int r=node_cmp(p_node,p_close->p[j]);
		if(r==1)
			return 1;
	}
	return 0;
}

int node_cmp(struct node *p,struct node *q)
{
	int i,j;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
		{
			if(p->status[i][j]!=q->status[i][j])
				return 0;
		}
	return 1;
}

int m(struct node *p,int target[][3])
{
	int i,j;
	int count=0;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
		{
			if(p->status[i][j]!=target[i][j])
				count++;
		}
	return count;
}

