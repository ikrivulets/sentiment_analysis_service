drop keyspace if exists sentiment_service;
create keyspace sentiment_service
with replication = { 'class' : 'SimpleStrategy', 'replication_factor' : 2};

use sentiment_service;
drop table if exists tweets;
drop table if exists objects;

create table tweets (
    tweet_id timeuuid,
    tweet_text text,
    object_id timeuuid,
    object_name text,
    object_mark float,
    createdts timestamp,
    primary key (object_id)
);

create table objects (
    object_id timeuuid,
    object_name text,
    average_mark float,
    num_of_marks int,
    primary key (object_name)
);

insert into sentiment_service.tweets (tweet_id, tweet_text, object_id, object_name, object_mark, createdts)
values (now(), 'hello world', now(),'wert', 6, dateof(now()));

insert into sentiment_service.objects (object_id, object_name, average_mark, num_of_marks)
values (now(), 'iphone', 4.6, 1);

update sentiment_service.objects set average_mark = 2, num_of_marks = 4 where object_name = 'iphone';

select * from sentiment_service.objects where object_name = 'iphone';