const ESP_IP = window.location.origin;

function turnOn()
{
fetch("/on",{method:"POST"});
}

function turnOff()
{
fetch("/off",{method:"POST"});
}

function startTimer()
{
const h=parseInt(hours.value)||0;
const m=parseInt(minutes.value)||0;
const s=parseInt(seconds.value)||0;

fetch("/setDuration",
{
method:"POST",
headers:{"Content-Type":"application/json"},
body:JSON.stringify(
{
hours:h,
minutes:m,
seconds:s
})
});
}

setInterval(async()=>
{
let res=await fetch("/status");
let data=await res.json();

status.innerHTML=
"Status: "+(data.light?"ON":"OFF")+
"<br>Remaining: "+data.remaining+" sec";

},1000);
