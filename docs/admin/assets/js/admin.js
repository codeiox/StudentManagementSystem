const profileCard = document.querySelector(".profile-card");
const editBtn = document.getElementById("edit-btn");
const saveBtn = document.getElementById("save-btn");
const cancelBtn = document.getElementById("cancel-btn");
const successMessage = document.getElementById("successMessage");

const changePhotoBtn = document.getElementById("changePhotoBtn");

// Store original values
let originalValues = {};

// Edit button click
editBtn.addEventListener("click", () => {
    // Store original values before editing
    originalValues = {
        name: document.getElementById("nameInput").value,
        //dob: document.getElementById("dobInput").value,
        email: document.getElementById("emailInput").value,
        phone: document.getElementById("phoneInput").value,
        address: document.getElementById("addressInput").value,
    };

    profileCard.classList.add("edit-mode");
    successMessage.classList.remove("show");
});

// Cancel button click
cancelBtn.addEventListener("click", () => {
    // Restore original values
    document.getElementById("nameInput").value = originalValues.name;
    //document.getElementById("dobInput").value = originalValues.dob;
    document.getElementById("emailInput").value = originalValues.email;
    document.getElementById("phoneInput").value = originalValues.phone;
    document.getElementById("addressInput").value = originalValues.address;

    profileCard.classList.remove("edit-mode");
    successMessage.classList.remove("show");
});

// Save button click
saveBtn.addEventListener("click", () => {
    // Get values from inputs
    const name = document.getElementById("nameInput").value;
    //const dob = document.getElementById("dobInput").value;
    const email = document.getElementById("emailInput").value;
    const phone = document.getElementById("phoneInput").value;
    const address = document.getElementById("addressInput").value;

    // Format date for display
    const dobFormatted = new Date(dob).toLocaleDateString("en-US");

    // Update display values
    document.getElementById("nameDisplay").textContent = name;
   // document.getElementById("dobDisplay").textContent = dobFormatted;
    document.getElementById("emailDisplay").textContent = email;
    document.getElementById("phoneDisplay").textContent = phone;
    document.getElementById("addressDisplay").textContent = address;

    // Exit edit mode
    profileCard.classList.remove("edit-mode");

    // Show success message
    successMessage.classList.add("show");
    setTimeout(() => {
        successMessage.classList.remove("show");
    }, 3000);

    //!important - TODO: Here you would typically send the data to a server
    console.log("Profile updated:", { name, dob, email, phone, address });
});

// Change photo button
changePhotoBtn.addEventListener("click", () => {
    //TODO: Change photo functionality to be implemented later
    alert(
        "i will implement photo change later."
    );
});

// Logout button
document.getElementById("logout-btn").addEventListener("click", () => {
        window.location.href = '../../../index.html';
});

