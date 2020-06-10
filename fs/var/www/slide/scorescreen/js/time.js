function startTime() {
    var today = new Date();
    var h = today.getHours();
    var m = today.getMinutes();
    m = checkTime(m);
    document.getElementById('time').innerHTML = h + ":" + m;
    var t = setTimeout(startTime,15000);
}

function checkTime(i) {
    if (i < 10) {i = "0" + i}; //add zero infront of numbers < 10
    return i;
}