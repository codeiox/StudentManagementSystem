USE studentmanagementsystem;

CREATE TABLE IF NOT EXISTS Majors (
    major_id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    type ENUM('major', 'minor') NOT NULL,
    total_credits_required INT NOT NULL,
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

-- Insert majors (Associate Degree: ~60–65 credits)
INSERT INTO Majors (name, type, total_credits_required) VALUES
('Computer Science', 'major', 10), -- change it to 62 after testing is done
('Information Technology', 'major', 60),
('Software Engineering', 'major', 64),
('Cybersecurity', 'major', 62),
('Data Science', 'major', 60),
('Electrical Engineering', 'major', 65),
('Mechanical Engineering', 'major', 65),
('Civil Engineering', 'major', 65),
('Biology', 'major', 60),
('Chemistry', 'major', 60),
('Physics', 'major', 60),
('Mathematics', 'major', 60),
('Environmental Science', 'major', 60),
('Accounting', 'major', 60),
('Finance', 'major', 60),
('Marketing', 'major', 60),
('Management', 'major', 60),
('Business Administration', 'major', 60),
('Economics', 'major', 60),
('Nursing', 'major', 65),
('Public Health', 'major', 60),
('Psychology', 'major', 60),
('Social Work', 'major', 60),
('Kinesiology', 'major', 60),
('Nutrition Science', 'major', 60),
('English', 'major', 60),
('History', 'major', 60),
('Philosophy', 'major', 60),
('Art', 'major', 60),
('Graphic Design', 'major', 60),
('Music', 'major', 60),
('Theatre', 'major', 60),
('Communication Studies', 'major', 60),
('Journalism', 'major', 60),
('Elementary Education', 'major', 62),
('Secondary Education', 'major', 62),
('Special Education', 'major', 62),
('Early Childhood Education', 'major', 62),
('Sociology', 'major', 60),
('Political Science', 'major', 60),
('Anthropology', 'major', 60),
('Criminal Justice', 'major', 60);

-- Insert minors (Certificates: ~18–24 credits)
INSERT INTO Majors (name, type, total_credits_required) VALUES
('Computer Science', 'minor', 21),
('Information Technology', 'minor', 21),
('Software Engineering', 'minor', 21),
('Data Science', 'minor', 21),
('Mathematics', 'minor', 22),
('Economics', 'minor', 21),
('Biology', 'minor', 21),
('Psychology', 'minor', 21),
('English', 'minor', 21),
('History', 'minor', 21),
('Art', 'minor', 21),
('Communication Studies', 'minor', 21);

-- Modify Users table to add major + minor
ALTER TABLE Users
ADD COLUMN major_id INT NULL AFTER student_id,
ADD COLUMN minor_id INT NULL AFTER major_id,
ADD FOREIGN KEY (major_id) REFERENCES Majors(major_id),
ADD FOREIGN KEY (minor_id) REFERENCES Majors(major_id);
