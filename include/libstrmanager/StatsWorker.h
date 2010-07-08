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

#ifndef STATSWORKER_H_
#define STATSWORKER_H_

#include <libstrmanager/Worker.h>

/**
 * StatsWorker, processes the incoming streams from the sources and forwards the information to all the destinations,
 * estimates load per Worker.
 */

class StatsWorker: public Worker {

public:
	/**
	 * Constructor of the StatsWorker object.
	 * @param workerId an integer argument, with the worker identifier.
	 */
	StatsWorker(int workerId);

	bool addSource(int src_id, char* ip, int port);

private:

	int inputBytes;
	int outputBytes;
	boost::thread* stats_thread; /** a boost::thread pointer, that holds the thread memory address.*/
	boost::mutex stats_mutex; /**< a boost::mutex attribute, to perform synchronization between threads.*/
	/**
	 * process method that executes the action for an incoming datagram.
	 * @param fd file descriptor of the incomming information socket.
	 */
	void process(int fd);

	void generateStats();
};

#endif /* STATSWORKER_H_ */
