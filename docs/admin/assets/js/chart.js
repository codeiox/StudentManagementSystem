async function loadStatusChart() {
    try {
        const responseData = await fetch('/api/students/status');
        if (!responseData.ok) {
            throw new Error(`HTTP error! status: ${responseData.status}`);
        }
        const studentData = await responseData.json();
        
        const currentYear = new Date().getFullYear();
        const statusData = {
            labels: ['Active', 'Graduated', 'Pending', 'Probation', 'Suspended'],
            datasets: [{
                label: `${currentYear} Students Status`,
                data: [
                    studentData.active, 
                    studentData.graduated, 
                    studentData.pending, 
                    studentData.probation, 
                    studentData.suspended
                ],
                backgroundColor: ['#386641', '#2174f9ff', '#fcbf49', '#780000', '#d00000'],
                borderWidth: 1
            }]
        };

        // Get the canvas context and create the chart
        const ctx = document.getElementById('statusChart').getContext('2d');
        new Chart(ctx, {
            type: 'bar',
            data: statusData,
            options: {
                scales: {
                    y: { beginAtZero: true }
                }
            }
        });
    } catch (error) {
        console.error('Failed to load status chart:', error);
        // Optional: Display error message to user
    }
}

// Run when page loads
document.addEventListener('DOMContentLoaded', loadStatusChart);