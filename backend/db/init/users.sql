CREATE DATABASE IF NOT EXISTS studentmanagementsystem;

USE studentmanagementsystem;

CREATE TABLE IF NOT EXISTS Users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    first_name VARCHAR(255) NOT NULL,
    last_name VARCHAR(255) NOT NULL,
    dob VARCHAR(50),
    email VARCHAR(255) NOT NULL UNIQUE,
    phone VARCHAR(50) NOT NULL UNIQUE,
    address VARCHAR(255),
    gender VARCHAR(10),
    student_id VARCHAR(50) UNIQUE,
    enrollment_status VARCHAR(20) DEFAULT 'Pending',
    username VARCHAR(100) UNIQUE NOT NULL,
    hashed_password VARCHAR(100) NOT NULL,
    role ENUM('admin', 'student') NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    );
