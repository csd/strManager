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

#ifndef RTCPWORKER_H_
#define RTCPWORKER_H_

#include <libstrmanager/ForwardWorker.h>

/**
 * ForwardWorker, processes the incoming streams from the sources and forwards the information to all the destinations.
 */

class RtcpWorker: public ForwardWorker {

public:
	/**
	 * Constructor of the RtcpWorker object.
	 * @param workerId an integer argument, with the worker identifier.
	 */
	RtcpWorker(int workerId);
	
	/**
	 * Add source method, creates a new streamList object and sets source with the given parameters.
	 * @param src_id
	 * 		integer argument with the source identifier.
	 * @param ip
	 * 		char array that contains the source IP address.
	 * @param port
	 * 		integer argument with the destination port.
	 * @return a boolean value, true if it is added, false if not.
	 */
	bool addSource(int src_id, char* ip, int port);

	/**
	 * Remove source methed, erases the source and the streamList associated with the source.
	 * @param src_id
	 * 		integer argument with the source identifier.
	 */
	void removeSource(int src_id);

	/**
	 * Add destination method, gets the asociated streamList object and sets destination with the given parameters.
	 * @param src_id
	 * 		integer argument with the streamList identifier.
	 * @param dest_id
	 * 		integer argument with the destination identifier.
	 * @param ip
	 * 		char array that contains the destination IP address.
	 * @param port
	 * 		integer argument with the destination port.
	 */
	void addDestination(int src_id, int dest_id, char* ip, int port);

	/**
	 * Remove destination, removes a destination udpStream in the specifed streamList.
	 * @param src_id
	 * 		integer argument with the streamList identifier.
	 * @param dest_id
	 * 		integer argument with the destination identifier.
	 */
	void removeDestination(int src_id, int dest_id);

protected:
	/**
	 * socketWorker add_socket method, adds a listening socket to this worker.
	 * @param port
	 * 		integer argument, with the desired port.
	 * @return a int value, 1 if added, 0 if already open, -1 if a problem was found.
	 */
	int add_socket(int port);

	/**
	 * socketWorker remove_socket method, removes a listening socket in this worker.
	 * @param port
	 * 		integer argument, with the desired port.
	 * @return a boolean value, true if removed, false if still open.
	 */
	bool remove_socket(int port);
	
};

#endif /* RTCPWORKER_H_ */
