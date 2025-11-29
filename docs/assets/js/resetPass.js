const successMessage = document.getElementById("successMessage");
const passwordError = document.getElementById("password-error");

function showError(message) {
    const errorElement = document.getElementById("password-error");
    if (errorElement) {
        errorElement.textContent = message;
        errorElement.style.display = "block";
        setTimeout(() => {
            errorElement.style.display = "none";
        }, 7000);
    } else {
        console.error("Error element not found:", message);
    }
}

function showSuccess(message) {
    successMessage.textContent = message;
    successMessage.style.display = "block";
    setTimeout(() => {
        successMessage.style.display = "none";
        window.location.href = "../index.html";
    }, 3000);
}

async function resetPassword(event) {
    event.preventDefault();

    // Clear previous messages
    successMessage.style.display = "none";
    passwordError.style.display = "none";

    const username = document.getElementById("username").value;
    const newPassword = document.getElementById("new-password").value;

    try {
        const response = await fetch("/api/reset_password", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify({ username, new_password: newPassword }),
        });

        const result = await response.json();

        if (response.ok) {
            showSuccess("Password reset successfully!");
        } else {
            showError(result.message || "Failed to reset password.");
        }
    } catch (error) {
        showError("An error occurred. Please try again.");
    }
}

document.addEventListener("DOMContentLoaded", () => {
    document
        .getElementById("resetPass")
        .addEventListener("submit", resetPassword);
});

const btn = document.querySelector(".fa-eye");
const passwordEl = document.querySelector(".my-password");

btn.addEventListener("click", () => {
    if (passwordEl.type === "password") {
        passwordEl.type = "text";
        btn.classList.replace("fa-eye", "fa-eye-slash");
    } else {
        passwordEl.type = "password";
        btn.classList.replace("fa-eye-slash", "fa-eye");
    }
});

const back_btn = document.querySelector(`.back-btn button`);
back_btn.addEventListener("click", () => {
    window.location.href = "index.html";
});
