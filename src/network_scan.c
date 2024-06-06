/*
 * network_scan.c
 * 
 * Copyright 2024  <pi@raspberrypi>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>

unsigned short checksum(void *b, int len){
	unsigned short *buf = b;
	unsigned int sum = 0;
	unsigned short result;
	
	for (sum = 0; len > 1; len -= 2) {
		sum += *buf++;
	}
	
	if (len == 1) {
		sum += *(unsigned char*)buf;
	}
	
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}
int ping(const char *ip_addr) {
	int sockfd;
	struct sockaddr_in addr;
	struct icmp icmp_hdr;
	struct timeval start, end;
	char packet[64];
	int seq_num = 1;
	
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		perror("Socket error");
		return 1;
	}
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip_addr);
	
    // Setup ICMP header
    icmp_hdr.icmp_type = ICMP_ECHO;
    icmp_hdr.icmp_code = 0;
    icmp_hdr.icmp_id = getpid();
    icmp_hdr.icmp_seq = seq_num++;
    icmp_hdr.icmp_cksum = 0;
    //Fill packet
    memset(packet, 0, sizeof(packet));
    memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));
    // Checksum calc
    icmp_hdr.icmp_cksum = checksum(packet, sizeof(packet));
    memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));
    
    // Send packet
    gettimeofday(&start, NULL);
    if (sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr)) <= 0) {
		perror("Sending error");
		close(sockfd);
		return 1;
	}
	
	char buf[1024];
	struct sockaddr_in from;
	socklen_t from_len = sizeof(from);
    if (recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&from, &from_len) <= 0) {
        close(sockfd);
        return 1;
    }
    gettimeofday(&end, NULL);
    double time_taken = ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
    printf("Ping to %s took %.2f ms\n", ip_addr, time_taken);

    close(sockfd);
    return 0;
}

// Scan a subnet
void scan_network(const char *subnet) {
    char ip[16];
    for (int i = 1; i < 255; i++) {
        snprintf(ip, sizeof(ip), "%s.%d", subnet, i);
        if (ping(ip) == 0) {
            printf("%s is up\n", ip);
        }
    }
}
int main(int argc, char **argv)
{
	char subnet[] = "10.0.0";
	scan_network(subnet);
	return 0;
}

