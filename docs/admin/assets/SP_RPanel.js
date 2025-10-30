// Extract studentId from URL query string (e.g., ?id=S1001)
const params = new URLSearchParams(window.location.search);
const studentId = params.get("id");

// If no ID is found, show an alert and stop
if (!studentId) {
  alert("No student ID found in URL!");
} else {
  // Fetch student data from backend using studentId
  fetch(`/api/admin/students/${studentId}`)
    .then(res => {
      if (!res.ok) throw new Error("Failed to fetch student data");
      return res.json();
    })
    .then(student => {
      // Fill in student profile fields with data from backend
      document.getElementById("studentId").textContent = student.student_id;
      document.getElementById("fullName").textContent = `${student.first_name} ${student.last_name}`;
      // Format DOB as MM-DD-YYYY
      const dob = new Date(student.dob);
      const formattedDob = `${String(dob.getMonth() + 1).padStart(2, '0')}-${String(dob.getDate()).padStart(2, '0')}-${dob.getFullYear()}`;
      document.getElementById("dob").textContent = formattedDob;
      document.getElementById("gender").textContent = student.gender;
      document.getElementById("email").textContent = student.email;
      document.getElementById("phone").textContent = student.phone;
      document.getElementById("address").textContent = student.address;

      // Set dropdown to current enrollment status (stored as lowercase)
      document.getElementById("enrollmentStatus").value = student.enrollmentStatus;

      // Display capitalized version of status for readability
      document.getElementById("status").textContent = capitalize(student.enrollmentStatus);
    })
    .catch(err => {
      console.error(err);
      alert("Error loading student profile.");
    });

  // Handle status update when "Update Status" button is clicked
  document.getElementById("updateStatusBtn").addEventListener("click", () => {
    const newStatus = document.getElementById("enrollmentStatus").value;

    // Send updated status to backend
    fetch(`/api/admin/students/${studentId}/status`, {
      method: "PUT",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ enrollmentStatus: newStatus })
    })
      .then(res => {
        if (res.ok) {
          // Show success message and update display
          showMessage("Enrollment status updated successfully!", "success");
          document.getElementById("status").textContent = capitalize(newStatus);
        } else {
          return res.text().then(err => { throw new Error(err); });
        }
      })
      .catch(err => {
        console.error("Error updating status:", err);
        showMessage("Error updating status: " + err.message, "error");
      });
  });

  // Handle back button to return to student list
  document.getElementById("backBtn").addEventListener("click", () => {
    window.location.href = "/admin/list_of_students.html";
  });
}

// Utility: Capitalize first letter of a string (e.g., "active" → "Active")
function capitalize(str) {
  if (!str) return "";
  return str.charAt(0).toUpperCase() + str.slice(1);
}

// Utility: Show a temporary success or error message on the page
function showMessage(message, type = "success") {
  const statusMessage = document.getElementById("statusMessage");
  if (!statusMessage) return;

  statusMessage.textContent = message;
  statusMessage.className = `status-message ${type}`;
  statusMessage.style.display = "block";

  // Hide message after 4 seconds
  setTimeout(() => {
    statusMessage.style.display = "none";
  }, 4000);
}
