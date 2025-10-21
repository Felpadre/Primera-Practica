const tempValue = document.getElementById("tempValue");
const humValue = document.getElementById("humValue");
const toggle = document.getElementById("darkmode-toggle");

// === Aplicar tema guardado ===
const savedTheme = localStorage.getItem("theme");
if (savedTheme === "dark") {
  document.body.classList.add("dark");
  toggle.checked = true;
}

// === Cambiar tema ===
toggle.addEventListener("change", () => {
  document.body.classList.toggle("dark");
  const mode = document.body.classList.contains("dark") ? "dark" : "light";
  localStorage.setItem("theme", mode);
});

// === Cargar datos guardados ===
const savedData = JSON.parse(localStorage.getItem("sensorData"));
if (savedData) {
  tempValue.textContent = `${savedData.temp} °C`;
  humValue.textContent = `${savedData.hum} %`;
}

// === Obtener datos del ESP32 ===
async function getSensorData() {
  try {
    // ⚠️ Cambia la IP por la de tu ESP32
    const response = await fetch("http://192.168.0.150/data");
    const data = await response.json();

    tempValue.textContent = `${data.temp.toFixed(1)} °C`;
    humValue.textContent = `${data.hum.toFixed(1)} %`;

    localStorage.setItem("sensorData", JSON.stringify(data));
  } catch (error) {
    console.warn("Error al obtener datos del ESP32, usando últimos guardados.");
  }
}

getSensorData();
setInterval(getSensorData, 5000);
