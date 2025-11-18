// ===============================
// Fetch and render student data
// ===============================

let studentsData = []; // Store students data globally for sorting
let currentSortOrder = "asc"; // Track current sort order

// Function to render students table
function renderStudents(students) {
    const tableBody = document.querySelector("#studentTableBody");
    tableBody.innerHTML = ""; // Clear any existing rows in the table

    // Loop through each student and build a table row
    students.forEach((student, index) => {
        // Capitalize enrollment status for display (e.g., "active" → "Active")
        const status = student.enrollmentStatus;
        const displayStatus = status
            ? status[0].toUpperCase() + status.slice(1)
            : "";

        // Build HTML row with student data
        const row = `
      <tr class="clickable-row" style="cursor: pointer;" data-student-id="${student.student_id}">
        <td><input type="radio" name="selectedStudent" value="${student.student_id}"></td>
        <td>${student.student_id}</td>
        <td>${student.first_name} ${student.last_name}</td>
        <td>${student.dob}</td>
        <td>${student.gender}</td>
        <td>${student.phone}</td>
        <td>${student.email}</td>
        <td>${student.address}</td>
        <td>${displayStatus}</td>
      </tr>
    `;

        // Append row to table body
        tableBody.innerHTML += row;
    });
}

// Function to sort students by name
function sortStudentsByName(order = "asc") {
    const sorted = [...studentsData].sort((a, b) => {
        const nameA = `${a.first_name} ${a.last_name}`.toLowerCase();
        const nameB = `${b.first_name} ${b.last_name}`.toLowerCase();

        if (order === "asc") {
            return nameA.localeCompare(nameB);
        } else {
            return nameB.localeCompare(nameA);
        }
    });

    renderStudents(sorted);
}

// Send GET request to backend API to retrieve all students
fetch("/api/admin/students")
    .then((res) => res.json())
    .then((students) => {
        studentsData = students; // Store data globally
        renderStudents(students); // Initial render

        // Add event delegation for row clicks
        const tableBody = document.querySelector("#studentTableBody");
        tableBody.addEventListener("click", (e) => {
            const row = e.target.closest("tr.clickable-row");
            if (row) {
                const radio = row.querySelector('input[type="radio"]');
                if (radio) {
                    radio.checked = true;
                    document
                        .querySelectorAll("tr.clickable-row")
                        .forEach((r) => r.classList.remove("selected"));
                    row.classList.add("selected");
                }
            }
        });
    })
    .catch((error) => {
        console.error("Error fetching students:", error);
    });

// ===============================
// Sort toggle button handler
// ===============================
document.querySelector("#toggleSort").addEventListener("click", () => {
    // Toggle between ascending and descending
    currentSortOrder = currentSortOrder === "asc" ? "desc" : "asc";
    sortStudentsByName(currentSortOrder);

    // Update button text to show current sort direction
    const sortButton = document.querySelector("#toggleSort");
    if (currentSortOrder === "asc") {
        sortButton.innerHTML = "FullName &uarr;"; // Up arrow for A-Z
    } else {
        sortButton.innerHTML = "FullName &darr;"; // Down arrow for Z-A
    }
});

// ===============================
// Handle "Confirm" button click
// ===============================
document.querySelector(".confirm-button").addEventListener("click", () => {
    // Get selected student from radio buttons
    const selected = document.querySelector(
        "input[name='selectedStudent']:checked"
    );

    // If no student is selected, show alert
    if (!selected) {
        alert("Please select a student first.");
        return;
    }

    // Redirect to student profile page with selected student ID
    const studentId = selected.value;
    window.location.href = `/admin/student_profile.html?id=${studentId}`;
});
