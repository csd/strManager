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

/**
 * @brief
 *
 * @mainpage C++ strManager library
 * <hr>
 * @authors
 *
 * Javier López Rubio 			<javi.lopez@i2cat.net> <br>
 * Guillem Cabrera Añon 		<guillem.cabrera@i2cat.net> <br>
 * Xavier Calvo Brugal 			<xavier.calvo-brugal@upc.edu> <br>
 * Julio Carlos Barrera Juez 	<julio.carlos.barrera@i2cat.net> <br>
 *
 * <hr>
 * @section intro Introduction
 * General description
 *
 * <hr>
 * @section requirements Requirements
 * requirements
 * <hr>
 *
 * @todo
 *
 * todo 1 <br>
 * todo 2 <br>
 * todo 3 <br>
 */

#ifndef MANAGER_H_
#define MANAGER_H_

#include <tr1/unordered_map>
#include <libstrmanager/Worker.h>

/**
 * workerTable unordered_map container that Maps an integer value to a Worker*.
 */

typedef std::tr1::unordered_map<int, Worker*> workerTable;

/**
 * Manager interface that controls all the Worker that will be used within the application.
 */

class Manager {
private:

	int currentWorkerId; /** an integer attribute, that holds the currentWorkerId.*/
	workerTable srcIdWorker; /** a workerTable attribute, that holds a reference from srcId to Worker.*/
	workerTable workerIdWorker; /** a workerTable attribute, that holds a reference from workerId to Worker.*/

	boost::thread* m_thread; /** a boost::thread pointer, that holds the thread memory address.*/
	bool isListeningMessages; /**  a bool attribute, true if is Listening to messages, false if not.*/

	/**
	 * Get Worker by port method, returns a reference to the desired worker.
	 * @param workerType an integer argument with workerType.
	 * @param port an integer argument with the port.
	 * @return Worker pointer with the desired worker.
	 */
	Worker* getWorker(int workerType, int port);
	
	/**
	 * Get Worker by id method, returns a reference to the desired worker.
	 * @param workerId an integer argument with the workerId.
	 * @return Worker pointer with the desired worker.
	 */
	Worker* getWorkerById(int workerId);
	
	/**
	 * Get Worker by Port and Type method, returns a reference to the desired worker.
	 * @param port an integer argument with the port.
	 * @param workerType an integer argument with workerType.
	 * @return Worker pointer with the desired worker.
	 */
	Worker* getWorkerByPortType(int port, int workerType);
	
	/**
	 * Get Worker by type method, returns a reference to the desired worker.
	 * @param workerType an integer argument with workerType..
	 * @return Worker pointer with the desired worker.
	 */
	Worker* getWorkerByType(int workerType);
	
	/**
	 * Create Worker by type method, returns a reference to the desired worker.
	 * @param workerType an integer argument with workerType..
	 * @return Worker pointer with the desired worker.
	 */
	Worker* createWorker(int workerType);

public:

	/**
	 * Constructor of the Manager object.
	 */
	Manager();
	
	/**
	 * Add source method, creates a new streamList object and sets source with the given parameters.
	 * @param workerType an integer argument with the workerType value.
	 * @param src_id an integer argument with the source identifier.
	 * @param ip a char array that contains the source IP address.
	 * @param port an integer argument with the destination port.
	 * @return 0 if no errors, otherwise error code.
	 */
	int addSource(int workerType, int src_id, char* ip, int port);
	
	/**
	 * Remove source methed, erases the source and the streamList associated with the source.
	 * @param src_id an integer argument with the source identifier.
	 * @return 0 if no errors, otherwise error code.
	 */
	int removeSource(int src_id);
	
	/**
	 * Add destination method, gets the asociated streamList object and sets destination with the given parameters.
	 * @param src_id an integer argument with the streamList identifier.
	 * @param dest_id an integer argument with the destination identifier.
	 * @param ip a char array that contains the destination IP address.
	 * @param port an integer argument with the destination port.
	 * @return 0 if no errors, otherwise error code.
	 */
	int addDestination(int src_id, int dest_id, char* ip, int port);
	
	/**
	 * Remove destination, removes a destination udpStream in the specifed streamList.
	 * @param src_id an integer argument with the streamList identifier.
	 * @param dest_id an integer argument with the destination identifier.
	 * @return 0 if no errors, otherwise error code.
	 */
	int removeDestination(int src_id, int dest_id);
	
	/**
	 * Method that that launch the messageHandler to process messages from Workers.
	 */
	void messageHandler();
	
	/**
	 * Auxiliary method that prints the values of Management Map in a human readable form.
	 * @return an string with the information.
	 */
	string printManagement();
	
	/**
	 * Auxiliary method that prints the values of Forwarding Map in a human readable form.
	 * @return an string with the information.
	 */
	string printForwarding();
	
	/**
	 * Auxiliary method that prints the values of PortFd Map in a human readable form.
	 * @return an string with the information.
	 */
	string printPortFd();
	
	/**
	 * Method that reads the last time a packet from the src_id parameter was received.
	 * @param src_id an integer argument with the streamList identifier.
	 * @return a time_t with the last time of a packet.
	 */
	time_t readTimeStamp(int src_id);
	
	
	/**
	 * @param src_id an integer argument with the streamList identifier.
	 * Destructor of the Manager object
	 */
	virtual ~Manager();
};

#endif /* MANAGER_H_ */
