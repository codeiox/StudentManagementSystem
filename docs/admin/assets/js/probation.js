
// Fetch and display probation students
document.addEventListener('DOMContentLoaded', function() {
    fetchProbationStudents();
});

async function fetchProbationStudents() {
    const probationList = document.getElementById('probation-list');

    try {
        // TODO: Replace with your actual API endpoint
        const response = await fetch('/api/students/probation');
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }

        const staticData = [
    { gpa: 1.85, course: 'Computer Science', semester: 'Fall 2025', absences: 8 },
    { gpa: 1.95, course: 'Business Administration', semester: 'Fall 2025', absences: 12 },
    { gpa: 1.78, course: 'Engineering', semester: 'Fall 2025', absences: 6 },
    { gpa: 2.05, course: 'Psychology', semester: 'Fall 2025', absences: 10 },
    { gpa: 1.92, course: 'Mathematics', semester: 'Fall 2025', absences: 7 },
    { gpa: 1.88, course: 'Biology', semester: 'Fall 2025', absences: 9 }
];

        const apiStudents = await response.json();
        const students = apiStudents.map((student, index) => ({
            id: student.student_id, 
            name: `${student.first_name} ${student.last_name}`, 
            gpa: staticData[index % staticData.length].gpa, //  placeholder
            course: staticData[index % staticData.length].course, //  placeholder
            semester: staticData[index % staticData.length].semester, //  placeholder
            absences: staticData[index % staticData.length].absences //  placeholder
        }));

       console.log('Fetched students:', students);

        displayProbationStudents(students);

    } catch (error) {
        console.error('Error fetching probation students:', error);
        probationList.innerHTML = '<div class="no-data-message">Error loading student data. Please try again later.</div>';
    }
}

function displayProbationStudents(students) {
    const probationList = document.getElementById('probation-list');

    if (!students || students.length === 0) {
        probationList.innerHTML = '<div class="no-data-message">No students on probation at this time.</div>';
        return;
    }

    probationList.innerHTML = students.map(student => `
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
                    <span class="detail-value ${student.gpa < 2.0 ? 'gpa-low' : 'gpa-medium'}">${student.gpa.toFixed(2)}</span>
                </div>
                <div class="student-detail-row">
                    <span class="detail-label">Absences:</span>
                    <span class="detail-value">${student.absences} days</span>
                </div>
                <div class="student-detail-row">
                    <span class="detail-label">Semester:</span>
                    <span class="detail-value">${student.semester}</span>
                </div>
            </div>
        </div>
    `).join('');
}