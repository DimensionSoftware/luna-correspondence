
#include "Transmitter.h"


/* ---------
   Luna Correspondence

	Copyleft 2002 Smurfboy <smurfboy@chungsdonutshop.com>
     
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
	Please send an email to smurfboy@chungsdonutshop.com

*/


/* begin class methods */
/* constructor */
Transmitter*
Transmitter_alloc()
{
	Transmitter *self = (Transmitter *)malloc(sizeof(Transmitter));
	#ifdef LUNA_DEBUG
	DEBUG("+ Transmitter");
	#endif
	return (self) ? self : NULL;
}

/* constructor w/ default initialize */
Transmitter*
Transmitter_new(Luna *anApplication, LinkedList *relayHosts, int *entropyList)
{
	Transmitter *self = Transmitter_alloc();

	if (self) {
		if (Transmitter_initWithLuna(self, anApplication, relayHosts, entropyList)) {
			return self;
		} else {
			Transmitter_dealloc(self);
			return NULL;
		}
	} else {
		return NULL;
	}
}


/* begin instance methods */
/* initializer */
Transmitter*
Transmitter_initWithLuna(Transmitter *self, Luna *anApplication, LinkedList *relayHosts, int *entropyList)
{
	int flags = 1;

	if (!self) { return 0; }

	/* apply defaults */
	/* FIXME accessor methods would be safe, but that's overkill for now */
	self->_relayHosts   = relayHosts;
	self->_datagramSize = anApplication->_datagramSize;
	self->_bytes        = 0;
	self->_count        = 0;

	/* psudo-random numbers */
	srand(time(NULL) + getpid());
	self->_entropyIndex = 0;
	self->_entropyList  = entropyList;

	/* initialize networking */
	self->_packet = 0;
	if ((self->_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
		printf("Unable to open raw socket; ensure root privileges.\n"); 
		return 0;
	}

	if (setsockopt(self->_socket, IPPROTO_IP, IP_HDRINCL, &flags, sizeof(flags)) < 0) {
		printf("Unable to set socket options.\n"); 
		return 0;
	}

	return self;
}

u_long
Transmitter_relayHost(Transmitter *self)
{
	char *hostData;
	u_long relayHost;

	hostData  = LinkedList_dataAtIndex(self->_relayHosts, rand() % LinkedList_count(self->_relayHosts) + 1);
	relayHost = Luna_resolve(hostData);
	if (!relayHost) {
		printf("Unable to resolve relay host: %s\n", hostData); 
		return 0; 
	}
	return relayHost;
}

int 
Transmitter_injectAtMaxTimeLapse(Transmitter *self, u_long source, u_long destination, int seconds, char *data, char *(encodingHandler)(short type, char *data)) 
{
	int pid;

	pid = fork();
	switch (pid) {
		case -1:
			printf("Unable to fork() transmitter.\n%s\n", LUNA_HAIKU_7);
			return 0;

		case 0:	/* child */
			break;

		default: /* parent */
			return pid;
	}

	
	srand(time(NULL) + getpid());
	sleep(rand() % seconds + 1);

	Transmitter_inject(self, source, destination, data, encodingHandler);

	kill(getpid(), SIGKILL);
	exit(0); /* hopefully it was a good life */
}

int 
Transmitter_relayInjectAtMaxTimeLapse(Transmitter *self, u_long destination, int seconds, char *data, char *(encodingHandler)(short type, char *data)) 
{
	/* send data relay style */
	return Transmitter_injectAtMaxTimeLapse(
		self, 
		destination, 
		Transmitter_relayHost(self), 
		seconds, 
		data, 
		encodingHandler
	);
}

int 
Transmitter_inject(Transmitter *self, u_long source, u_long destination, char *data, char *(encodingHandler)(short type, char *data))
{
	int sendBytes, dataSize, packetSize;
	struct ip *ip;
	struct icmp *icmp;
	struct sockaddr_in sin;

	/* process (encode) data */
	//data = encodingHandler(1, data);

	/* create packet */
	dataSize   = strlen(data);
	packetSize = sizeof(struct ip) + sizeof(struct icmp) + dataSize;
	if (!(self->_packet = malloc(packetSize))) {
		printf("Unable to malloc memory for packet creation.");
	}
	bzero(self->_packet, packetSize);
	ip   = (struct ip*)self->_packet;
	icmp = (struct icmp*)(self->_packet + sizeof(struct ip));
	memcpy((self->_packet + sizeof(struct ip) + sizeof(struct icmp)), data, dataSize);

	/* initialize packet */
	ip->ip_len        = packetSize;
	ip->ip_hl         = sizeof *ip >> 2;
	ip->ip_v          = 4;
	ip->ip_ttl        = 255;
	ip->ip_tos        = 0;
	ip->ip_off        = 0;
	ip->ip_id         = htons(31337);
	ip->ip_p          = 1;
	ip->ip_src.s_addr = source;
	ip->ip_dst.s_addr = destination;

	icmp->icmp_type  = 8;
	icmp->icmp_code  = 0;
	icmp->icmp_cksum = __icmpChecksum((u_short *)icmp, sizeof(struct icmp) + dataSize);

	sin.sin_family      = AF_INET;
	sin.sin_addr.s_addr = source;

	/* send data */
	if ((sendBytes = sendto(self->_socket, self->_packet, packetSize, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr))) == -1) {
		printf("Unable to send packet to.\n");
		free(self->_packet);
		return 0;
	}

	/* check for partial send failure */
	if (sendBytes < dataSize) {
		printf("Warning: Only wrote %d bytes of %d.\n", sendBytes, dataSize);
	}
	self->_bytes += sendBytes;
	self->_count += 1;

	free(self->_packet);
	return sendBytes;
}

int 
Transmitter_relayInject(Transmitter *self, u_long destination, char *data, char *(encodingHandler)(short type, char *data)) {
	/* send data relay style */
	return Transmitter_inject(
		self, 
		destination, 
		Transmitter_relayHost(self), 
		data, 
		encodingHandler
	);
}


/* destructor */
int
Transmitter_dealloc(Transmitter *self)
{
	if (!self) return 0;
	free(self);
	self = NULL;

	#ifdef LUNA_DEBUG
	DEBUG("- Transmitter");
	#endif
	return 1;
}

/* destructor for initialization values too */
int
Transmitter_delete(Transmitter *self)
{
	if (!self) { return 0; }
	close(self->_socket); 
	return Transmitter_dealloc(self);
}

/* accessor methods */
unsigned int 
Transmitter_count(Transmitter *self)
{
	return self->_count;
}

unsigned int 
Transmitter_bytes(Transmitter *self)
{
	return self->_bytes;
}


/* begin private methods */

/* copied from freebsd ping -- embrace open source */
int __icmpChecksum(u_short *addr, int len) {
	register int nleft = len;
	register u_short *w = addr;
	register int sum = 0;
	u_short answer = 0;
	while (nleft > 1) 
	{
		sum += *w++;
		nleft -= 2;
	}       
	if (nleft == 1)
	{
		*(u_char *)(&answer) = *(u_char *)w;
		sum += answer;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return (answer);
}


/* FIXME - it's a dummy for now */
char*
__defaultEncodingHandler(short type, char *data)
{
	switch (type) {
		case TRANSMIT_OPEN_CONNECTION:
			break;

		case TRANSMIT_SEND_DATA:
			break;

		case TRANSMIT_CLOSE_CONNECTION:
			break;
	}
	return data;
}
