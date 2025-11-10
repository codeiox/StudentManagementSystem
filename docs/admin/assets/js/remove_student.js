document.getElementById("removeStudentForm").addEventListener("submit", async (e) => {
  e.preventDefault();

  const rawInput = document.getElementById("studentId").value.trim();
  const messageEl = document.getElementById("message");

  if (!rawInput) {
    messageEl.textContent = "Please enter a student ID or full name.";
    messageEl.style.color = "red";
    return;
  }

  try {
    let student = null;

    // 1) Try direct GET by ID
    try {
      const infoResponse = await fetch(`/api/admin/students/${encodeURIComponent(rawInput)}`);
      if (infoResponse.ok) {
        student = await infoResponse.json();
      }
    } catch {
      // ignore fetch errors here
    }

    // 2) If not found, fetch all and search by name
    if (!student) {
      const allResponse = await fetch("/api/admin/students");
      if (!allResponse.ok) throw new Error("Failed to load student list from server.");

      const allStudents = await allResponse.json();

      // Try exact ID match
      student = allStudents.find(s => {
        const sid = s.student_id || s.studentId || s.id || "";
        return sid.toString().toLowerCase() === rawInput.toLowerCase();
      });

      // If no ID match, try name match
      if (!student) {
        const normalized = rawInput.replace(/,/g, "").split(/\s+/).filter(Boolean);
        if (normalized.length >= 2) {
          const first = normalized[0].toLowerCase();
          const last = normalized[normalized.length - 1].toLowerCase();
          student = allStudents.find(s => {
            const sf = (s.first_name || s.firstName || "").toLowerCase();
            const sl = (s.last_name || s.lastName || "").toLowerCase();
            return sf === first && sl === last;
          });
        }
      }

      if (!student) {
        messageEl.textContent = "❌ Student not found.";
        messageEl.style.color = "red";
        return;
      }
    }

    // Normalize fields
    const sid = student.student_id || student.studentId || student.id || "Unknown ID";
    const first = student.first_name || student.firstName || "";
    const last = student.last_name || student.lastName || "";

    // Confirm removal
    const confirmed = confirm(
      `Are you sure you want to remove this student?\n\n` +
      `Name: ${first} ${last}\n` +
      `ID: ${sid}`
    );
    if (!confirmed) return;

    // DELETE request
    const deleteResponse = await fetch(`/api/admin/students/${encodeURIComponent(sid)}`, {
      method: "DELETE"
    });

    const deleteText = await deleteResponse.text();

    // Show message
    if (deleteResponse.ok) {
      messageEl.textContent = `${first} ${last}, ID "${sid}" removed successfully.`;
      messageEl.style.color = "green";
    } else {
      messageEl.textContent = deleteText || "❌ Failed to remove student.";
      messageEl.style.color = "red";
    }

  } catch (err) {
    messageEl.textContent = "Error: " + err.message;
    messageEl.style.color = "red";
  }
});
