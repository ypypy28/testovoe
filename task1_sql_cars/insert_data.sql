INSERT INTO cars (id, brand, model, year)
VALUES 
    (1, "BMW", "X6", 2020), 
    (2, "VAZ", "2107", 2012), 
    (3, "LADA", "Granta", 2017), 
    (4, "MAZDA","CX5", 2015), 
    (5, "VAZ", "2107", 2009), 
    (6, "HYUNDAI", "Azera", 2013), 
    (7, "HONDA", "City", 2011), 
    (8, "CHEVROLET", "Tornado", 2014);

INSERT INTO persons (id, full_name, birth_date)
VALUES
    (1, "Mike Tyson", "1966-06-30"),
    (2, "Robin Martin", "1952-12-05"),
    (3, "Guido Van Rossum", "1956-01-31"),
    (4, "Brandon Eich", "1961-07-04"),
    (5, "Rob Pike", "1956-01-01");


INSERT INTO person_car_relationship (id, person_id, car_id, purchase_timestamp)
VALUES
    (1, 1, 2, "2021-01-10T10:12:15"),
    (2, 1, 2, "2022-09-08T10:12:15"),
    (3, 1, 1, "2015-11-21T10:12:15"),
    (4, 2, 7, "2018-12-10T10:12:15"),
    (5, 3, 3, "2005-01-10T10:12:15"),
    (6, 1, 6, "2022-06-14T10:12:15"),
    (7, 4, 4, "2023-01-09T10:12:15"),
    (8, 5, 8, "2023-02-10T10:12:15"),
    (9, 3, 3, "2021-01-10T10:12:15"),
    (11, 4, 4, "2022-08-14T10:12:15"),
    (12, 2, 4, "2023-02-22T10:12:15"),
    (13, 5, 8, "2023-01-10T10:12:15"),
    (14, 5, 3, "2023-01-28T10:12:15"),
    (15, 4, 2, "2023-01-10T10:12:15");
                  