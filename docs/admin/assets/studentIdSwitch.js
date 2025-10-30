document.addEventListener('DOMContentLoaded', () => {
    const roleSelect = document.querySelector('select[name="role"]');
    const studentIdContainer = document.getElementById('studentIdContainer');
    const studentIdInput = studentIdContainer.querySelector('input[name="studentId"]');

    // Function to toggle student_id field visibility and required attribute
    function toggleStudentIdField() {
        if (roleSelect.value === 'admin') {
            studentIdContainer.style.display = 'none'; // Hide the field
            studentIdInput.removeAttribute('required'); // Remove required
        } else {
            studentIdContainer.style.display = 'block'; // Show the field
            studentIdInput.setAttribute('required', 'true'); // Add required
        }
    }

    // Run on page load to set initial state
    toggleStudentIdField();

    // Run whenever role selection changes
    roleSelect.addEventListener('change', toggleStudentIdField);
});