/************************************************************************************//**
* \file			GocontrollMqtt.c
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
* \endinternal
****************************************************************************************/

#include "mqtt.h"
#include "pthread.h"
#include "GocontrollMqtt.h"
#include <errno.h>
#include "posix_sockets.h"

struct json_tokener *tokener;
pthread_t mqtt_thread;

/****************************************************************************************/

void reconnect_client(struct mqtt_client* client, void **reconnect_state_vptr){
    struct reconnect_state_t *reconnect_state = *((struct reconnect_state_t**) reconnect_state_vptr);

    /* Close the clients socket if this isn't the initial reconnect call */
    if (client->error != MQTT_ERROR_INITIAL_RECONNECT) {
        close(client->socketfd);
    }

    /* Perform error handling here. */
    if (client->error != MQTT_ERROR_INITIAL_RECONNECT) {
        printf("reconnect_client: called while client was in error state \"%s\"\n",
               mqtt_error_str(client->error)
        );
    }

    /* Open a new socket. */
    int sockfd = open_nb_socket(reconnect_state->hostname, reconnect_state->port);
    if (sockfd == -1) {
        fprintf(stderr, "Failed to open socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Reinitialize the client. */
    mqtt_reinit(client, sockfd,
                reconnect_state->sendbuf, reconnect_state->sendbufsz,
                reconnect_state->recvbuf, reconnect_state->recvbufsz
    );

    /* Create an anonymous session */
    const char* client_id = NULL;
    /* Ensure we have a clean session */
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    /* Send connection request to the broker. */
    mqtt_connect(client, client_id, NULL, NULL, 0, reconnect_state->username, reconnect_state->password, connect_flags, 400);

    /* Subscribe to the topics. */
    for (uint8_t i = 0; i < reconnect_state->numclients; i++){
        mqtt_subscribe(client, reconnect_state->client[i]->topic, reconnect_state->client[i]->qos);
    }
}


void publish_callback(void** unused, struct mqtt_response_publish *published){
    mqtt_put_buffer(published, NULL);
}

/****************************************************************************************/

void mqtt_put_buffer(struct mqtt_response_publish *published, void** reconnect_state_vptr) {
    static struct reconnect_state_t *reconnect_state = NULL;
    struct json_object *message;

    /*store the reconnect state to access the buffers at a later time*/
    if (reconnect_state_vptr != NULL) {
        reconnect_state = *((struct reconect_state_t**)reconnect_state_vptr);
        return;
    }

    message = json_tokener_parse_ex(tokener, published->application_message, published->application_message_size);

    for (uint8_t i = 0; i < reconnect_state->numclients; i++){
        if (memcmp(published->topic_name, reconnect_state->client[i]->topic, published->topic_name_size) == 0){
            reconnect_state->client[i]->message = message;
            return;
        }
    }
}

/****************************************************************************************/

struct json_object *mqtt_get_buffer(char* topic, void** reconnect_state_vptr) {
    static struct reconnect_state_t *reconnect_state = NULL;
    struct json_object *message;

    /*store the reconnect state to access the buffers at a later time*/
    if (reconnect_state_vptr != NULL) {
        reconnect_state = *((struct reconect_state_t**)reconnect_state_vptr);
        return;
    }

    for (uint8_t i = 0; i < reconnect_state->numclients; i++){
        if (strcmp(topic, reconnect_state->client[i]->topic) == 0){
            return reconnect_state->client[i]->message;
        }
    }
    return NULL;
}

/****************************************************************************************/

void* mqtt_thread_worker(void* client) {
    pthread_cleanup_push(client_refresher_cleanup, client);
    while(1){
        mqtt_sync((struct mqtt_client *) client);
        usleep(100000U);
    }
    pthread_cleanup_pop(0);
    return NULL;
}

/****************************************************************************************
 * @brief The thread cleanup function, closes the \p client's socket and frees the json tokener.
 *
 * @param client
 */
static void client_refresher_cleanup(void* client) {
    struct mqtt_client *clientLocal = (struct mqtt_client *) client;
    close(clientLocal->socketfd);
    json_tokener_free(tokener);
}