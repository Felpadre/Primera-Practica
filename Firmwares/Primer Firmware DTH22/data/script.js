window.addEventListener("DOMContentLoaded", () => {
  const tempValue = document.getElementById("tempValue");
  const humValue = document.getElementById("humValue");
  const themeToggle = document.getElementById("themeToggle");

  // ====== TEMA OSCURO/CLARO ======
  const savedTheme = localStorage.getItem("theme") || "light";
  document.body.classList.toggle("dark", savedTheme === "dark");
  themeToggle.checked = savedTheme === "dark";

  themeToggle.addEventListener("change", () => {
    const mode = themeToggle.checked ? "dark" : "light";
    document.body.classList.toggle("dark", mode === "dark");
    localStorage.setItem("theme", mode);
  });

  // ====== SIMULACIÓN DE DATOS ======
  let temperature = parseFloat(localStorage.getItem("temperature")) || 25.0;
  let humidity = parseFloat(localStorage.getItem("humidity")) || 50.0;

  function updateSensorData() {
    temperature += (Math.random() - 0.5) * 0.5;
    humidity += (Math.random() - 0.5) * 0.7;

    temperature = Math.min(Math.max(temperature, 20), 35);
    humidity = Math.min(Math.max(humidity, 30), 90);

    tempValue.textContent = temperature.toFixed(1) + " °C";
    humValue.textContent = humidity.toFixed(1) + " %";

    localStorage.setItem("temperature", temperature.toFixed(1));
    localStorage.setItem("humidity", humidity.toFixed(1));
  }

  // Actualización automática
  updateSensorData();
  setInterval(updateSensorData, 1000);
});
