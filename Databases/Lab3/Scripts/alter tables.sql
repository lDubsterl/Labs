ALTER TABLE IF EXISTS "Auto" RENAME TO "Autos";

ALTER TABLE IF EXISTS "Client" RENAME TO "Clients";

ALTER TABLE IF EXISTS "Elevator" RENAME TO "Elevators";

ALTER TABLE IF EXISTS "Floor" RENAME TO "Floors";

ALTER TABLE IF EXISTS "Place" RENAME TO "Places";

ALTER TABLE IF EXISTS "Table" RENAME TO "Tables";

--alter table "Clients" rename column "Otchestvo" to "Patronymic";

alter table "Autos" add check("Length" > 0);

alter table "Autos" drop constraint if exists "Autos_Length_check1";

create table if not exists "Temporary table"
(
	Id serial not null,
	"MockUpInt" integer
);

drop table if exists "Temporary table";
