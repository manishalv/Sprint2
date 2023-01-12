#define MAX_CLIENTS 1
#define MAX_USERS   5
#define PORT 4444
#define SEND_BUFFER_SIZE 512
#define RECV_BUFFER_SIZE 512
#define USER_FOUND  1
#define USER_NOT_FOUND 2
#define USER_PASSWD_MATCH_FOUND 3
#define USER_PASSWD_MATCH_NOT_FOUND 4





typedef struct user_info
{
  char uname[32];
  char pwd[8];
  struct user_info *next;

} user_info_t;

typedef struct single_room
{
  int room_no;
  bool available;
  char check_in[10];
  char check_out[10];
  char username[32];
  int price;
  
} single_t[4];

typedef struct double_room
{
  int room_no;
  bool available;
  char check_in[10];
  char check_out[10];
  int price;
  char username[32];

} double_t[4];

typedef struct deluxe_room
{
  int room_no;
  bool available;
  char check_in[10];
  char check_out[10];
  int price;
  char username[32];
  
} deluxe_t[4];

typedef struct suite_room
{
  int room_no;
  bool available;
  char check_in[10];
  char check_out[10];
  int price;
  char username[32];
  
} suite_t[4];



