CREATE TABLE IF NOT EXISTS EnrollmentDetails (
    id INT AUTO_INCREMENT PRIMARY KEY,
    student_id VARCHAR(50) NOT NULL UNIQUE,
    startDate DATE,
    graduation DATE
    );