/**
 * LibDazeus -- a library that implements the DaZeus 2 Plugin Protocol
 * 
 * Copyright (c) Sjors Gielen, 2010-2012
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the DaVinci or DaZeus team nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL SJORS GIELEN OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBDAZEUS_H
#define LIBDAZEUS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LIBDAZEUS_VERSION_STR "2.0-beta1"
#define LIBDAZEUS_VERSION 0x010901

typedef struct dazeus_stringlist_struct
{
	char *value;
	struct dazeus_stringlist_struct *next;
} dazeus_stringlist;

typedef struct dazeus_event_struct
{
	char *event;
	dazeus_stringlist *parameters;
	struct dazeus_event_struct *_next;
} dazeus_event;

#define LIBDAZEUS_MAX_JSON_SIZE 20 * 1024 // 20 kb
typedef struct dazeus_struct
{
	char *socketfile;
	int socket;
	char *error;
	char *readahead;
	unsigned int readahead_size;
	unsigned int readahead_capac;
	dazeus_event *event;
	dazeus_event *lastEvent;
} dazeus;

#define DAZEUS_GLOBAL_SCOPE 0x00
#define DAZEUS_NETWORK_SCOPE 0x01
#define DAZEUS_RECEIVER_SCOPE 0x03
#define DAZEUS_SENDER_SCOPE 0x07

typedef struct dazeus_scope_struct
{
	unsigned int scope_type;
	char *network;
	char *receiver;
	char *sender;
} dazeus_scope;

/**
 * Variable scope variable constructors. Don't forget to clean them up with
 * libdazeus_scope_free() too!
 */
dazeus_scope *libdazeus_scope_global();
dazeus_scope *libdazeus_scope_network(const char *network);
dazeus_scope *libdazeus_scope_receiver(const char *network, const char *receiver);
dazeus_scope *libdazeus_scope_sender(const char *network, const char *receiver, const char *sender);
void libdazeus_scope_free(dazeus_scope*);

/**
 * Create a new libdazeus instance, or NULL if there was no memory.
 */
dazeus *libdazeus_create();

/**
 * Retrieve the last error generated or NULL if there was no error.
 */
const char *libdazeus_error(dazeus*);

/**
 * Connect this dazeus instance. Takes socket location as its only parameter.
 * Socket can be either of the form "unix:/path/to/unix/socket" or
 * "tcp:hostname:port" (including "tcp:127.0.0.1:1234"). Returns 1 on success.
 */
int libdazeus_open(dazeus*, const char *socketfile);

/**
 * Get the internal file descriptor of this dazeus instance. Do not read or
 * write from/to this socket. This method can be used to add the given file
 * descriptor to an event loop (i.e. using select()). Returns 0 if no
 * connection was made, or it has recently failed.
 */
int libdazeus_get_socket(dazeus*);

/**
 * Clean up a dazeus instance.
 */
void libdazeus_close(dazeus*);

/**
 * Returns a linked list of networks on this DaZeus instance, or NULL if
 * an error occured or there were no networks. Remember to free the returned
 * structure with libdazeus_networks_free().
 */
dazeus_stringlist *libdazeus_networks(dazeus*);

/**
 * Clean up memory allocated by earlier stringlist functions.
 */
void libdazeus_stringlist_free(dazeus_stringlist*);

/**
 * Returns a linked list of channels on this DaZeus instance, or NULL if
 * an error occured or there were no channels. Remember to free the returned
 * structure with libdazeus_stringlist_free().
 */
dazeus_stringlist *libdazeus_channels(dazeus*, const char *network);

/**
 * Send a message to an IRC channel or user. Returns 1 if the call succeeded.
 */
int libdazeus_message(dazeus*, const char *network, const char *receiver, const char *message);

/**
 * Retrieve the value of a variable in the DaZeus 2 database. Remember to
 * free() the returned variable after use. Returns NULL if the variable did
 * not exist, or an error occured.
 */
char *libdazeus_get_property(dazeus*, const char *variable, dazeus_scope*);

/**
 * Set the value of a variable in the DaZeus 2 database. Returns 1 if the
 * call succeeded.
 */
int libdazeus_set_property(dazeus*, const char *variable, const char *value, dazeus_scope*);

/**
 * Forget a variable in the DaZeus 2 database. Returns 1 if the call succeeded.
 */
int libdazeus_unset_property(dazeus*, const char *variable, dazeus_scope*);

/**
 * Subscribe to a DaZeus 2 Event. Returns 1 if the call succeeded.
 */
int libdazeus_subscribe(dazeus*, const char *event);

/**
 * Return the next event received (in order of coming in). Don't forget to free
 * the resulting structure using libdazeus_event_free. Do NOT
 * libdazeus_stringlist_free its 'parameters' property, though.
 * If the 'timeout' parameter is -1, will wait forever until the first event.
 * If the 'timeout' parameter is 0, will try getting an event exactly once.  If
 * the 'timeout' parameter is > 0, will try that many seconds to get an event.
 * If this method returns NULL, it means either an error occured, or there was
 * no event inside the given timeout period.
 */
dazeus_event *libdazeus_handle_event(dazeus*, int timeout);

/**
 * Free an event returned by libdazeus_handle_event. This function calls
 * libdazeus_stringlist_free on the 'parameters' property, so don't do this
 * yourself.
 */
void libdazeus_event_free(dazeus_event*);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
