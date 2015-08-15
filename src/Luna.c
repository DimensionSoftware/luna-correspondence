
#include "Luna.h"
#include "Receiver.h"
#include "Transmitter.h"
#include "User.h"


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
Luna*
Luna_alloc()
{
	Luna *self = (Luna *)malloc(sizeof(Luna));
	#ifdef LUNA_DEBUG
	DEBUG("+ Luna");
	#endif
	return (self) ? self : NULL;
}

/* constructor w/ default initialize */
Luna*
Luna_new(int argc, char *argv[])
{
	Luna *self = Luna_alloc();

	if (self) {
		if (Luna_init(self, argc, argv)) {
			return self;
		} else {
			Luna_dealloc(self);
			return NULL;
		}
	} else {
		return NULL;
	}
}

u_long
Luna_resolve(char *host)
{
	struct in_addr in;
	struct hostent *he;

	if ((in.s_addr = inet_addr(host)) == -1) {
		if ((he = gethostbyname(host)) == NULL) {
			return 0;
		}
		memcpy((caddr_t)&in, he->h_addr, he->h_length);
	}
	return (in.s_addr);
}


/* begin instance methods */
/* initializer */
int
Luna_init(Luna *self, int argc, char *argv[])
{
	if (!self) { return 0; }

	/* apply defaults */
	self->_user          = 0;
	self->_source        = 0;
	self->_destination   = 0;
	self->_device        = 0;
	self->_datagramSize  = DEFAULT_DATAGRAM_SIZE;
	self->_fork          = 0;
	self->_maxErrors     = DEFAULT_MAX_ERRORS;
	self->_maxTimeLapse  = 0;
	self->_verbose       = 0;

	if ((self->_user = malloc(8)) == NULL) { return 0; }
	strncpy(self->_user, "default", 7);
	self->_user[8] = '\0';

	/* parse command line arguments */
	return (__parseCommandLine(self, argc, argv))
		? 1
		: 0;
}

/* destructor */
int
Luna_dealloc(Luna *self)
{
	if (!self) return 0;
	free(self);
	self = NULL;

	#ifdef LUNA_DEBUG
	DEBUG("- Luna");
	#endif
	return 1;
}

/* destructor for initialization values too */
int
Luna_delete(Luna *self)
{
	if (!self) { return 0; }
	free(self->_user);
	return Luna_dealloc(self);
}


/* --------- */
int
Luna_run(Luna *self)
{
	char *inputBuffer;
	int pid, sentBytes, errors;
	Transmitter *lcTransmitter;
	LinkedList *relayHosts;
	Receiver *lcReceiver;
	User *lcUser;

	if (!self) { return 0; }

	/* load user */
	if ((lcUser = User_new(self->_user)) == NULL) {
		printf("Unable to load user: %s\n%s\n", self->_user, LUNA_HAIKU_1);
		return 0;
	}

	if ((relayHosts = LinkedList_maskFromList(User_userData(lcUser), 5, LinkedList_count(User_userData(lcUser)) - 4)) == NULL) {
		printf("Unable to mask user %s's relay hosts; ensure relay hosts exist in %s's configuration.\n%s\n", 
				self->_user, self->_user, LUNA_HAIKU_3);
		User_delete(lcUser);
		return 0;
	}

	/* build transmission */
	if ((lcTransmitter = Transmitter_new(self, relayHosts, User_entropyList(lcUser))) == NULL) { 
		printf("Unable to create transmitter.\n%s\n", LUNA_HAIKU_3); 
		User_delete(lcUser);
		LinkedList_delete(relayHosts); 
		return 0; 
	}
	if (self->_verbose) { printf("Using max time lapse: %d.\n", self->_maxTimeLapse); }

	/* build receiver */
	if ((lcReceiver = Receiver_new(self)) == NULL) {
		printf("Unable to create receiver.\n%s\n", LUNA_HAIKU_3);
		User_delete(lcUser);
		LinkedList_delete(relayHosts); 
		Transmitter_delete(lcTransmitter);
		return 0;
	}
	if (!(pid = Receiver_listen(lcReceiver, __defaultListener))) {
		printf("Unable to listen on %s, transmitting only.\n", self->_device);
		exit(0);
	} else {
		if (self->_verbose) { printf("Listening for replies on %s\n", self->_device); }
	}

	/* initialize input */
	inputBuffer = (char *)malloc(self->_datagramSize);
	if (!inputBuffer) { 
		printf("Unable to alloc input buffer.\n%s\n", LUNA_HAIKU_3); 
		User_delete(lcUser);
		LinkedList_delete(relayHosts); 
		Transmitter_delete(lcTransmitter);
		Receiver_delete(lcReceiver);
		return 0; 
	}
	bzero(inputBuffer, self->_datagramSize);

	/* transfer data */
	errors = 0;
	printf("Please enter data on separate lines followed by a carriage return; EOF (Ctrl+D) to terminate Luna.\n");
	for (; fgets(inputBuffer, self->_datagramSize, stdin); ) {
		if ((sentBytes = (self->_maxTimeLapse)
			? Transmitter_relayInjectAtMaxTimeLapse(lcTransmitter, self->_destination, self->_maxTimeLapse, inputBuffer, __defaultEncodingHandler)
			: Transmitter_relayInject(lcTransmitter, self->_destination, inputBuffer, __defaultEncodingHandler))) {
		} else {
			/* failure */
			if (self->_maxErrors < ++errors) {
				printf("Quitting due to excessive errors.\n%s\n", LUNA_HAIKU_4);
				break;
			}
		}
	}

	/* vocalize */
	if (self->_verbose) {
		printf("Transmitted %u time(s) for a travelling total of %u bytes.\n",
			Transmitter_count(lcTransmitter), Transmitter_bytes(lcTransmitter));
	}

	/* cleanup */
	free(inputBuffer);
	User_delete(lcUser);
	LinkedList_delete(relayHosts);
	Receiver_delete(lcReceiver);
	Transmitter_delete(lcTransmitter);

	return 0;
}


/* begin private methods */
/* --------- */
void
__help(char *applicationName)
{
	assert(strlen(applicationName));
	
	printf(HELP_DIALOG, applicationName, 
		DEFAULT_DEVICE, DEFAULT_DATAGRAM_SIZE/1024, DEFAULT_MAX_TIME_LAPSE, DEFAULT_MAX_ERRORS, LUNA_VERSION);
}


/* --------- */
int
__parseCommandLine(Luna *self, int argCount, char *argList[])
{
	int  ndx         = 0;
	char *cmdLineArg = 0;
	char hostName[256];
	char *separator, *destination;

	if (!self) { return 0; }

	/* help */
	if (argCount < 2) {
		__help(argList[0]);
		return 0;
	}
	if (!strncmp(argList[1], "-h", 2) || !strncmp(argList[1], "--help", 6)) {
		__help(argList[0]); return 0; }

	/* set destination */
	if (!strncmp(argList[1], "-", 1)) {
		printf("Missing destination host.\n%s\n", LUNA_HAIKU_6); return 0; }

	if ((separator = strchr(argList[1], '@')) != NULL) {
		destination = separator + 1;

		ndx = (strlen(argList[1]) - strlen(destination)) - 1; // ndx == length
		if ((self->_user = malloc(ndx + 1)) == NULL) { return 0; }
		strncpy(self->_user, argList[1], ndx);
		self->_user[ndx + 1] = '\0';
	} else {
		destination = argList[1];
	}
	self->_destination = Luna_resolve(destination);
	if (!self->_destination) {
		printf("Unable to set destination host.\n%s\n", LUNA_HAIKU_5); return 0; }

	/* set options */
	for (ndx = 2; ndx < argCount; ++ndx) {
		cmdLineArg = argList[ndx];
		
		if (strncmp(argList[ndx], "-", 1)) { continue; }
		
		/* set source ip address */
		CASE("-s", "--source")
			CHECKARG
			self->_source = Luna_resolve(argList[++ndx]);
			if (!self->_source) {
				printf("Unable to set source host.\n%s\n", LUNA_HAIKU_5); return 0; }
		ESAC
		
		/* set device */
		CASE("-d", "--device")
			CHECKARG
			self->_device = argList[++ndx];
		ESAC
		
		/* set fork mode */
		CASE("-f", "--fork")
			self->_fork = 1;
		ESAC
		
		/* set datagram size */
		CASE("-g", "--datagram-size")
			CHECKARG
			self->_datagramSize = atoi(argList[++ndx]);

		ESAC
		
		/* set max time lapse */
		CASE("-l", "--max-time-lapse")
			CHECKARG
			self->_maxTimeLapse = atoi(argList[++ndx]);
		ESAC
		
		/* set verbosity */
		CASE("-v", "--verbose")
			self->_verbose = 1;
		ESAC

		/* help */
		CASE("-h", "--help")
			__help(argList[0]);
			return 0;
		ESAC

		printf("\nUnrecognizeable argument: %s\n%s\n", argList[ndx], LUNA_HAIKU_6);
		return 0;
	}

	/* set default values */
	if (!self->_source) {
		gethostname(hostName, 256);
		self->_source = Luna_resolve(hostName);
		if (!self->_source) {
			printf("Unable to determine source host; please manually specify the source with '-s'.\n%s\n", 
				LUNA_HAIKU_2); 
			return 0; 
		}
	}

	if (!self->_device) {
		self->_device = pcap_lookupdev((char *)NULL);
		if (!self->_device) {
			self->_device = DEFAULT_DEVICE;
		}
	}
	return 1;
}
