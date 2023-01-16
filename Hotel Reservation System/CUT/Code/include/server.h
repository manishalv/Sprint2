/*
 *
 *  File: server.h
 *
 *  Description: Contains macros, fuction declarations and room type structures 
 *  
 *
*/


/*----Macros----*/ 
#define MAX_CLIENTS 2
#define MAX_USERS   5
#define PORT 4444
#define SEND_BUFFER_SIZE 1024
#define RECV_BUFFER_SIZE 1024
#define USER_FOUND  1
#define USER_NOT_FOUND 2
#define USER_PASSWD_MATCH_FOUND 3
#define USER_PASSWD_MATCH_NOT_FOUND 4

/*User information structure*/
typedef struct user_info
{
  char uname[32];
  char pwd[8];
  struct user_info *next;

} user_info_t;

/*Room type structures*/

typedef struct single_room
{
  int room_no;
  bool available;
  char check_in[10];
  char check_out[10];
  char uname[32];
  
  
} single_t[4];

typedef struct double_room
{
  int room_no;
  bool available;
  char check_in[10];
  char check_out[10];
  char uname[32];

} double_t[4];

typedef struct deluxe_room
{
  int room_no;
  bool available;
  char check_in[10];
  char check_out[10];
  char uname[32];
  
} deluxe_t[4];

typedef struct suite_room
{
  int room_no;
  bool available;
  char check_in[10];
  char check_out[10];
  char uname[32];
  
} suite_t[4];


/*Function declaration*/

user_info_t* add_user_to_list(char *uname, char *pwd);
int search_user_pwd_in_list(char *uname, char *pwd);
void handle_login_msg (int sockfd, msg_t *msg);
void handle_uname_passwd_msg (int sockfd, msg_t *msg);
void handle_view_room(int sockfd, msg_t *msg);
int checking_dates(char* date1, char* date2);
void handle_book_room(int sockfd, msg_t *msg);
void handle_fare(int sockfd, msg_t *msg);
void handle_cancel_room(int sockfd, msg_t *msg);
void setup_message_handlers(void);
void handle_client_close(int sock_fd);
void process_client_messages(int sockfd, char *recv_buffer);
void* client_handler(void *client_sock_fd);



/*To count the number of clients connected to the server*/
int cl_count = 0;

/*Message action structure*/
msg_action_t  msg_actions[MSG_MAX];

/*User information structure*/
user_info_t  *user_list_start = NULL;



