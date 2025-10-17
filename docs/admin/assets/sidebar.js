document.addEventListener("DOMContentLoaded", () => {
  const sidebarContainer = document.getElementById("sidebar-container");

  if (sidebarContainer) {
    fetch("sidebar.html")
      .then(res => res.text())
      .then(html => {
        sidebarContainer.innerHTML = html;

        // Highlight the active link based on current page
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
