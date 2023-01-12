/*
 *
 *
 *  File: client.c
 *
 *  Description: Client side of Hotel Management system responsible for providing user interface  
 *  
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
#include "common.h"


/* Function to display Room Details sent by server */
void display_room_details()
{
	int n = 0; 
  	msg_t msg, *rx_msg;
  	char buffer[MAX_DATA_LEN];
  	//char str[]="VIEW ROOMS";
  	//memset(msg.msg_data, '\0', sizeof(msg.msg_data));
  	msg.msg_type = MSG_VIEW_ROOMS;
  	//strncpy(msg.msg_data, str , strlen(str));
  
	
	//Sending request message to server to receive room details
  	int ret = send(cl_sock_fd, (char *) &msg, sizeof(msg), 0);

  	printf("Sent bytes = %d \n", ret);
	

	
	printf("			    ROOM TYPES AND FARES PER DAY			    \n");
	printf("                  --------------------------------\n");
	printf("ROOM TYPE		FARE PER DAY\n");
	//Receiving room details from server
     
	while( n < 4 )
      {
		recv(cl_sock_fd,buffer,MAX_DATA_LEN, 0);
		printf("%s\n",buffer);
		bzero(buffer,MAX_DATA_LEN);
		n++;
      }
      return;  	

}


/*Function to confirm the reservation */
void confirm_booking()
{
	int ret1,ret2,ret3; 
   	msg_t msg, *rx_msg;
   	services_t *r1, *r2;

   	printf("Do you want to add on any services\n");
   
   	memset(msg.msg_data, '\0', sizeof(msg.msg_data));
  
  	msg.msg_type = MSG_SERVICES;

   	printf("Messaage Created \n");
   	printf("  Type: %d \n", msg.msg_type);

   	int ret = send(cl_sock_fd, (char *) &msg, sizeof(msg), 0);

   	printf("Sent bytes = %d \n", ret);
   
   	ret = recv(cl_sock_fd, (char *) recv_buffer, sizeof(recv_buffer), 0);

   	printf("Received %d bytes from Server \n", ret);

   	ret1 = recv(cl_sock_fd, (char *) recv_buffer, sizeof(recv_buffer), 0);
   	r1 = (services_t *) recv_buffer;
   	printf("%s\n",r1->services_type);
   	printf("%d\n\n",r1->price);

   	ret2 = recv(cl_sock_fd, (char *) recv_buffer, sizeof(recv_buffer), 0);
   	r2 = (services_t *) recv_buffer;
   	printf("%s\n",r2->services_type);
   	printf("%d\n\n",r2->price);
}
  
/*Function to generate the reservation bill */
 
void generate_ticket(msg_t* recv, msg_t recv1 )
{
  	char name[32];
  	int age;
  	long phone_no;
  
  	printf("\nEnter name : ");
  	scanf("%s",name);
  	printf("\nEnter age : ");
  	scanf("%d",&age);
  	printf("\nEnter phone number :");
 	scanf("%ld",&phone_no);

  	int room_no = recv->msg_int;

  	char room_type[15];
  	char check_in[10];
  	char check_out[10];


  	strcpy(room_type,recv1.msg_data);
  	strcpy(check_in,recv1.msg_add_data);
  	strcpy(check_out,recv1.msg_add1_data);
   
  
  	printf("\n\n=============================================================\n");
  	printf("\n\t\tHOTEL BILL\n");
 	printf("\n\n=============================================================\n");
  	printf("\n\t\tName           : %s\n",name);
  	printf("\n\t\tAge            : %d\n",age);
  	printf("\n\t\tPhone Number   : %ld\n",phone_no);
  	printf("\n\t\tRoom Number    : %d\n",room_no);
  	printf("\n\t\tRoom Type      : %s\n",room_type);
  	printf("\n\t\tCheck In Date  : %s\n",check_in);
  	printf("\n\t\tCheck Out Date : %s\n",check_in);
 	printf("\n\n=============================================================\n");
}

/*Function to check availability of room type requested by user */
int book_room()
{
  	char room_type[32];
  	char check_in[10];
  	char check_out[10];
  
  	int ret; 
  	msg_t msg, *rx_msg;
  	char ans;

  	printf("\n\nEnter the room type you wish to book: ");
  	scanf("%s",room_type);
  	printf("\nEnter your check in date: ");
  	scanf("%s",check_in);
  	printf("\nEnter your check out date: ");
  	scanf("%s",check_out);
 
  	memset(msg.msg_data, '\0', sizeof(msg.msg_data));
  	memset(msg.msg_add_data, '\0', sizeof(msg.msg_add_data));
  	memset(msg.msg_add1_data, '\0', sizeof(msg.msg_add1_data));
  
  	msg.msg_type = MSG_BOOK_ROOMS;
  	strncpy(msg.msg_data, room_type, strlen(room_type));
  	strncpy(msg.msg_add_data, check_in, strlen(check_in));
  	strncpy(msg.msg_add1_data, check_out, strlen(check_out));


  	ret = send(cl_sock_fd, (char *) &msg, sizeof(msg), 0);


  	ret = recv(cl_sock_fd, (char *) recv_buffer, sizeof(recv_buffer), 0);



  	if (ret == 0)
  	{

    		handle_server_close(cl_sock_fd);
		return -2;
  	}

  	rx_msg = (msg_t *) recv_buffer;

  	if (rx_msg->msg_type == MSG_ROOM_AVAIL)
  	{
      		printf("\nThe Room is available\n");
       		printf("\nDo you want to continue booking? : ");
      
     		scanf(" %c",&ans);
     
      		if(ans == 'y')
      		{
          		//confirm_booking();
          		generate_ticket(rx_msg , msg);
          
      		}
        }
  	else if(rx_msg->msg_type == MSG_ROOM_NOTAVAIL)
  	{ 
    		printf("\nRoom not available\n");
  	}
  	else if(rx_msg->msg_type == MSG_INVALID)
  	{	 
    		printf("\nInvalid room type\n");
  	}
  
}


void handle_server_close (int sockfd)
{

  	close(sockfd);

}


/*Function to display Hotel Functionalities Menu */
void display_hotel_menu()
{
  	int option;

  	while(1)
  	{
   	 	printf("\n\n=============================================================\n");
    		printf("\n\t\tWELCOME TO HOTEL RESERVATION SYSTEM\n");
    		printf("\n\n=============================================================\n");

    		printf("\n\t\t1. VIEW ROOM TYPES \n");
    		printf("\n\t\t2. BOOK ROOM \n");
    		printf("\n\t\t3. CANCEL BOOKING\n");
    		printf("\n\t\t4. EXIT TO MAIN MENU \n");

    		printf("\n\n=============================================================\n");
    
   	 	printf("\n\nEnter option : ");
    		scanf("%d", &option);

    		switch(option)
    		{
      			case DISP_ROOM: display_room_details();
		   				    break;
      			case BOOK_ROOM: book_room();
    		  				    break;
      			case EXIT:
              				return;
      			default:
              				continue;
    		}
  	}
}


/*Validation of user entered username and password */
int validate_user_name_passwd(char *uname, char *pwd)
{
  	if ((strlen(uname) > MAX_INPUT_LEN) || (strlen(uname) < 4))
  	{
    		printf("\nInvalid username, length must be between 4 and 32 \n");
    		return -1;
  	}
  	else if ((strlen(pwd) > 6) || (strlen(pwd) < 4)) 
  	{
    		printf("\nInvalid password, length must be between 4 and 6 \n");
    		return -1;
 	}

  	return 0;
}


int get_and_validate_input(char *input1, char *input2)
{
  	printf("\n\nEnter User Name  :  ");
  	scanf("%s", input1);

  	printf("\nEnter User Passwd  (Max 6 chars) : ");
  	scanf("%s", input2);

  	return validate_user_name_passwd(input1, input2); 
}

//Function to send username and password to server for authentication
int proces_login_msg(char *uname, char *pwd)
{
  	int ret; 
  	msg_t msg, *rx_msg;
  
  	memset(msg.msg_data, '\0', sizeof(msg.msg_data));
  	memset(msg.msg_add_data, '\0', sizeof(msg.msg_add_data));

  	msg.msg_type = MSG_LOGIN;
  	strncpy(msg.msg_data, uname, strlen(uname));
  	strncpy(msg.msg_add_data, pwd, strlen(pwd));

  	ret = send(cl_sock_fd, (char *) &msg, sizeof(msg), 0);

	ret = recv(cl_sock_fd, (char *) recv_buffer, sizeof(recv_buffer), 0);

  	if (ret == 0)
  	{
    		handle_server_close(cl_sock_fd);
		return -2;
  	}

  	rx_msg = (msg_t *) recv_buffer;

  	if (rx_msg->msg_type == MSG_AUTH_SUCCESS)
  	{
    		return MSG_SUCCESS;
  	}
  	else if (rx_msg->msg_type == MSG_AUTH_FAIL)
  	{
    		return MSG_FAIL;
  	}
  	else
  	{
    		return MSG_INVALID;
 	}

}

int proces_uname_pwd_msg(char *uname, char *pwd)
{
  	int ret; 
  	msg_t msg, *rx_msg;
  
  	memset(msg.msg_data, '\0', sizeof(msg.msg_data));
  	memset(msg.msg_add_data, '\0', sizeof(msg.msg_add_data));

  	msg.msg_type = MSG_UNAME_PASSWD;
  	strncpy(msg.msg_data, uname, strlen(uname));
  	strncpy(msg.msg_add_data, pwd, strlen(pwd));

  	ret = send(cl_sock_fd, (char *) &msg, sizeof(msg), 0);


  	ret = recv(cl_sock_fd, (char *) recv_buffer, sizeof(recv_buffer), 0);

  	if (ret == 0)
  	{

    		handle_server_close(cl_sock_fd);
		return -2;
  	}

 	rx_msg = (msg_t *) recv_buffer;

  	if (rx_msg->msg_type == MSG_AUTH_SUCCESS)
  	{
    		return MSG_SUCCESS;
  	}
  	else if (rx_msg->msg_type == MSG_AUTH_FAIL)
  	{
    		return MSG_FAIL;
  	}
  	else
  	{
    		return MSG_INVALID;
  	}
}

void handle_client_close(int sock_fd)
{

  /* TODO */
  close(sock_fd);

}

int main()
{
  	int ret;
  	int i, option;

  	struct sockaddr_in server_addr;

  	int  menu_data_int;
  	char menu_data_string1[MAX_INPUT_LEN];
  	char menu_data_string2[MAX_INPUT_LEN];

	//Socket Creation 

  	cl_sock_fd = socket(AF_INET,SOCK_STREAM, 0);

  	if (cl_sock_fd < 0) 
  	{
    		printf("\nError in connection.\n");
    		exit(1);
  	}

  	printf("\nThe Client Socket is created.\n");

  	memset(send_buffer, '\0', sizeof(send_buffer));
  	memset(recv_buffer, '\0', sizeof(recv_buffer));

  	server_addr.sin_family = AF_INET;
  	server_addr.sin_port = htons(PORT);

  	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");\

  	ret = connect(cl_sock_fd, (struct sockaddr*)&server_addr,
      	sizeof(server_addr));

  	if (ret < 0) 
  	{
    		printf("\nError in connection.\n");
    		exit(1);
  	}

  	printf("\nClient is connected to Server.\n");
	

	//Menu Interface for User Authentication
	
  	while (1) 
  	{
    		printf("\n\n=============================================================\n");
   	 	printf("\n\t\t   HOTEL RESERVATION SYSTEM\n");
    		printf("\n\n=============================================================\n");
    		printf("\n\t\t\t  MENU\n\n");


    		printf("\n\t\t\t1. Sign up\n");
    		printf("\n\t\t\t2. Log in\n");
    		printf("\n\t\t\t3. Logout\n\n");

    		printf("\nEnter option : ");
    		scanf("%d", &option);
  
    		memset(menu_data_string1, '\0', sizeof(menu_data_string1));
    		memset(menu_data_string2, '\0', sizeof(menu_data_string2));

    
    		switch(option)
    		{
     	 		case 1:
				//Call function to validate username and password
				ret = get_and_validate_input(menu_data_string1, menu_data_string2);

        			if (ret == -1)
        			{
          				printf("\nInvalid input format \n");
          				break;
        			}
				//Call function to process username and password	
        			ret = proces_uname_pwd_msg(menu_data_string1, menu_data_string2);

        			if (ret == MSG_FAIL)
        			{
        	 	 		printf("\nUser Authentication Failed \n");
          				break;
        			}
        			else if (ret == MSG_INVALID)
        			{
          				printf("\nUser Authentication Failed \n");
          				break;

        			}
        			else if (ret == MSG_SUCCESS)
        			{
          				printf("\nUser Authentication Success \n");

          				display_hotel_menu();
        			}

        			break;

      			case 2:

        			ret = get_and_validate_input(menu_data_string1, menu_data_string2);

        			if (ret == -1)
        			{
          				printf("\nInvalid input format \n");
          				break;
        			}

        			ret = proces_login_msg(menu_data_string1, menu_data_string2);

        			if (ret == MSG_FAIL)
        			{
        		  		printf("\nUser Authentication Failed \n");
          				break;
        			}
        			else if (ret == MSG_INVALID)
        			{
        	  			printf("\nUser Authentication Failed \n");
          				break;
        			}
        			else if (ret == MSG_SUCCESS)
       	 			{
          				printf("\nUser Authentication Success \n");
          				display_hotel_menu();
        			}

        			break;
      
      				case 3:
        				exit(0);

      				default:
        				printf("\nInvalid Option .. \n");
        				break;
    		}

  	}

  return 0;
}

