/**
 * \addtogroup uip
 * @{
 */

/**
 * \defgroup uiparp uIP Address Resolution Protocol
 * @{
 *
 * The Address Resolution Protocol ARP is used for mapping between IP
 * addresses and link level addresses such as the Ethernet MAC
 * addresses. ARP uses broadcast queries to ask for the link level
 * address of a known IP address and the host which is configured with
 * the IP address for which the query was meant, will respond with its
 * link level address.
 *
 * \note This ARP implementation only supports Ethernet.
 */

/**
 * \file
 * Implementation of the ARP Address Resolution Protocol.
 * \author Adam Dunkels <adam@dunkels.com>
 *
 */

/*
 * Copyright (c) 2001-2003, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: uip_arp.c,v 1.5 2008/02/07 01:35:00 adamdunkels Exp $
 *
 */


#include "uip_arp.h"

#include <string.h>

struct arp_hdr {
  struct uip_eth_hdr ethhdr;
  u16_t hwtype;
  u16_t protocol;
  u8_t hwlen;
  u8_t protolen;
  u16_t opcode;
  struct uip_eth_addr shwaddr;
  uip_ipaddr_t sipaddr;
  struct uip_eth_addr dhwaddr;
  uip_ipaddr_t dipaddr;
};

struct ethip_hdr {
  struct uip_eth_hdr ethhdr;
  /* IP header. */
  u8_t vhl,
    tos,
    len[2],
    ipid[2],
    ipoffset[2],
    ttl,
    proto;
  u16_t ipchksum;
  uip_ipaddr_t srcipaddr, destipaddr;
};

#define ARP_REQUEST 1
#define ARP_REPLY   2

#define ARP_HWTYPE_ETH 1


#define BUF   ((struct arp_hdr *)&uip_buf[0])
#define IPBUF ((struct ethip_hdr *)&uip_buf[0])

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


/*-----------------------------------------------------------------------------------*/
/**
 * ARP processing for incoming ARP packets.
 *
 * This function should be called by the device driver when an ARP
 * packet has been received. The function will act differently
 * depending on the ARP packet type: if it is a reply for a request
 * that we previously sent out, the ARP cache will be filled in with
 * the values from the ARP reply. If the incoming ARP packet is an ARP
 * request for our IP address, an ARP reply packet is created and put
 * into the uip_buf[] buffer.
 *
 * When the function returns, the value of the global variable uip_len
 * indicates whether the device driver should send out a packet or
 * not. If uip_len is zero, no packet should be sent. If uip_len is
 * non-zero, it contains the length of the outbound packet that is
 * present in the uip_buf[] buffer.
 *
 * This function expects an ARP packet with a prepended Ethernet
 * header in the uip_buf[] buffer, and the length of the packet in the
 * global variable uip_len.
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_arpin(void)
{
  if(uip_len < sizeof(struct arp_hdr)) {
    uip_len = 0;
    return;
  }
  uip_len = 0;

  switch(BUF->opcode) {
  case HTONS(ARP_REQUEST):
    /* ARP request. If it asked for our address, we send out a
       reply. */
    /*    if(BUF->dipaddr[0] == uip_hostaddr[0] &&
	  BUF->dipaddr[1] == uip_hostaddr[1]) {*/
    PRINTF("uip_arp_arpin: request for %d.%d.%d.%d (we are %d.%d.%d.%d)\n",
	   BUF->dipaddr.u8[0], BUF->dipaddr.u8[1],
	   BUF->dipaddr.u8[2], BUF->dipaddr.u8[3],
	   uip_hostaddr.u8[0], uip_hostaddr.u8[1],
	   uip_hostaddr.u8[2], uip_hostaddr.u8[3]);
    if(uip_ipaddr_cmp(&BUF->dipaddr, &uip_hostaddr)) {
      /* First, we register the one who made the request in our ARP
	 table, since it is likely that we will do more communication
	 with this host in the future. */
      //uip_arp_update(&BUF->sipaddr, &BUF->shwaddr);

      BUF->opcode = HTONS(ARP_REPLY);

      memcpy(BUF->dhwaddr.addr, BUF->shwaddr.addr, 6);
      memcpy(BUF->shwaddr.addr, uip_ethaddr.addr, 6);
      memcpy(BUF->ethhdr.src.addr, uip_ethaddr.addr, 6);
      memcpy(BUF->ethhdr.dest.addr, BUF->dhwaddr.addr, 6);

      uip_ipaddr_copy(&BUF->dipaddr, &BUF->sipaddr);
      uip_ipaddr_copy(&BUF->sipaddr, &uip_hostaddr);

      BUF->ethhdr.type = HTONS(UIP_ETHTYPE_ARP);
      uip_len = sizeof(struct arp_hdr);
    }
    break;

  }

  return;
}
/*-----------------------------------------------------------------------------------*/
/**
 * Prepend Ethernet header to an outbound IP packet and see if we need
 * to send out an ARP request.
 *
 * This function should be called before sending out an IP packet. The
 * function checks the destination IP address of the IP packet to see
 * what Ethernet MAC address that should be used as a destination MAC
 * address on the Ethernet.
 *
 * If the destination IP address is in the local network (determined
 * by logical ANDing of netmask and our IP address), the function
 * checks the ARP cache to see if an entry for the destination IP
 * address is found. If so, an Ethernet header is prepended and the
 * function returns. If no ARP cache entry is found for the
 * destination IP address, the packet in the uip_buf[] is replaced by
 * an ARP request packet for the IP address. The IP packet is dropped
 * and it is assumed that they higher level protocols (e.g., TCP)
 * eventually will retransmit the dropped packet.
 *
 * If the destination IP address is not on the local network, the IP
 * address of the default router is used instead.
 *
 * When the function returns, a packet is present in the uip_buf[]
 * buffer, and the length of the packet is in the global variable
 * uip_len.
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_out(void)
{
  /* Find the destination IP address in the ARP table and construct
     the Ethernet header. If the destination IP address isn't on the
     local network, we use the default router's IP address instead.

*/

  /* First check if destination is a local broadcast. */
  if(uip_ipaddr_cmp(&IPBUF->destipaddr, &uip_broadcast_addr)) 
  {
    memset(IPBUF->ethhdr.dest.addr, 0xFF, 6);
  } 
  else 
  {
	// We always go through the router (adaptor)
      /* Destination address was not on the local network, so we need to
	 use the default router's IP address instead of the destination
	 address when determining the MAC address. */

    /* Build an ethernet header. */
    memcpy(IPBUF->ethhdr.dest.addr, uip_drethaddr.addr, 6);
  }
  memcpy(IPBUF->ethhdr.src.addr, uip_ethaddr.addr, 6);

  IPBUF->ethhdr.type = HTONS(UIP_ETHTYPE_IP);

  uip_len += sizeof(struct uip_eth_hdr);
}
/*-----------------------------------------------------------------------------------*/

/** @} */
/** @} */

