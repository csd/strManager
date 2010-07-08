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

#ifndef UDPSTREAM_H_
#define UDPSTREAM_H_

#include <netinet/in.h>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace std;

/**
 * 	Object that will hold all the necessary information from a stream.
 */

class UdpStream {
private:

	int id; /**< an integer parameter, that identifies the stream.*/
	struct sockaddr_in addr; /**< a sock_addr_in structure that contains IP Address, transport protocol, and used port.*/

public:

	/**
	 * Constructor of the udpStream object
	 */
	UdpStream();

	/**
	 * Constructor of the udpStream object with given parameters:
	 * @param src_id
	 * 		integer argument, that identifies the stream.
	 * @param ip
	 * 		char array that contains the IP address of the stream.
	 * @param port
	 * 		integer argument, that has the port of the stream.
	 */
	UdpStream(int src_id, char* ip, int port);

	/**
	 * Id setter
	 * @param id
	 * 		integer argument, that identifies the stream.
	 */
	void setId(int id);

	/**
	 * Id getter
	 * @return an integer value, that identifies the stream.
	 */
	int getId();

	/**
	 * Address setter
	 * @param addr
	 * 		sockaddr_in structure that contains IP Address, used transport protocol, and used port.
	 */
	void setAddr(struct sockaddr_in addr);

	/**
	 * Address setter
	 * @param ip a char array that contains the IP address of the stream.
	 * @param port an integer argument, that has the port of the stream.
	 */
	void setAddr(char *ip, int port);

	/**
	 * Address getter
	 * @return a sock_addr_in structure that contains IP Address, used transport protocol, and used port.
	 */
	sockaddr_in getAddr();

	/**
	 * Auxiliary method that prints the value of the udpStream in a human readable form.
	 * @return string with the information.
	 */
	string printStream();

	/**
	 * Auxiliary method that clears all the values contained in a udpStream.
	 */
	void clear();

	/**
	 * Destructor of the udpStream object
	 */
	virtual ~UdpStream();
};

#endif /*UDPSTREAM_H_*/
