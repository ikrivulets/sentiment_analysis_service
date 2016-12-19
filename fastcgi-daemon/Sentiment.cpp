#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

#include <iostream>
#include <string>
#include <stdlib.h>

class Sentiment : virtual public fastcgi::Component, virtual public fastcgi::Handler {

public:
    Sentiment(fastcgi::ComponentContext *context) :
    fastcgi::Component(context) {
    }
    virtual ~Sentiment() {
    }

public:
    virtual void onLoad() {
    }
    virtual void onUnload() {
    }
    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context) 
    {
        request->setContentType("text/plain");
        std::string tweet = std::string(request->getArg("tweet"));
	int mark = -5 + rand() % 11;
        std::string resp = "Mark(" + tweet + ")=" + std::to_string(mark) + "\n";
        request->write(resp.c_str(), resp.size());
    }

};

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("sentiment_factory", Sentiment)
FCGIDAEMON_REGISTER_FACTORIES_END()
