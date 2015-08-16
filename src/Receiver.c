#include "Receiver.h"
#include "Luna.h"
#include <unistd.h>
#include <pcap.h> 


/* ---------
   Luna Correspondence

	Copyleft 2002 Smurfboy <keith@dimensionsoftware.com>
     
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
	Public License as published by the Free Software Foundation; either version 2 of the License, or (at your
	option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
	for more details.

	You should have received a copy of the GNU General Public License along with this program; if not, write to
	the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   ---------
   Feedback

	If you've found this source code useful, I'd love to hear about it.
	Please send an email to keith@dimensionsoftware.com

*/


/* begin class methods */
/* constructor */
Receiver*
Receiver_alloc()
{
	Receiver *self = (Receiver *)malloc(sizeof(Receiver));
	#ifdef LUNA_DEBUG
	DEBUG("+ Receiver");
	#endif
	return (self) ? self : NULL;
}

/* constructor w/ default initialize */
Receiver*
Receiver_new(Luna *anApplication)
{
	Receiver *self = Receiver_alloc();

	if (self) {
		if (Receiver_initWithLuna(self, anApplication)) {
			return self;
		} else {
			Receiver_dealloc(self);
			return NULL;
		}
	} else {
		return NULL;
	}
}


/* begin instance methods */
/* initializer */
int 
Receiver_initWithLuna(Receiver *self, Luna *anApplication)
{
	if (!self) { return 0; }

	/* apply defaults */
	self->_pcap       = 0;
	self->_device     = anApplication->_device;
	self->_source     = anApplication->_source;
	self->_sourceMask = 0x00111111; /* class c */

	return 1;
}


/* receive data */
int 
Receiver_listen(Receiver *self, void (protocolReceiver)(u_char *, pcap_pkthdr*, u_char *))
{
	char pcapError[PCAP_ERRBUF_SIZE];
	struct bpf_program filter;
 	
	self->_childPid = fork();
	switch (self->_childPid) {
		case -1:
			printf("Unable to fork() receiver.\n%s\n", LUNA_HAIKU_7);
			return 0;

		case 0:	/* child */
			break;

		default: /* parent */
			return self->_childPid;
	}

	/* setup listener */
	if  (pcap_lookupnet(self->_device, &self->_source, &self->_sourceMask, pcapError) == -1) {
		printf("Unable to lookup network: %s\n", pcapError);
		return 0;
	}
	if (!(self->_pcap = pcap_open_live(self->_device, 65535, 1, 1000, pcapError))) {
		printf("Unable to open pcap for listening: %s\n", pcapError);
		return 0;
	}
	/* FIXME - compile in icmp-echorequest packets only */
	if ((pcap_compile(self->_pcap, &filter, "ip proto \\icmp", 1, 0x00111111)) == -1) {
		printf("Unable to compile bpf filter: %s\n", pcap_geterr(self->_pcap));
	}
	if ((pcap_setfilter(self->_pcap, &filter)) == -1) {
		printf("Unable to set filter: %s\n", pcap_geterr(self->_pcap));
	}

	/* callback receiver protocol */
	switch(pcap_datalink(self->_pcap)) 
	{
		case DLT_EN10MB:
		case DLT_IEEE802:
			linkOffset = 14;
			break;
		case DLT_SLIP: 
			linkOffset = 16;
			break;
		case DLT_PPP:
		case DLT_NULL:
			linkOffset = 4;
			break;
		case DLT_RAW: 
			linkOffset = 0;
			break;
		default:
			printf("Unsupported interface type.\n%s\n", LUNA_HAIKU_4);
			return 0;
	}

	pcap_loop(self->_pcap, 0, (pcap_handler)__defaultListener, 0);

	exit(0); /* nothing against children */
}


void
__defaultListener(u_char *data, pcap_pkthdr* packetHeader, u_char *p)
{
	struct ip *ip     = (struct ip *)(p + linkOffset);
	struct icmp *icmp = (struct icmp *)(ip + 1);
	int dataLength = (packetHeader->len - (sizeof(struct ip) + sizeof(struct icmp) + linkOffset));
	char buffer[dataLength];

	if (icmp->icmp_type == ICMP_ECHOREPLY) {
		strncpy(buffer, (char *)ip + sizeof(struct ip) + sizeof(struct icmp), dataLength);
		buffer[dataLength] = '\0';
		printf("%s", buffer);
		fflush(stdout);
	}
}


/* destructor */
int
Receiver_dealloc(Receiver *self)
{
	if (!self) return 0;
	free(self);
	self = NULL;

	#ifdef LUNA_DEBUG
	DEBUG("- Receiver");
	#endif
	return 1;
}

/* destructor for initialization values too */
int
Receiver_delete(Receiver *self)
{
	if (!self) { return 0; }

	if (self->_childPid > 0) { kill(self->_childPid, SIGKILL); }
	if (self->_pcap) { pcap_close(self->_pcap); }

	return Receiver_dealloc(self);
}
