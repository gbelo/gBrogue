#ifdef BROGUE_WEB

#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/socket.h>
#include "platform.h"

#define SERVER_SOCKET "server-socket"
#define CLIENT_SOCKET "client-socket"

#define OUTPUT_SIZE             10
#define EVENT_SIZE              100
#define MAX_INPUT_SIZE          5
#define MOUSE_INPUT_SIZE        4
#define KEY_INPUT_SIZE          4
#define OUTPUT_BUFFER_SIZE      1000

//Custom events
#define REFRESH_SCREEN          50

#define PERMIT_CLIENT_MOUSELOOK 1

enum StatusTypes {
    DEEPEST_LEVEL_STATUS,
    GOLD_STATUS,
    SEED_STATUS,
    EASY_MODE_STATUS,
    STATUS_TYPES_NUMBER
};

extern playerCharacter rogue;
static struct sockaddr_un addr_write;
static int wfd, rfd;

static FILE *logfile;
static char output_buffer[OUTPUT_BUFFER_SIZE];
static int output_buffer_pos = 0;
static int refresh_screen_only = 0;

static void open_logfile();
static void close_logfile();
static void write_to_log(const char *msg);
static void setup_sockets();
static int read_from_socket(char *buf, int size);
static void write_to_socket(char *buf, int size);
static void flush_output_buffer();

static void gameLoop()
{
  open_logfile();
  write_to_log("Logfile started\n");

  setup_sockets();

  rogueMain();

  close_logfile();
}


static void open_logfile() {
  logfile = fopen ("brogue-web.txt", "a");
  if (logfile == NULL) {
    fprintf(stderr, "Logfile not created, errno = %d\n", errno);
  }
}

static void close_logfile() {
  fclose(logfile);
}

static void write_to_log(const char *msg) {
  fprintf(logfile, "%s", msg);
  fflush(logfile);
}

static void setup_sockets() {

  struct sockaddr_un addr_read;

  // Read socket (from external)

  rfd = socket(AF_UNIX, SOCK_DGRAM, 0);
  remove(SERVER_SOCKET);

  memset(&addr_read, 0, sizeof(struct sockaddr_un));
  addr_read.sun_family = AF_UNIX;
  strncpy(addr_read.sun_path, SERVER_SOCKET, sizeof(addr_read.sun_path) - 1);

  bind(rfd, (struct sockaddr *) &addr_read, sizeof(struct sockaddr_un));

  //non-blocking may not be desirable
  //fcntl(socket, F_SETFL, O_NONBLOCK);

  // Write socket (to external)

  wfd = socket(AF_UNIX, SOCK_DGRAM, 0);

  memset(&addr_write, 0, sizeof(struct sockaddr_un));
  addr_write.sun_family = AF_UNIX;
  strncpy(addr_write.sun_path, CLIENT_SOCKET, sizeof(addr_write.sun_path) - 1);
}

//Returns -1 if no data available (if in non-blocking mode)
int read_from_socket(char *buf, int size) {

  int bytes_received = recvfrom(rfd, buf, size, 0, NULL, NULL);
  return bytes_received;
}

static void flush_output_buffer() {

  //char msg[80];
  //snprintf(msg, 80, "Flushing at %i\n", output_buffer_pos);
  //write_to_log(msg);

  int no_bytes_sent;
  no_bytes_sent = sendto(wfd, output_buffer, output_buffer_pos, 0, (struct sockaddr *) &addr_write, sizeof(struct sockaddr_un));
  if (no_bytes_sent != output_buffer_pos) {
    char msg[80];
    snprintf(msg, 80, "Sent %ld bytes only %s\n", (long) no_bytes_sent, strerror(errno));
    write_to_log(msg);
    sleep(1);
  }

  output_buffer_pos = 0;
}

static void write_to_socket(char *buf, int size) {

  if(output_buffer_pos + size > OUTPUT_BUFFER_SIZE) {
    flush_output_buffer();
  }

  //memcpy(output_buffer + output_buffer_pos, buf, size);
  int i;
  for(i = 0; i < size; i++) {
    output_buffer[i + output_buffer_pos] = buf[i];
  }

  output_buffer_pos += size;
}

static void web_plotChar(uchar inputChar,
			  short xLoc, short yLoc,
			  short foreRed, short foreGreen, short foreBlue,
			  short backRed, short backGreen, short backBlue) {

    // just pack up the output and ship it off to the webserver
    char outputBuffer[OUTPUT_SIZE];
    
    char firstCharByte = inputChar >> 8 & 0xff;
    char secondCharByte = inputChar;
    
    outputBuffer[0] = (char) xLoc;
    outputBuffer[1] = (char) yLoc;
    outputBuffer[2] = firstCharByte;
    outputBuffer[3] = secondCharByte;
    outputBuffer[4] = (char) foreRed * 255 / 100;
    outputBuffer[5] = (char) foreGreen * 255 / 100;
    outputBuffer[6] = (char) foreBlue * 255 / 100;
    outputBuffer[7] = (char) backRed * 255 / 100;
    outputBuffer[8] = (char) backGreen * 255 / 100;
    outputBuffer[9] = (char) backBlue * 255 / 100;
    
    write_to_socket(outputBuffer, OUTPUT_SIZE);
}

static void sendStatusUpdate() {
    
    char statusOutputBuffer[OUTPUT_SIZE];
    
    unsigned long statusValues[STATUS_TYPES_NUMBER]; 
    statusValues[DEEPEST_LEVEL_STATUS] = rogue.deepestLevel;
    statusValues[GOLD_STATUS] = rogue.gold;
    statusValues[SEED_STATUS] = rogue.seed;
    statusValues[EASY_MODE_STATUS] = rogue.easyMode;
    
    int i;
    int j;
    for (i = 0; i < STATUS_TYPES_NUMBER; i++){
        
        // Coordinates of (255, 255) will let the server and client know that this is a status update rather than a cell update 
        statusOutputBuffer[0] = (char)255;
        statusOutputBuffer[1] = (char)255;
        
        // The status type
        statusOutputBuffer[2] = i;
        
        // I am just going to explicitly send the status big-endian so we can be consistent on the client and server
        statusOutputBuffer[3] = statusValues[i] >> 24 & 0xff;
        statusOutputBuffer[4] = statusValues[i] >> 16 & 0xff;
        statusOutputBuffer[5] = statusValues[i] >> 8 & 0xff;
        statusOutputBuffer[6] = statusValues[i];
        
        // The rest is filler so we keep consistent output size
        for (j = 7; j < OUTPUT_SIZE; j++){
            statusOutputBuffer[j] = 0;
        }
        
        write_to_socket(statusOutputBuffer, OUTPUT_SIZE);
    }
}

// This function is used both for checking input and pausing
static boolean web_pauseForMilliseconds(short milliseconds)
{
  usleep(milliseconds * 1000);

  //Poll for input data

  fd_set input;
  FD_ZERO(&input);
  FD_SET(rfd, &input);

  struct timeval timeout;
  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;

  return select(rfd + 1, &input, NULL, NULL, &timeout);
}

static void web_nextKeyOrMouseEvent(rogueEvent *returnEvent, boolean textInput, boolean colorsDance)
{
    // because we will halt execution until we get more input, we definitely cannot have any dancing colors from the server side.
    colorsDance = false;
    
    // We must avoid the main menu, so we spawn this process with noMenu, and quit instead of going to the menu
    if (noMenu && rogue.nextGame == NG_NOTHING) rogue.nextGame = NG_QUIT;

    // Send a status update of game variables we want on the client
    if(!refresh_screen_only) {
      sendStatusUpdate();
    }
    refresh_screen_only = 0;

    // Flush output buffer
    flush_output_buffer();

    char inputBuffer[MAX_INPUT_SIZE];

    read_from_socket(inputBuffer, MAX_INPUT_SIZE);
    returnEvent->eventType = inputBuffer[0];

	if(!PERMIT_CLIENT_MOUSELOOK && returnEvent->eventType == MOUSE_ENTERED_CELL)
    	return;

    if (returnEvent->eventType == REFRESH_SCREEN) {

      //Custom event type - brogue should ignore, not status update
      refreshScreen();
      //Don't send a status update if this was only a screen refresh (may be sent by observer)
      refresh_screen_only = 1;
      return;
    }

    if (returnEvent->eventType == KEYSTROKE) {

        unsigned short keyCharacter = inputBuffer[1] << 8 | inputBuffer[2];
        
        returnEvent->param1 = keyCharacter;  //key character
        returnEvent->controlKey = inputBuffer[3];
        returnEvent->shiftKey = inputBuffer[4];
    }
    else // it is a mouseEvent
    {    
        fread(inputBuffer, sizeof(char), MOUSE_INPUT_SIZE, stdin);
        returnEvent->param1 = inputBuffer[1];  //x coord
        returnEvent->param2 = inputBuffer[2];  //y coord
        returnEvent->controlKey = inputBuffer[3];
        returnEvent->shiftKey = inputBuffer[4];
    }
    
}

static void web_remap(const char *input_name, const char *output_name) {
    // Not needed
}

static boolean modifier_held(int modifier) {
    // Not needed, I am passing in the modifiers directly with the event data
	return 0;
}

static void notify_event(short eventId, int data1, int data2, const char *str1, const char *str2) {

  char statusOutputBuffer[EVENT_SIZE];
  char msg[100];

  snprintf(msg, 100, "event: %i d1: %i d2: %i s1: %s s2: %s\n", eventId, data1, data2, str1, str2);
  write_to_log(msg);

  // Coordinates of (254, 254) will let the server and client know that this is a event notification update rather than a cell update
  statusOutputBuffer[0] = (char)254;
  statusOutputBuffer[1] = (char)254;

  // The event id
  statusOutputBuffer[2] = eventId;

  // I am just going to explicitly send the status big-endian so we can be consistent on the client and server
  statusOutputBuffer[3] = data1 >> 24 & 0xff;
  statusOutputBuffer[4] = data1 >> 16 & 0xff;
  statusOutputBuffer[5] = data1 >> 8 & 0xff;
  statusOutputBuffer[6] = data1;
  statusOutputBuffer[7] = rogue.depthLevel >> 8 & 0xff;
  statusOutputBuffer[8] = rogue.depthLevel;
  statusOutputBuffer[9] = rogue.easyMode >> 8 & 0xff;
  statusOutputBuffer[10] = rogue.easyMode;
  statusOutputBuffer[11] = rogue.gold >> 24 & 0xff;
  statusOutputBuffer[12] = rogue.gold >> 16 & 0xff;
  statusOutputBuffer[13] = rogue.gold >> 8 & 0xff;
  statusOutputBuffer[14] = rogue.gold;
  statusOutputBuffer[15] = rogue.seed >> 24 & 0xff;
  statusOutputBuffer[16] = rogue.seed >> 16 & 0xff;
  statusOutputBuffer[17] = rogue.seed >> 8 & 0xff;
  statusOutputBuffer[18] = rogue.seed;

  // Copy str1 (death message)

  int j;
  int msg1_start = 19;
  int msg1_end = 70;
  for (j = msg1_start; j < msg1_end; j++){
      statusOutputBuffer[j] = str1[j - msg1_start];
      if(!str1[j - msg1_start])
        break;
  }

  int msg2_end = EVENT_SIZE;
  for (j = msg1_end; j < msg2_end; j++){
      statusOutputBuffer[j] = str2[j - msg1_end];
      if(!str2[j - msg1_end])
        break;
  }

  write_to_socket(statusOutputBuffer, EVENT_SIZE);
  flush_output_buffer();
}

struct brogueConsole webConsole = {
	gameLoop,
	web_pauseForMilliseconds,
	web_nextKeyOrMouseEvent,
	web_plotChar,
	web_remap,
	modifier_held,
	notify_event
};

#endif
