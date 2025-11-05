document.addEventListener('DOMContentLoaded', function() {

 const usHolidays = [
        { title: 'New Year\'s Day', start: '2025-01-01', color: '#6c757d', display: 'background' },
        { title: 'Martin Luther King Jr. Day', start: '2025-01-20', color: '#6c757d', display: 'background' },
        { title: 'Presidents Day', start: '2025-02-17', color: '#6c757d', display: 'background' },
        { title: 'Memorial Day', start: '2025-05-26', color: '#6c757d', display: 'background' },
        { title: 'Independence Day', start: '2025-07-04', color: '#6c757d', display: 'background' },
        { title: 'Labor Day', start: '2025-09-01', color: '#6c757d', display: 'background' },
        { title: 'Columbus Day', start: '2025-10-13', color: '#6c757d', display: 'background' },
        { title: 'Veterans Day', start: '2025-11-11', color: '#6c757d', display: 'background' },
        { title: 'Thanksgiving', start: '2025-11-27', color: '#6c757d', display: 'background' },
        { title: 'Christmas', start: '2025-12-25', color: '#6c757d', display: 'background' }
    ];

    const customEvents = [
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
    ];

    const calendarEl = document.getElementById('calendar-section');
    const calendar = new FullCalendar.Calendar(calendarEl, {
        headerToolbar: {
            left: 'prev,next today',
            center: 'title',
            right: 'dayGridMonth,timeGridWeek,timeGridDay'
        },
        // Sets the initial view as Monthly calendar
        initialView: 'dayGridMonth',
        events: [
            ...usHolidays,
            ...customEvents
        ],
        height: 'auto',
        nowIndicator: true, // Show current time indicator
        slotMinTime: '08:00:00',
        slotMaxTime: '18:00:00',
        timeZone: 'local',
        allDaySlot: true
    });
    calendar.render();
});


