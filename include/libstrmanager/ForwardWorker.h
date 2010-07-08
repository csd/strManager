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

#ifndef FORWARDWORKER_H_
#define FORWARDWORKER_H_

#include <libstrmanager/Worker.h>

/**
 * ForwardWorker, processes the incoming streams from the sources and forwards the information to all the destinations.
 */

class ForwardWorker: public Worker {

public:
	/**
	 * Constructor of the ForwardWorker object.
	 * @param workerId an integer argument, with the worker identifier.
	 */
	ForwardWorker(int workerId);

private:
	/**
	 * process method that executes the action for an incoming datagram.
	 * @param fd file descriptor of the incomming information socket.
	 */
	void process(int fd);
};

#endif /* FORWARDWORKER_H_ */
