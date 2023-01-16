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
#include "../include/common.h"
#include "../include/server.h"


/* Room Structure Initializtion */
single_t r1[4] = {{101,0,"00/00","00/00"} , {102,0,"00/00","00/00"} , {103,0,"00/00","00/00"} , {104,0,"00/00","00/00"} };
double_t r2[4] = {{201,0,"00/00","00/00"} , {202,0,"00/00","00/00"} , {203,0,"00/00","00/00"} , {204,0,"00/00","00/00"} };
deluxe_t r3[4] = {{301,0,"00/00","00/00"} , {302,0,"00/00","00/00"} , {303,0,"00/00","00/00"} , {304,0,"00/00","00/00"} };
suite_t r4[4] = {{401,0,"00/00","00/00"} , {402,0,"00/00","00/00"} , {403,0,"00/00","00/00"} , {404,0,"00/00","00/00"} };

/*Room type fare structure initialization*/
int room_fare[4] = {3000,5000,10000,15000};

int main()
{

  	int server_fd, ret;

  	struct sockaddr_in server_addr;
  
  	int cl_sock_fd[MAX_CLIENTS];  
  	
	//Call for setting up message handlers
  	setup_message_handlers();
	

  	pthread_t cl_threads[MAX_CLIENTS];
	
	//Creates socket
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
	
	//Binding done to address and port number to receive data on socket
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
		//accepts clients
    		ret = accept(server_fd, NULL, NULL);

    		if (ret < 0) 
    		{
      			perror("accept failed: ");
      			exit(1);
    		}

    		cl_sock_fd[cl_count] = ret;
      
    		printf("cl_count = %d fd = %d clfd = %d \n",
             	cl_count, ret, cl_sock_fd[cl_count]);
		
		Pthread created
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

/*Assigns fuctions to the message type received*/
void setup_message_handlers()
{

  	msg_actions[MSG_UNAME_PASSWD].msg_action_function = handle_uname_passwd_msg;
  	msg_actions[MSG_LOGIN].msg_action_function = handle_login_msg;
  	msg_actions[MSG_VIEW_ROOMS].msg_action_function = handle_view_room;
  	msg_actions[MSG_BOOK_ROOMS].msg_action_function = handle_book_room;
	msg_actions[MSG_GET_FARE].msg_action_function = handle_fare;
      	msg_actions[MSG_CANCEL_ROOMS].msg_action_function = handle_cancel_room;

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

/* Adding New User to Linked list*/
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

/* Validating Existing User */
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


/*Authenticating User*/
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

/*Handles username and password send by client
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

/*View Room Module*/
void handle_view_room(int sockfd, msg_t *msg)
{

      FILE *fp;
      fp = fopen("./data/room_details.txt","r");
	if(fp == NULL)
	{
		printf("Error in opening file\n");
	}
	while(fgets(msg->msg_data,MAX_DATA_LEN,fp) != NULL)
	{
		if(send(sockfd,msg->msg_data,sizeof(msg->msg_data),0) == -1)
		{
			msg->msg_type = MSG_FAIL;
			printf("Error\n");
                  break;
		}
		bzero(msg->msg_data,MAX_DATA_LEN);
	}
      fclose(fp);
      return;
}


/*Checking dates*/ 
int checking_dates(char* date1, char* date2)
{
	char check_out_1[10];
	char check_in_2[10];
	strcpy(check_out_1,date1);
	strcpy(check_in_2,date2);
	char delimiter[] = "-";

	int d1 = atoi(strtok(check_out_1,delimiter));
      	int m1 = atoi(strtok(NULL,delimiter));

      	int d2 = atoi(strtok(check_in_2,delimiter));
      	int m2 = atoi(strtok(NULL,delimiter));

	if(m1 < m2)
	{
		return 1;
	}	
	else if(m1 == m2)
 	{
		if(d1 <= d2)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
		return 0;     
}





/*Book Room Module*/
void handle_book_room(int sockfd, msg_t *msg)
{

  	msg_t reply_msg;
  	int flag = 0;
  	int single_room_count = 0;
	int double_room_count = 0;
	int deluxe_room_count = 0;
	int suite_room_count = 0;
 	

  	printf("Handling message: %d \n", msg->msg_type);
  	printf("Requested Room type : %s \n", msg->msg_data);
  	printf("Check in date : %s \n", msg->msg_add_data);
  	printf("Check out date : %s \n", msg->msg_add1_data);
      	printf("User name : %s \n", msg->msg_add2_data);
  	
	
  	//Reserving single room for a user 
  	if(strcmp(msg->msg_data,"single") == 0)
  	{
      		  
  		for(int i = 0 ; i < 4 ; i++)
		{
			if(r1[i]->available == 1 )
			{
				int ret1 = checking_dates(r1[i]->check_out,msg->msg_add_data);
				if(ret1 == 1)
                     		{
					flag = 1;
                        		strcpy(r1[i]->uname,msg->msg_add2_data);     
                        		reply_msg.msg_int = r1[i]->room_no;
                        		strcpy(reply_msg.msg_data,msg->msg_add2_data); 
					strcpy(r1[i]->check_in,msg->msg_add_data);
					strcpy(r1[i]->check_out,msg->msg_add1_data);
					reply_msg.msg_type = MSG_ROOM_AVAIL;
                                
					break;
				}
			}
			
 
			if(r1[i]->available == 0)
			{
                     
				flag = 1;
                        	r1[i]->available = 1;
				strcpy(r1[i]->check_in,msg->msg_add_data);
				strcpy(r1[i]->check_out,msg->msg_add1_data);
                        	strcpy(r1[i]->uname,msg->msg_add2_data);  
   
                       	 	reply_msg.msg_int = r1[i]->room_no;
                        	strcpy(reply_msg.msg_data,msg->msg_add2_data); 
				reply_msg.msg_type = MSG_ROOM_AVAIL;
                                
				break;
			}
                 
        	}
		if(flag == 0)
			reply_msg.msg_type = MSG_ROOM_NOTAVAIL;

  	}
	//Reserving double room for a user 
  	else if(strcmp(msg->msg_data,"double") == 0)
  	{

      
  		for(int i = 0 ; i < 4 ; i++)
		{

			if(r2[i]->available == 1 )
			{
				int ret1 = checking_dates(r2[i]->check_out,msg->msg_add_data);
				if(ret1 == 1)
                     		{
					flag = 1;
                        		strcpy(r2[i]->uname,msg->msg_add2_data);     
                        		reply_msg.msg_int = r2[i]->room_no;
                        		strcpy(reply_msg.msg_data,msg->msg_add2_data); 
					strcpy(r2[i]->check_in,msg->msg_add_data);
					strcpy(r2[i]->check_out,msg->msg_add1_data);
					reply_msg.msg_type = MSG_ROOM_AVAIL;
                                
					break;
				}
			}
			
                  
			if(r2[i]->available == 0 )
			{
				flag = 1;
                        	r2[i]->available = 1;
                        	strcpy(r2[i]->uname,msg->msg_add2_data);
                                 
                       	 	reply_msg.msg_int = r2[i]->room_no;
                        	strcpy(reply_msg.msg_data,msg->msg_add2_data);
                                 
				reply_msg.msg_type = MSG_ROOM_AVAIL;
                                
				break;
			}
      	}

      	if(flag == 0)
		reply_msg.msg_type = MSG_ROOM_NOTAVAIL;
  	}

	//Reserving deluxe room for a user 
  	else if(strcmp(msg->msg_data,"deluxe") == 0)
  	{
      		
  		for(int i = 0 ; i < 4 ; i++)
		{
			if(r3[i]->available == 1 )
			{
				int ret1 = checking_dates(r3[i]->check_out,msg->msg_add_data);
				if(ret1 == 1)
                     		{
					flag = 1;
                        		strcpy(r3[i]->uname,msg->msg_add2_data);     
                        		reply_msg.msg_int = r3[i]->room_no;
                        		strcpy(reply_msg.msg_data,msg->msg_add2_data); 
					strcpy(r3[i]->check_in,msg->msg_add_data);
					strcpy(r3[i]->check_out,msg->msg_add1_data);
					reply_msg.msg_type = MSG_ROOM_AVAIL;
                                
					break;
				}
			}
			
			if(r3[i]->available == 0)
			{
				flag = 1;
                        	r3[i]->available = 1;
                        	strcpy(r3[i]->uname,msg->msg_add2_data);
                                 
                        	reply_msg.msg_int = r3[i]->room_no;
                        	strcpy(reply_msg.msg_data,msg->msg_add2_data);
                                 
				reply_msg.msg_type = MSG_ROOM_AVAIL;
                                
				break;
			}
      	}

      	if(flag == 0)
		reply_msg.msg_type = MSG_ROOM_NOTAVAIL;
  	}

	//Reserving suite room for a user 
  	else if(strcmp(msg->msg_data,"suite") == 0)
  	{
      		
  		for(int i = 0 ; i < 4 ; i++)
		{
			if(r4[i]->available == 1 )
			{
				int ret1 = checking_dates(r4[i]->check_out,msg->msg_add_data);
				if(ret1 == 1)
                     	{
					flag = 1;
                        		strcpy(r4[i]->uname,msg->msg_add2_data);     
                        		reply_msg.msg_int = r1[i]->room_no;
                        		strcpy(reply_msg.msg_data,msg->msg_add2_data); 
					strcpy(r4[i]->check_in,msg->msg_add_data);
					strcpy(r4[i]->check_out,msg->msg_add1_data);
					reply_msg.msg_type = MSG_ROOM_AVAIL;
                                
					break;
				}
			}
			
			if(r4[i]->available == 0)
			{
				flag = 1;
                        	r4[i]->available = 1;
                        	strcpy(r4[i]->uname,msg->msg_add2_data);
                                 
                        	reply_msg.msg_int = r4[i]->room_no;
                        	strcpy(reply_msg.msg_data,msg->msg_add2_data);
                                 
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
	
	//Counting number of rooms booked
	for(int i = 0 ; i < 4 ; i++)
      	{ 
		if(r1[i]->available == 1)
			single_room_count++;
 		if(r2[i]->available == 1)
			double_room_count++;
		if(r3[i]->available == 1)
			deluxe_room_count++;
		if(r4[i]->available == 1)
			suite_room_count++;
	}
	
	//Displaying the room statistics 
	printf("----------+----------+----------+\n");
      	printf("Room Type | Occupied | Available |\n");
	printf("Single Room   %d		%d    \n",single_room_count, 4 - single_room_count);
	printf("Double Room   %d		%d    \n",double_room_count, 4 - double_room_count);
	printf("Deluxe Room   %d		%d    \n",deluxe_room_count, 4 - deluxe_room_count);
	printf("Suite  Room   %d		%d    \n",suite_room_count, 4 - suite_room_count);
	printf("----------+----------+----------+\n");
	

  	
  	int ret = send(sockfd, (char *) &reply_msg, sizeof(&reply_msg), 0);

	

  	printf("Sent %d bytes reply .. \n", ret);

  
}



/*Handle fare */
void handle_fare(int sockfd, msg_t *msg)
{
	msg_t reply_msg;
	reply_msg.msg_int = room_fare[msg->msg_int];
	int ret = send(sockfd, (char *) &reply_msg, sizeof(msg), 0);

	printf("Sent %d bytes reply .. \n", ret);
	
}



/*Handle cancel room message*/
void handle_cancel_room(int sockfd, msg_t *msg)
{
	
	msg_t reply_msg;

      	printf("Handling message: %d \n", msg->msg_type);
      	printf("Room Number : %d \n", msg->msg_int);
      	printf("User name : %s \n", msg->msg_data);
	
	//Cancelling reservation for single room type
    	if(msg->msg_int < 200 && msg->msg_int > 100)
    	{
        	for(int i = 0; i < 4; i++)
        	{ 
			//Checks for the username and room number present in the room structures    
            		if(msg->msg_int == r1[i]->room_no && strcmp(msg->msg_data,r1[i]->uname) == 0 && r1[i]->available == 1)
            		{
				//Reply sent to the client
                		r1[i]->available = 0;
                		reply_msg.msg_type = MSG_CANCELLED;
                		break;
            		}
            		else 
            		{
                		reply_msg.msg_type = MSG_NOT_CANCELLED;

            		}
        	}
    	}

	//Cancelling reservation for double room type
   	else if(msg->msg_int < 300 && msg->msg_int > 200)
    	{
        	for(int i = 0; i < 4; i++)
        	{        
            		if(msg->msg_int == r2[i]->room_no && strcmp(msg->msg_data,r2[i]->uname) == 0 && r2[i]->available == 1)
            		{
                		r2[i]->available = 0;
                		reply_msg.msg_type = MSG_CANCELLED;
                		break;
            		}
            		else 
            		{
                		reply_msg.msg_type = MSG_NOT_CANCELLED;

 			}
        	}    
    	}
	
	//Cancelling reservation for deluxe room type
    	else if(msg->msg_int < 400 && msg->msg_int > 300)
    	{
        	for(int i = 0; i < 4; i++)
        	{        
            		if(msg->msg_int == r3[i]->room_no && strcmp(msg->msg_data,r3[i]->uname) == 0 && r3[i]->available == 1)
            		{
                		r3[i]->available = 0;
                		reply_msg.msg_type = MSG_CANCELLED;
                		break;
            		}
            		else 
            		{
                		reply_msg.msg_type = MSG_NOT_CANCELLED;
            		}
        	}    
    	}

	//Cancelling reservation for suite room type
    	else if(msg->msg_int < 500 && msg->msg_int > 400)
    	{
        	for(int i = 0; i < 4; i++)
        	{        
            		if(msg->msg_int == r4[i]->room_no && strcmp(msg->msg_data,r4[i]->uname) == 0 && r4[i]->available == 1)
            		{
               	 		r4[i]->available = 0;
                		reply_msg.msg_type = MSG_CANCELLED;
                		break;
            		}

            		else 
            		{
                		reply_msg.msg_type = MSG_NOT_CANCELLED;
            		}
        	}    
    	}
    	else 
    	{
        	reply_msg.msg_type = MSG_NOT_CANCELLED;

    	}

 

    	int ret = send(sockfd, (char *) &reply_msg, sizeof(msg), 0);

      	printf("Sent %d bytes reply .. \n", ret);
		
} 



/*Closes client */
void handle_client_close(int sock_fd)
{

  	printf("Client on socket %d closed \n", sock_fd);
  
  	close(sock_fd);

}


/*Process cleints message received from client*/
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

		case MSG_GET_FARE: 
				printf("Received msg type %d socket = %d ... \n", m->msg_type, sockfd);
				msg_actions[m->msg_type].msg_action_function(sockfd, m);
                  		break;

		case MSG_CANCEL_ROOMS:
				printf("Received msg type %d socket = %d ... \n", m->msg_type, sockfd);
				msg_actions[m->msg_type].msg_action_function(sockfd, m);
                  		break;
          
    		default:
                  		printf("Received invalid msg type ... \n");
                  		break;
  	}

  	return;

}

