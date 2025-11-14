USE studentmanagementsystem;

CREATE TABLE IF NOT EXISTS Majors (
    major_id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS MajorCourses (
    major_id INT,
    course_id INT,
    required BOOLEAN DEFAULT TRUE,
    PRIMARY KEY (major_id, course_id),
    FOREIGN KEY (major_id) REFERENCES Majors(major_id),
    FOREIGN KEY (course_id) REFERENCES Courses(course_id)
);