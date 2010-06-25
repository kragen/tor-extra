/*
 *  The Tor TCP DNS Daemon
 *
 *  Copyright (c) Collin R. Mulliner <collin(AT)mulliner.org>
 *  Copyright (c) 2010, The Tor Project, Inc.
 *
 *  http://www.mulliner.org/collin/ttdnsd.php
 *
 */

#define DEBUG 0

// number of parallel connected tcp peers
#define MAX_PEERS 1
// request timeout
#define MAX_TIME 3 /* QUASIBUG 3 seconds is too short! */
// number of trys per request (not used so far)
#define MAX_TRY 1
// maximal number of nameservers
#define MAX_NAMESERVERS 32
// request queue size (use a prime number for hashing)
#define MAX_REQUESTS 499
// 199, 1009
// max line size for configuration processing
#define MAX_LINE_SIZE 1025

// Magic numbers
#define RECV_BUF_SIZE 1502

#define NOBODY 65534
#define NOGROUP 65534
#define DEFAULT_BIND_PORT 53
#define DEFAULT_BIND_IP "127.0.0.1"
#define DEFAULT_RESOLVERS "ttdnsd.conf"
#define DEFAULT_LOG "ttdnsd.log"
#define DEFAULT_CHROOT "/var/run/ttdnsd"
#define DEFAULT_TSOCKS_CONF "tsocks.conf"
#define TSOCKS_CONF_ENV "TSOCKS_CONF_FILE"
#define DEFAULT_PID_FILE DEFAULT_CHROOT"/ttdnsd.pid"

#define HELP_STR ""\
    "syntax: ttdnsd [bpfPCcdl]\n"\
    "\t-b\t<local ip>\tlocal IP to bind to\n"\
    "\t-p\t<local port>\tbind to port\n"\
    "\t-f\t<resolvers>\tfilename to read resolver IP(s) from\n"\
    "\t-P\t<PID file>\tfile to store process ID - pre-chroot\n"\
    "\t-C\t<chroot dir>\tchroot(2) to <chroot dir>\n"\
    "\t-c\t\t\tDON'T chroot(2) to /var/run/ttdnsd\n"\
    "\t-d\t\t\tDEBUG (don't fork and print debug)\n"\
    "\t-l\t\t\twrite debug log to: " DEFAULT_LOG "\n\n"\
    "export TSOCKS_CONF_FILE to point to config file inside the chroot\n"\
    "\n"

typedef enum {
    DEAD = 0,
    CONNECTING,
    CONNECTING2,
    CONNECTED
} CON_STATE;

typedef enum {
    WAITING = 0,
    SENT
} REQ_STATE;

struct request_t {
    struct sockaddr_in a; /* client’s IP/port */
    socklen_t al;
    unsigned char b[1502]; /**< request buffer */
    int bl; /**< bytes in request buffer */
    uint id; /**< dns request id; must be nonnegative */
    int rid; /**< real dns request id */
    REQ_STATE active; /**< 1=sent, 0=waiting for tcp to become connected */
    time_t timeout; /**< timeout of request */
};

struct peer_t
{
    struct sockaddr_in tcp;
    int tcp_fd;
    time_t timeout;
    CON_STATE con; /**< connection state 0=dead, 1=connecting..., 3=connected */
    unsigned char b[1502]; /**< receive buffer */
    int bl; /**< bytes in receive buffer */ // bl? Why don't we call this bytes_in_recv_buf or something meaningful?
};


int request_find(uint id);
int peer_connect(struct peer_t *p, int ns);
int peer_connected(uint peer);
int peer_sendreq(struct peer_t *p, int req);
int peer_readres(uint peer);
void peer_handleoutstanding(uint peer);
struct peer_t *peer_select(void);
int ns_select(void);
int request_add(struct request_t *r);
int server(char *bind_ip, int bind_port);
int load_nameservers(char *filename);

