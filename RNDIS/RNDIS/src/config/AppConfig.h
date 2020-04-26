/*
             LUFA Library
     Copyright (C) Dean Camera, 2017.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2017  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *  \brief Application Configuration Header File
 *
 *  This is a header file which is be used to configure some of
 *  the application's compile time options, as an alternative to
 *  specifying the compile time constants supplied through a
 *  makefile or build system.
 *
 *  For information on what each token does, refer to the
 *  \ref Sec_Options section of the application documentation.
 */

#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

	//#define ENABLE_DHCP_CLIENT
	//#define ENABLE_DHCP_SERVER
	//#define ENABLE_TELNET_SERVER
	#define MAX_URI_LENGTH                50

	#define DEVICE_IP_ADDRESS0             10
	#define DEVICE_IP_ADDRESS1             0
	#define DEVICE_IP_ADDRESS2             1
	#define DEVICE_IP_ADDRESS3             2
	#define DEVICE_NETMASK0                255
	#define DEVICE_NETMASK1                255
	#define DEVICE_NETMASK2                255
	#define DEVICE_NETMASK3                0
	#define DEVICE_GATEWAY0                10
	#define DEVICE_GATEWAY1                0
	#define DEVICE_GATEWAY2                1
	#define DEVICE_GATEWAY3                1
	#define SERVER_MAC_ADDRESS0            1
	#define SERVER_MAC_ADDRESS1            0
	#define SERVER_MAC_ADDRESS2            1
	#define SERVER_MAC_ADDRESS3            0
	#define SERVER_MAC_ADDRESS4            1
	#define SERVER_MAC_ADDRESS5            0
	#define ADAPTOR_MAC_ADDRESS0          0x02
	#define ADAPTOR_MAC_ADDRESS1          0x00
	#define ADAPTOR_MAC_ADDRESS2          0x02
	#define ADAPTOR_MAC_ADDRESS3          0x00
	#define ADAPTOR_MAC_ADDRESS4          0x02
	#define ADAPTOR_MAC_ADDRESS5          0x00
	#define ADAPTOR_MAC_ADDRESS           { ADAPTOR_MAC_ADDRESS0, ADAPTOR_MAC_ADDRESS1, ADAPTOR_MAC_ADDRESS2, ADAPTOR_MAC_ADDRESS3, ADAPTOR_MAC_ADDRESS4, ADAPTOR_MAC_ADDRESS5}

	#define UIP_IPADDR0						DEVICE_IP_ADDRESS0
	#define UIP_IPADDR1						DEVICE_IP_ADDRESS1
	#define UIP_IPADDR2						DEVICE_IP_ADDRESS2
	#define UIP_IPADDR3						DEVICE_IP_ADDRESS3

	#define UIP_DRIPADDR0					DEVICE_GATEWAY0
	#define UIP_DRIPADDR1					DEVICE_GATEWAY1
	#define UIP_DRIPADDR2					DEVICE_GATEWAY2
	#define UIP_DRIPADDR3					DEVICE_GATEWAY3

	#define UIP_NETMASK0					DEVICE_NETMASK0
	#define UIP_NETMASK1					DEVICE_NETMASK1
	#define UIP_NETMASK2					DEVICE_NETMASK2
	#define UIP_NETMASK3					DEVICE_NETMASK3

	#define UIP_ETHADDR0					SERVER_MAC_ADDRESS0
	#define UIP_ETHADDR1					SERVER_MAC_ADDRESS1
	#define UIP_ETHADDR2					SERVER_MAC_ADDRESS2
	#define UIP_ETHADDR3					SERVER_MAC_ADDRESS3
	#define UIP_ETHADDR4					SERVER_MAC_ADDRESS4
	#define UIP_ETHADDR5					SERVER_MAC_ADDRESS5

	#define UIP_DRETHADDR0					ADAPTOR_MAC_ADDRESS0
	#define UIP_DRETHADDR1					ADAPTOR_MAC_ADDRESS1
	#define UIP_DRETHADDR2					ADAPTOR_MAC_ADDRESS2
	#define UIP_DRETHADDR3					ADAPTOR_MAC_ADDRESS3
	#define UIP_DRETHADDR4					ADAPTOR_MAC_ADDRESS4
	#define UIP_DRETHADDR5					ADAPTOR_MAC_ADDRESS5

	#define UIP_CONF_UDP                  (defined(ENABLE_DHCP_CLIENT) || defined(ENABLE_DHCP_SERVER))
	#define UIP_CONF_BROADCAST            0
	#define UIP_CONF_TCP                  1
	#define UIP_CONF_UDP_CONNS            1
	#define UIP_CONF_MAX_CONNECTIONS      2
	#define UIP_CONF_MAX_LISTENPORTS      2
	#define UIP_CONF_BUFFER_SIZE          512
	#define UIP_CONF_LL_802154            0
	#define UIP_CONF_LL_80211             0
	#define UIP_CONF_ROUTER               0
	#define UIP_CONF_ICMP6                0
	#define UIP_CONF_ICMP_DEST_UNREACH    0
	#define UIP_URGDATA                   0
	#define UIP_ARCH_CHKSUM               0
	#define UIP_ARCH_ADD32                0
	#define UIP_NEIGHBOR_CONF_ADDRTYPE    0
	#define UIP_CONF_ARPTAB_SIZE		  1
	#define UIP_CONF_FIXEDADDR			  1
	#define UIP_CONF_FIXEDETHADDR		  1

#endif
