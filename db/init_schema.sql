CREATE TABLE pilots (
    id SERIAL PRIMARY KEY,
    last_name VARCHAR(50) NOT NULL,
    first_name VARCHAR(50) NOT NULL,
    middle_name VARCHAR(50),
    license_number VARCHAR(50) UNIQUE NOT NULL,
    rank VARCHAR(50) NOT NULL
);

CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    first_name VARCHAR(50) NOT NULL,
    middle_name VARCHAR(50),
    role VARCHAR(20) NOT NULL DEFAULT 'operator',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE airports (
    code CHAR(3) PRIMARY KEY,
    city VARCHAR(100) NOT NULL,
    name VARCHAR(200) NOT NULL,
    timezone VARCHAR(50) NOT NULL
);

CREATE TABLE aircrafts (
    id SERIAL PRIMARY KEY,
    model VARCHAR(100) NOT NULL,
    registration_number VARCHAR(20) UNIQUE NOT NULL,
    total_seats INT NOT NULL
);

CREATE TYPE flight_status AS ENUM (
    'scheduled', 
    'check_in',
    'boarding', 
    'departed', 
    'arrived', 
    'delayed', 
    'cancelled'
);

CREATE TABLE flights (
    id SERIAL PRIMARY KEY,
    flight_number VARCHAR(10) NOT NULL,
    departure_airport_code CHAR(3) NOT NULL REFERENCES airports(code),
    arrival_airport_code CHAR(3) NOT NULL REFERENCES airports(code),
    scheduled_departure TIMESTAMP WITH TIME ZONE NOT NULL,
    scheduled_arrival TIMESTAMP WITH TIME ZONE NOT NULL,
    status flight_status DEFAULT 'scheduled',
    gate VARCHAR(10),
    aircraft_id INT REFERENCES aircrafts(id),
    pilot_id INT REFERENCES pilots(id),   
    copilot_id INT REFERENCES pilots(id),
    actual_departure TIMESTAMP WITH TIME ZONE,
    passengers_count INT DEFAULT 0
);

CREATE TABLE audit_logs (
    id SERIAL PRIMARY KEY,
    user_id INT REFERENCES users(id),
    action_type VARCHAR(20) NOT NULL,
    table_name VARCHAR(50) NOT NULL,
    record_id INT NOT NULL,
    changes JSONB,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ТЕСТОВЫЕ ДАННЫЕ 

INSERT INTO airports (code, city, name, timezone) VALUES
('SVO', 'Москва', 'Шереметьево', 'Europe/Moscow'),
('LED', 'Санкт-Петербург', 'Пулково', 'Europe/Moscow'),
('DXB', 'Дубай', 'Международный аэропорт Дубай', 'Asia/Dubai'),
('IST', 'Стамбул', 'Аэропорт Стамбул', 'Europe/Istanbul');

INSERT INTO aircrafts (model, registration_number, total_seats) VALUES
('Boeing 737-800', 'RA-73001', 189),
('Airbus A320', 'RA-32005', 150);

INSERT INTO pilots (last_name, first_name, middle_name, license_number, rank) VALUES
('Иванов', 'Иван', 'Иванович', 'L-100', 'Captain'),
('Петров', 'Петр', 'Петрович', 'L-101', 'First Officer'),
('Сидоров', 'Алексей', 'Николаевич', 'L-102', 'First Officer');

INSERT INTO flights (flight_number, departure_airport_code, arrival_airport_code, scheduled_departure, scheduled_arrival, status, gate, aircraft_id, pilot_id, copilot_id) 
VALUES 
('SU-100', 'SVO', 'LED', NOW() - INTERVAL '30 minutes', NOW() + INTERVAL '30 minutes', 'departed', 'B12', 1, 1, 2),
('SU-520', 'SVO', 'DXB', NOW() + INTERVAL '2 hours', NOW() + INTERVAL '7 hours', 'check_in', 'D05', 2, 1, 3);

INSERT INTO flights (flight_number, departure_airport_code, arrival_airport_code, scheduled_departure, scheduled_arrival, status, gate) 
VALUES 
('SU-101', 'LED', 'SVO', NOW() - INTERVAL '1 hour', NOW() + INTERVAL '10 minutes', 'arrived', 'B12'),
('SU-521', 'DXB', 'SVO', NOW(), NOW() + INTERVAL '5 hours', 'delayed', NULL);

INSERT INTO users (username, password_hash, last_name, first_name, role) VALUES
('admin', '0d1e661db928505e45f649bc2b29b8d6814e2eee934aee167e30a8f5b3a80908', 'Админов', 'Сергей', 'admin'),
('operator', '4996c1c8fb63454c38b3b7236578df275b47649a860db99a5624dbcdbb1e4c1e', 'Операторова', 'Анна', 'operator');

INSERT INTO audit_logs (user_id, action_type, table_name, record_id, changes)
VALUES (1, 'CREATE', 'flights', 1, '{"message": "Создан рейс SU-100 (SVO -> LED)"}');