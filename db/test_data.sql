TRUNCATE flights, pilots, aircrafts, airports, users RESTART IDENTITY CASCADE;

INSERT INTO users (username, password_hash, last_name, first_name, role) VALUES
('operator', '4996c1c8fb63454c38b3b7236578df275b47649a860db99a5624dbcdbb1e4c1e', 'Операторова', 'Анна', 'operator');

INSERT INTO airports (code, city, name, timezone) VALUES
('SVO', 'Москва', 'Шереметьево', 'Europe/Moscow'),
('LED', 'Санкт-Петербург', 'Пулково', 'Europe/Moscow'),
('DXB', 'Дубай', 'Dubai International', 'Asia/Dubai'),
('IST', 'Стамбул', 'Istanbul Airport', 'Europe/Istanbul'),
('JFK', 'Нью-Йорк', 'John F. Kennedy', 'America/New_York'),
('HND', 'Токио', 'Haneda', 'Asia/Tokyo');

INSERT INTO aircrafts (model, registration_number, total_seats) VALUES
('Boeing 737-800', 'RA-73001', 189), 
('Boeing 737-800', 'RA-73002', 189), 
('Airbus A320', 'RA-32005', 150),    
('Boeing 777-300', 'RA-77001', 400), 
('SSJ-100', 'RA-89001', 87);         

INSERT INTO pilots (last_name, first_name, middle_name, license_number, rank) VALUES
('Иванов', 'Иван', 'Иванович', 'L-100', 'Captain'),       
('Петров', 'Петр', 'Петрович', 'L-101', 'First Officer'), 
('Сидоров', 'Алексей', 'Николаевич', 'L-102', 'Captain'), 
('Кузнецов', 'Дмитрий', 'Олегович', 'L-103', 'First Officer'), 
('Смирнов', 'Андрей', 'Сергеевич', 'L-104', 'Captain'),    
('Попов', 'Максим', 'Викторович', 'L-105', 'First Officer'); 


INSERT INTO flights (flight_number, departure_airport_code, arrival_airport_code, scheduled_departure, scheduled_arrival, status, gate, aircraft_id, pilot_id, copilot_id, passengers_count) 
VALUES 

-- ВЧЕРА 5 
-- Самолет 1 летает Мск-Питер
('SU-101', 'SVO', 'LED', CURRENT_DATE - 1 + TIME '08:00', CURRENT_DATE - 1 + TIME '09:30', 'arrived', 'B10', 1, 1, 2, 150),
('SU-102', 'LED', 'SVO', CURRENT_DATE - 1 + TIME '11:00', CURRENT_DATE - 1 + TIME '12:30', 'arrived', 'A15', 1, 1, 2, 145),
-- Самолет 2 летает Мск-Стамбул
('SU-201', 'SVO', 'IST', CURRENT_DATE - 1 + TIME '10:00', CURRENT_DATE - 1 + TIME '14:00', 'arrived', 'C05', 2, 3, 4, 180),
('SU-202', 'IST', 'SVO', CURRENT_DATE - 1 + TIME '16:00', CURRENT_DATE - 1 + TIME '20:00', 'arrived', 'Gate 3', 2, 3, 4, 175),
-- Самолет 4 (Дальний) прилетел из Токио
('SU-999', 'HND', 'SVO', CURRENT_DATE - 1 + TIME '05:00', CURRENT_DATE - 1 + TIME '15:00', 'arrived', 'F01', 4, 5, 6, 350),

-- СЕГОДНЯ 15 
-- Утро (уже улетели/прилетели)
('SU-110', 'SVO', 'LED', CURRENT_DATE + TIME '07:00', CURRENT_DATE + TIME '08:30', 'arrived', 'B10', 1, 1, 2, 160),
('SU-111', 'LED', 'SVO', CURRENT_DATE + TIME '09:30', CURRENT_DATE + TIME '11:00', 'arrived', 'A12', 1, 1, 2, 155),
('SU-500', 'SVO', 'DXB', CURRENT_DATE + TIME '08:00', CURRENT_DATE + TIME '13:00', 'departed', 'D01', 3, 3, 4, 140),

-- День 
('SU-112', 'SVO', 'LED', CURRENT_DATE + TIME '12:30', CURRENT_DATE + TIME '14:00', 'departed', 'B11', 1, 1, 2, 148),
('SU-501', 'DXB', 'SVO', CURRENT_DATE + TIME '14:30', CURRENT_DATE + TIME '19:30', 'check_in', 'Gate 5', 3, 3, 4, 130),
('SU-777', 'SVO', 'HND', CURRENT_DATE + TIME '13:00', CURRENT_DATE + TIME '23:00', 'departed', 'F05', 4, 5, 6, 380),

-- Вечер 
('SU-113', 'LED', 'SVO', CURRENT_DATE + TIME '15:30', CURRENT_DATE + TIME '17:00', 'scheduled', 'A12', 1, 1, 2, 0),
('SU-114', 'SVO', 'LED', CURRENT_DATE + TIME '18:00', CURRENT_DATE + TIME '19:30', 'scheduled', 'B12', 1, 1, 2, 0),
('SU-115', 'LED', 'SVO', CURRENT_DATE + TIME '21:00', CURRENT_DATE + TIME '22:30', 'scheduled', 'A12', 1, 1, 2, 0),

('SU-301', 'SVO', 'IST', CURRENT_DATE + TIME '16:00', CURRENT_DATE + TIME '20:00', 'scheduled', 'C02', 2, NULL, NULL, 0), -- Пилоты еще не назначены
('SU-302', 'IST', 'SVO', CURRENT_DATE + TIME '21:30', CURRENT_DATE + TIME '01:30', 'scheduled', 'G10', 2, NULL, NULL, 0),

-- Ночные
('SU-800', 'SVO', 'JFK', CURRENT_DATE + TIME '20:00', CURRENT_DATE + INTERVAL '1 day' + TIME '06:00', 'scheduled', 'F08', 4, 5, 6, 0),
('SU-050', 'SVO', 'LED', CURRENT_DATE + TIME '23:50', CURRENT_DATE + INTERVAL '1 day' + TIME '01:20', 'scheduled', 'B01', 5, NULL, NULL, 0),

-- Проблемные
('SU-666', 'SVO', 'LED', CURRENT_DATE + TIME '10:00', CURRENT_DATE + TIME '11:30', 'cancelled', '---', NULL, NULL, NULL, 0),
('SU-007', 'SVO', 'DXB', CURRENT_DATE + TIME '11:00', CURRENT_DATE + TIME '16:00', 'delayed', 'D05', NULL, NULL, NULL, 0),

-- ЗАВТРА 5 
('SU-120', 'SVO', 'LED', CURRENT_DATE + 1 + TIME '08:00', CURRENT_DATE + 1 + TIME '09:30', 'scheduled', 'B10', 1, 1, 2, 0),
('SU-121', 'LED', 'SVO', CURRENT_DATE + 1 + TIME '11:00', CURRENT_DATE + 1 + TIME '12:30', 'scheduled', 'A15', 1, 1, 2, 0),
('SU-401', 'SVO', 'IST', CURRENT_DATE + 1 + TIME '10:00', CURRENT_DATE + 1 + TIME '14:00', 'scheduled', 'C05', 2, 3, 4, 0),
('SU-402', 'IST', 'SVO', CURRENT_DATE + 1 + TIME '16:00', CURRENT_DATE + 1 + TIME '20:00', 'scheduled', 'Gate 2', 2, 3, 4, 0),
('SU-900', 'HND', 'SVO', CURRENT_DATE + 1 + TIME '02:00', CURRENT_DATE + 1 + TIME '12:00', 'scheduled', 'F01', 4, 5, 6, 0),

-- ПОСЛЕЗАВТРА 5 
('SU-130', 'SVO', 'LED', CURRENT_DATE + 2 + TIME '09:00', CURRENT_DATE + 2 + TIME '10:30', 'scheduled', 'B10', 1, 1, 2, 0),
('SU-131', 'LED', 'SVO', CURRENT_DATE + 2 + TIME '12:00', CURRENT_DATE + 2 + TIME '13:30', 'scheduled', 'A15', 1, 1, 2, 0),
('SU-505', 'SVO', 'DXB', CURRENT_DATE + 2 + TIME '14:00', CURRENT_DATE + 2 + TIME '19:00', 'scheduled', 'D02', 3, 3, 4, 0),
('SU-506', 'DXB', 'SVO', CURRENT_DATE + 2 + TIME '20:30', CURRENT_DATE + 2 + TIME '01:30', 'scheduled', 'Gate 4', 3, 3, 4, 0),
('SU-888', 'SVO', 'JFK', CURRENT_DATE + 2 + TIME '18:00', CURRENT_DATE + 3 + TIME '04:00', 'scheduled', 'F09', 4, 5, 6, 0);