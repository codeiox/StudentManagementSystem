document.addEventListener("DOMContentLoaded", () => {
  const form = document.getElementById('studentForm');
  const actions = document.getElementById('actions');
  const status = document.getElementById('status');
  const backBtn = document.getElementById('backBtn');
  const registerBtn = document.getElementById('registerBtn');

  form.addEventListener('submit', async (e) => {
    e.preventDefault();

    const data = {
      //name: form.firstName.value + ' ' + form.lastName.value,
      firstName: form.firstName.value,
      lastName: form.lastName.value,
      dateofbirth: form.dateofbirth.value,
      phone: form.phone.value,
      email: form.email.value,
      address: form.address.value,
      gender: form.gender.value,
      role: form.role.value,
      studentId: form.studentId.value,
      username: form.username.value,
      password: form.password.value
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
        form.style.display = "none";
        actions.style.display = "block";
        backBtn.onclick = () => window.location.href = '/admin/dashboard.html';
        registerBtn.onclick = () => window.location.reload();
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