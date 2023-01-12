
#define MAX_DATA_LEN  128

#define PORT 4444
#define SEND_BUFFER_SIZE 512
#define RECV_BUFFER_SIZE 512

#define MSG_SUCCESS  1
#define MSG_FAIL     2
#define MSG_INVALID  3
#define DISP_ROOM    1
#define BOOK_ROOM    2
#define EXIT         4
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
  	MSG_SERVICES=10

} msg_type_e;

typedef struct
{
  	char room_type[32];
  	float fare;
} room_t;



typedef struct 
{
  	char services_type[20];
  	int price;

}services_t;



typedef struct 
{
  	msg_type_e  msg_type; 
 	int         msg_int;
  	char        msg_data[MAX_DATA_LEN];
  	char        msg_add_data[MAX_DATA_LEN]; 
  	char        msg_add1_data[MAX_DATA_LEN];
  
} msg_t;

typedef struct 
{
  	void        (*msg_action_function) (int, msg_t *);

} msg_action_t;
