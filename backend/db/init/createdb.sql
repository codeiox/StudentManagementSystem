-- Database name student-data
CREATE Database IF NOT EXISTS studentmanagementsystem;


USE studentmanagementsystem;

CREATE TABLE IF NOT EXISTS TemporaryTable (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100), 
    phone VARCHAR(20), 
    email VARCHAR(100)

);
