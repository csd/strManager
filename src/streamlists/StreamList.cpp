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

#include <libstrmanager/StreamList.h>
#include <algorithm>
#include <iostream>

using namespace log4cxx;

static LoggerPtr logger(Logger::getLogger("strManager"));

StreamList::StreamList() {
	this->clear();
	//time (&timeStamp);
}

void StreamList::setListId(int listId) {
	this->listId = listId;
}

int StreamList::getListId() {
	return this->listId;
}

void StreamList::setSource(std::tr1::shared_ptr<UdpStream> source) {
	this->source = source;
}

void StreamList::remSource() {
	source.reset();
}

std::tr1::shared_ptr<UdpStream> StreamList::getSource() {
	return this->source;
}

int StreamList::getSourcePort() {
	return ntohs(source->getAddr().sin_port);
}

sockaddr_in StreamList::getSourceSockAddr() {
	return source->getAddr();
}

bool StreamList::addDestination(std::tr1::shared_ptr<UdpStream> destination) {
	if (this->existDestination(destination->getId())) {
		LOG4CXX_ERROR(logger,"Destination Id: " << destination->getId() << " already exists");
		return false;
	} else {
		destinations.push_back(destination);
		return true;
	}
}

std::tr1::shared_ptr<UdpStream> StreamList::getDestination(int id) {
	list<std::tr1::shared_ptr<UdpStream> >::iterator it =
			this->findDestination(id);
	std::tr1::shared_ptr<UdpStream> value;

	if ((*it)->getId() == id) // error control
	{
		value = (*it);
	}
	return value;
}

std::tr1::shared_ptr<UdpStream> StreamList::popDestination(int id) {
	list<std::tr1::shared_ptr<UdpStream> >::iterator it =
			this->findDestination(id);
	std::tr1::shared_ptr<UdpStream> value;

	if ((*it)->getId() == id) // error control
	{
		value = (*it);
		destinations.erase(it);
		//return value;
	}
	return value;

}

list<std::tr1::shared_ptr<UdpStream> >::iterator StreamList::findDestination(
		int id) {
	//Use existDestination first to check if that dest_id exists in the streamList
	list<std::tr1::shared_ptr<UdpStream> >::iterator it;

	it = destinations.begin();
	// do the search

	while (it != destinations.end()) {
		if ((*it)->getId() == id) {
			return it;
		}

		++it;
	}

	return it;
}

bool StreamList::existDestination(int id) {
	list<std::tr1::shared_ptr<UdpStream> >::iterator it;

	it = destinations.begin();
	// do the search

	while (it != destinations.end()) {
		if ((*it)->getId() == id) {
			return true;
		}
		++it;
	}

	return false;
}

list<std::tr1::shared_ptr<UdpStream> > StreamList::getDestinations() {
	return this->destinations;
}

void StreamList::setDestinations(
		list<std::tr1::shared_ptr<UdpStream> > destinations) {
	this->destinations = destinations;
}

void StreamList::clearDestinations() {
	this->destinations.clear();
}

bool StreamList::remDestination(int id) {
	list<std::tr1::shared_ptr<UdpStream> >::iterator it =
			this->findDestination(id);

	if ((*it)->getId() == id) // findDestination error control
	{
		destinations.erase(it);
		return true;
	}
	return false;
}

string StreamList::printList() {
	list<std::tr1::shared_ptr<UdpStream> >::iterator it;

	ostringstream stm;
	stm << "Printing streamList " << this->getListId() << "\nSource:\n"
			<< source->printStream() << "\nDestinations:\n";
	for (it = destinations.begin(); it != destinations.end(); ++it)
		stm << (*it)->printStream() << "\n";
	return stm.str();
}

void StreamList::clear() {
	this->setListId(0);
	this->destinations.clear();
}

void StreamList::updateTimeStamp() {
	time (&timeStamp);
}

time_t StreamList::getTimeStamp() {
	return timeStamp;
}

StreamList::~StreamList() {
}
