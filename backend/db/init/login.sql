USE studentmanagementsystem;

CREATE TABLE IF NOT EXISTS Login (
    id INT AUTO_INCREMENT PRIMARY KEY, 
    username VARCHAR(20),
    passwd VARCHAR(20),
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)