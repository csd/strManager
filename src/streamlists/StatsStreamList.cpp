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

#include <libstrmanager/StatsStreamList.h>
#include "iostream"

StatsStreamList::StatsStreamList() : StreamList () {	// TODO Auto-generated constructor stub
	this->actualRate = 0;
	this->limitRate = 0;
	this->inputBytes = 0;
	time (&timeStamp);

}

bool StatsStreamList::update() {

	time_t now;
	time (&now);

	int diff = difftime (now,this->timeStamp);

	if (diff > 10)
	{
		this->calculateRate(diff);
		cout << "list_id " << getListId() << " rate= "<< actualRate << endl;
		this->updateTimeStamp();
	}

}

void StatsStreamList::calculateRate(int diff) {

	actualRate = (inputBytes/125) / diff;
	list_mutex.lock();
	this->inputBytes = 0;
	list_mutex.unlock();
}

void StatsStreamList::updateInputBytes(int length) {
	list_mutex.lock();
	this->inputBytes = this->inputBytes + length;
	list_mutex.unlock();
}

StatsStreamList::~StatsStreamList() {
	// TODO Auto-generated destructor stub
}
