document.addEventListener("DOMContentLoaded", () => {
    const tabContainer = document.querySelector(".tabs");
    if (!tabContainer) return console.error("Tabs container not found.");

    const tabButtons = tabContainer.querySelectorAll(".tab-link");
    const tabContents = document.querySelectorAll(".tab-content");

    const params = new URLSearchParams(window.location.search);
    const studentId = params.get("id");
    if (!studentId) {
        alert("No student ID found in URL!");
        return;
    }

    async function fetchStudentData(studentId) {
        const [coursesRes, gradesRes, studentRes, programRes, documentsRes] =
            await Promise.all([
                fetch(`/api/admin/students/${studentId}/courses`).then((r) =>
                    r.ok ? r.json() : []
                ),
                fetch(`/api/admin/students/${studentId}/grades`).then((r) =>
                    r.ok ? r.json() : []
                ),
                fetch(`/api/admin/students/${studentId}`).then((r) =>
                    r.ok ? r.json() : {}
                ),
                fetch(`/api/admin/students/${studentId}/program`).then(r => r.ok ? r.json() : { major: "Undeclared", minor: "" }),
                fetch(`/api/admin/students/${studentId}/documents`).then((r) =>
                    r.ok ? r.json() : []
                ),
            ]);

        return {
            courses: {
                major: programRes.major || "Undeclared",
                minor: programRes.minor || "",
                currentCourses: coursesRes
                    .filter((c) => c.status?.toLowerCase() === "current")
                    .map((c) => ({
                        course_name: c.course_name,
                        credits: c.credits,
                        term: formatTerm(c.term),
                    })),
                previousCourses: coursesRes
                    .filter((c) => c.status?.toLowerCase() === "completed")
                    .map((c) => ({
                        course_name: c.course_name,
                        credits: c.credits,
                        term: formatTerm(c.term),
                    })),
            },
            grades: {
                current: gradesRes.filter(
                    (g) => g.status?.toLowerCase() === "current"
                ),
                previous: gradesRes.filter(
                    (g) => g.status?.toLowerCase() === "completed"
                ),
            },
            enrollment: {
                startDate: "August 21, 2022",
                status: "Active",
                graduation: "May 2026",
                history: [
                    "Transferred from Springfield Community College (Fall 2021)",
                    "Leave of Absence: Spring 2023",
                    "Re‑enrolled: Fall 2023",
                ],
            },
            // enrollment: {
            //     startDate: studentRes.startDate || "",
            //     status: studentRes.enrollment_status || "",
            //     graduation: studentRes.graduation || "",
            //     history: studentRes.history || []
            // },
            documents: documentsRes || [],
        };
    }

    function formatTerm(term) {
        if (!term) return "";
        const [season, year] = term.split(" ");
        const capitalized =
            season.charAt(0).toUpperCase() + season.slice(1).toLowerCase();
        return `${capitalized} ${year}`;
    }

    function renderTableWithTermSeparators(items, columns, isGrade = false) {
        let rows = "";
        let lastTerm = "";

        items.forEach((item) => {
            const termFormatted = formatTerm(item.term); // Capitalized term

            // Insert a blank row if term changes (but not before the first row)
            if (termFormatted !== lastTerm && lastTerm !== "") {
                rows += `<tr class="year-separator"><td colspan="${columns.length}"></td></tr>`;
            }

            rows += "<tr>";
            columns.forEach((col) => {
                if (col === "grade" && isGrade) {
                    // Handle "IP" for in-progress
                    let grade = item[col] || "";
                    if (item.status?.toLowerCase() === "current" && !grade) grade = "IP";
                    rows += `<td>${grade}</td>`;
                } else if (col === "term") {
                    rows += `<td>${termFormatted}</td>`; // Use formatted term
                } else {
                    rows += `<td>${item[col]}</td>`;
                }
            });
            rows += "</tr>";

            lastTerm = termFormatted;
        });

        return rows;
    }

    // Render tab content dynamically based on tabId
    function renderTabContent(tabId, data) {
        const el = document.getElementById(tabId);
        if (!el) return;

        if (tabId === "courses") {
            const c = data.courses || {};
            el.innerHTML = `
        <div class="class-registration">
            <a href="../admin/register.html?id=${studentId}">Course Registration</a>
        </div>
        <h2>Courses & Major</h2>
        <p><strong>Major:</strong> ${c.major || "N/A"}</p>
        ${c.minor ? `<p><strong>Minor:</strong> ${c.minor}</p>` : ""}
        <h3>Current Courses</h3>
        ${c.currentCourses && c.currentCourses.length
                    ? `<table class="courses-table">
                   <thead><tr><th>Term</th><th>Course Name</th><th>Credits</th></tr></thead>
                   <tbody>${renderTableWithTermSeparators(c.currentCourses, ["term", "course_name", "credits"])}</tbody>
               </table>`
                    : "<p>No current courses</p>"}
        <h3>Previous Courses</h3>
        ${c.previousCourses && c.previousCourses.length
                    ? `<table class="courses-table">
                   <thead><tr><th>Term</th><th>Course Name</th><th>Credits</th></tr></thead>
                   <tbody>${renderTableWithTermSeparators(c.previousCourses, ["term", "course_name", "credits"])}</tbody>
               </table>`
                    : "<p>No previous courses</p>"}
    `;
            return;
        }

        if (tabId === "grades") {
            const g = data.grades || {};
            el.innerHTML = `
        <h2>Grades</h2>
        <h3>Current Courses</h3>
        ${g.current && g.current.length
                    ? `<table class="courses-table">
                   <thead><tr><th>Term</th><th>Course</th><th>Grade</th></tr></thead>
                   <tbody>${renderTableWithTermSeparators(g.current, ["term", "course_name", "grade"], true)}</tbody>
               </table>`
                    : "<p>No current grades</p>"}
        <h3>Previous Courses</h3>
        ${g.previous && g.previous.length
                    ? `<table class="courses-table">
                   <thead><tr><th>Term</th><th>Course</th><th>Grade</th></tr></thead>
                   <tbody>${renderTableWithTermSeparators(g.previous, ["term", "course_name", "grade"], true)}</tbody>
               </table>`
                    : "<p>No previous grades</p>"}
    `;
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
                <ul>${(e.history || [])
                    .map((item) => `<li>${item}</li>`)
                    .join("")}</ul>`;
            return;
        }

        if (tabId === "documents") {
            const d = data.documents || [];
            el.innerHTML = `<h2>Documents</h2>${d.length
                ? `<ul>${d
                    .map(
                        (doc) =>
                            `<li><a href="${doc.url}" target="_blank">${doc.name}</a></li>`
                    )
                    .join("")}</ul>`
                : "<p>No documents available</p>"
                }`;
            return;
        }
    }

    // Handle tab switching
    tabContainer.addEventListener("click", async (ev) => {
        const target = ev.target.closest(".tab-link");
        if (!target) return;
        ev.preventDefault();

        tabButtons.forEach((b) => b.classList.remove("active"));
        tabContents.forEach((c) => c.classList.remove("active"));

        target.classList.add("active");
        const id = target.dataset.tab;
        const content = document.getElementById(id);
        if (!content) return;
        content.classList.add("active");

        // Render content for the selected tab
        const studentData = await fetchStudentData(studentId);
        renderTabContent(id, studentData);
    });

    // Load first tab automatically
    const initial =
        tabContainer.querySelector(".tab-link.active") ||
        tabContainer.querySelector(".tab-link");
    if (initial) initial.click();

    // -----------------------------
    // Advising Notes Feature
    // -----------------------------
    const noteInput = document.getElementById("advisingNoteInput");
    const saveNoteBtn = document.getElementById("saveNoteBtn");
    const notesList = document.getElementById("advisingNotesList");

    // Load notes from localStorage
    function loadNotes() {
        const notes = JSON.parse(localStorage.getItem(`advisingNotes_${studentId}`) || "[]");
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
        document.querySelectorAll(".delete-note-btn").forEach((btn) => {
            btn.addEventListener("click", () => deleteNote(btn.dataset.index));
        });
    }

    // Delete a note by index
    function deleteNote(index) {
        const notes = JSON.parse(localStorage.getItem(`advisingNotes_${studentId}`) || "[]");
        notes.splice(index, 1);
        localStorage.setItem(`advisingNotes_${studentId}`, JSON.stringify(notes));
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
            hour12: true,
        });

        const note = { text, date: formattedDate };
        const notes = JSON.parse(localStorage.getItem(`advisingNotes_${studentId}`) || "[]");

        notes.push(note); // add to bottom as AC says chronological ascending
        localStorage.setItem(`advisingNotes_${studentId}`, JSON.stringify(notes));

        noteInput.value = "";
        loadNotes();
    });
    loadNotes();
});
