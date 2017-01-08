#include <cassandra.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/value.h>


std::vector< std::pair<std::string, float> > analyzeTweet(std::string tweet);


void insertTweetObjectMark(CassCluster* cluster, CassSession* session,
                           CassFuture* connect_future, std::string tweet_text,
                           std::string object_name, float object_mark);

void addOrUpdateObject(CassCluster* cluster, CassSession* session, CassFuture* connect_future,
                       std::string object_name, float object_mark);

std::string parseRequest(CassCluster* cluster, CassSession* session,
                         CassFuture* connect_future, std::string request_body, bool with_update);

std::string getAverageMark(CassCluster* cluster, CassSession* session,
                           CassFuture* connect_future, std::string object_name);