
#ifndef Luna_h
#define Luna_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <pcap.h>

#include "LinkedList.h"

typedef struct Luna {
	char *_user;		/* user name */
	u_long _source;		/* source ip address */
	u_long _destination;	/* destination ip address */
	char *_device;		/* device name */
	int _datagramSize;	/* datagram size */
	short _fork;		/* work in background */
	short _maxErrors;	/* maximum allowed errors */
	short _maxTimeLapse;	/* maximum lapse before injecting packets */
	short _verbose;		/* adjust noise level */
} Luna;


/* defines */
#define LUNA_VERSION "0.0.1gamma"
#define LUNA_HAIKU_1 "\n\tWith searching comes loss\n\tand the presence of absence:\n\tfile not found.\n"
#define LUNA_HAIKU_2 "\n\tStay the patient course,\n\tof little worth is your ire.\n\tThe network is down.\n"
#define LUNA_HAIKU_3 "\n\tOut of memory.\n\tWe wish to hold the whole sky,\n\tbut never will.\n"
#define LUNA_HAIKU_4 "\n\tYesterday it worked.\n\tToday it is not working.\n\tSoftware is like that.\n"
#define LUNA_HAIKU_5 "\n\tThe host you seek\n\tcannot be located but\n\tendless others exist.\n"
#define LUNA_HAIKU_6 "\n\tThree things are certain:\n\tDeath, taxes, and incorrect arguments.\n\tGuess which has occurred.\n"
#define LUNA_HAIKU_7 "\n\tAborted effort:\n\tClose all that you have worked on.\n\tYou ask way too much.\n"

#define DEFAULT_MAX_ERRORS     3
#define DEFAULT_MAX_TIME_LAPSE 0
#define DEFAULT_DEVICE         "en0"
#define DEFAULT_DATAGRAM_SIZE  20480 /* bytes */

#define DEBUG(description) printf("%s\n", description);
#define CASE(shortName, gnuName) \
	if (!strcmp(cmdLineArg, shortName) || !strcmp(cmdLineArg, gnuName)) {
#define	CHECKARG \
	if (ndx + 1 > argCount - 1 || !strncmp(argList[ndx + 1], "-", 1)) { \
		printf("Option %s requires an argument.\n", cmdLineArg); \
		return 0; \
	}
#define ESAC \
	continue; }

/*       [[host:]file1 [...]] [[INPUT] OUTPUT]\n\ */
#define HELP_DIALOG \
" \n\
Usage: %s user@destination [-fv] [-s source] [-d device] \n\
			   [-g datagram-size] [-l time-lapse] [-e error-count] [[INPUT] OUTPUT]\n\
 \n\
Luna is an anonymous finitely improbable data transfer and security nexus. \n\
 \n\
Switches: \n\
  -s, --source			Source \n\
  -d, --device			Sniff device (default: probe or %s) \n\
  -g. --datagram-size		Maximum datagram size in bytes (default: %dk) \n\
  -l, --max-time-lapse		Maximum time lapse before injecting packets (default: %d) \n\
  -e, --max-errors		Maximum allowed errors before giving up (default: %d) \n\
  -f. --fork			Work in the background \n\
  -v, --verbose			Vocalize (potentially noisy) \n\
 \n\
This is Luna version %s; report bugs to Smurfboy <smurfboy@chungsdonutshop.com> \n\
\"For all things so finitely improbable, they're probable -- I give you this.\"\n\n"


/* class methods */
Luna *Luna_alloc();
Luna *Luna_new(int, char *[]);

/* instance methods */
int Luna_init(Luna *, int, char *[]);
int Luna_dealloc(Luna *);
int Luna_delete(Luna *);
int Luna_run(Luna *);

/* private methods */
int  __parseCommandLine(Luna *, int, char *[]);
void __help(char *);
u_long Luna_resolve(char *);


#ifdef __cplusplus
}
#endif

#endif /* Luna_h */
