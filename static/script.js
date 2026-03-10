
let startTime=null;
let endTime=null;

let countdownInterval=null;


// AUDIO

function playSound(type)
{
let audio=document.getElementById(
type=="on"?"audio-on":"audio-off"
);

audio.currentTime=0;

audio.play();
}


// ON

async function turnON()
{
await fetch("/on",{method:"POST"});

playSound("on");
}


// OFF

async function turnOFF()
{
await fetch("/off",{method:"POST"});

playSound("off");
}


// SCHEDULE

async function setSchedule()
{
let on=document.getElementById("onTime").value;

let off=document.getElementById("offTime").value;

await fetch("/setTimer",
{
method:"POST",
headers:
{"Content-Type":"application/json"},
body:JSON.stringify({on:on,off:off})
});

startCountdown(off);
}


// DURATION

async function setDuration()
{

let h=parseInt(durHours.value)||0;
let m=parseInt(durMins.value)||0;
let s=parseInt(durSecs.value)||0;

let now=new Date();

let off=new Date(
now.getTime()+(h*3600+m*60+s)*1000
);

let onStr=formatDate(now);
let offStr=formatDate(off);

await fetch("/setTimer",
{
method:"POST",
headers:
{"Content-Type":"application/json"},
body:JSON.stringify({on:onStr,off:offStr})
});

startCountdown(offStr);
}


// FORMAT DATE

function formatDate(d)
{
return d.getFullYear()+"-"+
pad(d.getMonth()+1)+"-"+
pad(d.getDate())+"T"+
pad(d.getHours())+":"+
pad(d.getMinutes())+":"+
pad(d.getSeconds());
}

function pad(n)
{
return n.toString().padStart(2,"0");
}


// COUNTDOWN

function startCountdown(offTime)
{

startTime=new Date();

endTime=new Date(offTime);

clearInterval(countdownInterval);

countdownInterval=setInterval(()=>{

let now=new Date();

let diff=endTime-now;

if(diff<=0)
{
countdown.innerText="Completed";

progressBar.style.width="100%";

clearInterval(countdownInterval);

return;
}

let h=Math.floor(diff/3600000);
let m=Math.floor(diff%3600000/60000);
let s=Math.floor(diff%60000/1000);

countdown.innerText=
`${h}h ${m}m ${s}s`;

let percent=
(now-startTime)/(endTime-startTime)*100;

progressBar.style.width=percent+"%";

},1000);

}


// LIVE ESP32 SYNC

async function syncESP32()
{

try
{

let res=await fetch("/status");

let data=await res.json();

if(data.status=="on")
{
statusText.innerText="Device ON";
statusIndicator.className="status-dot bg-green-500";
}

else
{
statusText.innerText="Device OFF";
statusIndicator.className="status-dot bg-red-500";
}

}
catch{}

}


setInterval(syncESP32,1000);
