/*
 * Constants.h
 *
 *  Created on: Oct 13, 2016
 *      Author: lord
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <sys/epoll.h>

#include <errno.h>
#include <string.h>

#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <sstream>
#include <memory>

#define MESSAGE_PART_LEN 1024
#define MAX_EVENTS 32

#endif /* CONSTANTS_H_ */
