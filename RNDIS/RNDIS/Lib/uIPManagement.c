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
 *
 *  uIP Management functions. This file contains the functions and globals needed to maintain the uIP
 *  stack once an RNDIS device has been attached to the system.
 */

#define  INCLUDE_FROM_UIPMANAGEMENT_C
#include "uIPManagement.h"

/** Connection timer, to retain the time elapsed since the last time the uIP connections were managed. */
static struct timer ConnectionTimer;

/** Configures the uIP stack ready for network traffic processing. */
void uIPManagement_Init(void)
{
	/* uIP Timing Initialization */
	clock_init();
	timer_set(&ConnectionTimer, CLOCK_SECOND / 2);

	/* uIP Stack Initialization */
	uip_init();
	//uip_arp_init();

	/* HTTP Webserver Initialization */
	HTTPServerApp_Init();
}

/** uIP Management function. This function manages the uIP stack when called while an RNDIS device has been
 *  attached to the system.
 */
void uIPManagement_ManageNetwork(void)
{
	if ( USB_DeviceState == DEVICE_STATE_Configured )
	{
		uIPManagement_ProcessIncomingPacket();
		uIPManagement_ManageConnections();
	}
}

/** uIP TCP/IP network stack callback function for the processing of a given TCP connection. This routine dispatches
 *  to the appropriate TCP protocol application based on the connection's listen port number.
 */
void uIPManagement_TCPCallback(void)
{
	/* Call the correct TCP application based on the port number the connection is listening on */
	switch (uip_conn->lport)
	{
		case HTONS(HTTP_SERVER_PORT):
			HTTPServerApp_Callback();
			break;
		#if defined(ENABLE_TELNET_SERVER)
		case HTONS(TELNET_SERVER_PORT):
			TELNETServerApp_Callback();
			break;
		#endif
	}
}

///** uIP TCP/IP network stack callback function for the processing of a given UDP connection. This routine dispatches
 //*  to the appropriate UDP protocol application based on the connection's listen port number.
 //*/
//void uIPManagement_UDPCallback(void)
//{
	///* Call the correct UDP application based on the port number the connection is listening on */
	//switch (uip_udp_conn->lport)
	//{
		//#if defined(ENABLE_DHCP_CLIENT)
		//case HTONS(DHCP_CLIENT_PORT):
			//DHCPClientApp_Callback();
			//break;
		//#endif
		//#if defined(ENABLE_DHCP_SERVER)
		//case HTONS(DHCP_SERVER_PORT):
			//DHCPServerApp_Callback();
			//break;
		//#endif
	//}
//}

/** Processes Incoming packets to the server from the connected RNDIS device, creating responses as needed. */
static void uIPManagement_ProcessIncomingPacket(void)
{

	/* If no packet received, exit processing routine */
	if (!(RNDIS_Device_IsPacketReceived(&Ethernet_RNDIS_Interface_Device)))
		return;

	LEDs_SetAllLEDs(LEDMASK_USB_BUSY);

	/* Read the Incoming packet straight into the UIP packet buffer */
	RNDIS_Device_ReadPacket(&Ethernet_RNDIS_Interface_Device, uip_buf, &uip_len);

	/* If the packet contains an Ethernet frame, process it */
	if (uip_len > 0)
	{
		switch (((struct uip_eth_hdr*)uip_buf)->type)
		{
			case HTONS(UIP_ETHTYPE_IP):
				/* Filter packet by MAC destination */
				uip_arp_ipin();

				/* Process Incoming packet */
				uip_input();

				/* If a response was generated, send it */
				if (uip_len > 0)
				{
					/* Add destination MAC to outgoing packet */
					uip_arp_out();

					uip_split_output();
				}

				break;
			case HTONS(UIP_ETHTYPE_ARP):
				/* Process ARP packet */
				uip_arp_arpin();

				/* If a response was generated, send it */
				if (uip_len > 0)
				  uip_split_output();

				break;
		}
	}

	LEDs_SetAllLEDs(LEDMASK_USB_READY);
}

/** Manages the currently open network connections, including TCP and (if enabled) UDP. */
static void uIPManagement_ManageConnections(void)
{
	/* Poll TCP connections for more data to send back to the host */
	for (uint8_t i = 0; i < UIP_CONNS; i++)
	{
		uip_poll_conn(&uip_conns[i]);

		/* If a response was generated, send it */
		if (uip_len > 0)
		{
			/* Add destination MAC to outgoing packet */
			uip_arp_out();

			/* Split and send the outgoing packet */
			uip_split_output();
		}
	}

	/* Manage open connections for timeouts */
	if (timer_expired(&ConnectionTimer))
	{
		timer_reset(&ConnectionTimer);

		LEDs_SetAllLEDs(LEDMASK_USB_BUSY);

		for (uint8_t i = 0; i < UIP_CONNS; i++)
		{
			/* Run periodic connection management for each TCP connection */
			uip_periodic(i);

			/* If a response was generated, send it */
			if (uip_len > 0)
			{
				/* Add destination MAC to outgoing packet */
				uip_arp_out();

				/* Split and send the outgoing packet */
				uip_split_output();
			}
		}

		#if defined(ENABLE_DHCP_CLIENT)
		for (uint8_t i = 0; i < UIP_UDP_CONNS; i++)
		{
			/* Run periodic connection management for each UDP connection */
			uip_udp_periodic(i);

			/* If a response was generated, send it */
			if (uip_len > 0)
			{
				/* Add destination MAC to outgoing packet */
				uip_arp_out();

				/* Split and send the outgoing packet */
				uip_split_output();
			}
		}
		#endif

		LEDs_SetAllLEDs(LEDMASK_USB_READY);
	}
}

