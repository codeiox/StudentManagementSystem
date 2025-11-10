// Load sidebar HTML and highlight the active link
document.addEventListener("DOMContentLoaded", () => {
  const sidebarContainer = document.getElementById("sidebar-container");

  if (sidebarContainer) {
    // Inject sidebar.html into the container
    fetch("sidebar.html")
      .then(res => res.text())
      .then(html => {
        sidebarContainer.innerHTML = html;

        // Add 'active' class to the link matching current page
        const links = sidebarContainer.querySelectorAll("a");
        const currentPage = window.location.pathname;
        links.forEach(link => {
          if (link.getAttribute("href") === currentPage) {
            link.classList.add("active");
          }
        });
      })
      .catch(err => {
        console.error("Error loading sidebar:", err);
      });
  }
});


// admin Profile popup window
document.addEventListener('DOMContentLoaded', function() {
    // Wait a bit for sidebar to load
    setTimeout(() => {
        const profilePic = document.getElementById('profilePic');
        const popupWindow = document.getElementById('popupWindow');
        const popupOverlay = document.getElementById('popupOverlay');
        const closeBtn = document.getElementById('closeBtn');

        if (profilePic) {
            // Open popup
            profilePic.addEventListener('click', () => {
                console.log('Profile clicked!');
                
                popupWindow.classList.add('active');
                popupOverlay.classList.add('active');
            });
        }

        if (closeBtn) {
            // Close popup when clicking close button
            closeBtn.addEventListener('click', () => {
                popupWindow.classList.remove('active');
                popupOverlay.classList.remove('active');
            });
        }

        if (popupOverlay) {
            // Close popup when clicking overlay
            popupOverlay.addEventListener('click', () => {
                popupWindow.classList.remove('active');
                popupOverlay.classList.remove('active');
            });
        }

        // Close popup with Escape key
        document.addEventListener('keydown', (e) => {
            if (e.key === 'Escape' && popupWindow && popupWindow.classList.contains('active')) {
                popupWindow.classList.remove('active');
                popupOverlay.classList.remove('active');
            }
        });
    }, 100);
});