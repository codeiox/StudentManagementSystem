// ===============================
// Fetch and render student data
// ===============================

// Send GET request to backend API to retrieve all students
fetch("/api/admin/students")
    .then(res => res.json())
    .then(students => {
        const tableBody = document.querySelector("#studentTableBody");
        tableBody.innerHTML = ""; // Clear any existing rows in the table

        // Loop through each student and build a table row
        students.forEach((student, index) => {
            // Capitalize enrollment status for display (e.g., "active" → "Active")
            const status = student.enrollmentStatus;
            const displayStatus = status ? status[0].toUpperCase() + status.slice(1) : "";

            // Build HTML row with student data
            const row = `
        <tr>
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
    });

// ===============================
// Handle "Confirm" button click
// ===============================

document.querySelector(".confirm-button").addEventListener("click", () => {
    // Get selected student from radio buttons
    const selected = document.querySelector("input[name='selectedStudent']:checked");

    // If no student is selected, show alert
    if (!selected) {
        alert("Please select a student first.");
        return;
    }

    // Redirect to student profile page with selected student ID
    const studentId = selected.value;
    window.location.href = `/admin/student_profile.html?id=${studentId}`;
});