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
 *  Simple HTTP Webserver Application. When connected to the uIP stack,
 *  this will serve out files to HTTP clients on port 80.
 */

#define  INCLUDE_FROM_HTTPSERVERAPP_C
#include "HTTPServerApp.h"

/** HTTP server response header, for transmission before the page contents. This indicates to the host that a page exists at the
 *  given location, and gives extra connection information.
 */
const char PROGMEM HTTP200Header[] = "HTTP/1.1 200 OK\r\n"
                                     "Server: LUFA " LUFA_VERSION_STRING "\r\n"
                                     "Connection: close\r\n"
                                     "MIME-version: 1.0\r\n"
                                     "Content-Type: text/html"
									 "\r\n\r\n";

const char PROGMEM index_html[] = R"(<html>
<title></title>
<body>
This is the DCC configuration page.
</body>
</html>)";




/** Initialization function for the simple HTTP webserver. */
void HTTPServerApp_Init(void)
{
	/* Listen on port 80 for HTTP connections from hosts */
	uip_listen(HTONS(HTTP_SERVER_PORT));

}

/** uIP stack application callback for the simple HTTP webserver. This function must be called each time the
 *  TCP/IP stack needs a TCP packet to be processed.
 */
void HTTPServerApp_Callback(void)
{
	uip_tcp_appstate_t* const AppState = &uip_conn->appstate;

	if (uip_aborted() || uip_timedout() || uip_closed())
	{
		/* Lock to the closed state so that no further processing will occur on the connection */
		AppState->HTTPServer.CurrentState  = WEBSERVER_STATE_Closing;
		AppState->HTTPServer.NextState     = WEBSERVER_STATE_Closing;
	}

	if (uip_connected())
	{
		/* New connection - initialize connection state values */
		AppState->HTTPServer.CurrentState  = WEBSERVER_STATE_OpenRequestedFile;
		AppState->HTTPServer.NextState     = WEBSERVER_STATE_OpenRequestedFile;
	}

	if (uip_acked())
	{
		/* Add the amount of ACKed file data to the total sent file bytes counter */
		AppState->HTTPServer.buffer_len -= AppState->HTTPServer.SentChunkSize;

		/* Progress to the next state once the current state's data has been ACKed */
		AppState->HTTPServer.CurrentState = AppState->HTTPServer.NextState;
	}

	if (uip_rexmit() || uip_acked() || uip_newdata() || uip_connected() || uip_poll())
	{
		switch (AppState->HTTPServer.CurrentState)
		{
			case WEBSERVER_STATE_OpenRequestedFile:
				HTTPServerApp_OpenRequestedFile();
				break;
			case WEBSERVER_STATE_SendResponseHeader:
				HTTPServerApp_SendResponseHeader();
				break;
			case WEBSERVER_STATE_SendData:
				HTTPServerApp_SendData();
				break;
			case WEBSERVER_STATE_Closing:
				/* Connection is being terminated for some reason - close file handle */
				/* If connection is not already closed, close it */
				uip_close();

				AppState->HTTPServer.CurrentState = WEBSERVER_STATE_Closed;
				AppState->HTTPServer.NextState    = WEBSERVER_STATE_Closed;
				break;
		}
	}
}

/** HTTP Server State handler for the Request Process state. This state manages the processing of incoming HTTP
 *  GET requests to the server from the receiving HTTP client.
 */
static void HTTPServerApp_OpenRequestedFile(void)
{
	uip_tcp_appstate_t* const AppState    = &uip_conn->appstate;
	char*               const AppData     = (char*)uip_appdata;

	/* No HTTP header received from the client, abort processing */
	if (!(uip_newdata()))
	  return;

	char* RequestToken      = strtok(AppData, " ");

	/* Must be a GET request, abort otherwise */
	if (strcmp_P(RequestToken, PSTR("GET")) != 0)
	{
		uip_abort();
		return;
	}

	/* Copy over the requested filename */
	AppState->HTTPServer.buffer = index_html;
	AppState->HTTPServer.buffer_len = sizeof(index_html)-1;

	/* Lock to the SendResponseHeader state until connection terminated */
	AppState->HTTPServer.CurrentState = WEBSERVER_STATE_SendResponseHeader;
	AppState->HTTPServer.NextState    = WEBSERVER_STATE_SendResponseHeader;
}

/** HTTP Server State handler for the HTTP Response Header Send state. This state manages the transmission of
 *  the HTTP response header to the receiving HTTP client.
 */
static void HTTPServerApp_SendResponseHeader(void)
{
	uip_tcp_appstate_t* const AppState    = &uip_conn->appstate;
	char*               const AppData     = (char*)uip_appdata;

	/* Copy over the HTTP 200 response header and send it to the receiving client */
	strcpy_P(AppData, HTTP200Header);

	/* Send the MIME header to the receiving client */
	uip_send(AppData, strlen(AppData));

	/* When the MIME header is ACKed, progress to the data send stage */
	AppState->HTTPServer.NextState = WEBSERVER_STATE_SendData;
}

/** HTTP Server State handler for the Data Send state. This state manages the transmission of file chunks
 *  to the receiving HTTP client.
 */
static void HTTPServerApp_SendData(void)
{
	uip_tcp_appstate_t* const AppState    = &uip_conn->appstate;
	char*               const AppData     = (char*)uip_appdata;

	/* Get the maximum segment size for the current packet */
	uint16_t MaxChunkSize = uip_mss();

	if ( AppState->HTTPServer.buffer_len < MaxChunkSize )
		MaxChunkSize = AppState->HTTPServer.buffer_len;
	
	memcpy_P( AppData, AppState->HTTPServer.buffer,MaxChunkSize);
	AppState->HTTPServer.SentChunkSize = MaxChunkSize;

	/* Send the next file chunk to the receiving client */
	uip_send(AppData, AppState->HTTPServer.SentChunkSize);

	/* Check if we are at the last chunk of the file, if so next ACK should close the connection */
	if (AppState->HTTPServer.buffer_len == MaxChunkSize)
	  AppState->HTTPServer.NextState = WEBSERVER_STATE_Closing;
}

