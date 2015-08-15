/*	---------
	Luna Correspondence

Copyleft 2002 Smurfboy
    
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

	----------
	Feedback

If you've found this source code useful, I'd love to hear about it.
Email: keith@lbox.org  */


#include "Luna.h"


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


int main(int argc, char *argv[])
{
	int exitCode;
	Luna *application = Luna_new(argc, argv);

	if (application) { 
		exitCode = Luna_run(application);
		Luna_delete(application); 
		/*if (!exitCode) { 
			printf("Error running Luna; aborted effort, all good things must come to an end.  Evidently, yours has come.\n"); 
		}*/
		return exitCode;
	/*} else {
		printf("Error creating luna; a simple crash reduces your expensive computer to a simple stone.  Hopefully help arrived.\n");
	*/}

	return 0;
}
