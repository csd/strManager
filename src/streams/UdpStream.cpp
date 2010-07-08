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

#include <libstrmanager/UdpStream.h>
#include <arpa/inet.h> //inet_aton inet_nton
#include <iostream>

using namespace log4cxx;

static LoggerPtr logger(Logger::getLogger("strManager"));

UdpStream::UdpStream() {
	this->setId(0);
}

UdpStream::UdpStream(int id, char* ip, int port) {
	this->setId(id);
	this->setAddr(ip, port);
}

void UdpStream::setId(int id) {
	this->id = id;
}

int UdpStream::getId() {
	return this->id;
}

void UdpStream::setAddr(struct sockaddr_in addr) {
	this->addr = addr;
}

void UdpStream::setAddr(char *ip, int port) {
	this->addr.sin_family = AF_INET;
	this->addr.sin_port = htons(port);
	inet_aton(ip, &this->addr.sin_addr);
}

sockaddr_in UdpStream::getAddr() {
	return this->addr;
}

string UdpStream::printStream() {
	ostringstream stm;
	stm << "udpStream: " << this->getId() << " " << inet_ntoa(
			this->addr.sin_addr) << ":" << ntohs(this->addr.sin_port) << "\n";
	return stm.str();
}

void UdpStream::clear() {
	//this->setId(0);
	//this->setAddr("0.0.0.0",0);
}

UdpStream::~UdpStream() {

}
