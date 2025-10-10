-- backend/db/init/created.sql
CREATE DATABASE IF NOT EXISTS studentmanagementsystem;

USE studentmanagementsystem;

-- ✅ Table for user authentication
CREATE TABLE IF NOT EXISTS Users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(100) UNIQUE NOT NULL,
    password VARCHAR(100) NOT NULL,
    role ENUM('admin', 'student') NOT NULL
);
