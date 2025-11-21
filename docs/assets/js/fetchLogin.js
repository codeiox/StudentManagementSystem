document.getElementById("loginForm").addEventListener("submit", async function (e) {
    e.preventDefault(); // stop form from reloading the page

    const username = document.getElementById("first").value.trim();
    const password = document.getElementById("password").value;

    console.log("Username:", username);
    console.log("Password:", password);

    try {
        const res = await fetch('/api/login', {   // <-- fixed leading slash
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            credentials: 'same-origin',  // keep cookies/session
            body: JSON.stringify({ username, password })
        });

        const data = await res.json();

        if (res.ok && data.redirect) {
            // Redirect based on backend response
            window.location.href = data.redirect;
        } else {
            alert(data.message || "Login failed. Try again.");
        }
    } catch (err) {
        console.error("Login error:", err);
        alert("Server error. Try again later.");
    }
});
