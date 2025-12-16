let currentTab = 'flights';
let resources = {};
let currentData = [];

let currentPage = 1;
const pageSize = 20;

const STATUS_MAP = {
    'scheduled': 'По расписанию', 'check_in': 'Регистрация', 'boarding': 'Посадка',
    'departed': 'Вылетел', 'arrived': 'Прибыл', 'delayed': 'Задержан', 'cancelled': 'Отменен'
};
const STATUS_ICONS = {
    'scheduled': 'fa-clock', 'check_in': 'fa-passport', 'boarding': 'fa-person-walking-luggage',
    'departed': 'fa-plane-departure', 'arrived': 'fa-plane-arrival', 'delayed': 'fa-triangle-exclamation', 'cancelled': 'fa-ban'
};
const ROLE_MAP = { 'admin': 'Администратор', 'operator': 'Оператор' };

const userName = localStorage.getItem('name') || 'User';
const userRole = localStorage.getItem('role') || 'operator';

document.getElementById('user-name').innerText = userName;
document.getElementById('user-role-label').innerText = ROLE_MAP[userRole] || userRole;

if (userRole === 'admin') {
    document.querySelectorAll('.admin-only').forEach(el => el.classList.remove('hidden'));
}

async function api(url, method = 'GET', body = null) {
    const token = localStorage.getItem('token');
    if (!token) {
        window.location.href = '/login';
        return;
    }
    
    const opts = { method, headers: { 'Authorization': token, 'Content-Type': 'application/json' } };
    if (body) opts.body = JSON.stringify(body);

    const res = await fetch(url, opts);
    
    if (res.status === 401) { 
        localStorage.clear(); 
        window.location.href = '/login'; 
        return;
    }

    if (res.ok) {
        const text = await res.text();
        if (!text) return {}; 
        try { return JSON.parse(text); } catch (e) { return {}; }
    } else {
        const errorText = await res.text();
        throw new Error(errorText || `Ошибка сервера: ${res.status}`);
    }
}

document.getElementById('logout-btn').onclick = () => { localStorage.clear(); window.location.href = '/'; };
document.getElementById('add-btn').onclick = () => openModal();

const prevBtn = document.getElementById('prev-page');
const nextBtn = document.getElementById('next-page');

if (prevBtn && nextBtn) {
    prevBtn.onclick = () => {
        if (currentPage > 1) {
            currentPage--;
            loadData();
        }
    };
    nextBtn.onclick = () => {
        currentPage++;
        loadData();
    };
}

function updatePaginationUI(meta) {
    const controls = document.getElementById('pagination-controls');
    if (!controls) return;

    controls.classList.remove('hidden');
    
    document.getElementById('total-records').innerText = meta.total;
    document.getElementById('current-page').innerText = meta.page;

    const totalPages = Math.ceil(meta.total / meta.limit);

    prevBtn.disabled = meta.page <= 1;
    nextBtn.disabled = meta.page >= totalPages;
}

function hidePaginationUI() {
    const controls = document.getElementById('pagination-controls');
    if (controls) controls.classList.add('hidden');
}

function switchTab(tab) {
    currentTab = tab;
    currentPage = 1; 
    document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
    event.currentTarget.classList.add('active');
    document.getElementById('add-btn').classList.toggle('hidden', tab === 'audit');
    loadData();
}

async function loadResources() {
    if (Object.keys(resources).length === 0) resources = await api('/api/admin/resources');
}

async function loadData() {
    const container = document.getElementById('table-container');
    container.innerHTML = '<div style="text-align:center; padding:40px; color:#999"><i class="fa-solid fa-spinner fa-spin fa-2x"></i></div>';
    
    try {
        if (currentTab === 'flights') {
            await loadResources();
            const data = await api(`/api/admin/flights?page=${currentPage}&limit=${pageSize}`);
            
            currentData = data.data; 
            renderFlights(data.data, container);
            updatePaginationUI(data);

        } else if (currentTab === 'users') {
            const data = await api('/api/admin/users');
            currentData = data.users;
            renderUsers(data.users, container);
            hidePaginationUI();
        } else {
            const data = await api('/api/admin/audit');
            renderAudit(data.logs, container);
            hidePaginationUI();
        }
    } catch (e) { 
        container.innerHTML = '<div style="text-align:center; color:#ef4444; padding:20px"><i class="fa-solid fa-circle-exclamation"></i> Ошибка загрузки данных</div>'; 
        console.error(e); 
    }
}

function renderFlights(list, container) {
    if (!list || list.length === 0) {
        container.innerHTML = '<div style="text-align:center; padding:40px; color:#999">Список пуст</div>'; return;
    }

    let html = `<table><thead><tr>
        <th width="10%">Рейс</th>
        <th width="15%">Маршрут</th>
        <th width="15%">Время</th>
        <th width="15%">Персонал</th>
        <th width="15%">Борт</th>
        <th width="8%" style="text-align:center">Пасс.</th>
        <th width="12%">Статус</th>
        <th width="15%">Выход</th>
        <th width="10%">Действия</th>
    </tr></thead><tbody>`;

    list.forEach(i => {
        const icon = STATUS_ICONS[i.status] || 'fa-circle';
        const pilot = i.pilot !== 'Не назначен' ? i.pilot : '<span style="color:#ccc">---</span>';
        const copilot = i.copilot !== 'Не назначен' ? i.copilot : '<span style="color:#ccc">---</span>';

        html += `<tr>
            <td>
                <div style="font-weight:700; font-size:16px; color:#111;">${i.flight_number}</div>
            </td>
            <td>
                <div style="display:flex; flex-direction:column; gap:4px;">
                    <div style="font-size:13px;"><i class="fa-solid fa-plane-departure" style="color:#bbb; width:16px;"></i> ${i.origin}</div>
                    <div style="font-size:13px;"><i class="fa-solid fa-plane-arrival" style="color:#bbb; width:16px;"></i> ${i.destination}</div>
                </div>
            </td>
            <td>
                <div style="font-size:13px; font-variant-numeric: tabular-nums;">${i.dep_time}</div>
                <div style="font-size:13px; font-variant-numeric: tabular-nums; color:#666;">${i.arr_time}</div>
            </td>
            <td>
                <div class="crew-list" style="font-size:12px; line-height:1.6;">
                    <div title="Командир"><i class="fa-solid fa-user-tie" style="color:#4b5563; width:14px;"></i> ${pilot}</div>
                    <div title="Второй пилот"><i class="fa-solid fa-user" style="color:#9ca3af; width:14px;"></i> ${copilot}</div>
                </div>
            </td>
            <td>
                <div style="font-size:12px; color:#4b5563;">
                    <i class="fa-solid fa-plane" style="color:#bbb;"></i> ${i.aircraft}
                </div>
            </td>
            <td style="text-align:center;">
                <div style="font-weight:600; font-size:14px;">${i.passengers}</div>
                <div style="font-size:10px; color:#999;">чел.</div>
            </td>
            <td>
                <span class="status-badge st-${i.status}" style="font-size:12px;">
                    <i class="fa-solid ${icon}"></i> ${STATUS_MAP[i.status]}
                </span>
            </td>
            <td>
                <div style="display:flex; align-items:center; gap:8px; margin-bottom:4px;">
                    <span style="background:#f3f4f6; padding:2px 6px; border-radius:4px; font-weight:700; font-size:13px; border:1px solid #e5e7eb;">
                        ${i.gate}
                    </span>
                    <span style="font-size:11px; color:#999; text-transform:uppercase;"></span>
                </div>
            </td>
            <td>
                <div style="display:flex; gap:6px;">
                    <button class="action-btn btn-edit-icon" title="Редактировать" onclick="openModal(${i.id})"><i class="fa-solid fa-pen"></i></button>
                    ${userRole === 'admin' ? `<button class="action-btn btn-delete-icon" title="Удалить" onclick="removeItem(${i.id})"><i class="fa-solid fa-trash"></i></button>` : ''}
                </div>
            </td>
        </tr>`;
    });

    container.innerHTML = html + '</tbody></table>';
}

function renderUsers(list, container) {
    let html = `<table><thead><tr>
        <th>ID</th><th>Сотрудник</th><th>Логин</th><th>Роль</th><th>Действия</th>
    </tr></thead><tbody>`;
    list.forEach(i => {
        html += `<tr>
            <td>#${i.id}</td>
            <td>
                <div style="font-weight:600;">${i.last_name} ${i.first_name}</div>
                <div style="font-size:12px; color:#666;">${i.middle_name || ''}</div>
            </td>
            <td><code style="background:#f3f4f6; padding:2px 5px; border-radius:4px;">${i.username}</code></td>
            <td><span style="text-transform:uppercase; font-size:11px; font-weight:700; color:#666;">${ROLE_MAP[i.role]}</span></td>
            <td>
                <button class="action-btn btn-edit-icon" onclick="openModal(${i.id})"><i class="fa-solid fa-pen"></i></button>
                <button class="action-btn btn-delete-icon" onclick="removeItem(${i.id})"><i class="fa-solid fa-trash"></i></button>
            </td>
        </tr>`;
    });
    container.innerHTML = html + '</tbody></table>';
}

function renderAudit(list, container) {
    let html = `<table><thead><tr><th>Время</th><th>Пользователь</th><th>Действие</th><th>Детали</th></tr></thead><tbody>`;
    list.forEach(i => {
        let det = i.details; try { det = JSON.parse(i.details).message; } catch {}
        let actIcon = 'fa-circle-info';
        if(i.action === 'DELETE') actIcon = 'fa-trash text-danger';
        if(i.action === 'CREATE') actIcon = 'fa-plus text-success';
        if(i.action === 'UPDATE') actIcon = 'fa-pen text-warning';

        html += `<tr>
            <td style="white-space:nowrap; font-size:13px; color:#666;">${i.time}</td>
            <td><b>${i.user}</b></td>
            <td><span style="font-size:12px; font-weight:600;">${i.action}</span></td>
            <td style="color:#444;">${det}</td>
        </tr>`;
    });
    container.innerHTML = html + '</tbody></table>';
}

function openModal(id = null) {
    const modal = document.getElementById('modal');
    const form = document.getElementById('modal-form');
    modal.dataset.id = id || '';
    modal.classList.add('open');
    document.getElementById('modal-title').innerText = id ? 'Редактирование записи' : 'Новая запись';
    
    let html = '';
    
    if (currentTab === 'flights') {
        const item = id ? currentData.find(x => x.id === id) : {};
        const opts = (arr) => arr.map(x => `<option value="${x.id}">${x.name}</option>`).join('');
        
        html = `<div class="form-grid">
            <div class="form-group form-full">
                <label>Номер рейса</label>
                <input id="f-num" value="${item.flight_number||''}" placeholder="SU-100">
            </div>
            
            <div class="form-group">
                <label>Откуда</label>
                <select id="f-dep">${opts(resources.airports)}</select>
            </div>
            <div class="form-group">
                <label>Куда</label>
                <select id="f-arr">${opts(resources.airports)}</select>
            </div>

            <div class="form-group">
                <label>Время вылета</label>
                <input type="datetime-local" id="f-dtime" value="${item.dep_time_iso||''}">
            </div>
            <div class="form-group">
                <label>Время прилета</label>
                <input type="datetime-local" id="f-atime" value="${item.arr_time_iso||''}">
            </div>

            <div class="form-group">
                <label>Статус</label>
                <select id="f-stat">
                    ${Object.keys(STATUS_MAP).map(k => `<option value="${k}">${STATUS_MAP[k]}</option>`).join('')}
                </select>
            </div>
            <div class="form-group">
                <label>Гейт</label>
                <input id="f-gate" value="${item.gate||''}" placeholder="A1">
            </div>

            <div class="form-group form-full">
                <label>Самолет</label>
                <select id="f-air"><option value="0">-- Не назначен --</option>${opts(resources.aircrafts)}</select>
            </div>

            <div class="form-group">
                <label>Командир (КВС)</label>
                <select id="f-pil"><option value="0">-- Не назначен --</option>${opts(resources.pilots)}</select>
            </div>
            <div class="form-group">
                <label>Второй пилот</label>
                <select id="f-copil"><option value="0">-- Не назначен --</option>${opts(resources.pilots)}</select>
            </div>
        </div>`;
        
        form.innerHTML = html;
        if(id) {
            const setVal = (eid, val) => { const el = document.getElementById(eid); if(el) el.value = val; };
            const findAirport = (name) => resources.airports.find(x=>x.name.includes(name))?.id;
            
            setVal('f-dep', findAirport(item.origin));
            setVal('f-arr', findAirport(item.destination));
            setVal('f-stat', item.status);
            setVal('f-air', item.aircraft_id);
            setVal('f-pil', item.pilot_id);
            setVal('f-copil', item.copilot_id);
        }

    } else if (currentTab === 'users') {
        const item = id ? currentData.find(x => x.id === id) : {};
        html = `<div class="form-grid">
            <div class="form-group form-full">
                <label>Логин для входа</label>
                <input id="u-login" value="${item.username||''}">
            </div>
            <div class="form-group form-full">
                <label>Пароль</label>
                <input type="password" id="u-pass" placeholder="${id ? 'Оставьте пустым, чтобы не менять' : 'Придумайте пароль'}">
            </div>
            
            <div class="form-group form-full"><label>Фамилия</label><input id="u-last" value="${item.last_name||''}"></div>
            <div class="form-group"><label>Имя</label><input id="u-first" value="${item.first_name||''}"></div>
            <div class="form-group"><label>Отчество</label><input id="u-mid" value="${item.middle_name||''}"></div>
            
            <div class="form-group form-full">
                <label>Роль доступа</label>
                <select id="u-role">
                    <option value="operator">Оператор (Редактирование рейсов)</option>
                    <option value="admin">Администратор (Полный доступ)</option>
                </select>
            </div>
        </div>`;
        form.innerHTML = html;
        if(id) document.getElementById('u-role').value = item.role;
    }
}

function closeModal() { document.getElementById('modal').classList.remove('open'); }

async function submitModal() {
    const id = document.getElementById('modal').dataset.id;
    const url = `/api/admin/${currentTab}` + (id ? `/${id}` : '');
    const method = id ? 'PUT' : 'POST';
    let body = {};

    if (currentTab === 'flights') {
        body = {
            flight_number: document.getElementById('f-num').value,
            origin: document.getElementById('f-dep').value,
            destination: document.getElementById('f-arr').value,
            dep_time: document.getElementById('f-dtime').value,
            arr_time: document.getElementById('f-atime').value,
            status: document.getElementById('f-stat').value,
            gate: document.getElementById('f-gate').value,
            aircraft_id: Number(document.getElementById('f-air').value), 
            pilot_id: Number(document.getElementById('f-pil').value), 
            copilot_id: Number(document.getElementById('f-copil').value)
        };
    } else {
        body = {
            username: document.getElementById('u-login').value,
            password: document.getElementById('u-pass').value,
            last_name: document.getElementById('u-last').value,
            first_name: document.getElementById('u-first').value,
            middle_name: document.getElementById('u-mid').value,
            role: document.getElementById('u-role').value
        };
    }

    try {
        await api(url, method, body);
        closeModal();
        loadData();
    } catch(e) { 
        alert('Ошибка: ' + e.message); 
    }
}

async function removeItem(id) {
    if(confirm('Вы уверены? Это действие нельзя отменить.')) {
        try {
            await api(`/api/admin/${currentTab}/${id}`, 'DELETE');
            loadData();
        } catch(e) { 
             alert('Ошибка удаления: ' + e.message); 
        }
    }
}

loadData();