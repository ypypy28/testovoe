CREATE TABLE cars (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    brand TEXT NOT NULL,                
    model TEXT NOT NULL,
    year INTEGER NOT NULL CHECK (year > 1900 AND year <= 9999)
);

CREATE TABLE persons (
    id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    full_name TEXT NOT NULL,
    birth_date DATETIME NOT NULL
);

CREATE TABLE person_car_relationship (
    id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    person_id INTEGER NOT NULL,
    car_id INTEGER NOT NULL,
    
    purchase_timestamp DATETIME NOT NULL,

    FOREIGN KEY (person_id) REFERENCES persons (id) ON DELETE CASCADE,
    FOREIGN KEY (car_id) REFERENCES cars (id) ON DELETE CASCADE,
    CONSTRAINT uq_car_purchase_timestamp UNIQUE(car_id, purchase_timestamp)
);

