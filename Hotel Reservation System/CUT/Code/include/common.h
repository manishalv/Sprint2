
#define MAX_DATA_LEN  128

typedef enum 
{
  MSG_UNAME_PASSWD = 1,
  MSG_AUTH_SUCCESS = 2,
  MSG_AUTH_FAIL = 3,
  MSG_LOGIN = 4,
  MSG_MAX = 5,
  MSG_BOOK_ROOM = 6,
  MSG_VIEW_ROOM =7,
  MSG_CANCEL_ROOM_SUCCESS=8,
  MSG_VIEW_HOTEL_ROOMS_SUCCESS=9,
  MSG_BOOKING_SUCCESS=10

} msg_type_e;


typedef struct 
{
  msg_type_e  msg_type; 
  char        msg_data[MAX_DATA_LEN];
  char        msg_add_data[MAX_DATA_LEN];

} msg_t;

typedef struct 
{
  void        (*msg_action_function) (int, msg_t *);

} msg_action_t;

typedef struct 
{
  msg_type_e  msg_type; 
  char        msg_data[MAX_DATA_LEN];
  char        msg_add_data[MAX_DATA_LEN];
  char        msg_add1_data[MAX_DATA_LEN];
  char        msg_add2_data[MAX_DATA_LEN];

} booking_info_t;

typedef struct 
{
  msg_type_e  msg_type; 
  char        msg_data[MAX_DATA_LEN];
  int        msg_add_data;

} cancel_info_t;

typedef struct 
{
  msg_type_e  msg_type; 
  char        msg_data[MAX_DATA_LEN];

} view_hotel_info_t;

