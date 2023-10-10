create table if not exists userInfo(
       id integer primary key autoincrement,
       name varchar(32),
       pwd varchar(32)
);


 
create table if not exists friendInfo(
       id integer not null,
       friendId integer not null,
       primary key(id , friendId)
);

insert into userInfo(name,pwd) values ('jack','jack'),('Elysia','Elysia');