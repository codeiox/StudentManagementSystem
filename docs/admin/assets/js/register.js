document.addEventListener("DOMContentLoaded", () => {
    // --- FETCH AND DISPLAY STUDENT PROFILE INFO ---
    const studentId = new URLSearchParams(window.location.search).get("id");

    if (!studentId) {
        alert("No student ID found in URL!");
        return;
    }

    fetch(`/api/admin/students/${studentId}`)
        .then((res) => res.json())
        .then((student) => {
            document.getElementById(
                "fullName"
            ).textContent = `${student.first_name} ${student.last_name}`;

            document.getElementById("studentId").textContent =
                student.student_id;

            // Format DOB
            const dob = new Date(student.dob);
            const formattedDob =
                `${String(dob.getMonth() + 1).padStart(2, "0")}-` +
                `${String(dob.getDate()).padStart(2, "0")}-` +
                dob.getFullYear();

            document.getElementById("dob").textContent = formattedDob;
            document.getElementById("gender").textContent = student.gender;
            document.getElementById("email").textContent = student.email;
            document.getElementById("phone").textContent = student.phone;
            document.getElementById("address").textContent = student.address;
            document.getElementById("status").textContent =
                student.enrollmentStatus;

            // Set profile photo
            const photo = document.getElementById("studentPhoto");
            const gender = student.gender?.toLowerCase();

            if (gender === "female") {
                photo.src =
                    "https://api.dicebear.com/9.x/adventurer/svg?seed=Jude";
            } else if (gender === "male") {
                photo.src =
                    "https://api.dicebear.com/9.x/adventurer/svg?seed=Luis";
            } else {
                photo.src =
                    "https://api.dicebear.com/9.x/adventurer/svg?seed=other";
            }
        })
        .catch((err) => {
            console.error("Error loading profile:", err);
            alert("Could not load student info");
        });

    // --- COURSE REGISTRATION FORM LOGIC ---
    const form = document.getElementById("registrationForm");
    const messageDiv = document.getElementById("message");
    const courseSelect = document.getElementById("courses");
    const majorSelect = document.getElementById("major");
    const degreeTypeSelect = document.getElementById("degreeType");

    // Populate dropdown dynamically from backend
    fetch("/api/admin/courses")
        .then((res) => res.json())
        .then((courses) => {
            const groups = {};
            courses.forEach((c) => {
                const prefix = c.course_name.split(" ")[0]; // e.g., "CSC"
                if (!groups[prefix]) groups[prefix] = [];
                groups[prefix].push(c);
            });

            // Build optgroups
            Object.keys(groups).forEach((prefix) => {
                const optgroup = document.createElement("optgroup");
                optgroup.label = prefix; // shows as bold purple heading
                groups[prefix].forEach((c) => {
                    const option = document.createElement("option");
                    option.value = c.course_id;
                    option.textContent = `${c.course_name} (${c.credits} credits)`;
                    optgroup.appendChild(option);
                });
                courseSelect.appendChild(optgroup);
            });
        })
        .catch((err) => {
            console.error("Error fetching courses:", err);
            messageDiv.textContent = "Failed to load courses.";
            messageDiv.style.color = "red";
        });

    // Populate majors/minors and filter by degree type
    let allMajors = [];
    fetch("/api/admin/majors")
        .then((res) => res.json())
        .then((majors) => {
            allMajors = majors;
            renderMajorOptions();
        });

    degreeTypeSelect.addEventListener("change", renderMajorOptions);

    function renderMajorOptions() {
        const type = degreeTypeSelect.value; // "major" or "minor"
        majorSelect.innerHTML = "";
        const filtered = allMajors.filter((m) => m.type === type);
        filtered.forEach((m) => {
            const opt = document.createElement("option");
            opt.value = m.major_id;
            opt.textContent = m.name;
            majorSelect.appendChild(opt);
        });

        // Visual cue and guard against clipping
        majorSelect.style.position = "relative";
        majorSelect.style.zIndex = "1000";
    }

    form.addEventListener("submit", async (e) => {
        e.preventDefault();

        const studentId = new URLSearchParams(window.location.search).get("id");
        const course_id = courseSelect.value;
        const session = document.getElementById("session").value;
        const year = document.getElementById("year").value;
        const term = `${session} ${year}`;

        if (!studentId || !course_id || !term) {
            messageDiv.textContent =
                "Missing required fields: studentId, course_id, or term";
            messageDiv.style.color = "red";
            return;
        }

        try {
            const response = await fetch(
                `/api/admin/students/${studentId}/enrollments`,
                {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify({
                        course_id: parseInt(course_id),
                        term,
                    }),
                }
            );

            if (response.ok) {
                messageDiv.textContent = "Course registered successfully!";
                messageDiv.style.color = "green";
            } else {
                const errorText = await response.text();
                messageDiv.textContent = `Error: ${errorText}`;
                messageDiv.style.color = "red";
            }
        } catch (err) {
            console.error("Error submitting enrollment:", err);
            messageDiv.textContent = "Network error while registering course.";
            messageDiv.style.color = "red";
        }
    });

    // --Back button to student profile--
    document.getElementById("backBtn").addEventListener("click", () => {
        window.location.href = `/admin/student_profile.html?id=${studentId}`;
    });
});
