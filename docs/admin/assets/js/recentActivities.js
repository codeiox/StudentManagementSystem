// demo data for recent activities
const demoActivities = [
    {
        id: 1,
        type: 'login',
        description: 'User logged in from Chrome browser',
        timestamp: new Date(Date.now() - 1000 * 60 * 15), // 15 minutes ago
    },
    {
        id: 2,
        type: 'request',
        description: 'Requested password reset',
        timestamp: new Date(Date.now() - 1000 * 60 * 30), // 30 minutes ago
    },
    {
        id: 3,
        type: 'update',
        description: 'Updated profile information',
        timestamp: new Date(Date.now() - 1000 * 60 * 60 * 1), // 1 hour ago
    },
];


// Function to format timestamp into readable format
function formatTimestamp(date) {
    const now = new Date();
    const diff = now - date;
    const minutes = Math.floor(diff / 60000);
    const hours = Math.floor(diff / 3600000);
    const days = Math.floor(diff / 86400000);

    if (minutes < 60) return `${minutes} minute${minutes !== 1 ? 's' : ''} ago`;
    if (hours < 24) return `${hours} hour${hours !== 1 ? 's' : ''} ago`;
    return `${days} day${days !== 1 ? 's' : ''} ago`;
}

// Function to populate activities
function loadRecentActivities() {
    const container = document.getElementById('recentActivities');
    
    // Simulate loading delay
    setTimeout(() => {
        if (demoActivities.length === 0) {
            container.innerHTML = '<p class="no-activities">No recent activities</p>';
            return;
        }

        // Generate HTML for activities
        const activitiesHTML = demoActivities.map(activity => `
            <div class="activity-item" data-type="${activity.type}">
                <div class="activity-content">
                    <p class="activity-description">${activity.description}</p>
                    <span class="activity-time">${formatTimestamp(activity.timestamp)}</span>
                </div>
            </div>
        `).join('');

        container.innerHTML = activitiesHTML;
    }, 800); // 800ms delay to simulate loading
}

// Load activities when DOM is ready
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', loadRecentActivities);
} else {
    loadRecentActivities();
}
