USE studentmanagementsystem;

CREATE TABLE IF NOT EXISTS Courses (
    course_id INT AUTO_INCREMENT PRIMARY KEY,
    course_name VARCHAR(100) NOT NULL,
    course_description TEXT,
    credits INT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

USE studentmanagementsystem;

-- Computer Science
INSERT INTO Courses (course_name, course_description, credits) VALUES
('CSC 108 - Introduction to Programming', 'Introductory programming concepts in C++.', 4),
('CSC 109 - Intermediate C++', 'Object-oriented programming and advanced C++ topics.', 4),
('CSC 211 - Data Structures', 'Arrays, linked lists, stacks, queues, trees, and graphs.', 4),
('CSC 212 - Software Engineering', 'Principles of software design, testing, and project management.', 4),
('CSC 301 - Operating Systems', 'Processes, threads, memory management, and file systems.', 4),
('CSC 305 - Database Systems', 'Relational databases, SQL, and normalization.', 3),
('CSC 310 - Computer Networks', 'Networking fundamentals, TCP/IP, routing, and protocols.', 3),
('CSC 401 - Artificial Intelligence', 'Search, reasoning, and machine learning basics.', 3),
('CSC 402 - Machine Learning', 'Supervised and unsupervised learning algorithms.', 3),
('CSC 410 - Cybersecurity Fundamentals', 'Security principles, encryption, and threat analysis.', 3);

-- Mathematics
INSERT INTO Courses (course_name, course_description, credits) VALUES
('MATH 101 - Calculus I', 'Limits, derivatives, integrals, and applications.', 3),
('MATH 102 - Calculus II', 'Advanced integration techniques and series.', 3),
('MATH 201 - Probability and Statistics', 'Probability theory, distributions, and hypothesis testing.', 3),
('MATH 301 - Linear Algebra', 'Matrices, vector spaces, and transformations.', 3),
('MATH 305 - Differential Equations', 'Ordinary differential equations and applications.', 3),
('MATH 401 - Abstract Algebra', 'Groups, rings, and fields.', 3),
('MATH 402 - Real Analysis', 'Rigorous study of limits, continuity, and integration.', 3);

-- Biology
INSERT INTO Courses (course_name, course_description, credits) VALUES
('BIO 101 - General Biology', 'Cell biology, genetics, and evolution.', 3),
('BIO 201 - Human Anatomy', 'Structure and function of the human body.', 4),
('BIO 202 - Physiology', 'Study of organ systems and their functions.', 4),
('BIO 301 - Microbiology', 'Study of microorganisms and their impact.', 3),
('BIO 305 - Genetics', 'Principles of heredity and molecular genetics.', 3),
('BIO 401 - Molecular Biology', 'DNA, RNA, protein synthesis, and regulation.', 3);

-- Physics
INSERT INTO Courses (course_name, course_description, credits) VALUES
('PHYS 101 - Physics I', 'Mechanics, waves, and thermodynamics.', 4),
('PHYS 102 - Physics II', 'Electricity, magnetism, and optics.', 4),
('PHYS 201 - Modern Physics', 'Relativity, atomic structure, and nuclear physics.', 3),
('PHYS 301 - Quantum Mechanics', 'Wave functions, uncertainty, and quantum systems.', 3),
('PHYS 305 - Electromagnetism', 'Maxwell’s equations and electromagnetic theory.', 3);

-- English
INSERT INTO Courses (course_name, course_description, credits) VALUES
('ENG 101 - English I', 'Foundations of academic writing and composition.', 3),
('ENG 102 - English II', 'Advanced composition and critical analysis.', 3),
('ENG 201 - English Literature', 'Survey of classic and modern literature.', 3),
('ENG 301 - Shakespeare Studies', 'In-depth study of Shakespeare’s works.', 3),
('ENG 305 - Creative Writing', 'Writing fiction, poetry, and creative nonfiction.', 3);

-- History
INSERT INTO Courses (course_name, course_description, credits) VALUES
('HIST 101 - World History', 'Major events from ancient to modern times.', 3),
('HIST 201 - American History', 'Colonial period to present.', 3),
('HIST 301 - European History', 'Medieval to modern European developments.', 3),
('HIST 305 - History of Science', 'Scientific revolutions and their impact.', 3);

-- Art
INSERT INTO Courses (course_name, course_description, credits) VALUES
('ART 101 - History of Art', 'Survey of art movements and styles.', 3),
('ART 201 - Modern Art', '20th century art and beyond.', 3),
('ART 301 - Studio Art', 'Hands-on practice in drawing and painting.', 3),
('ART 305 - Digital Art', 'Art using digital tools and media.', 3);

-- Communication
INSERT INTO Courses (course_name, course_description, credits) VALUES
('COMM 101 - Public Communication', 'Principles of effective speaking and listening.', 3),
('COMM 201 - Interpersonal Communication', 'Dynamics of one-on-one and group communication.', 3),
('COMM 301 - Mass Media Studies', 'Analysis of media and its influence.', 3),
('COMM 305 - Organizational Communication', 'Communication in professional and organizational contexts.', 3);
