-- ========================
-- 1. Company
-- ========================
CREATE TABLE Company (
    company_id   INTEGER       PRIMARY KEY AUTOINCREMENT,
    company_name TEXT NOT NULL
);

-- ========================
-- 2. Line
--    Color is consistent for the entire line,
--    so we keep color here.
-- ========================
CREATE TABLE Line (
    line_id    INTEGER       PRIMARY KEY AUTOINCREMENT,
    line_name  TEXT NOT NULL,
    company_id INTEGER NOT NULL,
    color      TEXT,          -- e.g., "Green", "#00AA00"
    FOREIGN KEY (company_id) REFERENCES Company(company_id)
);

-- ========================
-- 3. Station
--    We store station names in JP and EN for multilingual.
-- ========================
CREATE TABLE Station (
    station_id       INTEGER       PRIMARY KEY AUTOINCREMENT,
    station_name_jp  TEXT NOT NULL,  -- Japanese name (e.g., 渋谷)
    station_name_en  TEXT NOT NULL,  -- English name (e.g., Shibuya)
    location         TEXT
);

-- ========================
-- 4. Station_Line
--    The junction that links stations to lines.
--    - station_number: order of the station on the line
--    - platform_number: optional; which platform for this station-line
--    - jingle: each Station-Line combo can have a unique jingle
-- ========================
CREATE TABLE Station_Line (
    station_id      INTEGER NOT NULL,
    line_id         INTEGER NOT NULL,
    station_number  INTEGER NOT NULL,       -- The position of the station on the line
    platform_number INTEGER,                -- Optional: which platform for this line at this station
    jingle          TEXT,                   -- The unique jingle for this station-line combination

    PRIMARY KEY (station_id, line_id),
    FOREIGN KEY (station_id) REFERENCES Station(station_id),
    FOREIGN KEY (line_id)    REFERENCES Line(line_id)
);

-- ========================
-- 5. Station_Connection (OPTIONAL)
--    For explicit adjacency, storing distance & travel time between two stations on a line.
-- ========================
CREATE TABLE Station_Connection (
    station_id_a        INTEGER  NOT NULL,
    station_id_b        INTEGER  NOT NULL,
    line_id             INTEGER  NOT NULL,  -- Ties adjacency to a specific line
    distance_km         REAL,               -- e.g., 3.25
    travel_time_minutes INTEGER,            -- e.g., 4 minutes

    PRIMARY KEY (station_id_a, station_id_b, line_id),
    FOREIGN KEY (station_id_a) REFERENCES Station(station_id),
    FOREIGN KEY (station_id_b) REFERENCES Station(station_id),
    FOREIGN KEY (line_id)      REFERENCES Line(line_id)
);

-- ========================
-- 6. Through_Service (OPTIONAL)
--    If a single train continues from one line to another at a station
--    without requiring a passenger transfer.
-- ========================
CREATE TABLE Through_Service (
    through_service_id INTEGER PRIMARY KEY AUTOINCREMENT,
    station_id         INTEGER NOT NULL,
    from_line_id       INTEGER NOT NULL,
    to_line_id         INTEGER NOT NULL,
    notes              TEXT,

    FOREIGN KEY (station_id)   REFERENCES Station(station_id),
    FOREIGN KEY (from_line_id) REFERENCES Line(line_id),
    FOREIGN KEY (to_line_id)   REFERENCES Line(line_id)
);

