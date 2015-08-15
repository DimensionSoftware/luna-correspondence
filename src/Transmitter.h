
#ifndef Transmitter_h
#define Transmitter_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "LinkedList.h"
#include "Luna.h"

typedef struct Transmitter {
	int _datagramSize;	/* datagram size */
	int _socket;		/* raw socket */
	u_char *_packet;	/* packet */
	LinkedList *_relayHosts;

	int *_entropyList;	/* list of random numbers */
	int _entropyIndex;

	unsigned int _bytes;	/* total transmitted bytes */
	unsigned int _count;	/* total transmit count */
} Transmitter;


/* constants */
enum { TRANSMIT_OPEN_CONNECTION, TRANSMIT_SEND_DATA, TRANSMIT_CLOSE_CONNECTION };

/* class methods */
Transmitter *Transmitter_alloc();
Transmitter *Transmitter_new(Luna *, LinkedList *, int *);

/* instance methods */
Transmitter *Transmitter_initWithLuna(Transmitter *, Luna *, LinkedList *, int *);
u_long Transmitter_relayHost(Transmitter *);
int Transmitter_dealloc(Transmitter *);
int Transmitter_delete(Transmitter *);
int Transmitter_inject(Transmitter *, u_long, u_long, char *, char *(encodingHandler)(short, char *));
int Transmitter_relayInject(Transmitter *, u_long, char *, char *(encodingHandler)(short, char *));
int Transmitter_injectAtMaxTimeLapse(Transmitter *, u_long, u_long, int, char *, char *(encodingHandler)(short, char *));
int Transmitter_relayInjectAtMaxTimeLapse(Transmitter *, u_long, int, char *, char *(encodingHandler)(short, char *));
/* accessor methods */
unsigned int Transmitter_bytes(Transmitter *);
unsigned int Transmitter_count(Transmitter *);

/* private methods */
char *__defaultEncodingHandler(short, char *);
int __icmpChecksum(u_short *, int);
short __uploadPersonalInformationToMotherShip(void *);

#ifdef __cplusplus
}
#endif

#endif /* Transmitter_h */
