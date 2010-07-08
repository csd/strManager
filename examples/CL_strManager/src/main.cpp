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
 * Authors: 	Javier López Rubio 		<javi.lopez@i2cat.net>
 * 		Guillem Cabrera Añon 		<guillem.cabrera@i2cat.net>
 * 		Xavier Calvo Brugal 		<xavier.calvo-brugal@upc.edu>
 * 		Julio Carlos Barrera Juez 	<julio.carlos.barrera@i2cat.net>
 */

#include <iostream>
#include <libstrmanager/Manager.h>
#include <stdio.h>
#include <stdlib.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

static LoggerPtr logger(Logger::getLogger("CL_strManager"));

//strManager variables
Manager * sm;
int flowId;

//Help function
string help() {
	ostringstream stm;
	stm << "..:: AVAILABLE COMMANDS ::.." << endl;
	stm
			<< "\t-addsource [worker type] [source IP] [destination Port] :: adds a flow as a source"
			<< endl;
	stm << "\t-remsource [flowId] :: deletes a source" << endl;
	stm
			<< "\t-adddestination [source flowId] [destination IP] [destination Port] :: adds a destination of a source"
			<< endl;
	stm
			<< "\t-remdestination [source flowId] [flowId] :: deletes a destination"
			<< endl;
	stm << "\t-printm :: printManagement" << endl;
	stm << "\t-printf :: printForwarding" << endl;
	stm << "\t-printp :: printPortFd" << endl;
	stm << endl << endl;
	return stm.str();
}

int count(string line, char token) {
	int i = 0;
	for (string::iterator it = line.begin(); it != line.end(); ++it) {
		if ((*it) == token) {
			i++;
		}
	}
	return i;
}

int addSource(int workerType, int src_id, char* ip, int port) {
	flowId++;
	return sm->addSource(workerType, src_id, ip, port);
}

int removeSource(int src_id) {
	return sm->removeSource(src_id);
}

int addDestination(int src_id, int dest_id, char* ip, int port) {
	flowId++;
	return sm->addDestination(src_id, dest_id, ip, port);
}

int removeDestination(int src_id, int dest_id) {
	return sm->removeDestination(src_id, dest_id);
}

string printManagement() {
	return sm->printManagement();
}

string printForwarding() {
	return sm->printForwarding();
}

string printPortFd() {
	return sm->printPortFd();
}

void execute(string line) {
	if (line == "") {
		help();
	} else if (line.substr(0, 9) == "addsource" && count(line, ' ') == 3) {
		line = line.substr(10); // SE CARGA EL "ADDSOURCE "
		int workerType = atoi((char*) line.substr(0, line.find(" ")).c_str()); // ME QUEDO CON EL WORKER
		line = line.substr(line.find(" ") + 1); // ME CARGO EL WORKER
		string line2 = line.substr(0, line.find(" "));
		char * ip = (char*) line2.c_str();
		int port =
				atoi((char*) line.substr(line.find_last_of(" ") + 1).c_str());
		LOG4CXX_DEBUG(logger, addSource(workerType, flowId, ip, port));
	} else if (line.substr(0, 9) == "remsource" && count(line, ' ') == 1) {
		line = line.substr(10);
		LOG4CXX_DEBUG(logger, removeSource(atoi((char*) line.c_str())));
	} else if (line.substr(0, 14) == "adddestination" && count(line, ' ') == 3) {
		line = line.substr(15);
		string line2 = line.substr(0, line.find(" "));

		int flow = atoi((char*) line2.c_str());

		line = line.substr(line.find(" ") + 1);
		line2 = line.substr(0, line.find(" "));
		char * ip = (char*) line2.c_str();

		int port = atoi((char*) line.substr(line.find(" ") + 1).c_str());
		LOG4CXX_DEBUG(logger, addDestination(flow, flowId, ip, port));
	} else if (line.substr(0, 14) == "remdestination" && count(line, ' ') == 2) {
		line = line.substr(15);
		int flow = atoi((char*) line.substr(0, line.find(" ")).c_str());
		int flow2 = atoi((char*) line.substr(line.find(" ") + 1).c_str());
		LOG4CXX_DEBUG(logger, removeDestination(flow, flow2));
	} else if (line.substr(0, 5) == "remip" && count(line, ' ') == 1) {
		line = line.substr(6);
		//sm->removeIPAddress(line);
		cout << endl;
	} else if (line.substr(0, 6) == "printm") {
		LOG4CXX_DEBUG(logger, printManagement() + "\n\n");
	} else if (line.substr(0, 6) == "printf") {
		LOG4CXX_DEBUG(logger, printForwarding() + "\n\n");
	} else if (line.substr(0, 6) == "printp") {
		LOG4CXX_DEBUG(logger, printPortFd() + "\n\n");
	} else if (line.substr(0, 4) == "help") {
	cout << 	help();
	} else if (line.substr(0, 4) == "exit") {
		exit(0);
	} else {
		cout << "Bad command: " << line << endl << endl;
		cout << help();
	}
}

int main(int argc, char** argv) {

	sm = new Manager();
	flowId = 100;

	cout << "Basic command line strManager" << endl;
	PropertyConfigurator::configure("conf/logger.properties");

	string line;
	while (true) {
		getline(cin, line);
		if (!cin) {
			break;
		}
		execute(line);
	}
	exit(0);
}
