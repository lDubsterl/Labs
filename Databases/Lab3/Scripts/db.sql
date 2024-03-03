CREATE TABLE IF NOT EXISTS public."Client"
(
    "Passport number" character varying COLLATE pg_catalog."default" NOT NULL,
    "Parktime" double precision NOT NULL,
    "Surname" character varying COLLATE pg_catalog."default",
    "Name" character varying COLLATE pg_catalog."default",
    "Otchestvo" character varying COLLATE pg_catalog."default",
    CONSTRAINT "Client_pkey" PRIMARY KEY ("Passport number")
);

CREATE TABLE IF NOT EXISTS public."Auto"
(
    "Length" smallint NOT NULL,
    "Width" smallint NOT NULL,
    "Height" smallint NOT NULL,
    "Mass" smallint NOT NULL,
    "Auto number" character varying COLLATE pg_catalog."default" NOT NULL,
    "Client number" character varying COLLATE pg_catalog."default",
    CONSTRAINT "Auto_pkey" PRIMARY KEY ("Auto number"),
    CONSTRAINT "Client number" FOREIGN KEY ("Client number")
        REFERENCES public."Client" ("Passport number") MATCH SIMPLE
        ON UPDATE CASCADE
        ON DELETE CASCADE
);
	
CREATE TABLE IF NOT EXISTS public."Floor"
(
    "Floor number" serial NOT NULL,
    "Places amount" smallint NOT NULL,
    "Height" double precision NOT NULL,
    CONSTRAINT "Floor_pkey" PRIMARY KEY ("Floor number")
);

CREATE TABLE IF NOT EXISTS public."Place"
(
    "Place number" serial NOT NULL,
    "Length" smallint NOT NULL,
    "Width" smallint NOT NULL,
    "Auto number" character varying COLLATE pg_catalog."default",
    "Floor number" smallint NOT NULL,
    CONSTRAINT "Place_pkey" PRIMARY KEY ("Place number"),
    CONSTRAINT "Auto number" FOREIGN KEY ("Auto number")
        REFERENCES public."Auto" ("Auto number") MATCH SIMPLE
        ON UPDATE CASCADE
        ON DELETE CASCADE,
    CONSTRAINT "Floor number" FOREIGN KEY ("Floor number")
        REFERENCES public."Floor" ("Floor number") MATCH SIMPLE
        ON UPDATE CASCADE
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS public."Elevator"
(
    "Elevator number" serial,
    "Length" smallint NOT NULL,
    "Width" smallint NOT NULL,
    "Height" smallint NOT NULL,
    "lift capacity" smallint NOT NULL,
    CONSTRAINT "Elevator_pkey" PRIMARY KEY ("Elevator number")
);

CREATE TABLE IF NOT EXISTS public."Table"
(
    "Table number" serial,
    "Place numbers" character varying,
    "Place type" character varying COLLATE pg_catalog."default",
    "Floor number" smallint NOT NULL,
    CONSTRAINT "Table_pkey" PRIMARY KEY ("Table number"),
    CONSTRAINT "Floor number" FOREIGN KEY ("Floor number")
        REFERENCES public."Floor" ("Floor number") MATCH SIMPLE
        ON UPDATE CASCADE
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS public."Client_Table"
(
    "Client number" character varying COLLATE pg_catalog."default" NOT NULL,
    "Table number" smallint NOT NULL,
    CONSTRAINT "Client_table_pkey" PRIMARY KEY ("Client number", "Table number"),
    CONSTRAINT "Client number" FOREIGN KEY ("Client number")
        REFERENCES public."Client" ("Passport number") MATCH SIMPLE
        ON UPDATE CASCADE
        ON DELETE CASCADE,
    CONSTRAINT "Table number" FOREIGN KEY ("Table number")
        REFERENCES public."Table" ("Table number") MATCH SIMPLE
        ON UPDATE CASCADE
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS public."Floor_Elevator"
(
    "Floor number" smallint NOT NULL,
    "Elevator number" smallint NOT NULL,
    CONSTRAINT "Floor_Elevator_pkey" PRIMARY KEY ("Floor number", "Elevator number"),
    CONSTRAINT "Elevator number" FOREIGN KEY ("Elevator number")
        REFERENCES public."Elevator" ("Elevator number") MATCH SIMPLE
        ON UPDATE CASCADE
        ON DELETE CASCADE,
    CONSTRAINT "Floor number" FOREIGN KEY ("Floor number")
        REFERENCES public."Floor" ("Floor number") MATCH SIMPLE
        ON UPDATE CASCADE
        ON DELETE CASCADE
);