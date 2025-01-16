-- Insert JR East into the Company table
INSERT INTO Company (company_name)
VALUES ('JR East');

-- Get the company_id for JR East
-- Assuming the company_id is 1 for simplicity
-- You can verify this using a SELECT query if needed.

-- Insert lines into the Line table
INSERT INTO Line (line_name, company_id, color)
VALUES 
('Yamanote', 1, 'Green'),
('Sobu', 1, 'Yellow'),
('Yokosuka', 1, 'Blue');

-- Get the line_id for each line
-- Assuming the line_id values are assigned sequentially:
-- Yamanote = 1, Sobu = 2, Yokosuka = 3
-- Verify with a SELECT query if necessary.

-- Insert stations into the Station table
INSERT INTO Station (station_name_jp, station_name_en, location)
VALUES 
('東京', 'Tokyo', '35.681236, 139.767125'), 
('品川', 'Shinagawa', '35.628472, 139.73876'), 
('渋谷', 'Shibuya', '35.658034, 139.701636'),
('池袋', 'Ikebukuro', '35.728926, 139.71038'), 
('新宿', 'Shinjuku', '35.690921, 139.700258'), 
('秋葉原', 'Akihabara', '35.698353, 139.773114'), 
('上野', 'Ueno', '35.713768, 139.777254');

-- Get the station_id for each station
-- Assuming station_id values are assigned sequentially from 1.

-- Insert into Station_Line (Yamanote Line)
INSERT INTO Station_Line (station_id, line_id, station_number, platform_number, jingle)
VALUES 
(1, 1, 1, 1, 'Tokyo_Yamanote.mp3'),    -- Tokyo
(2, 1, 2, 2, 'Shinagawa_Yamanote.mp3'), -- Shinagawa
(3, 1, 3, 3, 'Shibuya_Yamanote.mp3'),   -- Shibuya
(4, 1, 4, 4, 'Ikebukuro_Yamanote.mp3'), -- Ikebukuro
(5, 1, 5, 5, 'Shinjuku_Yamanote.mp3'),  -- Shinjuku
(6, 1, 6, 6, 'Akihabara_Yamanote.mp3'), -- Akihabara
(7, 1, 7, 7, 'Ueno_Yamanote.mp3');      -- Ueno

-- Insert into Station_Line (Sobu Line)
INSERT INTO Station_Line (station_id, line_id, station_number, platform_number, jingle)
VALUES 
(1, 2, 1, 1, 'Tokyo_Sobu.mp3'),      -- Tokyo
(2, 2, 2, 2, 'Shinagawa_Sobu.mp3'),  -- Shinagawa
(6, 2, 3, 3, 'Akihabara_Sobu.mp3'),  -- Akihabara
(7, 2, 4, 4, 'Ueno_Sobu.mp3');       -- Ueno

-- Insert into Station_Line (Yokosuka Line)
INSERT INTO Station_Line (station_id, line_id, station_number, platform_number, jingle)
VALUES 
(1, 3, 1, 1, 'Tokyo_Yokosuka.mp3'),      -- Tokyo
(2, 3, 2, 2, 'Shinagawa_Yokosuka.mp3'), -- Shinagawa
(7, 3, 3, 3, 'Ueno_Yokosuka.mp3');
