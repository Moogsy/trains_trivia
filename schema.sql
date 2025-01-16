-- ========================
-- 1. Company
-- ========================
CREATE TABLE Company (
    company_id   SERIAL       PRIMARY KEY,
    company_name VARCHAR(255) NOT NULL
);

-- ========================
-- 2. Line
--    Color is consistent for the entire line,
--    so we keep color here.
-- ========================
CREATE TABLE Line (
    line_id    SERIAL       PRIMARY KEY,
    line_name  VARCHAR(255) NOT NULL,
    company_id INT          NOT NULL,
    color      VARCHAR(50),    -- e.g., "Green", "#00AA00"
    FOREIGN KEY (company_id) REFERENCES Company(company_id)
);

-- ========================
-- 3. Station
--    We store station names in JP and EN for multilingual.
-- ========================
CREATE TABLE Station (
    station_id       SERIAL       PRIMARY KEY,
    station_name_jp  VARCHAR(255) NOT NULL,  -- Japanese name (e.g., 渋谷)
    station_name_en  VARCHAR(255) NOT NULL,  -- English name (e.g., Shibuya)
    location         VARCHAR(255)
);

-- ========================
-- 4. Station_Line
--    The junction that links stations to lines.
--    - station_number: order of the station on the line
--    - platform_number: optional; which platform for this station-line
--    - jingle: each Station-Line combo can have a unique jingle
-- ========================
CREATE TABLE Station_Line (
    station_id      INT NOT NULL,
    line_id         INT NOT NULL,
    station_number  INT NOT NULL,       -- The position of the station on the line
    platform_number INT,                -- Optional: which platform for this line at this station
    jingle          VARCHAR(255),       -- The unique jingle for this station-line combination

    PRIMARY KEY (station_id, line_id),
    FOREIGN KEY (station_id) REFERENCES Station(station_id),
    FOREIGN KEY (line_id)    REFERENCES Line(line_id)
);

-- ========================
-- 5. Station_Connection (OPTIONAL)
--    For explicit adjacency, storing distance & travel time between two stations on a line.
-- ========================
CREATE TABLE Station_Connection (
    station_id_a        INT  NOT NULL,
    station_id_b        INT  NOT NULL,
    line_id             INT  NOT NULL,  -- Ties adjacency to a specific line
    distance_km         DECIMAL(5,2),   -- e.g., 3.25
    travel_time_minutes INT,            -- e.g., 4 minutes

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
    through_service_id SERIAL PRIMARY KEY,
    station_id         INT NOT NULL,
    from_line_id       INT NOT NULL,
    to_line_id         INT NOT NULL,
    notes              VARCHAR(255),

    FOREIGN KEY (station_id)   REFERENCES Station(station_id),
    FOREIGN KEY (from_line_id) REFERENCES Line(line_id),
    FOREIGN KEY (to_line_id)   REFERENCES Line(line_id)
);

