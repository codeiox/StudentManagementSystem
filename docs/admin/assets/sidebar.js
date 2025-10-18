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
