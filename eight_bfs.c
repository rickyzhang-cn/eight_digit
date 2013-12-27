#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 362880

struct node{
	int status[3][3];
	int row_0,col_0;
	struct node *father;
};

struct table{
	struct node *p[MAXSIZE];
	long front,rear;
};

void bfs_search(int (*start)[3],int (*target)[3]);
void display(struct node *p);
void table_free(struct table *p_open,struct table *p_close);
void open_to_close(struct table *p_close,struct table *p_open);
int check_is_found(struct node *p,int (*target)[3]);
void init_table(struct table *p_open,struct table *p_close,int (*start)[3]);
void mv_up(struct node *dst,struct node *src);
void mv_down(struct node *dst,struct node *src);
void mv_left(struct node *dst,struct node *src);
void mv_right(struct node *dst,struct node *src);
void cp_proc(struct node *dst,struct node *src);
void add_to_open(struct node *p_node,struct table *p_open,struct table *p_close);
void child_generator(struct node *p_node,struct table *p_open,struct table *p_close);
int check_is_solvable(int start[][3],int target[][3]);
int is_exist(struct node *p_node,struct table *p_open,struct table *p_close);
int node_cmp(struct node *p,struct node *q);

int target[3][3];

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
	bfs_search(start,target);
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

void bfs_search(int (*start)[3],int (*target)[3])
{
	int bingo=0;
	struct node *current;
	struct table open,close;
	init_table(&open,&close,start);

	while((open.rear-open.front)!=0)
	{
		int open_f=open.front;
		current=open.p[open_f];
		bingo=check_is_found(current,target);
		if(bingo==1)
			break;
		open_to_close(&close,&open);
		child_generator(current,&open,&close);
	}

	printf("\n^_^the result starts here^_^\n");
	while(current->father!=NULL)
	{
		display(current);
		printf("\n");
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
	int open_f=p_open->front,open_r=p_open->rear;
	int close_r=p_close->rear;
	int i,j;
	if(open_r>=open_f)
	{
		for(i=open_f;i<open_r;i++)
			free(p_open->p[i]);
	}
	else
	{
		for(i=0;i<open_r;i++)
			free(p_open->p[i]);
		for(i=open_f;i<MAXSIZE;i++)
			free(p_open->p[i]);
	}
	for(j=0;j<close_r;j++)
		free(p_close->p[j]);
}

void open_to_close(struct table *p_close,struct table *p_open)
{
	int open_f=p_open->front;
	int close_r=p_close->rear;
	p_close->p[close_r]=p_open->p[open_f];
	p_open->front=(open_f+1)%MAXSIZE;
	p_close->rear++;
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
	p_close->p[0]=start_node;
	p_close->rear=1;
	p_open->front=p_open->rear=0;
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
	dst->father=src;
}

void add_to_open(struct node *p_node,struct table *p_open,struct table *p_close)
{
	int open_r=p_open->rear;
	int r=is_exist(p_node,p_open,p_close);
	if(r==1)
	{
		free(p_node);
		return;
	}
	p_open->p[open_r]=p_node;
	(p_open->rear)=(open_r+1)%MAXSIZE;
}

int is_exist(struct node *p_node,struct table *p_open,struct table *p_close)
{
	int i,j;
	int open_f=p_open->front,open_r=p_open->rear;
	int close_r=p_close->rear;
	for(i=open_f;i<open_r;i++)
	{
		int r=node_cmp(p_node,p_open->p[i]);
		if(r==1)
			return 1;
	}
	for(j=0;j<close_r;j++)
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
