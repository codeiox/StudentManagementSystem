// ===============================
// Fetch and render student data
// ===============================

// Send GET request to backend API
fetch("/api/admin/students")
  .then(res => res.json())
  .then(students => {
    const tableBody = document.querySelector("#studentTableBody");
    tableBody.innerHTML = ""; // Clear any existing rows

    // Loop through each student and build a table row
    students.forEach((student, index) => {
      const row = `
    <tr>
      <td><input type="radio" name="selectedStudent" value="${student.student_id}"></td>
      <td>${student.student_id}</td>
      <td>${student.first_name} ${student.last_name}</td>
      <td>${student.dob}</td>
      <td>${student.sex}</td>
      <td>${student.phone}</td>
      <td>${student.email}</td>
      <td>${student.address}</td>
      <td>${student.enrollmentStatus || "Active"}</td>
    </tr>
  `;
      tableBody.innerHTML += row;
    });
  });
