// remove_student.js
document.getElementById("removeStudentForm").addEventListener("submit", async (e) => {
    e.preventDefault();
    const id = document.getElementById("studentId").value;
  
    try {
      const response = await fetch(`/api/admin/students/${id}`, {
        method: "DELETE"
      });
  
      const text = await response.text();
      document.getElementById("message").textContent = text;
    } catch (err) {
      document.getElementById("message").textContent = "Error: " + err.message;
    }
  });
  