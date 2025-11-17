const profileCard = document.querySelector(".profile-card");
const editBtn = document.getElementById("edit-btn");
const saveBtn = document.getElementById("save-btn");
const cancelBtn = document.getElementById("cancel-btn");
const successMessage = document.getElementById("successMessage");
const changePhotoBtn = document.getElementById("changePhotoBtn");
const togglePasswordBtn = document.getElementById("toggle-password-btn");
const passwordSection = document.getElementById("password-change-section");
const passwordInput = document.getElementById("password");

// API endpoints
const API = "/api/admin/profile";
const UPDATE_API = "/api/admin/profile/update";

// Store original values
let originalValues = {};

// Helper functions
function setText(id, value) {
    const element = document.getElementById(id);
    if (element) {
        element.textContent = value || "-";
    }
}

function setValue(id, value) {
    const element = document.getElementById(id);
    if (element) {
        element.value = value || "";
    }
}

function formatDate(dateString) {
    if (!dateString) return "-";
    try {
        const date = new Date(dateString);
        const month = String(date.getMonth() + 1).padStart(2, "0");
        const day = String(date.getDate()).padStart(2, "0");
        const year = date.getFullYear();
        return `${month}/${day}/${year}`;
    } catch (e) {
        return "-";
    }
}

function showError(message) {
    const errorElement = document.getElementById("profile-error");
    if (errorElement) {
        errorElement.textContent = message;
        errorElement.style.display = "block";
        setTimeout(() => {
            errorElement.style.display = "none";
        }, 5000);
    } else {
        console.error("Error element not found:", message);
    }
}

function hideError() {
    const errorElement = document.getElementById("profile-error");
    if (errorElement) {
        errorElement.style.display = "none";
    }
}

// Fetch and populate profile data on page load
async function loadAdminProfile() {
    try {
        const response = await fetch(API, {
            credentials: "include",
        });

        const data = await response.json();

        if (!response.ok) {
            showError(data.error || "Failed to load profile.");
            return;
        }

        // Fill display elements
        setText("nameDisplay", data.full_name);
        setText("emailDisplay", data.email);
        setText("phoneDisplay", data.phone);
        setText("addressDisplay", data.address);
        setText("dobDisplay", formatDate(data.dob));
        setText("usernameDisplay", data.username);
        setText("employeeIdDisplay", data.employee_id || "-");

        // Fill input fields (hidden until edit)
        setValue("nameInput", data.full_name);
        setValue("emailInput", data.email);
        setValue("phoneInput", data.phone);
        setValue("addressInput", data.address);

        // Store original values
        originalValues = {
            name: data.full_name,
            email: data.email,
            phone: data.phone || "",
            address: data.address || "",
        };
    } catch (error) {
        console.error("Fetch error:", error);
        showError(
            error.message || "An error occurred while loading the profile."
        );
    }
}

// Update profile function
async function updateProfile(profileData) {
    try {
        const response = await fetch(UPDATE_API, {
            method: "PUT",
            credentials: "include",
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify(profileData),
        });

        const data = await response.json();

        if (!response.ok) {
            throw new Error(data.error || "Failed to update profile");
        }

        return { success: true, data };
    } catch (error) {
        console.error("Update error:", error);
        return { success: false, error: error.message };
    }
}

// Toggle password change section
if (togglePasswordBtn) {
    togglePasswordBtn.addEventListener("click", () => {
        passwordSection.classList.add("show");
        passwordInput.focus();
    });
}

// Edit button click
editBtn.addEventListener("click", () => {
    hideError();

    // Store current values before editing
    originalValues = {
        name: document.getElementById("nameInput").value,
        email: document.getElementById("emailInput").value,
        phone: document.getElementById("phoneInput").value,
        address: document.getElementById("addressInput").value,
    };

    profileCard.classList.add("edit-mode");
    successMessage.classList.remove("show");

    // Hide password section when entering edit mode
    passwordSection.classList.remove("show");
    passwordInput.value = "";
});

// Cancel button click
cancelBtn.addEventListener("click", () => {
    hideError();

    // Restore original values
    document.getElementById("nameInput").value = originalValues.name;
    document.getElementById("emailInput").value = originalValues.email;
    document.getElementById("phoneInput").value = originalValues.phone;
    document.getElementById("addressInput").value = originalValues.address;

    // Hide and clear password field
    passwordSection.classList.remove("show");
    passwordInput.value = "";

    profileCard.classList.remove("edit-mode");
    successMessage.classList.remove("show");
});

// Save button click
saveBtn.addEventListener("click", async () => {
    hideError();

    // Get values from inputs
    const name = document.getElementById("nameInput").value.trim();
    const email = document.getElementById("emailInput").value.trim();
    const phone = document.getElementById("phoneInput").value.trim();
    const address = document.getElementById("addressInput").value.trim();
    const password = passwordInput.value;

    // Basic validation
    if (!name || !email) {
        showError("Name and email are required");
        return;
    }

    // Email validation
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    if (!emailRegex.test(email)) {
        showError("Please enter a valid email address");
        return;
    }

    // Password validation (only if password is entered)
    if (password && password.length > 0) {
        const passwordRegex =
            /^(?=.*\d)(?=.*[a-z])(?=.*[A-Z])(?=.*[\W_]).{8,}$/;
        if (!passwordRegex.test(password)) {
            showError(
                "Password must contain at least one number, one uppercase letter, one lowercase letter, one special character, and be at least 8 characters long"
            );
            return;
        }
    }

    // Prepare profile data
    const profileData = {
        full_name: name,
        email: email,
        phone: phone,
        address: address,
    };

    // Only include password if it was entered
    if (password && password.length > 0) {
        profileData.password = password;
    }

    // Show loading state
    saveBtn.disabled = true;
    saveBtn.textContent = "Saving...";

    // Send update request
    const result = await updateProfile(profileData);

    // Reset button state
    saveBtn.disabled = false;
    saveBtn.textContent = "Save Changes";

    if (result.success) {
        // Update display values
        document.getElementById("nameDisplay").textContent = name;
        document.getElementById("emailDisplay").textContent = email;
        document.getElementById("phoneDisplay").textContent = phone || "-";
        document.getElementById("addressDisplay").textContent = address || "-";

        // Clear and hide password field
        passwordInput.value = "";
        passwordSection.classList.remove("show");

        // Update original values
        originalValues = {
            name: name,
            email: email,
            phone: phone,
            address: address,
        };

        // Exit edit mode
        profileCard.classList.remove("edit-mode");

        // Show success message
        successMessage.classList.add("show");
        setTimeout(() => {
            successMessage.classList.remove("show");
        }, 3000);

        console.log("Profile updated successfully");
    } else {
        showError(result.error || "Failed to update profile");
    }
});

// Change photo button
changePhotoBtn.addEventListener("click", () => {
    alert("Photo change functionality will be implemented later.");
});

// Logout button
document.getElementById("logout-btn").addEventListener("click", () => {
    document.cookie =
        "user_role=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;";
    document.cookie =
        "username=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;";
    window.location.href = "../../../index.html";
});

// Load profile on page load
document.addEventListener("DOMContentLoaded", loadAdminProfile);
