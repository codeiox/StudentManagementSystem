document.addEventListener("DOMContentLoaded", () => {
  const stateSelect = document.getElementById("state");
  const cityInput = document.querySelector("input[name='city']");

  // Full list of US states
  const states = [
    "Alabama","Alaska","Arizona","Arkansas","California","Colorado","Connecticut",
    "Delaware","Florida","Georgia","Hawaii","Idaho","Illinois","Indiana","Iowa",
    "Kansas","Kentucky","Louisiana","Maine","Maryland","Massachusetts","Michigan",
    "Minnesota","Mississippi","Missouri","Montana","Nebraska","Nevada",
    "New Hampshire","New Jersey","New Mexico","New York","North Carolina","North Dakota",
    "Ohio","Oklahoma","Oregon","Pennsylvania","Rhode Island","South Carolina","South Dakota",
    "Tennessee","Texas","Utah","Vermont","Virginia","Washington","West Virginia","Wisconsin","Wyoming"
  ];

  // Populate state dropdown
  states.forEach(state => {
    const option = document.createElement("option");
    option.value = state;
    option.textContent = state;
    stateSelect.appendChild(option);
  });

  // --- Form submit ---
  const form = document.getElementById('studentForm');
  const status = document.getElementById("status");

  form.addEventListener('submit', async (e) => {
    e.preventDefault();

    // Validate address fields
    if (!form.street.value || !stateSelect.value || !cityInput.value) {
      status.textContent = "❌ Please fill in street, city, and state!";
      status.style.color = "red";
      return;
    }

    // --- Email validation ---
    const emailPattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    if (!emailPattern.test(form.email.value.trim())) {
      status.textContent = "❌ Please enter a valid email address.";
      status.style.color = "red";
      return;
    }

    // --- Phone validation ---
    const phoneValue = form.phone.value.replace(/\D/g, "");
    if (phoneValue.length !== 10) {
      status.textContent = "❌ Please enter a valid 10-digit phone number.";
      status.style.color = "red";
      return;
    }
    form.phone.value = `${phoneValue.slice(0,3)}-${phoneValue.slice(3,6)}-${phoneValue.slice(6)}`;

    // --- DOB validation (Age 15–65) ---
    const dob = new Date(form.dateofbirth.value);
    const today = new Date();
    let age = today.getFullYear() - dob.getFullYear();
    const monthDiff = today.getMonth() - dob.getMonth();
    if (monthDiff < 0 || (monthDiff === 0 && today.getDate() < dob.getDate())) {
      age--;
    }
    if (age < 15 || age > 65) {
      status.textContent = "❌ Age must be between 15 and 65 years.";
      status.style.color = "red";
      return;
    }

    // --- Build data object ---
    const data = {
      firstName: form.firstName.value,
      lastName: form.lastName.value,
      dateofbirth: form.dateofbirth.value,
      phone: form.phone.value,
      email: form.email.value,
      gender: form.gender.value,
      role: form.role.value,
      studentId: form.studentId.value,
      username: form.username.value,
      password: form.password.value,
      address: `${form.street.value}, ${cityInput.value}, ${stateSelect.value}`
    };

    try {
      const res = await fetch('/api/admin/students', { 
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify(data)
      });

      if (res.ok) {
        status.textContent = "✅ Student created successfully!";
        status.style.color = "green";
        form.reset();
      } else {
        const text = await res.text();
        status.textContent = "❌ Error: " + text;
        status.style.color = "red";
      }
    } catch (err) {
      status.textContent = "❌ Error: " + err.message;
      status.style.color = "red";
    }
  });
});
