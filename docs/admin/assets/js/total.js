// Function to fetch and update all totals
async function updateTotals() {
    try {
        // 1. Fetch Students
        const studentsRes = await fetch('/api/students/total');
        if (!studentsRes.ok) throw new Error(`Students API error: ${studentsRes.status}`);
        const studentsData = await studentsRes.json();
        
        // Update only the <p> inside #total-student card
        document.querySelector('#total-student p').textContent = studentsData.total_students;
    } catch (error) {
        console.error('Failed to load totals:', error);
        // Optional: Show user-friendly message in UI
        document.querySelectorAll('.totals p').forEach(p => p.textContent = 'Error');
    }
}
// Run when page loads
document.addEventListener('DOMContentLoaded', updateTotals);