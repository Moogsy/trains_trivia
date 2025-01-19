-- ========================
-- 1. Company
-- ========================
CREATE TABLE Company (
    company_name TEXT PRIMARY KEY NOT NULL -- Company name becomes the primary key
) STRICT;

-- ========================
-- 2. Line
-- ========================
CREATE TABLE Line (
    line_name  TEXT PRIMARY KEY NOT NULL,     -- Line name becomes the primary key
    company_name TEXT NOT NULL,      -- Reference by name instead of ID
    color      TEXT,                 -- e.g., "Green", "#00AA00"
    FOREIGN KEY (company_name) REFERENCES Company(company_name)
) STRICT;

-- ========================
-- 3. Station
--    We store station names in JP and EN for multilingual.
-- ========================
CREATE TABLE Station (
    station_name_en TEXT PRIMARY KEY NOT NULL,  -- English name as the primary key
    station_name_jp TEXT NOT NULL      -- Japanese name
) STRICT;

-- ========================
-- 4. Station_Line
--    The junction that links stations to lines.
--    - station_number: order of the station on the line
--    - jingle: each Station-Line combo can have a unique jingle
-- ========================
CREATE TABLE Station_Line (
    station_name_en TEXT NOT NULL, -- Reference by station English name
    line_name       TEXT NOT NULL, -- Reference by line name
    station_number  INTEGER NOT NULL, -- The station number on this line
    jingle          TEXT,             -- The unique jingle for this station-line combination

    PRIMARY KEY (station_name_en, line_name),
    FOREIGN KEY (station_name_en) REFERENCES Station(station_name_en),
    FOREIGN KEY (line_name)       REFERENCES Line(line_name)
) STRICT;

