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

#include <libstrmanager/StatsWorker.h>
#include <libstrmanager/StatsStreamList.h>
#include <libstrmanager/Constants.h>

using namespace log4cxx;

static LoggerPtr logger(Logger::getLogger("strManager"));

StatsWorker::StatsWorker(int id) :
	Worker(id) {
	setWorkerType(STATS_WORKER_TYPE);
	this->inputBytes = 0;
	this->outputBytes = 0;
	stats_thread = new boost::thread(boost::bind(&StatsWorker::generateStats,
			this));
}

bool StatsWorker::addSource(int src_id, char* ip, int port) {
	LOG4CXX_INFO(logger, "Adding source: " << src_id << " with address: " << ip << ":" << port << " in worker: " << workerId);

	boost::mutex::scoped_lock scoped_lock(m_mutex);

	std::tr1::shared_ptr<UdpStream> udpstream(new UdpStream());
	std::tr1::shared_ptr<StatsStreamList> streamlist(new StatsStreamList());

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

void StatsWorker::process(int fd) {

	socklen_t flen, rlen;
	struct sockaddr_in from, a;
	char buf[2048];

	//Read the unicast packet.
	flen = sizeof(struct sockaddr_in);
	rlen = recvfrom(fd, buf, 2048, 0, (struct sockaddr *) &from, &flen);

	if (rlen < 0)
		LOG4CXX_ERROR(logger, "Error in recvfrom");
	if (rlen > 0) {

		int port = fdPort[fd];

		a = (sockaddr_in) from;
		a.sin_port = htons(port); // ÑAPA very ugly.

		std::tr1::shared_ptr<StreamList> tmp = this->getStreamListBySocket(a);

		std::tr1::shared_ptr<StatsStreamList> b =
				std::tr1::dynamic_pointer_cast<StatsStreamList>(tmp);

		if (!b) {
			LOG4CXX_WARN(logger,"NULL pointer streamList not found in worker " << getWorkerId() <<
					" while receiving a packet from: " << inet_ntoa(from.sin_addr));
			return;
		}

		list<std::tr1::shared_ptr<UdpStream> > c = b->getDestinations();

		stats_mutex.lock();
		this->inputBytes = this->inputBytes + rlen;
		//this->outputBytes = this->outputBytes + rlen * c.size();
		stats_mutex.unlock();

		b->updateInputBytes(rlen);

		for (list<std::tr1::shared_ptr<UdpStream> >::iterator it = c.begin(); it
				!= c.end(); ++it) {
			sockaddr_in d = (*it)->getAddr();
			sendto(fd, buf, rlen, 0, (struct sockaddr *) &d,
					sizeof(struct sockaddr_in));
		}
	}
}

void StatsWorker::generateStats() {
	while (this->isWorking()) {
		sleep(10);
		stats_mutex.lock();
		int inputInfo = this->inputBytes;
		int outputInfo = this->outputBytes;
		this->inputBytes = 0;
		this->outputBytes = 0;
		stats_mutex.unlock();

		int inputRate = inputInfo / 1250;
		int outputRate = outputInfo / 1250;
		int totalRate = inputRate + outputRate;

		cout << "Input\tOutput\tTotal (Kbps)" << endl;
		cout << inputRate << "\t" << outputRate << "\t" << totalRate << endl;

		for (managementTable::const_iterator it = management.begin(); it
				!= management.end(); ++it) {
			std::tr1::shared_ptr<StreamList> tmp = (std::tr1::shared_ptr<
					StreamList>) it->second;
			if (!tmp) {
				LOG4CXX_FATAL(logger,"Hash_map::iterator returned an empty object.");
			} else {
				std::tr1::shared_ptr<StatsStreamList> b =
						std::tr1::dynamic_pointer_cast<StatsStreamList>(tmp);
				b->update();

			}
		}
	}
}
