#include <cassandra.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/value.h>

bool addTweet(std::string tweet) {
    std::vector<std::string> splitted_tweet;
    std::istringstream iss(tweet);

    for(std::string s; iss >> s;) {
        if (s.length() > 3) {
            splitted_tweet.push_back(s);
        }
    }

    int num_of_objects = rand() % (splitted_tweet.size() + 1);
    std::vector< std::pair<std::string, int> > tweet_objects;

    for (int index = 0; index < num_of_objects; index++) {
        int word_num = rand() % (splitted_tweet.size());
        std::string word = splitted_tweet[word_num];
        splitted_tweet.erase(splitted_tweet.begin() + word_num);
        int word_mark = -5 + rand() % 11;
//        std::cout << word << " " << word_mark << std::endl;
        tweet_objects.push_back(std::make_pair(word, word_mark));
    }

}

void insertNewRow(CassFuture* connect_future) {
    if (cass_future_error_code(connect_future) == CASS_OK) {
        CassFuture* close_future = NULL;

        /* Build statement and execute query */
        const char* query = "SELECT release_version FROM system.local";
        CassStatement* statement = cass_statement_new(query, 0);

        CassFuture* result_future = cass_session_execute(session, statement);

        if (cass_future_error_code(result_future) == CASS_OK) {
            /* Retrieve result set and get the first row */
            const CassResult* result = cass_future_get_result(result_future);
            const CassRow* row = cass_result_first_row(result);

            if (row) {
                const CassValue* value = cass_row_get_column_by_name(row, "release_version");

                const char* release_version;
                size_t release_version_length;
                cass_value_get_string(value, &release_version, &release_version_length);
                printf("release_version: '%.*s'\n", (int)release_version_length, release_version);
            }

            cass_result_free(result);
        } else {
            /* Handle error */
            const char* message;
            size_t message_length;
            cass_future_error_message(result_future, &message, &message_length);
            fprintf(stderr, "Unable to run query: '%.*s'\n", (int)message_length, message);
        }

        cass_statement_free(statement);
        cass_future_free(result_future);

        /* Close the session */
        close_future = cass_session_close(session);
        cass_future_wait(close_future);
        cass_future_free(close_future);
    } else {
        /* Handle error */
        const char* message;
        size_t message_length;
        cass_future_error_message(connect_future, &message, &message_length);
        fprintf(stderr, "Unable to connect: '%.*s'\n", (int)message_length, message);
    }
}



int main(int argc, char* argv[]) {
    srand (time(NULL));
    std::string strJson = "{\"event\" : 2000}";
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( strJson.c_str(), root );
    if (!parsingSuccessful) {
        std::cout  << "Failed to parse" << reader.getFormattedErrorMessages();
        return 0;
    }
    std::cout << root.get("event", "A Default Value if not exists" ).asString() << std::endl;

    return 0;
}
