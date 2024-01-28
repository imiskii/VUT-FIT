
// Bluetooth options filter
const deviceOptions = {
    filters: [{ 
        name: 'Bluetooth Metronome'
     }],
     optionalServices: ['31e8370e-26c8-4a46-bd51-f77873cc72eb']
};

// Message encoder
const encoder = new TextEncoder('utf-8');

// Global variables for handeling bluetooth actions
let gattOperationInProgress = false;
var bluetoothDevice;
var bluetoothChr;


function bluetooth_connect()
{
    var stateText = document.getElementById("log-state");
    const tempoLabel = document.getElementById("bpm"); // BPM counter
    const volume = document.getElementById("volume-range"); // Volume fader
    const rythmSelect = document.getElementById('rythm-select'); // Rythm select

    if ('bluetooth' in navigator)
    {
        // Connect to a bluetooth
        navigator.bluetooth.requestDevice(deviceOptions)
        .then(device => {
            // Set bluetooth device
            bluetoothDevice = device;
            // Set up event listener for when device gets disconnected
            device.addEventListener('gattserverdisconnected', bluetooth_disconnect);
            // Connect to GATT server
            console.log('Bluetooth connecting...');
            return device.gatt.connect();
        })
        .then(server => {
            // Get the service
            console.log('Getting primary service...');
            return server.getPrimaryService('31e8370e-26c8-4a46-bd51-f77873cc72eb');
        })
        .then(service => {
            // Change web element -> Connection successful
            var connectBtn = document.getElementById("conection-btn");
            
            connectBtn.textContent = "Disconnect";
            connectBtn.onclick = onDisconnectButtonClick;
            
            stateText.textContent = "Device connected!"
            stateText.style.color = "green";
            // Get the characteristic
            console.log('Getting characteristics...');
            return service.getCharacteristic('84cd9b4d-00e0-454f-bd2c-12f54ea14f8b');
        })
        .then(characteristic => {
            // Connection successful message
            console.log('Connection successful!');
            // Set bluetooth characteristic
            bluetoothChr = characteristic;
            // Write initial settings of the metronome. Order is always TEMPO + <space> + RYTHM + <space> + VOLUME
            const initData = tempoLabel.textContent + ' ' + volume.value + ' ' + rythmSelect.value;
            // Uint8Array([0x35]) => message code '5': init metronome
            const data = new Uint8Array([0x35].concat(Array.from(encoder.encode(initData))));
            return characteristic.writeValue(data);
        })
        .catch(error => {
            console.error('Error:', error);
        });
    }
    else
    {
        // Bluetooth is not enabled
        stateText.textContent = "Web Bluetooth is not supported in this browser!"
        stateText.style.color = "red";
    }
}


/**
 * Handler for Disconnect button click
 * 
 * @returns void
 */
function onDisconnectButtonClick()
{
    if (!bluetoothDevice)
    {
        return;
    }
    if (bluetoothDevice.gatt.connected)
    {
        bluetoothDevice.gatt.disconnect();
    }
    bluetoothChr = null;
    bluetoothDevice = null
    bluetooth_disconnect(null);
}


/**
 * Handler for bluetooth disconnect event
 * 
 * @param {*} event 
 */
function bluetooth_disconnect(event)
{
    // Change web elements
    var connectBtn = document.getElementById("conection-btn");
    var stateText = document.getElementById("log-state");

    connectBtn.textContent = "Connect";
    connectBtn.onclick = bluetooth_connect;

    stateText.textContent = "Device is not connected!"
    stateText.style.color = "red";

    // Play/Stop button
    var btnPlayStop = document.getElementById("play-stop");
    // change onclick function
    btnPlayStop.onclick = run_metronome;
    // change button icon
    var iconElements = btnPlayStop.querySelectorAll('i');
    iconElements[1].classList.add('hide');
    iconElements[0].classList.remove('hide');
}


async function tempo_plus()
{
    const tempoLabel = document.getElementById("bpm"); // BPM counter
    const tempoRange = document.getElementById("tempo-range"); // Range fader
    // tempo++
    var value = parseInt(tempoLabel.textContent) + 1;
    if (value <= 240)
    {
        // if bluetooth is connected
        if (bluetoothChr)
        {
            if (bluetoothDevice.gatt.connected)
            {
                try {
                    // Uint8Array([0x30]) => message code '0': set tempo
                    const data = new Uint8Array([0x30].concat(Array.from(encoder.encode(value.toString()))));
                    await bluetoothChr.writeValue(data);
                    // Change web element
                    tempoLabel.textContent = value;
                    tempoRange.value = value;
                } catch (error){
                    console.error('Error writing value:', error);
                }
            }
        }
        else
        {
            // Change web element
            tempoLabel.textContent = value;
            tempoRange.value = value;
        }
    }
}


async function tempo_minus()
{
    const tempoLabel = document.getElementById("bpm"); // BPM counter
    const tempoRange = document.getElementById("tempo-range"); // Range fader
    // tempo++
    var value = parseInt(tempoLabel.textContent) - 1;
    if (value >= 20)
    {
        // if bluetooth is connected
        if (bluetoothChr)
        {
            if (bluetoothDevice.gatt.connected)
            {
                try {
                    // Uint8Array([0x30]) => message code '0': set tempo
                    const data = new Uint8Array([0x30].concat(Array.from(encoder.encode(value.toString()))));
                    await bluetoothChr.writeValue(data);
                    // Change web element
                    tempoLabel.textContent = value;
                    tempoRange.value = value;
                } catch (error){
                    console.error('Error writing value:', error);
                }
            }
        }
        else
        {
            // Change web element
            tempoLabel.textContent = value;
            tempoRange.value = value;
        }
    }
}


function set_tempo()
{
    const tempoLabel = document.getElementById("bpm"); // BPM counter
    const tempoRange = document.getElementById("tempo-range"); // Range fader
    // if bluetooth is connected
    if (bluetoothChr)
    {
        if (bluetoothDevice.gatt.connected)
        {
            if (!gattOperationInProgress)
            {
                gattOperationInProgress = true;
                let debounceTimer;
                clearTimeout(debounceTimer);
                
                debounceTimer = setTimeout(() => {
                    try {
                        // Uint8Array([0x30]) => message code '0': set tempo
                        const data = new Uint8Array([0x30].concat(Array.from(encoder.encode(tempoRange.value))));
                        bluetoothChr.writeValue(data).then(() => {
                            gattOperationInProgress = false;
                        }).catch(error => {
                            console.error('Error writing value:', error);
                            gattOperationInProgress = false;
                        });
                        // Change web element
                        tempoLabel.textContent = tempoRange.value;
                    } catch (error){
                        console.error('Error writing value:', error);
                        gattOperationInProgress = false;
                    }
                }, 300);
            }
        }
    }
    else
    {
        // Change web element
        tempoLabel.textContent = parseInt(tempoRange.value);
    }
}


async function run_metronome()
{
    // If bluetooth is not connectet then return
    if (!bluetoothChr)
    {
        return;
    }

    if(bluetoothDevice.gatt.connected)
    {
        try{
            // Uint8Array([0x33]) => message code '3': start metronome
            const data = new Uint8Array([0x33]);
            await bluetoothChr.writeValue(data);
            // Change web element
            var btnPlayStop = document.getElementById("play-stop");
            // change onclick function
            btnPlayStop.onclick = stop_metronome;
            // change button icon
            var iconElements = btnPlayStop.querySelectorAll('i');
            iconElements[0].classList.add('hide');
            iconElements[1].classList.remove('hide');
        } catch(error) {
            console.error('Error writing value:', error);
        }
    }
}


async function stop_metronome()
{
    // If bluetooth is not connectet then return
    if (!bluetoothChr)
    {
        return;
    }

    if(bluetoothDevice.gatt.connected)
    {
        try{
            // Uint8Array([0x34]) => message code '4': stop metronome
            const data = new Uint8Array([0x34]);
            await bluetoothChr.writeValue(data);
            // Change web element
            var btnPlayStop = document.getElementById("play-stop");
            // change onclick function
            btnPlayStop.onclick = run_metronome;
            // change button icon
            var iconElements = btnPlayStop.querySelectorAll('i');
            iconElements[1].classList.add('hide');
            iconElements[0].classList.remove('hide');
        } catch(error) {
            console.error('Error writing value:', error);
        }
    }
}


/**
 * Sets metronome rythm
 * 
 */
async function set_rythm()
{
    const rythmSelect = document.getElementById('rythm-select');

    // if bluetooth is connected
    if (bluetoothChr)
    {
        if (bluetoothDevice.gatt.connected)
        {
            try {
                // Uint8Array([0x31]) => message code '1': set rythm
                const data = new Uint8Array([0x31].concat(Array.from(encoder.encode(rythmSelect.value))));
                await bluetoothChr.writeValue(data);
            } catch (error){
                console.error('Error writing value:', error);
            }
        }
    }
}


function set_volume()
{
    const volume = document.getElementById("volume-range"); // Volume fader
    // if bluetooth is connected
    if (bluetoothChr)
    {
        if (bluetoothDevice.gatt.connected)
        {
            if (!gattOperationInProgress)
            {
                gattOperationInProgress = true;
                let debounceTimer;
                clearTimeout(debounceTimer);
    
                debounceTimer = setTimeout(() => {
                    // Sned volume message
                    try {
                        // Uint8Array([0x32]) => message code '2': set volume
                        const data = new Uint8Array([0x32].concat(Array.from(encoder.encode(volume.value))));
                        bluetoothChr.writeValue(data).then(() => {
                            gattOperationInProgress = false;
                        }).catch(error => {
                            console.error('Error writing value:', error);
                            gattOperationInProgress = false; 
                        });
                    } catch (error){
                        console.error('Error writing value:', error);
                        gattOperationInProgress = false;
                    }
                }, 300);
            }


        }
    }
}