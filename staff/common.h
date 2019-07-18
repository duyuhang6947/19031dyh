/**********************************
 *说明：员工管理系统共用头文件
 **********************************/

#ifndef __COMMON_H__
#define __COMMON_H__

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sqlite3.h>
#include<sys/wait.h>
#include<signal.h>
#include<time.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sqlite3.h>

#define STAFF_DATABASE 	 "staff_manage_system.db"

#define USER_LOGIN 		0x00000000  // login	登陆    0x00000001
#define USER_MODIFY 	0x00000001  // user-modification  修改
#define USER_QUERY 		0x00000002  // user-query   查询

#define ADMIN_LOGIN 	0x10000000  // login	登陆    0x00000001
#define ADMIN_MODIFY 	0x10000001 	// admin_modification
#define ADMIN_ADDUSER 	0x10000002 	// admin_adduser
#define ADMIN_DELUSER 	0x10000004 	// admin_deluser
#define ADMIN_QUERY 	0x10000008  //hitory_query
#define ADMIN_HISTORY 	0x10000010  //hitory_query

#define QUIT 			0x11111111


#define ADMIN 0	//管理员
#define USER  1	//用户

#define NAMELEN 16
#define DATALEN 128

//员工基本信息结构体
typedef struct staff_info{
	int    no;				//员工号
	int    usertype;		//用户类型；root 1   user 2
	char   name[NAMELEN];	//名字
	char   passwd[8];		//密码
	int    age;				//年龄
	char   phone[NAMELEN];	//电话
	char   addr[DATALEN]; 	// 地址
	char   work[DATALEN]; 	//职位
	char   date[DATALEN];	//入职年月
	int    level;			// 等级
	double salary ;			// 工资
}staff_info_t;

typedef struct{
	int  msgtype;			//请求的消息类型
	int  usertype;			//用户类型  root 1  user 2
	char username[NAMELEN]; //姓名
	char passwd[8];			//登陆密码
	char recvmsg[DATALEN];  //通信的消息
	int  flags;     		//标志位
	staff_info_t info;      //员工信息
}MSG;

/*定义线程处理的结构体信息*/
typedef struct thread_data{
	int acceptfd;
	pthread_t thread;
	int state; //线程状态
	MSG *msg; 
	void *prvi_data;
}thread_data_t;

//扩展:线程链表信息
typedef struct thread_node{
	thread_data_t data;
	struct thread_node *next;
}linklist, *plinklist;

//客户端函数声明
int do_login(int clientfd);
int admin_or_user_login(int clientfd,MSG *msg);
void user_menu(int clientfd, MSG *msg);
void do_user_query(int clientfd, MSG *msg);
void do_user_modification(int clientfd, MSG *msg);
void admin_menu(int clientfd, MSG *msg);
void do_admin_query(int clientfd, MSG *msg);
void do_admin_modification(int clientfd, MSG *msg);
void do_admin_adduser(int clientfd, MSG *msg);
void do_admin_deluser(int clientfd, MSG *msg);
void do_admin_history(int clientfd, MSG *msg);
void show_userinfo(MSG *msg);


//服务器函数声明
void get_system_time(char* timedata);
void history_init(MSG *msg,char *buf);
int process_client_request(int clientfd, MSG *msg);
int process_user_or_admin_login_request(int clientfd,MSG *msg);
int process_user_query_request(int clientfd,MSG *msg);
int process_user_modify_request(int clientfd, MSG *msg);
int process_admin_query_request(int clientfd, MSG *msg);
int process_admin_modify_request(int clientfd, MSG *msg);
int process_admin_adduser_request(int clientfd,MSG *msg);
int process_admin_deluser_request(int clientfd, MSG *msg);
int process_admin_history_request(int clientfd,MSG *msg);
int process_client_quit_request(int clientfd, MSG *msg);
int history_callback(void *arg, int ncolumn, char **f_value, char **f_name);



#endif		//__COMMON_H__
