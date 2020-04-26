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

const char PROGMEM HTTP404Header[] = "HTTP/1.1 404 Not Found\r\n"
                                     "Server: LUFA " LUFA_VERSION_STRING "\r\n"
                                     "Connection: close\r\n"
                                     "MIME-version: 1.0\r\n"
                                     "Content-Type: text/plain\r\n\r\n"
                                     "Error 404: File Not Found";
/** HTTP server response header, for transmission before the page contents. This indicates to the host that a page exists at the
 *  given location, and gives extra connection information.
 */
const char PROGMEM HTTP200Header[] = "HTTP/1.1 200 OK\r\n"
                                     "Server: LUFA " LUFA_VERSION_STRING "\r\n"
                                     "Connection: close\r\n"
                                     "MIME-version: 1.0\r\n"
                                     "Content-Type: ";
const char PROGMEM content_type_html[] = "text/html\r\nContent-Encoding: deflate\r\nContent-Length: ";
const char PROGMEM content_type_json[] = "application/json";

const char PROGMEM index_html[] = {
#include "../index.htm.z"
};

static char html_buffer[512];


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
		AppState->HTTPServer.CurrentState  = WEBSERVER_STATE_ProcessRequest;
		AppState->HTTPServer.NextState     = WEBSERVER_STATE_ProcessRequest;
		AppState->HTTPServer.ACKedFilePos  = 0;
		AppState->HTTPServer.SentChunkSize = 0;
	}

	if (uip_acked())
	{
		/* Add the amount of ACKed file data to the total sent file bytes counter */
		AppState->HTTPServer.ACKedFilePos += AppState->HTTPServer.SentChunkSize;

		/* Progress to the next state once the current state's data has been ACKed */
		AppState->HTTPServer.CurrentState = AppState->HTTPServer.NextState;
	}

	if (uip_rexmit() || uip_acked() || uip_newdata() || uip_connected() || uip_poll())
	{
		switch (AppState->HTTPServer.CurrentState)
		{
			case WEBSERVER_STATE_ProcessRequest:
				HTTPServerApp_ProcessRequest();
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
static void HTTPServerApp_ProcessRequest(void)
{
	uip_tcp_appstate_t* const AppState    = &uip_conn->appstate;
	char*               const AppData     = (char*)uip_appdata;

	/* No HTTP header received from the client, abort processing */
	if (!(uip_newdata()))
	  return;

	char* RequestToken      = strtok(AppData, " ");
	char* RequestedFileName = strtok(NULL, " ");

	/* Must be a GET request, abort otherwise */
	if (strcmp_P(RequestToken, PSTR("GET")) == 0)
	{
		if ( *RequestedFileName == '/' && *(RequestedFileName+1) == 0 )
		{
			AppState->HTTPServer.progmem = 1;
			AppState->HTTPServer.buffer = index_html;
			AppState->HTTPServer.buffer_len = sizeof(index_html);
		}
		else if ( *RequestedFileName == '/' && *(RequestedFileName+1) == 'd' )
		{
			strcpy( html_buffer, "{\"I0\":0,\"I1\":1,\"I2\":");
			clock_time_t t = clock_time();
			itoa(t,html_buffer+strlen(html_buffer),10);
			strcat(html_buffer,"}");
			AppState->HTTPServer.progmem = 0;
			AppState->HTTPServer.buffer = html_buffer;
			AppState->HTTPServer.buffer_len = strlen(html_buffer);
		}
		else if ( *RequestedFileName == '/' && *(RequestedFileName+1) == 'c' )
		{
			strcpy( html_buffer, "{\"C0\":0,\"C1\":1,\"C2\":0}");
			AppState->HTTPServer.progmem = 0;
			AppState->HTTPServer.buffer = html_buffer;
			AppState->HTTPServer.buffer_len = strlen(html_buffer);
		}
		else
		{
			/* Copy over the HTTP 404 response header and send it to the receiving client */
			strcpy_P(AppData, HTTP404Header);
			uip_send(AppData, strlen(AppData));

			AppState->HTTPServer.NextState = WEBSERVER_STATE_Closing;
			return;
		}
	}
	else
	{
		uip_abort();
		return;
	}
	AppState->HTTPServer.SentChunkSize = 0;

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
	if ( AppState->HTTPServer.progmem )
	{
		strcat_P(AppData, content_type_html);
		itoa(AppState->HTTPServer.buffer_len, AppData + strlen(AppData), 10 );
	}
	else
	{
		strcat_P(AppData, content_type_json);
	}
	strcat_P(AppData, PSTR("\r\n\r\n"));

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

#define MIN(a,b) ((a) < (b)? (a): (b))
	AppState->HTTPServer.SentChunkSize = MIN(AppState->HTTPServer.buffer_len,MaxChunkSize);
	
	if ( AppState->HTTPServer.progmem )
	{
		memcpy_P( AppData, AppState->HTTPServer.buffer + AppState->HTTPServer.ACKedFilePos, AppState->HTTPServer.SentChunkSize);
	}
	else
	{
		memcpy( AppData, AppState->HTTPServer.buffer + AppState->HTTPServer.ACKedFilePos, AppState->HTTPServer.SentChunkSize);
	}

	/* Send the next file chunk to the receiving client */
	uip_send(AppData, AppState->HTTPServer.SentChunkSize);

	/* Check if we are at the last chunk of the file, if so next ACK should close the connection */
	if (AppState->HTTPServer.SentChunkSize+AppState->HTTPServer.ACKedFilePos == AppState->HTTPServer.buffer_len)
	  AppState->HTTPServer.NextState = WEBSERVER_STATE_Closing;
}

