/*
 *
 *  File: common.h
 *
 *  Description: Contains the Macros , message structure, and fuction declarations common to both client and server  
 *  
 *
*/
#define MAX_DATA_LEN  128

#define PORT 4444
#define SEND_BUFFER_SIZE 1024
#define RECV_BUFFER_SIZE 1024

#define MSG_SUCCESS  1
#define MSG_FAIL     2
#define MSG_INVALID  3
#define DISP_ROOM    1
#define BOOK_ROOM    2
#define CANCEL_ROOM  3
#define EXIT         4
#define DATE_VALID   1
#define FORMAT_INVALID 0
#define INVALID_MONTH 2
#define INVALID_DATE 3
#define MAX_INPUT_LEN   32


int cl_sock_fd;

char send_buffer[SEND_BUFFER_SIZE];
char recv_buffer[RECV_BUFFER_SIZE];
void handle_server_close (int sockfd);



typedef enum 
{
  MSG_UNAME_PASSWD = 1,
  MSG_AUTH_SUCCESS = 2,
  MSG_AUTH_FAIL = 3,
  MSG_LOGIN = 4,
  MSG_MAX = 5,
  MSG_VIEW_ROOMS = 6,
  MSG_BOOK_ROOMS = 7,
  MSG_ROOM_AVAIL = 8,
  MSG_ROOM_NOTAVAIL = 9,
  MSG_SERVICES=10,
  MSG_CANCEL_ROOMS= 11,
  MSG_CANCELLED = 12,
  MSG_NOT_CANCELLED = 13,
  MSG_GET_FARE = 14

} msg_type_e;


typedef struct 
{
  msg_type_e  msg_type;
  int         msg_int;
  char        msg_data[MAX_DATA_LEN];
  char        msg_add_data[MAX_DATA_LEN]; 
  char        msg_add1_data[MAX_DATA_LEN];
  char        msg_add2_data[MAX_DATA_LEN];
  
} msg_t;

typedef struct 
{
  void        (*msg_action_function) (int, msg_t *);

} msg_action_t;

int get_and_validate_input(char *input1, char *input2);
int validate_user_name_passwd(char *uname, char *pwd);
int proces_uname_pwd_msg(char *uname, char *pwd);
int proces_login_msg(char *uname, char *pwd);
void display_hotel_menu(void);
void display_room_details(void);
int book_room(void);
int validate_date(char* date);
int calculate_fare(char* room_type, char* check_in,char* check_out);
void generate_ticket(msg_t* recv, msg_t recv1);
int checkin_checkout(char* check_in, char* check_out);
int cancel_room(void);
void handle_server_close (int sockfd);
void handle_client_close(int sock_fd);
