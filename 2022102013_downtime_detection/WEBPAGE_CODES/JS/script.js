const channelId = '2515894';
const apiKey = 'QGW11KUKJMCIALDW';
const threshold = 0.5; 
let lastDowntimeTimestamp = "2024-05-08T12:00:00";

const dataElement = document.getElementById('data');
const statusElement = document.getElementById('status');

function displayLastDowntime() {
    if (lastDowntimeTimestamp === "Not Applicable") {
        statusElement.textContent = `Last downtime - ${lastDowntimeTimestamp}`;
    } else {
        const lastDowntime = new Date(lastDowntimeTimestamp).toLocaleString("en-US", {timeZone: "Asia/Kolkata"});
        statusElement.textContent = `Last downtime - ${lastDowntime}`;
    }
}


displayLastDowntime();

function fetchData() {
    fetch(`https://api.thingspeak.com/channels/${channelId}/fields/2/last.json?api_key=${apiKey}`)
        .then(response => response.json())
        .then(data => {
            const value = parseFloat(data.field2);
            dataElement.textContent = `Input RMS Current: ${value}A`;
            
            if (value < threshold) {
                const currentTime = new Date().toISOString(); 
                lastDowntimeTimestamp = currentTime;
                const currentTimeDisplay = new Date(currentTime).toLocaleString("en-US", {timeZone: "Asia/Kolkata"});
                statusElement.textContent = `Downtime Detected at ${currentTimeDisplay}`;
                statusElement.style.color = 'red';
            } else {
                statusElement.textContent = 'No Downtime';
                statusElement.style.color = 'green';
            }
        })
        .catch(error => console.error('Error fetching data:', error))
        .finally(() => displayLastDowntime()); 
}

function fetchFieldData() {
    const url = `https://api.thingspeak.com/channels/${channelId}/feeds.json?results=1`;
    fetch(url)
        .then(response => response.json())
        .then(data => {
            if (data && data.feeds && data.feeds.length > 0) {
                const fieldData = data.feeds[0];
                const field1 = fieldData.field1;
                const field2 = fieldData.field2;

                document.getElementById('fieldData').innerHTML = `
                    <p>Output RMS Current: ${field1}A</p>
                    <p>Input RMS Current: ${field2}A</p>
                `;
            }
        })
        .catch(error => console.error('Error fetching field data:', error));
}

fetchData();
fetchFieldData();

setInterval(fetchData, 10000);
setInterval(fetchFieldData, 10000);
