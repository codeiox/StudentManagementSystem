document.addEventListener("DOMContentLoaded", () => {
  const tabContainer = document.querySelector(".tabs");
  if (!tabContainer) return console.error("Tabs container not found.");

  const tabButtons = tabContainer.querySelectorAll(".tab-link");
  const tabContents = document.querySelectorAll(".tab-content");

  // Temporary demo data (replace with API later)
  const sampleStudentData = {
    courses: {
      major: "Computer Science",
      minor: "Mathematics",
      currentCourses: ["CSC208 - Systems Programming", "MTH201 - Linear Algebra", "ENG210 - Technical Writing"],
      previousCourses: ["CSC101 - Introduction to Computer Science", "MTH100 - College Algebra", "ENG100 - English Composition"]
    },
    grades: {
      current: [
        { course: "CSC208", grade: "A" },
        { course: "MTH201", grade: "B+" },
        { course: "ENG210", grade: "A-" }
      ],
      previous: [
        { course: "CSC101", grade: "A" },
        { course: "MTH100", grade: "B" },
        { course: "ENG100", grade: "A-" }
      ]
    },
    enrollment: {
      startDate: "August 21, 2022",
      status: "Active",
      graduation: "May 2026",
      history: [
        "Transferred from Springfield Community College (Fall 2021)",
        "Leave of Absence: Spring 2023",
        "Re‑enrolled: Fall 2023"
      ]
    },
    documents: [
      { name: "Transcript.pdf", url: "#" },
      { name: "DegreeAudit.pdf", url: "#" }
    ]
  };

  // Render tab content dynamically based on tabId
  function renderTabContent(tabId, data) {
    const el = document.getElementById(tabId);
    if (!el) return;

    if (tabId === "courses") {
      const c = data.courses || {};
      el.innerHTML = `
        <h2>Courses & Major</h2>
        <p><strong>Major:</strong> ${c.major || "N/A"}</p>
        ${c.minor ? `<p><strong>Minor:</strong> ${c.minor}</p>` : ""}
        <h3>Current Courses</h3>
        <ul>${(c.currentCourses || []).map(x => `<li>${x}</li>`).join("")}</ul>
        <h3>Previous Courses</h3>
        <ul>${(c.previousCourses || []).map(x => `<li>${x}</li>`).join("")}</ul>`;
      return;
    }

    if (tabId === "grades") {
      const g = data.grades || {};
      el.innerHTML = `
        <h2>Grades</h2>
        <h3>Current Courses</h3>
        ${g.current && g.current.length ? `
          <table>
            <thead><tr><th>Course</th><th>Grade</th></tr></thead>
            <tbody>${g.current.map(r => `<tr><td>${r.course}</td><td>${r.grade}</td></tr>`).join("")}</tbody>
          </table>` : "<p>No current grades</p>"}
        <h3>Previous Courses</h3>
        ${g.previous && g.previous.length ? `
          <table>
            <thead><tr><th>Course</th><th>Grade</th></tr></thead>
            <tbody>${g.previous.map(r => `<tr><td>${r.course}</td><td>${r.grade}</td></tr>`).join("")}</tbody>
          </table>` : "<p>No previous grades</p>"}`;
      return;
    }

    if (tabId === "enrollment") {
      const e = data.enrollment || {};
      el.innerHTML = `
        <h2>Enrollment</h2>
        <p><strong>Enrollment Date:</strong> ${e.startDate}</p>
        <p><strong>Status:</strong> ${e.status}</p>
        <p><strong>Expected Graduation:</strong> ${e.graduation}</p>
        <h3>Enrollment History</h3>
        <ul>${(e.history || []).map(item => `<li>${item}</li>`).join("")}</ul>`;
      return;
    }

    if (tabId === "documents") {
      const d = data.documents || [];
      el.innerHTML = `<h2>Documents</h2>${
        d.length
          ? `<ul>${d.map(doc => `<li><a href="${doc.url}" target="_blank">${doc.name}</a></li>`).join("")}</ul>`
          : "<p>No documents available</p>"
      }`;
      return;
    }
  }

  // Handle tab switching (adds/removes active class)
  tabContainer.addEventListener("click", (ev) => {
    const target = ev.target.closest(".tab-link");
    if (!target) return;
    ev.preventDefault();

    tabButtons.forEach(b => b.classList.remove("active"));
    tabContents.forEach(c => c.classList.remove("active"));

    target.classList.add("active");
    const id = target.dataset.tab;
    const content = document.getElementById(id);
    if (!content) return;
    content.classList.add("active");

    // Render content for the selected tab
    renderTabContent(id, sampleStudentData);
  });

  // Load first tab automatically
  const initial = tabContainer.querySelector(".tab-link.active") || tabContainer.querySelector(".tab-link");
  if (initial) initial.click();

  // -----------------------------
  // Advising Notes Feature
  // -----------------------------
  const noteInput = document.getElementById("advisingNoteInput");
  const saveNoteBtn = document.getElementById("saveNoteBtn");
  const notesList = document.getElementById("advisingNotesList");

  // Load notes from localStorage (persisted between refreshes)
  function loadNotes() {
    const notes = JSON.parse(localStorage.getItem("advisingNotes") || "[]");
    notesList.innerHTML = "";

    notes.forEach((note, index) => {
      const div = document.createElement("div");
      div.classList.add("note-item");
      div.innerHTML = `
        <div class="note-header">
          <p class="note-date">${note.date}</p>
          <button class="delete-note-btn" data-index="${index}">🗑️</button>
        </div>
        <p>${note.text}</p>
      `;
      notesList.appendChild(div);
    });

    // Attach delete button events
    document.querySelectorAll(".delete-note-btn").forEach(btn => {
      btn.addEventListener("click", () => {
        const index = btn.dataset.index;
        deleteNote(index);
      });
    });
  }

  // Delete a note by index
  function deleteNote(index) {
    const notes = JSON.parse(localStorage.getItem("advisingNotes") || "[]");
    notes.splice(index, 1);
    localStorage.setItem("advisingNotes", JSON.stringify(notes));
    loadNotes();
  }

  // Save new note with timestamp
  saveNoteBtn.addEventListener("click", () => {
    const text = noteInput.value.trim();
    if (!text) {
      alert("Please write a note before saving.");
      return;
    }

    const now = new Date();
    const formattedDate = now.toLocaleString("en-US", {
      month: "2-digit",
      day: "2-digit",
      year: "numeric",
      hour: "numeric",
      minute: "2-digit",
      hour12: true
    });

    const note = { text, date: formattedDate };
    const notes = JSON.parse(localStorage.getItem("advisingNotes") || "[]");
    notes.unshift(note); // newest note at top
    localStorage.setItem("advisingNotes", JSON.stringify(notes));

    noteInput.value = "";
    loadNotes();
  });

  // Initial load of notes on page start
  loadNotes();
});
