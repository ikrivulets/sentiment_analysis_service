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

class Sentiment : virtual public fastcgi::Component, virtual public fastcgi::Handler {

public:
    Sentiment(fastcgi::ComponentContext *context) :
    fastcgi::Component(context) {
    }
    virtual ~Sentiment() {
    }
    CassFuture* connect_future;
    CassCluster* cluster;
    CassSession* session;

public:
    virtual void onLoad() {
        srand (time(NULL));

        CassCluster* cluster;
        CassSession* session;
        CassFuture *connect_future = NULL;
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
            std::string tweet = std::string(request->getArg("tweet"));
            int mark = -5 + rand() % 11;
            std::string resp = "Mark(" + tweet + ")=" + std::to_string(mark) + "\n";
            request->write(resp.c_str(), resp.size());
        } else if (request_method == "POST") {
            fastcgi::DataBuffer dataBuffer = request->requestBody();
            std::string db_string;
            dataBuffer.toString(db_string);
            std::string response_body = parseRequest(cluster, session, connect_future, db_string);
            request->write(response_body.c_str(), response_body.length());
        }
    }

};

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("sentiment_factory", Sentiment)
FCGIDAEMON_REGISTER_FACTORIES_END()