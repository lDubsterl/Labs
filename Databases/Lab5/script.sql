select "Client"."Surname", "Name", "Parktime", "Auto"."Auto number"
from "Client" join "Auto" on "Passport number" = "Client number"
join "Place" on "Auto"."Auto number"="Place"."Auto number"
group by "Name", "Surname", "Parktime", "Auto"."Auto number" having "Parktime" > (select avg("Parktime") from "Client")
order by "Auto"."Auto number";

select sum("Mass") as "Summary auto mass", min("Parktime") as "Minimum parktime",
max("Table"."Table number") as "Maximum Table number",
(select count(*) from "Client" where "Surname" = 'Kirkorov') as "Kirkorov count"
from "Client"
join "Auto" on "Passport number" = "Auto"."Client number"
join "Client_Table" on "Client_Table"."Client number" = "Passport number"
join "Table" on "Client_Table"."Table number" = "Table"."Table number";

with ClientAuto as (
	select "Passport number" from "Client"
	union select "Auto"."Client number" from "Auto"
), AutoPlace as (
	select "Auto"."Auto number" from "Auto"
	union select "Place"."Auto number" from "Place"
)
select "Surname", "Passport number", "Auto number" from "Client"
join "Auto" on "Passport number" = "Client number"
where "Passport number" in (select * from ClientAuto) and "Auto number" in (select * from AutoPlace);

with ClientAuto as (
	select "Passport number" from "Client"
	union all select "Auto"."Client number" from "Auto"
), AutoPlace as (
	select "Auto"."Auto number" from "Auto"
	union all select "Place"."Auto number" from "Place"
)
select "Surname", "Passport number", "Auto number" from "Client"
join "Auto" on "Passport number" = "Client number"
where "Passport number" not in (select * from ClientAuto where "Passport number" = 'AA1024464')
and "Auto number" in (select * from AutoPlace);

with ClientAuto as (
	select "Passport number" from "Client"
	except select "Auto"."Client number" from "Auto"
), AutoPlace as (
	select "Auto"."Auto number" from "Auto"
	intersect select "Place"."Auto number" from "Place"
)
select "Surname", "Passport number", "Auto number" from "Client"
join "Auto" on "Passport number" = "Client number"
where "Passport number" not in (select * from ClientAuto)
and "Auto number" in (select * from AutoPlace);

select "Surname", "Name", "Parktime", "Auto"."Auto number"
from "Client" join "Auto" on "Passport number" = "Client number"
join "Place" on "Auto"."Auto number"="Place"."Auto number"
where exists (select * from "Client" where "Surname" = 'Kirkorov')
order by "Surname" asc;

select "Surname", "Name", "Parktime", "Auto"."Auto number"
from "Client" join "Auto" on "Passport number" = "Client number"
join "Place" on "Auto"."Auto number"="Place"."Auto number"
where "Parktime" > any(select "Parktime" from "Client" where "Surname" = 'Sinitsin')
order by "Auto"."Auto number";

select "Surname", "Name", "Parktime", "Auto"."Auto number", "Mass"
from "Client" join "Auto" on "Passport number" = "Client number"
join "Place" on "Auto"."Auto number"="Place"."Auto number"
group by "Surname", "Name", "Mass", "Parktime", "Auto"."Auto number"
having "Parktime" > (select avg("Parktime") from "Client") 
and "Mass" between min("Mass") and max("Mass")
order by "Auto"."Auto number";

select "Client"."Surname", "Name", "Parktime", "Auto"."Auto number"
from "Client" join "Auto" on "Passport number" = "Client number"
join "Place" on "Auto"."Auto number"="Place"."Auto number" where "Name" <> 'Vasiliy'
order by "Name" asc;

select "Surname", "Passport number", "Auto number" from "Client"
join "Auto" on "Passport number" = "Client number"
where "Passport number" like '%B%' order by "Passport number";

select "Passport number", "Floor"."Floor number", "Place type", "Height" from "Client"
join "Client_Table" on "Client number" = "Passport number"
join "Table" on "Table"."Table number" = "Client_Table"."Table number"
join "Floor" on "Table"."Floor number" = "Floor"."Floor number";

select avg("Mass") as "Average auto mass", avg("Parktime") as "Average parktime",
(select count(*) from "Client" where "Surname" = 'Kirkorov') as "Kirkorov count"
from "Client"
join "Auto" on "Passport number" = "Auto"."Client number"
join "Client_Table" on "Client_Table"."Client number" = "Passport number";

select * from "Client"
join "Client_Table" on "Client number" = "Passport number"
join "Table" on "Table"."Table number" = "Client_Table"."Table number"
join "Floor" on "Table"."Floor number" = "Floor"."Floor number";

select "Surname", "Passport number", "Auto"."Auto number", "Place number" from "Client"
join "Auto" on "Passport number" = "Client number"
join "Place" on "Auto"."Auto number" = "Place"."Auto number"
where "Passport number" not in (select "Passport number" from "Client" where "Passport number" like '%4%');

select "Surname", "Passport number", "Auto"."Auto number", "Place number" from "Client"
join "Auto" on "Passport number" = "Client number"
join "Place" on "Auto"."Auto number" = "Place"."Auto number"
where "Passport number" in (select "Passport number" from "Client" where "Passport number" like '%13%')
order by "Surname";