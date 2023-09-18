CREATE TABLE students (
    student_id INT AUTO_INCREMENT PRIMARY KEY,
    student_name VARCHAR(255) NOT NULL,
    student_score DECIMAL(5, 2)
);
INSERT INTO students (student_name, student_score) VALUES
    ('Alice', 95.50),
    ('Bob', 88.00),
    ('Charlie', 75.25),
    ('David', 92.75);

