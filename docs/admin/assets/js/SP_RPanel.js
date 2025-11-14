// Get studentId from URL (e.g., ?id=S1001)
const params = new URLSearchParams(window.location.search);
const studentId = params.get("id");

// If no ID, stop here
if (!studentId) {
  alert("No student ID found in URL!");
} else {
  // Fetch student data from backend
  fetch(`/api/admin/students/${studentId}`)
    .then(res => {
      if (!res.ok) throw new Error("Failed to fetch student data");
      return res.json();
    })
    .then(student => {
      // Fill profile fields
      document.getElementById("studentId").textContent = student.student_id;
      document.getElementById("fullName").textContent = `${student.first_name} ${student.last_name}`;

      // Format DOB as MM-DD-YYYY
      const dob = new Date(student.dob);
      const formattedDob = `${String(dob.getMonth() + 1).padStart(2, '0')}-${String(dob.getDate()).padStart(2, '0')}-${dob.getFullYear()}`;
      document.getElementById("dob").textContent = formattedDob;

      document.getElementById("gender").textContent = student.gender;

      // Set profile photo based on gender
      const photo = document.getElementById("studentPhoto");
      if (student.gender) {
        const gender = student.gender.toLowerCase();
        if (gender === "female") photo.src = "https://api.dicebear.com/9.x/adventurer/svg?seed=Jude";
        else if (gender === "male") photo.src = "https://api.dicebear.com/9.x/adventurer/svg?seed=Luis";
        else photo.src = "https://api.dicebear.com/9.x/adventurer/svg?seed=other";
      } else {
        photo.src = "/admin/assets/img/default-avatar.png";
      }

      // Contact info
      document.getElementById("email").textContent = student.email;
      document.getElementById("phone").textContent = student.phone;
      document.getElementById("address").textContent = student.address;

      // Enrollment status dropdown + display
      document.getElementById("enrollmentStatus").value = student.enrollmentStatus;
      document.getElementById("status").textContent = capitalize(student.enrollmentStatus);
    })
    .catch(err => {
      console.error(err);
      alert("Error loading student profile.");
    });

  // Update status when button clicked
  document.getElementById("updateStatusBtn").addEventListener("click", () => {
    const newStatus = document.getElementById("enrollmentStatus").value;

    fetch(`/api/admin/students/${studentId}/status`, {
      method: "PUT",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ enrollmentStatus: newStatus })
    })
      .then(res => {
        if (res.ok) {
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

  // Back button → student list
  document.getElementById("backBtn").addEventListener("click", () => {
    window.location.href = "/admin/list_of_students.html";
  });
}

// Utility: capitalize first letter
function capitalize(str) {
  if (!str) return "";
  return str.charAt(0).toUpperCase() + str.slice(1);
}

// Utility: show temporary success/error message
function showMessage(message, type = "success") {
  const statusMessage = document.getElementById("statusMessage");
  if (!statusMessage) return;

  statusMessage.textContent = message;
  statusMessage.className = `status-message ${type}`;
  statusMessage.style.display = "block";

  // Hide after 4 seconds
  setTimeout(() => {
    statusMessage.style.display = "none";
  }, 4000);
}
