/*
 * \brief  Network echo tool
 * \author Christian Helmuth
 * \date   2020-03-10
 */

/*
 * Copyright (C) 2020 Genode Labs GmbH
 *
 * This file is distributed under the terms of the GNU Affero General Public
 * License version 3.
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <errno.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define DIE(step) \
	do {               \
		perror(step);  \
		exit(1);       \
	} while (0)


static char const * const finalizer = "\n";

int main(int argc, char **argv)
{
	if (argc != 4) {
		fprintf(stderr, "usage: %s <ip> <port> <msg>\n", argv[0]);
		exit(1);
	}

	char const *   const ip   = argv[1];
	unsigned short const port = strtoul(argv[2], nullptr, 0);
	char const *   const data = argv[3];

	int sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) DIE("socket");

#ifdef __linux__
	sockaddr_in const addr {               AF_INET, htons(port), { inet_addr(ip) }, { 0 } };
#else
	sockaddr_in const addr { sizeof(addr), AF_INET, htons(port), { inet_addr(ip) }, { 0 } };
#endif
	sockaddr const * const paddr = reinterpret_cast<sockaddr const *>(&addr);

	int ret = 0;

	ret = connect(sd, paddr, sizeof(addr));
	if (ret == -1) DIE("connect");

	ret = send(sd, data, strlen(data), 0);
	if (ret == -1) DIE("send");

	ret = send(sd, finalizer, strlen(finalizer), 0);
	if (ret == -1) DIE("send");

	ret = shutdown(sd, SHUT_RDWR);
	if (ret == -1) DIE("shutdown");

	ret = close(sd);
	if (ret == -1) DIE("close");
}
