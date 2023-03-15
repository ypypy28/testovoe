-- DISPLAY 10 latest purchases with person's name and car's brand and model
SELECT p.full_name AS "Owner",
       c.brand AS "Car brand",
       c.model AS "Car model",
       pcr.purchase_timestamp AS "Date of purchase"
FROM person_car_relationship AS pcr
JOIN cars AS c ON c.id = pcr.car_id
JOIN persons AS p ON p.id = pcr.person_id
ORDER BY pcr.purchase_timestamp DESC
LIMIT 10;


-- DISPLAY current owners of cars
WITH latest_car_purchases AS (
    SELECT car_id, person_id, MAX(purchase_timestamp)
    FROM person_car_relationship
    GROUP BY car_id
) 
SELECT c.brand AS "Car brand",
       c.model AS "Car model",
       p.full_name AS "Owner"
FROM cars AS c
JOIN latest_car_purchases AS lcr ON c.id = lcr.car_id
JOIN persons AS p ON p.id = lcr.person_id;


-- DISPLAY how many cars each person owns
WITH latest_car_purchases AS (
    SELECT car_id, person_id, MAX(purchase_timestamp)
    FROM person_car_relationship
    GROUP BY car_id
) 
SELECT p.full_name AS "Owner",
       COUNT(lcr.car_id) as "Cars"
FROM persons as p
LEFT OUTER JOIN latest_car_purchases AS lcr ON p.id = lcr.person_id
GROUP BY lcr.person_id
ORDER BY "Cars" DESC;


-- DISPLAY owner of the car with id = 3 by day 2022-14-03
WITH owner_by_day AS (
    SELECT "2022-14-03" AS req_day,
           MAX(pcr.purchase_timestamp) AS ts,
           pcr.car_id AS car_id,
           pcr.person_id AS person_id
    FROM person_car_relationship AS pcr
    WHERE pcr.car_id = 3 AND pcr.purchase_timestamp < req_day
    GROUP BY pcr.car_id
)
SELECT c.brand AS "Car brand",
       c.model AS  "Car model",
       p.full_name AS "Owner",
       date(obd.ts) AS "Day of purchase",
       obd.req_day AS "Requested day"
FROM cars as c
JOIN owner_by_day AS obd ON c.id = obd.car_id
JOIN persons AS p ON p.id = obd.person_id
WHERE c.id = obd.car_id;
