document.addEventListener('DOMContentLoaded', function() {
    const calendarEl = document.getElementById('calendar-section');
    const calendar = new FullCalendar.Calendar(calendarEl, {
        initialView: 'dayGridMonth',
        headerToolbar: {
            left: 'prev,next today',
            center: 'title',
            right: 'dayGridMonth,dayGridWeek'
        },
        events: [
            {
                title: 'School Holiday',
                start: '2025-11-01',
                color: '#dc3545'
            },
            {
                title: 'Parent-Teacher Meeting',
                start: '2025-11-15',
                color: '#0d6efd'
            },
            {
                title: 'Thanksgiving Break',
                start: '2025-11-28',
                end: '2025-11-30',
                color: '#fd7e14'
            },
            {
                title: 'Winter Break Starts',
                start: '2025-12-20',
                color: '#198754'
            }
        ],
        height: 'auto',
    });
    calendar.render();
});


