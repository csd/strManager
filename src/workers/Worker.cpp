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

#include <libstrmanager/Worker.h>
#include <libstrmanager/Constants.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace log4cxx;

static LoggerPtr logger(Logger::getLogger("strManager"));

Worker::Worker(int workerId) {
	this->workerType = DEF_WORKER_TYPE;
	LOG4CXX_DEBUG(logger, "Created a new worker with id: " << workerId);
	this->workerId = workerId;
	working = true;
}

Worker::~Worker() {
	if (m_thread != NULL)
		delete m_thread;

	LOG4CXX_DEBUG(logger, "Deleted worker with id: " << workerId);
}

void Worker::launch() {
	m_thread = new boost::thread(boost::bind(&Worker::run, this));
}

void Worker::stop() {
	LOG4CXX_DEBUG(logger, "Stopping worker: "<< workerId);

	working = false;
	m_thread->join();

	LOG4CXX_DEBUG(logger, "Stopped worker: "<< workerId);
}

bool Worker::isWorking() {
	return this->working;
}

void Worker::run() {
	while (working) {

		if (portFd.size() > 0) {

			this->build_select_list();

			int readsocks = select(highsock + 1, &fdset, (fd_set *) 0,
					(fd_set *) 0, &timeout);

			if (readsocks > 0) {
				this->check_select_list();
			} else {
				Message m(0, workerId, "NOTHING TO DO");
				this->sendMessage(m);
				//m.print();
			}
		}
	}
	//this->stop();
}

int Worker::add_socket(int port) {
	LOG4CXX_DEBUG(logger, "Opening socket at port: " << port << " in worker: " << workerId);

	if (portListener[port]) {
		LOG4CXX_DEBUG(logger, "Reused opened socket at port: " << port << " in worker: " << workerId);
		portListener[port]++;
		return 0;
	}

	int fd = this->init_socket(port);

	//boost::mutex::scoped_lock scoped_lock(m_mutex);

	if (fd == -1) {
		LOG4CXX_ERROR(logger, "Problem opening the socket at port: " << port);
		return -1;
	}

	//	portFd.insert(intTable::value_type(port,fd));
	//	fdPort.insert(intTable::value_type(fd,port));
	//	portListener.insert(intTable::value_type(port,1));


	portFd[port] = fd;
	fdPort[fd] = port;
	portListener[port] = 1;

	return 1;

}

bool Worker::remove_socket(int port) {
	LOG4CXX_DEBUG(logger, "Removing socket at port: " << port << " in worker: " << workerId);

	//boost::mutex::scoped_lock scoped_lock(m_mutex);

	portListener[port]--;

	if (portListener[port] == 0) {
		// Get the fd of the port
		int fd = portFd[port];

		// delete references within the data structures

		portFd.erase(port);
		fdPort.erase(fd);
		portListener.erase(port);

		// TODO stop the thread if needed, now does not work.

		/*
		 if (portFd.size()==0)
		 this->stop();
		 */
		close(fd);

		LOG4CXX_DEBUG(logger, "Closed socket at port: " << port << " with file descriptor " << fd << " at worker: " << workerId );

		return true;
	}

	return false;
}

int Worker::init_socket(int port) {
	struct sockaddr_in address;
	int sock;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);

	/* Creates a socket. */
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		LOG4CXX_ERROR(logger, "Error opening datagram socket at port: "<< port)
		return -1;
	}

	/* Bind the socket to the port. */
	int result = ::bind(sock, (struct sockaddr *) &address,
			sizeof(struct sockaddr_in));

	if (result < 0) {
		LOG4CXX_ERROR(logger, "Error binding datagram socket at port: "<< port)
		return -1;
	}

	LOG4CXX_DEBUG(logger, "Opened socket at port: " << port << " in worker: " << workerId << " with file descriptor: " << sock);

	return sock;

}

bool Worker::isEmpty() {
	if (this->management.size())
		return false;
	return true;
}

void Worker::build_select_list() {
	highsock = -1; // Sets to -1 the highSock fd value

	timeout.tv_sec = 15; // HARDCODE: Every 15 second the select expires
	timeout.tv_usec = 0;

	FD_ZERO(&fdset); // Clears the fdset structure


	//boost::mutex::scoped_lock scoped_lock(m_mutex);


	for (intTable::const_iterator it = portFd.begin(); it != portFd.end(); ++it) {
		int fd = (int) it->second;
		FD_SET(fd,&fdset);

		if (fd > highsock)
			highsock = fd;
	}

	highsock++;
}

void Worker::check_select_list() {

	for (intTable::const_iterator it = portFd.begin(); it != portFd.end(); ++it) {
		int fd = (int) it->second;
		if (FD_ISSET (fd, &fdset)) {
			// Process method.
			this->process(fd);
		}
	}
}

void Worker::process(int fd) {

}

bool Worker::hasPort(int port) {
	if (portFd.count(port) > 0) {
		LOG4CXX_DEBUG(logger, "Found port: "<< port << " in worker: " << workerId);
		return true;
	}
	return false;
}

int Worker::getWorkerId() {
	return this->workerId;
}

int Worker::getWorkerType() {
	return this->workerType;
}

void Worker::setWorkerType(int workerType) {
	this->workerType = workerType;
}

bool Worker::isWorkerType(int workerType) {
	if (this->workerType == workerType)
		return true;
	return false;
}

void Worker::addStreamList(int listId, std::tr1::shared_ptr<StreamList> sList) {
	LOG4CXX_DEBUG(logger, "Saved streamList with Id: " << listId);
	management[listId] = sList;
	forwarding[sList->getSourceSockAddr()] = listId;
}

void Worker::removeStreamList(int listId) {
	LOG4CXX_DEBUG(logger, "Removed streamList with Id: " << listId);
	std::tr1::shared_ptr<StreamList> tmp = management[listId];

	forwarding.erase(tmp->getSourceSockAddr());

	management.erase(listId);
	//management[listId];
}

void Worker::conmuteStreamList(int from, int to) {
	boost::mutex::scoped_lock scoped_lock(m_mutex);
	LOG4CXX_DEBUG(logger, "Conmuting streamList: " << from << " to " << to );

	std::tr1::shared_ptr<StreamList> a = management[from]; // I get the streamList that has the destinations
	std::tr1::shared_ptr<StreamList> b = management[to]; // I get the streamlist that will have the destinations

	b->setDestinations(a->getDestinations());

	a->clearDestinations();

	management[from] = a;
	management[to] = b;
}

void Worker::conmuteUdpStream(int id, int from, int to) {
	boost::mutex::scoped_lock scoped_lock(m_mutex);

	LOG4CXX_DEBUG(logger, "Conmuting UdpStream: " << id << ", " << from << " to " << to );

	std::tr1::shared_ptr<StreamList> a = management[from]; // I get the streamList that has the destinations
	std::tr1::shared_ptr<StreamList> b = management[to]; // I get the streamlist that will have the destinations


	b->addDestination(a->getDestination(id));

	management[from] = a;
	management[to] = b;
}

bool Worker::addSource(int src_id, char* ip, int port) {
	LOG4CXX_INFO(logger, "Adding source: " << src_id << " with address: " << ip << ":" << port << " in worker: " << workerId);

	boost::mutex::scoped_lock scoped_lock(m_mutex);

	std::tr1::shared_ptr<UdpStream> udpstream(new UdpStream());
	std::tr1::shared_ptr<StreamList> streamlist(new StreamList());

	// Setters of the UdpStream object
	udpstream->setId(src_id);
	udpstream->setAddr(ip, port);

	// Check if a udpStream source with same values exists.
	if (this->getListIdBySocket(udpstream->getAddr()))
		return false;

	// Setters of the streamList object (streamList_id == source_id)
	streamlist->setListId(src_id);
	streamlist->setSource(udpstream);

	// Create the listening socket
	int ret = this->add_socket(port);

	if (ret != -1) {
		// Call to the AddStreamList function to add to the internal structures.
		this->addStreamList(src_id, streamlist);

		return true;
	}

	return false;

}

void Worker::removeSource(int src_id) {
	LOG4CXX_INFO(logger, "Removing source: " << src_id << " in worker: " << workerId);

	boost::mutex::scoped_lock scoped_lock(m_mutex);

	// Try to remove the socket if necessary.
	this->remove_socket(this->getStreamList(src_id)->getSourcePort());

	// If a source is deleted the streamList will be deleted too.
	this->removeStreamList(src_id);

	// If the Worker doesn't have more streamList the Worker will be deleted
	if (this->isEmpty()) {
		this->sendMessage(Message(100, this->getWorkerId(),
				"WORKER DOESN'T HAVE ANY JOB TO PERFORM"));
		this->stop();
	}

}

void Worker::addDestination(int src_id, int dest_id, char* ip, int port) {
	LOG4CXX_INFO(logger, "Adding destination on: " << src_id << " with destination: " << dest_id << " in worker: " << workerId);

	boost::mutex::scoped_lock scoped_lock(m_mutex);

	// Create a new udpstream
	std::tr1::shared_ptr<UdpStream> udpstream(new UdpStream());

	// Setters of the UdpStream object
	udpstream->setId(dest_id);
	udpstream->setAddr(ip, port);

	// Get the selected streamList and add the new udpStream object to the streamList
	std::tr1::shared_ptr<StreamList> streamlist = this->getStreamList(src_id);

	if (streamlist->addDestination(udpstream)) {
		this->addStreamList(src_id, streamlist); // Call to the AddStreamList function to add to the internal structures.
	}
}

void Worker::removeDestination(int src_id, int dest_id) {
	LOG4CXX_INFO(logger, "Removing destination: " << dest_id << " of list: " << src_id);

	boost::mutex::scoped_lock scoped_lock(m_mutex);

	// Get the selected streamList and delete the new udpStream object to the streamList
	std::tr1::shared_ptr<StreamList> streamlist = this->getStreamList(src_id);
	if (streamlist->existDestination(dest_id)) {
		streamlist->remDestination(dest_id);

		// Call to the AddStreamList function to add to the internal structures.
		this->addStreamList(src_id, streamlist);
	} else {
		LOG4CXX_ERROR(logger, "Destination: " << dest_id << " of list: " << src_id << " not found ");
	}

}

std::tr1::shared_ptr<StreamList> Worker::getStreamList(int listId) {
	return management[listId];
}

int Worker::getListIdBySocket(sockaddr_in addr) {
	return forwarding[addr];
}

std::tr1::shared_ptr<StreamList> Worker::getStreamListBySocket(sockaddr_in addr) {
	return this->getStreamList(this->getListIdBySocket(addr));
}

string Worker::printManagement() {
	boost::mutex::scoped_lock scoped_lock(m_mutex);

	ostringstream stm;
	stm << "Replication lists of worker " << this->getWorkerId() << "\n";

	for (managementTable::const_iterator it = management.begin(); it
			!= management.end(); ++it) {
		std::tr1::shared_ptr<StreamList> b =
				(std::tr1::shared_ptr<StreamList>) it->second;
		if (!b) {
			LOG4CXX_FATAL(logger,"Hash_map::iterator returned an empty object.");
		} else {
			stm << b->printList() << "\n";
		}
	}
	return stm.str();
}

string Worker::printList(int src_id) {
	boost::mutex::scoped_lock scoped_lock(m_mutex);
	std::tr1::shared_ptr<StreamList> b = management[src_id];
	if (!b) {
		LOG4CXX_FATAL(logger,"Hash_map::iterator returned an empty object.");
	} else {
		return b->printList() + "\n";
	}
}

string Worker::printForwarding() {
	boost::mutex::scoped_lock scoped_lock(m_mutex);

	ostringstream stm;
	stm << "Sources of: " << this->getWorkerId() << "\n";
	for (forwardingTable::const_iterator it = forwarding.begin(); it
			!= forwarding.end(); ++it) {
		ostringstream stmSec;
		stm << "ListID: " << (int) it->second << "\n";
	}
	return stm.str();
}

string Worker::printPortFd() {
	boost::mutex::scoped_lock scoped_lock(m_mutex);

	ostringstream stm;
	stm << "portFd size = " << portFd.size() << " in socketWoker " << workerId
			<< "\n";
	for (intTable::const_iterator it = portFd.begin(); it != portFd.end(); ++it) {

		stm << (*it).first << " " << (*it).second << " in socketWoker "
				<< workerId + "\n";
	}
	return stm.str();
}

time_t Worker::readTimeStamp(int src_id) {
	return this->getStreamList(src_id)->getTimeStamp();
}

void Worker::sendMessage(Message message) {
	try {
		//Open a message queue.
		boost::interprocess::message_queue mq(
				boost::interprocess::open_or_create, "message_queue", 100,
				sizeof(Message));

		mq.send(&message, sizeof(Message), 0);

	} catch (boost::interprocess::interprocess_exception &ex) {

		boost::interprocess::message_queue::remove("message_queue");
		std::cout << ex.what() << std::endl;
		return;
	}
}
