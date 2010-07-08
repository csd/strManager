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

#include <libstrmanager/ForwardWorker.h>
#include <libstrmanager/Constants.h>

using namespace log4cxx;

static LoggerPtr logger(Logger::getLogger("strManager"));

ForwardWorker::ForwardWorker(int id) : Worker (id) {
	setWorkerType(FWD_WORKER_TYPE);
}

void ForwardWorker::process(int fd){

	socklen_t flen, rlen;
	struct sockaddr_in from, a;
	char buf[2048];

	//Read the unicast packet.
	flen = sizeof (struct sockaddr_in);
	rlen = recvfrom (fd, buf, 2048, 0, (struct sockaddr *)&from, &flen);

	if (rlen < 0)
			LOG4CXX_ERROR(logger, "Error in recvfrom");
	if (rlen > 0)
	{
	int port = fdPort[fd];

	//cout << "Thread->" << threadId << ", detecta paquete en: " << port << endl;

		a = (sockaddr_in)from;
		a.sin_port = htons(port);

		//int sn = ntohl(((uint16_t*)(&buf[0]))[1]);

		std::tr1::shared_ptr<StreamList> b = this->getStreamListBySocket(a);

		if (!b) {
			LOG4CXX_WARN(logger,"NULL pointer streamList not found in worker " << getWorkerId() <<
					" while receiving a packet from: " << inet_ntoa(from.sin_addr));
			return;
		}
		
		//update last received time
		b->updateTimeStamp();

		list < std::tr1::shared_ptr<UdpStream> > c = b->getDestinations();

		list < std::tr1::shared_ptr<UdpStream> >::iterator it;

		for(it = c.begin(); it != c.end(); ++it)
		{
			sockaddr_in d= (*it)->getAddr();
			sendto (fd, buf, rlen, 0,
		        (struct sockaddr *)&d,
				sizeof (struct sockaddr_in));
		}
	}
}
