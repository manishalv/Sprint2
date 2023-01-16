/*
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
#include<stdbool.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/common.h"




/* Main function */
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

  	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

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
    		printf("\n\t\t\t3. Exit from the System\n\n");

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


/*Prompt the user to enter username and password*/
int get_and_validate_input(char *input1, char *input2)
{
  	printf("\n\nEnter User Name  :  ");
  	scanf("%s", input1);

  	printf("\nEnter User Passwd  (Max 6 chars) : ");
  	scanf("%s", input2);

  	return validate_user_name_passwd(input1, input2); 
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


/*Function to send username and password to server for adding the user to the list*/

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
                //display_hotel_menu(uname);
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

/*Function to send username and password to server for authentication*/

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

/*Function to display Hotel Functionalities Menu */

void display_hotel_menu(void)
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
    		printf("\n\t\t4. LOGOUT \n");

    		printf("\n\n=============================================================\n");
    
   	 	printf("\n\nEnter option : ");
    		scanf("%d", &option);

    		switch(option)
    		{
      			case DISP_ROOM:   display_room_details();
		   			  break;

      			case BOOK_ROOM:   book_room();
    		  			  break;

			case CANCEL_ROOM: cancel_room();
                                          break;

      			case EXIT:	  return;

      			default:          printf("Invalid choice\n");
              				  continue;
    		}
  	}
}

/* Function to display Room Details */

void display_room_details(void)
{
	int n = 0; 
  	msg_t msg, *rx_msg;
  	char buffer[MAX_DATA_LEN];
  	msg.msg_type = MSG_VIEW_ROOMS;
 
  
	
	//Sending request message to server to receive room details
  	int ret = send(cl_sock_fd, (char *) &msg, sizeof(msg), 0);

  	printf("Sent bytes = %d \n", ret);
	

	
	printf("\t\t\t HOTEL SAPPHIRE\n");
	printf("\t\t  -----------------------------\n");
	printf("\t\tROOM TYPE		FARE PER DAY\n\n");
	//Receiving room details from server
     
	while( n < 4 )
      {
		recv(cl_sock_fd,buffer,MAX_DATA_LEN, 0);
		printf("\t\t%s\n",buffer);
		bzero(buffer,MAX_DATA_LEN);
		n++;
      }
      return;  	

}

/*Function to check availability of room type requested by user and book the room*/

int book_room(void)
{
  	char room_type[32];
  	char check_in[10];
  	char check_out[10];
	char date[10];
	
  
  	char uname[32];
	char pwd[10];
	
  
  	int ret,ret1; 
  	msg_t msg, *rx_msg;

	printf("\nEnter the username to continue booking : ");
	scanf("%s",uname); 

  	printf("\n\nEnter your password : ");
  	scanf("%s",pwd);
        
	ret = validate_user_name_passwd(uname, pwd);

      	ret = proces_login_msg(uname, pwd);
	
	if (ret == MSG_FAIL)
      	{
        	printf("\nUser Authentication Failed \n");
          	
      	}
      	else if (ret == MSG_INVALID)
      	{
		printf("\nUser Authentication Failed \n");
          	
      	}
      	else if (ret == MSG_SUCCESS)
      	{
		while(1)
		{
  			printf("\n\nEnter the room type you wish to book(single/double/deluxe/suite)? : ");
  			scanf("%s",room_type);
 
			if((strcmp(room_type,"single") == 0) || (strcmp(room_type,"double") == 0) || (strcmp(room_type,"deluxe") == 0) || (strcmp(room_type,"suite") == 0))
			{ 
				break;
			}
            		else
			{
				printf("Invalid room type\n");
			}
	
		}
      		while(1) 
 		{
  			printf("\nEnter your check in date in the format dd-mm: ");
  			scanf("%s",check_in);
			strcpy(date,check_in);
			int ret = validate_date(date);
             
			if(ret == DATE_VALID)
				break;
			else if( ret == FORMAT_INVALID){
				printf("Invalid date format\n");
				continue;
			}
			else if( ret == INVALID_MONTH){
				printf("Invalid month\n");
				continue;
			}
			else if( ret == INVALID_DATE){
				printf("Invalid date\n");
				continue;
			}
		}
       
	 	while(1) 
 		{
  		
  			printf("\nEnter your check out date in the format dd-mm: ");
  			scanf("%s",check_out);
			strcpy(date,check_out);
			int ret = validate_date(date);
             
			if(ret == DATE_VALID)
				break;
			else if( ret == FORMAT_INVALID){
				printf("Invalid date format\n");
				continue;
			}
			else if( ret == INVALID_MONTH){
				printf("Invalid month\n");
				continue;
			}
			else if( ret == INVALID_DATE){
				printf("Invalid date\n");
				continue;
			}
		}
	
 		ret = checkin_checkout(check_in,check_out);
	 
		if(ret == 0)
 		{
    			printf("Check in date is greater than check out date\n");
			return -1;
		}

  		memset(msg.msg_data, '\0', sizeof(msg.msg_data));
  		memset(msg.msg_add_data, '\0', sizeof(msg.msg_add_data));
  		memset(msg.msg_add1_data, '\0', sizeof(msg.msg_add1_data));
     	 	memset(msg.msg_add2_data, '\0', sizeof(msg.msg_add2_data));
  
  		msg.msg_type = MSG_BOOK_ROOMS;
  		strncpy(msg.msg_data, room_type, strlen(room_type));
  		strncpy(msg.msg_add_data, check_in, strlen(check_in));
  		strncpy(msg.msg_add1_data, check_out, strlen(check_out));
      		strncpy(msg.msg_add2_data, uname, strlen(uname));

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
       		
          		generate_ticket(rx_msg , msg);
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
  
}

/*Validates the check in and check out dates and format entered by the user*/

int validate_date(char* date)
{
		
	if(strlen(date) != 5 || date[2] != '-')
	{
		return FORMAT_INVALID;
	}
   
    	int day_int = atoi(strtok(date,"-"));
   	int month_int = atoi(strtok(NULL,"-"));
    
    	if(day_int >= 1 && day_int <= 31)
    	{
        	if(month_int >= 1  && month_int <= 12)
        	{
            		return DATE_VALID;
        	}
       		else
        	{
            		return INVALID_MONTH;
        	}
    	}
    	else
    	{
        	return INVALID_DATE;
    	}
}

/*Calculates the fare for the number of days the room is booked*/

int calculate_fare(char* room_type, char* check_in,char* check_out)
{
	msg_t msg,*rx_msg;
	char delimiter[] = "-";
	int total = 0;
	int fare;
  	char date1[20];
      	char date2[20];


      	strcpy(date1,check_in);
      	strcpy(date2,check_out);
      	int daysPerMonth[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
	int d1 = atoi(strtok(date1,delimiter));
      	int m1 = atoi(strtok(NULL,delimiter));

      	int d2 = atoi(strtok(date2,delimiter));
      	int m2 = atoi(strtok(NULL,delimiter));


      	msg.msg_type = MSG_GET_FARE;

	if(strcmp(room_type,"single") == 0)
      	{ 
		msg.msg_int = 0;
      	}
	if(strcmp(room_type,"double") == 0)
      	{ 
		msg.msg_int = 1;
      	}
      	if(strcmp(room_type,"deluxe") == 0)
      	{ 
		msg.msg_int = 2;
      	}
      	if(strcmp(room_type,"suite") == 0)
      	{ 
		msg.msg_int = 3;
      	}

      	int ret1 = send(cl_sock_fd, (char *) &msg, sizeof(msg), 0);

      	int ret2 = recv(cl_sock_fd, (char *) recv_buffer, sizeof(recv_buffer), 0);
	rx_msg = (msg_t *) recv_buffer;
      
      	fare = rx_msg->msg_int;
      	printf("%d",fare);
    
      	if(m1 == m2)
      	{
      		if(d1 == d2)
        	{
            	total = 1;
        	}
        	else
        	{
            	total = d2 - d1;
        	}
    	}
    	else
    	{
        	total = daysPerMonth[m1] - d1;
        	for(int i = m1 + 1; i < m2; i++)        // Days remaining between dates (excluding last month)
        	{
                    total += daysPerMonth[i];
        	}
        	total += d2;  
    	}

      	if(total == 0)
		return fare;
	else
	{
      		fare = fare * total;
		return fare;
	}
}

  
/*Function to generate the hotel reservation bill */

void generate_ticket(msg_t* recv, msg_t recv1)
{
  	char name[32];
  	char age_char[10];
  	char phone_no_char[12];
	int age;
	long phone_no;
      
      
      	int total_fare;
	int room_no = recv->msg_int;
  	char room_type[15];
  	char check_in[10];
  	char check_out[10];
	char username[32];
  
  	printf("\nEnter name : ");
  	scanf("%s",name);
  	
	printf("\nEnter age : ");
      	while(1){
		scanf("%s",age_char);
		age = atoi(age_char);
		if(age >= 18){
			break;
		}
		else{
			printf("Sorry booking cannot be initiated if under 18.\nPlease enter a valid age: ");
		}
	}
	printf("\nEnter phone number :");
      	while(1){
      	scanf("%s",phone_no_char);
		phone_no = atol(phone_no_char);

		//Checks for validity of phone number
		if(phone_no > 6000000000 && phone_no < 9999999999){
			break;
		}
		else{
			printf("\nInvalid phone number.\nPlease enter a valid phone number: ");
		}
	}

  	strcpy(room_type,recv1.msg_data);
  	strcpy(check_in,recv1.msg_add_data);
  	strcpy(check_out,recv1.msg_add1_data);
      	strcpy(username,recv1.msg_add2_data);
      
      	total_fare = calculate_fare(room_type,check_in,check_out);
	

  	printf("\n\n=============================================================\n");
  	printf("\n\t\tHOTEL BILL\n");
 	printf("\n\n=============================================================\n");
  	printf("\n\t\tName           : %s\n",name);
  	printf("\n\t\tAge            : %d\n",age);
  	printf("\n\t\tPhone Number   : %ld\n",phone_no);
	printf("\n\t\tRoom Number    : %d\n",room_no);
  	printf("\n\t\tRoom Type      : %s room\n",room_type);
  	printf("\n\t\tCheck In Date  : %s\n",check_in);
  	printf("\n\t\tCheck Out Date : %s\n",check_out);
	printf("\n\t\tTotal cost : Rs.%d\n",total_fare);
 	printf("\n\n=============================================================\n");
 
}



/*Checks if check is greater than check out*/

int checkin_checkout(char* check_in, char* check_out)
{
	char date1[10];
	char date2[10];
 	char delimiter[] = "-";

	strcpy(date1,check_in);
	strcpy(date2,check_out);

	int d1 = atoi(strtok(date1,delimiter));
      	int m1 = atoi(strtok(NULL,delimiter));

      	int d2 = atoi(strtok(date2,delimiter));
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

/*Authenticates user and cancels the room booked*/

int cancel_room(void)
{
  
  	int ret, room_no; 
  	msg_t msg, *rx_msg;
  	char pwd[10];
	char uname[10];

	printf("\n\nTo Cancel your booking enter your username : ");
  	scanf("%s",uname);

  	printf("\n\nTo Cancel your booking enter your password : ");
  	scanf("%s",pwd);
        
	ret = validate_user_name_passwd(uname, pwd);

        ret = proces_login_msg(uname, pwd);

        if (ret == MSG_FAIL)
        {
        	printf("\nUser Authentication Failed \n");
          	
        }
        else if (ret == MSG_INVALID)
        {
		printf("\nUser Authentication Failed \n");
          	
        }
        else if (ret == MSG_SUCCESS)
       	{
          	printf("\nUser Authentication Success \n");

		printf("\nEnter your room_no: ");
  		scanf("%d",&room_no);

    		memset(msg.msg_data, '\0', sizeof(msg.msg_data));
  	
  		msg.msg_type = MSG_CANCEL_ROOMS;
  		msg.msg_int = room_no;
  		strncpy(msg.msg_data, uname, strlen(uname));

  		ret = send(cl_sock_fd, (char *) &msg, sizeof(msg), 0);


  		ret = recv(cl_sock_fd, (char *) recv_buffer, sizeof(recv_buffer), 0);



  		if (ret == 0)
  		{

    			handle_server_close(cl_sock_fd);
			return -2;
  		}

  		rx_msg = (msg_t *) recv_buffer;

  		if (rx_msg->msg_type == MSG_CANCELLED)
  		{
      			printf("\nYour Reservation is cancelled\n");
       		
		}
		else if(rx_msg->msg_type == MSG_NOT_CANCELLED)
  		{
      			printf("\nThe Cancellation request is invalid\n");
       		
		}
	}
	return 0;
}


/*Socket for server closes*/

void handle_server_close (int sockfd)
{

  	close(sockfd);

}

/*Socket for client closes*/
void handle_client_close(int sock_fd)
{

  	close(sock_fd);

}

