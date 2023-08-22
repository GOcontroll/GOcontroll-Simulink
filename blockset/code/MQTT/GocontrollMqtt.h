/************************************************************************************//**
* \file			GocontrollMqtt.h
* \brief		Utility functions to use the MQTT protocol in Simulink
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*  Copyright 2023 (c) by GOcontroll      http://www.gocontroll.com   All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
* endinternal
****************************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <json.h>

struct gocontroll_mqtt_client {
    struct json_object *message;
    char * topic;
    uint8_t qos;
    uint8_t newMessage;
};

struct reconnect_state_t {
    const char *hostname;
    const char *port;
    struct gocontroll_mqtt_client *client[256];
    const uint8_t numclients;
    const char *username;
    const char *password;
    uint8_t* sendbuf;
    size_t sendbufsz;
    uint8_t* recvbuf;
    size_t recvbufsz;
};

/**
** \brief The callback function for when the the client is disconnected to attempt a reconnect.
** \param client the mqtt client.
** \param reconnect_state_vptr pointer to the saved state, used to build up the connection again.
*/
void reconnect_client(struct mqtt_client* client, void **reconnect_state_vptr);

/**
 * @brief The callback function for when a message was received.
 *
 * @param unused ignore.
 * @param published the published message.
 */
void publish_callback(void** unused, struct mqtt_response_publish *published);

/**
 * @brief Put a mqtt message \p published into the buffer that is available to model_step when \p reconnect_state_vptr == NULL\
 * @brief otherwise initialize the buffer to the value of \p reconnect_state_vptr and exit.
 *
 * @param published the message to store in the buffer.
 * @param reconnect_state_vptr value to set up the buffer, NULL if you want to store a message in the buffer.
 */
void mqtt_put_recv_buffer(struct mqtt_response_publish *published, void** reconnect_state_vptr);

/**
 * @brief Get the \p topic's json_object from the buffer when \p reconnect_state_vptr == NULL\
 * @brief otherwise initialize the buffer to the value of \p reconnect_state_vptr and exit.
 *
 * @param topic the topic to look for in the buffer.
 * @param reconnect_state_vptr value to set up the buffer, NULL if you want get a message from the buffer.
 * @return struct json_object* or NULL if no message was found.
 */
struct json_object *mqtt_get_recv_buffer(char* topic, void** reconnect_state_vptr);

/**
 * @brief A function to store and use an \p mqtt_client outside of context in which the connection is made.\
 * @brief First the client must be stored by providing the function with a client. \
 * @brief After this call the function with \p client == NULL and the other fields filled in.
 * 
 * @param client The client to store in the helper, make NULL to publish a message.
 * @param topic The topic to publish to.
 * @param message The message (JSON) to publish.
 * @param publish_flags MQTT flags QOS/Retain
 */
void mqtt_publish_helper(struct mqtt_client *client, char *topic, struct json_object *message, uint8_t publish_flags)

/**
 * @brief The mqtt thread worker to refresh the mqtt messages
 *
 * @param client The active mqtt client
 * @return void* NULL if the thread somehow finishes normally.
 */
void* mqtt_thread_worker(void* client);