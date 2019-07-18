/**********************************
*		客户端功能的实现
**********************************/

#include<stdio.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "common.h"


/**********************************
*		1一级登录选择界面
**********************************/
int do_login(int clientfd)
{
	int n;
	MSG msg;

	while(1){
		printf("*****************************************************\n");
		printf("************ 1:管理员  2:普通用户 3:退出 ************\n");
		printf("*****************************************************\n");
		printf("请输入您的选择>>");
		scanf("%d",&n);
		getchar();

		printf("1.1");

		switch(n){
		case 1:
			printf("case1");
			msg.msgtype  = ADMIN_LOGIN;
			msg.usertype = ADMIN;
			break;
		case 2:
			msg.msgtype  = USER_LOGIN;
			msg.usertype = USER;
			break;
		case 3:
			msg.msgtype = QUIT;
			if(send(clientfd, &msg, sizeof(MSG),0) < 0){
				perror("do_login send");
				return -1;
			}
			close(clientfd);
			exit(0);
			break;
		default:
			printf("输入有误，请重新输入\n");
		}

		printf("1.2");
		admin_or_user_login(clientfd, &msg);
	}

}

/**********************************
*		2二级登录操作界面
**********************************/
int admin_or_user_login(int clientfd,MSG *msg)
{
	//登录帐号和密码
	memset(msg->username, 0, NAMELEN);
	printf("请输入用户名: ");
	scanf("%s",msg->username);
	getchar();

	memset(msg->passwd, 0, DATALEN);
	printf("请输入密码：");
	scanf("%s", msg->passwd);
	getchar();

	printf("text2.1");

	//发送登录请求
	send(clientfd, msg, sizeof(MSG), 0);
	//接收服务器响应
	recv(clientfd, msg, sizeof(MSG), 0);
	printf("msg->recvmsg:%s\n",msg->recvmsg);

	//判断是否登录成功
	if(strncmp(msg->recvmsg, "OK", 2) == 0){
		if(msg->usertype == ADMIN){
			printf("尊敬的管理员，欢迎登录员工管理系统！\n");
			admin_menu(clientfd,msg);
		}else if(msg->usertype == USER){
			printf("亲爱的用户，欢迎登录员工管理系统！\n");
			user_menu(clientfd,msg);
		}
	}else{
		printf("登录失败！%s\n",msg->recvmsg);
		return -1;
	}
	return 0;
}

/**********************************
*		3普通用户操作目录
**********************************/
void user_menu(int clientfd, MSG *msg)
{
	int n;
	while(1){
		printf("*****************************************************\n");
		printf("*************  1:查询  2:修改  3:退出  **************\n");
		printf("*****************************************************\n");
		printf("请输入您的选择>>");
		scanf("%d",&n);
		getchar();

		switch(n){
		case 1:
			do_user_query(clientfd, msg);
			break;
		case 2:
			do_user_modification(clientfd,msg);
			break;
		case 3:
			msg->msgtype = QUIT;
			send(clientfd, msg, sizeof(MSG), 0);
			close(clientfd);
			exit(0);
		default:
			printf("输入有误，请重新输入\n");
			break;
		}
	}
}

/**********************************
*		3.1普通用户查询个人信息
**********************************/
void do_user_query(int clientfd, MSG *msg)
{
	//封装发送消息--等待服务器响应
	msg->msgtype = USER_QUERY;

	send(clientfd, msg, sizeof(MSG), 0);
	recv(clientfd, msg, sizeof(MSG), 0);
	printf("工号\t用户类型\t 姓名\t密码\t年龄\t电话\t地址\t职位\t入职年月\t等级\t 工资\n");
	printf("%s.\n",msg->recvmsg);
}

/**********************************
*		3.2普通用户修改个人信息	  *
**********************************/
void do_user_modification(int clientfd, MSG *msg)
{
	int n = 0;
	msg->msgtype = USER_MODIFY;
	
	printf("请输入您的工号：");
	scanf("%d", &msg->info.no);
	getchar();

	printf("*****************************************************\n");
	printf("******请输入您要修改的选项(其他信息请联系管理员)*****\n");
	printf("**********1:家庭住址 2:电话 3:密码 4:退出 ***********\n");
	printf("*****************************************************\n");
	printf("请输入您的选择>>");
	scanf("%d", &n);
	getchar();
	
	switch(n){
		case 1:
			printf("请输入新的家庭住址：");
			msg->recvmsg[0] = 'D';
			scanf("%s", msg->info.addr);
			getchar();
			break;
		case 2:
			printf("请输入新的电话：");
			msg->recvmsg[0] = 'P';
			scanf("%s",msg->info.phone);
			getchar();
			break;
		case 3:
			printf("请输入新的密码:");
			msg->recvmsg[0] = 'M';
			scanf("%6s",msg->info.passwd);
			getchar();
			break;
		case 4:
			return;
	}
	send(clientfd, msg, sizeof(MSG), 0);
	recv(clientfd, msg, sizeof(MSG), 0);
	printf("%s",msg->recvmsg);

	printf("修改结束\n");
}

/**********************************
*		4管理员操作目录			  *
**********************************/
void admin_menu(int clientfd, MSG *msg)
{
	int n;
	while(1){		
		printf("*****************************************************\n");
		printf("*1:查询 2:修改 3:添加用户 4:删除用户 5:查询历史记录 *\n");
		printf("**                6:退出                           **\n");
		printf("*****************************************************\n");
		printf("请输入您的选择>>");
		scanf("%d", &n);
		getchar();
	
		switch(n){
			case 1:
				do_admin_query(clientfd,msg);
				break;
			case 2:
				do_admin_modification(clientfd,msg);
				break;
			case 3:
				do_admin_adduser(clientfd,msg);
				break;
			case 4:
				do_admin_deluser(clientfd,msg);
				break;
			case 5:
				do_admin_history(clientfd,msg);
				break;
			case 6:
				msg->msgtype = QUIT;
				send(clientfd, msg, sizeof(MSG), 0);
				close(clientfd);
				exit(0);
			default:
				printf("输入有误，请重新输入\n");
		}
	}
}


/**********************************
*		4.1管理员查询数据  		  *
**********************************/
void do_admin_query(int clientfd, MSG *msg)
{
	//选择查询方式--填充封装命令请求---发送数据---等待服务器响应 
	int n;
	msg->msgtype = ADMIN_QUERY;

	while(1){
		memset(&msg->info, 0, sizeof(staff_info_t));
		if(msg->usertype == ADMIN){			
			printf("*****************************************************\n");
			printf("**********1:按人名查找 2:查找所有 3:退出 ************\n");
			printf("*****************************************************\n");
			printf("请输入您的选择>>");
			scanf("%d", &n);
			getchar();

			switch(n){
				case 1:
					msg->flags = 1;		//按人名查找
					break;
				case 2:
					msg->flags = 0;		//默认查找所有员工信息
					break;
				case 3:
					return;
			}	
		}
		
		if(msg->flags == 1){
			printf("请输入您要查找的用户名：");
			scanf("%s", msg->info.name);
			getchar();

			send(clientfd, msg, sizeof(MSG), 0);
			recv(clientfd, msg, sizeof(MSG), 0);
			printf("工号\t用户类型\t 姓名\t密码\t年龄\t电话\t地址\t职位\t入职年月\t等级\t 工资\n");
			printf("%s.\n",msg->recvmsg);
		}else{
			send(clientfd, msg, sizeof(MSG), 0);
			printf("工号\t用户类型\t 姓名\t密码\t年龄\t电话\t地址\t职位\t入职年月\t等级\t 工资\n");
			while(1){
				//循环接收服务器发送的数据
				recv(clientfd, msg, sizeof(MSG), 0);
				if(strncmp(msg->recvmsg, "over*", 5) == 0)
					break;
				printf("%s.\n",msg->recvmsg);
			}	
		}
	}
	printf("查找结束\n");
}

/**********************************
*		4.2管理员修改数据		  *
**********************************/
void do_admin_modification(int clientfd, MSG *msg)
{
	int n = 0;
	msg->msgtype = ADMIN_MODIFY;
	memset(&msg->info, 0, sizeof(staff_info_t));

	send(clientfd, msg, sizeof(MSG), 0);
	recv(clientfd, msg, sizeof(MSG), 0);

	printf("请输入要修改员工的工号：");
	scanf("%d",&msg->info.no);
	getchar();

	printf("*****************************************************\n");
	printf("******************请输入要修改的选项*****************\n");
	printf("*********** 1:姓名 2:年龄 3:家庭住址 4:电话 *********\n");
	printf("*********** 5:职位 6:工龄 7:入职日期 8:评级 *********\n");
	printf("*********** 9:密码 10:退出                  *********\n");
	printf("*****************************************************\n");
	printf("请输入您的选择>>");
	scanf("%d", &n);
	getchar();

	switch(n){
		case 1:
			printf("请输入用户名：");
			msg->recvmsg[0] = 'N';
			scanf("%s",msg->info.name);getchar();
			break;
		case 2:
			printf("请输入年龄：");
			msg->recvmsg[0] = 'A';
			scanf("%d",&msg->info.age);getchar();
			break;
		case 3:
			printf("请输入家庭住址：");
			msg->recvmsg[0] = 'D';
			scanf("%s",msg->info.addr);getchar();
			break;
		case 4:
			printf("请输入电话：");
			msg->recvmsg[0] = 'P';
			scanf("%s",msg->info.phone);getchar();
			break;
		case 5:
			printf("请输入职位：");
			msg->recvmsg[0] = 'W';
			scanf("%s",msg->info.work);getchar();
			break;
		case 6:
			printf("请输入工资：");
			msg->recvmsg[0] = 'S';
			scanf("%lf",&msg->info.salary);getchar();
			break;
		case 7:
			printf("请输入入职日期(格式：0000.00.00.)：");
			msg->recvmsg[0] = 'T';
			scanf("%s",msg->info.date);getchar();
			break;
		case 8:
			printf("请输入评级(1~5,5为最高)：");
			msg->recvmsg[0] = 'L';
			scanf("%d",&msg->info.level);getchar();
			break;
		case 9:
			printf("请输入新密码：(6位数字)");
			msg->recvmsg[0] = 'M';
			scanf("%6s",msg->info.passwd);getchar();
			break;
		case 10:
			return ;
	}

	send(clientfd, msg, sizeof(MSG), 0);
	recv(clientfd, msg, sizeof(MSG), 0);
	printf("%s",msg->recvmsg);

	printf("修改结束.\n");

}


/**********************************
*		4.3管理员添加用户		  *
**********************************/
void do_admin_adduser(int clientfd, MSG *msg)
{
	//输入用户信息--填充封装命令请求---发送数据---等待服务器响应---
	//成功之后自动查询当前添加的用户
	char temp;
	msg->msgtype  = ADMIN_ADDUSER;
	msg->usertype = ADMIN;
	memset(&msg->info,0,sizeof(staff_info_t));

	while(1){
		printf("******************* 热烈欢迎新员工 ******************\n");		
		printf("请输入工号:");	
		scanf("%d",&msg->info.no);
		getchar();
		printf("您输入的工号是：%d\n",msg->info.no);
		printf("工号信息一旦录入无法更改，请确认您所输入的是否正确！(Y/N)");
		scanf("%c",&temp);
		getchar();
		if(temp == 'N' || temp == 'n'){
			printf("请重新添加用户：");
			break;
		}

		printf("请输入用户名：");
		scanf("%s",msg->info.name);
		getchar();

		printf("请输入用户密码：");
		scanf("%6s",msg->info.passwd);
		getchar();

		printf("请输入年龄：");
		scanf("%d",&msg->info.age);
		getchar();

		printf("请输入电话：");
		scanf("%s",msg->info.phone);
		getchar();

		printf("请输入家庭住址：");
		scanf("%s",msg->info.addr);
		getchar();

		printf("请输入职位：");
		scanf("%s",msg->info.work);
		getchar();

		printf("请收入入职日期(格式：0000.00.00)：");
		scanf("%s",msg->info.date);
		getchar();

		printf("请输入评级(1~5,5为最高，新员工为 1)：");
		scanf("%d",&msg->info.level);
		getchar();

		printf("请输入工资：");
		scanf("%lf",&msg->info.salary);
		getchar();

		printf("是否为管理员：(Y/N)");
		scanf("%c",&temp);
		getchar();
		if(temp == 'Y' || temp == 'y')
			msg->info.usertype = ADMIN;
		else if(temp == 'N' || temp == 'n')
			msg->info.usertype = USER;
		printf("msg->info.usertype:%d\n",msg->info.usertype);

		//将用户数据发送到服务器
		send(clientfd, msg, sizeof(MSG), 0);
		//等待接收服务器响应
		recv(clientfd, msg, sizeof(MSG), 0);

		if(strncmp(msg->recvmsg,"OK",2) == 0)
			printf("添加成功！\n");
		else
			printf("%s",msg->recvmsg);

		printf("是否继续添加员工:(Y/N)");
		scanf("%c",&temp);
		getchar();
		if(temp == 'N' || temp == 'n')
			break;	
	}
}

/**********************************
*		4.4管理员删除用户		  *
**********************************/
void do_admin_deluser(int clientfd, MSG *msg)
{
	msg->msgtype = ADMIN_DELUSER;

	//根据工号唯一来删除用户
	printf("请输入要删除用户的工号：");
	scanf("%d",&msg->info.no);
	getchar();
//	printf("请输入要删除用户的用户名：");
//	scanf("%d",&msg->info.name);
//	getchar();
	
	send(clientfd, msg, sizeof(MSG), 0);
	recv(clientfd, msg, sizeof(MSG), 0);
	
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
		printf("删除成功\n");
	else
		printf("%s", msg->recvmsg);

	printf("工号为 %d 的用户已被删除\n",msg->info.no);
}

/**********************************
*		4.5管理员查询历史记录	  *
**********************************/
void do_admin_history(int clientfd, MSG *msg)
{
//封装命令请求---发送数据---等待服务器响应----打印输出结果		
	msg->msgtype = ADMIN_HISTORY;
	send(clientfd, msg, sizeof(MSG), 0);

	while(1){
		recv(clientfd, msg, sizeof(MSG), 0);
		if(strncmp(msg->recvmsg, "over*", 5) == 0)
			break;
		printf("msg->recvmsg:%s",msg->recvmsg);
	}
	printf("管理员admin查询历史记录结束\n");
}


int main(int argc, const char *argv[])
{
	//tcp客户端的创建
	int serverfd,clientfd,len,ret;
	struct sockaddr_in serveraddr,clientaddr;
	socklen_t ser_len = sizeof(serveraddr);
	socklen_t cli_len = sizeof(clientaddr);

	//创建网络套接字
	clientfd = socket(AF_INET,SOCK_STREAM,0);
	if(clientfd < 0){
		printf("failed to socket\n");
		return -1;
	}

	//填充网络结构体
	memset(&serveraddr,0,sizeof(serveraddr));
	memset(&clientaddr,0,sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;   				 /* 网络类型*/
	clientaddr.sin_addr.s_addr = inet_addr(argv[1]);  /* 终端输入IP*/
	clientaddr.sin_port = htons(atoi(argv[2])); 		 /* 终端输入端口号，字节序转换*/

	bind(clientfd, (struct sockaddr *)&clientaddr,cli_len);

	//连接服务器
	if(connect(clientfd,(const struct sockaddr *)&serveraddr,ser_len) == -1){
		perror("connect failed\n");
		exit(-1);
	}

	do_login(clientfd);

	close(clientfd);

	return 0;
}
