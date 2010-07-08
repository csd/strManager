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

#ifndef WORKER_H_
#define WORKER_H_

#include <libstrmanager/StreamList.h>
#include <libstrmanager/Message.h>

#include <tr1/unordered_map>
#include <boost/thread/thread.hpp>
#include <arpa/inet.h>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <boost/interprocess/ipc/message_queue.hpp>

using namespace std;

/**
 *  sock_addr_in comparer
 */

struct sock_addr_cmp {
	bool operator()(const sockaddr_in s1, const sockaddr_in s2) const {

		if (s1.sin_addr.s_addr == s2.sin_addr.s_addr && s1.sin_port
				== s2.sin_port) {
			return true;
		}

		return false;
	}
};

/**
 *  sock_addr_in hasher
 */

struct sock_addr_hasher {

	int operator()(const sockaddr_in s) const {
		return s.sin_port + (s.sin_addr.s_addr ^ 2);
	}
};

typedef std::tr1::unordered_map<int, int> intTable;
typedef std::tr1::unordered_map<int, std::tr1::shared_ptr<StreamList> >
		managementTable;
typedef std::tr1::unordered_map<const sockaddr_in, int, sock_addr_hasher,
		sock_addr_cmp> forwardingTable;

/**
 * Generic Worker, default implementation for a Worker,  process method not defined.
 * To develop a new Worker type, it is necessary to inherit from this class,
 * and implement the constructor and the process method, for a practical example
 * check the ForwarWorker class.
 */

class Worker {
public:

	intTable fdPort; /**< hash_map attribute, used to relation fd with port.*/

	/**
	 * Constructor of the Worker object.
	 * @param workerId
	 *		integer argument, with the worker identifier.
	 */
	Worker(int workerId);

	/**
	 * Destructor of the socketWorker object
	 */
	~Worker();

	/**
	 * Worker Id getter
	 * @return an integer value, that identifies the worker.
	 */
	int getWorkerId();

	/**
	 * hasPort method, asks to the worker if it is controlling the given port.
	 * @param port
	 * 		integer argument, that identifies the stream.
	 * @return a boolean value, true if it is controlling the port, false if not.
	 */
	bool hasPort(int port);

	/**
	 * go method that starts the new thread, binds and the run the worker.
	 */
	void launch();

	/**
	 * stop method that stops the current worker.
	 */
	void stop();

	// ********************** Select() manipulation methods ************************ //

	/**
	 * build_select_list method that creates the fd_set structure.
	 */
	void build_select_list();

	/**
	 * check_select_list method that checks the returned fd_set structure.
	 */
	void check_select_list();

	// ********************** Packet processing methods ************************ //

	/**
	 * process method that executes the action for an incoming datagram.
	 * VIRTUAL METHOD! Should be implemented in the inherited class!
	 * @param fd
	 * 		file descriptor of the incomming information socket.
	 */
	virtual void process(int fd);

	// ********************** Streams management methods ************************ //

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
	virtual bool addSource(int src_id, char* ip, int port);

	/**
	 * Remove source methed, erases the source and the streamList associated with the source.
	 * @param src_id
	 * 		integer argument with the source identifier.
	 */
	virtual void removeSource(int src_id);

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
	virtual void addDestination(int src_id, int dest_id, char* ip, int port);

	/**
	 * Remove destination, removes a destination udpStream in the specifed streamList.
	 * @param src_id
	 * 		integer argument with the streamList identifier.
	 * @param dest_id
	 * 		integer argument with the destination identifier.
	 */
	virtual void removeDestination(int src_id, int dest_id);

	/**
	 * Get Streamlist by socket method, returns a streamList object with a given sock_addr_in struct.
	 * @param addr
	 * 		sock_addr_in structure that contains IP Address, used transport protocol, and used port.
	 * @return the searched streamList object.
	 */
	std::tr1::shared_ptr<StreamList> getStreamListBySocket(sockaddr_in addr);

	/**
	 * StreamList conmute method, moves all the destinations from a StreamList to another StreamList
	 * @param from
	 * 		integer argument, with the "from" StreamList identifier.
	 * @param to
	 * 		integer argument, with the "to" StreamList identifier.
	 */
	void conmuteStreamList(int from, int to);

	/**
	 * UdpStream conmute method, moves one destination from a StreamList to another StreamList
	 * @param id
	 * 		integer argument, with the id of the moved UdpStream.
	 * @param from
	 * 		integer argument, with the "from" StreamList identifier.
	 * @param to
	 * 		integer argument, with the "to" StreamList identifier.
	 */
	void conmuteUdpStream(int id, int from, int to);

	/**
	 * Auxiliary method that prints the values of Management HashMap in a human readable way.
	 * @return string with the information.
	 */
	string printManagement();

	/**
	 * Auxiliary method that prints a value of Management HashMap in a human readable way.
	 * @param src_id
	 * 		integer argument with the streamList identifier.
	 * @return string with the information.
	 */

	string printList(int src_id);

	/**
	 * Auxiliary method that prints the values of Forwarding HashMap in a human readable way.
	 * @return string with the information.
	 */
	string printForwarding();

	/**
	 * Auxiliary method that prints the values of Port File Descriptor in a human readable way.
	 * @return string with the information.
	 */
	string printPortFd();

	/**
	 * Getter for the WorkerType value.
	 * @return an integer value with the WorkerType.
	 */
	int getWorkerType();
	
	/**
	 * Setter for the WorkerType value.
	 */
	void setWorkerType(int workerType);
	/**
	 * Checks if the Worker is the WorkerType that we are looking for.
	 * @return boolean attribute, true if it is, false if not.
	 */
	bool isWorkerType(int workerType);

	/**
	 * Checks if the Worker is running.
	 * @return boolean attribute, true if it is, false if not.
	 */
	bool isWorking();
	
	
	/**
	 * Method that reads the last time a packet from the src_id parameter was received.
	 * @param src_id an integer argument with the streamList identifier.
	 * @return a time_t with the last time of a packet.
	 */
	time_t readTimeStamp(int src_id);

protected:

	volatile bool working; /**< a boolean attribute, to show the thread state.*/

	int workerId; /**< an integer attribute, that holds the workerId.*/
	int workerType; /**< an integer attribute, that holds the wokrerType.*/

	boost::thread* m_thread; /**< a boost::thread pointer, that holds the thread memory address.*/
	boost::mutex m_mutex; /**< a boost::mutex attribute, to perform synchronization between threads.*/

	intTable portFd; /**< hash_map attribute, used to relation port with fd.*/

	intTable portListener; /**< hash_map attribute, used to relation port with number of listeners.*/

	managementTable management; /**< hash_map attribute, used for management task of the application.*/
	forwardingTable forwarding; /**< hash_map attribute, used for packet forwarding tasks of the application.*/

	int highsock; /**< an integer attribute, that holds the highest socket fd.*/
	fd_set fdset; /**< a fd_set structure, to perform the select.*/
	timeval timeout; /**< a timeval structure, to setup the select timeout.*/

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

	/**
	 * socketWorker init_socket method, creates a listening socket and returns the fd.
	 * @param port
	 * 		integer argument, with the desired port.
	 * @return int fd, file descriptor of the new socket.
	 */
	int init_socket(int port);

	/**
	 * socketWorker isEmpty method, checks if the socketWorker has any streamList assigned
	 * @return boolean, true if has, false if not.
	 */
	bool isEmpty();

	/**
	 * StreamList add method, adds a new StreamList to the manager.
	 * @param listId
	 * 		integer argument, with the StreamList identifier.
	 * @param sList
	 * 		streamList argument, with the StreamList object that will be added.
	 */
	void addStreamList(int listId, std::tr1::shared_ptr<StreamList> sList);

	/**
	 * StreamList remove method, deletes a StreamList from the manager
	 * @param listId
	 * 		integer argument, with the StreamList identifier.
	 */
	void removeStreamList(int listId);

	/**
	 * Get list id by socket method, returns the streamList identifier with a given sock_addr_in struct.
	 * @param addr
	 * 		sock_addr_in structure that contains IP Address, used transport protocol, and used port.
	 * @return an integer argument with the streamList identifier that has been found.
	 */
	int getListIdBySocket(sockaddr_in addr);

	/**
	 * Get StreamList method, returns the streamList object with a given streamList identifier.
	 * @param listId
	 * 		integer argument with the streamList identifier.
	 * @return the searched streamList object.
	 */
	std::tr1::shared_ptr<StreamList> getStreamList(int listId);

	/**
	 * run thread method.
	 */
	void run();

	/**
	 * Send Message method, this method sends a new Message through the Message Queue.
	 * @param message
	 * 		object with the message that will be sent.
	 */
	void sendMessage(Message message);
};

#endif /*WORKER_H_*/
