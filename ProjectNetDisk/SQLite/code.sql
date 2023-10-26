create table if not exists userInfo(
       id integer primary key autoincrement,
       name varchar(32) unique,
       pwd varchar(32),
       online integer default 0
);


 
create table if not exists friend(
       id integer not null,
       friendId integer not null,
       primary key(id , friendId),
       FOREIGN KEY (id) REFERENCES userInfo(id),
       FOREIGN KEY (friendId) REFERENCES userInfo(id)
);

insert into userInfo(name,pwd) values ('jack','jack'),('Elysia','Elysia');

insert into userInfo (name,pwd) values('1','1');

update userInfo set online=0 where name='1' and pwd='1';
select * from userInfo where online='1'