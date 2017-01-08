drop keyspace if exists sentiment_service;
create keyspace sentiment_service
with replication = { 'class' : 'SimpleStrategy', 'replication_factor' : 2};

use sentiment_service;
drop table if exists objects;

create table objects (
    tweet_id timeuuid,
    tweet_text text,
    object_id timeuuid,
    object_name text,
    object_mark int,
    createdts timestamp,
    primary key (object_id)
);

insert into sentiment_service.objects (tweet_id, tweet_text, object_id, object_name, object_mark, createdts)
values (now(), 'hello world', now(),'wert', 6, dateof(now()));

select * from sentiment_service.objects ;