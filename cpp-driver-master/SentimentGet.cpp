#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

#include <iostream>
#include <string>
#include <stdlib.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/value.h>

#include <cassandra.h>
#include "CassandraManager.h"

class SentimentGet : virtual public fastcgi::Component, virtual public fastcgi::Handler {

public:
    SentimentGet(fastcgi::ComponentContext *context) :
    fastcgi::Component(context) {
    }
    virtual ~SentimentGet() {
    }
    CassFuture* connect_future;
    CassCluster* cluster;
    CassSession* session;

public:
    virtual void onLoad() {
        srand (time(NULL));

        connect_future = NULL;
        cluster = cass_cluster_new();
        session = cass_session_new();
        char *hosts = (char *)"192.168.56.1";
        /* Add contact points */
        cass_cluster_set_contact_points(cluster, hosts);

        /* Provide the cluster object as configuration to connect the session */
        connect_future = cass_session_connect(session, cluster);
        if (cass_future_error_code(connect_future) != CASS_OK) {
            /* Handle error */
            const char* message;
            size_t message_length;
            cass_future_error_message(connect_future, &message, &message_length);
            fprintf(stderr, "Unable to connect: '%.*s'\n", (int)message_length, message);
        }
    }

    virtual void onUnload() {
        cass_future_free(connect_future);
        cass_cluster_free(cluster);
        cass_session_free(session);
    }

    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context) 
    {
        std::string request_method = request->getRequestMethod();
        if (request_method == "GET") {
            request->setContentType("text/plain");
            std::string object_name = std::string(request->getArg("object_name"));
            float mark = -5 + rand() % 11;
            std::string response_body = getAverageMark(cluster, session, connect_future, object_name);
            request->write(response_body.c_str(), response_body.size());
        }
    }
};

