/*
 Copyright (C) 2007-2009 Fundació i2Cat
 Departament d'Enginyeria Telemàtica (Universitat Politècnica de Catalunya)

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * Authors: 	Javier López Rubio 			<javi.lopez@i2cat.net>
 * 				Guillem Cabrera Añon 		<guillem.cabrera@i2cat.net>
 * 				Xavier Calvo Brugal 		<xavier.calvo-brugal@upc.edu>
 * 				Julio Carlos Barrera Juez 	<julio.carlos.barrera@i2cat.net>
 */

#include <iostream>
#include <libstrmanager/Manager.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace log4cxx;
using namespace log4cxx::helpers;

static LoggerPtr logger(Logger::getLogger("BC_strManager"));

//strManager variables
Manager * sm;

int addSource(int workerType, int src_id, char* ip, int port) {
	return sm->addSource(workerType, src_id, ip, port);
}

int addDestination(int src_id, int dest_id, char* ip, int port) {
	return sm->addDestination(src_id, dest_id, ip, port);
}

string usage() {
	return "usage:\nBC_strManager [source IP1] [MCU port1]\n              [destination2 IP2] [destination port2]\n              [destination3 IP3] [destination port3]\n              [destination4 IP4] [destination port4]\n";
}

int main(int argc, char** argv) {

	if (argc != 9) {
		ostringstream oss;
		oss << "Number of parameters = " << (argc - 1) << endl;
		cout << oss.str();
		cout << usage();
		return -1;
	}

	sm = new Manager();

	cout << "Broadcast strManager\n" << endl;
	PropertyConfigurator::configure("conf/logger.properties");

	int res = 0;
	//adding source with srcId 0 on local machine and port 1111
	res += addSource(1, 0, argv[1], atoi(argv[2]));

	//adding 3 destinations with destinationIds 1, 2 and 3
	//on local machine and ports 2222, 3333 and 4444
	res += addDestination(0, 1, argv[3], atoi(argv[4]));
	res += addDestination(0, 2, argv[5], atoi(argv[6]));
	res += addDestination(0, 3, argv[7], atoi(argv[8]));

	if (res != 0) {
		cout << "Some error has ocurred. See log file!" << endl;
		return -2;
	}

	cout << "Source:\n" << argv[1] << ":" << argv[2] << "\n\nDestinations:\n"
			<< argv[3] << ":" << argv[4] << "\n" << argv[5] << ":" << argv[6]
			<< "\n" << argv[7] << ":" << argv[8] << "\n\n";
	cout << "Continuous program. Control^C to interrupt it." << endl;

	//continuous program
	while (true)
		;
}
