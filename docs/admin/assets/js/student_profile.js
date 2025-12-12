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

    // For enrollment tab: date only
    function formatDateOnly(dateStr) {
        if (!dateStr) return "N/A";
        const [year, month, day] = dateStr.split("-"); // "YYYY-MM-DD"
        return `${month}-${day}-${year}`;
    }

    // Format date strings to local time with timezone
    function formatDate(dateStr) {
        if (!dateStr) return "N/A";
        const date = new Date(dateStr + "Z"); // the "Z" ensures UTC
        return date.toLocaleString("en-US", {
            timeZone: "America/New_York",
            month: "2-digit",
            day: "2-digit",
            year: "numeric",
            hour: "2-digit",
            minute: "2-digit",
            second: "2-digit",
            hour12: true
        });
    }

    // Capitalize and format enrollment status
    function formatStatus(status) {
        if (!status) return "N/A";
        status = status.toLowerCase();
        return status.charAt(0).toUpperCase() + status.slice(1);
    }

    // Fetch all relevant student data in parallel from multiple endpoints
    async function fetchStudentData(studentId) {
        const [coursesRes, gradesRes, studentRes, programRes, documentsRes, enrollmentRes] =
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
                fetch(`/api/admin/students/${studentId}/enrollment-details`).then((r) =>
                    r.ok ? r.json() : {}
                ),
            ]);

        // Organize and format data
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
                startDate: enrollmentRes.startDate || "",
                status: enrollmentRes.status || "",
                graduation: enrollmentRes.graduation || "",
                history: enrollmentRes.history || []
            },
            documents: documentsRes || [],
        };
    }

    // Format term string, e.g., "fall 2022" → "Fall 2022"
    function formatTerm(term) {
        if (!term) return "";
        const [season, year] = term.split(" ");
        const capitalized =
            season.charAt(0).toUpperCase() + season.slice(1).toLowerCase();
        return `${capitalized} ${year}`;
    }

    // Render table rows with blank separators when the term changes
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
                <p><strong>Enrollment Date:</strong> ${formatDateOnly(e.startDate) || "N/A"}</p>
                <p><strong>Status:</strong> ${formatStatus(e.status)}</p>
                <p><strong>Expected Graduation:</strong> ${formatDateOnly(e.graduation) || "N/A"}</p>
                <h3>Enrollment History</h3>
                <ul>${(e.history || [])
                    .map((item) => `<li>${item}</li>`)
                    .join("")}</ul>`;
            return;
        }

        if (tabId === "advising") {
            // Setup advising notes HTML
            el.innerHTML = `
                <h2>Advising Notes</h2>
                <div id="advisingNotesList" class="notes-list"></div>
                <div class="add-note-section">
                    <textarea id="advisingNoteInput" placeholder="Write a new note..." rows="4"></textarea>
                    <button id="saveNoteBtn">Save</button>
                </div>
            `;
            initAdvisingNotes(studentId); // call function to handle load/save/delete
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

        // Update URL without reload
        const url = new URL(window.location);
        url.searchParams.set("tab", id);
        window.history.replaceState(null, "", url);

        // Render content for the selected tab
        const studentData = await fetchStudentData(studentId);
        renderTabContent(id, studentData);
    });

    // Load tab from URL or default to first
    const initialTab = params.get("tab") || "courses";
    const initial = tabContainer.querySelector(`.tab-link[data-tab="${initialTab}"]`) ||
        tabContainer.querySelector(".tab-link");
    if (initial) initial.click();

    // -----------------------------
    // Advising Notes Feature
    // -----------------------------
    function initAdvisingNotes(studentId) {
        const noteInput = document.getElementById("advisingNoteInput");
        const saveNoteBtn = document.getElementById("saveNoteBtn");
        const notesList = document.getElementById("advisingNotesList");
        const modal = document.getElementById("deleteModal");
        const passwordInput = document.getElementById("deletePassword");
        const confirmBtn = document.getElementById("confirmDeleteBtn");
        const cancelBtn = document.getElementById("cancelDeleteBtn");

        let noteToDeleteId = null;

        async function loadNotes() {
            const notesRes = await fetch(`/api/admin/students/${studentId}/advising-notes`);
            const notes = notesRes.ok ? await notesRes.json() : [];

            notesList.innerHTML = "";
            notes.forEach(note => {
                const div = document.createElement("div");
                div.classList.add("note-item");
                div.innerHTML = `
                    <div class="note-header">
                        <p class="note-date">${formatDate(note.date)}</p>
                        <button class="delete-note-btn" data-id="${note.id}">🗑️</button>
                    </div>
                    <p>${note.text}</p>
                `;
                notesList.appendChild(div);
            });
        }

        saveNoteBtn.addEventListener("click", async () => {
            const text = noteInput.value.trim();
            if (!text) return alert("Please write a note before saving.");

            const res = await fetch(`/api/admin/students/${studentId}/advising-notes`, {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify({ note: text })
            });

            if (res.ok) {
                noteInput.value = "";
                await loadNotes();
            } else {
                alert("Failed to save note.");
            }
        });

        notesList.addEventListener("click", (e) => {
            console.log("🗑️ Delete button clicked");
            const btn = e.target.closest(".delete-note-btn");
            if (!btn) return;
            noteToDeleteId = btn.dataset.id;
            passwordInput.value = "";
            modal.classList.add("show");
        });

        cancelBtn.onclick = () => {
            modal.classList.remove("show");
            noteToDeleteId = null;
            passwordInput.value = "";
        };


        confirmBtn.onclick = async () => {
            try {
                const password = passwordInput.value.trim();
                if (!password) return result["message"] = "Please enter your password.";
                if (!noteToDeleteId) return result["message"] = "No note selected to delete.";

                console.log("Sending DELETE for note:", noteToDeleteId, "with password:", password);
                const res = await fetch(`/api/admin/students/${studentId}/advising-notes/${noteToDeleteId}`, {
                    method: "DELETE",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify({ password })
                });
                console.log("Response status:", res.status);

                let data;
                try {
                    data = await res.json();
                } catch (err) {
                    console.error("Failed to parse JSON:", err);
                    result["message"] = "Server did not return valid JSON.";
                    return;
                }

                console.log("Response body:", data);

                if (res.ok) {
                    modal.classList.remove("show");
                    noteToDeleteId = null;
                    passwordInput.value = "";
                    await loadNotes();
                    result["message"] = data.message || "Note deleted successfully!";
                } else {
                    result["message"] = data.message || "Failed to delete note.";
                }
            } catch (err) {
                console.error("Delete request failed:", err);
                result["message"] = "Delete request failed. Check console for details.";
            }
        };
        loadNotes();
    }
});