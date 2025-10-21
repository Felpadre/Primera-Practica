async function getData() {
  try {
    const res = await fetch('/data');
    const json = await res.json();
    if (json.temp !== undefined) {
      document.getElementById('temp').textContent = json.temp.toFixed(1) + ' °C';
      document.getElementById('hum').textContent = json.hum.toFixed(1) + ' %';
    }
  } catch (err) {
    console.error("Error al obtener datos:", err);
  }
}

setInterval(getData, 2000);
getData();

// Modo oscuro
const toggle = document.getElementById('theme-toggle');
toggle.addEventListener('change', () => {
  document.body.classList.toggle('dark', toggle.checked);
});
