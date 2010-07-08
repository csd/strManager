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
 * Authors: Javier López Rubio 			<javi.lopez@i2cat.net>
 * 			Guillem Cabrera Añon 		<guillem.cabrera@i2cat.net>
 * 			Xavier Calvo Brugal 		<xavier.calvo-brugal@upc.edu>
 * 			Julio Carlos Barrera Juez 	<julio.carlos.barrera@i2cat.net>
 */

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

#include <libstrmanager/Manager.h>

#include "ws/web_service.h"
#include "ws/soapH.h"
#include "ws/WS_strManager.nsmap"

using namespace std;
using namespace log4cxx;
using namespace log4cxx::helpers;

static LoggerPtr logger(Logger::getLogger("WS_strManager"));

//strManager variables
Manager* sm;
int flowId;

string usage() {
	ostringstream stm;
	stm << "usage: " << endl;
	stm << "WS_strManager [listen_port]" << endl;
	stm << endl;
	return stm.str();
}

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

int count(string line, char token) {
	int i = 0;
	for (string::iterator it = line.begin(); it != line.end(); ++it) {
		if ((*it) == token) {
			i++;
		}
	}
	return i;
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
		help();
	} else if (line.substr(0, 4) == "exit") {
		exit(0);
	} else {
		cout << "Bad command: " << line << endl << endl;
		cout << help();
	}
}

//WS functions implementation
int ns__command(struct soap* soap, xsd__string* command, bool& result) {
	LOG4CXX_DEBUG(logger, "WS Command received: " + string(*command));
	execute(string(*command));
	result = true;
	return SOAP_OK;
}

int ns__addSource(struct soap* soap, int* workerType, int* src_id,
		xsd__string* ip, int* port, int& result) {
	LOG4CXX_DEBUG(logger, "WS AddSource received");
	result = addSource(*workerType, *src_id, (char*) *ip, *port);
	return SOAP_OK;
}

int ns__removeSource(struct soap* soap, int* src_id, int& result) {
	LOG4CXX_DEBUG(logger, "WS RemoveSource received");
	result = removeSource(*src_id);
	return SOAP_OK;
}

int ns__addDestination(struct soap* soap, int* src_id, int* dest_id,
		xsd__string* ip, int* port, int& result) {
	LOG4CXX_DEBUG(logger, "WS AddDestination received");
	result = addDestination(*src_id, *dest_id, (char*) ip, *port);
	return SOAP_OK;
}

int ns__removeDestination(struct soap* soap, int* src_id, int* dest_id,
		int& result) {
	LOG4CXX_DEBUG(logger, "WS RemoveDestination received");
	result = removeDestination(*src_id, *dest_id);
	return SOAP_OK;
}

int ns__printManagement(struct soap* soap, xsd__string& result) {
	LOG4CXX_DEBUG(logger, "WS PrintManagement received");
	result = (char *) printManagement().c_str();
	return SOAP_OK;
}

int ns__printForwarding(struct soap* soap, xsd__string& result) {
	LOG4CXX_DEBUG(logger, "WS PrintForwarding received");
	result = (char *) printForwarding().c_str();
	return SOAP_OK;
}

int ns__printPortFd(struct soap* soap, xsd__string& result) {
	LOG4CXX_DEBUG(logger, "WS PrintPortFd received");
	result = (char *) printPortFd().c_str();
	return SOAP_OK;
}

int main(int argc, char** argv) {

	sm = new Manager();
	flowId = 100;

	PropertyConfigurator::configure("conf/logger.properties");
	cout << "Starting server..." << endl;
	string line;
	if (argc == 2) {
		try {
			//Listen port for WS endpoint
			int listen_port = atoi(argv[1]);
			if (listen_port <= 0 || listen_port > 65535) {
				throw listen_port;
			}
			struct soap soap;
			int m, s; // master and slave sockets
			soap_init(&soap);
			//NULL to listen on all interfaces
			m = soap_bind(&soap, NULL, listen_port, 100);
			if (m < 0)
				soap_print_fault(&soap, stderr);
			else {
				cout << "Listening on socket " << m << endl;
				for (int i = 1;; i++) {
					s = soap_accept(&soap);
					if (s < 0) {
						soap_print_fault(&soap, stderr);
						break;
					}
					cout << "Message from IP = " << ((soap.ip >> 24) & 0xFF)
							<< "." << ((soap.ip >> 16) & 0xFF) << "."
							<< ((soap.ip >> 8) & 0xFF) << "." << (soap.ip
							& 0xFF) << " and socket = " << s << endl;
					if (soap_serve(&soap) != SOAP_OK) // process RPC request
						soap_print_fault(&soap, stderr); // print error
					soap_destroy(&soap); // clean up class instances
					soap_end(&soap); // clean up everything and close socket
				}
			}
			soap_done(&soap); // close master socket and detach environment
		} catch (int l) {
			cout << "ERROR: Bad port: " << l << ", " << argv[1] << endl << endl;
			cout << usage();
			exit(-1);
		}
	} else {
		cout << usage();
	}
	exit(0);
}
