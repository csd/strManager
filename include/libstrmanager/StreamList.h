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

#ifndef STREAMLIST_H_
#define STREAMLIST_H_

#include <list>
#include <tr1/memory>
#include <libstrmanager/UdpStream.h>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace std;

/**
 * Object that contains a list of udpStream destinations, a reference to the udpStream source object
 * and a unique identifier for each the streamList.
 */

class StreamList {
private:

	int listId; /**< an integer parameter, that identifies the streamList.*/
	std::tr1::shared_ptr<UdpStream> source; /**< an udpStream parameter, that contains the source.*/
	list<std::tr1::shared_ptr<UdpStream> > destinations; /**< an udpStream list, that contains the destinations of the source.*/
	
protected:

	time_t timeStamp; /**< a time_t register with the last time when a packet from this streamList was received. */

public:

	/**
	 * Constructor of the streamList object.
	 */
	StreamList();

	/**
	 * List identifier setter.
	 * @param listId an integer argument, that identifies the streamList.
	 */
	void setListId(int listId);

	/**
	 * List identifier getter.
	 * @return an integer argument, that identifies the streamList.
	 */
	int getListId();

	/**
	 * Source udpStream setter.
	 * @param source an udpStream argument, that contains the source object of the streamList.
	 */
	void setSource(std::tr1::shared_ptr<UdpStream> source);

	/**
	 * Source udpStream getter.
	 * @return an udpStream the source object of the streamList.
	 */
	std::tr1::shared_ptr<UdpStream> getSource();

	/**
	 * Source port getter.
	 * @return an int with the source port of the streamList.
	 */
	int getSourcePort();

	/**
	 * Source udpStream deleter, deletes the udpStream source reference at the streamList.
	 */
	void remSource();

	/**
	 * Source SockAdress getter.
	 * @return the sockaddr_in structure of the source udpStream object.
	 */
	sockaddr_in getSourceSockAddr();

	/**
	 * Destination udpStream add method, adds a new destination to the streamList object.
	 * @param destination an udpStream argument, with the destination values.
	 * @return true if Ok and false if KO
	 */
	bool addDestination(std::tr1::shared_ptr<UdpStream> destination);

	/**
	 * Destination udpStream get method, returns the desired destination UdpStream.
	 * @param id integer that identifies the udpStream.
	 * @return the destination UdpStream object.
	 */
	std::tr1::shared_ptr<UdpStream> getDestination(int id);

	/**
	 * Destination udpStream pop method, returns the desired destination UdpStream
	 * and erases from streamList.
	 * @param id integer that identifies the udpStream.
	 * @return the destination UdpStream object.
	 */
	std::tr1::shared_ptr<UdpStream> popDestination(int id);

	/**
	 * Find a destination with given identifier.
	 * @param id integer that identifies the searched udpStream.
	 * @return An iterator that holds the position found at the list.
	 */
	list<std::tr1::shared_ptr<UdpStream> >::iterator findDestination(int id);

	/**
	 * Checks if a destination with given identifier exissts.
	 * @param id integer that identifies the searched udpStream.
	 * @return boolean attribute true if exists, false if not.
	 */
	bool existDestination(int id);

	/**
	 * Destination list setter.
	 * @param destinations list of udpStream objects that will be destinations of the stream.
	 */
	void setDestinations(list<std::tr1::shared_ptr<UdpStream> > destinations);

	/**
	 * Destination list getter.
	 * @return the destinations list.
	 */
	list<std::tr1::shared_ptr<UdpStream> > getDestinations();

	/**
	 * Destination udpStream deleter, deletes the udpStream destination reference at the streamList.
	 * @param id integer that identifies the destination udpStream.
	 * @return true if removed, false if not.
	 */
	bool remDestination(int id);

	/**
	 * Auxiliary method that clears all the values contained in a streamList.
	 */
	void clearDestinations();

	/**
	 * Auxiliary method that prints the values of the streamList in a human readable form.
	 * @return string with the information.
	 */
	string printList();

	/**
	 * Auxiliary method that clears the all the values of the streamList.
	 */
	void clear();

	/**
	 * Destructor of the streamList object.
	 */
	virtual ~StreamList();
	
	/**
	 * Timestamp updater at the current time.
	 */
	void updateTimeStamp();
	
	/**
	 * Getter for the timestamp value.
	 */
	time_t getTimeStamp();
};

#endif /*STREAMLIST_H_*/
