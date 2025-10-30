document.getElementById("removeStudentForm").addEventListener("submit", async (e) => {
  e.preventDefault();
  const input = document.getElementById("studentId").value.trim();
  if (!input) {
    document.getElementById("message").textContent = "Please enter a student ID or full name";
    return;
  }

  try {
    const response = await fetch(`/api/admin/students/${encodeURIComponent(input)}`, { method: "DELETE" });
    const text = await response.text();
    document.getElementById("message").textContent = text;
  } catch (err) {
    document.getElementById("message").textContent = "Error: " + err.message;
  }
});
