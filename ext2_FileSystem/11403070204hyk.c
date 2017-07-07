#include<stdio.h>
#include<string.h>
#define N 200
#define M 4
#define NAME 20

struct User
{
    char account[NAME];
    int password;
    struct User *next;
};
struct info
{
	char name[NAME];
	int toInode;
	struct info *next;
};
struct Block
{
    int blockIsUsed;
	struct info *p;
};
struct Inode
{
    int inodeIsUsed;
	int model;		//1代表文件		0代表目录
	int size;		//文件大小
	int limit;		//权限
	int block[N];		//所占block的序号
	int nb;				// 一共占用几个block
};
struct Block m_block[N];
struct Inode m_inode[N];
struct User *users;
char account[NAME];
char access[NAME]="/";
int nowBlock=0;
void init()
{
    int i;
    char s;
	struct User *u,*t;
	FILE *fp;
	users=(struct User *)malloc(sizeof(struct User));
	users->next=NULL;
	t=users;
    for(i=0;i<N;i++)
    {
        m_block[i].blockIsUsed=0;
        m_block[i].p=NULL;
        m_inode[i].inodeIsUsed=0;
        m_inode[i].model=0;
        m_inode[i].nb=0;
    }
    m_inode[0].inodeIsUsed=1;
    m_block[0].blockIsUsed=1;
    m_inode[0].nb=1;
    m_inode[0].block[0]=0;
    nowBlock=0;

	if((fp=fopen("user.txt","r"))==NULL)
	{
		printf("open user file failed!!!\n");
		exit(0);
	}
	while(!feof(fp))
	{
		u=(struct User *)malloc(sizeof(struct User));
		fscanf(fp,"%s %d ",u->account,&u->password);
		u->next=NULL;
		t->next=u;
		t=u;
	}
	fclose(fp);
	//printAccount();
}
int findEmptyBlock()
{
    int i=0;
    for(i=0;i<N;i++)
    {
        if(m_block[i].blockIsUsed==0)
        {
           m_block[i].blockIsUsed=1;
            return i;
        }
    }
    return -1;
}
int findEmptyInode()
{
   int i=0;
    for(i=0;i<N;i++)
    {
        if(m_inode[i].inodeIsUsed==0)
        {
           m_inode[i].inodeIsUsed=1;
            return i;
        }
    }
    return -1;
}
void login()
{
	char acc[NAME];
	struct User *p;
	int pas=-1,pass;
	 fflush(stdin);
	 initOrder();
	while(1)
    {
        printf("login:");
        gets(acc);
        printf("password:");
        p=users->next;
        while(p)
        {
            if(strcmp(acc,p->account)==0)
            {
                pas=p->password;
                break;
            }
            p=p->next;
        }
        scanf("%d",&pass);
        fflush(stdin);
        if(pass==pas&&pas!=-1)
        {
            strcpy(account,acc);
            printf("Login success\n\n");
            return;
        }else{
            printf("Login incorrect\n\n");
        }
    }

}
void printAccount()
{
	struct User *p;
	p=users->next;
	while(p)
	{
		printf("%s\t%d\n",p->account,p->password);
		p=p->next;
	}
}
void del(int i)
{
    int j;
    struct info *m,*n;
    m_inode[i].inodeIsUsed=0;
    m_inode[i].size=0;
    for(j=0;j<m_inode[i].nb;j++)
    {
        m_block[m_inode[i].block[j]].blockIsUsed=0;

        if(m_block[m_inode[i].block[j]].p!=NULL&&m_block[m_inode[i].block[j]].p->next!=NULL)
        {
			n=m_block[m_inode[i].block[j]].p->next;
            while(n)
			{
				m=n;
				n=n->next;
				del(m->toInode);
				free(m);
			}
			free(m_block[m_inode[i].block[j]].p);
        }
    }
    m_inode[i].nb=0;
}
//创建文件
void createFile(char myName[])
{
    int size,emptyInode,i;
    struct info *n=(struct info *)malloc(sizeof(struct info));
    struct info *m;

    //printf("请输入文件大小!!!");
    //scanf("%d",&size);
    size=2;//默认文件大小为2个block

    emptyInode=findEmptyInode(m_inode);
    //printf("\n\n%d\n\n\n%d",emptyInode,nowBlock);
    m_inode[emptyInode].model=0;
    m_inode[emptyInode].size=size;
    m_inode[emptyInode].nb=2;
    for(i=0;i<m_inode[emptyInode].nb;i++)
    {
        m_inode[emptyInode].block[i]=findEmptyBlock(m_block);
    }

     m=m_block[nowBlock].p;
    if(m==NULL)
    {
        m_block[nowBlock].p=(struct info *)malloc(sizeof(struct info));
        m_block[nowBlock].p->next=NULL;
        m=m_block[nowBlock].p;
    }
    else
    {
        while(m->next)
            m=m->next;
    }
	strcpy(n->name,myName);
	n->toInode=emptyInode;
	n->next=NULL;
	m->next=n;


}


//创建目录
void createCatalog(char myName[])
{
    int emptyInode,emptyBlock;
    struct info *n=(struct info *)malloc(sizeof(struct info));
    struct info *m;
 //                                                                               ？此处需要加入检测文件名字是否重复
	//找空闲区域
    emptyInode=findEmptyInode();
    emptyBlock=findEmptyBlock();
    //printf("\n\n%d\n\n\n%d",emptyInode,nowBlock);
    m_inode[emptyInode].model=1;
    m_inode[emptyInode].nb=1;
    m_inode[emptyInode].block[0]=emptyBlock;

    m=m_block[nowBlock].p;
    if(m==NULL)
    {
        m_block[nowBlock].p=(struct info *)malloc(sizeof(struct info));
        m_block[nowBlock].p->next=NULL;
        m=m_block[nowBlock].p;
    }
    else
    {
        while(m->next)
            m=m->next;
    }
	strcpy(n->name,myName);
	n->toInode=emptyInode;
	n->next=NULL;
	m->next=n;
}
void printCatalog(int i)
{
    struct info *m;
    if(m_block[i].p!=NULL)
    {
        m=m_block[i].p->next;
        while(m)
        {
            if(m_inode[m->toInode].model==1)
                printf("%s`\t",m->name);
            else
                printf("%s\t",m->name);
            m=m->next;
        }
        printf("\n");
    }
    else
    {
        printf("\n");
    }
}
void printFile(int i)
{
    printf("该文件大小:%d\t占用%d个block区域\t\n",m_inode[i].size,m_inode[i].nb);
}
char *getOrder(char all[])
{
    int i,j=0;
    char order[NAME];
    char contents[NAME];
    if(all[0]=='\0')
        return "";
    for(i=0;;i++)
    {
        if(all[i]=='\0'||all[i]==' ')
            break;
        order[i]=all[i];
    }
    order[i]='\0';
    if(all[i]=='\0')
    {
        all[0]='\0';
    }
    else
    {
        i++;
        for(i;all[i]!='\0';i++)
        {
            contents[j]=all[i];
            j++;
        }
        contents[j]='\0';
        strcpy(all,contents);
    }
    return order;
}
int find(char n[],int *tempBlock,char tempAccess[])
{
    struct info *m;
    m=m_block[*tempBlock].p;
    if(m!=NULL)
    {
        m=m->next;
        while(m)
        {
            if(strcmp(n,m->name)==0)
            {
                if(m_inode[m->toInode].model==1)
                {
                    *tempBlock=m_inode[m->toInode].block[0];
                    strcat(tempAccess,n);
                    strcat(tempAccess,"/");
                    return 1;
                }
                else
                {
                    printf("cannot open %s is not the catalog\n",n);
                    return 0;
                }
            }
            m=m->next;
        }
        if(m==NULL)
        {
            printf("no %s catalog\n",n);
            return 0;
        }
    }
    else
    {
        printf("no %s catalog\n",n);
            return 0;
    }
}

void dealAccess(char contents[],int *u)
{
    int i,j=0,h=0;
    char temp[NAME];
    int tempBlock;
    char tempAccess[NAME];
    if(contents[0]=='/')        //绝对路径
    {
        tempBlock=0;
        strcpy(tempAccess,"/");
        if(contents[1]=='/')
        {
            strcpy(access,tempAccess);
            nowBlock=tempBlock;
            return;
        }
        for(j=1;contents[j]!='\0';j++)
        {
            if(contents[j]!='/')
            {
                temp[h++]=contents[j];
            }
            else
            {
                temp[h]='\0';
                if(find(temp,&tempBlock,tempAccess))
                {
                    h=0;
                }
                else
                {
                    temp[0]='\0';
                    return;
                }
            }
        }
        if(contents[j]=='\0')
        {
            temp[0]='\0';
            if(*u==-1)
            {
                strcpy(access,tempAccess);
                nowBlock=tempBlock;
            }
            else
            {
                *u=tempBlock;
            }
            return;
        }
    }
    else
    {
        j=0;
        tempBlock=nowBlock;
        strcpy(tempAccess,access);
        for(i=0;contents[i]!='\0';i++)
        {
            if(contents[i]!='/')
            {
                temp[j]=contents[i];
                j++;
            }
            else
            {
                temp[j]='\0';
                if(strcmp(temp,"..")==0)
                {
                    j=0;
                    printf("..");
                }
                else if(strcmp(temp,".")==0)
                {
                    j=0;
                }
                else if(strcmp(temp,"")==0)
                    j=0;
                else
                {
                    if(find(temp,&tempBlock,tempAccess))
                    {
                        j=0;
                    }
                }
            }
        }
        if(*u==-1)
        {
            strcpy(access,tempAccess);
            nowBlock=tempBlock;
        }
        else
        {
            *u=tempBlock;
        }
    }
}
void cdOrder(char contents[])
{
    int i,j=0,h=0,u=-1;
    char temp[NAME];
    int tempBlock;
    char tempAccess[NAME];
    for(i=0;contents[i]!='\0';i++)
    {
        if(contents[i]==' ')
        {
            printf("%s :no this access\n",contents);
            return;
        }
    }
    dealAccess(contents,&u);
}
void lsOrder(char contents[])
{
    if(contents[0]=='\0')
    {
        printCatalog(nowBlock);
    }
    else
    {
        printf("格式错误!\n");
    }
}
void createOrder(char contents[],int flag)
{
    int i,j=0;
    char temp[NAME];
    for(i=0;contents[i]!='\0';i++)
    {
        if(contents[i]==' ')
        {
            temp[j]='\0';
            //检查文件名字是否合法
           if(isFind(temp,nowBlock))
           {
               printf("%s is exist\n",temp);
               return;
           }

            if(flag==0)
                createFile(temp);
            else
                createCatalog(temp);
            j=0;
        }
        else
        {
            temp[j]=contents[i];
            j++;
        }
    }
}
int isFind(char fileName[],int tempBlock)
{
   struct info *m,*n;
   if(m_block[tempBlock].p!=NULL)
   {
       m=m_block[tempBlock].p->next;
       while(m)
       {
           if(strcmp(fileName,m->name)==0)
           {
               return 1;
           }
           m=m->next;
       }
   }

   return 0;
}
void rmOrder(char contents[])
{
   int i,j=0;
   char str[NAME];
   struct info *w,*e;
   for(i=0;contents[i]!='\0';i++)
   {
       if(contents[i]==' ')
       {
           str[j]='\0';
           j=0;
           if(isFind(str,nowBlock))
           {

               if(m_block[nowBlock].p!=NULL)
               {
                    w=m_block[nowBlock].p;
                     while(w->next)
                    {
                        if(strcmp(str,w->next->name)==0)
                        {
                            e=w->next;
                            w->next=e->next;
                            del(e->toInode);
                            free(e);
                            break;
                        }
                        w=w->next;
                    }
               }

           }
           else
           {
               printf("no find %s file!\n",str);
           }
       }
       else
       {
           str[j]=contents[i];
           j++;
       }
   }


}
void saveUser()
{
    FILE *fp;
    struct User *p;
    if((fp=fopen("user.txt","w"))==NULL)
    {
        printf("user file cannot open\n");
        return;
    }
    p=users->next;
    while(p)
    {
        fprintf(fp,"%s %d\n",p->account,p->password);
        p=p->next;
    }
    fclose(fp);
}
int isFindUser(char na[])
{
    struct User *m,*n;
    m=users->next;
    while(m)
    {
        if(strcmp(m->account,na)==0)
            return 1;
        m=m->next;
    }
    return 0;
}
void userOrder(char contents[],int flag)
{
    int i,j=0,pas;
    char na[NAME];
    struct User *p,*m,*n;
    for(i=0;contents[i]!='\0';i++)
    {
        if(contents[i]==' ')
        {
            printf("命令格式错误!\n");
            return;
        }
        else
        {
            na[j]=contents[i];
            j++;
        }

    }
    na[j]='\0';

    if(flag==0)     //添加用户
    {
        if(isFindUser(na))
        {
            printf("%s is exist\n",na);
            return;
        }

        m=(struct User *)malloc(sizeof(struct User));
        strcpy(m->account,na);
        m->next=NULL;
        printf("password:");
        scanf("%d",&pas);
        m->password=pas;
        m->next=users->next;
        users->next=m;
//        if(users->next!=NULL)
//        {
//            p=users->next;
//            while(p->next)
//            {
//                p=p->next;
//            }
//            p->next=m;
//        }
//        else
//        {
//            users->next=m;
//        }
    }
    else
    {
        p=users;
        if(strcmp(na,"kk")==0)
        {
            printf("kk user cannot del\n");
            return;
        }
        if(strcmp(account,"kk")==0)
        {
            while(p->next)
            {
                if(strcmp(p->next->account,na)==0)
                    break;
                p=p->next;
            }
            if(p->next==NULL)
            {
                printf("no this user..\n");
                return;
            }
            n=p->next;
            p->next=p->next->next;
            free(n);
        }
        else
        {
            printf("权限不够!!!\n");
        }

    }
    printAccount();
    saveUser();
}

void save()
{
    FILE *fp;
    fp=fopen("account.txt","w");
    if(fp==NULL)
    {
        printf("cannot open %s file\n",account);
        return;
    }
}
void initOrder()
{
    int i;
    struct info *m,*n;
    if(m_block[0].p!=NULL)
    {
        m=m_block[0].p->next;
        while(m)
        {
            del(m->toInode);
            m=m->next;
        }
        free(m_block[0].p);
        m_block[0].p=NULL;
//        while(m_block[0].p)
//        {
//            n=m_block[0].p;
//            while(n->next)
//            {
//                n=n->next;
//            }
//            free(n);
//        }
    }
    nowBlock=0;
    strcpy(access,"/");
}
void main()
{
    char order[NAME],all[NAME];
	init();
	//printAccount(users);
	login();
	while(1)
    {
        fflush(stdin);
        printf("[%s@myShell %s]#",account,access);
        gets(all);
        strcpy(order,getOrder(all));
        if(strcmp(order,"cd")==0)
        {
            if(all[0]!='\0')
            {
                strcat(all,"/");
                cdOrder(all);
            }
        }
        else if(strcmp(order,"ls")==0)
        {
            lsOrder(all);
        }
        else if(strcmp(order,"touch")==0)
        {
            if(all[0]!='\0')
            {
                strcat(all," ");
                createOrder(all,0);
            }
        }
        else if(strcmp(order,"rm")==0)
        {
            if(all[0]!='\0')
            {
                strcat(all," ");
                rmOrder(all);
            }
        }
        else if(strcmp(order,"mkdir")==0)
        {
            if(all[0]!='\0')
            {
                strcat(all," ");
                createOrder(all,1);
            }
        }
        else if(strcmp(order,"logout")==0)
        {
            login();
        }
        else if(strcmp(order,"exit")==0)
        {
            save();
            exit(1);
        }
        else if(strcmp(order,"useradd")==0)
        {
            userOrder(all,0);
        }
        else if(strcmp(order,"userdel")==0)
        {
            userOrder(all,1);
        }
        else if(strcmp(order,"init")==0)
        {
            initOrder();
        }
        else if(strcmp(order,"clear")==0)
        {
            system("cls");
        }
        else if(strcmp(order,"users")==0)
        {
            printAccount();
        }
    }
}
