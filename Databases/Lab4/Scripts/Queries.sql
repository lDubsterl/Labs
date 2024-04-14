--select "Surname", "Name", "Otchestvo", "Parktime", "Auto number" from "Clients"
--inner join "Autos" on "Autos"."Client number" = "Clients"."Passport number";

--select "Client number", "Autos"."Auto number", "Place number" from "Autos"
--inner join "Places" on "Places"."Auto number" = "Autos"."Auto number";

--select "Elevator number", "lift capacity", "Auto number", "Mass" from "Elevators"
--left outer join "Autos" on "Autos"."Mass" <= "Elevators"."lift capacity" where "Mass" > 7200;

--select "Elevator number", "lift capacity", "Auto number", "Mass" from "Elevators"
--right outer join "Autos" on "Autos"."Mass" <= "Elevators"."lift capacity" where "Mass" > 7200;

--select "Tables"."Floor number", "Floors"."Height", "Place type", "Place numbers" from "Floors"
--right outer join "Tables" on "Floors"."Floor number" = "Tables"."Floor number";

--select "Tables"."Floor number", "Floors"."Height", "Place type", "Place numbers" from "Floors"
--left outer join "Tables" on "Floors"."Floor number" = "Tables"."Floor number";

--select * from "Clients" cross join "Places" where "Parktime" > 22 and "Surname" = 'Kirkorov';

--select * from "Tables" cross join "Clients" where "Floor number" = 2 and "Parktime" > 10;

--select * from "Autos" full outer join "Places" 
--on "Autos"."Auto number" = "Places"."Auto number" order by "Autos"."Auto number" desc;

select * from "Clients" full outer join "Autos" 
on "Passport number" = "Client number";

--SELECT * FROM "Clients" ORDER BY "Parktime" DESC;

--SELECT * FROM "Autos" WHERE "Mass" > 5000;

--SELECT * FROM "Clients" WHERE "Parktime" > 15 ORDER BY "Passport number" ASC;

--SELECT * FROM "Places";

--SELECT "Floor number", "Places amount" FROM "Floors";










