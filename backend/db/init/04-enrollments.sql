USE studentmanagementsystem;

CREATE TABLE IF NOT EXISTS Enrollments (
    enrollment_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    course_id INT NOT NULL,
    term VARCHAR(20),
    status ENUM('current','completed') NOT NULL,
    grade VARCHAR(5),
    course_credits INT DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES Users(id),
    FOREIGN KEY (course_id) REFERENCES Courses(course_id)
);
