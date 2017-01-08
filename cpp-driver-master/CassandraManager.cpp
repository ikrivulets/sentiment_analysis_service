#include "CassandraManager.h"


std::vector< std::pair<std::string, float> > analyzeTweet(std::string tweet) {
    std::vector<std::string> split_tweet;
    std::istringstream iss(tweet);

    for(std::string s; iss >> s;) {
        if (s.length() > 3) {
            split_tweet.push_back(s);
        }
    }

    int num_of_objects = rand() % (split_tweet.size() + 1);
    std::vector< std::pair<std::string, float> > tweet_objects;

    for (int index = 0; index < num_of_objects; index++) {
        int word_num = rand() % (split_tweet.size());
        std::string word = split_tweet[word_num];
        split_tweet.erase(split_tweet.begin() + word_num);
        float word_mark = -5 + rand() % 11;
        tweet_objects.push_back(std::make_pair(word, word_mark));
    }
    return tweet_objects;
}

void insertTweetObjectMark(CassCluster* cluster, CassSession* session, CassFuture* connect_future,
                           std::string tweet_text, std::string object_name, float object_mark) {
    /* Build statement and execute query */
    char *insert_query = (char *)"INSERT INTO sentiment_service.tweets (tweet_id, tweet_text, object_id, object_name, object_mark, createdts) "
            "values (now(), ?, now(),?, ?, dateof(now()));";
    CassStatement *statement = cass_statement_new(insert_query, 3);
    cass_statement_bind_string(statement, 0, tweet_text.c_str());
    cass_statement_bind_string(statement, 1, object_name.c_str());
    cass_statement_bind_float(statement, 2, object_mark);
    CassFuture *result_future = cass_session_execute(session, statement);

    if (cass_future_error_code(result_future) == CASS_OK) {
        /* Retrieve result set and get the first row */
        const CassResult *result = cass_future_get_result(result_future);
        const CassRow *row = cass_result_first_row(result);
        if (row) {
            const CassValue *value = cass_row_get_column_by_name(row, "release_version");

            const char *release_version;
            size_t release_version_length;
            cass_value_get_string(value, &release_version, &release_version_length);
            printf("release_version: '%.*s'\n", (int) release_version_length, release_version);
        }
        cass_result_free(result);
    } else {
        /* Handle error */
        const char *message;
        size_t message_length;
        cass_future_error_message(result_future, &message, &message_length);
        fprintf(stderr, "Unable to run query: '%.*s'\n", (int) message_length, message);
    }

    cass_statement_free(statement);
    cass_future_free(result_future);
}

void insertTweetObjectMarkWithUpdate(CassCluster* cluster, CassSession* session, CassFuture* connect_future,
                           std::string tweet_text, std::string object_name, float object_mark) {
    /* Build statement and execute query */
    char *insert_query = (char *)"INSERT INTO sentiment_service.tweets (tweet_id, tweet_text, object_id, object_name, object_mark, createdts) "
            "values (now(), ?, now(),?, ?, dateof(now()));";
    CassStatement *statement = cass_statement_new(insert_query, 3);
    cass_statement_bind_string(statement, 0, tweet_text.c_str());
    cass_statement_bind_string(statement, 1, object_name.c_str());
    cass_statement_bind_float(statement, 2, object_mark);
    CassFuture *result_future = cass_session_execute(session, statement);
    cass_statement_free(statement);

    if (cass_future_error_code(result_future) != CASS_OK) {
        /* Handle error */
        const char *message;
        size_t message_length;
        cass_future_error_message(result_future, &message, &message_length);
        fprintf(stderr, "Unable to run query: '%.*s'\n", (int) message_length, message);
    }
    cass_future_free(result_future);

    char *select_query = (char *)"select * from sentiment_service.objects where object_name = ?;";
    statement = cass_statement_new(select_query, 1);
    cass_statement_bind_string(statement, 0, object_name.c_str());
//    cass_statement_bind_string(statement, 0, "iphone");
    result_future = cass_session_execute(session, statement);
    cass_statement_free(statement);

    if (cass_future_error_code(result_future) == CASS_OK) {
        /* Retrieve result set and get the first row */
        const CassResult *result = cass_future_get_result(result_future);
        const CassRow *row = cass_result_first_row(result);
        float avg_mark;
        int num_of_marks;

        if (row) {
            const CassValue *value = cass_row_get_column_by_name(row, "average_mark");
            cass_value_get_float(value, &avg_mark);
            value = cass_row_get_column_by_name(row, "num_of_marks");
            cass_value_get_int32(value, &num_of_marks);
            avg_mark = (avg_mark * num_of_marks + object_mark) / (num_of_marks + 1);
            num_of_marks++;

            char *update_query_objects = (char *)"update sentiment_service.objects set average_mark = ?, num_of_marks = ? where object_name = ?;";
            statement = cass_statement_new(update_query_objects, 3);
            cass_statement_bind_float(statement, 0, avg_mark);
            cass_statement_bind_int32(statement, 1, num_of_marks);
            cass_statement_bind_string(statement, 2, object_name.c_str());
            CassFuture *update_future = cass_session_execute(session, statement);
            cass_statement_free(statement);
            if (cass_future_error_code(update_future) != CASS_OK) {
                /* Handle error */
                const char *message;
                size_t message_length;
                cass_future_error_message(result_future, &message, &message_length);
                fprintf(stderr, "Unable to update: '%.*s'\n", (int) message_length, message);
            }
            cass_future_free(update_future);
        } else {
            char *insert_query_objects = (char *)"insert into sentiment_service.objects (object_id, object_name, average_mark, num_of_marks) values (now(), ?, ?, 1);";
            CassStatement *statement = cass_statement_new(insert_query_objects, 2);
            cass_statement_bind_string(statement, 0, object_name.c_str());
            cass_statement_bind_float(statement, 1, object_mark);
            CassFuture *result2_future = cass_session_execute(session, statement);
            cass_statement_free(statement);
            if (cass_future_error_code(result2_future) != CASS_OK) {
                /* Handle error */
                const char *message;
                size_t message_length;
                cass_future_error_message(result_future, &message, &message_length);
                fprintf(stderr, "Unable to what: '%.*s'\n", (int) message_length, message);
            }
            cass_future_free(result2_future);
        }
        cass_result_free(result);
    } else {
        /* Handle error */
        const char *message;
        size_t message_length;
        cass_future_error_message(result_future, &message, &message_length);
        fprintf(stderr, "Unable to run query: '%.*s'\n", (int) message_length, message);
    }

    cass_future_free(result_future);
}



std::string parseRequest(CassCluster* cluster, CassSession* session,
                         CassFuture* connect_future, std::string request_body, bool with_update) {
    Json::Value root;
    Json::Reader reader;
    std::string response_body = "";
    bool parsingSuccessful = reader.parse(request_body.c_str(), root);
    if (!parsingSuccessful) {
        std::cout  << "Failed to parse" << reader.getFormattedErrorMessages();
    } else {
        std::string tweet_text = root.get("text", "A Default Value if not exists" ).asString();

        Json::Value response_root;
        Json::Value response_objects(Json::arrayValue);

        auto tweet_objects = analyzeTweet(tweet_text);
        for (auto object_pair : tweet_objects) {
            Json::Value object;
            object["id"] = 15;
            object["name"] = object_pair.first;
            object["mark"] = object_pair.second;
            response_objects.append(object);
            if (with_update) {
                insertTweetObjectMarkWithUpdate(cluster, session, connect_future, tweet_text, object_pair.first, object_pair.second);
            } else {
                insertTweetObjectMark(cluster, session, connect_future, tweet_text, object_pair.first, object_pair.second);
            }
        }
        int tweet_mark = -5 + rand() % 11;

        response_root["text"] = tweet_text;
        response_root["mark"] = tweet_mark;
        response_root["id"] = 100;
        response_root["objects"] = response_objects;

        Json::StyledWriter writer;
        response_body = writer.write( response_root );

    }
    return response_body;
}

bool getAverageMark(CassCluster* cluster, CassSession* session,
                     CassFuture* connect_future, std::string object_name, float avg_mark) {
    /* Build statement and execute query */
    char *select_query = (char *)"select * from sentiment_service.objects where object_name = ?;";
    CassStatement *statement = cass_statement_new(insert_query, 1);
    cass_statement_bind_string(statement, 0, object_name.c_str());
    CassFuture *result_future = cass_session_execute(session, statement);
    cass_statement_free(statement);


    if (cass_future_error_code(result_future) == CASS_OK) {
        /* Retrieve result set and get the first row */
        const CassResult *result = cass_future_get_result(result_future);
        const CassRow *row = cass_result_first_row(result);

        if (row) {
            const CassValue *value = cass_row_get_column_by_name(row, "average_mark");
            cass_value_get_float(value, &avg_mark);
            cass_result_free(result);
            return true;
        } else {
            cass_result_free(result);
            return false;
        }
    } else {
        /* Handle error */
        const char *message;
        size_t message_length;
        cass_future_error_message(result_future, &message, &message_length);
        fprintf(stderr, "Unable to run query: '%.*s'\n", (int) message_length, message);
    }

    cass_future_free(result_future);
    return false;
}


//int main(int argc, char* argv[]) {
//    srand (time(NULL));
//    CassFuture* connect_future;
//    CassCluster* cluster;
//    CassSession* session;
//
//    connect_future = NULL;
//    cluster = cass_cluster_new();
//    session = cass_session_new();
//    char *hosts = (char *)"192.168.56.1";
//    /* Add contact points */
//    cass_cluster_set_contact_points(cluster, hosts);
//
//    /* Provide the cluster object as configuration to connect the session */
//    connect_future = cass_session_connect(session, cluster);
//    if (cass_future_error_code(connect_future) != CASS_OK) {
//        /* Handle error */
//        const char* message;
//        size_t message_length;
//        cass_future_error_message(connect_future, &message, &message_length);
//        fprintf(stderr, "Unable to connect: '%.*s'\n", (int)message_length, message);
//    }
//    std::string tweet_text = "{\"text\" : \"hello deaar woorld iphone\"}";
//
//    parseRequest(cluster, session, connect_future, tweet_text);
//    cass_future_free(connect_future);
//    cass_cluster_free(cluster);
//    cass_session_free(session);
//    return 0;
//}
