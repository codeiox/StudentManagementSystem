USE studentmanagementsystem;

CREATE TABLE IF NOT EXISTS Majors (
    major_id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    type ENUM('major', 'minor') NOT NULL,
    UNIQUE(name, type)
);

CREATE TABLE IF NOT EXISTS MajorCourses (
    major_id INT,
    course_id INT,
    required BOOLEAN DEFAULT TRUE,
    PRIMARY KEY (major_id, course_id),
    FOREIGN KEY (major_id) REFERENCES Majors(major_id),
    FOREIGN KEY (course_id) REFERENCES Courses(course_id)
);

-- Insert majors
INSERT INTO Majors (name, type) VALUES
('Computer Science', 'major'),
('Information Technology', 'major'),
('Software Engineering', 'major'),
('Cybersecurity', 'major'),
('Data Science', 'major'),
('Electrical Engineering', 'major'),
('Mechanical Engineering', 'major'),
('Civil Engineering', 'major'),
('Biology', 'major'),
('Chemistry', 'major'),
('Physics', 'major'),
('Mathematics', 'major'),
('Environmental Science', 'major'),
('Accounting', 'major'),
('Finance', 'major'),
('Marketing', 'major'),
('Management', 'major'),
('Business Administration', 'major'),
('Economics', 'major'),
('Nursing', 'major'),
('Public Health', 'major'),
('Psychology', 'major'),
('Social Work', 'major'),
('Kinesiology', 'major'),
('Nutrition Science', 'major'),
('English', 'major'),
('History', 'major'),
('Philosophy', 'major'),
('Art', 'major'),
('Graphic Design', 'major'),
('Music', 'major'),
('Theatre', 'major'),
('Communication Studies', 'major'),
('Journalism', 'major'),
('Elementary Education', 'major'),
('Secondary Education', 'major'),
('Special Education', 'major'),
('Early Childhood Education', 'major'),
('Sociology', 'major'),
('Political Science', 'major'),
('Anthropology', 'major'),
('Criminal Justice', 'major');

-- Insert minors
INSERT INTO Majors (name, type) VALUES
('Computer Science', 'minor'),
('Information Technology', 'minor'),
('Software Engineering', 'minor'),
('Data Science', 'minor'),
('Mathematics', 'minor'),
('Economics', 'minor'),
('Biology', 'minor'),
('Psychology', 'minor'),
('English', 'minor'),
('History', 'minor'),
('Art', 'minor'),
('Communication Studies', 'minor');
