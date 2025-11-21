/*----- STATUS CHART -----*/
async function loadStatusChart() {
    const canvas = document.getElementById("statusChart");
    if (!canvas) return; // Prevent crash

    try {
        const response = await fetch("/api/students/status");
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }

        const studentData = await response.json();

        const currentYear = new Date().getFullYear();
        const statusData = {
            labels: [
                "Active",
                "Graduated",
                "Pending",
                "Probation",
                "Suspended",
            ],
            datasets: [
                {
                    label: `${currentYear} Students Status`,
                    data: [
                        studentData.active,
                        studentData.graduated,
                        studentData.pending,
                        studentData.probation,
                        studentData.suspended,
                    ],
                    backgroundColor: [
                        "#386641",
                        "#2174f9ff",
                        "#fcbf49",
                        "#780000",
                        "#d00000",
                    ],
                    borderWidth: 1,
                },
            ],
        };

        const ctx = canvas.getContext("2d");
        new Chart(ctx, {
            type: "bar",
            data: statusData,
            options: {
                scales: {
                    y: { beginAtZero: true },
                },
            },
        });
    } catch (error) {
        console.error("Failed to load status chart:", error);
    }
}

/*-----  PROBATION STUDENTS FETCH -----*/
async function fetchProbationStudents() {
    const probationList = document.getElementById("probation-list");
    if (!probationList) return; // Prevent crash

    try {
        const response = await fetch("/api/students/probation");
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }

        // Placeholder data (remove later)
        const staticData = [
            {
                gpa: 1.85,
                course: "Computer Science",
                semester: "Fall 2025",
                absences: 8,
            },
            {
                gpa: 1.95,
                course: "Business Administration",
                semester: "Fall 2025",
                absences: 12,
            },
            {
                gpa: 1.78,
                course: "Engineering",
                semester: "Fall 2025",
                absences: 6,
            },
            {
                gpa: 2.05,
                course: "Psychology",
                semester: "Fall 2025",
                absences: 10,
            },
            {
                gpa: 1.92,
                course: "Mathematics",
                semester: "Fall 2025",
                absences: 7,
            },
            {
                gpa: 1.88,
                course: "Biology",
                semester: "Fall 2025",
                absences: 9,
            },
        ];

        const apiStudents = await response.json();
        const students = apiStudents.map((student, index) => ({
            id: student.student_id,
            name: `${student.first_name} ${student.last_name}`,
            gpa: staticData[index % staticData.length].gpa,
            course: staticData[index % staticData.length].course,
            semester: staticData[index % staticData.length].semester,
            absences: staticData[index % staticData.length].absences,
        }));

        displayProbationStudents(students);
    } catch (error) {
        console.error("Error fetching probation students:", error);
        probationList.innerHTML =
            '<div class="no-data-message">Error loading student data. Please try again later.</div>';
    }
}

function displayProbationStudents(students) {
    const probationList = document.getElementById("probation-list");
    if (!probationList) return;

    if (!students || students.length === 0) {
        probationList.innerHTML =
            '<div class="no-data-message">No students on probation at this time.</div>';
        return;
    }

    probationList.innerHTML = students
        .map(
            (student) => `
            <div class="student-card">
                <div class="student-header">
                    <div>
                        <div class="student-name">${student.name}</div>
                        <div class="student-id">${student.id}</div>
                    </div>
                    <div class="status-badge">PROBATION</div>
                </div>
                <div class="student-details">
                    <div class="student-detail-row">
                        <span class="detail-label">Course:</span>
                        <span class="detail-value">${student.course}</span>
                    </div>
                    <div class="student-detail-row">
                        <span class="detail-label">GPA:</span>
                        <span class="detail-value ${
                            student.gpa < 2.0 ? "gpa-low" : "gpa-medium"
                        }">${student.gpa.toFixed(2)}</span>
                    </div>
                    <div class="student-detail-row">
                        <span class="detail-label">Absences:</span>
                        <span class="detail-value">${
                            student.absences
                        } days</span>
                    </div>
                    <div class="student-detail-row">
                        <span class="detail-label">Semester:</span>
                        <span class="detail-value">${student.semester}</span>
                    </div>
                </div>
            </div>
        `
        )
        .join("");
}

/*-----  RECENT ACTIVITIES -----*/
const demoActivities = [
    {
        id: 1,
        type: "login",
        description: "User logged in from Chrome browser",
        timestamp: new Date(Date.now() - 1000 * 60 * 15),
    },
    {
        id: 2,
        type: "request",
        description: "Requested password reset",
        timestamp: new Date(Date.now() - 1000 * 60 * 30),
    },
    {
        id: 3,
        type: "update",
        description: "Updated profile information",
        timestamp: new Date(Date.now() - 1000 * 60 * 60),
    },
];

function formatTimestamp(date) {
    const now = new Date();
    const diff = now - date;

    const min = Math.floor(diff / 60000);
    const hr = Math.floor(diff / 3600000);
    const day = Math.floor(diff / 86400000);

    if (min < 60) return `${min} minute${min !== 1 ? "s" : ""} ago`;
    if (hr < 24) return `${hr} hour${hr !== 1 ? "s" : ""} ago`;
    return `${day} day${day !== 1 ? "s" : ""} ago`;
}

function loadRecentActivities() {
    const container = document.getElementById("recentActivities");
    if (!container) return;

    setTimeout(() => {
        if (demoActivities.length === 0) {
            container.innerHTML =
                '<p class="no-activities">No recent activities</p>';
            return;
        }

        container.innerHTML = demoActivities
            .map(
                (activity) => `
                <div class="activity-item" data-type="${activity.type}">
                    <div class="activity-content">
                        <p class="activity-description">${
                            activity.description
                        }</p>
                        <span class="activity-time">${formatTimestamp(
                            activity.timestamp
                        )}</span>
                    </div>
                </div>
            `
            )
            .join("");
    }, 800);
}

/*-------- FULLCALENDAR -------*/
function loadCalendar() {
    const calendarEl = document.getElementById("calendar-section");
    if (!calendarEl) return;

    const usHolidays = [
        {
            title: "New Year's Day",
            start: "2025-01-01",
            color: "#6c757d",
            display: "background",
        },
        {
            title: "Martin Luther King Jr. Day",
            start: "2025-01-20",
            color: "#6c757d",
            display: "background",
        },
        {
            title: "Presidents Day",
            start: "2025-02-17",
            color: "#6c757d",
            display: "background",
        },
        {
            title: "Memorial Day",
            start: "2025-05-26",
            color: "#6c757d",
            display: "background",
        },
        {
            title: "Independence Day",
            start: "2025-07-04",
            color: "#6c757d",
            display: "background",
        },
        {
            title: "Labor Day",
            start: "2025-09-01",
            color: "#6c757d",
            display: "background",
        },
        {
            title: "Columbus Day",
            start: "2025-10-13",
            color: "#6c757d",
            display: "background",
        },
        {
            title: "Veterans Day",
            start: "2025-11-11",
            color: "#6c757d",
            display: "background",
        },
        {
            title: "Thanksgiving",
            start: "2025-11-27",
            color: "#6c757d",
            display: "background",
        },
        {
            title: "Christmas",
            start: "2025-12-25",
            color: "#6c757d",
            display: "background",
        },
    ];

    const customEvents = [
        { title: "School Holiday", start: "2025-11-01", color: "#dc3545" },
        {
            title: "Parent-Teacher Meeting",
            start: "2025-11-15",
            color: "#0d6efd",
        },
        {
            title: "Thanksgiving Break",
            start: "2025-11-28",
            end: "2025-11-30",
            color: "#fd7e14",
        },
        { title: "Winter Break Starts", start: "2025-12-20", color: "#198754" },
    ];

    const calendar = new FullCalendar.Calendar(calendarEl, {
        headerToolbar: {
            left: "prev,next today",
            center: "title",
            right: "dayGridMonth,timeGridWeek,timeGridDay",
        },
        initialView: "dayGridMonth",
        events: [...usHolidays, ...customEvents],
        height: "auto",
        nowIndicator: true,
        slotMinTime: "00:00:00",
        slotMaxTime: "24:00:00",
        timeZone: "local",
        allDaySlot: true,
    });

    calendar.render();
}
/*----- TOTAL STUDENTS COUNTER -----*/
async function updateTotals() {
    try {
        // 1. Fetch Students
        const studentsRes = await fetch("/api/students/total");
        if (!studentsRes.ok)
            throw new Error(`Students API error: ${studentsRes.status}`);
        const studentsData = await studentsRes.json();

        // Update only the <p> inside #total-student card
        document.querySelector("#total-student p").textContent =
            studentsData.total_students;
    } catch (error) {
        console.error("Failed to load totals:", error);
        // Optional: Show user-friendly message in UI
        document
            .querySelectorAll(".totals p")
            .forEach((p) => (p.textContent = "Error"));
    }
}

/*----- UNIVERSAL DOMCONTENTLOADED -----*/
document.addEventListener("DOMContentLoaded", function () {
    loadStatusChart();
    fetchProbationStudents();
    loadRecentActivities();
    loadCalendar();
    updateTotals();
});
