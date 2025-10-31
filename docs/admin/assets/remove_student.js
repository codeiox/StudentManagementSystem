document.getElementById("removeStudentForm").addEventListener("submit", async (e) => {
  e.preventDefault();

  // Get student ID or full name from input
  const input = document.getElementById("studentId").value.trim();

  // Check if input is empty
  if (!input) {
    document.getElementById("message").textContent = "Please enter a student ID or full name";
    return;
  }

  // Show confirmation popup
  const confirmed = confirm(`Are you sure you want to remove the student "${input}"?`);
  if (!confirmed) return; // Stop if user clicks "Cancel"

  try {
    // Send DELETE request to server
    const response = await fetch(`/api/admin/students/${encodeURIComponent(input)}`, { method: "DELETE" });
    const text = await response.text();

    // Display response message
    document.getElementById("message").textContent = text;
  } catch (err) {
    // Display error message if fetch fails
    document.getElementById("message").textContent = "Error: " + err.message;
  }
});
