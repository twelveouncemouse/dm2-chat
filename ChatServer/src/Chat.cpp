//============================================================================
// Name        : Chat.cpp
// Author      : Roman Vasilyev
// Version     :
// Copyright   : Created for Technosphere Project @Mail.ru
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "Constants.h"
#include "Server.h"

int main(int argc, char **argv)
{
	Server serv(3100);
	serv.start();
	return 0;
}
