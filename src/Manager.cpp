/*
 *   strManager - High Speed Packet Reflector
 *   Copyright (C) 2007-2009 Fundació i2CAT, Internet i Innovació digital a Catalunya
 *   
 *   This file is part of strManager.
 *
 *   strManager is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   strManager is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: 	Javier López Rubio <javi.lopez@i2cat.net>
 * 		Guillem Cabrera Añon <guillem.cabrera@i2cat.net>
 * 		Xavier Calvo Brugal <xavier.calvo-brugal@upc.edu>
 * 		Julio Carlos Barrera Juez <julio.carlos.barrera@i2cat.net>
 */

#include <libstrmanager/Manager.h>
#include <libstrmanager/Message.h>
#include <libstrmanager/Constants.h>
#include <libstrmanager/ForwardWorker.h>
#include <libstrmanager/StatsWorker.h>
#include <libstrmanager/RtcpWorker.h>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <boost/interprocess/ipc/message_queue.hpp>


using namespace log4cxx;

static LoggerPtr logger(Logger::getLogger("strManager"));
static int MAX_WORKERS = boost::thread::hardware_concurrency();

Manager::Manager() {
	currentWorkerId = 0;
	isListeningMessages = true;
	m_thread = new boost::thread(boost::bind(&Manager::messageHandler, this));
}

int Manager::addSource(int workerType, int src_id, char* ip, int port) {

	LOG4CXX_DEBUG(logger, "Adding source: " << src_id << " with address: " << ip << ":" << port);

	//Pointer to socketWorker
	Worker *s = this->getWorker(workerType, port);

	// Add the new source to the worker
	if (s != NULL && s->addSource(src_id, ip, port)) {
		// Save the new src_id to worker reference
		srcIdWorker[src_id] = s;
		workerIdWorker[s->getWorkerId()] = s;

		// Update the currentWorker Identifier
		currentWorkerId++;

		if (currentWorkerId >= MAX_WORKERS)
			currentWorkerId = 0;
		return 0;
	} else {
		LOG4CXX_ERROR(logger, "Error adding the source: " << src_id );
		return -1;
	}
}

int Manager::removeSource(int src_id) {

	LOG4CXX_DEBUG(logger, "Removing source: " << src_id );
	//Pointer to socketWorker
	Worker *s = NULL;

	if (srcIdWorker.count(src_id) > 0) {
		s = srcIdWorker[src_id];
		// remove the src_id to worker reference
		srcIdWorker.erase(src_id);
		// remove info at the worker
		s->removeSource(src_id);
		return 0;
	} else {
		LOG4CXX_ERROR(logger, "Source Id: " << src_id << " doesn't exist");
		return -1;
	}
}

int Manager::addDestination(int src_id, int dest_id, char* ip, int port) {
	LOG4CXX_DEBUG(logger, "Adding destination at src_id: " << src_id << " dest_id: " << dest_id);

	//Pointer to socketWorker
	Worker *s = NULL;

	if (srcIdWorker.count(src_id) > 0) {
		s = srcIdWorker[src_id];
		// Add the new destination to the worker
		s->addDestination(src_id, dest_id, ip, port);
		return 0;
	} else {
		LOG4CXX_ERROR(logger, "Source Id: " << src_id << " doesn't exist");
		return -1;
	}
}

int Manager::removeDestination(int src_id, int dest_id) {
	LOG4CXX_DEBUG(logger, "Removing destination at src_id: " << src_id << " of dest_id: " << dest_id );

	//Pointer to socketWorker
	Worker *s = NULL;

	if (srcIdWorker.count(src_id) > 0) {
		s = srcIdWorker[src_id];
		// remove the new destination to the worker
		s->removeDestination(src_id, dest_id);
		return 0;
	} else {
		LOG4CXX_ERROR(logger, "Source Id: " << src_id << " doesn't exist");
		return -1;
	}
}

Worker* Manager::createWorker(int workerType) {

	// Use the Constants to set a workerType ID per worker

	switch (workerType) {
	case FWD_WORKER_TYPE:
		return new ForwardWorker(currentWorkerId);
	case RTCP_WORKER_TYPE:
		return new RtcpWorker(currentWorkerId);
	case STATS_WORKER_TYPE:
		return new StatsWorker(currentWorkerId);
	default:
		LOG4CXX_ERROR(logger, "WorkerType: " << workerType << " doesn't exist")
		;
		return NULL;
	}

}

Worker* Manager::getWorker(int workerType, int port) {

	// Try to get the pointer to socketWorker that manages the port or a worker that has this type
	Worker *worker = this->getWorkerByPortType(port, workerType);

	if (worker == NULL) { // There is no worker that fulfill the requirements

		worker = createWorker(workerType);

		if (worker != NULL) {
			// Time to launch the thread
			worker->launch();
		}

	}

	return worker;

}

Worker* Manager::getWorkerById(int workerId) {

	LOG4CXX_DEBUG(logger, "Searching for workerId " << workerId << " in the pool of workers");

	if (workerIdWorker.count(workerId) > 0)
		return workerIdWorker[workerId];
	return NULL;
}

Worker* Manager::getWorkerByPortType(int port, int WorkerType) {
	LOG4CXX_DEBUG(logger, "Searching the port " << port << " in the pool of workers");

	// TRY TO GET THE WORKER FOR THIS PORT AND WORKER TYPE

	for (workerTable::const_iterator it = workerIdWorker.begin(); it
			!= workerIdWorker.end(); ++it) {
		if ((*it).second->hasPort(port) && (*it).second->isWorkerType(
				WorkerType)) {
			LOG4CXX_DEBUG(logger, "Found the port " << port << " in the worker " << (*it).second->getWorkerId());
			return (*it).second;
		}
	}
	// If not return a worker from that type
	return this->getWorkerByType(WorkerType);
}

Worker* Manager::getWorkerByType(int workerType) {

	LOG4CXX_DEBUG(logger, "Searching for workerType " << workerType);

	for (workerTable::const_iterator it = workerIdWorker.begin(); it
			!= workerIdWorker.end(); ++it) {
		if ((*it).second->getWorkerType() == workerType) {
			LOG4CXX_DEBUG(logger, "Assigned to the workerId: " << (*it).second->getWorkerId());
			return (*it).second;
		}
	}
	return NULL;
}

void Manager::messageHandler() {

	try {

		//Erase previous message queue
		boost::interprocess::message_queue::remove("message_queue");
		//Open a message queue.
		boost::interprocess::message_queue mq(
				boost::interprocess::open_or_create, "message_queue", 100,
				sizeof(Message));

		LOG4CXX_DEBUG(logger, "MessageHandler is waiting for messages.");

		Message * m = new Message();

		while (isListeningMessages) {

			unsigned int priority;
			std::size_t recvd_size;

			mq.receive(m, sizeof(Message), recvd_size, priority);

			if (m->type == 100) {
				m->print();
				workerIdWorker.erase(m->from);
			}
		}

	} catch (boost::interprocess::interprocess_exception &ex) {

		boost::interprocess::message_queue::remove("message_queue");
		std::cout << ex.what() << std::endl;
		return;
	}

	boost::interprocess::message_queue::remove("message_queue");

}

string Manager::printManagement() {
	ostringstream stm;
	stm << "Printing management list of all workers\n";
	for (workerTable::const_iterator it = workerIdWorker.begin(); it
			!= workerIdWorker.end(); ++it) {
		stm << ((*it).second->printManagement()) << "\n";
	}
	return stm.str();
}

string Manager::printForwarding() {
	ostringstream stm;
	stm << "Printing forwarding list of all workers\n";
	for (workerTable::const_iterator it = srcIdWorker.begin(); it
			!= srcIdWorker.end(); ++it) {
		stm << (*it).second->printForwarding() << "\n";
	}
	return stm.str();
}

string Manager::printPortFd() {
	ostringstream stm;
	stm << "Printing PortFd list of all workers\n";
	for (workerTable::const_iterator it = srcIdWorker.begin(); it
			!= srcIdWorker.end(); ++it) {
		stm << (*it).second->printPortFd() << "\n";
	}
	return stm.str();
}

time_t Manager::readTimeStamp(int src_id) {
	//Pointer to socketWorker
	Worker *s = NULL;

	if (srcIdWorker.count(src_id) > 0) {
		s = srcIdWorker[src_id];
		return s->readTimeStamp(src_id);
	} else {
		LOG4CXX_ERROR(logger, "Source Id: " << src_id << " doesn't exist");
		time_t now;
	time(&now);
		return now;
	}
}

Manager::~Manager() {
	// TODO Auto-generated destructor stub
}
