const tableBody = document.querySelector('#board-table tbody');
const timeHeader = document.getElementById('time-header');
const tabs = document.querySelectorAll('.tab');
let currentType = 'departures';

const STATUS_MAP = {
    'scheduled': 'По расписанию',
    'check_in': 'Регистрация',
    'boarding': 'Посадка',
    'departed': 'Вылетел',
    'arrived': 'Прибыл',
    'delayed': 'Задержан',
    'cancelled': 'Отменен'
};

const STATUS_ICONS = {
    'scheduled': '<i class="fa-regular fa-clock"></i>',
    'check_in': '<i class="fa-solid fa-passport"></i>',
    'boarding': '<i class="fa-solid fa-person-walking-luggage"></i>',
    'departed': '<i class="fa-solid fa-plane-departure"></i>',
    'arrived': '<i class="fa-solid fa-plane-arrival"></i>',
    'delayed': '<i class="fa-solid fa-triangle-exclamation"></i>',
    'cancelled': '<i class="fa-solid fa-ban"></i>'
};

async function loadFlights() {
    try {
        const res = await fetch(`/api/public/${currentType}`);
        const data = await res.json();
        
        tableBody.innerHTML = '';
        if (data.flights.length === 0) {
            tableBody.innerHTML = `
                <tr>
                    <td colspan="5" class="empty-state">
                        <i class="fa-solid fa-plane-slash"></i>
                        На данный момент рейсов нет
                    </td>
                </tr>`;
            return;
        }

        data.flights.forEach(f => {
            const row = document.createElement('tr');
            
            let routeHtml = '';
            if (currentType === 'departures') {
                routeHtml = `<b>${f.destination}</b>`;
            } else {
                routeHtml = `<b>${f.origin}</b>`;
            }

            const icon = STATUS_ICONS[f.status] || '';

            row.innerHTML = `
                <td><span class="flight-num">${f.flight_number}</span></td>
                <td>${routeHtml}</td>
                <td><span class="time-display">${f.time}</span></td>
                <td>
                    <span class="status-badge st-${f.status}">
                        ${icon} ${STATUS_MAP[f.status] || f.status}
                    </span>
                </td>
                <td style="font-weight:bold; color:#111;">${f.gate}</td>
            `;
            tableBody.appendChild(row);
        });
    } catch (e) {
        console.error("Ошибка загрузки:", e);
    }
}

tabs.forEach(tab => {
    tab.addEventListener('click', () => {
        tabs.forEach(t => t.classList.remove('active'));
        tab.classList.add('active');
        currentType = tab.dataset.type;
        
        if (tab.dataset.title) {
            timeHeader.innerText = tab.dataset.title;
        }
        
        loadFlights();
    });
});

loadFlights();

setInterval(loadFlights, 30000);