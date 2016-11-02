/*
 * Server.h
 *
 *  Created on: Oct 13, 2016
 *      Author: lord
 */

#ifndef SERVER_H_
#define SERVER_H_

#pragma once
#include "Constants.h"

class Client;

class Server {
public:
	Server(int port);
	int start();
	void broadcast(const char* pData, size_t len);
private:
	int m_portNumber;
	int m_masterSocketFd;
	std::map<int, std::shared_ptr<Client>> m_clients;
	static int set_nonblock(int fd);
};

#endif /* SERVER_H_ */
