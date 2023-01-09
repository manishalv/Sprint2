#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"

#define PORT 4444
#define SEND_BUFFER_SIZE 512
#define RECV_BUFFER_SIZE 512

#define MSG_SUCCESS  1
#define MSG_FAIL     2
#define MSG_INVALID  3
#define MAX_INPUT_LEN   32


int cl_sock_fd;

char send_buffer[SEND_BUFFER_SIZE];
char recv_buffer[RECV_BUFFER_SIZE];


void proces_booking_info(char *uname, char *room_type, char *check_in_date,char *check_out_date)
{
  int ret; 
  booking_info_t msg, *rx_msg;
  
  memset(msg.msg_data, '\0', sizeof(msg.msg_data));
  memset(msg.msg_add_data, '\0', sizeof(msg.msg_add_data));
  memset(msg.msg_add1_data, '\0', sizeof(msg.msg_add_data));
  memset(msg.msg_add2_data, '\0', sizeof(msg.msg_add_data));

  msg.msg_type = MSG_BOOK_ROOM ;
  strncpy(msg.msg_data, uname, strlen(uname));
  strncpy(msg.msg_add_data, room_type, strlen(room_type));
  strncpy(msg.msg_add1_data, check_in_date, strlen(check_in_date));
  strncpy(msg.msg_add2_data, check_out_date, strlen(check_out_date));

  printf("Messaage Created \n");
  printf("  Type: %d \n", booking_info.msg_type);
  printf("  Name: %s len = %ld \n", booking_info.msg_data, strlen(uname));
  printf("  Room Type: %s len = %ld\n", booking_info.msg_add_data,strlen(room_type));
  printf("  check_in_date: %s len = %ld\n", booking_info.msg_add1_data,strlen(check_in_date));
  printf("  check_out_date: %s len = %ld\n", booking_info.msg_add2_data,strlen(check_out_date));

  ret = send(cl_sock_fd, (char *) &msg, sizeof(msg), 0);

  printf("Sent bytes = %d \n", ret);


  ret = recv(cl_sock_fd, (char *) recv_buffer, sizeof(recv_buffer), 0);

  printf("Received %d bytes from Server \n", ret);

  if (ret == 0)
  {
    printf("%s():%d Server has closed on socket fd = %d \n",
        __FUNCTION__, __LINE__, cl_sock_fd);

    handle_server_close(cl_sock_fd);

    return -2;
  }

  rx_msg = (booking_info_t *) recv_buffer;

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

void reserve_room(char *uname)
{
        
	char room_type[MAX_INPUT_LEN];
        char check_in_date[MAX_INPUT_LEN];
        char check_out_date[MAX_INPUT_LEN];

	memset(room_type, '\0', sizeof(room_type));
    	memset(check_in_date, '\0', sizeof(check_in_date));
	memset(check_out_date, '\0', sizeof(check_out_date));

	printf("\nEnter room type :  ");
	scanf("%s",room_type);
	printf("\nEnter check in date :  ");
	scanf("%s",check_in_date);
	printf("\nEnter check out date :  ");
	scanf("%s",check_out_date);


	ret = proces_booking_info(uname, room_type, check_in_date, check_out_date);

        if (ret == MSG_FAIL)
        {
          printf(" Room not Available \n");
          break;
        }
        
        else if (ret == MSG_SUCCESS)
        {
          printf("Room  Available \n");
          break;
        }

}


void view_rooms()
{
  int ret; 
  view_room_t msg, *rx_msg;
 
  msg.msg_type = MSG_VIEW_ROOM;
  
  ret = send(cl_sock_fd, (char *) &msg, sizeof(msg), 0);

  printf("Sent bytes = %d \n", ret);
  
  
}

  
  	

void display_hotel_menu(char *uname)
{
  int option;

  while(1)
  {
    printf(" Welcome to Hotel Reservation Menu \n");
    printf("===================================\n");

    printf("1. View Rooms and Services\n");
    printf("2. Reserve a room \n");
    printf("3. Cancel a reservation \n");
    printf("4. Exit to main menu \n");
    
    printf("Enter option: ");
    scanf("%d", &option);

    switch(option)
    {
      case 1:view_rooms();
             break;

      case 2:reserve_room(uname);
             break;

      case 3:cancel_room();
             break;

      case 4:return;
             
      default:
              continue;
    }
  }
}



int validate_user_name_passwd(char *uname, char *pwd)
{
  if ((strlen(uname) > MAX_INPUT_LEN) || (strlen(uname) < 4))
  {
    printf("Invalid username, length must be between 4 and 32 \n");
    return -1;
  }
  else if ((strlen(pwd) > 6) || (strlen(pwd) < 4)) 
  {
    printf("Invalid password, length must be between 4 and 6 \n");
    return -1;
  }

  return 0;
}

int get_and_validate_input(char *input1, char *input2)
{
  printf("Enter User Name(Max 32 characters): \n");

  scanf("%s", input1);
  printf("Enter User Passwd(Max 6 chars): \n");

  scanf("%s", input2);

  return validate_user_name_passwd(input1, input2); 
}


void handle_server_close (int sockfd)
{

  close(sockfd);

}

int proces_login_msg(char *uname, char *pwd)
{
  int ret; 
  msg_t msg, *rx_msg;
  
  memset(msg.msg_data, '\0', sizeof(msg.msg_data));
  memset(msg.msg_add_data, '\0', sizeof(msg.msg_add_data));

  msg.msg_type = MSG_LOGIN;
  strncpy(msg.msg_data, uname, strlen(uname));
  strncpy(msg.msg_add_data, pwd, strlen(pwd));

  printf("Messaage Created \n");
  printf("  Type: %d \n", msg.msg_type);
  printf("  Data: %s len = %ld \n", msg.msg_data, strlen(uname));
  printf("  Additional Data: %s len = %ld\n", msg.msg_add_data,strlen(pwd));

  ret = send(cl_sock_fd, (char *) &msg, sizeof(msg), 0);

  printf("Sent bytes = %d \n", ret);


  ret = recv(cl_sock_fd, (char *) recv_buffer, sizeof(recv_buffer), 0);

  printf("Received %d bytes from Server \n", ret);


  if (ret == 0)
  {
    printf("%s():%d Server has closed on socket fd = %d \n",
        __FUNCTION__, __LINE__, cl_sock_fd);

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

  printf("Message Created \n");
  printf("  Type: %d \n", msg.msg_type);
  printf("  Data: %s len = %ld \n", msg.msg_data, strlen(uname));
  printf("  Additional Data: %s len = %ld\n", msg.msg_add_data,strlen(pwd));

  ret = send(cl_sock_fd, (char *) &msg, sizeof(msg), 0);

  printf("Sent bytes = %d \n", ret);


  ret = recv(cl_sock_fd, (char *) recv_buffer, sizeof(recv_buffer), 0);

  printf("Received %d bytes from Server \n", ret);


  if (ret == 0)
  {
    printf("%s():%d Server has closed on socket fd = %d \n",
        __FUNCTION__, __LINE__, cl_sock_fd);

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


  cl_sock_fd = socket(AF_INET,SOCK_STREAM, 0);

  if (cl_sock_fd < 0) 
  {
    printf("Error in connection.\n");
    exit(1);
  }

  printf("Client Socket is created.\n");

  memset(send_buffer, '\0', sizeof(send_buffer));
  memset(recv_buffer, '\0', sizeof(recv_buffer));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);

  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  ret = connect(cl_sock_fd, (struct sockaddr*)&server_addr,
      sizeof(server_addr));

  if (ret < 0) 
  {
    printf("Error in connection.\n");
    exit(1);
  }

  printf("Connected to Server.\n");

  while (1) 
  {
    printf("Menu Interface\n");
    printf("==============\n");

    printf("1. New User \n");
    printf("2. Existing User \n");
    printf("3. Logout \n");

    scanf("%d", &option);
  
    memset(menu_data_string1, '\0', sizeof(menu_data_string1));
    memset(menu_data_string2, '\0', sizeof(menu_data_string2));
    

    printf("S1: %s \n", menu_data_string1);
    printf("S2: %s \n", menu_data_string2);
    switch(option)
    {
      case 1:

        ret = get_and_validate_input(menu_data_string1, menu_data_string2);

        if (ret == -1)
        {
          printf("Invalid input format \n");
          break;
        }

        ret = proces_uname_pwd_msg(menu_data_string1, menu_data_string2);

        if (ret == MSG_FAIL)
        {
          printf("User Authentication Failed \n");
          break;
        }
        else if (ret == MSG_INVALID)
        {
          printf("User Authentication Failed \n");
          break;

        }
        else if (ret == MSG_SUCCESS)
        {
          printf("User Authentication Success \n");

          display_hotel_menu(menu_data_string1);
        }

        break;

      case 2:

        ret = get_and_validate_input(menu_data_string1, menu_data_string2);

        if (ret == -1)
        {
          printf("Invalid input format \n");
          break;
        }

        ret = proces_login_msg(menu_data_string1, menu_data_string2);

        if (ret == MSG_FAIL)
        {
          printf("User Authentication Failed \n");
          break;
        }
        else if (ret == MSG_INVALID)
        {
          printf("User Authentication Failed \n");
          break;

        }
        else if (ret == MSG_SUCCESS)
        {
          printf("User Authentication Success \n");

          display_hotel_menu(menu_data_string1);
        }

        break;
      
      case 3:
        exit(0);

      default:
        printf("Invalid Option .. \n");
        break;
    }

  }

  return 0;
}

