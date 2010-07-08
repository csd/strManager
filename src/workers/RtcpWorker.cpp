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

#include <libstrmanager/RtcpWorker.h>
#include <libstrmanager/Constants.h>

using namespace log4cxx;

static LoggerPtr logger(Logger::getLogger("strManager"));

RtcpWorker::RtcpWorker(int workerId) : ForwardWorker (workerId) {
	setWorkerType(RTCP_WORKER_TYPE);
}

bool RtcpWorker::addSource(int src_id, char* ip, int port) {
	LOG4CXX_INFO(logger, "Adding source: " << src_id << " with address: " << ip << ":" << port << " in worker: " << workerId);
	
	boost::mutex::scoped_lock scoped_lock(m_mutex);
		
	//RTP flow in the even port
	std::tr1::shared_ptr<UdpStream> udpstream(new UdpStream());
	std::tr1::shared_ptr<StreamList> streamlist(new StreamList());
	
	//RTCP flow in the odd port
	std::tr1::shared_ptr<UdpStream> udpstream2(new UdpStream());
	std::tr1::shared_ptr<StreamList> streamlist2(new StreamList());
	
	// Setters of the UdpStream object
	udpstream->setId(src_id); 
	udpstream->setAddr(ip,port);
	
	udpstream2->setId(src_id+1); 
	udpstream2->setAddr(ip,port+1);
	
	// Check if a udpStream source with same values exists.
	if (this->getListIdBySocket(udpstream->getAddr()) || this->getListIdBySocket(udpstream2->getAddr()))
		return false;
		
	// Setters of the streamList object (streamList_id == source_id)
	streamlist->setListId(src_id);
	streamlist->setSource(udpstream);
	
	streamlist2->setListId(src_id+1);
	streamlist2->setSource(udpstream2);
	
	// Create the listening socket
	int ret = this->add_socket(port);
	int ret2 = this->add_socket(port+1);
	
	if(ret != -1 && ret2 != -1)
	{	
		// Call to the AddStreamList function to add to the internal structures.
		this->addStreamList(src_id,streamlist);
		this->addStreamList(src_id+1,streamlist2);
		
		return true;
	}
	
	return false;
}

void RtcpWorker::removeSource(int src_id) {
	LOG4CXX_INFO(logger, "Removing source: " << src_id << " in worker: " << workerId);
	
	boost::mutex::scoped_lock scoped_lock(m_mutex);
	
	// Try to remove the socket if necessary.
	this->remove_socket(this->getStreamList(src_id)->getSourcePort()); //RTP port
	this->remove_socket(this->getStreamList(src_id+1)->getSourcePort()); //RTCP port
	
	// If a source is deleted the streamList will be deleted too.	
	this->removeStreamList(src_id);
	this->removeStreamList(src_id+1);
}

void RtcpWorker::addDestination(int src_id, int dest_id, char* ip, int port){
	
	LOG4CXX_INFO(logger, "Adding destination on: " << src_id << " with destination: " << dest_id << " in worker: " << workerId);
	
	boost::mutex::scoped_lock scoped_lock(m_mutex);
	
	// Create a new udpstream
	std::tr1::shared_ptr<UdpStream> udpstream(new UdpStream());
	std::tr1::shared_ptr<UdpStream> udpstream2(new UdpStream());
		
	// Setters of the UdpStream object
	udpstream->setId(dest_id);
	udpstream->setAddr(ip,port);
	
	udpstream2->setId(dest_id+1);
	udpstream2->setAddr(ip,port+1);
	
	// Get the selected streamList and add the new udpStream object to the streamList
	std::tr1::shared_ptr<StreamList> streamlist = this->getStreamList(src_id);
	std::tr1::shared_ptr<StreamList> streamlist2 = this->getStreamList(src_id+1);
	
	if (streamlist->addDestination(udpstream) && streamlist2->addDestination(udpstream2))
	{
		this->addStreamList(src_id,streamlist); // Call to the AddStreamList function to add to the internal structures.
		this->addStreamList(src_id+1,streamlist2);
	}
}

void RtcpWorker::removeDestination(int src_id, int dest_id){
	LOG4CXX_INFO(logger, "Removing destination: " << dest_id << " of list: " << src_id);
	
	boost::mutex::scoped_lock scoped_lock(m_mutex);
	
	// Get the selected streamList and delete the new udpStream object to the streamList
	std::tr1::shared_ptr<StreamList> streamlist = this->getStreamList(src_id);
	std::tr1::shared_ptr<StreamList> streamlist2 = this->getStreamList(src_id+1);
	
	if(streamlist->existDestination(dest_id) && streamlist2->existDestination(dest_id+1)) {
		streamlist->remDestination(dest_id);
		streamlist2->remDestination(dest_id+1);
	
		// Call to the AddStreamList function to add to the internal structures.
		this->addStreamList(src_id,streamlist);
		this->addStreamList(src_id+1,streamlist2);
	}
	else {
		LOG4CXX_ERROR(logger, "Destination: " << dest_id << " of list: " << src_id << " not found ");
	}
}

int RtcpWorker::add_socket(int port) {
	LOG4CXX_DEBUG(logger, "Opening socket at port: " << port << " in worker: " << workerId);
	
	if (portListener[port]) {
		LOG4CXX_DEBUG(logger, "Reused opened socket at port: " << port << " in worker: " << workerId);
		portListener[port]++;
		return 0;
	}
		
	int fd = this->init_socket(port); //RTP port
	int fd2 = this->init_socket(port+1); //RTCP port
		
	//boost::mutex::scoped_lock scoped_lock(m_mutex);
	
	if(fd==-1 ) {
		LOG4CXX_ERROR(logger, "Problem opening the socket at port: " << port);
		return -1;
	}
	
	if(fd2==-1 ) {
		LOG4CXX_ERROR(logger, "Problem opening the socket at port: " << port+1);
		return -1;
	}
	
	//RTP port
	portFd[port] = fd;
	fdPort[fd] = port;
	portListener[port] = 1;
	
	//RTCP port
	portFd[port+1] = fd2;
	fdPort[fd2] = port+1;
	portListener[port+1] = 1;
	
	return 1;
}

bool RtcpWorker::remove_socket(int port) {
	LOG4CXX_DEBUG(logger, "Removing socket at port: " << port << " in worker: " << workerId);
	
	//boost::mutex::scoped_lock scoped_lock(m_mutex);
	
	portListener[port]--;
	
	if (portListener[port]==0)
	{
		// Get the fd of the port
		int fd = portFd[port];
		int fd2 = portFd[port+1]; 
		
		// delete references within the data structures
	
		//RTP port
		portFd.erase(port);	
		fdPort.erase(fd);
		portListener.erase(port);
		close(fd);
		
		//RTCP port
		portFd.erase(port+1);	
		fdPort.erase(fd2);
		portListener.erase(port+1);
		close(fd2);
		
		LOG4CXX_DEBUG(logger, "Closed socket at port: " << port << " with file descriptor " << fd << " at worker: " << workerId );
		
		return true;
	}
	
	return false;
}
