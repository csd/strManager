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

static LoggerPtr logger(Logger::getLogger("ZAP_strManager"));

//strManager variables
Manager * sm;

int addSource(int workerType, int src_id, char* ip, int port) {
	return sm->addSource(workerType, src_id, ip, port);
}

int addDestination(int src_id, int dest_id, char* ip, int port) {
	return sm->addDestination(src_id, dest_id, ip, port);
}

int removeDestination(int src_id, int dest_id) {
	return sm->removeDestination(src_id, dest_id);
}

string usage() {
	return "usage:\nZAP_strManager [source1 IP1] [MCU port1] [source2 IP2] [MCU port2]\n[source3 IP3] [MCU port3] [destination IP4] [destination port4]\n";
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

	cout << "Zapping strManager" << endl;

	//Using log4c only to file (see configuration file)
	PropertyConfigurator::configure("conf/logger.properties");

	int res = 0;
	//adding 3 channels on local machine and ports
	//1111, 2222 and 3333 with srcId 0, 1 and 2
	res += addSource(1, 0, argv[1], atoi(argv[2]));
	res += addSource(1, 1, argv[3], atoi(argv[4]));
	res += addSource(1, 2, argv[5], atoi(argv[6]));

	//circular counter to select channel, starting by 0
	int actual = 0;

	//default destination on channel 0
	res += addDestination(actual, 3, argv[7], atoi(argv[8]));

	if (res != 0) {
		cout << "Some error has ocurred. See log file!" << endl;
		return -2;
	}

	cout << "Sources:\n" << argv[1] << ":" << argv[2] << "\n" << argv[3] << ":"
			<< argv[4] << "\n" << argv[5] << ":" << argv[6]
			<< "\n\nDestination:\n" << argv[7] << ":" << argv[8] << "\n\n";
	cout << "Press enter to change channel..." << endl;

	string line;
	//continous program
	while (true) {
		ostringstream in;
		in << "Actual channel = " << actual;
		//read standard input
		cout << in.str();
		getline(cin, line);
		if (line == "") {
			cout << "Changing channel..." << endl;
			//erase actual channel
			res += removeDestination(actual, 3);
			//actual counter to next channel
			actual = (actual + 1) % 3;
			//adding next channel
			res += addDestination(actual, 3, argv[7], atoi(argv[8]));
			if (res != 0) {
				cout << "Some error has ocurred. See log file!" << endl;
				return -2;
			}
		}
	}
}
