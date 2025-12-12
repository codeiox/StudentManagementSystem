# StudentManagementSystem

## Features Demo Video

https://github.com/user-attachments/assets/ef7dc518-ce3d-4313-b722-ea796c675018

## A comprehensive student management system capStone project built with C++ and Drogon framework, featuring a MySQL backend and a HTML, CSS, JavaScript frontend.

## Features

-   User authentication and authorization
-   Role-based access control (Admin, Student)
-   Student record and advising note management
-   Course enrollment and management
-   Grade tracking and reporting
-   Enrollment status monitoring
-   Responsive web interface

---

## Technologies Used

-   C++ with Drogon framework for backend development
-   MySQL for database management
-   HTML, CSS, JavaScript for frontend development
-   Docker for containerization
-   CMake for build management
-   Git for version control
-   GitHub for repository hosting
-   Visual Studio Code for code editing
-   Postman for API testing
-   .env file for environment variable management
-   JSON for configuration files
-   RESTful API design principles

---

## Setup Introduction

```bash
git clone https://github.com/yourusername/StudentManagementSystem.git
cd StudentManagementSystem
```

###### Configure Environment Variables

Create a .env file:

```bash
MYSQL_ROOT_PASSWORD=yourDB_password
MYSQL_DATABASE=yourlocalDb_Name
```

###### Run With Docker (Recommended)

```bash
docker-compose up --build
```

###### Access the Website Visit:

```
http://localhost:8080/index.html
```

###### API Testing Examples:

Use Postman or cURL to test endpoints such as:

```bash
GET /api/students/:id
POST /api/login
GET /api/students/probation
POST /api/enroll
```

---

## Contributions

If you’d like to improve this project, feel free to open an issue or submit a pull request. Bug fixes, new features, and suggestions are all appreciated.

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
