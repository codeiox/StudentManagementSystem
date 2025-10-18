USE studentmanagementsystem;

CREATE TABLE IF NOT EXISTS Students (
    id INT AUTO_INCREMENT PRIMARY KEY,
    first_name VARCHAR(255) NOT NULL,
    last_name VARCHAR(255) NOT NULL,
    dob VARCHAR(255),
    email VARCHAR(255) NOT NULL,
    phone VARCHAR(50),
    address VARCHAR(255),
    sex VARCHAR(10),
    student_id VARCHAR(50) NOT NULL,
    username VARCHAR(255),
    password_hash VARCHAR(255) NOT NULL
    );
