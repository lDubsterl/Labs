--select "Passport number", "Parktime", "Auto number" from "Client"
--inner join "Auto" on "Auto"."Client number" = "Client"."Passport number";

--select "Client number", "Auto"."Auto number", "Place number" from "Auto"
--inner join "Place" on "Place"."Auto number" = "Auto"."Auto number";

--select "Elevator number", "lift capacity", "Auto number", "Mass" from "Elevator"
--left outer join "Auto" on "Auto"."Mass" <= "Elevator"."lift capacity" where "Mass" > 7200;

--select "Elevator number", "lift capacity", "Auto number", "Mass" from "Elevator"
--right outer join "Auto" on "Auto"."Mass" <= "Elevator"."lift capacity" where "Mass" > 7200;

--select "Table"."Floor number", "Floor"."Height", "Place type", "Place numbers" from "Floor"
--right outer join "Table" on "Floor"."Floor number" = "Table"."Floor number";

--select "Table"."Floor number", "Floor"."Height", "Place type", "Place numbers" from "Floor"
--left outer join "Table" on "Floor"."Floor number" = "Table"."Floor number";

--select * from "Client" cross join "Place" where "Parktime" > 22 and "Surname" = 'Kirkorov';

--select * from "Table" cross join "Client" where "Floor number" = 2 and "Parktime" > 10;

select * from "Auto" full outer join "Place" 
on "Auto"."Auto number" = "Place"."Auto number" order by "Auto"."Auto number" desc;

--select * from "Client" full outer join "Auto" 
--on "Passport number" = "Client number";