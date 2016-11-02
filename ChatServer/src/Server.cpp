/*
 * Server.cpp
 *
 *  Created on: Oct 13, 2016
 *      Author: lord
 */

#include "Server.h"
#include "Client.h"

const char* GREETING = "Welcome to hell!\n";
const size_t GREEINTG_LEN = 17;

Server::Server(int port) :
	m_portNumber { port },
	m_masterSocketFd { 0 } {
	// TODO Auto-generated constructor stub
}

int Server::start() {
	int MasterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(MasterSocket == -1)
	{
		std::cout << strerror(errno) << std::endl;
		return 1;
	}

	int enable = 1;
	if (setsockopt(MasterSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
		exit(1);
	}

	struct sockaddr_in SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(m_portNumber);
	SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int Result = bind(MasterSocket, (struct sockaddr *)&SockAddr, sizeof(SockAddr));

	if(Result == -1)
	{
		std::cout << strerror(errno) << std::endl;
		return 1;
	}

	set_nonblock(MasterSocket);

	Result = listen(MasterSocket, SOMAXCONN);

	if(Result == -1)
	{
		std::cout << strerror(errno) << std::endl;
		return 1;
	}

	struct epoll_event Event;
	Event.data.fd = MasterSocket;
	Event.events = EPOLLIN;

	struct epoll_event * Events;
	Events = (struct epoll_event *) calloc(MAX_EVENTS, sizeof(struct epoll_event));

	int EPoll = epoll_create1(0);
	epoll_ctl(EPoll, EPOLL_CTL_ADD, MasterSocket, &Event);

	while(true)
	{
		size_t N = size_t(epoll_wait(EPoll, Events, MAX_EVENTS, -1));
		for (size_t i = 0; i < N; i++)
		{
			if((Events[i].events & EPOLLERR)||(Events[i].events & EPOLLHUP))
			{
				if (Events[i].events & EPOLLHUP) {
					std::cout << "connection terminated" << std::endl;
				}
				shutdown(Events[i].data.fd, SHUT_RDWR);
				close(Events[i].data.fd);
			}
			else if(Events[i].data.fd == MasterSocket)
			{
				int SlaveSocket = accept(MasterSocket, 0, 0);
				set_nonblock(SlaveSocket);

				struct epoll_event Event;
				Event.data.fd = SlaveSocket;
				Event.events = EPOLLIN;

				epoll_ctl(EPoll, EPOLL_CTL_ADD, SlaveSocket, &Event);
				Client* client_ptr = new Client(SlaveSocket, this);
				m_clients[SlaveSocket] = std::unique_ptr<Client>(client_ptr);
				std::cout << "accepted connection" << std::endl;
				send(SlaveSocket, GREETING, GREEINTG_LEN, MSG_NOSIGNAL);
			}
			else
			{
				Client* cl_ptr = m_clients[Events[i].data.fd].get();
				static char Buffer[MESSAGE_PART_LEN];
				int RecvSize = recv(Events[i].data.fd, Buffer, 1024, MSG_NOSIGNAL);
				if (RecvSize <= 0) {
					epoll_ctl(EPoll, EPOLL_CTL_DEL, Events[i].data.fd, &Event);
					m_clients.erase(Events[i].data.fd);
					std::cout << "connection terminated" << std::endl;
				}
				else {
					cl_ptr->schedule_message_cast(Buffer, RecvSize);
				}
			}
		}
	}

	return 0;
}

int Server::set_nonblock(int fd)
{
	int flags;
#if defined(O_NONBLOCK)
	if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
		flags = 0;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
	flags = 1;
	return ioctl(fd, FIOBIO, &flags);
#endif
}

void Server::broadcast(const char* pData, size_t len) {
	for (auto client : m_clients) {
		int fd = client.first;
		send(fd, pData, len, MSG_NOSIGNAL);
	}
	char stringBuffer[MESSAGE_PART_LEN + 1];
	memcpy(stringBuffer, pData, len);
	stringBuffer[len] = '\0';
	std::cout << std::string(stringBuffer)<< std::flush;
}
