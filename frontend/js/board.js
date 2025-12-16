const tableBody = document.querySelector('#board-table tbody');
const timeHeader = document.getElementById('time-header');
const tabs = document.querySelectorAll('.tab');
const dateSelector = document.getElementById('date-selector');

let currentType = 'departures';
let selectedDate = new Date().toISOString().split('T')[0];

const STATUS_MAP = {
    'scheduled': 'По расписанию', 'check_in': 'Регистрация', 'boarding': 'Посадка',
    'departed': 'Вылетел', 'arrived': 'Прибыл', 'delayed': 'Задержан', 'cancelled': 'Отменен'
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

function initDateButtons() {
    dateSelector.innerHTML = '';
    const today = new Date();
    
    for (let i = -1; i <= 2; i++) {
        const d = new Date(today);
        d.setDate(today.getDate() + i);
        
        const year = d.getFullYear();
        const monthStr = String(d.getMonth() + 1).padStart(2, '0');
        const dayStr = String(d.getDate()).padStart(2, '0');
        const apiDate = `${year}-${monthStr}-${dayStr}`;
        const displayDate = `${dayStr}.${monthStr}`;

        let label = '';
        if (i === -1) label = 'Вчера';
        else if (i === 0) label = 'Сегодня';
        else if (i === 1) label = 'Завтра';
        else label = 'Послезавтра';

        const btn = document.createElement('button');
        btn.className = `date-btn ${i === 0 ? 'active' : ''}`;
        btn.innerHTML = `<span class="date-label">${label}</span>${displayDate}`;
        
        btn.onclick = () => {
            document.querySelectorAll('.date-btn').forEach(b => b.classList.remove('active'));
            btn.classList.add('active');
            selectedDate = apiDate;
            loadFlights();
        };

        dateSelector.appendChild(btn);
    }
}

async function loadFlights() {
    try {
        const res = await fetch(`/api/public/${currentType}?date=${selectedDate}`);
        const data = await res.json();
        
        tableBody.innerHTML = '';
        if (!data.flights || data.flights.length === 0) {
            tableBody.innerHTML = `
                <tr>
                    <td colspan="5" class="empty-state">
                        <i class="fa-solid fa-plane-slash"></i>
                        На выбранную дату рейсов не найдено
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

initDateButtons();
loadFlights();

setInterval(loadFlights, 30000);