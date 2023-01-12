/* 
*  File: server.c
*
*  Description: Server side of Hotel Management System handling Viewing Rooms, Booking and Canceling rooms 
*
*/


#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include "common.h"
#include "server.h"

/*To count the number of clients connected to the server*/
int cl_count = 0;

/*Message action structure*/
msg_action_t  msg_actions[MSG_MAX];

/*User information structure*/
user_info_t  *user_list_start = NULL;


single_t r1[4];
double_t r2[4];
deluxe_t r3[4];
suite_t r4[4];
services_t s[2];

/*---------- Room Structure Initializtion -----------*/

single_t* initialize_single_func()
{
   
	r1[0]->room_no = 101;
   	r1[0]->available = 0;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");
   
   	r1[1]->room_no = 102;
   	r1[1]->available = 0;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");
   
   	r1[2]->room_no = 103;
   	r1[2]->available = 0;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");

   	r1[3]->room_no = 104;
   	r1[3]->available = 0;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");

   	return r1;
}

double_t* initialize_double_func()
{
   
   	r2[0]->room_no = 201;
   	r2[0]->available = 0;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");

   	r2[1]->room_no = 202;
   	r2[1]->available = 0;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");
   
   	r2[2]->room_no = 203;
   	r2[2]->available = 0;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");
   
   	r2[3]->room_no = 204;
   	r2[3]->available = 0;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");
   
   	return r2;
}

deluxe_t* initialize_deluxe_func()
{
   
   	r3[0]->room_no = 201;
  	r3[0]->available = 1;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");
   
   	r3[1]->room_no = 202;
   	r3[1]->available = 1;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");
   
   	r3[2]->room_no = 203;
   	r3[2]->available = 1;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");
   
   	r3[3]->room_no = 204;
   	r3[3]->available = 1;
   	strcpy(r1[0]->check_in,"00/00/00");
  	strcpy(r1[0]->check_out,"00/00/00");
   
   	return r3;
}

suite_t* initialize_suite_func()
{
   
   	r4[0]->room_no = 201;
   	r4[0]->available = 1;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");
   
   	r4[1]->room_no = 202;
   	r4[1]->available = 1;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");
   
   	r4[2]->room_no = 203;
   	r4[2]->available = 1;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");
   
   	r4[3]->room_no = 204;
   	r4[3]->available = 1;
   	strcpy(r1[0]->check_in,"00/00/00");
   	strcpy(r1[0]->check_out,"00/00/00");
   
   	return r4;
}


/*----------- Adding New User to Linked list-----*/
user_info_t* add_user_to_list(char *uname, char *pwd)
{
  	user_info_t *new_user = malloc(sizeof(user_info_t));
  	user_info_t *temp = user_list_start;

  	if (new_user == NULL)
  	{
    		printf("Failed to alloc memory \n");
    		return NULL;
  	}

  	strncpy(new_user->uname, uname, strlen(uname));
  	strncpy(new_user->pwd, pwd, strlen(pwd));
  	new_user->next = NULL;

  	if (user_list_start == NULL)
  	{
    		user_list_start = new_user;
    		printf("Added first user \n");
    		return new_user;
  	}

  	while(temp->next != NULL)
    	temp = temp->next;

  	temp->next = new_user;
  	printf("Added new user \n");
    
  	return user_list_start;
}

/*----------- Validating Existing User  ----------*/
int search_user_pwd_in_list(char *uname, char *pwd)
{
  	user_info_t *temp = user_list_start;

  	if (temp == NULL)
 	{
    		printf("List is empty \n");
    		return USER_NOT_FOUND;
  	}
 

  	while(temp != NULL)
  	{
    		if (strcmp(temp->uname, uname) == 0)
    		{
      			printf("User %s found \n", uname);
      			if (pwd != NULL) 
      			{
        			if (strcmp(temp->pwd, pwd) == 0)
        			{
          				printf("User %s password matched \n", uname);
          				return USER_PASSWD_MATCH_FOUND;
        			}
        			else
        			{
         	 			printf("User %s password not matched \n", uname);
          				return USER_PASSWD_MATCH_NOT_FOUND;
        			}

      			}
      			else
      			{
        			return USER_FOUND;
      			}
    		}

    		temp = temp->next;
  	}

  	printf("User %s not found \n", uname);
  	return USER_NOT_FOUND;
}


/*------------Authenticating User------------*/

void handle_login_msg (int sockfd, msg_t *msg)
{
  	msg_t reply_msg;
  	int ret;

  	printf("Handling message: %d \n", msg->msg_type);
  	printf("Username : %s \n", msg->msg_data);
  	printf("Password : %s \n", msg->msg_add_data);

  	ret = search_user_pwd_in_list(msg->msg_data, msg->msg_add_data);

  	if (ret == USER_PASSWD_MATCH_FOUND)
  	{
    		printf("Auth Success : %s \n", msg->msg_add_data);
    		reply_msg.msg_type = MSG_AUTH_SUCCESS;
  	}
  	else 
  	{
    		printf("Auth Failure : %s \n", msg->msg_add_data);
    		reply_msg.msg_type = MSG_AUTH_FAIL;
  	}
  
  	ret = send(sockfd, (char *) &reply_msg, sizeof(msg), 0);

  	printf("Sent %d bytes reply .. \n", ret);
}

void handle_uname_passwd_msg (int sockfd, msg_t *msg)
{
  	msg_t reply_msg;
  
  	printf("Handling message: %d \n", msg->msg_type);
  	printf("Username : %s \n", msg->msg_data);
 	printf("Password : %s \n", msg->msg_add_data);

  	add_user_to_list(msg->msg_data, msg->msg_add_data);

  	reply_msg.msg_type = MSG_AUTH_SUCCESS;
  
  	int ret = send(sockfd, (char *) &reply_msg, sizeof(msg), 0);

  	printf("Sent %d bytes reply .. \n", ret);
}

/*--------------View Room Module------------*/
void handle_view_room(int sockfd, msg_t *msg)
{

      msg_t reply_msg;
      room_t r1,r2;
      memset(r1.room_type, '\0',  sizeof(r1.room_type));
      memset(r2.room_type, '\0',  sizeof(r2.room_type));


     // reply_msg.msg_type = MSG_AUTH_SUCCESS;

	strcpy(r1.room_type,"SINGLE");
	strcpy(r2.room_type,"DOUBLE");
	
	
	r1.fare=1000;
	r2.fare=2000;
	
      int ret1 = send(sockfd, (char *) &r1, sizeof(room_t), 0);
	int ret2 = send(sockfd, (char *) &r2, sizeof(room_t), 0);
}

/*--------------Book Room Module------------*/
void handle_book_room(int sockfd, msg_t *msg)
{
  	msg_t reply_msg;
  	int flag = 0;
  	

  	printf("Handling message: %d \n", msg->msg_type);
  	printf("Requested Room type : %s \n", msg->msg_data);
  	printf("Check in date : %s \n", msg->msg_add_data);
  	printf("Check out date : %s \n", msg->msg_add1_data);
  
  
  
  	if(strcmp(msg->msg_data,"single") == 0)
  	{
      		initialize_single_func();
      
      
      
  		for(int i = 0 ; i < 4 ; i++)
		{
 
			if(r1[i]->available == 0  )
			{
                     
				flag = 1;
                        
                        	reply_msg.msg_int = r1[i]->room_no;
				reply_msg.msg_type = MSG_ROOM_AVAIL;
                        
				break;
			}
                 
        	}
		if(flag == 0)
			reply_msg.msg_type = MSG_ROOM_NOTAVAIL;

  	}
  	else if(strcmp(msg->msg_data,"double") == 0)
  	{

      		initialize_double_func();
      
  		for(int i = 0 ; i < 4 ; i++)
		{
                  
			if(r2[i]->available == 0 )
			{
				flag = 1;
				reply_msg.msg_type = MSG_ROOM_AVAIL;
				break;
			}
      		}

      		if(flag == 0)
			reply_msg.msg_type = MSG_ROOM_NOTAVAIL;
  	}
  	else if(strcmp(msg->msg_data,"deluxe") == 0)
  	{
      		initialize_deluxe_func();
  		for(int i = 0 ; i < 4 ; i++)
		{
			if(r3[i]->available == 0)
			{
				flag = 1;
				reply_msg.msg_type = MSG_ROOM_AVAIL;
				break;
			}
      		}

      		if(flag == 0)
			reply_msg.msg_type = MSG_ROOM_NOTAVAIL;
  	}
  	else if(strcmp(msg->msg_data,"suite") == 0)
  	{
      		initialize_suite_func();
  		for(int i = 0 ; i < 4 ; i++)
		{
			if(r4[i]->available == 0)
			{
				flag = 1;
				reply_msg.msg_type = MSG_ROOM_AVAIL;
				break;
			}
      		}

      		if(flag == 0)
 
			reply_msg.msg_type = MSG_ROOM_NOTAVAIL;
  	}
  	else
  	{
        	reply_msg.msg_type = MSG_INVALID;

  	} 


  
  	int ret = send(sockfd, (char *) &reply_msg, sizeof(msg), 0);

  	printf("Sent %d bytes reply .. \n", ret);
  
}


void handle_services(int sockfd, msg_t *msg)
{
   	msg_t reply_msg;
   
   	services_t s1 , s2;
   
 

   	strcpy(s1.services_type,"Spa");
   	s1.price = 3000;

   	strcpy(s2.services_type,"Cafeteria");
  	s2.price = 1000;
   
   

   	int ret1 = send(sockfd, (char *) &s1, sizeof(services_t ), 0);
   	int ret2 = send(sockfd, (char *) &s2, sizeof(services_t ), 0);
   
}


void setup_message_handlers()
{
  	msg_actions[MSG_UNAME_PASSWD].msg_action_function = handle_uname_passwd_msg;
  	msg_actions[MSG_LOGIN].msg_action_function = handle_login_msg;
  	msg_actions[MSG_VIEW_ROOMS].msg_action_function = handle_view_room;
  	msg_actions[MSG_BOOK_ROOMS].msg_action_function = handle_book_room;
  	msg_actions[MSG_SERVICES].msg_action_function = handle_services;

}


void handle_client_close(int sock_fd)
{

  	printf("Client on socket %d closed \n", sock_fd);

  
  	close(sock_fd);

}


void process_client_messages(int sockfd, char *recv_buffer)
{
  

  	msg_t *m = (msg_t *) recv_buffer;

  	switch (m->msg_type)
  	{
    		case MSG_UNAME_PASSWD:
                  		printf("Received msg type %d socket = %d ... \n", m->msg_type, sockfd);
                  		msg_actions[m->msg_type].msg_action_function(sockfd, m);
                  		break;
    		case MSG_LOGIN:
                  		printf("Received msg type %d socket = %d ... \n", m->msg_type, sockfd);
                 	 	msg_actions[m->msg_type].msg_action_function(sockfd, m);
                  		break;
    		case MSG_VIEW_ROOMS:
				printf("Received msg type %d socket = %d ... \n", m->msg_type, sockfd);
				msg_actions[m->msg_type].msg_action_function(sockfd, m);
                  		break;
    		case MSG_BOOK_ROOMS:
				printf("Received msg type %d socket = %d ... \n", m->msg_type, sockfd);
				msg_actions[m->msg_type].msg_action_function(sockfd, m);
                  		break;
    		case MSG_SERVICES:
				printf("Received msg type %d socket = %d ... \n", m->msg_type, sockfd);
				msg_actions[m->msg_type].msg_action_function(sockfd, m);
                  		break;
    		default:
                  		printf("Received invalid msg type ... \n");
                  		break;
  	}

  	return;

}

/* Thread to handle clients */
void* client_handler(void *client_sock_fd)
{
  	int *cl_sock_fd = (int *) client_sock_fd;
  	int ret = 0;

  	char send_buffer[SEND_BUFFER_SIZE];
  	char recv_buffer[RECV_BUFFER_SIZE];

  	printf("%s():%d Client Fd = %d\n",__FUNCTION__, __LINE__, *cl_sock_fd);

  	while(1)
  	{
    		printf("%s():%d Waiting on recv for fd = %d \n",
           			__FUNCTION__, __LINE__, *cl_sock_fd);
	
    		ret = recv(*cl_sock_fd, recv_buffer, sizeof(recv_buffer), 0);

    		if (ret == 0)
    		{
      			printf("%s():%d Client has closed on socket fd = %d \n",
           			__FUNCTION__, __LINE__, *cl_sock_fd);

      

      			handle_client_close(*cl_sock_fd);

      			cl_count--;
      
      			pthread_exit(NULL);
    		}

    		process_client_messages(*cl_sock_fd, recv_buffer);

	}

  	pthread_exit(NULL);
}

int main()
{

  	int server_fd, ret;

  	struct sockaddr_in server_addr;
  
  	int cl_sock_fd[MAX_CLIENTS];  
  
  	setup_message_handlers();

  	pthread_t cl_threads[MAX_CLIENTS];

  	server_fd = socket(AF_INET, SOCK_STREAM, 0);

  	if (server_fd < 0) 
  	{
    		printf("Error in connection.\n");
    		exit(1);
  	}

  	printf("TCP Server Socket is created.\n");

  	memset(&server_addr, '\0',  sizeof(server_addr));

  	server_addr.sin_family = AF_INET;
  	server_addr.sin_port = htons(PORT);

  	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  	ret = bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

  	if (ret < 0) 
  	{
    		printf("Error in binding.\n");
    		exit(1);
  	}

  	if (listen(server_fd, 10) == 0) 
  	{
    		printf("Listening...\n\n");
  	}

  	while (1) 
  	{
    		ret = accept(server_fd, NULL, NULL);

    		if (ret < 0) 
    		{
      			perror("accept failed: ");
      			exit(1);
    		}

    		cl_sock_fd[cl_count] = ret;
      
    		printf("cl_count = %d fd = %d clfd = %d \n",
             	cl_count, ret, cl_sock_fd[cl_count]);

    		ret = pthread_create(&cl_threads[cl_count], NULL, client_handler,&cl_sock_fd[cl_count]);
   
    		if (ret == 0)
    		{
      
      			printf("A new thread is created for client on fd: %d \n", cl_sock_fd[cl_count]);
      			cl_count++;
      			printf("Total clients connected : %d\n\n", cl_count);
    		}  

    		if (cl_count == MAX_CLIENTS)
    		{
      			printf("Max clients %d are connected..No more connections will be accepted\n", cl_count);
      			break;
   		}
  	}

  	for (int i = 0; i < cl_count; i++)
  	{  
    		pthread_join(cl_threads[i], NULL);
  	}

  	return 0;
}

