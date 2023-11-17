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

update userInfo set online=0 ;
update userInfo set online=0 where name='2' and pwd='2';
select * from userInfo where online='1'

select * from userInfo where name='1'


insert into friend  values (1,2);


select * from friend where (id=(select id from userInfo where name='jack') and friendid=(select id from userInfo where name='Elysia')) or (id=(select id from userInfo where name='Elysia') and friendid=(select id from userInfo where name='jack'))

select * from friend where (id=(select id from userInfo where name='Elysia') and friendid=(select id from userInfo where name='jack')) or (id=(select id from userInfo where name='Elysia') and friendid=(select id from userInfo where name='jack'))




insert into friend (id, friendid) values ((select id from userInfo where name='Elysia'),(select id from userInfo where name='1'));

delete from  friend where id =2;
